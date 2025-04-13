#pragma once

#include <optional>
#include <memory>
#include <string>
#include "math/Vec3.h"
#include "math/Quaternion.h"
#include "render/colors.h"
#include "render/RenderSystem.h"

enum class PrimitiveShape;

struct HuskSegment {
    Vec3 position;
    Vec3 scale;
    Quaternion rotation;
    PrimitiveShape shape;
    Vec3 color;
};

class CharacterData;

class Husk {
public:
    Husk();

    void update(float deltaTime);
    void render(RenderSystem* renderSystem);

    void attachCharacter(std::shared_ptr<CharacterData> character);
    void enableTrail(bool active);
    void setEmotionAura(const std::string& emotionalTag);

    void setColor(const Vec3& c) {color = c;}
    Vec3 getColor() const {return color;}
    

private:
    Vec3 color = {1.0f, 1.0f, 1.0f};
    
    HuskSegment head;
    HuskSegment torso;
    HuskSegment leftEye;
    HuskSegment rightEye;
    HuskSegment leftHand;
    HuskSegment rightHand;
    std::optional<HuskSegment> aura;

    std::shared_ptr<CharacterData> sourceData;
    bool ghostTrail = false;

    void applyCharacterVisuals();
};
