#include "living-legacy/faction/FactionSystem.h"
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonValue>

namespace living_legacy::faction {
    void FactionSystem::loadFromJsonFile(const QString& filePath) {
        QFile file(filePath);
        if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            throw std::runtime_error("Failed to open faction JSON file.");
        }
    
        QByteArray rawData = file.readAll();
        QJsonParseError parseError;
        QJsonDocument doc = QJsonDocument::fromJson(rawData, &parseError);
    
        if (parseError.error != QJsonParseError::NoError) {
            throw std::runtime_error("JSON parse error: " + parseError.errorString().toStdString());
        }
    
        if (!doc.isArray()) {
            throw std::runtime_error("Expected top-level JSON array for factions.");
        }
    
        QJsonArray factionArray = doc.array();
    
        for (const QJsonValue& val : factionArray) {
            if (!val.isObject()) continue;
    
            QJsonObject obj = val.toObject();
            Faction faction(
                obj["id"].toString().toStdString(),
                obj["name"].toString().toStdString()
            );
    
            faction.isOutlaw = obj.value("isOutlaw").toBool(false);
            faction.isAtWar = obj.value("isAtWar").toBool(false);
    
            QJsonObject rels = obj.value("relationships").toObject();
            for (const QString& key : rels.keys()) {
                int standing = rels.value(key).toInt(0);
                faction.relationships[key.toStdString()] = static_cast<FactionStanding>(standing);
            }
    
            factions_[faction.id] = std::move(faction);
        }
    }

    void FactionSystem::addFaction(const FactionID& id, const std::string& name) {
        factions_.emplace(id, Faction{id, name});
    }

    void FactionSystem::removeFaction(const FactionID& id) {
        factions_.erase(id);
        for(auto& [fid, faction] : factions_) {
            faction.relationships.erase(id);
        }
    }

    bool FactionSystem::factionExists(const FactionID& id) const {
        return factions_.find(id) != factions_.end();
    }

    void FactionSystem::setRelationship(const FactionID& a, const FactionID& b, FactionStanding standing) {
        auto* fa = getFaction(a);
        auto* fb = getFaction(b);

        if(fa && fb) {
            fa->relationships[b] = standing;
            fb->relationships[a] = standing;
        }
    }

    Faction* FactionSystem::getFaction(const FactionID& id) {
        auto it = factions_.find(id);
        return(it != factions_.end()) ? &it->second : nullptr;
    }

    const Faction* FactionSystem::getFaction(const FactionID& id) const {
        auto it = factions_.find(id);
        return(it != factions_.end()) ? &it->second : nullptr;
    }

    FactionStanding FactionSystem::getRelationship(const FactionID& a, const FactionID& b) const {
        const auto* fa = getFaction(a);
        if(!fa) return FactionStanding::Neutral;

        auto it = fa->relationships.find(b);
        if(it != fa->relationships.end()) {
            return it->second;
        }
        return FactionStanding::Neutral;
    }
    const std::unordered_map<FactionID, Faction>& FactionSystem::getAllFactions() const {
        return factions_;
    }

    void FactionSystem::evaluatePromotions() {
        for (auto& [id, faction] : factions_) {
            for (NPC& member : faction.members) {
                if (member.loyalty > 70 && member.rank < faction.maxRank) {
                    member.rank++;
                    faction.history.push_back(member.name + " was promoted to rank " + std::to_string(member.rank));
                }
            }
        }
    }    
}