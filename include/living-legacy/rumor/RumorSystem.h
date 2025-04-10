#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include "living-legacy/faction/FactionSystem.h"

namespace living_legacy::rumor {
    struct Rumor {
        std::string description;
        std::string sourceFaction;
        std::string targetFaction;

        int impact = 0;
        bool isTrue = true;
        std::string eventTag;

        std::string id;
        int age = 0;
        bool expired = false;
    };

    class RumorSystem {
    public:
        void addRumor(const Rumor& rumor);
        void clearRumorsForFaction(const std::string& factionId);

        const std::vector<Rumor>& getRumorsForFaction(const std::string& factionId) const;
        const std::vector<Rumor>& getAllRumors() const;

        void spreadRumors(const faction::FactionSystem& factions);
    private:
        std::unordered_map<std::string, std::vector<Rumor>> rumorMap_;
        std::unordered_map<std::string, std::vector<Rumor>> factionRumors_;
        std::vector<Rumor> globalRumors_;
    };
}