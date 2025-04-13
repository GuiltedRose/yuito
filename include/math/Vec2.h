#pragma once

#include <cmath>
#include <functional> 

struct Vec2 {
    float x = 0.0f;
    float y = 0.0f;

    Vec2() = default;
    Vec2(float x_, float y_) : x(x_), y(y_) {}

    Vec2 operator+(const Vec2& other) const { return Vec2(x + other.x, y + other.y); }
    Vec2 operator-(const Vec2& other) const { return Vec2(x - other.x, y - other.y); }
    Vec2 operator*(float scalar) const { return Vec2(x * scalar, y * scalar); }
    Vec2 operator/(float scalar) const { return Vec2(x / scalar, y / scalar); }

    float length() const { return std::sqrt(x * x + y * y); }

    Vec2 normalized() const {
        float len = length();
        return (len > 0.0f) ? Vec2(x / len, y / len) : Vec2(0.0f, 0.0f);
    }
};
namespace Math {
    struct Vec2i {
        int x;
        int y;

        bool operator==(const Vec2i& other) const {
            return x == other.x && y == other.y;
        }
    };
}

namespace std {
    template<>
    struct hash<Math::Vec2i> {
        std::size_t operator()(const Math::Vec2i& v) const {
            return std::hash<int>()(v.x) ^ (std::hash<int>()(v.y) << 1);
        }
    };
}
