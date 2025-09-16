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

inline std::vector<uint8_t> build_resp(uint16_t rc, uint32_t txn, const std::vector<uint32_t> &params);

class FakeTransport : public Transport
{
public:
  // recorded writes for assertions
  std::vector<std::vector<uint8_t>> writes;

  // optional RX queue (useful for future data-in tests)
  void queue_read(const std::vector<uint8_t> &v);

  // Transport API
  void open_first() override ;
  void open_vid_pid(uint16_t, uint16_t) override;
  bool is_open() const override;
  void close() override;

  void write_exact(const void *data, int len, unsigned) override;

  int read_some(void *buf, int max, unsigned) override;

  int read_intr(void *buf, int max, unsigned) override;

private:
  void ensure_auto_ok();

  bool open_{true};
  std::deque<uint8_t> rx;
  std::deque<uint8_t> ev_;
  uint32_t last_txn{0};
  bool auto_ok{true}; // always on; no setter needed
};
