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

std::vector<Location> WorldGenerator::generateRegion(const Vec2i& regionCoords) {
    const int locationsPerRegion = 5;

    std::vector<Location> locations;
    for (int i = 0; i < locationsPerRegion; ++i) {
        std::string id = makeLocationID(regionCoords, i);
        std::string terrain = pickRandom(terrainTypes);
        std::string tag = pickRandom(tags);
        std::string name = tag + " " + terrain;
        std::string description = pickRandom(loreHooks);

        locations.push_back(Location{
            id, name, description, terrain, { tag }, {}
        });
    }

    // Connect locations in sequence
    for (int i = 1; i < locationsPerRegion; ++i) {
        locations[i - 1].connections.push_back(locations[i].id);
        locations[i].connections.push_back(locations[i - 1].id);
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
