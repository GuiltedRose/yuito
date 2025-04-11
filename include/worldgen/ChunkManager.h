#pragma once

#include "worldgen/WorldGenerator.h"
#include <thread>
#include <mutex>
#include <queue>
#include <condition_variable>
#include <unordered_set>

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
    ~ChunkManager();
    void unloadFarChunks();

    // Rendering:
    std::vector<Tile> collectRenderTiles() const;

private:
    std::unordered_map<Vec2i, Chunk, Vec2i::Hash> activeChunks;

    WorldGenerator& generator;
    unsigned int seed;

    const int chunkRadius = 3;
    Vec2i playerRegion;

    std::unordered_set<Vec2i, Vec2i::Hash> enqueuedRegions;

    std::thread workerThread;
    mutable std::mutex chunkMutex;
    std::condition_variable chunkCV;

    std::queue<Vec2i> chunkLoadQueue;
    bool running = true;

    bool isWithinRadius(const Vec2i& a, const Vec2i& b, int radius);
};
