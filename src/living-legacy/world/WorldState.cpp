#include "living-legacy/world/WorldState.h"

namespace living_legacy::world {

    void WorldState::update() {
        // Placeholder logic for future time progression, faction updates, etc.
        // For example:
        // - spread rumors
        // - promote faction members
        // - decay old reputation
        // - simulate passive world events

        // This will likely call into rumors(), factions(), etc. in the future.
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
