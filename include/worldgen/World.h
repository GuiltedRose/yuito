#pragma once

#include "living-legacy/world/WorldState.h"
#include "worldgen/ChunkManager.h"
#include "math/Vec2.h"
#include "render/RenderTypes.h"
#include "render/RenderSystem.h"
#include "worldgen/WorldTypes.h"
#include "worldgen/WorldGenerator.h"
#include "worldgen/CaveGenerator.h"

class World {
public:
    World();
    void update();
    void prepareRender(MapLayer layer);
    const std::vector<Tile>& getRenderTiles(MapLayer layer) const;

    void setPlayerPosition(const Math::Vec2i& pos);
    const Math::Vec2i& getPlayerPosition() const;

    void setCurrentLayer(MapLayer layer);
    MapLayer getCurrentLayer() const;

    living_legacy::world::WorldState& state();
    ChunkManager& chunks();

    // New fields to inject dependencies externally
    void setRenderSystem(RenderSystem* render);

private:
    WorldGenerator generator_;
    ChunkManager chunkManager_;
    Math::Vec2i playerPosition;
    MapLayer currentLayer;
    std::vector<Tile> renderTiles_;
    living_legacy::world::WorldState state_;

    // 🔹 Add these:
    std::unordered_map<std::string, CaveNetwork> caveNetworks;
    RenderSystem* renderSystem = nullptr;
};
 
