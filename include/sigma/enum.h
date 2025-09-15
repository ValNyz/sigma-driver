#pragma once
#include <cstdint>
#include <stdexcept>

enum class DirectoryType : std::uint8_t
{
  UInt8 = 0x01,
  String = 0x02,
  UInt16 = 0x03,
  UInt32 = 0x04,
  URational = 0x05,
  Int8 = 0x06,
  Any8 = 0x07,
  Int16 = 0x08,
  Int32 = 0x09,
  Rational = 0x0A,
  Float32 = 0x0B,
  Float64 = 0x0C,
};

inline std::uint32_t dir_type_size(DirectoryType t)
{
  switch (t)
  {
  case DirectoryType::UInt8:
    return 1;
  case DirectoryType::Any8:
    return 1;
  case DirectoryType::Int8:
    return 1;
  case DirectoryType::UInt16:
    return 2;
  case DirectoryType::Int16:
    return 2;
  case DirectoryType::UInt32:
    return 4;
  case DirectoryType::Int32:
    return 4;
  case DirectoryType::Float32:
    return 4;
  case DirectoryType::Float64:
    return 8;
  case DirectoryType::URational:
    return 8; // 2x u32
  case DirectoryType::Rational:
    return 8; // 2x i32
  case DirectoryType::String:
    return 1; // variable, null-terminated
  }
  throw std::runtime_error("dir_type_size: unknown DirectoryType");
}

enum class ProgramShift : std::uint8_t
{
  Null = 0,
  Plus = 0x01,
  Minus = 0xFF,
};

enum class ISOAuto : std::uint8_t
{
  Manual = 0,
  Auto = 1,
};

enum class ABSetting : std::uint8_t
{
  Null = 0x00,
  AB3ZeroMinusPlus = 0x01,
  AB3MinusZeroPlus = 0x02,
  AB3PlusZeroMinus = 0x03,
  AB5ZeroMinusPlus = 0x04,
  AB5MinusZeroPlus = 0x05,
  AB5PlusZeroMinus = 0x06,
};

enum class DriveMode : std::uint8_t
{
  Null = 0,
  SingleCapture = 1,
  ContinuousCapture = 2,
  TwoSecondsSelfTimer = 3,
  TenSecondsSelfTimer = 4,
  IntervalTimer = 7,
};

enum class SpecialMode : std::uint8_t
{
  Null = 0x00,
  LiveView = 0x02,
};

enum class ExposureMode : std::uint8_t
{
  Null = 0,
  ProgramAuto = 1,
  AperturePriority = 2,
  ShutterPriority = 3,
  Manual = 4,
  C1 = 0x10,
  C2 = 0x20,
  C3 = 0x40,
  Star = 0x80,
};

enum class AEMeteringMode : std::uint8_t
{
  Null = 0,
  Evaluative = 1,
  CenterWeightedAverage = 2,
  CenterArea = 3,
  Spot = 4,
};

enum class FlashType : std::uint8_t
{
  Null = 0,
  InternalPopupFlash = 1,
  ExternalFlash = 2,
};

enum class FlashMode : std::uint8_t
{
  Normal = 0,
  RedEyeReduction = 0x01,
  FPEmission = 0x02,
  MultiFlash = 0x04,
  WirelessFlash1 = 0x08,
  WirelessFlash2 = 0x10,
  WirelessFlash3 = 0x20,
  SlowSync = 0x40,
};

enum class FlashSetting : std::uint8_t
{
  Null = 0,
  TTLAuto = 0x1,
  TTLManual = 0x2,
  EmissionDisabled = 0x80,
  ExposureWarning = 0x81,
};

enum class WhiteBalance : std::uint8_t
{
  Null = 0x0,
  Auto = 0x1,
  Sunlight = 0x2,
  Shade = 0x3,
  Overcast = 0x4,
  Incandescent = 0x5,
  Fluorescent = 0x6,
  Flash = 0x7,
  Custom1 = 0x8,
  CustomCapt1 = 0x9,
  Custom2 = 0xA,
  CustomCapt2 = 0xB,
  Custom3 = 0xC,
  CustomCapt3 = 0xD,
  ColorTemp = 0x0E,
  LightSource = 0x0F,
};

enum class Resolution : std::uint8_t
{
  Null = 0x0,
  High = 0x1,
  Medium = 0x2,
  Low = 0x4,
};

enum class ImageQuality : std::uint8_t
{
  JPEGFine = 0x2,
  JPEGNormal = 0x4,
  JPEGBasic = 0x8,
  DNG = 0x10,
  DNGAndJPEG = 0x12,
};

enum class ColorSpace : std::uint8_t
{
  Null = 0x00,
  sRGB = 0x01,
  AdobeRGB = 0x02,
};

enum class ColorMode : std::uint8_t
{
  Normal = 0x00,
  Sepia = 0x01,
  Monochrome = 0x02,
  Standard = 0x03,
  Vivid = 0x04,
  Neutral = 0x05,
  Portrait = 0x06,
  Landscape = 0x07,
  FovClassicBlue = 0x08,
  Sunset = 0x09,
  Forest = 0x0A,
  Cinema = 0x0B,
  FovClassicYellow = 0x0C,
};

enum class BatteryKind : std::uint8_t
{
  Null = 0x00,
  BodyBattery = 0x01,
  ACAdapter = 0x02,
};

enum class AFAuxLight : std::uint8_t
{
  Null = 0x00,
  On = 0x01,
  Off = 0x02,
};

enum class CaptureMode : std::uint8_t
{
  Null = 0x00,
  GeneralCapt = 0x01,
  NonAFCapt = 0x02,
  AFDriveOnly = 0x03,
  StartAF = 0x04,
  StopAF = 0x05,
  StartCap = 0x06,
  StopCapt = 0x07,
  StartRecMovieAF = 0x10,
  StartRecMovie = 0x20,
  StopRecMovie = 0x30,
};

enum class CaptStatus : std::uint16_t
{
  Null = 0xFFFF,
  Cleared = 0x0000,
  ShootInProgress = 0x0001,
  ShootSuccess = 0x0002,
  ImageGenInProgress = 0x0004,
  ImageGenCompleted = 0x0005,
  StopMovieRec = 0x0006,
  MovieGenCompleted = 0x0007,
  AFSuccess = 0x8001,
  CWBSuccess = 0x8002,
  ImageDataStorageCompleted = 0x8003,
  Interrupted = 0x8004,
  AFFailed = 0x6001,
  BufferFull = 0x6002,
  CWBFailed = 0x6003,
  ImageGenFailed = 0x6004,
  Failed = 0x6005,
};

enum class DestToSave : std::uint8_t
{
  Null = 0x00,
  InCamera = 0x01,
  InComputer = 0x02,
  Both = 0x03,
};

enum class DCCropMode : std::uint8_t
{
  Auto = 0x00,
  Off = 0x01,
  On = 0x02,
};

enum class LVMagnifyRatio : std::uint8_t
{
  Null = 0x00,
  x1 = 0x01,
  x4 = 0x02,
  x8 = 0x03,
};

enum class HighISOExt : std::uint8_t
{
  Auto = 0x00,
  Off = 0x01,
  On = 0x02,
};

enum class ContShootSpeed : std::uint8_t
{
  Auto = 0x00,
  High = 0x01,
  Medium = 0x02,
  Low = 0x03,
};

enum class HDR : std::uint8_t
{
  Null = 0x00,
  Off = 0xFF,
  Auto = 0xFE,
  PlusMinus1 = 0x01,
  PlusMinus2 = 0x02,
  PlusMinus3 = 0x03,
};

enum class DNGQuality : std::uint8_t
{
  Q12bit = 12,
  Q14bit = 14,
};

enum class LOCDistortion : std::uint8_t
{
  Null = 0x00,
  Auto = 0x01,
  Off = 0x02,
};

enum class LOCChromaticAberration : std::uint8_t
{
  Null = 0x00,
  Auto = 0x01,
  Off = 0x02,
};

enum class LOCDiffraction : std::uint8_t
{
  Null = 0x00,
  On = 0x01,
  Off = 0x02,
};

enum class LOCVignetting : std::uint8_t
{
  Null = 0x00,
  Auto = 0x01,
  Off = 0x02,
};

enum class LOCColorShade : std::uint8_t
{
  Null = 0x00,
  Auto = 0xFF,
  Off = 0xFE,
  No1 = 0x01,
  No2 = 0x02,
  No3 = 0x03,
  No4 = 0x04,
  No5 = 0x05,
  No6 = 0x06,
  No7 = 0x07,
  No8 = 0x08,
  No9 = 0x09,
  No10 = 0x0A,
};

enum class LOCColorShadeAcq : std::uint8_t
{
  Null = 0x00,
  On = 0x01,
  Off = 0x02,
};

enum class EImageStab : std::uint8_t
{
  Null = 0x00,
  On = 0x01,
  Off = 0x02,
};

enum class AspectRatio : std::uint8_t
{
  Null = 0x00,
  W21H9 = 0x01,
  W16H9 = 0x02,
  W3H2 = 0x03,
  W4H3 = 0x04,
  W7H6 = 0x05,
  W1H1 = 0x06,
  WSQRT2H1 = 0x07,
};

enum class ToneEffect : std::uint8_t
{
  Null = 0x00,
  BAndW = 0x01,
};

enum class AFAuxLightEF : std::uint8_t
{
  Null = 0x00,
  On = 0x01,
  Off = 0x02,
};

enum class FocusMode : std::uint8_t
{
  MF = 1,
  AF = 2,
  AF_S = 3,
  AF_C = 4,
};

enum class AFLock : std::uint8_t
{
  Off = 0,
  On = 1,
};

enum class FaceEyeAF : std::uint8_t
{
  Off = 0,
  FaceOnly = 1,
  FaceEyeAuto = 2,
};

enum class FaceEyeAFStatus : std::uint8_t
{
  NonDetection = 0,
  Detection = 1,
};

enum class FocusArea : std::uint8_t
{
  MultiAutoFocusPoints = 1,
  OnePointSelection = 2,
  Tracking = 3,
};

enum class OnePointSelection : std::uint8_t
{
  Free = 0,
  X49 = 49,
};

enum class PreConstAF : std::uint8_t
{
  Off = 0,
  On = 1,
};

enum class FocusLimit : std::uint8_t
{
  Off = 0,
  On = 1,
};