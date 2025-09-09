#pragma once
#include <cstdint>
#include <vector>
#include <optional>
#include <string>

#include "enum.h"

class CamDataGroup1
{
    public:
        // same fields, enum-typed where applicable
        std::optional<std::uint8_t> shutterSpeed;
        std::optional<std::uint8_t> aperture;
        std::optional<ProgramShift> programShift;
        std::optional<ISOAuto> isoAuto;
        std::optional<std::uint8_t> isoSpeed;
        std::optional<std::uint8_t> expComp;
        std::optional<std::uint8_t> abValue;
        std::optional<ABSetting> abSetting;

        // decoded-only (i.e. Read-only)
        std::optional<std::uint8_t> frameBufferState;
        std::optional<std::uint16_t> mediaFreeSpace; // LE
        std::optional<std::uint8_t> mediaStatus;
        std::optional<float> currentLensFocalLength; // u12.4
        std::optional<std::uint8_t> batteryState;
        std::optional<std::uint8_t> abShotRemainNumber;
        std::optional<std::uint8_t> expCompExcludeAB;

        std::vector<std::uint8_t> encode() const;
        void decode(const std::vector<std::uint8_t> &rawdata);

    private:
        enum : std::uint16_t
        { // FieldPresent bits (big-endian Int16)
            FP_ABSetting = 0x8000,
            FP_ABValue = 0x4000,
            FP_ExpComp = 0x2000,
            FP_ISOSpeed = 0x1000,
            FP_ISOAuto = 0x0800,
            FP_ProgramShift = 0x0400,
            FP_Aperture = 0x0200,
            FP_ShutterSpeed = 0x0100,
            FP__Reserved0 = 0x0080,
            FP_ExpCompExcludeAB = 0x0040,
            FP_ABShotRemainNumber = 0x0020,
            FP_BatteryState = 0x0010,
            FP_CurrentLensFocalLength = 0x0008,
            FP_MediaStatus = 0x0004,
            FP_MediaFreeSpace = 0x0002,
            FP_FrameBufferState = 0x0001,
        };
};

class CamDataGroup2
{
    public:
        std::optional<DriveMode> driveMode;
        std::optional<SpecialMode> specialMode;
        std::optional<ExposureMode> exposureMode;
        std::optional<AEMeteringMode> aeMeteringMode;
        std::optional<FlashType> flashType;
        std::optional<FlashMode> flashMode;
        std::optional<FlashSetting> flashSetting;
        std::optional<WhiteBalance> whiteBalance;
        std::optional<Resolution> resolution;
        std::optional<ImageQuality> imageQuality;

        std::vector<std::uint8_t> encode() const;
        void decode(const std::vector<std::uint8_t> &raw);

    private:
        enum : std::uint16_t
        {
            FP__Reserved3 = 0x8000,
            FP__Reserved2 = 0x4000,
            FP__Reserved1 = 0x2000,
            FP__Reserved0 = 0x1000,
            FP_AE = 0x0800,
            FP_Exp = 0x0400,
            FP_Spec = 0x0200,
            FP_Drive = 0x0100,
            FP_IQ = 0x0080,
            FP_Res = 0x0040,
            FP_WB = 0x0020,
            FP__Reserved5 = 0x0010,
            FP_FlashS = 0x0008,
            FP_FlashM = 0x0004,
            FP__Reserved4 = 0x0002,
            FP_FlashT = 0x0001,
        };
};

class CamDataGroup3
{
public:
    std::optional<ColorSpace> colorSpace;
    std::optional<ColorMode> colorMode;
    std::optional<BatteryKind> batteryKind;
    std::optional<float> lensWideFocalLength; // u12.4
    std::optional<float> lensTeleFocalLength; // u12.4
    std::optional<AFAuxLight> afAuxLight;
    std::optional<std::uint8_t> afBeep;
    std::optional<std::uint8_t> timerSound;
    std::optional<DestToSave> destToSave;

    std::vector<std::uint8_t> encode() const;
    void decode(const std::vector<std::uint8_t> &raw);

private:
    enum : std::uint16_t
    {
        FP_Tele = 0x8000,
        FP_Wide = 0x4000,
        FP_Batt = 0x2000,
        FP_CMode = 0x1000,
        FP_CSpace = 0x0800,
        FP__Reserved2 = 0x0400,
        FP__Reserved1 = 0x0200,
        FP__Reserved0 = 0x0100,
        FP_Save = 0x0080,
        FP__Reserved6 = 0x0040,
        FP_Timer = 0x0020,
        FP__Reserved5 = 0x0010,
        FP__Reserved4 = 0x0008,
        FP__Reserved3 = 0x0004,
        FP_Beep = 0x0002,
        FP_AFL = 0x0001,
    };
};

class CamDataGroup4
{
public:
    std::optional<DCCropMode> dcCropMode;
    std::optional<LVMagnifyRatio> lvMagnifyRatio;
    std::optional<HighISOExt> highISOExt;
    std::optional<ContShootSpeed> contShootSpeed;
    std::optional<HDR> hdr;
    std::optional<DNGQuality> dngQuality;
    std::optional<std::int8_t> fillLight;
    std::optional<LOCDistortion> locDistortion;
    std::optional<LOCChromaticAberration> locChromaticAberration;
    std::optional<LOCDiffraction> locDiffraction;
    std::optional<LOCVignetting> locVignetting;
    std::optional<LOCColorShade> locColorShade;
    std::optional<LOCColorShadeAcq> locColorShadeAcq;
    std::optional<EImageStab> eImageStab;
    std::optional<std::uint8_t> shutterSound;

    std::vector<std::uint8_t> encode() const;
    void decode(const std::vector<std::uint8_t> &raw);

private:
    enum : std::uint16_t
    {
        FP_Cont = 0x8000,
        FP_HiISO = 0x4000,
        FP_LVM = 0x2000,
        FP_Crop = 0x1000,
        FP__Reserved3 = 0x0800,
        FP__Reserved2 = 0x0400,
        FP__Reserved1 = 0x0200,
        FP__Reserved0 = 0x0100,
        FP__Reserved5 = 0x0080,
        FP__Reserved6 = 0x0040,
        FP_Snd = 0x0020,
        FP_Stab = 0x0010,
        FP_LOC = 0x0008,
        FP_Fill = 0x0004,
        FP_DNG = 0x0002,
        FP_HDR = 0x0001,
    };
};

class CamDataGroup5
{
    public:
        std::optional<std::uint16_t> intervalTimerSecond;
        std::optional<std::uint8_t> intervalTimerFrame;
        std::optional<std::uint16_t> intervalTimerSecondRemain;
        std::optional<std::uint8_t> intervalTimerFrameRemain;
        std::optional<std::uint16_t> colorTemp;
        std::optional<AspectRatio> aspectRatio;
        std::optional<ToneEffect> toneEffect;
        std::optional<AFAuxLightEF> afAuxLightEF;

        std::vector<std::uint8_t> encode() const;
        void decode(const std::vector<std::uint8_t> &raw);

    private:
        enum : std::uint16_t
        {
            FP__Reserved3 = 0x8000,
            FP__Reserved2 = 0x4000,
            FP_Tone = 0x2000,
            FP__Reserved1 = 0x1000,
            FP_AR = 0x0800,
            FP__Reserved0 = 0x0400,
            FP_CT = 0x0200,
            FP_Int = 0x0100,
            FP_AF = 0x0080,
            FP__Reserved10 = 0x0040,
            FP__Reserved9 = 0x0020,
            FP__Reserved8 = 0x0010,
            FP__Reserved7 = 0x0008,
            FP__Reserved6 = 0x0004,
            FP__Reserved5 = 0x0002,
            FP__Reserved4 = 0x0001,
        };
};

// ---------- CamCaptStatus (decode-only) ----------
class CamCaptStatus
{
    public:
        std::uint8_t ImageId{0};
        std::uint8_t ImageDBHead{0};
        std::uint8_t ImageDBTail{0};
        CaptStatus Status{CaptStatus::Cleared}; // Int16 LE
        DestToSave Dest{DestToSave::Null};   // Int8/

        void decode(const std::vector<std::uint8_t> &raw);
};

// ---------- SnapCommand (encode only) ----------
class SnapCommand
{
    public:
        CaptureMode Mode{CaptureMode::GeneralCapt};
        std::uint8_t Amount{1};

        // returns: [_Header=0x00][Mode u8][Amount u8][_Parity=0x00]
        std::vector<std::uint8_t> encode() const;
};

// ---------- PictFileInfo2 (decode-only) ----------
class PictFileInfo2
{
    public:
        std::uint32_t FileAddress{0};
        std::uint32_t FileSize{0};
        char PictureFormat[5]{}; // 4 chars + NUL
        std::uint16_t SizeX{0};
        std::uint16_t SizeY{0};
        std::string PathName; // C-string
        std::string FileName; // C-string

        void decode(const std::vector<std::uint8_t> &raw);
};

// ---------- BigPartialPictFile (decode-only) ----------
class BigPartialPictFile
{
    public:
        std::uint32_t AcquiredSize{0};
        std::vector<std::uint8_t> PartialData;

        void decode(const std::vector<std::uint8_t> &raw);
};

// ---------- ViewFrame (decode-only) ----------
class ViewFrame
{
    public:
        std::vector<std::uint8_t> Data; // JPEG bytes

        void decode(const std::vector<std::uint8_t> &raw);
};

class ApiConfig {
    public:
        ApiConfig() = default;

        void decode(const std::vector<std::uint8_t>& raw);
        std::string to_string() const;

        const std::string& camera_model() const { return camera_model_; }
        const std::string& serial_number() const { return serial_number_; }
        const std::string& firmware_version() const { return firmware_version_; }
        double communication_version() const { return communication_version_; }

    private:
        std::string camera_model_;
        std::string serial_number_;
        std::string firmware_version_;
        double      communication_version_ = 0.0;
};