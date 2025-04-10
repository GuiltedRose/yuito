#pragma once

#include <memory>
#include <string>
#include "player/PlayerEntity.h"
#include <QString>
#include "render/Husk.h"
#include "player/PlayerCharAdapter.h"
#include "render/colors.h"
#include "render/RenderSystem.h"
#include <vector>


class PlayerManager {
public:
    PlayerManager();

    // Setup
    void createNewEntity(const std::string& playerID);
    std::shared_ptr<PlayerEntity> getPlayerEntity() const;

    // Character lifecycle
    std::shared_ptr<PlayerCharacter> createNewCharacter(const std::string& worldID);
    std::shared_ptr<PlayerCharacter> getCurrentCharacter() const;

    bool saveToFile(const QString& path);
    bool loadFromFile(const QString& path);

    void updateHusk(float deltaTime);
    void renderHusk(RenderSystem* renderSystem);

    Vec3 color;

private:
    Husk husk;
    std::shared_ptr<PlayerCharAdapter> visualAdapter;

    std::shared_ptr<PlayerEntity> entity;
    std::shared_ptr<PlayerCharacter> currentCharacter;
    std::vector<Husk> playerHusks;
};
