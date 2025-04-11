#pragma once

#include <string>
#include <unordered_map>
#include <vector>
#include <random>
#include <memory>
#include "worldgen/Noise.h"

struct Tile;

struct VisualData {
    float x, y, height;
    float r, g, b;
};

struct Vec2i {
    int x;
    int y;

    bool operator==(const Vec2i& other) const {
        return x == other.x && y == other.y;
    }

    struct Hash {
        std::size_t operator()(const Vec2i& v) const {
            return std::hash<int>()(v.x) ^ (std::hash<int>()(v.y) << 1);
        }
    };
};

struct Location {
    std::string id;
    std::string name;
    std::string description;
    std::string terrainType;
    std::vector<std::string> tags;
    std::vector<std::string> connections;

    std::shared_ptr<VisualData> visual;
};

class WorldGenerator {
public:
    WorldGenerator(unsigned int seed);
    
    static constexpr float smoothFactor = 0.5f;

    float hybridNoise(float x, float y);
    float smoothNoise(float x, float y);

    float hashToFloat(const std::string& key, float min, float max);

    std::vector<Location> generateRegion(const Vec2i& regionCoords);
    void linkAdjacentRegions(const Vec2i& regionCoords);

    const std::unordered_map<Vec2i, std::vector<Location>, Vec2i::Hash>& getWorld() const;

private:
    std::mt19937 rng;
    std::unordered_map<Vec2i, std::vector<Location>, Vec2i::Hash> regionMap;

    std::vector<std::string> terrainTypes = {
        "Forest", "Mountains", "Plains", "Swamp", "Tundra", "Desert", "Coast"
    };

    std::vector<std::string> tags = {
        "ancient", "haunted", "sacred", "blighted", "abandoned", "prosperous"
    };

    std::vector<std::string> loreHooks = {
        "This was once a battleground of forgotten empires.",
        "The stones whisper names of the dead.",
        "Old magic pulses beneath your feet.",
        "Locals fear this place, though no one remembers why.",
        "A ruined monument stands here, worn by centuries."
    };

    std::string makeLocationID(const Vec2i& region, int index);
    std::string pickRandom(const std::vector<std::string>& list);

    float perlin;
    float worley;
};
