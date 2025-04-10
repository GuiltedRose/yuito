#pragma once

#include <QOpenGLFunctions>
#include <QOpenGLFunctions_4_5_Core>
#include <QOpenGLShaderProgram>
#include <unordered_map>
#include <memory>
#include "render/PrimitiveMeshGenerator.h" // for PrimitiveShape

class RenderSystem : public QOpenGLFunctions_4_5_Core {
    public:
        struct Mesh {  // move this up here
            GLuint vao = 0;
            GLuint vbo = 0;
            int vertexCount = 0;

            void draw(QOpenGLFunctions_4_5_Core* gl) const;
        };
    
        void initialize();
        void initGL();
        void drawPrimitive(PrimitiveShape shape);
        void drawMesh(const Mesh& mesh);       // now valid
        void cleanupMesh(Mesh& mesh);          // now valid
        QOpenGLShaderProgram* getShader(const std::string& name);
    
    private:
        QOpenGLFunctions_4_5_Core* gl = nullptr;

        void loadBasicShaders();
        void generatePrimitives();
    
        std::unordered_map<std::string, std::unique_ptr<QOpenGLShaderProgram>> shaders;
    
        Mesh cubeMesh, capsuleMesh, sphereMesh;
    };
    
