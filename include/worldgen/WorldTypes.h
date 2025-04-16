#pragma once

#include "math/Vec2.h"

enum class MapLayer {
    Surface,
    Underground
};

struct RegionKey {
    Math::Vec2i coords;
    MapLayer layer;

    bool operator==(const RegionKey& other) const {
        return coords == other.coords && layer == other.layer;
    }
};

namespace std {
    template<>
    struct hash<RegionKey> {
        std::size_t operator()(const RegionKey& key) const {
            return std::hash<int>()(key.coords.x) ^ 
                   (std::hash<int>()(key.coords.y) << 1) ^
                   (std::hash<int>()(static_cast<int>(key.layer)) << 2);
        }
    };
}

struct CaveTile {
    float height;  // e.g. for lava, water, or lighting
    float r, g, b;  // colors for rendering
    int x, y;
    bool solid;
};
