#include "player/PlayerManager.h"
#include <QFile>
#include <QDir>
#include <QDebug>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonParseError>

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

    // Attach to Husk
    visualAdapter = std::make_shared<PlayerCharAdapter>(character);
    husk.attachCharacter(visualAdapter);
    visualAdapter->setColor(ColorPalette::accessibleColors[rand() % ColorPalette::accessibleColors.size()]);


    return character;
}

std::shared_ptr<PlayerCharacter> PlayerManager::getCurrentCharacter() const {
    return currentCharacter;
}

bool PlayerManager::saveToFile(const QString& path) {
    if (!entity) return false;

    QJsonObject root = entity->toJson();
    QJsonDocument doc(root);

    QDir().mkpath(QFileInfo(path).absolutePath()); // ensure save directory exists

    QFile file(path);
    if (!file.open(QIODevice::WriteOnly)) {
        qWarning() << "Failed to save player to" << path << ":" << file.errorString();
        return false;
    }

    file.write(doc.toJson(QJsonDocument::Indented));
    file.close();
    return true;
}

bool PlayerManager::loadFromFile(const QString& path) {
    QFile file(path);
    if (!file.open(QIODevice::ReadOnly)) {
        qWarning() << "Failed to load player from" << path << ":" << file.errorString();
        return false;
    }

    QByteArray data = file.readAll();
    file.close();

    QJsonParseError err;
    QJsonDocument doc = QJsonDocument::fromJson(data, &err);
    if (err.error != QJsonParseError::NoError) {
        qWarning() << "JSON parse error:" << err.errorString();
        return false;
    }

    QJsonObject root = doc.object();
    entity = std::make_shared<PlayerEntity>("TEMP"); // id will be overwritten in fromJson
    entity->fromJson(root);

    // Optionally auto-assign the latest character:
    currentCharacter = entity->getLatestCharacter();

    return true;
}


// Uses a shared mesh between Humanoid NPCs and Players to generate 3D characters.
void PlayerManager::updateHusk(float deltaTime) {
    husk.update(deltaTime);
}

void PlayerManager::renderHusk(RenderSystem* renderSystem) {
    for (auto& husk : playerHusks) {
        husk.render(renderSystem);  // ✅ renderSystem is now valid
    }
}

