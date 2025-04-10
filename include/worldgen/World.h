#pragma once

#include "living-legacy/world/WorldState.h"
#include "worldgen/ChunkManager.h"
#include "worldgen/WorldGenerator.h" // includes Vec2i, Vec2
#include "worldgen/GLWidget.h"       // for Tile

class World {
    public:
        World();
    
        void update();
        void prepareRender();
    
        const std::vector<Tile>& getRenderTiles() const;
        void setPlayerPosition(const Vec2i& pos);
        const Vec2i& getPlayerPosition() const;
    
        living_legacy::world::WorldState& state();
        ChunkManager& chunks();
    
    private:
        WorldGenerator generator_;
        ChunkManager chunkManager_;
        Vec2i playerPosition;
        std::vector<Tile> renderTiles_;
        living_legacy::world::WorldState state_;
    };
