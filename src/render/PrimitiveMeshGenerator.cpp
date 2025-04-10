#include "render/PrimitiveMeshGenerator.h"
#include <cmath>
#include <array>

void appendSphereVerts(std::vector<float>& out, int rings, int sectors) {
    const float PI = 3.14159265f;

    for (int r = 0; r < rings; ++r) {
        float phi1 = PI * r / rings;
        float phi2 = PI * (r + 1) / rings;

        for (int s = 0; s < sectors; ++s) {
            float theta1 = 2 * PI * s / sectors;
            float theta2 = 2 * PI * (s + 1) / sectors;

            auto get = [](float phi, float theta) {
                float x = sinf(phi) * cosf(theta);
                float y = cosf(phi);
                float z = sinf(phi) * sinf(theta);
                return std::array<float, 3>{x, y, z};
            };

            auto v1 = get(phi1, theta1);
            auto v2 = get(phi2, theta1);
            auto v3 = get(phi2, theta2);
            auto v4 = get(phi1, theta2);

            // Triangle 1
            out.insert(out.end(), { v1[0], v1[1], v1[2] });
            out.insert(out.end(), { v2[0], v2[1], v2[2] });
            out.insert(out.end(), { v3[0], v3[1], v3[2] });

            // Triangle 2
            out.insert(out.end(), { v1[0], v1[1], v1[2] });
            out.insert(out.end(), { v3[0], v3[1], v3[2] });
            out.insert(out.end(), { v4[0], v4[1], v4[2] });
        }
    }
}

void appendCapsuleVerts(std::vector<float>& out, int rings, int sectors, float height) {
    const float PI = 3.14159265f;
    float half = height / 2.0f;

    auto get = [&](float phi, float theta, float yOffset) {
        float x = sinf(phi) * cosf(theta);
        float y = cosf(phi) + yOffset;
        float z = sinf(phi) * sinf(theta);
        return std::array<float, 3>{x, y, z};
    };

    // === Top Hemisphere ===
    for (int r = 0; r < rings / 2; ++r) {
        float phi1 = (PI / 2.0f) * r / (rings / 2);
        float phi2 = (PI / 2.0f) * (r + 1) / (rings / 2);

        for (int s = 0; s < sectors; ++s) {
            float theta1 = 2 * PI * s / sectors;
            float theta2 = 2 * PI * (s + 1) / sectors;

            auto v1 = get(phi1, theta1, half);
            auto v2 = get(phi2, theta1, half);
            auto v3 = get(phi2, theta2, half);
            auto v4 = get(phi1, theta2, half);

            out.insert(out.end(), { v1[0], v1[1], v1[2] });
            out.insert(out.end(), { v2[0], v2[1], v2[2] });
            out.insert(out.end(), { v3[0], v3[1], v3[2] });

            out.insert(out.end(), { v1[0], v1[1], v1[2] });
            out.insert(out.end(), { v3[0], v3[1], v3[2] });
            out.insert(out.end(), { v4[0], v4[1], v4[2] });
        }
    }

    // === Cylinder Body ===
    for (int s = 0; s < sectors; ++s) {
        float theta1 = 2 * PI * s / sectors;
        float theta2 = 2 * PI * (s + 1) / sectors;

        float x1 = cosf(theta1), z1 = sinf(theta1);
        float x2 = cosf(theta2), z2 = sinf(theta2);

        std::array<float, 3> a = {x1, -half, z1};
        std::array<float, 3> b = {x1,  half, z1};
        std::array<float, 3> c = {x2,  half, z2};
        std::array<float, 3> d = {x2, -half, z2};

        out.insert(out.end(), { a[0], a[1], a[2] });
        out.insert(out.end(), { b[0], b[1], b[2] });
        out.insert(out.end(), { c[0], c[1], c[2] });

        out.insert(out.end(), { a[0], a[1], a[2] });
        out.insert(out.end(), { c[0], c[1], c[2] });
        out.insert(out.end(), { d[0], d[1], d[2] });
    }

    // === Bottom Hemisphere (mirrored top) ===
    for (int r = 0; r < rings / 2; ++r) {
        float phi1 = (PI / 2.0f) * r / (rings / 2);
        float phi2 = (PI / 2.0f) * (r + 1) / (rings / 2);

        for (int s = 0; s < sectors; ++s) {
            float theta1 = 2 * PI * s / sectors;
            float theta2 = 2 * PI * (s + 1) / sectors;

            auto v1 = get(phi1, theta1, -half);
            auto v2 = get(phi2, theta1, -half);
            auto v3 = get(phi2, theta2, -half);
            auto v4 = get(phi1, theta2, -half);

            out.insert(out.end(), { v1[0], v1[1], v1[2] });
            out.insert(out.end(), { v3[0], v3[1], v3[2] });
            out.insert(out.end(), { v2[0], v2[1], v2[2] });

            out.insert(out.end(), { v1[0], v1[1], v1[2] });
            out.insert(out.end(), { v4[0], v4[1], v4[2] });
            out.insert(out.end(), { v3[0], v3[1], v3[2] });
        }
    }
}


