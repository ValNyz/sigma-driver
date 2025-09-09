#include <iostream>
#include <fstream>
#include <unordered_set>
#include <thread>

#include "apex.h"
#include "enum.h"
#include "log.h"
#include "sigma_ptp.h"
#include "ptp/usb_transport.h"

static void save(const std::string &path, const std::vector<uint8_t> &data)
{
	std::ofstream f(path, std::ios::binary);
	f.write(reinterpret_cast<const char *>(data.data()), (std::streamsize)data.size());
}

int main()
{
	try
	{
		USBTransport usb;
		usb.open_first();

		SigmaCamera cam(usb);
		cam.open_session(1);
		cam.config_api();

		// Snapshot handles BEFORE capture
		auto before = cam.get_object_handles();
		CamCaptStatus st0 = cam.get_cam_capt_status();

		// --- 1) Setup a few Group1 fields (safe defaults) ---
		CamDataGroup1 g1;
		g1.isoAuto = ISOAuto::Manual;
		g1.isoSpeed = ISOSpeedConverter.encode_uint8(1600); // ~ISO 400
		g1.shutterSpeed = ShutterSpeed3Converter.encode_uint8(4);
		cam.set_group(g1);

		// 1) Set RAW (and optionally JPEG) via Group2
		CamDataGroup2 g2;
		// Pick ONE of:
		g2.imageQuality = ImageQuality::DNG; // RAW only
		// g2.imageQuality = ImageQuality::JPEGNormal; // RAW + JPEG
		cam.set_group(g2);

		CamDataGroup3 g3;
		DestToSave mode = DestToSave::InCamera;
		g3.destToSave = mode;
		cam.set_group(g3);

		// Trigger
		cam.snap(CaptureMode::NonAFCapt, 1);

		// Wait to complete shooting (Python passed 0; we keep same)
		// CamCaptStatus st = cam.wait_completion(0, /*polls*/ 50, /*sleep_ms*/ 200);
		// LOG_INFO("CamCaptStatus: id=%u head=%u tail=%u code=0x%04X dest=0x%02X",
		// 		  st.ImageId, st.ImageDBHead, st.ImageDBTail, st.Status, st.Dest);
		// Prefer: find the new handle (event or diff)
		uint32_t handle = 0;
		if (auto evh = cam.wait_object_added(3000, 100))
		{
			handle = *evh;
		}
		else
		{
			// diff fallback
			for (int i = 0; i < 50 && !handle; i++)
			{
				auto after = cam.get_object_handles();
				std::unordered_set<uint32_t> seen(before.begin(), before.end());
				for (auto h : after)
					if (!seen.count(h))
					{
						handle = h;
						break;
					}
				if (!handle)
					std::this_thread::sleep_for(std::chrono::milliseconds(100));
			}
		}
		if (!handle)
		{
			LOG_INFO("No new handle");
			return 2;
		}

		// Download with standard PTP
		std::vector<uint8_t> bytes = cam.get_object(handle);
		if (bytes.empty())
		{
			LOG_INFO("GetObject returned 0 bytes");
			return 3;
		}

		// Save a picture
		PictFileInfo2 info = cam.get_pict_file_info2();
		LOG_INFO("PictFileInfo2: %s/%s size=%u addr=0x%08X",
				  info.PathName.c_str(), info.FileName.c_str(), info.FileSize, info.FileAddress);

		// Download the whole image (vendor chunk loop inside)
		// If you prefer a fixed chunk size, pass e.g. 128*1024.
		// BigPartialPictFile img = cam.get_big_partial_pict_file(info.FileAddress, info.FileSize);
		// cam.get_object_vendor(256 * 1024);
		// std::vector<uint8_t> bytes = img.PartialData;

		// Filename
		std::string filename = info.FileName.empty() ? "capture.jpg" : info.FileName;
		save(filename, bytes);
		LOG_INFO("A picture is saved as %s (%zu bytes).", filename.c_str(), bytes.size());

		// Clear CaptStatus DB entry
		cam.clear_image_db_single(0);

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