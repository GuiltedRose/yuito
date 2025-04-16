#pragma once

#include <QOpenGLFunctions_4_5_Core>
#include <QOpenGLShaderProgram>
#include <unordered_map>
#include <memory>
#include <vector>

#include "render/Vertex.h"
#include "render/PrimitiveMeshGenerator.h" // for PrimitiveShape
#include "render/CaveMeshGenerator.h"

class RenderSystem {
public:
    struct Mesh {
        GLuint vao = 0;
        GLuint vbo = 0;
        int vertexCount = 0;

        void draw(QOpenGLFunctions_4_5_Core* gl) const;
    };

    void initGL();
    void initialize();

    void drawPrimitive(PrimitiveShape shape);
    void drawMesh(const Mesh& mesh);
    void cleanupMesh(Mesh& mesh);

    void uploadCaveMesh(const CaveMeshGenerator::CaveMesh& mesh);
    void drawCaveMesh();

    QOpenGLShaderProgram* getShader(const std::string& name);

private:
    // Shader programs
    std::unordered_map<std::string, std::unique_ptr<QOpenGLShaderProgram>> shaders;

    // Primitive shapes
    Mesh cubeMesh, sphereMesh, capsuleMesh;

    // Cave mesh data
    std::vector<Vertex> caveVertices;
    std::vector<unsigned int> caveIndices;
    GLuint caveVAO = 0, caveVBO = 0, caveEBO = 0;

    // Internal setup
    void loadBasicShaders();
    void generatePrimitives();

    // Raw pointer to functions (can point to 'this' or override)
    QOpenGLFunctions_4_5_Core* gl = nullptr;
};
