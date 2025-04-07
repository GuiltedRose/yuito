#pragma once

#include "worldgen/WorldGenerator.h"

struct Chunk {
    Vec2i coords;
    std::vector<Location> locations;
    bool isModified = false;
};

class ChunkManager {
public:
    ChunkManager(WorldGenerator& generator, unsigned int seed);

    Chunk* getChunk(const Vec2i& coords);
    void updatePlayerRegion(const Vec2i& region);
    void unloadFarChunks();

    // Rendering:
    std::vector<Tile> collectRenderTiles() const;

private:
    std::unordered_map<Vec2i, Chunk, Vec2i::Hash> activeChunks;

    WorldGenerator& generator;
    unsigned int seed;

    const int chunkRadius = 3;
    Vec2i playerRegion;

    bool isWithinRadius(const Vec2i& a, const Vec2i& b, int radius);
};
