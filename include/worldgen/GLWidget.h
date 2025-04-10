#pragma once

#include "render/RenderSystem.h"
#include "render/ShaderSource.h"
#include "worldgen/WorldGenerator.h"
#include "worldgen/ChunkManager.h"

#include <QOpenGLWidget>
#include <QOpenGLFunctions_4_5_Core>
#include <QTimer>
#include <QKeyEvent>
#include <QMouseEvent>
#include <vector>

class World;

struct Tile {
    float x, height, y;
    float r, g, b;
};

class GLWidget : public QOpenGLWidget, protected QOpenGLFunctions_4_5_Core {
    Q_OBJECT
public:
    explicit GLWidget(QWidget* parent = nullptr);

    static Tile fromLocation(const Location& loc, const Vec2i& regionCoords, int index);

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

    std::vector<Tile> tiles;
    QTimer timer;

    float camAngleX = 45.0f;
    float camAngleY = 45.0f;
    float camDist = 20.0f;
    QPoint lastMousePos;

    RenderSystem* renderSystem = nullptr;
    ChunkManager* chunkManager = nullptr;
    World* world = nullptr;
};
