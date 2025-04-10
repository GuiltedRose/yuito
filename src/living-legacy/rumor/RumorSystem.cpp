#include "living-legacy/rumor/RumorSystem.h"
#include <algorithm>

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

    void RumorSystem::spreadRumors(const faction::FactionSystem& factions) {
        std::vector<Rumor> newRumors;
    
        for (auto& [factionId, rumorsList] : rumorMap_) {
            for (Rumor& rumor : rumorsList) {
                // Age the rumor
                rumor.age++;
    
                // Chance to evolve or mutate
                if (rumor.age > 3 && (rand() % 100) < 10) {
                    Rumor evolved = rumor;
                    evolved.id += "_mut";
                    evolved.description += " (twisted over time)";
                    evolved.age = 0;
                    newRumors.push_back(evolved);
                }
    
                // Chance to fade
                if (rumor.age > 10 && (rand() % 100) < 30) {
                    // Remove later
                    rumor.expired = true;
                }
            }
    
            // Remove expired rumors
            rumorsList.erase(std::remove_if(rumorsList.begin(), rumorsList.end(),
                [](const Rumor& r) { return r.expired; }),
                rumorsList.end());
        }
    
        // Inject new rumors
        for (Rumor& r : newRumors) {
            rumorMap_[r.sourceFaction].push_back(r);
        }
    }       
}