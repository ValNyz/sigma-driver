#pragma once
#include <cstdint>
#include <vector>
#include <utility>
#include <optional>

class ApexConverter {
    public:
        explicit ApexConverter(std::vector<std::pair<std::uint8_t, double>> table);

        // Decode an 8-bit code to its APEX value.
        // Returns std::nullopt if the code is not an exact match.
        std::optional<double> decode_uint8(std::uint8_t code) const;

        // Encode an APEX value to the nearest 8-bit code.
        std::uint8_t encode_uint8(double val) const;

    private:
        std::vector<std::pair<std::uint8_t, double>> dectable_; // as given
        std::vector<std::pair<std::uint8_t, double>> enctable_; // sorted by value
};

// Converters
extern const ApexConverter ISOSpeedConverter;
extern const ApexConverter ExpComp2Converter;
extern const ApexConverter ExpComp3Converter;
extern const ApexConverter ShutterSpeed2Converter;
extern const ApexConverter ShutterSpeed3Converter;
extern const ApexConverter Aperture2Converter;
extern const ApexConverter Aperture3Converter;
