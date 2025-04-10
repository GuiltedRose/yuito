#include "worldgen/WorldGenerator.h"
#include <sstream>
#include <algorithm>
#include <random>

WorldGenerator::WorldGenerator(unsigned int seed) : rng(seed) {}

std::string WorldGenerator::makeLocationID(const Vec2i& region, int index) {
    std::ostringstream oss;
    oss << "r" << region.x << "_" << region.y << "_loc" << index;
    return oss.str();
}

std::string WorldGenerator::pickRandom(const std::vector<std::string>& list) {
    std::uniform_int_distribution<int> dist(0, static_cast<int>(list.size()) - 1);
    return list[dist(rng)];
}

float WorldGenerator::hybridNoise(float x, float y) {
    float perlin = perlinNoise(x * 0.1f, y * 0.1f);     // smooth terrain shape
    float worley = worleyNoise(x * 0.05f, y * 0.05f);   // region clustering
    
    // Worley gives distance; invert it to get height
    float worleyHeight = 1.0f - worley;

    // Blend them
    float blend = 0.6f * perlin + 0.4f * worleyHeight;
    return blend;
}


std::vector<Location> WorldGenerator::generateRegion(const Vec2i& regionCoords) {
    const int chunkSize = 16;
    std::vector<Location> locations;

    for (int y = 0; y < chunkSize; ++y) {
        for (int x = 0; x < chunkSize; ++x) {
            int index = y * chunkSize + x;
            std::string id = makeLocationID(regionCoords, index);
            std::string terrain = pickRandom(terrainTypes);
            std::string tag = pickRandom(tags);
            std::string name = tag + " " + terrain;
            std::string description = pickRandom(loreHooks);

            // 👇 Use global world coordinates for seamless noise
            float globalX = regionCoords.x * chunkSize + x;
            float globalY = regionCoords.y * chunkSize + y;
            float h = hybridNoise(globalX, globalY);

            auto visual = std::make_shared<VisualData>();
            visual->x = x;
            visual->y = y;
            visual->height = 0.4f + h * 0.6f; // scale height

            visual->r = 0.2f + h * 0.6f;
            visual->g = 0.4f + h * 0.5f;
            visual->b = 0.2f + h * 0.4f;

            locations.push_back(Location{
                id, name, description, terrain, { tag }, {}, visual
            });
        }
    }

    regionMap[regionCoords] = locations;
    return locations;
}



void WorldGenerator::linkAdjacentRegions(const Vec2i& regionCoords) {
    static const std::vector<Vec2i> offsets = {
        {1, 0}, {-1, 0}, {0, 1}, {0, -1}
    };

    for (const auto& offset : offsets) {
        Vec2i neighbor = { regionCoords.x + offset.x, regionCoords.y + offset.y };
        if (regionMap.find(neighbor) == regionMap.end()) continue;

        auto& thisRegion = regionMap[regionCoords];
        auto& otherRegion = regionMap[neighbor];

        if (!thisRegion.empty() && !otherRegion.empty()) {
            thisRegion[0].connections.push_back(otherRegion[0].id);
            otherRegion[0].connections.push_back(thisRegion[0].id);
        }
    }
}

const std::unordered_map<Vec2i, std::vector<Location>, Vec2i::Hash>& WorldGenerator::getWorld() const {
    return regionMap;
}
