// myglwidget.h

#ifndef MYGLWIDGET_H
#define MYGLWIDGET_H

#ifndef GLEW_STATIC
#define GLEW_STATIC
#endif

#ifdef __WIN32__
#include <GL/glew.h>
#elif __linux__
#include <GL/glew.h>
#elif __APPLE__
#include <glew.h>
#endif

#include <stdio.h>

#include <QKeyEvent>
#include <QGLWidget>
#include <QGLBuffer>
#include <QMouseEvent>
#include <QTransform>

#include "window.h"

class MyGLWidget : public QGLWidget
{
    Q_OBJECT


public:
    explicit MyGLWidget(QWidget *parent = 0);
    ~MyGLWidget();

    QVector<double> equalizedValue();
    QVector<double> nonEqualizedValue();
    QVector<double> keyValue();
    QVector<double> lutEqualizedValue();

//    GLuint initVol3DTex(const char* filename, int fileSize);
    GLuint initVol3DTex(const char* filename, GLuint w, GLuint h, GLuint d);



signals:

protected:
    void initializeGL();
    void paintGL();
    void resizeGL(int w, int h);

    QSize minimumSizeHint() const;
    QSize sizeHint() const;
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);

public slots:
    void keyPressed(int key);

    // Aplha
    void alphaWidthUniform(int value);
    void alphaCenterUniform(int value);
    // Red
    void redWidthUniform(int value);
    void redCenterUniform(int value);
    // Green
    void greenWidthUniform(int value);
    void greenCenterUniform(int value);
    // Blue
    void blueWidthUniform(int value);
    void blueCenterUniform(int value);

    // Intensity Control
    void intensityMaxSliderUniform(int value);
    void intensityMinSliderUniform(int value);

    // Free Clip Plane
    void clipEnableUniform(bool checked);
    void azimuthUniform(int value);
    void elevationUniform(int value);
    void clipPlaneDepthUniform(int value);

    // Bounding Box Clip Planes
    void cutBBTop(int value);
    void cutBBBottom(int value);
    void cutBBLeft(int value);
    void cutBBRight(int value);
    void cutBBFront(int value);
    void cutBBBack(int value);

private:

};

#endif // MYGLWIDGET_H

