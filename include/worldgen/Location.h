#pragma once

#include <string>
#include <memory>

struct VisualData {
    float x, y, height;
    float r, g, b;
};

struct Location {
    std::string id;
    std::string name;
    std::string description;
    std::string terrainType;
    std::vector<std::string> tags;
    std::vector<std::string> connections;

    std::shared_ptr<VisualData> visual;
};