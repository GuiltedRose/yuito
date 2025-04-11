#include "worldgen/WorldGenerator.h"
#include <sstream>
#include <algorithm>
#include <random>
#include <cmath> // for std::lerp if using C++20; otherwise implement manually
#include <iostream>

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

float WorldGenerator::hashToFloat(const std::string& key, float min, float max) {
    std::hash<std::string> hasher;
    size_t hash = hasher(key);
    float t = static_cast<float>(hash % 10000) / 10000.0f;
    return min + t * (max - min);
}

float WorldGenerator::hybridNoise(float x, float y) {
    float baseX = (x + 1000.0f) * 0.03f;
    float baseY = (y + 2000.0f) * 0.03f;

    float detailX = (x + 42.0f) * 0.15f;
    float detailY = (y - 77.0f) * 0.15f;

    float erosionX = (x - 88.0f) * 0.01f;  // Lower frequency
    float erosionY = (y + 57.0f) * 0.01f;

    float base = perlinNoise(baseX, baseY);     // [0, 1]
    float detail = perlinNoise(detailX, detailY); // [0, 1]
    float erosion = worleyNoise(erosionX, erosionY); // [0, 1]

    // Center around 0.5 with better spread
    float height = 0.6f * base + 0.3f * detail;

    // Optional: subtract erosion to add valleys
    height *= (0.7f + 0.3f * (1.0f - erosion));

    return std::clamp(height, 0.0f, 1.0f);
}

float WorldGenerator::smoothNoise(float x, float y) {
    // Smooth the noise using the default constant for smoothFactor
    float topLeft = hybridNoise(x - 1.0f, y - 1.0f);
    float topRight = hybridNoise(x + 1.0f, y - 1.0f);
    float bottomLeft = hybridNoise(x - 1.0f, y + 1.0f);
    float bottomRight = hybridNoise(x + 1.0f, y + 1.0f);

    // Average neighboring values and return smoothed noise
    return std::lerp(std::lerp(topLeft, topRight, smoothFactor),
                     std::lerp(bottomLeft, bottomRight, smoothFactor), smoothFactor);
}

std::vector<Location> WorldGenerator::generateRegion(const Vec2i& regionCoords) {
    const int chunkSize = 16;
    std::vector<Location> locations;

    for (int y = 0; y < chunkSize; ++y) {
        for (int x = 0; x < chunkSize; ++x) {
            int index = y * chunkSize + x;

            // Stable ID and metadata
            std::string id = makeLocationID(regionCoords, index);
            std::string name = "Untamed Wilderness";
            std::string terrain = "wilds";
            std::string tag = "wilderness";
            std::string description = "An untamed area of the world.";

            // World-space coordinates
            float globalX = static_cast<float>(regionCoords.x * chunkSize + x);
            float globalY = static_cast<float>(regionCoords.y * chunkSize + y);

            // Apply smoothing to the height calculation
            float h = smoothNoise(globalX, globalY);
            float normHeight = std::clamp(h, 0.0f, 1.0f);

            // Declare visual data here
            auto visual = std::make_shared<VisualData>();

            // Terrain detection (mountains, rivers, caverns, etc.)
            bool isMountain = normHeight > 0.75f;
            bool isRiver = normHeight < 0.2f && std::abs(worleyNoise(globalX, globalY)) < 0.1f;
            bool isCavern = normHeight < 0.15f;
            bool isVolcanic = normHeight > 0.8f && worleyNoise(globalX, globalY) < 0.2f;  // Volcanic regions
            bool isSnowy = normHeight > 0.9f;

            // Handle mountain terrain
            if (isMountain) {
                terrain = "Mountain";
                tag = "ridge";
                name = "Craggy Mountains";
                description = "Jagged peaks rise into the clouds.";
                visual->r = 0.4f;
                visual->g = 0.4f;
                visual->b = 0.4f;
                visual->height = h * 16.0f; // Height exaggeration for mountains
            }

            // Handle rivers
            else if (isRiver) {
                terrain = "River";
                tag = "water";
                name = "Flowing River";
                description = "A river winding through the terrain, carrying fresh waters.";
                visual->r = 0.2f;
                visual->g = 0.5f;
                visual->b = 0.8f;  // River's blue tone
                visual->height = 0.0f; // River level at sea level
            }

            // Handle caverns
            else if (isCavern) {
                terrain = "Cavern";
                tag = "dark";
                name = "Underground Cavern";
                description = "A dark and damp cavern, home to ancient creatures.";
                visual->r = 0.1f;
                visual->g = 0.1f;
                visual->b = 0.2f;  // Dark tone for caverns
                visual->height = h * 2.0f; // Low exaggeration for underground
            }

            // Handle volcanic terrain
            else if (isVolcanic) {
                terrain = "Volcanic Region";
                tag = "lava";
                name = "Fiery Volcano";
                description = "A land scorched by lava and volcanic ash.";
                visual->r = 0.8f;
                visual->g = 0.2f;
                visual->b = 0.0f;  // Lava color
                visual->height = h * 20.0f; // Higher exaggeration for volcanic regions
            }

            // Handle snowy regions
            else if (isSnowy) {
                terrain = "Snowy Peak";
                tag = "snow";
                name = "Frozen Summit";
                description = "Snow-covered peaks high above the world.";
                visual->r = 0.9f;
                visual->g = 0.9f;
                visual->b = 1.0f;  // Snowy peaks
                visual->height = h * 16.0f; // Height exaggeration for snow peaks
            }

            // Default terrain handling
            else {
                setColorForHeight(normHeight, visual->r, visual->g, visual->b);
                visual->height = h * 12.0f;  // Default height exaggeration
            }

            // Set position of the location
            visual->x = static_cast<float>(x);
            visual->y = static_cast<float>(y);

            // Push location into the region
            locations.push_back(Location{
                id, name, description, terrain, { tag }, {}, visual
            });
        }
    }

    // Add the generated region to the region map
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

        // Try linking similar terrain types together
        if (!thisRegion.empty() && !otherRegion.empty()) {
            for (auto& loc : thisRegion) {
                if (loc.terrainType == "Mountain") {
                    // Link to nearby plains, forest, or river
                    otherRegion[0].connections.push_back(loc.id);
                }
            }
        }
    }
}

const std::unordered_map<Vec2i, std::vector<Location>, Vec2i::Hash>& WorldGenerator::getWorld() const {
    return regionMap;
}
