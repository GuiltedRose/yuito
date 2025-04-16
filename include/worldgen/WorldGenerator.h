#pragma once

#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <random>
#include <memory>
#include "math/Vec2.h"
#include "worldgen/Noise.h"
#include "worldgen/WorldTypes.h"
#include "worldgen/Location.h"
#include "worldgen/CaveGenerator.h"
#include "render/RenderTypes.h"

class WorldGenerator {
    public:
        WorldGenerator(unsigned int seed, float smoothFactor = 0.5f);
    
        std::string makeLocationID(const Math::Vec2i& region, int index);
        std::string pickRandom(const std::vector<std::string>& list);
    
        float hashToFloat(const std::string& key, float min, float max);
    
        std::vector<Location> generateRegion(const Math::Vec2i& regionCoords);
        std::vector<Location> generateRegionWithSeed(const Math::Vec2i& regionCoords, unsigned int trialSeed);
        std::vector<Location> generateUndergroundRegionWithSeed(const RegionKey& key, unsigned int seed);
    
        bool regionIsTooMountainy(const std::vector<Location>& region);
    
        void applyGaussianSmoothingPadded(std::vector<std::shared_ptr<VisualData>>& grid, int paddedSize);
        void stitchHeights(std::shared_ptr<VisualData>& a, std::shared_ptr<VisualData>& b);
    
        void linkAdjacentRegions(const RegionKey& currentKey);
        const std::unordered_map<RegionKey, std::vector<Location>, std::hash<RegionKey>>& getLayeredWorld() const;
    
    private:
        unsigned int seed;
        float smoothFactor;
        std::mt19937 rng;
    
        std::unordered_map<RegionKey, std::vector<Location>, std::hash<RegionKey>> layeredRegionMap;
        std::unordered_map<std::string, CaveNetwork> caveNetworks;
    };  
