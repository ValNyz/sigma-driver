#pragma once
#include <cstdint>
#include <stdexcept>
#include <string>
#include <vector>

// ---- helpers ----
// Put 8 bits, 1 byte
static inline void put_8(std::vector<std::uint8_t> &b, std::uint8_t v) {
  b.push_back(v);
}

// Get 8 bits, 1 byte
static inline std::uint8_t get_8(const std::vector<std::uint8_t> &v,
                                 size_t &i) {
  return v.at(i++);
}

// Put 16 bits, 2 bytes (big endian)
static inline void put_16be(std::vector<std::uint8_t> &b, std::uint16_t v) {
  b.push_back(v >> 8);
  b.push_back(v);
}

// Put 16 bits, 2 bytes (little endian)
static inline void put_16le(std::vector<std::uint8_t> &b, std::uint16_t v) {
  b.push_back(v);
  b.push_back(v >> 8);
}

// Get 16 bits, 2 bytes (big endian)
static inline std::uint16_t get_16be(const std::vector<std::uint8_t> &v,
                                     size_t &i) {
  auto x = std::uint16_t((v.at(i) << 8) | v.at(i + 1));
  i += 2;
  return x;
}

// Get 16 bits, 2 bytes (little endian)
static inline std::uint16_t get_16le(const std::vector<std::uint8_t> &v,
                                     size_t &i) {
  auto x = std::uint16_t(v.at(i) | (v.at(i + 1) << 8));
  i += 2;
  return x;
}

// Read 16 bits, 2 bytes (little endian)
static inline std::uint16_t read_16le(const std::uint8_t *p) {
  return std::uint16_t(p[0] | (p[1] << 8));
}

// Put 32 bits, 8 bytes (little endian)
static inline void put_32le(std::vector<std::uint8_t> &b, std::uint32_t v) {
  b.push_back(v);
  b.push_back(v >> 8);
  b.push_back(v >> 16);
  b.push_back(v >> 24);
}

// Read 32 bits, 4 bytes (little endian)
static inline std::uint32_t read_32le(const std::uint8_t *p) {
  return std::uint32_t(p[0]) | (std::uint32_t(p[1]) << 8) |
         (std::uint32_t(p[2]) << 16) | (std::uint32_t(p[3]) << 24);
}

// Appends a 32-bit integer to a byte vector in little-endian order.
static inline void append_u32le(std::vector<uint8_t> &b, uint32_t v) {
  b.push_back(uint8_t(v));
  b.push_back(uint8_t(v >> 8));
  b.push_back(uint8_t(v >> 16));
  b.push_back(uint8_t(v >> 24));
}

// Decode 16-bit 12.4 fixed-point
static inline float fp12_4_decode(std::uint16_t x) {
  return float(x >> 4) + float(x & 0x0F) / 16.0f;
}

// Encode 16-bit 12.4 fixed-point
static inline std::uint16_t fp12_4_encode(float f) {
  if (f < 0)
    f = 0;
  if (f > 4095.9375f)
    f = 4095.9375f;
  return std::uint16_t(f * 16.0f + 0.5f);
}

// Read C-String
static inline std::string read_cstr(const std::vector<std::uint8_t> &raw,
                                    size_t &i) {
  size_t start = i;
  while (i < raw.size() && raw[i] != 0)
    ++i;
  if (i >= raw.size())
    throw std::runtime_error("CString not terminated");
  std::string s(reinterpret_cast<const char *>(&raw[start]), i - start);
  ++i; // skip NUL
  return s;
}

// ----------------------------------------------------------------------------
//                            Tests utility
// ----------------------------------------------------------------------------

inline static uint8_t hex_nibble(char c) {
  if (c >= '0' && c <= '9')
    return c - '0';
  c = std::tolower(static_cast<unsigned char>(c));
  if (c >= 'a' && c <= 'f')
    return 10 + (c - 'a');
  throw std::runtime_error("bad hex");
}

inline static std::vector<uint8_t> hex2bin(std::string_view s) {
  std::vector<uint8_t> out;
  int hi = -1;
  for (char c : s) {
    if (std::isspace(static_cast<unsigned char>(c)) || c == '_')
      continue;
    uint8_t v = hex_nibble(c);
    if (hi < 0)
      hi = v;
    else {
      out.push_back(uint8_t((hi << 4) | v));
      hi = -1;
    }
  }
  if (hi >= 0)
    throw std::runtime_error("odd hex length");
  return out;
}

inline static void mask_tid(std::vector<uint8_t> &v, uint32_t value = 0) {
  if (v.size() < 12)
    return;
  v[8] = uint8_t(value);
  v[9] = uint8_t(value >> 8);
  v[10] = uint8_t(value >> 16);
  v[11] = uint8_t(value >> 24);
}
