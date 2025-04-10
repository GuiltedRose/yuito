#include "npc/NPCManager.h"
#include "NameGenerator.h"
#include "render/CharacterDraw.h"
#include "render/colors.h"

void NPCManager::spawnNPC(const Vec3& position) {
    NPC npc;
    npc.id = UUID::generate();
    npc.name = NameGenerator::randomName();
    npc.age = 18 + rand() % 60;

    npc.position = position;
    npc.rotation = Quaternion::identity();

    npc.kindness = rand() / (float)RAND_MAX;
    npc.suspicion = rand() / (float)RAND_MAX;
    npc.courage = rand() / (float)RAND_MAX;
    npc.curiosity = rand() / (float)RAND_MAX;

    npc.color = ColorPalette::accessibleColors[rand() % ColorPalette::accessibleColors.size()];

    static const std::vector<std::string> archetypes = {
        "Wanderer", "Homesteader", "Scholar", "Cultist"
    };
    npc.archetype = archetypes[rand() % archetypes.size()];
    npc.origin = "Unknown";

    npcs.push_back(npc);
}

void NPCManager::updateAll(float deltaTime) {
    for (auto& npc : npcs) {
        // AI logic placeholder
    }
}

void NPCManager::drawAll() {
    for (const auto& npc : npcs) {
        drawCharacterMesh(npc.position, npc.rotation); // assume this exists
    }
}

std::vector<NPC>& NPCManager::getAll() {
    return npcs;
}
