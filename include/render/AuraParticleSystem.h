#pragma once

#include <vector>
#include "math/Vec3.h"

struct AuraParticle {
    Vec3 position;
    Vec3 velocity;
    Vec3 color;
    float lifetime;
    float maxLifetime;
    float scale;
};

class AuraParticleSystem {
public:
    void update(float deltaTime);
    void emitParticles(const Vec3& origin, const Vec3& baseColor, int count = 4);
    void draw(class RenderSystem* renderSystem, class QOpenGLShaderProgram* shader) const;

private:
    std::vector<AuraParticle> particles;
};
