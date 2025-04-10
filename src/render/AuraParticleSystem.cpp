#include "render/AuraParticleSystem.h"
#include "render/RenderSystem.h"
#include <algorithm>
#include <cstdlib>

void AuraParticleSystem::update(float dt) {
    for (auto& p : particles) {
        p.position += p.velocity * dt;
        p.lifetime -= dt;
    }

    particles.erase(
        std::remove_if(particles.begin(), particles.end(),
            [](const AuraParticle& p) { return p.lifetime <= 0.0f; }),
        particles.end()
    );
}

void AuraParticleSystem::emitParticles(const Vec3& origin, const Vec3& baseColor, int count) {
    for (int i = 0; i < count; ++i) {
        AuraParticle p;
        p.position = origin;

        p.velocity = Vec3(
            (rand() % 100 - 50) / 200.0f,
            (rand() % 100) / 100.0f,
            (rand() % 100 - 50) / 200.0f
        );

        p.color = baseColor;
        p.lifetime = 0.0f;
        p.maxLifetime = 1.5f + (rand() % 100) / 100.0f;
        p.scale = 0.1f + (rand() % 100) / 1000.0f;

        particles.push_back(p);
    }
}

void AuraParticleSystem::draw(RenderSystem* renderSystem, QOpenGLShaderProgram* shader) const {
    for (const auto& p : particles) {
        float fade = p.lifetime / p.maxLifetime;

        shader->setUniformValue("uColor",
            p.color.x * fade,
            p.color.y * fade,
            p.color.z * fade
        );

        QMatrix4x4 model;
        model.translate(QVector3D(p.position.x, p.position.y, p.position.z));
        model.scale(p.scale, p.scale, p.scale);
        shader->setUniformValue("uModel", model);

        renderSystem->drawPrimitive(PrimitiveShape::Sphere);
    }
}
