#include "worldgen/ChunkManager.h"
#include "worldgen/GLWidget"

ChunkManager::ChunkManager(WorldGenerator& gen, unsigned int seed)
    : generator(gen), seed(seed) {}

Chunk* ChunkManager::getChunk(const Vec2i& coords) {
    auto it = activeChunks.find(coords);
    if (it != activeChunks.end()) {
        return &it->second;
    }

    // Generate on demand
    Chunk newChunk;
    newChunk.coords = coords;
    newChunk.locations = generator.generateRegion(coords);
    activeChunks[coords] = std::move(newChunk);

    return &activeChunks[coords];
}

void ChunkManager::updatePlayerRegion(const Vec2i& region) {
    playerRegion = region;

    for (int dx = -chunkRadius; dx <= chunkRadius; ++dx) {
        for (int dy = -chunkRadius; dy <= chunkRadius; ++dy) {
            Vec2i coords = { region.x + dx, region.y + dy };
            getChunk(coords); // load into memory
        }
    }

    unloadFarChunks();
}

bool ChunkManager::isWithinRadius(const Vec2i& a, const Vec2i& b, int radius) {
    int dx = a.x - b.x;
    int dy = a.y - b.y;
    return (dx * dx + dy * dy) <= (radius * radius);
}

void ChunkManager::unloadFarChunks() {
    std::vector<Vec2i> toUnload;

    for (const auto& [coords, chunk] : activeChunks) {
        if (!isWithinRadius(coords, playerRegion, chunkRadius)) {
            toUnload.push_back(coords);
        }
    }

    for (const Vec2i& coords : toUnload) {
        activeChunks.erase(coords);
    }
}

std::vector<Tile> ChunkManager::collectRenderTiles() const {
    std::vector<Tile> result;

    for (const auto& [regionCoords, chunk] : activeChunks) {
        const auto& locs = chunk.locations;
        for (size_t i = 0; i < locs.size(); ++i) {
            result.push_back(fromLocation(locs[i], regionCoords, static_cast<int>(i)));
        }
    }

    return result;
}
