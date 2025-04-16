#include "worldgen/ChunkManager.h"
#include <algorithm>
#include <iostream>

ChunkManager::ChunkManager(WorldGenerator& generator, unsigned int seed)
    : generator(generator), seed(seed), playerRegion({{0, 0}, MapLayer::Surface}) 
{
    workerThread = std::thread([this, &generator, &seed]() {
        while (running) {
            RegionKey key;

            {
                std::unique_lock<std::mutex> lock(chunkMutex);
                chunkCV.wait(lock, [this]() {
                    return !chunkLoadQueue.empty() || !running;
                });
                if (!running) break;

                key = chunkLoadQueue.front();
                chunkLoadQueue.pop();
            }

            const Math::Vec2i& coords = key.coords;
            MapLayer layer = key.layer;

            std::vector<Location> locations;
            std::optional<CaveMeshGenerator::CaveMesh> caveMesh = std::nullopt;

            if (layer == MapLayer::Underground) {
                // Step 1: generate cave tiles (locations only)
                locations = generator.generateUndergroundRegionWithSeed(key, seed);
                // Step 2: generate cave mesh using cached layout/grid
                caveMesh = CaveGenerator::getCachedMeshForRegion(key);
            } else {
                locations = generator.generateRegionWithSeed(coords, seed);
            }

            {
                std::lock_guard<std::mutex> lock(chunkMutex);
                activeChunks[key] = TimedChunk{
                    Chunk{ coords, locations },
                    0,
                    caveMesh
                };
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

    // Optional: remove all non-matching layer chunks
    for (auto it = activeChunks.begin(); it != activeChunks.end(); ) {
        if (it->first.layer != currentLayer) {
            it = activeChunks.erase(it);
        } else {
            ++it;
        }
    }
}

Chunk* ChunkManager::getChunk(const Math::Vec2i& coords, MapLayer layer) {
    if (layer != currentLayer) return nullptr;  // Skip chunks from other layers

    RegionKey key{ coords, layer };
    std::lock_guard<std::mutex> lock(chunkMutex);

    auto it = activeChunks.find(key);
    if (it != activeChunks.end()) {
        return &it->second.data;
    }

    std::vector<Location> locations;
    if (layer == MapLayer::Underground) {
        locations = generator.generateUndergroundRegionWithSeed(key, seed);
    } else {
        locations = generator.generateRegionWithSeed(coords, seed);
    }

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

    for (int dy = -chunkRadius; dy <= chunkRadius; ++dy) {
        for (int dx = -chunkRadius; dx <= chunkRadius; ++dx) {
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
        if (key.layer != layer) continue;

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

std::vector<CaveMeshGenerator::CaveMesh> ChunkManager::collectCaveMeshes() const {
    std::vector<CaveMeshGenerator::CaveMesh> result;
    std::lock_guard<std::mutex> lock(chunkMutex);

    for (const auto& [key, timedChunk] : activeChunks) {
        if (key.layer != MapLayer::Underground) continue;

        if (timedChunk.caveMesh.has_value() && !timedChunk.caveMesh->vertices.empty()) {
            result.push_back(*timedChunk.caveMesh);
        }
    }

    return result;
}

const std::unordered_map<RegionKey, TimedChunk>& ChunkManager::getActiveChunks() const {
    std::lock_guard<std::mutex> lock(chunkMutex);
    return activeChunks;
}
