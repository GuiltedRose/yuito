#include "living-legacy/reputation/ReputationSystem.h"

namespace living_legacy::reputation {
    void ReputationSystem::setReputation(const FactionID& factionId, int value) {
        Reputation& rep = reputationMap_[factionId];
        rep.score = value;
        rep.isKnown = true;
    }

    void ReputationSystem::modifyReputation(const FactionID& factionId, int delta) {
        Reputation& rep = reputationMap_[factionId];
        rep.score += delta;
        rep.isKnown = true;
    }

    int ReputationSystem::getRepuation(const FactionID& factionId) const {
        auto it = reputationMap_.find(factionId);
        return(it != reputationMap_.end()) ? it->second.score : 0;
    }

    void ReputationSystem::applyRumorImpact(const FactionID& factionId, int impact) {
        modifyReputation(factionId, impact);
    }

    const std::unordered_map<FactionID, Reputation>& ReputationSystem::getAllReputations() const {
        return reputationMap_;
    }
}