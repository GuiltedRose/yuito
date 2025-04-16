#include "worldgen/WorldGenerator.h"
#include "worldgen/Noise.h"
#include "worldgen/CaveGenerator.h"
#include <sstream>
#include <algorithm>
#include <random>
#include <cmath>
#include <iostream>

WorldGenerator::WorldGenerator(unsigned int seed, float smoothFactor)
    : seed(seed), rng(seed), smoothFactor(smoothFactor) {}

std::string WorldGenerator::makeLocationID(const Math::Vec2i& region, int index) {
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

std::vector<Location> WorldGenerator::generateRegion(const Math::Vec2i& regionCoords) {
    const std::vector<int> seedOffsets = {0, 7, 13, 42};
    for (int offset : seedOffsets) {
        unsigned int nudgedSeed = seed + offset;
        auto region = generateRegionWithSeed(regionCoords, nudgedSeed);
        if (!regionIsTooMountainy(region)) {
            RegionKey key = {regionCoords, MapLayer::Surface};
            layeredRegionMap[key] = region;
            return region;
        }
    }

    auto fallback = generateRegionWithSeed(regionCoords, seed + seedOffsets.back());
    RegionKey fallbackKey = {regionCoords, MapLayer::Surface};
    layeredRegionMap[fallbackKey] = fallback;
    return fallback;
}

bool WorldGenerator::regionIsTooMountainy(const std::vector<Location>& region) {
    int count = 0;
    for (const auto& loc : region)
        if (loc.terrainType == "Mountain") count++;

    return (static_cast<float>(count) / region.size()) > 0.35f;
}

void WorldGenerator::applyGaussianSmoothingPadded(std::vector<std::shared_ptr<VisualData>>& grid, int size) {
    auto copy = grid;
    const int weights[3][3] = {{1, 2, 1}, {2, 4, 2}, {1, 2, 1}};

    for (int y = 1; y < size - 1; ++y) {
        for (int x = 1; x < size - 1; ++x) {
            float sumH = 0, sumR = 0, sumG = 0, sumB = 0, weightSum = 0;

            for (int dy = -1; dy <= 1; ++dy) {
                for (int dx = -1; dx <= 1; ++dx) {
                    int w = weights[dy + 1][dx + 1];
                    auto& v = copy[(y + dy) * size + (x + dx)];
                    sumH += v->height * w;
                    sumR += v->r * w;
                    sumG += v->g * w;
                    sumB += v->b * w;
                    weightSum += w;
                }
            }

            auto& v = grid[y * size + x];
            v->height = sumH / weightSum;
            v->r = std::clamp(sumR / weightSum, 0.0f, 1.0f);
            v->g = std::clamp(sumG / weightSum, 0.0f, 1.0f);
            v->b = std::clamp(sumB / weightSum, 0.0f, 1.0f);
        }
    }
}

std::vector<Location> WorldGenerator::generateRegionWithSeed(const Math::Vec2i& regionCoords, unsigned int trialSeed) {
    const int chunkSize = 16;
    const int paddedSize = chunkSize + 2;
    std::vector<std::shared_ptr<VisualData>> visualGrid(paddedSize * paddedSize);
    std::vector<Location> locations;

    RegionKey surfaceKey = { regionCoords, MapLayer::Surface };

    // Step 1: Generate visual data with layered noise
    for (int y = 0; y < paddedSize; ++y) {
        for (int x = 0; x < paddedSize; ++x) {
            float globalX = static_cast<float>(regionCoords.x * chunkSize + (x - 1));
            float globalY = static_cast<float>(regionCoords.y * chunkSize + (y - 1));

            float base = getNoise(NoiseType::Worley, globalX * 0.01f, globalY * 0.01f, trialSeed);
            float detail = getNoise(NoiseType::Worley, globalX * 0.05f, globalY * 0.05f, trialSeed);
            float bumpiness = getNoise(NoiseType::Worley, globalX * 0.10f, globalY * 0.10f, trialSeed);
            float combined = base * 0.5f + detail * 0.3f + bumpiness * 0.2f;
            float shaped = std::pow(combined, 1.4f);

            Vec2 center = getVoronoiCellCenter(globalX * 0.05f, globalY * 0.05f, trialSeed);
            float dx = globalX * 0.05f - center.x;
            float dy = globalY * 0.05f - center.y;
            float dist = std::sqrt(dx * dx + dy * dy);
            float influence = std::exp(-dist * dist * 2.5f);

            float height = shaped * (1.0f - influence) + 0.3f * influence;
            height = std::clamp(height, 0.0f, 1.0f);

            auto visual = std::make_shared<VisualData>();
            visual->x = static_cast<float>(x - 1);
            visual->y = static_cast<float>(y - 1);
            visual->height = height * 12.0f;
            visual->r = std::clamp(0.2f + combined * 0.6f, 0.0f, 1.0f);
            visual->g = std::clamp(0.4f - dist * 0.3f, 0.0f, 1.0f);
            visual->b = std::clamp(0.3f + height * 0.5f, 0.0f, 1.0f);

            visualGrid[y * paddedSize + x] = visual;
        }
    }

    applyGaussianSmoothingPadded(visualGrid, paddedSize);

    // Step 2: Build region locations, mark cave entrances
    for (int y = 0; y < chunkSize; ++y) {
        for (int x = 0; x < chunkSize; ++x) {
            int visualIndex = (y + 1) * paddedSize + (x + 1);
            int locIndex = y * chunkSize + x;
            std::string id = makeLocationID(regionCoords, locIndex);

            float globalX = static_cast<float>(regionCoords.x * chunkSize + x);
            float globalY = static_cast<float>(regionCoords.y * chunkSize + y);
            auto visual = visualGrid[visualIndex];

            bool isSpecialSite = (std::hash<std::string>{}(id) % 12 == 0);
            float combined = getNoise(NoiseType::Worley, globalX * 0.05f, globalY * 0.05f, trialSeed);

            // Cave entrance placement only — no full cave generation here
            if (visual->height / 12.0f < 0.18f && combined < 0.15f && isSpecialSite) {
                std::string caveID = "cave_" + id;
                int size = CaveGenerator::rollCaveNetworkSize(caveID);

                CaveNetwork cave;
                cave.id = caveID;
                cave.entranceRegion = regionCoords;
                cave.size = size;
                cave.hasBoss = (size >= 10);
                cave.connectedEntrances.push_back(id);
                cave.connectedRegions = CaveGenerator::generateCaveLayout(regionCoords, size, std::hash<std::string>{}(caveID));

                caveNetworks[caveID] = cave;

                visual->r = 0.1f;
                visual->g = 0.1f;
                visual->b = 0.2f;
                visual->height = 0.1f * 12.0f;

                for (int oy = -2; oy <= 2; ++oy) {
                    for (int ox = -2; ox <= 2; ++ox) {
                        float dist = std::sqrt(ox * ox + oy * oy);
                        if (dist > 2.2f) continue;

                        int caveX = x + ox;
                        int caveY = y + oy;
                        int index = caveY * paddedSize + caveX;

                        if (index >= 0 && index < visualGrid.size()) {
                            auto& v = visualGrid[index];
                            v->height = 0.02f * 12.0f;
                            v->r = 0.1f;
                            v->g = 0.1f;
                            v->b = 0.2f;
                        }
                    }
                }

                std::cout << "[Cave Entrance] " << caveID << " @ " << regionCoords.x << "," << regionCoords.y
                          << " | size: " << size << (cave.hasBoss ? " [Boss]" : "") << "\n";
            }

            locations.push_back(Location{
                id, "Unknown", "Smoothed terrain", "wilds", { "wilderness" }, {}, visual
            });
        }
    }

    layeredRegionMap[surfaceKey] = locations;
    return locations;
}

std::vector<Location> WorldGenerator::generateUndergroundRegionWithSeed(const RegionKey& key, unsigned int seed) {
    return CaveGenerator::generateUndergroundRegionWithSeed(key, seed);
}

void WorldGenerator::stitchHeights(std::shared_ptr<VisualData>& a, std::shared_ptr<VisualData>& b) {
    float avgHeight = (a->height + b->height) * 0.5f;
    a->height = avgHeight;
    b->height = avgHeight;

    a->r = (a->r + b->r) * 0.5f;
    a->g = (a->g + b->g) * 0.5f;
    a->b = (a->b + b->b) * 0.5f;

    b->r = a->r;
    b->g = a->g;
    b->b = a->b;
}

void WorldGenerator::linkAdjacentRegions(const RegionKey& currentKey) {
    if (layeredRegionMap.find(currentKey) == layeredRegionMap.end()) return;
    auto& region = layeredRegionMap[currentKey];

    static const std::vector<Math::Vec2i> offsets = {
        {1, 0}, {-1, 0}, {0, 1}, {0, -1}
    };

    for (const auto& offset : offsets) {
        Math::Vec2i neighborCoords = {
            currentKey.coords.x + offset.x,
            currentKey.coords.y + offset.y
        };
        RegionKey neighborKey = { neighborCoords, currentKey.layer };

        if (layeredRegionMap.find(neighborKey) == layeredRegionMap.end()) continue;
        auto& neighbor = layeredRegionMap[neighborKey];

        if (offset.x == 1) {
            for (int y = 0; y < 16; ++y) {
                int a = y * 16 + 15;
                int b = y * 16 + 0;
                stitchHeights(region[a].visual, neighbor[b].visual);
            }
        } else if (offset.x == -1) {
            for (int y = 0; y < 16; ++y) {
                int a = y * 16 + 0;
                int b = y * 16 + 15;
                stitchHeights(region[a].visual, neighbor[b].visual);
            }
        } else if (offset.y == 1) {
            for (int x = 0; x < 16; ++x) {
                int a = 15 * 16 + x;
                int b = x;
                stitchHeights(region[a].visual, neighbor[b].visual);
            }
        } else if (offset.y == -1) {
            for (int x = 0; x < 16; ++x) {
                int a = x;
                int b = 15 * 16 + x;
                stitchHeights(region[a].visual, neighbor[b].visual);
            }
        }
    }
}

const std::unordered_map<RegionKey, std::vector<Location>, std::hash<RegionKey>>& WorldGenerator::getLayeredWorld() const {
    return layeredRegionMap;
}
