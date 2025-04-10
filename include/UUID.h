#pragma once

#include <array>
#include <string>
#include <sstream>
#include <iomanip>
#include <random>
#include <functional>

struct UUID {
    std::array<uint64_t, 2> value; // 128-bit UUID

    UUID() : value{0, 0} {}
    UUID(uint64_t high, uint64_t low) : value{high, low} {}

    bool operator==(const UUID& other) const {
        return value[0] == other.value[0] && value[1] == other.value[1];
    }

    bool operator!=(const UUID& other) const {
        return !(*this == other);
    }

    bool operator<(const UUID& other) const {
        return value < other.value; // std::array supports lexicographic comparison
    }

    // Pretty hex string, e.g. 3f12e1c4-79f1-4e6c-a789-dcf0a91d2ab3
    std::string toString() const {
        std::stringstream ss;
        ss << std::hex << std::setfill('0');

        // Split into 5 UUID segments (like classic UUID format)
        ss << std::setw(8) << (uint32_t)(value[0] >> 32) << "-";
        ss << std::setw(4) << (uint16_t)(value[0] >> 16) << "-";
        ss << std::setw(4) << (uint16_t)value[0] << "-";
        ss << std::setw(4) << (uint16_t)(value[1] >> 48) << "-";
        ss << std::setw(12) << (value[1] & 0xFFFFFFFFFFFFULL);

        return ss.str();
    }

    static UUID generate() {
        static thread_local std::mt19937_64 rng(std::random_device{}());
        static std::uniform_int_distribution<uint64_t> dist;
        return UUID(dist(rng), dist(rng));
    }
};

// Hash support for std::unordered_map / unordered_set
namespace std {
    template <>
    struct hash<UUID> {
        std::size_t operator()(const UUID& uuid) const {
            return std::hash<uint64_t>()(uuid.value[0]) ^ std::hash<uint64_t>()(uuid.value[1]);
        }
    };
}
