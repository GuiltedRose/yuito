#pragma once

#include <vector>
#include "render/Vertex.h"
#include "math/Vec2.h"
#include "math/Vec3.h"
#include "worldgen/WorldTypes.h"

namespace CaveMeshGenerator {
    struct CaveMesh {
        std::vector<Vertex> vertices;
        std::vector<unsigned int> indices;
    };

    CaveMesh generateCaveMesh(const std::vector<std::vector<CaveTile>>& grid, float tileSize = 1.0f);
}
