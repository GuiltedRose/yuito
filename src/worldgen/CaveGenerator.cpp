#include "worldgen/CaveGenerator.h"
#include <algorithm>
#include <cmath>

int CaveGenerator::rollCaveNetworkSize(const std::string& id) {
    return static_cast<int>((std::hash<std::string>{}(id) % 15) + 1); // 1 to 15
}

std::vector<Math::Vec2i> CaveGenerator::generateCaveLayout(const Math::Vec2i& start, int size, size_t seed) {
    std::vector<Math::Vec2i> layout = { start };
    std::unordered_set<Math::Vec2i, std::hash<Math::Vec2i>> visited = { start };
    std::vector<Math::Vec2i> frontier = { start };
    std::mt19937 rng(seed);

    const std::vector<Math::Vec2i> dirs = { {1,0}, {-1,0}, {0,1}, {0,-1} };

    while (!frontier.empty() && layout.size() < static_cast<size_t>(size)) {
        std::uniform_int_distribution<int> dist(0, static_cast<int>(frontier.size()) - 1);
        Math::Vec2i current = frontier[dist(rng)];

        auto shuffled = dirs;
        std::shuffle(shuffled.begin(), shuffled.end(), rng);

        for (const auto& dir : shuffled) {
            Math::Vec2i next = { current.x + dir.x, current.y + dir.y };
            if (visited.count(next)) continue;

            layout.push_back(next);
            visited.insert(next);
            frontier.push_back(next);
            break;
        }

        frontier.erase(std::remove(frontier.begin(), frontier.end(), current), frontier.end());
    }

    return layout;
}

std::vector<std::vector<CaveTile>> CaveGenerator::generateCaveGrid(const Math::Vec2i& regionCoords, unsigned int seed, int width, int height) {
    std::vector<std::vector<CaveTile>> grid(height, std::vector<CaveTile>(width));

    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            float gx = regionCoords.x * width + x;
            float gy = regionCoords.y * height + y;
            float n = getNoise(NoiseType::Worley, gx * 0.1f, gy * 0.1f, seed);

            grid[y][x] = CaveTile{
                .height = n,
                .r = 0.1f,
                .g = 0.1f,
                .b = 0.15f,
                .x = x,
                .y = y,
                .solid = n > 0.5f
            };
        }
    }

    return grid;
}

std::vector<Location> CaveGenerator::generateUndergroundRegionWithSeed(const RegionKey& key, unsigned int seed) {
    const int chunkSize = 16;
    std::vector<Location> locations;

    auto caveGrid = generateCaveGrid(key.coords, seed, chunkSize, chunkSize);

    for (int y = 0; y < chunkSize; ++y) {
        for (int x = 0; x < chunkSize; ++x) {
            int index = y * chunkSize + x;
            std::string id = "cave_r" + std::to_string(key.coords.x) + "_" + std::to_string(key.coords.y) + "_loc" + std::to_string(index);
            const CaveTile& tile = caveGrid[y][x];

            auto visual = std::make_shared<VisualData>();
            visual->x = static_cast<float>(x);
            visual->y = static_cast<float>(y);
            visual->height = tile.solid ? 0.0f : tile.height * 6.0f;
            visual->r = tile.r;
            visual->g = tile.g;
            visual->b = tile.b;

            locations.push_back(Location{
                id,
                "Cave Chamber",
                tile.solid ? "Collapsed tunnel" : "Spacious underground room",
                tile.solid ? "rock" : "cave",
                { "underground", tile.solid ? "solid" : "hollow" },
                {},
                visual
            });
        }
    }

    return locations;
}
