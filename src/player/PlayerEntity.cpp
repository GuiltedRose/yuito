#include "player/PlayerEntity.h"
#include <QJsonArray>

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

QJsonObject PlayerEntity::toJson() const {
    QJsonObject obj;
    obj["id"] = QString::fromStdString(id);

    // visitedWorlds
    QJsonObject worldsJson;
    for (const auto& [worldID, visited] : visitedWorlds)
        worldsJson[QString::fromStdString(worldID)] = visited;
    obj["visitedWorlds"] = worldsJson;

    // generations
    QJsonArray genArray;
    for (const auto& character : generations)
        genArray.append(character->toJson());
    obj["generations"] = genArray;

    j["echoLog"] = echoLog.toJson();

    return obj;
}

void PlayerEntity::fromJson(const QJsonObject& json) {
    id = json["id"].toString().toStdString();

    visitedWorlds.clear();
    auto worldsJson = json["visitedWorlds"].toObject();
    for (const auto& key : worldsJson.keys())
        visitedWorlds[key.toStdString()] = worldsJson[key].toBool();

    generations.clear();
    auto genArray = json["generations"].toArray();
    for (const auto& entry : genArray) {
        auto character = std::make_shared<PlayerCharacter>("", "", 0);
        character->fromJson(entry.toObject());
        generations.push_back(character);
    }

    echoLog.fromJson(j["echoLog"].toObject());
}

