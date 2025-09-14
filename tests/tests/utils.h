#include <string_view>
#include <vector>
#include <cctype>
#include <cstdint>
#include <stdexcept>

inline static uint8_t hex_nibble(char c)
{
  if (c >= '0' && c <= '9')
    return c - '0';
  c = std::tolower(static_cast<unsigned char>(c));
  if (c >= 'a' && c <= 'f')
    return 10 + (c - 'a');
  throw std::runtime_error("bad hex");
}

inline static std::vector<uint8_t> hex2bin(std::string_view s)
{
  std::vector<uint8_t> out;
  int hi = -1;
  for (char c : s)
  {
    if (std::isspace(static_cast<unsigned char>(c)) || c == '_')
      continue;
    uint8_t v = hex_nibble(c);
    if (hi < 0)
      hi = v;
    else
    {
      out.push_back(uint8_t((hi << 4) | v));
      hi = -1;
    }
  }
  if (hi >= 0)
    throw std::runtime_error("odd hex length");
  return out;
}

inline static void mask_tid(std::vector<uint8_t> &v, uint32_t value = 0)
{
  if (v.size() < 12)
    return;
  v[8] = uint8_t(value);
  v[9] = uint8_t(value >> 8);
  v[10] = uint8_t(value >> 16);
  v[11] = uint8_t(value >> 24);
}