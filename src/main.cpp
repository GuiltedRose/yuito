#include "worldgen/World.h"
#include "render/RenderSystem.h"
#include "worldgen/GLWidget.h"
#include <QApplication>

int main(int argc, char** argv) {
    QApplication app(argc, argv);

    World world;
    RenderSystem renderSystem;

    GLWidget* widget = new GLWidget();
    widget->resize(1280, 720);
    widget->setWorld(&world);
    widget->setRenderSystem(&renderSystem);  // We pass it in, but let GLWidget handle init
    widget->show();

    return app.exec();
}
