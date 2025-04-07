#pragma once

#include <string>
#include <vector>
#include <unordered_map>

namespace living_legacy::rumor {
    struct Rumor {
        std::string description;
        std::string sourceFaction;
        std::string targetFaction;

        int impact = 0;
        bool isTrue = true;
        std::string eventTag;
    };

    class RumorSystem {
    public:
        void addRumor(const Rumor& rumor);
        void clearRumorsForFaction(const std::string& factionId);

        const std::vector<Rumor>& getRumorsForFaction(const std::string& factionId) const;
        const std::vector<Rumor>& getAllRumors() const;
    private:
        std::unordered_map<std::string, std::vector<Rumor>> factionRumors_;
        std::vector<Rumor> globalRumors_;
    };
}