#include "worldgen/Noise.h"
#include <cmath>
#include <algorithm>
#include <random>

static unsigned int hash(int x, int y, int seed) {
    unsigned int h = seed;
    h ^= x * 374761393u;
    h ^= y * 668265263u;
    h = (h ^ (h >> 13)) * 1274126177u;
    return h;
}

static Vec2 randomPointInCell(int cx, int cy, int seed) {
    std::mt19937 rng(hash(cx, cy, seed));
    std::uniform_real_distribution<float> dist(0.0f, 1.0f);
    return Vec2(float(cx) + dist(rng), float(cy) + dist(rng));
}

float worleyNoise(float x, float y, int seed) {
    int xi = int(std::floor(x));
    int yi = int(std::floor(y));
    float minDist = 1e10f;

    for (int dx = -1; dx <= 1; ++dx) {
        for (int dy = -1; dy <= 1; ++dy) {
            Vec2 pt = randomPointInCell(xi + dx, yi + dy, seed);
            float dist = std::sqrt((pt.x - x) * (pt.x - x) + (pt.y - y) * (pt.y - y));
            if (dist < minDist) minDist = dist;
        }
    }

    return std::clamp(minDist, 0.0f, 1.0f);
}

Vec2 getVoronoiCellCenter(float x, float y, int seed) {
    int cx = int(std::floor(x));
    int cy = int(std::floor(y));
    return randomPointInCell(cx, cy, seed);
}

float getNoise(NoiseType type, float x, float y, int seed) {
    switch (type) {
        case NoiseType::Worley:
            return worleyNoise(x, y, seed);
        case NoiseType::Voronoi:

            {
                Vec2 center = getVoronoiCellCenter(x, y, seed);
                float dx = x - center.x;
                float dy = y - center.y;
                return std::clamp(std::sqrt(dx * dx + dy * dy), 0.0f, 1.0f);
            }
        default:
            return 0.0f;
    }
}
