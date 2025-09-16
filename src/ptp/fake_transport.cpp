#include "ptp/fake_transport.h"

inline std::vector<uint8_t>
build_resp(uint16_t rc, uint32_t txn,
           const std::vector<uint32_t> &params = {})
{
    std::vector<uint8_t> b;
    b.reserve(12 + params.size() * 4);
    put_32le(b, uint32_t(12 + params.size() * 4));
    put_16le(b, uint16_t(3)); // RESPONSE
    put_16le(b, rc);
    put_32le(b, txn);
    for (uint32_t p : params)
        put_32le(b, p);
    return b;
}

void FakeTransport::queue_read(const std::vector<uint8_t> &v)
{
    rx.insert(rx.end(), v.begin(), v.end());
}

void FakeTransport::open_first() { open_ = true; }
void FakeTransport::open_vid_pid(uint16_t, uint16_t) { open_ = true; }
bool FakeTransport::is_open() const { return open_; }
void FakeTransport::close()
{
    open_ = false;
    writes.clear();
    rx.clear();
}

void FakeTransport::write_exact(const void *data, int len, unsigned)
{
    const auto *p = static_cast<const uint8_t *>(data);
    writes.emplace_back(p, p + len);
    // capture txn from the Command to auto-reply later
    if (len >= 12 && read_16le(&writes.back()[4]) == PTP_CONTAINER_COMMAND)
        last_txn = read_32le(&writes.back()[8]);
}

int FakeTransport::read_some(void *buf, int max, unsigned)
{
    ensure_auto_ok(); // push OK response if nothing queued yet
    if (rx.empty())
        return 0;
    const int n = std::min<int>(max, (int)rx.size());
    auto *out = static_cast<uint8_t *>(buf);
    std::copy_n(rx.begin(), n, out);
    rx.erase(rx.begin(), rx.begin() + n);
    return n;
}

int FakeTransport::read_intr(void *buf, int max, unsigned)
{
    if (ev_.empty())
        return 0;
    const int n = std::min<int>(max, (int)ev_.size());
    auto *out = static_cast<uint8_t *>(buf);
    std::copy_n(ev_.begin(), n, out);
    ev_.erase(ev_.begin(), ev_.begin() + n);
    return n;
}

void FakeTransport::ensure_auto_ok()
{
    if (!auto_ok || !rx.empty() || last_txn == 0)
        return;
    queue_read(build_resp(PTP_RESP_OK, last_txn));
}