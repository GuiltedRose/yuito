#include "worldgen/ChunkManager.h"
#include <algorithm>

ChunkManager::ChunkManager(WorldGenerator& generator, unsigned int seed)
    : generator(generator), seed(seed), playerRegion({{0, 0}, MapLayer::Surface}) {

    workerThread = std::thread([this, &generator, seed]() {
        while (running) {
            RegionKey key;

            {
                std::unique_lock<std::mutex> lock(chunkMutex);
                chunkCV.wait(lock, [this]() { return !chunkLoadQueue.empty() || !running; });
                if (!running) break;

                key = chunkLoadQueue.front();
                chunkLoadQueue.pop();
            }

            auto locations = generator.generateRegionWithSeed(key.coords, seed);
            generator.linkAdjacentRegions(key);

            {
                std::lock_guard<std::mutex> lock(chunkMutex);
                activeChunks[key] = TimedChunk{ Chunk{ key.coords, locations }, 0 };
                enqueuedRegions.erase(key);
            }
        }
    });
}

ChunkManager::~ChunkManager() {
    {
        std::lock_guard<std::mutex> lock(chunkMutex);
        running = false;
    }
    chunkCV.notify_all();
    if (workerThread.joinable())
        workerThread.join();
}

void ChunkManager::setMapLayer(MapLayer layer) {
    std::lock_guard<std::mutex> lock(chunkMutex);
    currentLayer = layer;
}

Chunk* ChunkManager::getChunk(const Math::Vec2i& coords, MapLayer layer) {
    RegionKey key{ coords, layer };
    std::lock_guard<std::mutex> lock(chunkMutex);

    auto it = activeChunks.find(key);
    if (it != activeChunks.end()) {
        return &it->second.data;
    }

    auto locations = generator.generateRegionWithSeed(coords, seed);
    activeChunks[key] = TimedChunk{ Chunk{ coords, locations }, 0 };
    return &activeChunks[key].data;
}

bool ChunkManager::isWithinRadius(const Math::Vec2i& a, const Math::Vec2i& b, int radius) {
    int dx = a.x - b.x;
    int dy = a.y - b.y;
    return (dx * dx + dy * dy) <= (radius * radius);
}

void ChunkManager::updatePlayerRegion(const RegionKey& key) {
    playerRegion = key;

    for (int dy = -chunkRadius - 1; dy <= chunkRadius + 1; ++dy) {
        for (int dx = -chunkRadius - 1; dx <= chunkRadius + 1; ++dx) {
            Math::Vec2i coords = { key.coords.x + dx, key.coords.y + dy };
            RegionKey rkey = { coords, key.layer };

            std::lock_guard<std::mutex> lock(chunkMutex);
            if (activeChunks.find(rkey) == activeChunks.end() &&
                enqueuedRegions.find(rkey) == enqueuedRegions.end()) {
                chunkLoadQueue.push(rkey);
                enqueuedRegions.insert(rkey);
                chunkCV.notify_one();
            }
        }
    }

    unloadFarChunks();
}

void ChunkManager::unloadFarChunks() {
    for (auto it = activeChunks.begin(); it != activeChunks.end(); ) {
        if (!isWithinRadius(it->first.coords, playerRegion.coords, chunkRadius)) {
            it->second.staleFrames++;

            if (it->second.staleFrames > 30) {
                it = activeChunks.erase(it);
                continue;
            }
        } else {
            it->second.staleFrames = 0;
        }

        ++it;
    }
}

std::vector<Tile> ChunkManager::collectRenderTiles(MapLayer layer) const {
    std::vector<Tile> tiles;
    std::lock_guard<std::mutex> lock(chunkMutex);

    for (const auto& [key, timedChunk] : activeChunks) {
        if (key.layer != currentLayer) continue;

        const Chunk& chunk = timedChunk.data;

        for (const auto& loc : chunk.locations) {
            if (!loc.visual) continue;

            tiles.push_back(Tile{
                chunk.coords.x * 16 + loc.visual->x,
                loc.visual->height,
                chunk.coords.y * 16 + loc.visual->y,
                loc.visual->r,
                loc.visual->g,
                loc.visual->b
            });
        }
    }

    return tiles;
}
