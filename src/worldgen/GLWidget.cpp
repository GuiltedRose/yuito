#include "worldgen/GLWidget.h"

#include <cmath>
#include <QMatrix4x4>
#include <QTimer>
#include <QOpenGLShaderProgram>
#include <QMouseEvent>
#include <QKeyEvent>
#include <iostream>

GLWidget::GLWidget(QWidget* parent) : QOpenGLWidget(parent) {
    camDist = 20.0f;
    camAngleX = 45.0f;
    camAngleY = 45.0f;
    
    setFocusPolicy(Qt::StrongFocus);
    timer.setInterval(16);
    connect(&timer, &QTimer::timeout, this, QOverload<>::of(&GLWidget::update));
    timer.start();
}

void GLWidget::setWorld(World* w) {
    world = w;
}

void GLWidget::setRenderSystem(RenderSystem* rs) {
    renderSystem = rs;
}

void GLWidget::setChunkManager(ChunkManager* cm) {
    chunkManager = cm;
}

Tile GLWidget::fromLocation(const Location& loc, const Math::Vec2i& regionCoords, int index) {
    const float tileSize = 1.0f;
    const int regionSize = 16;

    float localX = index % regionSize;
    float localZ = index / regionSize;

    float worldX = regionCoords.x * regionSize + localX;
    float worldZ = regionCoords.y * regionSize + localZ;

    if (!loc.visual) {
        return Tile{ worldX * tileSize, 0.5f, worldZ * tileSize, 0.4f, 0.6f, 0.2f };
    }

    worldX = regionCoords.x * regionSize + loc.visual->x;
    worldZ = regionCoords.y * regionSize + loc.visual->y;

    return Tile{
        worldX * tileSize,
        loc.visual->height,
        worldZ * tileSize,
        loc.visual->r,
        loc.visual->g,
        loc.visual->b
    };
}



void GLWidget::initializeGL() {
    initializeOpenGLFunctions();
    glEnable(GL_DEPTH_TEST);
    glClearColor(0.1f, 0.1f, 0.15f, 1.0f);

    if (renderSystem) {
        renderSystem->initGL();         // << set up context-bound GL functions
        renderSystem->initialize();     // << set up VAOs, shaders, etc
    }

    if (chunkManager)
        visibleTiles = chunkManager->collectRenderTiles(currentLayer);
}


void GLWidget::resizeGL(int w, int h) {
    glViewport(0, 0, w, h);
}

void GLWidget::drawTiles(QOpenGLShaderProgram* shader) {
    for (const auto& tile : visibleTiles) {
        QMatrix4x4 model;
        model.translate(QVector3D(tile.x, tile.height, tile.y));
        model.scale(0.98f, 1.0f, 0.98f); 

        shader->setUniformValue("uModel", model);
        shader->setUniformValue("uColor", tile.r, tile.g, tile.b);

        if (renderSystem)
            renderSystem->drawPrimitive(PrimitiveShape::RoundedCube);
    }
}

void GLWidget::paintGL() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    QMatrix4x4 projection;
    projection.perspective(60.0f, float(width()) / float(height()), 0.1f, 100.0f);

    // Default to some center if world is missing
    QVector3D center(8.0f, 0.0f, 8.0f);

    if (world) {
        const Math::Vec2i& pos = world->getPlayerPosition();
        center = QVector3D(pos.x, 0.0f, pos.y);
    }

    QVector3D eye = center + QVector3D(0.0f, camDist, camDist); // Above and back
    QVector3D up(0.0f, 1.0f, 0.0f);

    QMatrix4x4 view;
    view.lookAt(eye, center, up);

    if (world) {
        world->prepareRender(currentLayer);
        visibleTiles = world->getRenderTiles(currentLayer);
    }    

    if (renderSystem) {
        QOpenGLShaderProgram* shader = renderSystem->getShader("husk");
        if (shader) {
            shader->bind();
            shader->setUniformValue("uView", view);
            shader->setUniformValue("uProjection", projection);
            drawTiles(shader);
            shader->release();
        }
    }
}


void GLWidget::keyPressEvent(QKeyEvent* event) {
    if (!world) return;

    Math::Vec2i pos = world->getPlayerPosition();

    switch (event->key()) {
        case Qt::Key_W: pos.y -= 1; break;
        case Qt::Key_S: pos.y += 1; break;
        case Qt::Key_A: pos.x -= 1; break;
        case Qt::Key_D: pos.x += 1; break;

        case Qt::Key_Z:
            currentLayer = (currentLayer == MapLayer::Surface)
                ? MapLayer::Underground
                : MapLayer::Surface;

            std::cout << "[Layer] Switched to "
                      << (currentLayer == MapLayer::Surface ? "Surface" : "Underground")
                      << std::endl;
            break;
    }

    world->setPlayerPosition(pos);
    world->update();
    update();
}

void GLWidget::mouseMoveEvent(QMouseEvent* event) {
    QPoint delta = event->pos() - lastMousePos;
    if (event->buttons() & Qt::LeftButton) {
        camAngleX += delta.x() * 0.5f;
        camAngleY += delta.y() * 0.5f;
    }
    lastMousePos = event->pos();
    update();
}
