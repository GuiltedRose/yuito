#pragma once

#include <vector>
#include <string>
#include "math/Vec3.h"
#include "math/Quaternion.h"
#include "render/colors.h"
#include "UUID.h"

struct NPC {
    UUID id;                  // Unique identifier
    std::string name;
    int age;
    int rank = 0;
    int loyalty = 0;

    Vec3 position;
    Quaternion rotation;

    float kindness;
    float suspicion;
    float courage;
    float curiosity;

    std::string origin;
    std::string archetype;

    Vec3 color;
    bool isAlive = true;
};

class NPCManager {
public:
    void spawnNPC(const Vec3& position);
    void updateAll(float deltaTime);
    void drawAll();

    std::vector<NPC>& getAll();

private:
    std::vector<NPC> npcs;
};
