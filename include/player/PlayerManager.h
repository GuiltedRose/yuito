#pragma once

#include <memory>
#include <string>
#include "player/PlayerEntity.h"

class PlayerManager {
public:
    PlayerManager();

    // Setup
    void createNewEntity(const std::string& playerID);
    std::shared_ptr<PlayerEntity> getPlayerEntity() const;

    // Character lifecycle
    std::shared_ptr<PlayerCharacter> createNewCharacter(const std::string& worldID);
    std::shared_ptr<PlayerCharacter> getCurrentCharacter() const;

private:
    std::shared_ptr<PlayerEntity> entity;
    std::shared_ptr<PlayerCharacter> currentCharacter;
};
