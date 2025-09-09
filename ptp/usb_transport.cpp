#include <libusb-1.0/libusb.h>
#include <stdexcept>
#include <string>

#include "usb_transport.h"

static inline void check(int rc, const char* what){
    if(rc<0) throw std::runtime_error(std::string(what)+": "+libusb_error_name(rc));
}

USBTransport::USBTransport(){ check(libusb_init(&ctx_), "libusb_init"); }
USBTransport::~USBTransport(){ try{ close(); }catch(...){} if(ctx_) libusb_exit(ctx_); }

void USBTransport::find_ptp_interface_(libusb_device* d, int& ifnum, uint8_t& ep_in, uint8_t& ep_out, uint8_t& ep_intr){
    libusb_config_descriptor* cfg{};
    check(libusb_get_active_config_descriptor(d,&cfg),"get_active_config");
    for(uint8_t i=0;i<cfg->bNumInterfaces;++i){
        const auto& intf=cfg->interface[i];
        for(int a=0;a<intf.num_altsetting;++a){
            const auto& alt=intf.altsetting[a];
            if(alt.bInterfaceClass==6){
                uint8_t in=0,out=0,intr=0;
                for(uint8_t e=0;e<alt.bNumEndpoints;++e){
                    const auto& ep=alt.endpoint[e];
                    auto type=ep.bmAttributes & LIBUSB_TRANSFER_TYPE_MASK;
                    if(type==LIBUSB_TRANSFER_TYPE_BULK){
                        if(ep.bEndpointAddress & LIBUSB_ENDPOINT_IN)
                            in=ep.bEndpointAddress;
                        else
                            out=ep.bEndpointAddress;
                    }
                    else if(type==LIBUSB_TRANSFER_TYPE_INTERRUPT && (ep.bEndpointAddress & LIBUSB_ENDPOINT_IN))
                        intr=ep.bEndpointAddress;
                }
                if(in && out){
                    ifnum=alt.bInterfaceNumber;
                    ep_in=in;
                    ep_out=out;
                    ep_intr=intr;
                    libusb_free_config_descriptor(cfg);
                    return;
                }
            }
        }
    }
    libusb_free_config_descriptor(cfg);
    throw std::runtime_error("No PTP bulk endpoints");
}

void USBTransport::open_device_(libusb_device* d){
    find_ptp_interface_(d, ifnum_, ep_in_, ep_out_, ep_intr_);
    check(libusb_open(d,&dev_),"libusb_open");
    if(libusb_kernel_driver_active(dev_, ifnum_)==1)
        libusb_detach_kernel_driver(dev_, ifnum_);
    check(libusb_claim_interface(dev_, ifnum_), "claim_interface");
}

void USBTransport::open_first(){
    if(dev_)
        close();
    libusb_device **list{};
    ssize_t n=libusb_get_device_list(ctx_, &list);
    if(n<0)
        throw std::runtime_error("get_device_list failed");
    for(ssize_t i=0;i<n;++i){
        try{ open_device_(list[i]);
            libusb_free_device_list(list,1);
            return;
        }catch(...){}
    }
    libusb_free_device_list(list,1);
    throw std::runtime_error("No PTP device found");
}

void USBTransport::open_vid_pid(uint16_t vid, uint16_t pid){
    if(dev_)
        close();
    libusb_device **list{};
    ssize_t n=libusb_get_device_list(ctx_, &list);
    if(n<0)
        throw std::runtime_error("get_device_list failed");
    for(ssize_t i=0;i<n;++i){
        libusb_device_descriptor dd{};
        libusb_get_device_descriptor(list[i], &dd);
        if(dd.idVendor==vid && dd.idProduct==pid){
            open_device_(list[i]);
            libusb_free_device_list(list,1);
            return;
        }
    }
    libusb_free_device_list(list,1);
    throw std::runtime_error("Requested VID/PID not found");
}

void USBTransport::close(){
    if(!dev_)
    return;
    libusb_release_interface(dev_, ifnum_);
    libusb_close(dev_);
    dev_=nullptr; ifnum_=-1;
    ep_in_=ep_out_=ep_intr_=0;
}

void USBTransport::write_exact(const void* data, int len, unsigned to){
    int x=0;
    check(libusb_bulk_transfer(dev_, ep_out_, (unsigned char*)data, len, &x, to), "bulk_out");
    if(x!=len)
        throw std::runtime_error("short bulk write");
}

int USBTransport::read_some(void* buf, int max, unsigned to){
    int x=0;
    check(libusb_bulk_transfer(dev_, ep_in_, (uint8_t*) buf, max, &x, to), "bulk_in");
    return x;
}

int USBTransport::read_intr(void* buf, int max, unsigned to){
    if(!ep_intr_)
        return 0;
    int x=0;
    int rc=libusb_interrupt_transfer(dev_, ep_intr_, (uint8_t*)  buf, max, &x, to);
    if(rc==LIBUSB_ERROR_TIMEOUT)
        return 0;
    check(rc, "intr_in");
    return x;
}