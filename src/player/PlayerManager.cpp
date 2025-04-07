#include "player/PlayerManager.h"

PlayerManager::PlayerManager() {}

void PlayerManager::createNewEntity(const std::string& playerID) {
    entity = std::make_shared<PlayerEntity>(playerID);
}

std::shared_ptr<PlayerEntity> PlayerManager::getPlayerEntity() const {
    return entity;
}

std::shared_ptr<PlayerCharacter> PlayerManager::createNewCharacter(const std::string& worldID) {
    if (!entity) return nullptr;

    int nextGen = entity->getGenerationCount() + 1;
    std::string charID = entity->getID() + "_Gen" + std::to_string(nextGen);

    auto character = std::make_shared<PlayerCharacter>(charID, worldID, nextGen);
    entity->addCharacter(character);
    currentCharacter = character;

    return character;
}

std::shared_ptr<PlayerCharacter> PlayerManager::getCurrentCharacter() const {
    return currentCharacter;
}
