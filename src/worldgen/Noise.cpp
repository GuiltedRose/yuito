#include "worldgen/Noise.h"
#include <cmath>
#include <cstdlib>
#include <algorithm>

// Simple placeholder noise (replace with real Perlin/Worley)
float perlinNoise(float x, float y) {
    float raw = std::sin(x * 0.1f) + std::cos(y * 0.1f); // [-2, 2]
    return (raw + 2.0f) / 4.0f; // Normalize to [0, 1]
}

float worleyNoise(float x, float y) {
    float cx = std::floor(x);
    float cy = std::floor(y);
    float fx = x - cx;
    float fy = y - cy;
    return std::min(fx, fy);  // returns [0,1) if coords are clean
}

void setColorForHeight(float h, float& r, float& g, float& b) {
    if (h < 0.28f) {
        // Deep water
        r = 0.1f; g = 0.2f; b = 0.5f;
    } else if (h < 0.48f) {
        float t = std::clamp((h - 0.28f) / 0.20f, 0.0f, 1.0f);  // water to sand
        r = std::lerp(0.1f, 0.76f, t);
        g = std::lerp(0.2f, 0.7f, t);
        b = std::lerp(0.5f, 0.5f, t);
    } else if (h < 0.68f) {
        float t = std::clamp((h - 0.48f) / 0.20f, 0.0f, 1.0f);  // sand to grass
        r = std::lerp(0.76f, 0.1f, t);
        g = std::lerp(0.7f, 0.6f, t);
        b = std::lerp(0.5f, 0.2f, t);
    } else if (h < 0.86f) {
        float t = std::clamp((h - 0.68f) / 0.18f, 0.0f, 1.0f);  // grass to rock
        r = std::lerp(0.1f, 0.4f, t);
        g = std::lerp(0.6f, 0.4f, t);
        b = std::lerp(0.2f, 0.4f, t);
    } else {
        float t = std::clamp((h - 0.86f) / 0.14f, 0.0f, 1.0f);  // rock to snow
        r = std::lerp(0.4f, 0.9f, t);
        g = std::lerp(0.4f, 0.9f, t);
        b = std::lerp(0.4f, 1.0f, t);
    }
}
