#pragma once

#include <vector>
#include <unordered_set>
#include <random>
#include "math/Vec2.h"
#include "worldgen/Noise.h"
#include "worldgen/WorldTypes.h"
#include "worldgen/Location.h"

struct CaveTile {
    float height;  // e.g. for lava, water, or lighting
    float r, g, b;  // colors for rendering
    int x, y;
    bool solid;
};

struct CaveNetwork {
    std::string id;
    Math::Vec2i entranceRegion;
    int size = 1;
    bool hasBoss = false;

    std::vector<Math::Vec2i> connectedRegions;
    std::vector<std::string> connectedEntrances;
    std::vector<std::vector<CaveTile>> caveGrid;
};

namespace CaveGenerator {
    int rollCaveNetworkSize(const std::string& id);
    std::vector<Math::Vec2i> generateCaveLayout(const Math::Vec2i& start, int size, size_t seed);
    std::vector<std::vector<CaveTile>> generateCaveGrid(const Math::Vec2i& regionCoords, unsigned int seed, int width, int height);
    std::vector<Location> generateUndergroundRegionWithSeed(const RegionKey& key, unsigned int seed);
}


