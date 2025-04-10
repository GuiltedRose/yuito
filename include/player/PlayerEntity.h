#pragma once

#include <string>
#include <vector>
#include <memory>
#include <unordered_map>

#include "player/PlayerCharacter.h"
#include "player/EchoTouchLog.h"
#include "player/Serializable.h"

class PlayerEntity : public Serializable {
public:
    PlayerEntity(const std::string& id);

    // Core data
    const std::string& getID() const;
    int getGenerationCount() const;

    // Generation management
    void addCharacter(std::shared_ptr<PlayerCharacter> character);
    const std::vector<std::shared_ptr<PlayerCharacter>>& getAllCharacters() const;
    std::shared_ptr<PlayerCharacter> getLatestCharacter() const;

    // Global memory (e.g. dreams, world knowledge)
    void rememberWorld(const std::string& worldID);
    bool hasVisitedWorld(const std::string& worldID) const;

    // Echo relic tracking
    EchoTouchLog& getEchoLog();

    QJsonObject toJson() const override;
    void fromJson(const QJsonObject& json) override;

private:
    std::string id; // Unique meta-player ID
    std::vector<std::shared_ptr<PlayerCharacter>> generations;
    std::unordered_map<std::string, bool> visitedWorlds;
    EchoTouchLog echoLog;
    QJsonObject j;
};