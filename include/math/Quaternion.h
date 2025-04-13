#pragma once

struct Quaternion {
    float x = 0;
    float y = 0;
    float z = 0;
    float w = 1;

    Quaternion() = default;
    Quaternion(float x, float y, float z, float w)
        : x(x), y(y), z(z), w(w) {}

    static Quaternion identity() {
        return Quaternion(0, 0, 0, 1);
    }
};

