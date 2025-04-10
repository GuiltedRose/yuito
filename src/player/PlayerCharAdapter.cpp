#include "player/PlayerCharAdapter.h"
#include "render/colors.h"

PlayerCharAdapter::PlayerCharAdapter(std::shared_ptr<PlayerCharacter> pc)
    : character(pc) {}

std::string PlayerCharAdapter::getID() const {
    return character->getID();
}

std::string PlayerCharAdapter::getEmotionalTag() const {
    auto& traits = character->getTraits();
    auto it = traits.find("emotion");
    return (it != traits.end()) ? it->second : "neutral";
}

std::string PlayerCharAdapter::getFaction() const {
    auto& traits = character->getTraits();
    auto it = traits.find("faction");
    return (it != traits.end()) ? it->second : "neutral";
}

Vec3 PlayerCharAdapter::getPoseHint() const {
    return Vec3(0.0f, 0.0f, 0.0f); // Optional: could use traits to vary this later
}

void PlayerCharAdapter::setColor(const Vec3& color) {
    this->color = color;
}
