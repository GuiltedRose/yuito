#include "render/CaveMeshGenerator.h"
#include "render/Vertex.h"

namespace CaveMeshGenerator {

    CaveMesh generateCaveMesh(const std::vector<std::vector<CaveTile>>& grid, float tileSize) {
        CaveMesh mesh;
        int height = static_cast<int>(grid.size());
        if (height == 0) return mesh;
        int width = static_cast<int>(grid[0].size());

        const float half = tileSize / 2.0f;

        for (int y = 0; y < height; ++y) {
            for (int x = 0; x < width; ++x) {
                const CaveTile& tile = grid[y][x];
                if (tile.solid) continue;

                float baseX = static_cast<float>(x) * tileSize;
                float baseY = tile.height * tileSize;
                float baseZ = static_cast<float>(y) * tileSize;

                Vec3 corners[8] = {
                    { baseX - half, baseY,     baseZ - half },
                    { baseX + half, baseY,     baseZ - half },
                    { baseX + half, baseY,     baseZ + half },
                    { baseX - half, baseY,     baseZ + half },
                    { baseX - half, baseY + tileSize, baseZ - half },
                    { baseX + half, baseY + tileSize, baseZ - half },
                    { baseX + half, baseY + tileSize, baseZ + half },
                    { baseX - half, baseY + tileSize, baseZ + half },
                };

                // Define cube faces (each 2 triangles)
                unsigned int baseIndex = static_cast<unsigned int>(mesh.vertices.size());

                auto pushFace = [&](int i0, int i1, int i2, int i3) {
                    mesh.indices.insert(mesh.indices.end(), {
                        baseIndex + i0, baseIndex + i1, baseIndex + i2,
                        baseIndex + i0, baseIndex + i2, baseIndex + i3
                    });
                };

                // Add vertices
                for (const auto& v : corners) {
                    mesh.vertices.push_back(Vertex {
                        .position = v,
                        .normal = {0, 1, 0}, // placeholder
                        .color = {tile.r, tile.g, tile.b}
                    });
                }

                // Add 6 faces (top, bottom, sides)
                pushFace(4, 5, 6, 7); // top
                pushFace(0, 1, 2, 3); // bottom
                pushFace(0, 4, 7, 3); // left
                pushFace(1, 5, 6, 2); // right
                pushFace(3, 7, 6, 2); // front
                pushFace(0, 4, 5, 1); // back
            }
        }

        return mesh;
    }
}
