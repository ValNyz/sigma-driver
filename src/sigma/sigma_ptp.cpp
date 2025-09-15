#include <chrono>
#include <iostream>
#include <stdexcept>
#include <thread>

#include "utils/log.h"

#include "sigma/sigma_ptp.h"

static char device[64];

ApiConfig SigmaCamera::config_api()
/*This is the first instruction issued to the camera by the application that
uses API.

After this instruction has been received, another custom command can be received
until the USB connection is shut down or the CloseApplication instruction is
received. When this function is executed, API resets the camera setting to the
default. (When API connection is closed, the camera setting returns to the
setting value which the user specified before using API. However, the
movie/still image setting is synchronized with the switch status.) Furthermore,
API does not accept any operation other than the power-off operation. The data
to be handled is based on the IFD structure.

Returns:
    ApiConfig: the set of values obtained from a camera.*/
{
  // Vendor op: SigmaConfigApi, with a single parameter 0
  auto r = transact(static_cast<std::uint16_t>(SigmaOp::ConfigApi), {0u},
                    nullptr, true);

  if (r.data.empty())
    throw std::runtime_error("config_api: empty data");

  ApiConfig cfg;
  cfg.decode(r.data);
  LOG_INFO("ConfigApi: %s", cfg.to_string().c_str());
  return cfg;
}

void SigmaCamera::close_application()
/*This instruction informs the camera that the session is closed when the
   application exits.*/
{
  std::vector<std::uint8_t> payload(10, 0x00); // undocumented 10 zeros
  auto r = transact(static_cast<std::uint16_t>(SigmaOp::CloseApplication), {},
                    &payload, false);
  LOG_INFO("CloseApplication sent, resp=0x%04X", r.response_code);
}

// --- CamCaptStatus ---
CamCaptStatus SigmaCamera::get_cam_capt_status() {
  auto r = transact(static_cast<std::uint16_t>(SigmaOp::GetCamCaptStatus), {},
                    nullptr, true);
  CamCaptStatus s;
  s.decode(r.data);
  return s;
}

CamCaptStatus SigmaCamera::get_cam_capt_status(std::uint8_t image_id)
/*This instruction acquires the shooting result from the camera.

Args:
    image_id (int): the image ID to obtain a status.

Returns:
    CamCaptStatus: CamCaptStatus object.*/
{
  auto r = transact(static_cast<std::uint16_t>(SigmaOp::GetCamCaptStatus),
                    {image_id}, nullptr, true);
  CamCaptStatus s;
  s.decode(r.data);
  return s;
}

CamCaptStatus SigmaCamera::wait_completion(std::uint8_t image_id, int polls,
                                           int sleep_ms) {

  std::this_thread::sleep_for(std::chrono::milliseconds(sleep_ms));
  CamCaptStatus st;
  for (int i = 0; i < polls; ++i) {
    st = get_cam_capt_status(image_id);
    const std::uint16_t code = static_cast<std::uint16_t>(st.Status);
    LOG_DEBUG("CaptStatus img=%u head=%u tail=%u code=0x%04X", st.ImageId,
              st.ImageDBHead, st.ImageDBTail, code);

    switch (static_cast<CaptStatus>(code)) {
    case CaptStatus::ImageGenCompleted:
    case CaptStatus::ImageDataStorageCompleted:
    case CaptStatus::Cleared:
      return st;

    case CaptStatus::ShootInProgress:
    case CaptStatus::ShootSuccess:
    case CaptStatus::ImageGenInProgress:
    case CaptStatus::AFSuccess:
    case CaptStatus::CWBSuccess:
      LOG_INFO("Current status 0x%04X, waiting...", code);

    default:
      // transient or unexpected; tolerate a few, then return
      LOG_WARN("Unexpected capture status 0x%04X", code);
      return st;
    }

    std::this_thread::sleep_for(std::chrono::milliseconds(sleep_ms));
  }
  return st;
}

// --- SnapCommand ---
void SigmaCamera::snap(const SnapCommand &cmd)
/*This command issues shooting instructions from the PC to the camera.

Args:
    data (SnapCommand): a snap command object.*/
{
  auto payload = cmd.encode();
  (void)transact(static_cast<std::uint16_t>(SigmaOp::SnapCommand), {}, &payload,
                 false);
}

void SigmaCamera::snap(CaptureMode mode, std::uint8_t amount)
/*This command issues shooting instructions from the PC to the camera.

Args:
    mode (CaptureMode): a CaptureMode object.
    amount: the number of image to capture.*/
{
  SnapCommand cmd;
  cmd.Mode = mode;
  cmd.Amount = amount;
  snap(cmd);
}

void SigmaCamera::clear_image_db_single(std::uint32_t image_id)
/*This instruction requests to clear the shooting result of the CaptStatus
   database in the camera.*/
{
  std::vector<std::uint8_t> payload(10, 0x00); // undocumented 10 zero bytes
  (void)transact(static_cast<std::uint16_t>(SigmaOp::ClearImageDBSingle),
                 {image_id}, &payload, false);
  LOG_INFO("ClearImageDBSingle image_id=%u", image_id);
}

// --- PictFileInfo2 ---
PictFileInfo2 SigmaCamera::get_pict_file_info2()
/*This function requests information of the data (image file) that is shot in
Camera Control mode.

Returns:
    PictFileInfo2: PictFileInfo2 object.*/
{
  auto r = transact(static_cast<std::uint16_t>(SigmaOp::GetPictFileInfo2), {},
                    nullptr, true);
  PictFileInfo2 info;
  info.decode(r.data);
  return info;
}

PictFileInfo2 SigmaCamera::get_pict_file_info2(std::uint32_t object_handle) {
  auto r = transact(static_cast<std::uint16_t>(SigmaOp::GetPictFileInfo2),
                    {object_handle}, nullptr, true);
  PictFileInfo2 info;
  info.decode(r.data);
  return info;
}

// --- BigPartialPictFile ---
BigPartialPictFile SigmaCamera::get_big_partial_pict_file(
    std::uint32_t address, std::uint32_t start, std::uint32_t max_bytes)
/*This function downloads image data (image file) shot by the camera in pieces.

Args:
    address (int): Image file storage location address (head address)
    max_bytes (int): Image file transfer size (units: bytes / Maximum value:
0x8000000)

Returns:
    BigPartialPictFile: BigPartialPictFile object.*/
{
  auto r = transact(static_cast<std::uint16_t>(SigmaOp::GetBigPartialPictFile),
                    {address, start, max_bytes}, nullptr, true);
  BigPartialPictFile part;
  part.decode(r.data);
  return part;
}

// --- ViewFrame (live view JPEG) ---
ViewFrame SigmaCamera::get_view_frame()
/*This function acquires image data when displaying LiveView.

When LiveView or QuickView can be prepared, the camera transfers image data to
the PC; otherwise, it transfers data, which means that the target image is not
found, to the PC. This function does not ensure checksum data to display
LiveView images as much as possible.

Returns:
    ViewFrame: a JPEG image.

Examples:
    You can obtain a RGB array from a returned JPEG data as follows::

        pict = camera.get_view_frame()
        img = cv2.imdecode(np.frombuffer(pict.Data, np.uint8),
cv2.IMREAD_COLOR)*/
{
  auto r = transact(static_cast<std::uint16_t>(SigmaOp::GetViewFrame), {},
                    nullptr, true);
  ViewFrame f;
  f.decode(r.data);
  return f;
}

// TODO remove ?
//  --- Vendor-chunked object download ---
std::vector<std::uint8_t>
SigmaCamera::get_object_vendor(std::uint32_t object_handle,
                               std::uint32_t chunk) {
  const auto info = get_pict_file_info2(object_handle);
  std::vector<std::uint8_t> out;
  out.reserve(info.FileSize);

  std::uint32_t addr = info.FileAddress;
  std::uint32_t left = info.FileSize;
  std::uint32_t start = 0;
  while (left) {
    const std::uint32_t req = std::min(chunk, left);
    BigPartialPictFile part = get_big_partial_pict_file(addr, start, req);
    if (part.AcquiredSize == 0 || part.PartialData.empty())
      break;

    out.insert(out.end(), part.PartialData.begin(), part.PartialData.end());
    start += part.AcquiredSize;
    left -= part.AcquiredSize;
    if (part.PartialData.size() < req)
      break; // safety
  }
  return out;
}

// TODO remove ?
std::vector<uint8_t> SigmaCamera::get_latest_image(DestToSave mode,
                                                   int timeout) {
  if (mode == DestToSave::InComputer) {
    if (auto h = SigmaCamera::wait_object_added(timeout, 200))
      return SigmaCamera::get_object(*h);
    return {};
  } else {
    return SigmaCamera::get_object_vendor(128 * 1024);
  }
}

// TODO get_cam_data_group_focus and set_cam_data_group_focus
// TODO get_cam_can_set_info5

// templates (definitions) + explicit instantiations
template <class GroupT>
GroupT SigmaCamera::get_group()
/*This instruction acquires DataGroup status information from the camera.

Returns:
    CamDataGroup: CamDataGroup object.*/
{
  auto r = transact(static_cast<std::uint16_t>(SigmaGroupMap<GroupT>::Get), {},
                    nullptr, true);
  GroupT g;
  g.decode(r.data);
  return g;
}

template <class GroupT>
void SigmaCamera::set_group(const GroupT &g)
/*This instruction acquires DataGroup status information from the camera.

Returns:
    CamDataGroup: CamDataGroup object.*/
{
  std::vector<uint8_t> bytes = g.encode();
  LOG_DEBUG("Bytes encoding for class %s:", typeid(GroupT).name());
  log_hex_preview(LogLevel::Debug, &bytes, bytes.size());
  // for (uint8_t i : bytes)
  // LOG_DEBUG("Byte: %04X", i);
  (void)transact(static_cast<std::uint16_t>(SigmaGroupMap<GroupT>::Set), {},
                 &bytes, false);
}

// explicit instantiation for 1..5
template CamDataGroup1 SigmaCamera::get_group<CamDataGroup1>();
template CamDataGroup2 SigmaCamera::get_group<CamDataGroup2>();
template CamDataGroup3 SigmaCamera::get_group<CamDataGroup3>();
template CamDataGroup4 SigmaCamera::get_group<CamDataGroup4>();
template CamDataGroup5 SigmaCamera::get_group<CamDataGroup5>();

template void SigmaCamera::set_group<CamDataGroup1>(const CamDataGroup1 &);
template void SigmaCamera::set_group<CamDataGroup2>(const CamDataGroup2 &);
template void SigmaCamera::set_group<CamDataGroup3>(const CamDataGroup3 &);
template void SigmaCamera::set_group<CamDataGroup4>(const CamDataGroup4 &);
template void SigmaCamera::set_group<CamDataGroup5>(const CamDataGroup5 &);
