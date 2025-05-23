#pragma once

#include <string>
#include <unordered_map>

namespace living_legacy::reputation {
    using FactionID = std::string;

    struct Reputation {
        int score = 0;
        bool isKnown = false;
        int lastInteraction = 0;
    };

    class ReputationSystem {
    public:
        void setReputation(const FactionID& factionId, int value);
        void modifyReputation(const FactionID& factionId, int delta);
        int getRepuation(const FactionID& factionId) const;

        void applyRumorImpact(const FactionID& factionId, int impact);
        const std::unordered_map<FactionID, Reputation>& getAllReputations() const;

        void decayReputation();
        

    private:
        std::unordered_map<FactionID, Reputation> reputationMap_;
    };
}