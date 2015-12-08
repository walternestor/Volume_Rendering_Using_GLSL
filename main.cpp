// main.cpp

#include <iostream>
#include <QApplication>
#include <QDesktopWidget>
#include <QGLFormat>

#include "window.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    if (!QGLFormat::hasOpenGL()) {
        std::cout << "This system has no OpenGL support" << std::endl;
        return 1;
    }

    std::cout << "This system has OpenGL support" << glGetString(GL_VERSION) << std::endl;

    Window window;
    window.resize(window.sizeHint());
    int desktopArea = QApplication::desktop()->width() *
            QApplication::desktop()->height();
    int widgetArea = window.width() * window.height();

    window.setWindowTitle("Raycating");

//    if (((float)widgetArea / (float)desktopArea) < 0.75f)
//        window.show();
//    else
        window.showMaximized();
    return app.exec();
}
