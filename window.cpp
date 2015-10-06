// window.cpp

//#include <QtWidgets>
#include "window.h"
#include "ui_window.h"

#include "myglwidget.h"

Window::Window(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Window)
{
    ui->setupUi(this);

    connect(ui->topSlider, SIGNAL(valueChanged(int)), ui->myGLWidget, SLOT(cutBBTop(int)));
    connect(ui->bottomSlider, SIGNAL(valueChanged(int)), ui->myGLWidget, SLOT(cutBBBottom(int)));
    connect(ui->leftSlider, SIGNAL(valueChanged(int)), ui->myGLWidget, SLOT(cutBBLeft(int)));
    connect(ui->rightSlider, SIGNAL(valueChanged(int)), ui->myGLWidget, SLOT(cutBBRight(int)));
    connect(ui->frontSlider, SIGNAL(valueChanged(int)), ui->myGLWidget, SLOT(cutBBFront(int)));
    connect(ui->backSlider, SIGNAL(valueChanged(int)), ui->myGLWidget, SLOT(cutBBBack(int)));

    connect(ui->azimuthSlider, SIGNAL(valueChanged(int)), ui->myGLWidget, SLOT(azimuthUniform(int)));
    connect(ui->elevationSlider, SIGNAL(valueChanged(int)), ui->myGLWidget, SLOT(elevationUniform(int)));
    connect(ui->clipPlaneDepthSlider, SIGNAL(valueChanged(int)), ui->myGLWidget, SLOT(clipPlaneDepthUniform(int)));
    connect(ui->clipCheckBox, SIGNAL(toggled(bool)), ui->myGLWidget, SLOT(clipEnableUniform(bool)));

}

//int Window::getSender(void)
//{
//    // e.g. check with member variable _foobarButton
//       QObject* obj = sender();

//       if(obj == topSlider)
//       {
//       }
//       else if (obj == bottomSlider)
//       {
//       }
//}

Window::~Window()
{
    delete ui;
}

void Window::keyPressEvent(QKeyEvent *e)
{
    if (e->key() == Qt::Key_Escape)
        close();
    else
        QWidget::keyPressEvent(e);
}
