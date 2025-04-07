#pragma once

#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <QString>

namespace living_legacy::faction {
    using FactionID = std::string;

    enum class FactionStanding {
        Neutral,
        Ally,
        Enemy
    };

    struct Faction {
        // Constructors
        Faction() : id("unknown"), name("Unnamed Faction"), isOutlaw(false), isAtWar(false) {}
        Faction(const FactionID& fid, const std::string& fname)
            : id(fid), name(fname), isOutlaw(false), isAtWar(false) {}

        // Core data
        FactionID id;
        std::string name;

        // Faction state
        bool isOutlaw;
        bool isAtWar;

        // Relationships with other factions
        std::unordered_map<FactionID, FactionStanding> relationships;
    };
    

    class FactionSystem {
    public:
        void loadFromJsonFile(const QString& filePath);

        void addFaction(const FactionID& id, const std::string& name);
        void removeFaction(const FactionID& id);

        bool factionExists(const FactionID& id) const;
        
        void setRelationship(const FactionID& a, const FactionID& b, FactionStanding standing);

        Faction* getFaction(const FactionID& id);
        const Faction* getFaction(const FactionID& id) const;
        FactionStanding getRelationship(const FactionID& a, const FactionID& b) const;

        const std::unordered_map<FactionID, Faction>& getAllFactions() const;

        void evaluatePromotions();   
    private:
        std::unordered_map<FactionID, Faction> factions_;
    };
}