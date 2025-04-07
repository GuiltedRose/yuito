#include "living-legacy/rumor/RumorSystem.h"

namespace living_legacy::rumor {
    void RumorSystem::addRumor(const Rumor& rumor) {
        globalRumors_.push_back(rumor);
        factionRumors_[rumor.targetFaction].push_back(rumor);
    }

    void RumorSystem::clearRumorsForFaction(const std::string& factionId) {
        factionRumors_.erase(factionId);
    }

    const std::vector<Rumor>& RumorSystem::getRumorsForFaction(const std::string& factionId) const {
        static const std::vector<Rumor> empty;
        auto it = factionRumors_.find(factionId);
        return(it != factionRumors_.end()) ? it->second : empty;
    }

    const std::vector<Rumor>& RumorSystem::getAllRumors() const {
        return globalRumors_;
    }

}