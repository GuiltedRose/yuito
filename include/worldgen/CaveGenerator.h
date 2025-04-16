#pragma once

#include <vector>
#include <unordered_set>
#include <random>
#include "math/Vec2.h"
#include "worldgen/Noise.h"
#include "worldgen/WorldTypes.h"
#include "worldgen/Location.h"
#include "worldgen/WorldTypes.h"
#include "render/CaveMeshGenerator.h"

struct CaveNetwork {
    std::string id;
    Math::Vec2i entranceRegion;
    int size = 1;
    bool hasBoss = false;

    std::vector<Math::Vec2i> connectedRegions;
    std::vector<std::string> connectedEntrances;
    std::vector<std::vector<CaveTile>> caveGrid;

    CaveMeshGenerator::CaveMesh mesh; 
};

namespace CaveGenerator {
    inline static std::unordered_map<RegionKey, std::vector<std::vector<CaveTile>>, std::hash<RegionKey>> tempCache;
    
    int rollCaveNetworkSize(const std::string& id);
    std::vector<Math::Vec2i> generateCaveLayout(const Math::Vec2i& start, int size, size_t seed);

    std::vector<std::vector<CaveTile>> generateCaveGrid(
        const Math::Vec2i& regionCoords,
        unsigned int seed,
        int width,
        int height,
        const std::vector<Math::Vec2i>& layout
    );

    CaveMeshGenerator::CaveMesh getCachedMeshForRegion(const RegionKey& key);

    std::vector<Location> generateUndergroundRegionWithSeed(const RegionKey& key, unsigned int seed);
}


