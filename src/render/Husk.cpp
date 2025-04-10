#include "render/Husk.h"
#include "render/CharacterData.h"
#include <iostream> // placeholder for rendering system

Husk::Husk() {
    // Default neutral values
    head.shape = PrimitiveShape::RoundedCube;
    head.scale = Vec3(0.5f, 0.5f, 0.5f);
    head.position = Vec3(0.0f, 1.5f, 0.0f);

    // Positioned relative to head center
    leftEye = {
        Vec3(-0.15f, 1.6f, 0.26f),
        Vec3(0.08f, 0.08f, 0.08f),
        Quaternion(),
        PrimitiveShape::Sphere,
        Vec3(1.0f, 1.0f, 1.0f) // white or glow color
    };

    rightEye = {
        Vec3(0.15f, 1.6f, 0.26f),
        Vec3(0.08f, 0.08f, 0.08f),
        Quaternion(),
        PrimitiveShape::Sphere,
        Vec3(1.0f, 1.0f, 1.0f)
    };

    leftHand = {
        Vec3(-0.4f, 0.9f, 0.1f),           // start offset
        Vec3(0.12f, 0.12f, 0.12f),
        Quaternion(),
        PrimitiveShape::Sphere,
        Vec3(0.9f, 0.9f, 0.9f)
    };
    
    rightHand = {
        Vec3(0.4f, 0.9f, 0.1f),
        Vec3(0.12f, 0.12f, 0.12f),
        Quaternion(),
        PrimitiveShape::Sphere,
        Vec3(0.9f, 0.9f, 0.9f)
    };
    
    torso.shape = PrimitiveShape::Capsule;
    torso.scale = Vec3(0.6f, 1.0f, 0.4f);
    torso.position = Vec3(0.0f, 0.5f, 0.0f);

}

void Husk::attachCharacter(std::shared_ptr<CharacterData> character) {
    sourceData = character;
    applyCharacterVisuals();
}

void Husk::enableTrail(bool active) {
    ghostTrail = active;
}

void Husk::setEmotionAura(const std::string& tag) {
    // Placeholder — could map emotion to color or aura mesh
    if (tag == "vengeful") {
        aura = HuskSegment{
            Vec3(0, 0, 0), Vec3(1, 1, 1), Quaternion(), PrimitiveShape::Sphere, Vec3(0.6f, 0.0f, 0.0f)
        };
    } else if (tag == "hopeful") {
        aura = HuskSegment{
            Vec3(0, 0, 0), Vec3(1, 1, 1), Quaternion(), PrimitiveShape::Sphere, Vec3(0.2f, 0.8f, 1.0f)
        };
    } else {
        aura.reset();
    }
}

void Husk::applyCharacterVisuals() {
    if (!sourceData) return;

    const std::string& emotion = sourceData->getEmotionalTag();
    setEmotionAura(emotion);

    // Simple color logic: different factions = different base hues
    const std::string& faction = sourceData->getFaction();
    if (faction == "outlaw") {
        head.color = Vec3(0.7f, 0.1f, 0.1f);
        torso.color = Vec3(0.5f, 0.0f, 0.0f);
    } else if (faction == "villager") {
        head.color = Vec3(0.9f, 0.9f, 0.7f);
        torso.color = Vec3(0.6f, 0.6f, 0.4f);
    } else {
        head.color = Vec3(0.5f, 0.5f, 0.5f);
        torso.color = Vec3(0.5f, 0.5f, 0.5f);
    }
}

void Husk::update(float deltaTime) {
    // Optional floating or pulsing animation logic here
}

void Husk::render(RenderSystem* renderSystem) {
    QOpenGLShaderProgram* shader = renderSystem->getShader("husk");
    if (!shader) return;

    shader->bind();

    auto drawSegment = [&](const HuskSegment& seg) {
        shader->setUniformValue("uColor", seg.color.x, seg.color.y, seg.color.z);

        // Set model matrix based on seg.position, scale, rotation
        QMatrix4x4 model;
        model.translate(QVector3D(seg.position.x, seg.position.y, seg.position.z));
        model.rotate(QQuaternion(seg.rotation.w, seg.rotation.x, seg.rotation.y, seg.rotation.z));
        model.scale(seg.scale.x, seg.scale.y, seg.scale.z);

        shader->setUniformValue("uModel", model);

        renderSystem->drawPrimitive(seg.shape); // Cube, Capsule, etc.
    };

    drawSegment(torso);
    drawSegment(head);
    
    drawSegment(leftEye);
    drawSegment(rightEye);


    if (aura.has_value()) {
        drawSegment(aura.value());
    }

    shader->release();
}
