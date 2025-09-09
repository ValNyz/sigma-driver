#include <algorithm>
#include <cstring>
#include <iostream>
#include <sstream>
#include <stdexcept>

#include "enum.h"
#include "ptp/utils.h"
#include "schema.h"

// ---------------- CamDataGroup1 ----------------
std::vector<std::uint8_t> CamDataGroup1::encode() const {
  std::uint16_t fp = 0;
  if (shutterSpeed)
    fp |= FP_ShutterSpeed;
  if (aperture)
    fp |= FP_Aperture;
  if (programShift)
    fp |= FP_ProgramShift;
  if (isoAuto)
    fp |= FP_ISOAuto;
  if (isoSpeed)
    fp |= FP_ISOSpeed;
  if (expComp)
    fp |= FP_ExpComp;
  if (abValue)
    fp |= FP_ABValue;
  if (abSetting)
    fp |= FP_ABSetting;

  std::vector<std::uint8_t> out;
  put_8(out, 0x00);  // _Header
  put_16be(out, fp); // FieldPresent

  if (shutterSpeed)
    put_8(out, *shutterSpeed);
  if (aperture)
    put_8(out, *aperture);
  if (programShift)
    put_8(out, static_cast<std::uint8_t>(*programShift));
  if (isoAuto)
    put_8(out, static_cast<std::uint8_t>(*isoAuto));
  if (isoSpeed)
    put_8(out, *isoSpeed);
  if (expComp)
    put_8(out, *expComp);
  if (abValue)
    put_8(out, *abValue);
  if (abSetting)
    put_8(out, static_cast<std::uint8_t>(*abSetting));

  put_8(out, 0x00); // _Parity
  return out;
}

void CamDataGroup1::decode(const std::vector<std::uint8_t> &rawdata) {
  size_t i = 0;
  (void)get_8(rawdata, i); // _Header
  std::uint16_t fp = get_16be(rawdata, i);
  auto has = [&](std::uint16_t b) { return (fp & b) != 0; };

  if (has(FP_ShutterSpeed))
    shutterSpeed = get_8(rawdata, i);
  else
    shutterSpeed.reset();
  if (has(FP_Aperture))
    aperture = get_8(rawdata, i);
  else
    aperture.reset();
  if (has(FP_ProgramShift))
    programShift = static_cast<ProgramShift>(get_8(rawdata, i));
  else
    programShift.reset();
  if (has(FP_ISOAuto))
    isoAuto = static_cast<ISOAuto>(get_8(rawdata, i));
  else
    isoAuto.reset();
  if (has(FP_ISOSpeed))
    isoSpeed = get_8(rawdata, i);
  else
    isoSpeed.reset();
  if (has(FP_ExpComp))
    expComp = get_8(rawdata, i);
  else
    expComp.reset();
  if (has(FP_ABValue))
    abValue = get_8(rawdata, i);
  else
    abValue.reset();
  if (has(FP_ABSetting))
    abSetting = static_cast<ABSetting>(get_8(rawdata, i));
  else
    abSetting.reset();

  if (has(FP_FrameBufferState))
    frameBufferState = get_8(rawdata, i);
  else
    frameBufferState.reset();
  if (has(FP_MediaFreeSpace))
    mediaFreeSpace = get_16le(rawdata, i);
  else
    mediaFreeSpace.reset();
  if (has(FP_MediaStatus))
    mediaStatus = get_8(rawdata, i);
  else
    mediaStatus.reset();
  if (has(FP_CurrentLensFocalLength))
    currentLensFocalLength = fp12_4_decode(get_16le(rawdata, i));
  else
    currentLensFocalLength.reset();
  if (has(FP_BatteryState))
    batteryState = get_8(rawdata, i);
  else
    batteryState.reset();
  if (has(FP_ABShotRemainNumber))
    abShotRemainNumber = get_8(rawdata, i);
  else
    abShotRemainNumber.reset();
  if (has(FP_ExpCompExcludeAB))
    expCompExcludeAB = get_8(rawdata, i);
  else
    expCompExcludeAB.reset();
  if (has(FP__Reserved0))
    (void)get_8(rawdata, i);
  (void)get_8(rawdata, i); // _Parity
}

// ---------------- CamDataGroup2 ----------------
std::vector<std::uint8_t> CamDataGroup2::encode() const {
  std::uint16_t fp = 0;
  if (aeMeteringMode)
    fp |= FP_AE;
  if (exposureMode)
    fp |= FP_Exp;
  if (specialMode)
    fp |= FP_Spec;
  if (driveMode)
    fp |= FP_Drive;
  if (imageQuality)
    fp |= FP_IQ;
  if (resolution)
    fp |= FP_Res;
  if (whiteBalance)
    fp |= FP_WB;
  if (flashSetting)
    fp |= FP_FlashS;
  if (flashMode)
    fp |= FP_FlashM;
  if (flashType)
    fp |= FP_FlashT;

  std::vector<std::uint8_t> out;
  put_8(out, 0x00);
  put_16be(out, fp);
  if (driveMode)
    put_8(out, static_cast<std::uint8_t>(*driveMode));
  if (specialMode)
    put_8(out, static_cast<std::uint8_t>(*specialMode));
  if (exposureMode)
    put_8(out, static_cast<std::uint8_t>(*exposureMode));
  if (aeMeteringMode)
    put_8(out, static_cast<std::uint8_t>(*aeMeteringMode));
  if (flashType)
    put_8(out, static_cast<std::uint8_t>(*flashType));
  if (flashMode)
    put_8(out, static_cast<std::uint8_t>(*flashMode));
  if (flashSetting)
    put_8(out, static_cast<std::uint8_t>(*flashSetting));
  if (whiteBalance)
    put_8(out, static_cast<std::uint8_t>(*whiteBalance));
  if (resolution)
    put_8(out, static_cast<std::uint8_t>(*resolution));
  if (imageQuality)
    put_8(out, static_cast<std::uint8_t>(*imageQuality));
  put_8(out, 0x00);
  return out;
}

void CamDataGroup2::decode(const std::vector<std::uint8_t> &raw) {
  size_t i = 0;
  (void)get_8(raw, i);
  std::uint16_t fp = get_16be(raw, i);
  auto has = [&](std::uint16_t b) { return (fp & b) != 0; };
  if (has(FP_Drive))
    driveMode = static_cast<DriveMode>(get_8(raw, i));
  else
    driveMode.reset();
  if (has(FP_Spec))
    specialMode = static_cast<SpecialMode>(get_8(raw, i));
  else
    specialMode.reset();
  if (has(FP_Exp))
    exposureMode = static_cast<ExposureMode>(get_8(raw, i));
  else
    exposureMode.reset();
  if (has(FP_AE))
    aeMeteringMode = static_cast<AEMeteringMode>(get_8(raw, i));
  else
    aeMeteringMode.reset();
  if (has(FP__Reserved3))
    (void)get_8(raw, i);
  if (has(FP__Reserved2))
    (void)get_8(raw, i);
  if (has(FP__Reserved1))
    (void)get_8(raw, i);
  if (has(FP__Reserved0))
    (void)get_8(raw, i);
  if (has(FP_FlashT))
    flashType = static_cast<FlashType>(get_8(raw, i));
  else
    flashType.reset();
  if (has(FP__Reserved4))
    (void)get_8(raw, i);
  if (has(FP_FlashM))
    flashMode = static_cast<FlashMode>(get_8(raw, i));
  else
    flashMode.reset();
  if (has(FP_FlashS))
    flashSetting = static_cast<FlashSetting>(get_8(raw, i));
  else
    flashSetting.reset();
  if (has(FP__Reserved5))
    (void)get_8(raw, i);
  if (has(FP_WB))
    whiteBalance = static_cast<WhiteBalance>(get_8(raw, i));
  else
    whiteBalance.reset();
  if (has(FP_Res))
    resolution = static_cast<Resolution>(get_8(raw, i));
  else
    resolution.reset();
  if (has(FP_IQ))
    imageQuality = static_cast<ImageQuality>(get_8(raw, i));
  else
    imageQuality.reset();
  (void)get_8(raw, i);
}

// ---------------- CamDataGroup3 ----------------
std::vector<std::uint8_t> CamDataGroup3::encode() const {
  std::uint16_t fp = 0;
  if (lensTeleFocalLength)
    fp |= FP_Tele;
  if (lensWideFocalLength)
    fp |= FP_Wide;
  if (batteryKind)
    fp |= FP_Batt;
  if (colorMode)
    fp |= FP_CMode;
  if (colorSpace)
    fp |= FP_CSpace;
  if (destToSave)
    fp |= FP_Save;
  if (timerSound)
    fp |= FP_Timer;
  if (afBeep)
    fp |= FP_Beep;
  if (afAuxLight)
    fp |= FP_AFL;

  std::vector<std::uint8_t> out;
  put_8(out, 0x00);
  put_16be(out, fp);
  if (colorSpace)
    put_8(out, static_cast<std::uint8_t>(*colorSpace));
  if (colorMode)
    put_8(out, static_cast<std::uint8_t>(*colorMode));
  if (batteryKind)
    put_8(out, static_cast<std::uint8_t>(*batteryKind));
  if (lensWideFocalLength)
    put_16le(out, fp12_4_encode(*lensWideFocalLength));
  if (lensTeleFocalLength)
    put_16le(out, fp12_4_encode(*lensTeleFocalLength));
  if (afAuxLight)
    put_8(out, static_cast<std::uint8_t>(*afAuxLight));
  if (afBeep)
    put_8(out, *afBeep);
  if (timerSound)
    put_8(out, *timerSound);
  if (destToSave)
    put_8(out, static_cast<std::uint8_t>(*destToSave));
  put_8(out, 0x00);
  return out;
}

void CamDataGroup3::decode(const std::vector<std::uint8_t> &raw) {
  size_t i = 0;
  (void)get_8(raw, i);
  std::uint16_t fp = get_16be(raw, i);
  auto has = [&](std::uint16_t b) { return (fp & b) != 0; };
  if (has(FP__Reserved0))
    (void)get_8(raw, i);
  if (has(FP__Reserved1))
    (void)get_8(raw, i);
  if (has(FP__Reserved2))
    (void)get_8(raw, i);
  if (has(FP_CSpace))
    colorSpace = static_cast<ColorSpace>(get_8(raw, i));
  else
    colorSpace.reset();
  if (has(FP_CMode))
    colorMode = static_cast<ColorMode>(get_8(raw, i));
  else
    colorMode.reset();
  if (has(FP_Batt))
    batteryKind = static_cast<BatteryKind>(get_8(raw, i));
  else
    batteryKind.reset();
  if (has(FP_Wide))
    lensWideFocalLength = fp12_4_decode(get_16le(raw, i));
  else
    lensWideFocalLength.reset();
  if (has(FP_Tele))
    lensTeleFocalLength = fp12_4_decode(get_16le(raw, i));
  else
    lensTeleFocalLength.reset();
  if (has(FP_AFL))
    afAuxLight = static_cast<AFAuxLight>(get_8(raw, i));
  else
    afAuxLight.reset();
  if (has(FP_Beep))
    afBeep = get_8(raw, i);
  else
    afBeep.reset();
  if (has(FP__Reserved3))
    (void)get_8(raw, i);
  if (has(FP__Reserved4))
    (void)get_8(raw, i);
  if (has(FP__Reserved5))
    (void)get_8(raw, i);
  if (has(FP_Timer))
    timerSound = get_8(raw, i);
  else
    timerSound.reset();
  if (has(FP__Reserved6))
    (void)get_8(raw, i);
  if (has(FP_Save))
    destToSave = static_cast<DestToSave>(get_8(raw, i));
  else
    destToSave.reset();
  (void)get_8(raw, i);
}

// ---------------- CamDataGroup4 ----------------
std::vector<std::uint8_t> CamDataGroup4::encode() const {
  std::uint16_t fp = 0;
  if (contShootSpeed)
    fp |= FP_Cont;
  if (highISOExt)
    fp |= FP_HiISO;
  if (lvMagnifyRatio)
    fp |= FP_LVM;
  if (dcCropMode)
    fp |= FP_Crop;
  if (shutterSound)
    fp |= FP_Snd;
  if (eImageStab)
    fp |= FP_Stab;
  bool hasLOC = locDistortion || locChromaticAberration || locDiffraction ||
                locVignetting || locColorShade || locColorShadeAcq;
  if (hasLOC)
    fp |= FP_LOC;
  if (fillLight)
    fp |= FP_Fill;
  if (dngQuality)
    fp |= FP_DNG;
  if (hdr)
    fp |= FP_HDR;

  std::vector<std::uint8_t> out;
  put_8(out, 0x00);
  put_16be(out, fp);
  if (dcCropMode)
    put_8(out, static_cast<std::uint8_t>(*dcCropMode));
  if (lvMagnifyRatio)
    put_8(out, static_cast<std::uint8_t>(*lvMagnifyRatio));
  if (highISOExt)
    put_8(out, static_cast<std::uint8_t>(*highISOExt));
  if (contShootSpeed)
    put_8(out, static_cast<std::uint8_t>(*contShootSpeed));
  if (hdr)
    put_8(out, static_cast<std::uint8_t>(*hdr));
  if (dngQuality)
    put_8(out, static_cast<std::uint8_t>(*dngQuality));
  if (fillLight)
    put_8(out, static_cast<std::uint8_t>(*fillLight));
  if (hasLOC) {
    put_8(out, static_cast<std::uint8_t>(
                   locDistortion.value_or(LOCDistortion::Off)));
    put_8(out, static_cast<std::uint8_t>(locChromaticAberration.value_or(
                   LOCChromaticAberration::Off)));
    put_8(out, static_cast<std::uint8_t>(
                   locDiffraction.value_or(LOCDiffraction::Off)));
    put_8(out, static_cast<std::uint8_t>(
                   locVignetting.value_or(LOCVignetting::Off)));
    put_8(out, static_cast<std::uint8_t>(
                   locColorShade.value_or(LOCColorShade::Off)));
    put_8(out, static_cast<std::uint8_t>(
                   locColorShadeAcq.value_or(LOCColorShadeAcq::Off)));
  }
  if (eImageStab)
    put_8(out, static_cast<std::uint8_t>(*eImageStab));
  if (shutterSound)
    put_8(out, *shutterSound);
  put_8(out, 0x00);
  return out;
}

void CamDataGroup4::decode(const std::vector<std::uint8_t> &raw) {
  size_t i = 0;
  (void)get_8(raw, i);
  std::uint16_t fp = get_16be(raw, i);
  auto has = [&](std::uint16_t b) { return (fp & b) != 0; };
  if (has(FP__Reserved0))
    (void)get_8(raw, i);
  if (has(FP__Reserved1))
    (void)get_8(raw, i);
  if (has(FP__Reserved2))
    (void)get_8(raw, i);
  if (has(FP__Reserved3))
    (void)get_8(raw, i);
  if (has(FP_Crop))
    dcCropMode = static_cast<DCCropMode>(get_8(raw, i));
  else
    dcCropMode.reset();
  if (has(FP_LVM))
    lvMagnifyRatio = static_cast<LVMagnifyRatio>(get_8(raw, i));
  else
    lvMagnifyRatio.reset();
  if (has(FP_HiISO))
    highISOExt = static_cast<HighISOExt>(get_8(raw, i));
  else
    highISOExt.reset();
  if (has(FP_Cont))
    contShootSpeed = static_cast<ContShootSpeed>(get_8(raw, i));
  else
    contShootSpeed.reset();
  if (has(FP_HDR))
    hdr = static_cast<HDR>(get_8(raw, i));
  else
    hdr.reset();
  if (has(FP_DNG))
    dngQuality = static_cast<DNGQuality>(get_8(raw, i));
  else
    dngQuality.reset();
  if (has(FP_Fill))
    fillLight = static_cast<std::int8_t>(get_8(raw, i));
  else
    fillLight.reset();
  if (has(FP_LOC)) {
    locDistortion = static_cast<LOCDistortion>(get_8(raw, i));
    locChromaticAberration = static_cast<LOCChromaticAberration>(get_8(raw, i));
    locDiffraction = static_cast<LOCDiffraction>(get_8(raw, i));
    locVignetting = static_cast<LOCVignetting>(get_8(raw, i));
    locColorShade = static_cast<LOCColorShade>(get_8(raw, i));
    locColorShadeAcq = static_cast<LOCColorShadeAcq>(get_8(raw, i));
  } else {
    locDistortion.reset();
    locChromaticAberration.reset();
    locDiffraction.reset();
    locVignetting.reset();
    locColorShade.reset();
    locColorShadeAcq.reset();
  }
  if (has(FP_Stab))
    eImageStab = static_cast<EImageStab>(get_8(raw, i));
  else
    eImageStab.reset();
  if (has(FP_Snd))
    shutterSound = get_8(raw, i);
  else
    shutterSound.reset();
  (void)get_8(raw, i);
}

// ---------------- CamDataGroup5 ----------------
std::vector<std::uint8_t> CamDataGroup5::encode() const {
  if (intervalTimerSecond.has_value() != intervalTimerFrame.has_value())
    throw std::invalid_argument("IntervalTimerSecond and IntervalTimerFrame "
                                "must both be set or both null");

  std::uint16_t fp = 0;
  bool interval = intervalTimerSecond && intervalTimerFrame;
  if (interval)
    fp |= FP_Int;
  if (colorTemp)
    fp |= FP_CT;
  if (aspectRatio)
    fp |= FP_AR;
  if (toneEffect)
    fp |= FP_Tone;
  if (afAuxLightEF)
    fp |= FP_AF;

  std::vector<std::uint8_t> out;
  put_8(out, 0x00);
  put_16be(out, fp);
  if (interval) {
    put_16le(out, *intervalTimerSecond);
    put_8(out, *intervalTimerFrame);
    put_16le(out, intervalTimerSecondRemain.value_or(0));
    put_8(out, intervalTimerFrameRemain.value_or(0));
  }
  if (colorTemp)
    put_16le(out, *colorTemp);
  if (aspectRatio)
    put_8(out, static_cast<std::uint8_t>(*aspectRatio));
  if (toneEffect)
    put_8(out, static_cast<std::uint8_t>(*toneEffect));
  if (afAuxLightEF)
    put_8(out, static_cast<std::uint8_t>(*afAuxLightEF));
  put_8(out, 0x00);
  return out;
}

void CamDataGroup5::decode(const std::vector<std::uint8_t> &raw) {
  size_t i = 0;
  (void)get_8(raw, i);
  std::uint16_t fp = get_16be(raw, i);
  auto has = [&](std::uint16_t b) { return (fp & b) != 0; };

  if (has(FP_Int)) {
    intervalTimerSecond = get_16le(raw, i);
    intervalTimerFrame = get_8(raw, i);
    intervalTimerSecondRemain = get_16le(raw, i);
    intervalTimerFrameRemain = get_8(raw, i);
  } else {
    intervalTimerSecond.reset();
    intervalTimerFrame.reset();
    intervalTimerSecondRemain.reset();
    intervalTimerFrameRemain.reset();
  }

  if (has(FP_CT))
    colorTemp = get_16le(raw, i);
  else
    colorTemp.reset();
  if (has(FP__Reserved0))
    (void)get_8(raw, i);
  if (has(FP_AR))
    aspectRatio = static_cast<AspectRatio>(get_8(raw, i));
  else
    aspectRatio.reset();
  if (has(FP__Reserved1))
    (void)get_8(raw, i);
  if (has(FP_Tone))
    toneEffect = static_cast<ToneEffect>(get_8(raw, i));
  else
    toneEffect.reset();
  if (has(FP__Reserved2))
    (void)get_8(raw, i); // reserved range; safe skip if present
  if (has(FP_AF))
    afAuxLightEF = static_cast<AFAuxLightEF>(get_8(raw, i));
  else
    afAuxLightEF.reset();
  if (has(FP__Reserved3))
    (void)get_8(raw, i);
  if (has(FP__Reserved4))
    (void)get_8(raw, i);
  if (has(FP__Reserved5))
    (void)get_8(raw, i);
  if (has(FP__Reserved6))
    (void)get_8(raw, i);
  if (has(FP__Reserved7))
    (void)get_8(raw, i);
  if (has(FP__Reserved8))
    (void)get_8(raw, i);
  if (has(FP__Reserved9))
    (void)get_8(raw, i);
  if (has(FP__Reserved10))
    (void)get_8(raw, i);
  (void)get_8(raw, i);
}

// ---------- CamCaptStatus ----------
void CamCaptStatus::decode(const std::vector<std::uint8_t> &raw) {
  // Layout:
  // [0]_Header u8, [1]ImageId u8, [2]ImageDBHead u8, [3]ImageDBTail u8,
  // [4..5]CaptStatus u16 LE, [6]DestToSave u8, [7]_Parity u8
  if (raw.size() < 8)
    throw std::runtime_error("CamCaptStatus: short buffer");
  ImageId = raw[1];
  ImageDBHead = raw[2];
  ImageDBTail = raw[3];
  Status = static_cast<CaptStatus>(read_16le(&raw[4]));
  Dest = static_cast<DestToSave>(raw[6]);
}

// ---------- SnapCommand ----------
std::vector<std::uint8_t> SnapCommand::encode() const {
  std::vector<std::uint8_t> out;
  out.reserve(4);
  out.push_back(0x00); // _Header
  out.push_back(static_cast<std::uint8_t>(Mode));
  out.push_back(Amount);
  out.push_back(0x00); // _Parity
  return out;
}

// ---------- PictFileInfo2 ----------
void PictFileInfo2::decode(const std::vector<std::uint8_t> &raw) {
  // Layout (little-endian):
  // [0..11]  Unknown (12 bytes)
  // [12..15] FileAddress u32
  // [16..19] FileSize u32
  // [20..23] PathNameOffset u32 (unused here)
  // [24..27] FileNameOffset u32 (unused here)
  // [28..31] PictureFormat 4 bytes (ASCII, no NUL)
  // [32..33] SizeX u16
  // [34..35] SizeY u16
  // then: PathName (CString), FileName (CString), then optional 2 unknown bytes
  if (raw.size() < 36)
    throw std::runtime_error("PictFileInfo2: short header");
  const std::uint8_t *p = raw.data();
  FileAddress = read_32le(&p[12]);
  FileSize = read_32le(&p[16]);
  // offsets present but not required because strings are inline after fixed
  // header
  std::memcpy(PictureFormat, &p[28], 4);
  PictureFormat[4] = 0;
  SizeX = read_16le(&p[32]);
  SizeY = read_16le(&p[34]);

  size_t i = 36;
  PathName = read_cstr(raw, i);
  FileName = read_cstr(raw, i);
  // trailing 2 bytes (unknown) may be present; ignore
}

// ---------- BigPartialPictFile ----------
void BigPartialPictFile::decode(const std::vector<std::uint8_t> &raw) {
  if (raw.size() < 4)
    throw std::runtime_error("BigPartialPictFile: short buffer");
  AcquiredSize = read_32le(raw.data());
  size_t have = std::min<std::size_t>(AcquiredSize, raw.size() - 4);
  PartialData.assign(raw.begin() + 4, raw.begin() + 4 + have);
}

// ---------- ViewFrame ----------
void ViewFrame::decode(const std::vector<std::uint8_t> &raw) {
  if (raw.size() < 10)
    throw std::runtime_error("ViewFrame: short buffer");
  Data.assign(raw.begin() + 10, raw.end());
}

void ApiConfig::decode(const std::vector<std::uint8_t> &raw) {
  camera_model_.clear();
  serial_number_.clear();
  firmware_version_.clear();
  communication_version_ = 0.0;

  if (raw.size() < 8)
    return;

  const uint32_t data_len = read_32le(&raw[0]); // not strictly needed
  const uint32_t dir_cnt = read_32le(&raw[4]);
  const size_t index_end = 8 + size_t(dir_cnt) * 12; // 12 bytes per entry
  if (raw.size() < index_end)
    return;

  for (uint32_t i = 0; i < dir_cnt; ++i) {
    const size_t off = 8 + i * 12;
    const uint16_t tag = read_16le(&raw[off + 0]);
    const auto type = static_cast<DirectoryType>(read_16le(&raw[off + 2]));
    const uint32_t cnt = read_32le(&raw[off + 4]);
    const uint8_t *v4 = &raw[off + 8];

    const uint32_t elem = dir_type_size(type);
    const uint32_t nbytes = (type == DirectoryType::String) ? cnt : cnt * elem;

    // load payload
    std::vector<uint8_t> payload;
    if (nbytes <= 4) {
      payload.assign(v4, v4 + nbytes);
    } else {
      const uint32_t ofs = read_32le(v4); // offset from start of blob
      if (ofs + nbytes > raw.size())
        continue;
      payload.assign(raw.begin() + ofs, raw.begin() + ofs + nbytes);
    }

    // map tags we care about
    switch (tag) {
    case 1: { // CameraModel (String)
      std::string s(reinterpret_cast<const char *>(payload.data()),
                    payload.size());
      if (!s.empty() && s.back() == '\0')
        s.pop_back();
      camera_model_ = std::move(s);
      break;
    }
    case 2: { // SerialNumber (String)
      std::string s(reinterpret_cast<const char *>(payload.data()),
                    payload.size());
      if (!s.empty() && s.back() == '\0')
        s.pop_back();
      serial_number_ = std::move(s);
      break;
    }
    case 3: { // FirmwareVersion (String)
      std::string s(reinterpret_cast<const char *>(payload.data()),
                    payload.size());
      if (!s.empty() && s.back() == '\0')
        s.pop_back();
      firmware_version_ = std::move(s);
      break;
    }
    case 5: { // CommunicationVersion (Float32 or Float64, count>=1)
      if (type == DirectoryType::Float32 && payload.size() >= 4) {
        uint32_t u = read_32le(payload.data());
        float f;
        std::memcpy(&f, &u, 4);
        communication_version_ = static_cast<double>(f);
      } else if (type == DirectoryType::Float64 && payload.size() >= 8) {
        uint64_t u =
            (uint64_t)payload[0] | ((uint64_t)payload[1] << 8) |
            ((uint64_t)payload[2] << 16) | ((uint64_t)payload[3] << 24) |
            ((uint64_t)payload[4] << 32) | ((uint64_t)payload[5] << 40) |
            ((uint64_t)payload[6] << 48) | ((uint64_t)payload[7] << 56);
        double d;
        std::memcpy(&d, &u, 8);
        communication_version_ = d;
      }
      break;
    }
    default:
      break; // ignore other tags
    }
  }
}

std::string ApiConfig::to_string() const {
  std::ostringstream oss;
  oss << "ApiConfig(CameraModel=" << camera_model_
      << ", SerialNumber=" << serial_number_
      << ", FirmwareVersion=" << firmware_version_
      << ", CommunicationVersion=" << communication_version_ << ")";
  return oss.str();
}

// int main() {
//     CamDataGroup1 g;

//     // set the first 8 fields (others are omitted by encode())
//     g.shutterSpeed = 0x0A;                 // APEX step
//     g.aperture     = 0x14;                 // APEX step
//     g.programShift = ProgramShift::Plus;
//     g.isoAuto      = ISOAuto::Auto;
//     g.isoSpeed     = 0x24;                 // APEX step
//     g.expComp      = 0x80;                 // APEX step
//     g.abValue      = 0x02;
//     g.abSetting    = ABSetting::AB3ZeroMinusPlus;

//     // encode to bytes
//     std::vector<std::uint8_t> bytes = g.encode();

//     // dump hex
//     std::cout << "Encoded (" << bytes.size() << " bytes): ";
//     for (auto b : bytes) std::cout << std::hex << std::setw(2) <<
//     std::setfill('0') << int(b) << " "; std::cout << std::dec << "\n";

//     // decode back
//     CamDataGroup1 parsed;
//     parsed.decode(bytes);

//     // read decoded values
//     if (parsed.shutterSpeed) std::cout << "ShutterSpeed: " <<
//     int(*parsed.shutterSpeed) << "\n"; if (parsed.aperture)     std::cout <<
//     "Aperture: "     << int(*parsed.aperture)     << "\n"; if
//     (parsed.programShift) std::cout << "ProgramShift: " <<
//     int(static_cast<std::uint8_t>(*parsed.programShift)) << "\n"; if
//     (parsed.isoAuto)      std::cout << "ISOAuto: "      <<
//     int(static_cast<std::uint8_t>(*parsed.isoAuto))      << "\n"; if
//     (parsed.isoSpeed)     std::cout << "ISOSpeed: "     <<
//     int(*parsed.isoSpeed)     << "\n"; if (parsed.expComp)      std::cout <<
//     "ExpComp: "      << int(*parsed.expComp)      << "\n"; if
//     (parsed.abValue)      std::cout << "ABValue: "      <<
//     int(*parsed.abValue)      << "\n"; if (parsed.abSetting)    std::cout <<
//     "ABSetting: "    << int(static_cast<std::uint8_t>(*parsed.abSetting)) <<
//     "\n";
// }
