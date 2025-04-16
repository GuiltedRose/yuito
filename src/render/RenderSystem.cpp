#include "render/RenderSystem.h"
#include "render/ShaderSource.h"
#include "render/PrimitiveMeshGenerator.h"
#include <QOpenGLContext>
#include <QOpenGLFunctions_4_5_Core>
#include <QDebug>

void RenderSystem::initGL() {
    if (!QOpenGLContext::currentContext()) {
        qFatal("No OpenGL context available!");
    }

    gl = new QOpenGLFunctions_4_5_Core;
    gl->initializeOpenGLFunctions();
}

void RenderSystem::initialize() {
    loadBasicShaders();
    generatePrimitives();
}

void RenderSystem::loadBasicShaders() {
    auto huskShader = std::make_unique<QOpenGLShaderProgram>();
    huskShader->addShaderFromSourceCode(QOpenGLShader::Vertex, ShaderSource::huskVertex);
    huskShader->addShaderFromSourceCode(QOpenGLShader::Fragment, ShaderSource::huskFragment);
    huskShader->link();
    shaders["husk"] = std::move(huskShader);
}

QOpenGLShaderProgram* RenderSystem::getShader(const std::string& name) {
    auto it = shaders.find(name);
    return (it != shaders.end()) ? it->second.get() : nullptr;
}

void RenderSystem::drawPrimitive(PrimitiveShape shape) {
    switch (shape) {
        case PrimitiveShape::RoundedCube: cubeMesh.draw(gl); break;
        case PrimitiveShape::Capsule:     capsuleMesh.draw(gl); break;
        case PrimitiveShape::Sphere:      sphereMesh.draw(gl); break;
    }
}

void RenderSystem::drawMesh(const Mesh& mesh) {
    gl->glBindVertexArray(mesh.vao);
    gl->glDrawArrays(GL_TRIANGLES, 0, mesh.vertexCount);
    gl->glBindVertexArray(0);
}

void RenderSystem::Mesh::draw(QOpenGLFunctions_4_5_Core* gl) const {
    gl->glBindVertexArray(vao);
    gl->glDrawArrays(GL_TRIANGLES, 0, vertexCount);
    gl->glBindVertexArray(0);
}

void RenderSystem::cleanupMesh(Mesh& mesh) {
    gl->glDeleteBuffers(1, &mesh.vbo);
    gl->glDeleteVertexArrays(1, &mesh.vao);
}

void RenderSystem::uploadCaveMesh(const CaveMeshGenerator::CaveMesh& mesh) {
    caveVertices = mesh.vertices;
    caveIndices = mesh.indices;

    if (caveVAO == 0) {
        gl->glGenVertexArrays(1, &caveVAO);
        gl->glGenBuffers(1, &caveVBO);
        gl->glGenBuffers(1, &caveEBO);
    }

    gl->glBindVertexArray(caveVAO);

    gl->glBindBuffer(GL_ARRAY_BUFFER, caveVBO);
    gl->glBufferData(GL_ARRAY_BUFFER, caveVertices.size() * sizeof(Vertex), caveVertices.data(), GL_STATIC_DRAW);

    gl->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, caveEBO);
    gl->glBufferData(GL_ELEMENT_ARRAY_BUFFER, caveIndices.size() * sizeof(unsigned int), caveIndices.data(), GL_STATIC_DRAW);

    gl->glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, position));
    gl->glEnableVertexAttribArray(0);
    gl->glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, color));
    gl->glEnableVertexAttribArray(1);

    gl->glBindVertexArray(0);
}

void RenderSystem::drawCaveMesh() {
    if (caveVAO == 0) return;
    gl->glBindVertexArray(caveVAO);
    gl->glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(caveIndices.size()), GL_UNSIGNED_INT, 0);
    gl->glBindVertexArray(0);
}

void RenderSystem::generatePrimitives() {
    // === Rounded Cube ===
    std::vector<float> cubeVerts = {
        // (same cube vertex data, no normals/colors)
        -0.5f,-0.5f,-0.5f,  0.5f,-0.5f,-0.5f,  0.5f, 0.5f,-0.5f,
        -0.5f,-0.5f,-0.5f,  0.5f, 0.5f,-0.5f, -0.5f, 0.5f,-0.5f,
        -0.5f,-0.5f, 0.5f,  0.5f,-0.5f, 0.5f,  0.5f, 0.5f, 0.5f,
        -0.5f,-0.5f, 0.5f,  0.5f, 0.5f, 0.5f, -0.5f, 0.5f, 0.5f,
        -0.5f,-0.5f,-0.5f, -0.5f, 0.5f,-0.5f, -0.5f, 0.5f, 0.5f,
        -0.5f,-0.5f,-0.5f, -0.5f, 0.5f, 0.5f, -0.5f,-0.5f, 0.5f,
         0.5f,-0.5f,-0.5f,  0.5f, 0.5f,-0.5f,  0.5f, 0.5f, 0.5f,
         0.5f,-0.5f,-0.5f,  0.5f, 0.5f, 0.5f,  0.5f,-0.5f, 0.5f,
        -0.5f,-0.5f,-0.5f, -0.5f,-0.5f, 0.5f,  0.5f,-0.5f, 0.5f,
        -0.5f,-0.5f,-0.5f,  0.5f,-0.5f, 0.5f,  0.5f,-0.5f,-0.5f,
        -0.5f, 0.5f,-0.5f, -0.5f, 0.5f, 0.5f,  0.5f, 0.5f, 0.5f,
        -0.5f, 0.5f,-0.5f,  0.5f, 0.5f, 0.5f,  0.5f, 0.5f,-0.5f
    };

    gl->glGenVertexArrays(1, &cubeMesh.vao);
    gl->glGenBuffers(1, &cubeMesh.vbo);
    gl->glBindVertexArray(cubeMesh.vao);
    gl->glBindBuffer(GL_ARRAY_BUFFER, cubeMesh.vbo);
    gl->glBufferData(GL_ARRAY_BUFFER, cubeVerts.size() * sizeof(float), cubeVerts.data(), GL_STATIC_DRAW);
    gl->glEnableVertexAttribArray(0);
    gl->glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    gl->glBindVertexArray(0);
    cubeMesh.vertexCount = cubeVerts.size() / 3;

    // === Sphere ===
    std::vector<float> sphereVerts;
    appendSphereVerts(sphereVerts);

    gl->glGenVertexArrays(1, &sphereMesh.vao);
    gl->glGenBuffers(1, &sphereMesh.vbo);
    gl->glBindVertexArray(sphereMesh.vao);
    gl->glBindBuffer(GL_ARRAY_BUFFER, sphereMesh.vbo);
    gl->glBufferData(GL_ARRAY_BUFFER, sphereVerts.size() * sizeof(float), sphereVerts.data(), GL_STATIC_DRAW);
    gl->glEnableVertexAttribArray(0);
    gl->glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    gl->glBindVertexArray(0);
    sphereMesh.vertexCount = sphereVerts.size() / 3;

    // === Capsule ===
    std::vector<float> capsuleVerts;
    appendCapsuleVerts(capsuleVerts);

    gl->glGenVertexArrays(1, &capsuleMesh.vao);
    gl->glGenBuffers(1, &capsuleMesh.vbo);
    gl->glBindVertexArray(capsuleMesh.vao);
    gl->glBindBuffer(GL_ARRAY_BUFFER, capsuleMesh.vbo);
    gl->glBufferData(GL_ARRAY_BUFFER, capsuleVerts.size() * sizeof(float), capsuleVerts.data(), GL_STATIC_DRAW);
    gl->glEnableVertexAttribArray(0);
    gl->glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    gl->glBindVertexArray(0);
    capsuleMesh.vertexCount = capsuleVerts.size() / 3;
}