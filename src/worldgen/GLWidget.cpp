#include "worldgen/GLWidget.h"
#include <cmath>
#include <QTimer>
#include <QMatrix4x4>
#include <QOpenGLFunctions>
#include <GL/glu.h>

GLWidget::GLWidget(QWidget* parent) : QOpenGLWidget(parent) {
    setFocusPolicy(Qt::StrongFocus);
    timer.setInterval(16);
    connect(&timer, &QTimer::timeout, this, QOverload<>::of(&GLWidget::update));
    timer.start();
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

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(60.0, width() / float(height()), 0.1, 100.0);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    // Simple orbit camera
    float radX = camAngleX * M_PI / 180.0f;
    float radY = camAngleY * M_PI / 180.0f;
    float eyeX = camDist * sin(radX) * cos(radY);
    float eyeY = camDist * sin(radY);
    float eyeZ = camDist * cos(radX) * cos(radY);

    gluLookAt(eyeX, eyeY, eyeZ, 5, 0, 5, 0, 1, 0);

    for (const auto& tile : tiles) {
        glPushMatrix();
        glTranslatef(tile.x, 0.0f, tile.y);

        glColor3f(tile.r, tile.g, tile.b);

        float h = tile.height;
        glBegin(GL_QUADS);
        // Top face
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
