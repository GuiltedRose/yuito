#pragma once

#include "math/Vec2.h"

enum class NoiseType {
    Worley,
    Voronoi
};

/// Returns a normalized noise value (0.0 to 1.0) for a given type
float getNoise(NoiseType type, float x, float y, int seed);

/// Returns the coordinate of the Voronoi cell center that (x, y) falls into
Vec2 getVoronoiCellCenter(float x, float y, int seed);
