#include "worldgen/World.h"
#include "worldgen/ChunkManager.h"
#include "worldgen/CaveGenerator.h"
#include <random>
#include <iostream>

World::World()
    : generator_(std::random_device{}()),
      chunkManager_(generator_, std::random_device{}()),
      playerPosition{0, 0},
      currentLayer(MapLayer::Surface)
{
    chunkManager_.updatePlayerRegion({playerPosition, currentLayer});
}

void World::update() {
    int chunkSize = 16;
    Math::Vec2i region = {
        static_cast<int>(std::floor(playerPosition.x / static_cast<float>(chunkSize))),
        static_cast<int>(std::floor(playerPosition.y / static_cast<float>(chunkSize)))
    };

    chunkManager_.updatePlayerRegion({region, currentLayer});
    state_.update();
}

void World::prepareRender(MapLayer layer) {
    renderTiles_.clear();
    auto tiles = chunkManager_.collectRenderTiles(layer);
    renderTiles_.insert(renderTiles_.end(), tiles.begin(), tiles.end());

    std::cout << "[Render] Preparing tiles for layer: "
              << (layer == MapLayer::Surface ? "Surface" : "Underground")
              << " - got " << tiles.size() << " tiles.\n";

    if (layer == MapLayer::Underground && !caveNetworks.empty()) {
        std::cout << "[Render] Caves detected: " << caveNetworks.size() << "\n";
        for (auto& [id, cave] : caveNetworks) {
            std::cout << "[Render] Cave mesh uploading: " << id << " - size: " << cave.caveGrid.size() << "\n";
            auto mesh = CaveMeshGenerator::generateCaveMesh(cave.caveGrid, 1.0f);
            renderSystem->uploadCaveMesh(mesh);
            break;
        }
    }
}

void World::setRenderSystem(RenderSystem* render) {
    renderSystem = render;
}

const std::vector<Tile>& World::getRenderTiles(MapLayer layer) const {
    return renderTiles_;
}

void World::setPlayerPosition(const Math::Vec2i& pos) {
    playerPosition = pos;
}

const Math::Vec2i& World::getPlayerPosition() const {
    return playerPosition;
}

void World::setCurrentLayer(MapLayer layer) {
    currentLayer = layer;
}

MapLayer World::getCurrentLayer() const {
    return currentLayer;
}

living_legacy::world::WorldState& World::state() {
    return state_;
}

ChunkManager& World::chunks() {
    return chunkManager_;
}
