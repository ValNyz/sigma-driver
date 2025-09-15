#include <fstream>
#include <iostream>
#include <chrono>
#include <thread>

#include "ptp/usb_transport.h"
#include "sigma/apex.h"
#include "sigma/enum.h"
#include "sigma/log.h"
#include "sigma/sigma_ptp.h"

static void save(const std::string &path, const std::vector<uint8_t> &data) {
  std::ofstream f(path, std::ios::binary);
  f.write(reinterpret_cast<const char *>(data.data()),
          (std::streamsize)data.size());
}

int main() {
  log_set_level(LogLevel::Debug);
  try {
    USBTransport usb;
    usb.open_first();

    SigmaCamera cam(usb);
    cam.open_session(1);
    cam.config_api();

    // 1) Set RAW (and optionally JPEG) via Group2
    CamDataGroup2 g2;
    g2.exposureMode = ExposureMode::Manual; // We need to first set Manual Exposure before setting ShutterSpeed
    // Pick ONE of:
    g2.imageQuality = ImageQuality::DNG; // RAW
    // g2.imageQuality = ImageQuality::JPEGNormal; // JPEG
    cam.set_group(g2);

    // 2) Setup a few Group1 fields (safe defaults) ---
    int iso = 1600;
    int encoded_iso = ISOSpeedConverter.encode_uint8(iso);
    float shutterSpeed = 2.0;
    int encoded_shutterSpeed = ShutterSpeed3Converter.encode_uint8(shutterSpeed);
    LOG_INFO(
        "CamDataGroup1: iso value/encoded=%d/%d/0x%01X  shutter speed value/encoded=%f/%d/%01X",
        iso, encoded_iso, encoded_iso, shutterSpeed, encoded_shutterSpeed, encoded_shutterSpeed);
    CamDataGroup1 g1;
    g1.isoAuto = ISOAuto::Manual;
    g1.isoSpeed = encoded_iso;
    g1.shutterSpeed = encoded_shutterSpeed;
    cam.set_group(g1);

    // 3) Set save destination in the camera via Group3
    CamDataGroup3 g3;
    DestToSave mode = DestToSave::InCamera;
    g3.destToSave = mode;
    cam.set_group(g3);

    CamDataGroup1 g1_bis = cam.get_group<CamDataGroup1>();
    LOG_INFO(
        "CamDataGroup1: iso mode=0x%01X/0x%01X iso=%u/%u shutter speed=%u/%u",
        g1_bis.isoAuto, g1.isoAuto, g1_bis.isoSpeed, g1.isoSpeed,
        g1_bis.shutterSpeed, g1.shutterSpeed);

    // 4) Trigger 1 capture without AF
    auto r = cam.snap(CaptureMode::NonAFCapt, 1);

    // 5) Wait to complete shooting
    CamCaptStatus st = cam.wait_completion(0, 50, 200);
    LOG_INFO("CamCaptStatus: id=%u head=%u tail=%u code=0x%04X dest=0x%02X",
             st.ImageId, st.ImageDBHead, st.ImageDBTail, st.Status, st.Dest);

    // 6) Get the picture info
    PictFileInfo2 info = cam.get_pict_file_info2();
    LOG_INFO("PictFileInfo2: %s/%s size=%u addr=0x%08X", info.PathName.c_str(),
             info.FileName.c_str(), info.FileSize, info.FileAddress);

    // Download the whole image (vendor chunk loop inside)
    // If you prefer a fixed chunk size, pass e.g. 128*1024.
    BigPartialPictFile img =
        cam.get_big_partial_pict_file(info.FileAddress, 0, info.FileSize);
    // cam.get_object_vendor(256 * 1024);
    std::vector<uint8_t> bytes = img.PartialData;

    // Filename
    std::string filename =
        info.FileName.empty() ? "capture.jpg" : info.FileName;
    save(filename, bytes);
    LOG_INFO("A picture is saved as %s (%zu bytes).", filename.c_str(),
             bytes.size());

    // Clear CaptStatus DB entry
    cam.clear_image_db_single(st.ImageId);

    cam.close_session();
    usb.close();
    return 0;
  } catch (const std::exception &e) {
    LOG_ERROR("Error: %s", e.what());
    return 1;
  }
}