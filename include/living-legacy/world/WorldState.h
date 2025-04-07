#pragma once

#include "living-legacy/reputation/ReputationSystem.h"
#include "living-legacy/rumor/RumorSystem.h"
#include "living-legacy/legacy/LegacySystem.h"

namespace living_legacy::world {
    class WorldState {
    public:
        void update();
        living_legacy::faction::FactionSystem& factions();
        living_legacy::reputation::ReputationSystem& reputation();
        living_legacy::rumor::RumorSystem& rumors();
        living_legacy::legacy::LegacySystem& legacy();
    private:
        living_legacy::faction::FactionSystem factionSystem_;
        living_legacy::reputation::ReputationSystem reputationSystem_;
        living_legacy::rumor::RumorSystem rumorSystem_;
        living_legacy::legacy::LegacySystem legacySystem_;
    };
}