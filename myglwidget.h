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

#include <QTimer>
#include <QKeyEvent>
#include <QGLWidget>
#include <QGLBuffer>
//#include <QGLShaderProgram>

#include "window.h"

class MyGLWidget : public QGLWidget
{
    Q_OBJECT
public:
    explicit MyGLWidget(QWidget *parent = 0);
    ~MyGLWidget();
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
//    void cutBoundBox(int value);

    void cutBBTop(int value);
    void cutBBBottom(int value);
    void cutBBLeft(int value);
    void cutBBRight(int value);
    void cutBBFront(int value);
    void cutBBBack(int value);

signals:

private:

};

#endif // MYGLWIDGET_H

