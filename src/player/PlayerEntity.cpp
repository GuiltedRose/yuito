#include "player/PlayerEntity.h"

PlayerEntity::PlayerEntity(const std::string& id)
    : id(id)
{}

const std::string& PlayerEntity::getID() const {
    return id;
}

int PlayerEntity::getGenerationCount() const {
    return static_cast<int>(generations.size());
}

void PlayerEntity::addCharacter(std::shared_ptr<PlayerCharacter> character) {
    generations.push_back(character);
}

const std::vector<std::shared_ptr<PlayerCharacter>>& PlayerEntity::getAllCharacters() const {
    return generations;
}

std::shared_ptr<PlayerCharacter> PlayerEntity::getLatestCharacter() const {
    if (generations.empty()) return nullptr;
    return generations.back();
}

void PlayerEntity::rememberWorld(const std::string& worldID) {
    visitedWorlds[worldID] = true;
}

bool PlayerEntity::hasVisitedWorld(const std::string& worldID) const {
    auto it = visitedWorlds.find(worldID);
    return it != visitedWorlds.end();
}

EchoTouchLog& PlayerEntity::getEchoLog() {
    return echoLog;
}
