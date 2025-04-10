#include "worldgen/Noise.h"
#include <cmath>
#include <cstdlib>

// Simple placeholder noise (replace with real Perlin/Worley)
float perlinNoise(float x, float y) {
    return 0.5f * (std::sin(x * 0.1f) + std::cos(y * 0.1f));
}

float worleyNoise(float x, float y) {
    float cx = std::floor(x);
    float cy = std::floor(y);
    float fx = x - cx;
    float fy = y - cy;
    return std::min(fx, fy);  // Placeholder: returns distance to corner
}
