#include "worldgen/GLWidget.h"
#include "worldgen/World.h"
#include <cmath>
#include <QMatrix4x4>
#include <QTimer>
#include <QOpenGLShaderProgram>
#include <QMouseEvent>
#include <QKeyEvent>

GLWidget::GLWidget(QWidget* parent) : QOpenGLWidget(parent) {
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

Tile GLWidget::fromLocation(const Location& loc, const Vec2i& regionCoords, int index) {
    if (!loc.visual) {
        return Tile{
            float(regionCoords.x * 16 + index % 4),
            0.5f,
            float(regionCoords.y * 16 + index / 4),
            0.4f, 0.6f, 0.2f
        };
    }

    return Tile{
        float(regionCoords.x * 16 + loc.visual->x),
        loc.visual->height,
        float(regionCoords.y * 16 + loc.visual->y),
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
        tiles = chunkManager->collectRenderTiles();
}


void GLWidget::resizeGL(int w, int h) {
    glViewport(0, 0, w, h);
}

void GLWidget::drawTiles(QOpenGLShaderProgram* shader) {
    for (const auto& tile : tiles) {
        QMatrix4x4 model;
        model.translate(QVector3D(tile.x, tile.height, tile.y));
        model.scale(1.0f, 1.0f, 1.0f);

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

    QMatrix4x4 view;
    float radX = camAngleX * M_PI / 180.0f;
    float radY = camAngleY * M_PI / 180.0f;
    float eyeX = camDist * sin(radX) * cos(radY);
    float eyeY = camDist * sin(radY);
    float eyeZ = camDist * cos(radX) * cos(radY);

    view.lookAt(QVector3D(eyeX, eyeY, eyeZ), QVector3D(5.0f, 0.0f, 5.0f), QVector3D(0.0f, 1.0f, 0.0f));

    if (world) {
        world->prepareRender();
        tiles = world->getRenderTiles();
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
    if (event->key() == Qt::Key_W) camDist -= 1.0f;
    if (event->key() == Qt::Key_S) camDist += 1.0f;
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
