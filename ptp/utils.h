#pragma once
#include <vector>
#include <cstdint>
#include <string>
#include <stdexcept>

// ---- helpers ----
// Put 8 bits, 1 byte
static inline void put_8(std::vector<std::uint8_t> &b, std::uint8_t v)
{
    b.push_back(v);
}

// Get 8 bits, 1 byte
static inline std::uint8_t get_8(const std::vector<std::uint8_t> &v, size_t &i)
{
    return v.at(i++);
}

// Put 16 bits, 2 bytes (big endian)
static inline void put_16be(std::vector<std::uint8_t> &b, std::uint16_t v)
{
    b.push_back(v >> 8);
    b.push_back(v);
}

// Put 16 bits, 2 bytes (little endian)
static inline void put_16le(std::vector<std::uint8_t> &b, std::uint16_t v)
{
    b.push_back(v);
    b.push_back(v >> 8);
}

// Get 16 bits, 2 bytes (big endian)
static inline std::uint16_t get_16be(const std::vector<std::uint8_t> &v, size_t &i)
{
    auto x = std::uint16_t((v.at(i) << 8) | v.at(i + 1));
    i += 2;
    return x;
}

// Get 16 bits, 2 bytes (little endian)
static inline std::uint16_t get_16le(const std::vector<std::uint8_t> &v, size_t &i)
{
    auto x = std::uint16_t(v.at(i) | (v.at(i + 1) << 8));
    i += 2;
    return x;
}

// Read 16 bits, 2 bytes (little endian)
static inline std::uint16_t read_16le(const std::uint8_t *p)
{
    return std::uint16_t(p[0] | (p[1] << 8));
}

// Read 32 bits, 4 bytes (little endian)
static inline std::uint32_t read_32le(const std::uint8_t *p)
{
    return std::uint32_t(p[0]) | (std::uint32_t(p[1]) << 8) | (std::uint32_t(p[2]) << 16) | (std::uint32_t(p[3]) << 24);
}

// Appends a 32-bit integer to a byte vector in little-endian order.
static inline void append_u32le(std::vector<uint8_t>& b, uint32_t v){
	b.push_back(uint8_t(v));
	b.push_back(uint8_t(v>>8));
	b.push_back(uint8_t(v>>16));
	b.push_back(uint8_t(v>>24));
}

// Decode 16-bit 12.4 fixed-point
static inline float fp12_4_decode(std::uint16_t x)
{
    return float(x >> 4) + float(x & 0x0F) / 16.0f;
}

// Encode 16-bit 12.4 fixed-point
static inline std::uint16_t fp12_4_encode(float f)
{
    if (f < 0)
        f = 0;
    if (f > 4095.9375f)
        f = 4095.9375f;
    return std::uint16_t(f * 16.0f + 0.5f);
}

// Read C-String
static inline std::string read_cstr(const std::vector<std::uint8_t> &raw, size_t &i)
{
    size_t start = i;
    while (i < raw.size() && raw[i] != 0)
        ++i;
    if (i >= raw.size())
        throw std::runtime_error("CString not terminated");
    std::string s(reinterpret_cast<const char *>(&raw[start]), i - start);
    ++i; // skip NUL
    return s;
}