#pragma once
#include "transport.h"

// forward-declare libusb types
struct libusb_context;
struct libusb_device_handle;
struct libusb_device;

class USBTransport : public Transport {
    public:
        USBTransport();
        ~USBTransport() override;

        void open_first() override;
        void open_vid_pid(uint16_t vid, uint16_t pid) override;
        bool is_open() const override { return dev_ != nullptr; }
        void close() override;

        void write_exact(const void* data, int len, unsigned timeout_ms) override;
        int  read_some (void* buf, int max, unsigned timeout_ms) override;
        int  read_intr (void* buf, int max, unsigned timeout_ms) override;

    private:
        void open_device_(libusb_device* dev);
        void find_ptp_interface_(libusb_device* dev, int& ifnum, uint8_t& ep_in, uint8_t& ep_out, uint8_t& ep_intr);

        libusb_context* ctx_{nullptr};
        libusb_device_handle* dev_{nullptr};
        int ifnum_{-1};
        uint8_t ep_in_{0}, ep_out_{0}, ep_intr_{0};
};
