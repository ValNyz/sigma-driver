#include <catch2/catch_test_macros.hpp>
#include <catch2/catch_version_macros.hpp>

static_assert(CATCH_VERSION_MAJOR >= 3, "Catch2 v3 required");

#include "utils/log.h"
#include "utils/utils.h"
#include "utils/apex.h"
#include "ptp/ptp.h"
#include "sigma/schema.h"
#include "sigma/sigma_ptp.h"
#include "ptp/fake_transport.h"

TEST_CASE("SetCamDataGroup1 cmd/data out frame")
{
  FakeTransport tp;
  SigmaCamera cam(tp);

  CamDataGroup1 g{};
  g.isoAuto = ISOAuto::Manual;
  g.shutterSpeed = ShutterSpeed3Converter.encode_uint8(2);
  g.isoSpeed = ISOSpeedConverter.encode_uint8(1600);
  auto payload = g.encode();

  constexpr uint16_t OP = static_cast<uint16_t>(SigmaOp::SetCamDataGroup1);

  auto r = cam.transact(OP, {}, &payload, false);
  REQUIRE(r.response_code == PTP_RESP_OK);

  REQUIRE(tp.writes.size() == 2);
  const auto &cmd = tp.writes[0];
  const auto &data = tp.writes[1];

  // --- Command container
  REQUIRE(cmd.size() == 12);
  REQUIRE(read_16le(&cmd[4]) == PTP_CONTAINER_COMMAND);
  REQUIRE(read_16le(&cmd[6]) == OP); // opcode, not DATA
  const uint32_t txn = read_32le(&cmd[8]);
  REQUIRE(read_32le(&cmd[0]) == cmd.size()); // length field

  // --- Data container
  REQUIRE(data.size() >= 12);
  REQUIRE(read_16le(&data[4]) == PTP_CONTAINER_DATA);
  REQUIRE(read_16le(&data[6]) == OP);          // same opcode
  REQUIRE(read_32le(&data[8]) == txn);         // same transaction id
  REQUIRE(read_32le(&data[0]) == data.size()); // length field
  REQUIRE(data.size() == 12 + payload.size());
  REQUIRE(std::equal(payload.begin(), payload.end(), data.begin() + 12));

  auto cmd_m = cmd, data_m = data;
  mask_tid(cmd_m);
  mask_tid(data_m);

  LOG_INFO("SetCamDataGroup1 cmd/data out frame");
  log_hex_preview(LogLevel::Info, cmd_m.data(), cmd_m.size());
  log_hex_preview(LogLevel::Info, data_m.data(), data_m.size());

  // golden byte-for-byte comparisons
  const auto golden_cmd  = hex2bin("0C 00 00 00 01 00 16 90 00 00 00 00");                      // True frame obtained using wireshark
  const auto golden_data = hex2bin("13 00 00 00 02 00 16 90 00 00 00 00 00 19 00 30 00 40 00"); // True frame obtained using wireshark

  REQUIRE(cmd_m == golden_cmd);
  REQUIRE(data_m == golden_data);
}

TEST_CASE("SetCamDataGroup2 cmd/data out frame")
{
  FakeTransport tp;
  SigmaCamera cam(tp);

  CamDataGroup2 g{};
  g.exposureMode = ExposureMode::Manual;
  g.imageQuality = ImageQuality::DNG;

  auto payload = g.encode();

  constexpr uint16_t OP = static_cast<uint16_t>(SigmaOp::SetCamDataGroup2);

  auto r = cam.transact(OP, {}, &payload, false);
  REQUIRE(r.response_code == PTP_RESP_OK);

  REQUIRE(tp.writes.size() == 2);
  const auto &cmd = tp.writes[0];
  const auto &data = tp.writes[1];

  REQUIRE(cmd.size() == 12);
  REQUIRE(read_16le(&cmd[4]) == PTP_CONTAINER_COMMAND);
  REQUIRE(read_16le(&cmd[6]) == OP);
  const uint32_t txn = read_32le(&cmd[8]);
  REQUIRE(read_32le(&cmd[0]) == cmd.size());

  REQUIRE(data.size() >= 12);
  REQUIRE(read_16le(&data[4]) == PTP_CONTAINER_DATA);
  REQUIRE(read_16le(&data[6]) == OP);
  REQUIRE(read_32le(&data[8]) == txn);
  REQUIRE(read_32le(&data[0]) == data.size());
  REQUIRE(data.size() == 12 + payload.size());
  REQUIRE(std::equal(payload.begin(), payload.end(), data.begin() + 12));

  auto cmd_m = cmd, data_m = data;
  mask_tid(cmd_m);
  mask_tid(data_m);

  LOG_INFO("SetCamDataGroup2 cmd/data out frame");
  log_hex_preview(LogLevel::Info, cmd_m.data(), cmd_m.size());
  log_hex_preview(LogLevel::Info, data_m.data(), data_m.size());

  const auto golden_cmd  = hex2bin("0C 00 00 00 01 00 17 90 00 00 00 00");
  const auto golden_data = hex2bin("12 00 00 00 02 00 17 90 00 00 00 00 00 04 80 04 10 00");

  REQUIRE(cmd_m == golden_cmd);
  REQUIRE(data_m == golden_data);
}

TEST_CASE("SetCamDataGroup3 cmd/data out frame")
{
  FakeTransport tp;
  SigmaCamera cam(tp);

  CamDataGroup3 g{};
  g.destToSave = DestToSave::Both;
  g.colorMode = ColorMode::Normal;

  auto payload = g.encode();

  constexpr uint16_t OP = static_cast<uint16_t>(SigmaOp::SetCamDataGroup3);

  auto r = cam.transact(OP, {}, &payload, false);
  REQUIRE(r.response_code == PTP_RESP_OK);

  REQUIRE(tp.writes.size() == 2);
  const auto &cmd = tp.writes[0];
  const auto &data = tp.writes[1];

  REQUIRE(cmd.size() == 12);
  REQUIRE(read_16le(&cmd[4]) == PTP_CONTAINER_COMMAND);
  REQUIRE(read_16le(&cmd[6]) == OP);
  const uint32_t txn = read_32le(&cmd[8]);
  REQUIRE(read_32le(&cmd[0]) == cmd.size());

  REQUIRE(data.size() >= 12);
  REQUIRE(read_16le(&data[4]) == PTP_CONTAINER_DATA);
  REQUIRE(read_16le(&data[6]) == OP);
  REQUIRE(read_32le(&data[8]) == txn);
  REQUIRE(read_32le(&data[0]) == data.size());
  REQUIRE(data.size() == 12 + payload.size());
  REQUIRE(std::equal(payload.begin(), payload.end(), data.begin() + 12));

  auto cmd_m = cmd, data_m = data;
  mask_tid(cmd_m);
  mask_tid(data_m);

  LOG_INFO("SetCamDataGroup3 cmd/data out frame");
  log_hex_preview(LogLevel::Info, cmd_m.data(), cmd_m.size());
  log_hex_preview(LogLevel::Info, data_m.data(), data_m.size());

  const auto golden_cmd  = hex2bin("0C 00 00 00 01 00 18 90 00 00 00 00");
  const auto golden_data = hex2bin("12 00 00 00 02 00 18 90 00 00 00 00 00 10 80 00 03 00");

  REQUIRE(cmd_m == golden_cmd);
  REQUIRE(data_m == golden_data);
}

TEST_CASE("SetCamDataGroup4 cmd/data out frame")
{
  FakeTransport tp;
  SigmaCamera cam(tp);

  CamDataGroup4 g{};
  g.dngQuality = DNGQuality::Q14bit;
  g.eImageStab = EImageStab::Off;
  g.shutterSound = 0;

  auto payload = g.encode();

  constexpr uint16_t OP = static_cast<uint16_t>(SigmaOp::SetCamDataGroup4);

  auto r = cam.transact(OP, {}, &payload, false);
  REQUIRE(r.response_code == PTP_RESP_OK);

  REQUIRE(tp.writes.size() == 2);
  const auto &cmd = tp.writes[0];
  const auto &data = tp.writes[1];

  REQUIRE(cmd.size() == 12);
  REQUIRE(read_16le(&cmd[4]) == PTP_CONTAINER_COMMAND);
  REQUIRE(read_16le(&cmd[6]) == OP);
  const uint32_t txn = read_32le(&cmd[8]);
  REQUIRE(read_32le(&cmd[0]) == cmd.size());

  REQUIRE(data.size() >= 12);
  REQUIRE(read_16le(&data[4]) == PTP_CONTAINER_DATA);
  REQUIRE(read_16le(&data[6]) == OP);
  REQUIRE(read_32le(&data[8]) == txn);
  REQUIRE(read_32le(&data[0]) == data.size());
  REQUIRE(data.size() == 12 + payload.size());
  REQUIRE(std::equal(payload.begin(), payload.end(), data.begin() + 12));

  auto cmd_m = cmd, data_m = data;
  mask_tid(cmd_m);
  mask_tid(data_m);

  LOG_INFO("SetCamDataGroup4 cmd/data out frame");
  log_hex_preview(LogLevel::Info, cmd_m.data(), cmd_m.size());
  log_hex_preview(LogLevel::Info, data_m.data(), data_m.size());

  const auto golden_cmd  = hex2bin("0C 00 00 00 01 00 24 90 00 00 00 00");
  const auto golden_data = hex2bin("13 00 00 00 02 00 24 90 00 00 00 00 00 00 32 0E 02 00 00");
  REQUIRE(cmd_m == golden_cmd);
  REQUIRE(data_m == golden_data);
}

TEST_CASE("SetCamDataGroup5 cmd/data out frame")
{
  FakeTransport tp;
  SigmaCamera cam(tp);

  CamDataGroup5 g{};
  g.aspectRatio = AspectRatio::W21H9;

  auto payload = g.encode();

  constexpr uint16_t OP = static_cast<uint16_t>(SigmaOp::SetCamDataGroup5);

  auto r = cam.transact(OP, {}, &payload, false);
  REQUIRE(r.response_code == PTP_RESP_OK);

  REQUIRE(tp.writes.size() == 2);
  const auto &cmd = tp.writes[0];
  const auto &data = tp.writes[1];

  REQUIRE(cmd.size() == 12);
  REQUIRE(read_16le(&cmd[4]) == PTP_CONTAINER_COMMAND);
  REQUIRE(read_16le(&cmd[6]) == OP);
  const uint32_t txn = read_32le(&cmd[8]);
  REQUIRE(read_32le(&cmd[0]) == cmd.size());

  REQUIRE(data.size() >= 12);
  REQUIRE(read_16le(&data[4]) == PTP_CONTAINER_DATA);
  REQUIRE(read_16le(&data[6]) == OP);
  REQUIRE(read_32le(&data[8]) == txn);
  REQUIRE(read_32le(&data[0]) == data.size());
  REQUIRE(data.size() == 12 + payload.size());
  REQUIRE(std::equal(payload.begin(), payload.end(), data.begin() + 12));

  auto cmd_m = cmd, data_m = data;
  mask_tid(cmd_m);
  mask_tid(data_m);

  LOG_INFO("SetCamDataGroup5 cmd/data out frame");
  log_hex_preview(LogLevel::Info, cmd_m.data(), cmd_m.size());
  log_hex_preview(LogLevel::Info, data_m.data(), data_m.size());

  const auto golden_cmd  = hex2bin("0C 00 00 00 01 00 28 90 00 00 00 00");
  const auto golden_data = hex2bin("11 00 00 00 02 00 28 90 00 00 00 00 00 08 00 01 00");
  REQUIRE(cmd_m == golden_cmd);
  REQUIRE(data_m == golden_data);
}

TEST_CASE("SetCamDataGroupFocus cmd/data out frame")
{
  FakeTransport tp;
  SigmaCamera cam(tp);

  CamDataGroupFocus g{};
  g.focusMode = FocusMode::MF;
  // g.faceEyeAF = FaceEyeAF::Off;
  g.preConstAF = PreConstAF::Off;

  auto payload = g.encode();

  constexpr uint16_t OP = static_cast<uint16_t>(SigmaOp::SetCamDataGroupFocus);

  auto r = cam.transact(OP, {}, &payload, false);
  REQUIRE(r.response_code == PTP_RESP_OK);

  REQUIRE(tp.writes.size() == 2);
  const auto &cmd = tp.writes[0];
  const auto &data = tp.writes[1];

  REQUIRE(cmd.size() == 12);
  REQUIRE(read_16le(&cmd[4]) == PTP_CONTAINER_COMMAND);
  REQUIRE(read_16le(&cmd[6]) == OP);
  const uint32_t txn = read_32le(&cmd[8]);
  REQUIRE(read_32le(&cmd[0]) == cmd.size());

  REQUIRE(data.size() >= 12);
  REQUIRE(read_16le(&data[4]) == PTP_CONTAINER_DATA);
  REQUIRE(read_16le(&data[6]) == OP);
  REQUIRE(read_32le(&data[8]) == txn);
  REQUIRE(read_32le(&data[0]) == data.size());
  REQUIRE(data.size() == 12 + payload.size());
  REQUIRE(std::equal(payload.begin(), payload.end(), data.begin() + 12));

  auto cmd_m = cmd, data_m = data;
  mask_tid(cmd_m);
  mask_tid(data_m);

  LOG_INFO("SetCamDataGroupFocus cmd/data out frame");
  log_hex_preview(LogLevel::Info, cmd_m.data(), cmd_m.size());
  log_hex_preview(LogLevel::Info, data_m.data(), data_m.size());

  const auto golden_cmd  = hex2bin("0C 00 00 00 01 00 32 90 00 00 00 00");
  const auto golden_data = hex2bin("2C 00 00 00 02 00 32 90 00 00 00 00 20 00 00 00 02 00 00 00 01 00 01 00 01 00 00 00 01 00 00 00 51 00 01 00 01 00 00 00 00 00 00 00");

  REQUIRE(cmd_m == golden_cmd);
  REQUIRE(data_m == golden_data);
}


TEST_CASE("GetCamDataGroup1 cmd/data out frame")
{
  FakeTransport tp;
  SigmaCamera cam(tp);
  constexpr uint16_t OP = static_cast<uint16_t>(SigmaOp::GetCamDataGroup1);
  auto r = cam.transact(OP, {}, nullptr, true);

  REQUIRE(r.response_code == PTP_RESP_OK);

  REQUIRE(tp.writes.size() == 1);
  const auto &cmd = tp.writes[0];


  auto cmd_m = cmd;
  mask_tid(cmd_m);

  LOG_INFO("GetCamDataGroup1 cmd/data out frame");
  log_hex_preview(LogLevel::Info, cmd_m.data(), cmd_m.size());

  // golden byte-for-byte comparisons
  const auto golden_cmd = hex2bin("0C 00 00 00 01 00 12 90 00 00 00 00");

  REQUIRE(cmd_m  == golden_cmd);
}

TEST_CASE("GetCamDataGroup2 cmd/data out frame")
{
  FakeTransport tp;
  SigmaCamera cam(tp);
  constexpr uint16_t OP = static_cast<uint16_t>(SigmaOp::GetCamDataGroup2);
  auto r = cam.transact(OP, {}, nullptr, true);

  REQUIRE(r.response_code == PTP_RESP_OK);

  REQUIRE(tp.writes.size() == 1);
  const auto &cmd = tp.writes[0];


  auto cmd_m = cmd;
  mask_tid(cmd_m);

  LOG_INFO("GetCamDataGroup2 cmd/data out frame");
  log_hex_preview(LogLevel::Info, cmd_m.data(), cmd_m.size());

  // golden byte-for-byte comparisons
  const auto golden_cmd = hex2bin("0C 00 00 00 01 00 13 90 00 00 00 00");

  REQUIRE(cmd_m  == golden_cmd);
}

TEST_CASE("GetCamDataGroup3 cmd/data out frame")
{
  FakeTransport tp;
  SigmaCamera cam(tp);
  constexpr uint16_t OP = static_cast<uint16_t>(SigmaOp::GetCamDataGroup3);
  auto r = cam.transact(OP, {}, nullptr, true);

  REQUIRE(r.response_code == PTP_RESP_OK);

  REQUIRE(tp.writes.size() == 1);
  const auto &cmd = tp.writes[0];


  auto cmd_m = cmd;
  mask_tid(cmd_m);

  LOG_INFO("GetCamDataGroup3 cmd/data out frame");
  log_hex_preview(LogLevel::Info, cmd_m.data(), cmd_m.size());

  // golden byte-for-byte comparisons
  const auto golden_cmd = hex2bin("0C 00 00 00 01 00 14 90 00 00 00 00");

  REQUIRE(cmd_m  == golden_cmd);
}

TEST_CASE("GetCamDataGroup4 cmd/data out frame")
{
  FakeTransport tp;
  SigmaCamera cam(tp);
  constexpr uint16_t OP = static_cast<uint16_t>(SigmaOp::GetCamDataGroup4);
  auto r = cam.transact(OP, {}, nullptr, true);

  REQUIRE(r.response_code == PTP_RESP_OK);

  REQUIRE(tp.writes.size() == 1);
  const auto &cmd = tp.writes[0];


  auto cmd_m = cmd;
  mask_tid(cmd_m);

  LOG_INFO("GetCamDataGroup4 cmd/data out frame");
  log_hex_preview(LogLevel::Info, cmd_m.data(), cmd_m.size());

  // golden byte-for-byte comparisons
  const auto golden_cmd = hex2bin("0C 00 00 00 01 00 23 90 00 00 00 00");

  REQUIRE(cmd_m  == golden_cmd);
}

TEST_CASE("GetCamDataGroup5 cmd/data out frame")
{
  FakeTransport tp;
  SigmaCamera cam(tp);
  constexpr uint16_t OP = static_cast<uint16_t>(SigmaOp::GetCamDataGroup5);
  auto r = cam.transact(OP, {}, nullptr, true);

  REQUIRE(r.response_code == PTP_RESP_OK);

  REQUIRE(tp.writes.size() == 1);
  const auto &cmd = tp.writes[0];


  auto cmd_m = cmd;
  mask_tid(cmd_m);

  LOG_INFO("GetCamDataGroup5 cmd/data out frame");
  log_hex_preview(LogLevel::Info, cmd_m.data(), cmd_m.size());

  // golden byte-for-byte comparisons
  const auto golden_cmd = hex2bin("0C 00 00 00 01 00 27 90 00 00 00 00");

  REQUIRE(cmd_m  == golden_cmd);
}

TEST_CASE("GetCamDataGroupFocus cmd/data out frame")
{
  FakeTransport tp;
  SigmaCamera cam(tp);
  constexpr uint16_t OP = static_cast<uint16_t>(SigmaOp::GetCamDataGroupFocus);
  auto r = cam.transact(OP, {}, nullptr, true);

  REQUIRE(r.response_code == PTP_RESP_OK);

  REQUIRE(tp.writes.size() == 1);
  const auto &cmd = tp.writes[0];


  auto cmd_m = cmd;
  mask_tid(cmd_m);

  LOG_INFO("GetCamDataGroupFocus cmd/data out frame");
  log_hex_preview(LogLevel::Info, cmd_m.data(), cmd_m.size());

  // golden byte-for-byte comparisons
  const auto golden_cmd = hex2bin("0C 00 00 00 01 00 31 90 00 00 00 00");

  REQUIRE(cmd_m  == golden_cmd);
}

