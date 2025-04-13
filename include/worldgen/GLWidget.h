#pragma once

#include "render/RenderSystem.h"
#include "render/ShaderSource.h"
#include "render/RenderTypes.h"
#include "math/Vec2.h"
#include "worldgen/ChunkManager.h"
#include "worldgen/World.h"

#include <QOpenGLWidget>
#include <QOpenGLFunctions_4_5_Core>
#include <QTimer>
#include <QKeyEvent>
#include <QMouseEvent>
#include <vector>

class GLWidget : public QOpenGLWidget, protected QOpenGLFunctions_4_5_Core {
    Q_OBJECT
public:
    explicit GLWidget(QWidget* parent = nullptr);

    static Tile fromLocation(const Location& loc, const Math::Vec2i& regionCoords, int index);

    void setWorld(World* w);
    void setRenderSystem(RenderSystem* rs);
    void setChunkManager(ChunkManager* cm);

protected:
    void initializeGL() override;
    void resizeGL(int w, int h) override;
    void paintGL() override;

    void keyPressEvent(QKeyEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;

private:
    void drawTiles(QOpenGLShaderProgram* shader);

    std::vector<Tile> visibleTiles;
    MapLayer currentLayer = MapLayer::Surface;
    QTimer timer;

    float camAngleX = 45.0f;
    float camAngleY = 45.0f;
    float camDist = 20.0f;
    QPoint lastMousePos;

    Math::Vec2i fakePlayerPos{0, 0};  // Simulated position

    RenderSystem* renderSystem = nullptr;
    ChunkManager* chunkManager = nullptr;
    World* world = nullptr;
};
