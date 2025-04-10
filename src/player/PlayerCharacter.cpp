#include "player/PlayerCharacter.h"
#include "player/EchoTouchLog.h"
#include <QJsonArray>

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


QJsonObject PlayerCharacter::toJson() const {
    QJsonObject obj;
    obj["id"] = QString::fromStdString(id);
    obj["worldID"] = QString::fromStdString(worldID);
    obj["generation"] = generation;
    obj["dead"] = dead;
    obj["deathCause"] = QString::fromStdString(deathCause);

    obj["birthTime"] = static_cast<qint64>(std::chrono::duration_cast<std::chrono::seconds>(birthTime.time_since_epoch()).count());
    obj["deathTime"] = static_cast<qint64>(std::chrono::duration_cast<std::chrono::seconds>(deathTime.time_since_epoch()).count());

    QJsonObject traitsJson;
    for (const auto& [key, value] : traits)
        traitsJson[QString::fromStdString(key)] = QString::fromStdString(value);

    obj["traits"] = traitsJson;
    return obj;
}

void PlayerCharacter::fromJson(const QJsonObject& json) {
    id = json["id"].toString().toStdString();
    worldID = json["worldID"].toString().toStdString();
    generation = json["generation"].toInt();
    dead = json["dead"].toBool();
    deathCause = json["deathCause"].toString().toStdString();

    auto traitsJson = json["traits"].toObject();
    traits.clear();
    for (const auto& key : traitsJson.keys())
        traits[key.toStdString()] = traitsJson[key].toString().toStdString();

    qint64 birthSec = json["birthTime"].toVariant().toLongLong();
    qint64 deathSec = json["deathTime"].toVariant().toLongLong();
    birthTime = std::chrono::system_clock::time_point(std::chrono::seconds(birthSec));
    deathTime = std::chrono::system_clock::time_point(std::chrono::seconds(deathSec));
}

