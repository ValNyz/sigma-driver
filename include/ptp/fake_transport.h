// tests/support/FakeTransport.h
#pragma once
#include "utils/utils.h"
#include "ptp/ptp.h"
#include "ptp/transport.h"
#include <algorithm>
#include <cstdint>
#include <cstring>
#include <deque>
#include <stdexcept>
#include <vector>

inline std::vector<uint8_t>
build_resp(uint16_t rc, uint32_t txn,
           const std::vector<uint32_t> &params = {}) {
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

class FakeTransport : public Transport {
public:
  // recorded writes for assertions
  std::vector<std::vector<uint8_t>> writes;

  // optional RX queue (useful for future data-in tests)
  void queue_read(const std::vector<uint8_t> &v) {
    rx.insert(rx.end(), v.begin(), v.end());
  }

  // Transport API
  void open_first() override { open_ = true; }
  void open_vid_pid(uint16_t, uint16_t) override { open_ = true; }
  bool is_open() const override { return open_; }
  void close() override {
    open_ = false;
    writes.clear();
    rx.clear();
  }

  void write_exact(const void *data, int len, unsigned) override {
    const auto *p = static_cast<const uint8_t *>(data);
    writes.emplace_back(p, p + len);
    // capture txn from the Command to auto-reply later
    if (len >= 12 && read_16le(&writes.back()[4]) == PTP_CONTAINER_COMMAND)
      last_txn = read_32le(&writes.back()[8]);
  }

  int read_some(void *buf, int max, unsigned) override {
    ensure_auto_ok(); // push OK response if nothing queued yet
    if (rx.empty())
      return 0;
    const int n = std::min<int>(max, (int)rx.size());
    auto *out = static_cast<uint8_t *>(buf);
    std::copy_n(rx.begin(), n, out);
    rx.erase(rx.begin(), rx.begin() + n);
    return n;
  }

  int read_intr(void *buf, int max, unsigned) override {
    if (ev_.empty())
      return 0;
    const int n = std::min<int>(max, (int)ev_.size());
    auto *out = static_cast<uint8_t *>(buf);
    std::copy_n(ev_.begin(), n, out);
    ev_.erase(ev_.begin(), ev_.begin() + n);
    return n;
  }

private:
  void ensure_auto_ok() {
    if (!auto_ok || !rx.empty() || last_txn == 0)
      return;
    queue_read(build_resp(PTP_RESP_OK, last_txn));
  }

  bool open_{true};
  std::deque<uint8_t> rx;
  std::deque<uint8_t> ev_;
  uint32_t last_txn{0};
  bool auto_ok{true}; // always on; no setter needed
};
