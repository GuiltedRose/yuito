#include "worldgen/GLWidget.h"
#include "worldgen/ChunkManager.h"

ChunkManager::ChunkManager(WorldGenerator& generator, unsigned int seed)
    : generator(generator), seed(seed), playerRegion({0, 0}) {

    workerThread = std::thread([this, &generator]() {
        while (running) {
            Vec2i region;

            {
                std::unique_lock<std::mutex> lock(chunkMutex);
                chunkCV.wait(lock, [this]() { return !chunkLoadQueue.empty() || !running; });
                if (!running) break;

                region = chunkLoadQueue.front();
                chunkLoadQueue.pop();
            }

            auto locs = generator.generateRegion(region);
            generator.linkAdjacentRegions(region);

            {
                std::lock_guard<std::mutex> lock(chunkMutex);
                activeChunks[region] = Chunk{ region, locs };
                enqueuedRegions.erase(region);
            }
        }
    });
}

Chunk* ChunkManager::getChunk(const Vec2i& coords) {
    std::lock_guard<std::mutex> lock(chunkMutex);

    auto it = activeChunks.find(coords);
    if (it != activeChunks.end()) {
        return &it->second;
    }

    Chunk newChunk;
    newChunk.coords = coords;
    newChunk.locations = generator.generateRegion(coords);
    activeChunks[coords] = std::move(newChunk);

    return &activeChunks[coords];
}

void ChunkManager::updatePlayerRegion(const Vec2i& region) {
    playerRegion = region;

    for (int dy = -chunkRadius; dy <= chunkRadius; ++dy) {
        for (int dx = -chunkRadius; dx <= chunkRadius; ++dx) {
            Vec2i coords = { region.x + dx, region.y + dy };

            std::lock_guard<std::mutex> lock(chunkMutex);
            if (activeChunks.find(coords) == activeChunks.end() &&
                enqueuedRegions.find(coords) == enqueuedRegions.end()) {
                chunkLoadQueue.push(coords);
                enqueuedRegions.insert(coords);
                chunkCV.notify_one();
            }
        }
    }

    unloadFarChunks();
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

bool ChunkManager::isWithinRadius(const Vec2i& a, const Vec2i& b, int radius) {
    int dx = a.x - b.x;
    int dy = a.y - b.y;
    return (dx * dx + dy * dy) <= (radius * radius);
}

void ChunkManager::unloadFarChunks() {
    std::vector<Vec2i> toErase;
    for (const auto& [coords, _] : activeChunks) {
        if (!isWithinRadius(coords, playerRegion, chunkRadius)) {
            toErase.push_back(coords);
        }
    }

    for (const auto& coords : toErase) {
        activeChunks.erase(coords);
    }
}

std::vector<Tile> ChunkManager::collectRenderTiles() const {
    std::vector<Tile> tiles;
    std::lock_guard<std::mutex> lock(chunkMutex);

    for (const auto& [coords, chunk] : activeChunks) {
        for (const auto& loc : chunk.locations) {
            if (!loc.visual) continue;

            tiles.push_back(Tile{
                coords.x * 16 + loc.visual->x,  // X = chunkX + localX
                loc.visual->height,             // Y = height
                coords.y * 16 + loc.visual->y,  // Z = chunkY + localY
                loc.visual->r,
                loc.visual->g,
                loc.visual->b
            });
        }
    }

    return tiles;
}
