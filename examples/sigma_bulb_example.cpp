#include <fstream>
#include <iostream>
#include <unistd.h>

#include "ptp/usb_transport.h"
#include "sigma/enum.h"
#include "sigma/sigma_ptp.h"
#include "utils/apex.h"
#include "utils/log.h"

static void save(const std::string &path, const std::vector<uint8_t> &data)
{
  std::ofstream f(path, std::ios::binary);
  f.write(reinterpret_cast<const char *>(data.data()),
          (std::streamsize)data.size());
}

int main()
{
  log_set_level(LogLevel::Debug);
  try
  {
    // FakeTransport usb;
    USBTransport usb;
    usb.open_first();

    SigmaCamera cam(usb);
    cam.open_session(1);
    cam.config_api();

    // 1) Set RAW (and optionally JPEG) via Group2
    CamDataGroup2 g2;
    g2.exposureMode =
        ExposureMode::Manual; // We need to first set Manual Exposure before
                              // setting ShutterSpeed
    g2.driveMode = DriveMode::SingleCapture;
    g2.imageQuality = ImageQuality::DNG; // RAW
    cam.set_group(g2);

    // 2) Setup a few Group1 fields (safe defaults) ---
    int iso = 1600;
    uint8_t encoded_iso = ISOSpeedConverter.encode_uint8(iso);
    double shutterSpeed = 0;
    uint8_t encoded_shutterSpeed = ShutterSpeed3Converter.encode_uint8(shutterSpeed);
    CamDataGroup1 g1;
    g1.isoAuto = ISOAuto::Manual;
    g1.isoSpeed = encoded_iso;
    g1.shutterSpeed = encoded_shutterSpeed;
    cam.set_group(g1);

    CamDataGroup1 g1_bis = cam.get_group<CamDataGroup1>();
    LOG_INFO("CamDataGroup1: iso mode=0x%01X, iso value:0x%01X, shutterSpeed value=0x%01X",
             g1_bis.isoAuto.value(), g1_bis.isoSpeed.value(), g1_bis.shutterSpeed.value());

    CamDataGroupFocus gf;
    gf.focusMode = FocusMode::MF;
    gf.preConstAF = PreConstAF::Off;
    cam.set_cam_data_group_focus(gf);
    cam.get_cam_data_group_focus();

    // 4) Trigger 1 capture without AF
    auto r = cam.snap(CaptureMode::NonAFCapt, 1);
    LOG_INFO("Start BULB capture: 0x%02X", r);
    for (int i = 0; i < 300; i++)
    {
      usleep(1000000); // sleep for 1 sec
      LOG_INFO("Capturing: %ds", i);
    }
    r = cam.snap(CaptureMode::StopCapt, 1);
    LOG_INFO("Stop BULB capture: 0x%02X", r);
    CamCaptStatus st = cam.get_cam_capt_status();
    LOG_INFO("CamCaptStatus: id=%u head=%u tail=%u code=0x%04X dest=0x%02X",
             st.ImageId, st.ImageDBHead, st.ImageDBTail, st.Status, st.Dest);

    // 5) Wait to complete shooting
    st = cam.wait_completion(st.ImageId, 50, 200);
    LOG_INFO("CamCaptStatus: id=%u head=%u tail=%u code=0x%04X dest=0x%02X",
             st.ImageId, st.ImageDBHead, st.ImageDBTail, st.Status, st.Dest);

    // 6) Get the picture info
    PictFileInfo2 info = cam.get_pict_file_info2();
    LOG_INFO("PictFileInfo2: %s/%s size=%u bytes addr=0x%08X",
             info.PathName.c_str(), info.FileName.c_str(), info.FileSize,
             info.FileAddress);

    // Download the whole image (vendor chunk loop inside)
    // If you prefer a fixed chunk size, pass e.g. 128*1024.
    BigPartialPictFile img =
        cam.get_big_partial_pict_file(info.FileAddress, 0, info.FileSize);
    LOG_INFO("BigPartialPictFile: size=%u bytes dataSize=%u bytes",
             img.AcquiredSize, img.PartialData.size());
    // cam.get_object_vendor(256 * 1024);
    std::vector<uint8_t> bytes = img.PartialData;

    // Filename
    std::string filename = info.FileName;
    save(filename, bytes);
    LOG_INFO("A picture is saved as %s (%zu bytes).", filename.c_str(),
             bytes.size());

    // Clear CaptStatus DB entry
    cam.clear_image_db_single(st.ImageId);

    cam.close_application();
    cam.close_session();
    usb.close();
    return 0;
  }
  catch (const std::exception &e)
  {
    LOG_ERROR("Error: %s", e.what());
    return 1;
  }
}
