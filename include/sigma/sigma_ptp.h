#pragma once
#include "ptp/ptp.h"
#include <cstdint>
#include <vector>

// data groups
#include "schema.h"

enum class SigmaOp : std::uint16_t
{
  GetCamDataGroup1 = 0x9012,
  GetCamDataGroup2 = 0x9013,
  GetCamDataGroup3 = 0x9014,
  GetCamCaptStatus = 0x9015,
  SetCamDataGroup1 = 0x9016,
  SetCamDataGroup2 = 0x9017,
  SetCamDataGroup3 = 0x9018,
  SetCamClockAdj = 0x9019,
  SnapCommand = 0x901B,
  ClearImageDBSingle = 0x901C,
  GetBigPartialPictFile = 0x9022,
  GetCamDataGroup4 = 0x9023,
  SetCamDataGroup4 = 0x9024,
  GetCamDataGroup5 = 0x9027,
  SetCamDataGroup5 = 0x9028,
  GetLastCommandData = 0x9029,
  FreeArrayMemory = 0x902A,
  GetViewFrame = 0x902B,
  GetPictFileInfo2 = 0x902D,
  CloseApplication = 0x902F,
  GetCamCanSetInfo5 = 0x9030,
  GetCamDataGroupFocus = 0x9031,
  SetCamDataGroupFocus = 0x9032,
  GetCamDataGroupMovie = 0x9033,
  SetCamDataGroupMovie = 0x9034,
  ConfigApi = 0x9035,
  GetMovieFileInfo = 0x9036,
  GetPartialMovieFile = 0x9037,
};

// type→opcode mapping
template <class T>
struct SigmaGroupMap;
template <>
struct SigmaGroupMap<CamDataGroup1>
{
  static constexpr SigmaOp Get = SigmaOp::GetCamDataGroup1;
  static constexpr SigmaOp Set = SigmaOp::SetCamDataGroup1;
};
template <>
struct SigmaGroupMap<CamDataGroup2>
{
  static constexpr SigmaOp Get = SigmaOp::GetCamDataGroup2;
  static constexpr SigmaOp Set = SigmaOp::SetCamDataGroup2;
};
template <>
struct SigmaGroupMap<CamDataGroup3>
{
  static constexpr SigmaOp Get = SigmaOp::GetCamDataGroup3;
  static constexpr SigmaOp Set = SigmaOp::SetCamDataGroup3;
};
template <>
struct SigmaGroupMap<CamDataGroup4>
{
  static constexpr SigmaOp Get = SigmaOp::GetCamDataGroup4;
  static constexpr SigmaOp Set = SigmaOp::SetCamDataGroup4;
};
template <>
struct SigmaGroupMap<CamDataGroup5>
{
  static constexpr SigmaOp Get = SigmaOp::GetCamDataGroup5;
  static constexpr SigmaOp Set = SigmaOp::SetCamDataGroup5;
};

class SigmaCamera : public CameraPTP
{
public:
  explicit SigmaCamera(Transport &t) : CameraPTP(t) {}

  ApiConfig config_api();
  void close_application();

  // typed get/set
  template <class GroupT>
  GroupT get_group();
  template <class GroupT>
  CameraPTP::Response set_group(const GroupT &g);

  // raw bytes
  // std::vector<std::uint8_t> get_bytes(SigmaOp op);
  // void set_bytes(SigmaOp op, const std::vector<std::uint8_t> &payload);

  CamCaptStatus get_cam_capt_status();
  CamCaptStatus get_cam_capt_status(std::uint8_t image_id);
  CamCaptStatus wait_completion(std::uint8_t image_id, int polls = 30,
                                int sleep_ms = 1000);

  CameraPTP::Response set_cam_data_group_focus(const CamDataGroupFocus& focus);
  CamDataGroupFocus get_cam_data_group_focus();

  uint16_t snap(const SnapCommand &cmd);
  uint16_t snap(CaptureMode mode, std::uint8_t amount); // convenience

  void clear_image_db_single(std::uint32_t image_id);

  PictFileInfo2 get_pict_file_info2();
  PictFileInfo2 get_pict_file_info2(std::uint32_t object_handle);
  BigPartialPictFile get_big_partial_pict_file(std::uint32_t address,
                                               std::uint32_t start,
                                               std::uint32_t max_bytes);
  ViewFrame get_view_frame();

  // vendor-chunked download using the two calls above
  std::vector<std::uint8_t> get_object_vendor(std::uint32_t object_handle,
                                              std::uint32_t chunk = 1024 * 1024);
  std::vector<uint8_t> get_latest_image(DestToSave mode, int timeout = 5000);
};

// explicit instantiations (built in .cpp)
extern template CamDataGroup1 SigmaCamera::get_group<CamDataGroup1>();
extern template CamDataGroup2 SigmaCamera::get_group<CamDataGroup2>();
extern template CamDataGroup3 SigmaCamera::get_group<CamDataGroup3>();
extern template CamDataGroup4 SigmaCamera::get_group<CamDataGroup4>();
extern template CamDataGroup5 SigmaCamera::get_group<CamDataGroup5>();

extern template CameraPTP::Response SigmaCamera::set_group<CamDataGroup1>(const CamDataGroup1 &);
extern template CameraPTP::Response SigmaCamera::set_group<CamDataGroup2>(const CamDataGroup2 &);
extern template CameraPTP::Response SigmaCamera::set_group<CamDataGroup3>(const CamDataGroup3 &);
extern template CameraPTP::Response SigmaCamera::set_group<CamDataGroup4>(const CamDataGroup4 &);
extern template CameraPTP::Response SigmaCamera::set_group<CamDataGroup5>(const CamDataGroup5 &);