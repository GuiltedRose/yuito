#include "player/PlayerCharacter.h"
#include "player/EchoTouchLog.h"


PlayerCharacter::PlayerCharacter(const std::string& id, const std::string& worldID, int generationNumber)
    : id(id), worldID(worldID), generation(generationNumber), birthTime(std::chrono::system_clock::now()) {
    personalEchoLog = std::make_unique<EchoTouchLog>();
}

const std::string& PlayerCharacter::getID() const {
    return id;
}

const std::string& PlayerCharacter::getWorldID() const {
    return worldID;
}

int PlayerCharacter::getGeneration() const {
    return generation;
}

void PlayerCharacter::setDeathInfo(const std::string& cause) {
    dead = true;
    deathCause = cause;
    deathTime = std::chrono::system_clock::now();
}

bool PlayerCharacter::isDead() const {
    return dead;
}

const std::string& PlayerCharacter::getDeathCause() const {
    return deathCause;
}

void PlayerCharacter::setTrait(const std::string& key, const std::string& value) {
    traits[key] = value;
}

const std::unordered_map<std::string, std::string>& PlayerCharacter::getTraits() const {
    return traits;
}

std::chrono::system_clock::time_point PlayerCharacter::getBirthTime() const {
    return birthTime;
}

std::chrono::system_clock::time_point PlayerCharacter::getDeathTime() const {
    return deathTime;
}

void PlayerCharacter::recordRelicTouch(const std::string& itemID, const std::string& emotionalTag) {
    EchoTouch touch;
    touch.characterID = id;
    touch.worldID = worldID;
    touch.generation = generation;
    touch.emotionalTag = emotionalTag;

    // You can replace this with a proper time library if needed
    touch.timestamp = "TODO:timestamp"; 

    personalEchoLog->logTouch(itemID, touch);

}

bool PlayerCharacter::hasTouchedItem(const std::string& itemID) const {
    return personalEchoLog->wasTouchedBy(itemID, id);
}

