#include <stdexcept>

#include "ptp/ptp.h"

std::vector<std::uint8_t> CameraPTP::read_full_container_()
{
  std::vector<std::uint8_t> buf(1 << 20);
  int n = transport_.read_some(buf.data(), (int)buf.size(), 3000);
  if (n < (int)sizeof(PtpContainerHeader))
    throw std::runtime_error("short PTP header");
  buf.resize(n);
  const auto *h = reinterpret_cast<const PtpContainerHeader *>(buf.data());
  const std::uint32_t need = h->total_length_bytes;
  if (need <= buf.size())
    return buf;
  std::vector<std::uint8_t> out = buf;
  while (out.size() < need)
  {
    std::vector<std::uint8_t> more(1 << 20);
    int m = transport_.read_some(more.data(), (int)more.size(), 3000);
    out.insert(out.end(), more.begin(), more.begin() + m);
  }
  out.resize(need);
  return out;
}

void CameraPTP::open_session(std::uint32_t sid)
{
  std::vector<std::uint8_t> cmd(sizeof(PtpContainerHeader));
  auto *h = reinterpret_cast<PtpContainerHeader *>(cmd.data());
  h->total_length_bytes = sizeof(PtpContainerHeader) + 4;
  h->container_type = PTP_CONTAINER_COMMAND;
  h->operation_or_response = PTP_OP_OpenSession;
  h->transaction_id = next_tid_++;
  put_32le(cmd, sid);
  transport_.write_exact(cmd.data(), (int)cmd.size());
  (void)read_full_container_();
}

void CameraPTP::close_session()
{
  std::vector<std::uint8_t> cmd(sizeof(PtpContainerHeader));
  auto *h = reinterpret_cast<PtpContainerHeader *>(cmd.data());
  h->total_length_bytes = sizeof(PtpContainerHeader);
  h->container_type = PTP_CONTAINER_COMMAND;
  h->operation_or_response = PTP_OP_CloseSession;
  h->transaction_id = next_tid_++;
  transport_.write_exact(cmd.data(), (int)cmd.size());
  (void)read_full_container_();
}

CameraPTP::Response CameraPTP::transact(
    std::uint16_t opcode, const std::vector<std::uint32_t> &params,
    const std::vector<std::uint8_t> *data_out, bool expect_data_in)
{
  std::vector<std::uint8_t> cmd(sizeof(PtpContainerHeader));
  auto *ch = reinterpret_cast<PtpContainerHeader *>(cmd.data());
  ch->total_length_bytes =
      std::uint32_t(sizeof(PtpContainerHeader) + params.size() * 4);
  ch->container_type = PTP_CONTAINER_COMMAND;
  ch->operation_or_response = opcode;
  ch->transaction_id = next_tid_++;
  for (auto p : params)
    put_32le(cmd, p);
  transport_.write_exact(cmd.data(), (int)cmd.size());

  if (data_out)
  {
    std::vector<std::uint8_t> dc(sizeof(PtpContainerHeader));
    auto *dh = reinterpret_cast<PtpContainerHeader *>(dc.data());
    dh->total_length_bytes =
        std::uint32_t(sizeof(PtpContainerHeader) + data_out->size());
    dh->container_type = PTP_CONTAINER_DATA;
    dh->operation_or_response = opcode;
    dh->transaction_id = ch->transaction_id;
    dc.insert(dc.end(), data_out->begin(), data_out->end());
    transport_.write_exact(dc.data(), (int)dc.size());
  }

  Response r{};

  // Read first inbound container. May be EVENT, DATA, or RESPONSE.
  auto pkt = read_full_container_();
  const auto *h = reinterpret_cast<const PtpContainerHeader *>(pkt.data());

  // Drop stray events
  int guard = 0;
  while (h->container_type == PTP_CONTAINER_EVENT && guard++ < 8)
  {
    pkt = read_full_container_();
    h = reinterpret_cast<const PtpContainerHeader *>(pkt.data());
  }

  // If DATA first, capture it, then read RESPONSE
  if (h->container_type == PTP_CONTAINER_DATA)
  {
    r.data.assign(pkt.begin() + sizeof(PtpContainerHeader),
                  pkt.begin() + h->total_length_bytes);

    // RESPONSE must follow
    auto rpkt = read_full_container_();
    const auto *rh = reinterpret_cast<const PtpContainerHeader *>(rpkt.data());
    if (rh->container_type != PTP_CONTAINER_RESPONSE)
      throw std::runtime_error("expected response container after data");

    r.response_code = rh->operation_or_response;

    const std::uint8_t *p = rpkt.data() + sizeof(PtpContainerHeader);
    while (p < rpkt.data() + rh->total_length_bytes)
    {
      std::uint32_t v = std::uint32_t(p[0]) | (std::uint32_t(p[1]) << 8) |
                        (std::uint32_t(p[2]) << 16) |
                        (std::uint32_t(p[3]) << 24);
      r.params.push_back(v);
      p += 4;
    }
    return r;
  }

  // If RESPONSE directly, accept it (some bodies skip DATA even when you
  // “expect” it)
  if (h->container_type == PTP_CONTAINER_RESPONSE)
  {
    r.response_code = h->operation_or_response;

    const std::uint8_t *p = pkt.data() + sizeof(PtpContainerHeader);
    while (p < pkt.data() + h->total_length_bytes)
    {
      std::uint32_t v = std::uint32_t(p[0]) | (std::uint32_t(p[1]) << 8) |
                        (std::uint32_t(p[2]) << 16) |
                        (std::uint32_t(p[3]) << 24);
      r.params.push_back(v);
      p += 4;
    }
    // r.data stays empty if device didn’t send DATA.
    return r;
  }

  throw std::runtime_error("unexpected container type");
}

std::optional<std::uint32_t> CameraPTP::wait_object_added(int timeout_ms,
                                                          int poll_ms)
{
  const int tries = timeout_ms / poll_ms;
  for (int i = 0; i < tries; ++i)
  {
    auto ev = event(poll_ms); // interrupt IN
    if (ev.size() >= 16)
    {
      const std::uint16_t type = ev[4] | (ev[5] << 8); // 4 = Event
      const std::uint16_t code = ev[6] | (ev[7] << 8); // 0x4002 = ObjectAdded
      if (type == PTP_CONTAINER_EVENT && code == PTP_EVENT_ObjectAdded)
      {
        std::uint32_t handle =
            ev[12] | (ev[13] << 8) | (ev[14] << 16) | (ev[15] << 24);
        return handle;
      }
    }
  }
  return std::nullopt;
}

// convenience
std::vector<std::uint8_t> CameraPTP::get_device_info()
{
  return transact(PTP_OP_GetDeviceInfo, {}, nullptr, true).data;
}

std::vector<std::uint32_t> CameraPTP::get_storage_ids()
{
  auto d = transact(PTP_OP_GetStorageIDs, {}, nullptr, true).data;
  std::vector<std::uint32_t> ids;
  for (size_t i = 0; i + 3 < d.size(); i += 4)
  {
    std::uint32_t v = std::uint32_t(d[i]) | (std::uint32_t(d[i + 1]) << 8) |
                      (std::uint32_t(d[i + 2]) << 16) |
                      (std::uint32_t(d[i + 3]) << 24);
    ids.push_back(v);
  }
  return ids;
}

std::vector<std::uint8_t>
CameraPTP::get_storage_info(std::uint32_t storage_id)
{
  return transact(PTP_OP_GetStorageInfo, {storage_id}, nullptr, true).data;
}

std::uint32_t CameraPTP::get_num_objects(std::uint32_t storage,
                                         std::uint32_t format,
                                         std::uint32_t assoc)
{
  auto r =
      transact(PTP_OP_GetNumObjects, {storage, format, assoc}, nullptr, false);
  return r.params.empty() ? 0 : r.params[0];
}

std::vector<std::uint32_t> CameraPTP::get_object_handles()
{
  // Pick a real storage ID if available; fall back to 0 (all).
  auto sids = get_storage_ids();
  std::uint32_t sid = sids.empty() ? 0u : sids[0];

  return get_object_handles(sid, 0u, 0xFFFFFFFFu);
}

std::vector<std::uint32_t> CameraPTP::get_object_handles(std::uint32_t storage,
                                                         std::uint32_t format,
                                                         std::uint32_t assoc)
{
  // Ask for all formats and all associations
  auto d =
      transact(PTP_OP_GetObjectHandles, {storage, format, assoc}, nullptr, true)
          .data;

  std::vector<std::uint32_t> out;
  if (d.size() < 4)
    return out;

  // Prefer the canonical PTP array: <count:u32><items...>
  std::uint32_t n = std::uint32_t(d[0]) | (std::uint32_t(d[1]) << 8) |
                    (std::uint32_t(d[2]) << 16) | (std::uint32_t(d[3]) << 24);

  if (4 + n * 4 <= d.size())
  {
    out.reserve(n);
    const std::uint8_t *p = d.data() + 4;
    for (std::uint32_t i = 0; i < n; ++i, p += 4)
    {
      out.push_back(std::uint32_t(p[0]) | (std::uint32_t(p[1]) << 8) |
                    (std::uint32_t(p[2]) << 16) | (std::uint32_t(p[3]) << 24));
    }
    return out;
  }

  // Fallback: treat payload as raw u32 list
  for (size_t i = 0; i + 3 < d.size(); i += 4)
  {
    out.push_back(std::uint32_t(d[i]) | (std::uint32_t(d[i + 1]) << 8) |
                  (std::uint32_t(d[i + 2]) << 16) |
                  (std::uint32_t(d[i + 3]) << 24));
  }
  return out;
}

std::vector<std::uint8_t> CameraPTP::get_object_info(std::uint32_t handle)
{
  return transact(PTP_OP_GetObjectInfo, {handle}, nullptr, true).data;
}

std::vector<std::uint8_t> CameraPTP::get_object(std::uint32_t handle)
{
  return transact(PTP_OP_GetObject, {handle}, nullptr, true).data;
}

std::vector<std::uint8_t>
CameraPTP::get_partial_object(std::uint32_t handle, std::uint32_t offset,
                              std::uint32_t max_bytes)
{
  return transact(PTP_OP_GetPartialObject, {handle, offset, max_bytes}, nullptr,
                  true)
      .data;
}

std::vector<std::uint8_t> CameraPTP::get_thumb(std::uint32_t handle)
{
  return transact(PTP_OP_GetThumb, {handle}, nullptr, true).data;
}

void CameraPTP::send_object_info(
    const std::vector<std::uint8_t> &info_dataset)
{
  (void)transact(PTP_OP_SendObjectInfo, {}, &info_dataset, false);
}

void CameraPTP::send_object(const std::vector<std::uint8_t> &object_bytes)
{
  (void)transact(PTP_OP_SendObject, {}, &object_bytes, false);
}

void CameraPTP::delete_object(std::uint32_t handle)
{
  (void)transact(PTP_OP_DeleteObject, {handle}, nullptr, false);
}

void CameraPTP::move_object(std::uint32_t handle, std::uint32_t storage,
                            std::uint32_t parent)
{
  (void)transact(PTP_OP_MoveObject, {handle, storage, parent}, nullptr, false);
}

void CameraPTP::copy_object(std::uint32_t handle, std::uint32_t storage,
                            std::uint32_t parent)
{
  (void)transact(PTP_OP_CopyObject, {handle, storage, parent}, nullptr, false);
}

void CameraPTP::initiate_capture(std::uint32_t storage, std::uint32_t format)
{
  (void)transact(PTP_OP_InitiateCapture, {storage, format}, nullptr, false);
}

void CameraPTP::initiate_open_capture()
{
  (void)transact(PTP_OP_InitiateOpenCapture, {}, nullptr, false);
}

void CameraPTP::terminate_open_capture()
{
  (void)transact(PTP_OP_TerminateOpenCapture, {}, nullptr, false);
}

void CameraPTP::reset_device()
{
  (void)transact(PTP_OP_ResetDevice, {}, nullptr, false);
}

void CameraPTP::power_down()
{
  (void)transact(PTP_OP_PowerDown, {}, nullptr, false);
}

std::vector<std::uint8_t>
CameraPTP::get_device_prop_desc(std::uint16_t prop_code)
{
  return transact(PTP_OP_GetDevicePropDesc, {prop_code}, nullptr, true).data;
}

std::vector<std::uint8_t>
CameraPTP::get_device_prop_value(std::uint16_t prop_code)
{
  return transact(PTP_OP_GetDevicePropValue, {prop_code}, nullptr, true).data;
}

void CameraPTP::set_device_prop_value(std::uint16_t prop_code,
                                      const std::vector<std::uint8_t> &raw)
{
  (void)transact(PTP_OP_SetDevicePropValue, {prop_code}, &raw, false);
}

void CameraPTP::reset_device_prop_value(std::uint16_t prop_code)
{
  (void)transact(PTP_OP_ResetDevicePropValue, {prop_code}, nullptr, false);
}

// PTPy-like helpers
std::vector<std::uint8_t>
CameraPTP::mesg(std::uint16_t opcode,
                const std::vector<std::uint32_t> &params)
{
  return transact(opcode, params, nullptr, true).data;
}

std::vector<std::uint8_t> CameraPTP::event(unsigned timeout_ms)
{
  std::uint8_t buf[64];
  int n = transport_.read_intr(buf, sizeof(buf), timeout_ms);
  return std::vector<std::uint8_t>(buf, buf + n);
}
