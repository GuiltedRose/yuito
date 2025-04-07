#pragma once

#include <QOpenWidget>
#include <QOpenGLFunctions>
#include <QTimer>
#include <QKeyEvent>
#include <QMouseEvent>
#include <vector>

struct Tile {
    float x, height, y;
    float r, g, b;
};


class GLWidget : public QOpenGLWidget, protected QOpenGLFunctions {
    Q_OBJECT
public:
    GLWidget(QWidget* parent = nullptr);

protected:
    void initializeGL() override;
    void paintGL() override;
    void resizeGL(int w, int h) override;

    void keyPressEvent(QKeyEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;

private:
    std::vector<Tile> tiles;
    QTimer timer;

    float camAngleX = 45.0f;
    float camAngleY = 45.0f;
    float camDist = 20.0f;
    QPoint lastMousePos;
};