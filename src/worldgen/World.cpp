#include "worldgen/World.h"
#include "worldgen/ChunkManager.h"
#include <random>

World::World()
    : generator_(std::random_device{}()),                      // Random seed for generator
      chunkManager_(generator_, std::random_device{}()),       // Random seed for chunk manager
      playerPosition{0, 0} {
    
    chunkManager_.updatePlayerRegion({0, 0}); // Load initial region
}

void World::update() {
    int chunkSize = 16;
    Vec2i region = {
        static_cast<int>(std::floor(playerPosition.x / static_cast<float>(chunkSize))),
        static_cast<int>(std::floor(playerPosition.y / static_cast<float>(chunkSize)))
    };

    chunkManager_.updatePlayerRegion(region);
    state_.update();
}

void World::prepareRender() {
    renderTiles_.clear();
    auto tiles = chunkManager_.collectRenderTiles();
    renderTiles_.insert(renderTiles_.end(), tiles.begin(), tiles.end());
}

const std::vector<Tile>& World::getRenderTiles() const {
    return renderTiles_;
}

void World::setPlayerPosition(const Vec2i& pos) {
    playerPosition = pos;
}

const Vec2i& World::getPlayerPosition() const {
    return playerPosition;
}

living_legacy::world::WorldState& World::state() {
    return state_;
}

ChunkManager& World::chunks() {
    return chunkManager_;
}
