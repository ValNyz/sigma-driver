#include <catch2/catch_test_macros.hpp>
#include <catch2/catch_version_macros.hpp>
#include <catch2/catch_approx.hpp>

static_assert(CATCH_VERSION_MAJOR >= 3, "Catch2 v3 required");

#include <cstdint>
#include <vector>
#include <sigma/schema.h>
#include <utils/utils.h>

static void rt_eq_u8(const std::optional<std::uint8_t> &a,
                     const std::optional<std::uint8_t> &b)
{
  REQUIRE(a.has_value());
  REQUIRE(b.has_value());
  CHECK(*a == *b);
}

// -------- CamDataGroup1 --------
TEST_CASE("CamDataGroup1: encode→decode roundtrip (writable fields)")
{
  CamDataGroup1 a;
  a.shutterSpeed = 0x20;
  a.aperture = 0x10;
  a.programShift = static_cast<ProgramShift>(1);
  a.isoAuto = static_cast<ISOAuto>(1);
  a.isoSpeed = 200;
  a.expComp = 3;
  a.abValue = 1;
  a.abSetting = static_cast<ABSetting>(2);

  const auto bytes = a.encode();

  CamDataGroup1 b;
  b.decode(bytes);

  rt_eq_u8(a.shutterSpeed, b.shutterSpeed);
  rt_eq_u8(a.aperture, b.aperture);
  REQUIRE(b.programShift.has_value());
  CHECK(*b.programShift == *a.programShift);
  REQUIRE(b.isoAuto.has_value());
  CHECK(*b.isoAuto == *a.isoAuto);
  rt_eq_u8(a.isoSpeed, b.isoSpeed);
  rt_eq_u8(a.expComp, b.expComp);
  rt_eq_u8(a.abValue, b.abValue);
  REQUIRE(b.abSetting.has_value());
  CHECK(*b.abSetting == *a.abSetting);

  // Read-only remain unset when not present.
  CHECK_FALSE(b.frameBufferState.has_value());
  CHECK_FALSE(b.mediaFreeSpace.has_value());
  CHECK_FALSE(b.mediaStatus.has_value());
  CHECK_FALSE(b.currentLensFocalLength.has_value());
}

// Presence bits for Group1 read-only fields
TEST_CASE("CamDataGroup1: decode read-only fields in order")
{
  using u8 = std::uint8_t;
  using u16 = std::uint16_t;
  const u16 FP = 0 | 0x0001 /*FrameBufferState*/
                 | 0x0002   /*MediaFreeSpace*/
                 | 0x0004   /*MediaStatus*/
                 | 0x0008   /*CurrentLensFocalLength*/
                 | 0x0010   /*BatteryState*/
                 | 0x0020   /*ABShotRemainNumber*/
                 | 0x0040 /*ExpCompExcludeAB*/;

  std::vector<u8> raw;
  put_8(raw, 0x00);
  put_16be(raw, FP);
  put_8(raw, 0x7A);                    // frameBufferState
  put_16le(raw, 0x3456);               // mediaFreeSpace
  put_8(raw, 0x02);                    // mediaStatus
  put_16le(raw, fp12_4_encode(35.5f)); // currentLensFocalLength
  put_8(raw, 0x80);                    // batteryState
  put_8(raw, 0x05);                    // abShotRemainNumber
  put_8(raw, 0x01);                    // expCompExcludeAB
  put_8(raw, 0x00);                    // _Parity

  CamDataGroup1 g;
  g.decode(raw);
  REQUIRE(g.frameBufferState.has_value());
  CHECK(*g.frameBufferState == 0x7A);
  REQUIRE(g.mediaFreeSpace.has_value());
  CHECK(*g.mediaFreeSpace == 0x3456);
  REQUIRE(g.mediaStatus.has_value());
  CHECK(*g.mediaStatus == 0x02);
  REQUIRE(g.currentLensFocalLength.has_value());
  CHECK(*g.currentLensFocalLength == Catch::Approx(35.5f).epsilon(1.0 / 16.0 + 1e-6));
  REQUIRE(g.batteryState.has_value());
  CHECK(*g.batteryState == 0x80);
  REQUIRE(g.abShotRemainNumber.has_value());
  CHECK(*g.abShotRemainNumber == 0x05);
  REQUIRE(g.expCompExcludeAB.has_value());
  CHECK(*g.expCompExcludeAB == 0x01);
}

// -------- CamDataGroup2 --------
TEST_CASE("CamDataGroup2: roundtrip")
{
  CamDataGroup2 a;
  a.driveMode = static_cast<DriveMode>(1);
  a.specialMode = static_cast<SpecialMode>(2);
  a.exposureMode = static_cast<ExposureMode>(3);
  a.aeMeteringMode = static_cast<AEMeteringMode>(4);
  a.flashType = static_cast<FlashType>(1);
  a.flashMode = static_cast<FlashMode>(2);
  a.flashSetting = static_cast<FlashSetting>(3);
  a.whiteBalance = static_cast<WhiteBalance>(4);
  a.resolution = static_cast<Resolution>(5);
  a.imageQuality = static_cast<ImageQuality>(6);

  CamDataGroup2 b;
  b.decode(a.encode());

  REQUIRE(b.driveMode.has_value());
  CHECK(*b.driveMode == *a.driveMode);
  REQUIRE(b.specialMode.has_value());
  CHECK(*b.specialMode == *a.specialMode);
  REQUIRE(b.exposureMode.has_value());
  CHECK(*b.exposureMode == *a.exposureMode);
  REQUIRE(b.aeMeteringMode.has_value());
  CHECK(*b.aeMeteringMode == *a.aeMeteringMode);
  REQUIRE(b.flashType.has_value());
  CHECK(*b.flashType == *a.flashType);
  REQUIRE(b.flashMode.has_value());
  CHECK(*b.flashMode == *a.flashMode);
  REQUIRE(b.flashSetting.has_value());
  CHECK(*b.flashSetting == *a.flashSetting);
  REQUIRE(b.whiteBalance.has_value());
  CHECK(*b.whiteBalance == *a.whiteBalance);
  REQUIRE(b.resolution.has_value());
  CHECK(*b.resolution == *a.resolution);
  REQUIRE(b.imageQuality.has_value());
  CHECK(*b.imageQuality == *a.imageQuality);
}

// -------- CamDataGroup3 --------
TEST_CASE("CamDataGroup3: roundtrip with u12.4 focal lengths")
{
  CamDataGroup3 a;
  a.colorSpace = static_cast<ColorSpace>(2);
  a.colorMode = static_cast<ColorMode>(1);
  a.batteryKind = static_cast<BatteryKind>(3);
  a.lensWideFocalLength = 24.0f;
  a.lensTeleFocalLength = 70.5f;
  a.afAuxLight = static_cast<AFAuxLight>(1);
  a.afBeep = 5;
  a.timerSound = 7;
  a.destToSave = static_cast<DestToSave>(2);

  CamDataGroup3 b;
  b.decode(a.encode());

  REQUIRE(b.colorSpace.has_value());
  CHECK(*b.colorSpace == *a.colorSpace);
  REQUIRE(b.colorMode.has_value());
  CHECK(*b.colorMode == *a.colorMode);
  REQUIRE(b.batteryKind.has_value());
  CHECK(*b.batteryKind == *a.batteryKind);
  REQUIRE(b.lensWideFocalLength.has_value());
  REQUIRE(b.lensTeleFocalLength.has_value());
  CHECK(*b.lensWideFocalLength == Catch::Approx(*a.lensWideFocalLength).epsilon(1.0 / 16.0 + 1e-6));
  CHECK(*b.lensTeleFocalLength == Catch::Approx(*a.lensTeleFocalLength).epsilon(1.0 / 16.0 + 1e-6));
  REQUIRE(b.afAuxLight.has_value());
  CHECK(*b.afAuxLight == *a.afAuxLight);
  rt_eq_u8(a.afBeep, b.afBeep);
  rt_eq_u8(a.timerSound, b.timerSound);
  REQUIRE(b.destToSave.has_value());
  CHECK(*b.destToSave == *a.destToSave);
}

// -------- CamDataGroup4 --------
TEST_CASE("CamDataGroup4: roundtrip with LOC block")
{
  CamDataGroup4 a;
  a.contShootSpeed = static_cast<ContShootSpeed>(1);
  a.highISOExt = static_cast<HighISOExt>(2);
  a.lvMagnifyRatio = static_cast<LVMagnifyRatio>(3);
  a.dcCropMode = static_cast<DCCropMode>(4);
  a.shutterSound = 9;
  a.eImageStab = static_cast<EImageStab>(1);
  a.locDistortion = static_cast<LOCDistortion>(1);
  a.locChromaticAberration = static_cast<LOCChromaticAberration>(2);
  a.locDiffraction = static_cast<LOCDiffraction>(3);
  a.locVignetting = static_cast<LOCVignetting>(4);
  a.locColorShade = static_cast<LOCColorShade>(5);
  a.locColorShadeAcq = static_cast<LOCColorShadeAcq>(6);
  a.fillLight = -2;
  a.dngQuality = static_cast<DNGQuality>(1);
  a.hdr = static_cast<HDR>(2);

  CamDataGroup4 b;
  b.decode(a.encode());

  REQUIRE(b.dcCropMode.has_value());
  CHECK(*b.dcCropMode == *a.dcCropMode);
  REQUIRE(b.lvMagnifyRatio.has_value());
  CHECK(*b.lvMagnifyRatio == *a.lvMagnifyRatio);
  REQUIRE(b.highISOExt.has_value());
  CHECK(*b.highISOExt == *a.highISOExt);
  REQUIRE(b.contShootSpeed.has_value());
  CHECK(*b.contShootSpeed == *a.contShootSpeed);
  REQUIRE(b.hdr.has_value());
  CHECK(*b.hdr == *a.hdr);
  REQUIRE(b.dngQuality.has_value());
  CHECK(*b.dngQuality == *a.dngQuality);
  REQUIRE(b.fillLight.has_value());
  CHECK(*b.fillLight == *a.fillLight);
  REQUIRE(b.locDistortion.has_value());
  CHECK(*b.locDistortion == *a.locDistortion);
  REQUIRE(b.locChromaticAberration.has_value());
  CHECK(*b.locChromaticAberration == *a.locChromaticAberration);
  REQUIRE(b.locDiffraction.has_value());
  CHECK(*b.locDiffraction == *a.locDiffraction);
  REQUIRE(b.locVignetting.has_value());
  CHECK(*b.locVignetting == *a.locVignetting);
  REQUIRE(b.locColorShade.has_value());
  CHECK(*b.locColorShade == *a.locColorShade);
  REQUIRE(b.locColorShadeAcq.has_value());
  CHECK(*b.locColorShadeAcq == *a.locColorShadeAcq);
  REQUIRE(b.eImageStab.has_value());
  CHECK(*b.eImageStab == *a.eImageStab);
  rt_eq_u8(a.shutterSound, b.shutterSound);
}

// -------- CamDataGroup5 --------
TEST_CASE("CamDataGroup5: roundtrip with interval")
{
  CamDataGroup5 a;
  a.intervalTimerSecond = 10;
  a.intervalTimerFrame = 3;
  a.intervalTimerSecondRemain = 8;
  a.intervalTimerFrameRemain = 1;
  a.colorTemp = 5500;
  a.aspectRatio = static_cast<AspectRatio>(2);
  a.toneEffect = static_cast<ToneEffect>(3);
  a.afAuxLightEF = static_cast<AFAuxLightEF>(1);

  CamDataGroup5 b;
  b.decode(a.encode());

  REQUIRE(b.intervalTimerSecond.has_value());
  CHECK(*b.intervalTimerSecond == *a.intervalTimerSecond);
  REQUIRE(b.intervalTimerFrame.has_value());
  CHECK(*b.intervalTimerFrame == *a.intervalTimerFrame);
  REQUIRE(b.intervalTimerSecondRemain.has_value());
  CHECK(*b.intervalTimerSecondRemain == *a.intervalTimerSecondRemain);
  REQUIRE(b.intervalTimerFrameRemain.has_value());
  CHECK(*b.intervalTimerFrameRemain == *a.intervalTimerFrameRemain);
  REQUIRE(b.colorTemp.has_value());
  CHECK(*b.colorTemp == *a.colorTemp);
  REQUIRE(b.aspectRatio.has_value());
  CHECK(*b.aspectRatio == *a.aspectRatio);
  REQUIRE(b.toneEffect.has_value());
  CHECK(*b.toneEffect == *a.toneEffect);
  REQUIRE(b.afAuxLightEF.has_value());
  CHECK(*b.afAuxLightEF == *a.afAuxLightEF);
}

// -------- SnapCommand --------
TEST_CASE("SnapCommand: exact bytes")
{
  SnapCommand s; // defaults: Mode=GeneralCapt, Amount=1
  auto v = s.encode();
  REQUIRE(v.size() == 4);
  CHECK(v[0] == 0x00);
  CHECK(v[3] == 0x00);
}

// -------- PictFileInfo2 --------
TEST_CASE("PictFileInfo2: decode nominal")
{
  // Fixed 36-byte header laid out exactly as schema.cpp expects.
  std::vector<std::uint8_t> b = {
      // [0..11] unknown
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      // [12..15] FileAddress (LE) = 0x11223344
      0x44, 0x33, 0x22, 0x11,
      // [16..19] FileSize (LE) = 0x00010020
      0x20, 0x00, 0x01, 0x00,
      // [20..23] PathNameOffset (unused)
      0x00, 0x00, 0x00, 0x00,
      // [24..27] FileNameOffset (unused)
      0x00, 0x00, 0x00, 0x00,
      // [28..31] PictureFormat = "JPEG"
      'J', 'P', 'E', 'G',
      // [32..33] SizeX (LE) = 4000
      0xA0, 0x0F,
      // [34..35] SizeY (LE) = 3000
      0xB8, 0x0B};

  // PathName, FileName (C-strings)
  const char *path = "DCIM/100SIGMA";
  const char *name = "IMG0001.JPEG";
  b.insert(b.end(), path, path + std::strlen(path));
  b.push_back(0);
  b.insert(b.end(), name, name + std::strlen(name));
  b.push_back(0);

  PictFileInfo2 p;
  p.decode(b);
  CHECK(p.FileAddress == 0x11223344u);
  CHECK(p.FileSize == 0x00010020u);
  CHECK(std::string(p.PictureFormat) == "JPEG");
  CHECK(p.SizeX == 4000);
  CHECK(p.SizeY == 3000);
  CHECK(p.PathName == "DCIM/100SIGMA");
  CHECK(p.FileName == "IMG0001.JPEG");
}

// -------- BigPartialPictFile --------
TEST_CASE("BigPartialPictFile: truncates to available")
{
  std::vector<std::uint8_t> b;
  put_32le(b, 10u);                      // AcquiredSize
  b.insert(b.end(), {1, 2, 3, 4, 5, 6}); // only 6 available
  BigPartialPictFile x;
  x.decode(b);
  CHECK(x.AcquiredSize == 10);
  REQUIRE(x.PartialData.size() == 6);
  CHECK(x.PartialData.front() == 1);
  CHECK(x.PartialData.back() == 6);
}

// -------- ViewFrame --------
TEST_CASE("ViewFrame: strips 10-byte header")
{
  std::vector<std::uint8_t> b(10, 0xAA);
  b.insert(b.end(), {9, 8, 7, 6, 5});
  ViewFrame v;
  v.decode(b);
  REQUIRE(v.Data.size() == 5);
  CHECK(v.Data[0] == 9);
  CHECK(v.Data[4] == 5);
}

// -------- CamCaptStatus --------
TEST_CASE("CamCaptStatus: decode fields")
{
  std::vector<std::uint8_t> b = {
      0x00, 0x2A, 0x10, 0x11, 0x34, 0x12, 0x03, 0x00};
  CamCaptStatus s;
  s.decode(b);
  CHECK(s.ImageId == 0x2A);
  CHECK(s.ImageDBHead == 0x10);
  CHECK(s.ImageDBTail == 0x11);
  CHECK(static_cast<int>(s.Status) == 0x1234);
  CHECK(static_cast<int>(s.Dest) == 0x03);
}
