#pragma once

#include <unordered_map>
#include <unordered_set>
#include <queue>
#include <mutex>
#include <thread>
#include <condition_variable>
#include <optional>
#include "math/Vec2.h"
#include "worldgen/WorldGenerator.h"
#include "render/RenderTypes.h"
#include "render/CaveMeshGenerator.h"

struct Chunk {
    Math::Vec2i coords;
    std::vector<Location> locations;
};

struct TimedChunk {
    Chunk data;
    int staleFrames = 0;
    std::optional<CaveMeshGenerator::CaveMesh> caveMesh;
};

class ChunkManager {
public:
    ChunkManager(WorldGenerator& generator, unsigned int seed);
    ~ChunkManager();

    Chunk* getChunk(const Math::Vec2i& coords, MapLayer layer);
    void updatePlayerRegion(const RegionKey& regionKey);
    void setMapLayer(MapLayer layer);

    std::vector<Tile> collectRenderTiles(MapLayer layer) const;
    std::vector<CaveMeshGenerator::CaveMesh> collectCaveMeshes() const;

    const std::unordered_map<RegionKey, TimedChunk >& getActiveChunks() const;

private:
    void unloadFarChunks();
    bool isWithinRadius(const Math::Vec2i& a, const Math::Vec2i& b, int radius);

    WorldGenerator& generator;
    unsigned int seed;

    MapLayer currentLayer = MapLayer::Surface;
    RegionKey playerRegion;

    int chunkRadius = 2;
    
    std::unordered_set<RegionKey> enqueuedRegions;
    std::queue<RegionKey> chunkLoadQueue;

    std::thread workerThread;
    std::condition_variable chunkCV;
    std::unordered_map<RegionKey, TimedChunk> activeChunks;
    mutable std::mutex chunkMutex;
    bool running = true;
};
