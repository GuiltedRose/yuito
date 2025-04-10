#pragma once

#include <vector>

enum class PrimitiveShape {
    RoundedCube,
    Capsule,
    Sphere
};

void appendSphereVerts(std::vector<float>& out, int rings = 16, int sectors = 32);
void appendRoundedCubeVerts(std::vector<float>& out);
void appendCapsuleVerts(std::vector<float>& out, int rings = 8, int sectors = 16, float height = 1.0f);

// void appendCapsuleVerts(std::vector<float>& out);   // future
