#pragma once

#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <QString>

namespace living_legacy::faction {
    using FactionID = std::string;

    enum class FactionStanding {
        Enemy = -100,
        Hostile = -50,
        Neutral = 0,
        Friendly = 50,
        Ally = 100
    };

    struct Faction {
        FactionID id;
        std::string name;
        std::unordered_map<FactionID, FactionStanding> relationships;
        bool isOutlaw = false;
        bool isAtWar = false;
        Faction(const FactionID& fid, const std::string& fname) : id(fid), name(fname) {}
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
    private:
        std::unordered_map<FactionID, Faction> factions_;
    };
}