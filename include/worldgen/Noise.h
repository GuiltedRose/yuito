#pragma once

#include "math/Vec2.h"

enum class NoiseType {
    Worley,
    Voronoi
};

float getNoise(NoiseType type, float x, float y, int seed);

Vec2 getVoronoiCellCenter(float x, float y, int seed);
