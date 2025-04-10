#include "living-legacy/world/WorldState.h"

namespace living_legacy::world {

    void WorldState::update() {
        rumorSystem_.spreadRumors(factionSystem_);
        factionSystem_.evaluatePromotions();
        reputationSystem_.decayReputation();
        legacySystem_.updateLegacy();
    }    

    living_legacy::faction::FactionSystem& WorldState::factions() {
        return factionSystem_;
    }

    living_legacy::reputation::ReputationSystem& WorldState::reputation() {
        return reputationSystem_;
    }

    living_legacy::rumor::RumorSystem& WorldState::rumors() {
        return rumorSystem_;
    }

    living_legacy::legacy::LegacySystem& WorldState::legacy() {
        return legacySystem_;
    }

}
