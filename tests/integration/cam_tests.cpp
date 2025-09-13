#include <catch2/catch_test_macros.hpp>
#include <catch2/catch_version_macros.hpp>

static_assert(CATCH_VERSION_MAJOR >= 3, "Catch2 v3 required");

#include "ptp/ptp.h"
#include "ptp/utils.h"
#include "sigma/schema.h"
#include "sigma/sigma_ptp.h"
#include "fake_transport.h"

TEST_CASE("CamDataGroup1 data-out frame bytes") {
  FakeTransport tp;
  SigmaCamera cam(tp);

  CamDataGroup1 g{};
  g.shutterSpeed = 0x2A;
  g.isoSpeed = 400;
  auto payload = g.encode();

  constexpr uint16_t OP = static_cast<uint16_t>(SigmaOp::GetCamDataGroup1); // replace with real opcode

  auto r = cam.transact(OP, {}, &payload, false);
  REQUIRE(r.response_code == PTP_RESP_OK);

  REQUIRE(tp.writes.size() == 2);
  const auto& cmd  = tp.writes[0];
  const auto& data = tp.writes[1];

  // --- Command container
  REQUIRE(cmd.size() == 12);
  REQUIRE(read_16le(&cmd[4]) == PTP_CONTAINER_COMMAND);
  REQUIRE(read_16le(&cmd[6]) == OP);                // opcode, not DATA
  const uint32_t txn = read_32le(&cmd[8]);
  REQUIRE(read_32le(&cmd[0]) == cmd.size());        // length field

  // --- Data container
  REQUIRE(data.size() >= 12);
  REQUIRE(read_16le(&data[4]) == PTP_CONTAINER_DATA);
  REQUIRE(read_16le(&data[6]) == OP);               // same opcode
  REQUIRE(read_32le(&data[8]) == txn);              // same transaction id
  REQUIRE(read_32le(&data[0]) == data.size());      // length field
  REQUIRE(data.size() == 12 + payload.size());
  REQUIRE(std::equal(payload.begin(), payload.end(), data.begin() + 12));
}