#pragma once
#include <cstdint>

class Transport {
public:
    virtual ~Transport() = default;

    virtual void open_first() = 0;
    virtual void open_vid_pid(std::uint16_t vid, std::uint16_t pid) = 0;
    virtual bool is_open() const = 0;
    virtual void close() = 0;

    // raw pipes
    virtual void write_exact(const void* data, int len, unsigned timeout_ms=3000) = 0;
    virtual int  read_some  (void* data, int max, unsigned timeout_ms=3000) = 0;
    virtual int  read_intr  (void* data, int max, unsigned timeout_ms=50) = 0;

    // convenience names (match PTPy-ish surface)
    void send(const void* p, int n, unsigned to=3000){ write_exact(p,n,to); }
    int  recv(void* p, int max, unsigned to=3000){ return read_some(p,max,to); }
    int  event(void* p, int max, unsigned to=50){ return read_intr(p,max,to); }
};