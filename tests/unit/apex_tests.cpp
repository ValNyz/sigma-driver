#include <catch2/catch_test_macros.hpp>
#include <catch2/catch_version_macros.hpp>

static_assert(CATCH_VERSION_MAJOR >= 3, "Catch2 v3 required");

#include <sigma/apex.h>
#include <limits>
#include <vector>
#include <utility>

static void check_roundtrip(const ApexConverter& conv, std::uint8_t code) {
  auto v = conv.decode_uint8(code);
  REQUIRE(v.has_value());
  CHECK(conv.encode_uint8(*v) == code);
}

TEST_CASE("ISO: decode ↔ encode roundtrip on known codes") {
  check_roundtrip(ISOSpeedConverter, 32);   // 100
  check_roundtrip(ISOSpeedConverter, 35);   // 125
  check_roundtrip(ISOSpeedConverter, 48);   // 400
  check_roundtrip(ISOSpeedConverter, 112);  // 102400
}

TEST_CASE("ISO: encode clamps to nearest endpoint") {
  // min {code=0, value=6}, max {code=112, value=102400}
  CHECK(ISOSpeedConverter.encode_uint8(1.0) == 0);
  CHECK(ISOSpeedConverter.encode_uint8(200000.0) == 112);
}

TEST_CASE("ISO: encode tie prefers lower value (midpoint 100↔125)") {
  // 112.5 is exactly halfway. enctable_ is value-sorted, min_element picks first → code for 100.
  CHECK(ISOSpeedConverter.encode_uint8(112.5) == 32);
}

TEST_CASE("ISO: monotonic non-decreasing after encode→decode") {
  // Sample across range and ensure decoded representative value is non-decreasing.
  const double samples[] = {6,8,10,12,16,20,25,32,40,50,64,80,100,125,160,200,250,320,400,
                            500,640,800,1000,1250,1600,2000,2500,3200,4000,5000,6400,8000,
                            10000,12800,16000,20000,25600,32000,40000,51200,64000,80000,102400};
  double last = -std::numeric_limits<double>::infinity();
  for (double x : samples) {
    auto code = ISOSpeedConverter.encode_uint8(x);
    auto v    = ISOSpeedConverter.decode_uint8(code);
    REQUIRE(v.has_value());
    CHECK(*v >= last);
    last = *v;
  }
}

TEST_CASE("ISO: decode returns empty for unknown codes") {
  // 1 is not present in the ISO table
  CHECK_FALSE(ISOSpeedConverter.decode_uint8(1).has_value());
}

// -------- Shutter speed (1/2-stop table) --------
TEST_CASE("Shutter 1/2-stop: roundtrip on representative codes") {
  check_roundtrip(ShutterSpeed2Converter, 56);   // 1 s
  check_roundtrip(ShutterSpeed2Converter, 112);  // 1/125 s
  check_roundtrip(ShutterSpeed2Converter, 160);  // 1/8000 s
  CHECK_FALSE(ShutterSpeed2Converter.decode_uint8(1).has_value());
}

// -------- Shutter speed (1/3-stop table) --------
TEST_CASE("Shutter 1/3-stop: roundtrip on representative codes") {
  check_roundtrip(ShutterSpeed3Converter, 56);   // 1 s
  check_roundtrip(ShutterSpeed3Converter, 112);  // 1/125 s
  check_roundtrip(ShutterSpeed3Converter, 160);  // 1/8000 s
  CHECK_FALSE(ShutterSpeed3Converter.decode_uint8(1).has_value());
}

// -------- Aperture (1/2-stop) --------
TEST_CASE("Aperture 1/2-stop: roundtrip on representative codes") {
  check_roundtrip(Aperture2Converter, 32);  // f/2.8
  check_roundtrip(Aperture2Converter, 56);  // f/8
  check_roundtrip(Aperture2Converter, 72);  // f/16
  CHECK_FALSE(Aperture2Converter.decode_uint8(7).has_value());
}

// -------- Aperture (1/3-stop) --------
TEST_CASE("Aperture 1/3-stop: roundtrip on representative codes") {
  check_roundtrip(Aperture3Converter, 32);  // f/2.8
  check_roundtrip(Aperture3Converter, 56);  // f/8
  check_roundtrip(Aperture3Converter, 72);  // f/16
  CHECK_FALSE(Aperture3Converter.decode_uint8(7).has_value());
}

// -------- Exposure compensation (1/2-stop table) --------
TEST_CASE("ExpComp 1/2-stop: roundtrip on positives and negatives") {
  check_roundtrip(ExpComp2Converter, 0);    // 0.0
  check_roundtrip(ExpComp2Converter, 8);    // +1.0
  check_roundtrip(ExpComp2Converter, 248);  // -1.0
  CHECK_FALSE(ExpComp2Converter.decode_uint8(1).has_value());
}

// -------- Exposure compensation (1/3-stop table) --------
TEST_CASE("ExpComp 1/3-stop: roundtrip on positives and negatives") {
  check_roundtrip(ExpComp3Converter, 8);    // +1.0
  check_roundtrip(ExpComp3Converter, 48);   // +6.0
  check_roundtrip(ExpComp3Converter, 248);  // -1.0
  check_roundtrip(ExpComp3Converter, 208);  // -6.0
  CHECK_FALSE(ExpComp3Converter.decode_uint8(2).has_value());
}