#pragma once

#include <string>
#include <vector>
#include <random>

namespace NameGenerator {

    // Internal RNG setup
    inline std::mt19937& rng() {
        static thread_local std::mt19937 rng(std::random_device{}());
        return rng;
    }

    // Utility to pick a random element from a list
    template <typename T>
    inline const T& pickRandom(const std::vector<T>& list) {
        std::uniform_int_distribution<> dist(0, (int)list.size() - 1);
        return list[dist(rng())];
    }

    // Pool of first names (you can categorize these later)
    inline const std::vector<std::string> firstNames = {
        "Lira", "Drogan", "Keth", "Navi", "Serel",
        "Tava", "Korin", "Venn", "Marn", "Ysra"
    };

    // Pool of surnames (can be used for region/family naming too)
    inline const std::vector<std::string> surnames = {
        "Ashborn", "Duskweaver", "Hollowroot", "Stonewake",
        "Nightfen", "Whisperbranch", "Graveleap", "Moonsinger"
    };

    // Full name generation: "Tava Hollowroot"
    inline std::string randomName() {
        return pickRandom(firstNames) + " " + pickRandom(surnames);
    }

    // For world generation or objects:
    inline std::string randomTitle() {
        static std::vector<std::string> adjectives = {
            "Ancient", "Broken", "Whispering", "Forgotten", "Burning", "Blessed"
        };
        static std::vector<std::string> nouns = {
            "Vale", "Watcher", "Forge", "Spire", "Path", "Remnant"
        };
        return pickRandom(adjectives) + " " + pickRandom(nouns);
    }
}
