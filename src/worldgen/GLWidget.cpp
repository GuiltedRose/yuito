#include "worldgen/GLWidget.h"
#include <cmath>
#include <QTimer>
#include <QMatrix4x4>
#include <QOpenGLFunctions>

GLWidget::GLWidget(QWidget* parent) : QOpenGLWidget(parent) {
    setFocusPolicy(Qt::StrongFocus);
    timer.setInterval(16);
    connect(&timer, &QTimer::timeout, this, QOverload<>::of(&GLWidget::update));
    timer.start();
}

Tile GLWidget::fromLocation(const Location& loc, const Vec2i& regionCoords, int index) {
    if (!loc.visual) {
        return Tile {
            float(regionCoords.x * 16 + index % 4),
            0.5f,
            float(regionCoords.y * 16 + index / 4),
            0.4f, 0.6f, 0.2f
        };
    }

    return *(loc.visual); // pull rendering info directly
}

void GLWidget::initializeGL() {
    initializeOpenGLFunctions();
    glEnable(GL_DEPTH_TEST);
    glClearColor(0.1f, 0.1f, 0.15f, 1.0f);
    tiles = chunkManager->collectRenderTiles();

    // Stub terrain grid
    for (int y = 0; y < 10; ++y) {
        for (int x = 0; x < 10; ++x) {
            tiles.push_back(Tile {
                float(x), float(y),
                0.2f + (x % 3) * 0.3f,
                0.2f + (y % 4) * 0.2f,
                0.5f
            });
        }
    }
}

void GLWidget::resizeGL(int w, int h) {
    glViewport(0, 0, w, h);
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

    view.lookAt(QVector3D(eyeX, eyeY, eyeZ),
                QVector3D(5.0f, 0.0f, 5.0f),
                QVector3D(0.0f, 1.0f, 0.0f));

    glMatrixMode(GL_PROJECTION);
    glLoadMatrixf(projection.constData());

    glMatrixMode(GL_MODELVIEW);
    glLoadMatrixf(view.constData());

    for (const auto& tile : tiles) {
        glPushMatrix();
        glTranslatef(tile.x, 0.0f, tile.y);
        glColor3f(tile.r, tile.g, tile.b);

        float h = tile.height;
        glBegin(GL_QUADS);
        glVertex3f(0, h, 0);
        glVertex3f(1, h, 0);
        glVertex3f(1, h, 1);
        glVertex3f(0, h, 1);
        glEnd();

        glPopMatrix();
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
