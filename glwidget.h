#ifndef GLWIDGET_H
#define GLWIDGET_H

//#define BUFFER_OFFSET(x)  ((const void*) (x))

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
#include <QGLShaderProgram>


class GLWidget : public QGLWidget
{
    Q_OBJECT        ///< must include this if you use Qt signals/slots

public:

     GLWidget(const QGLFormat& format, QWidget *parent = 0);

    ~GLWidget();

    QSize minimumSizeHint() const;

    QSize sizeHint() const;

public slots:
    void rotateDisplay();

signals:

protected:

    void initializeGL();

    void paintGL();

    void resizeGL (int w, int h);

};
#endif // GLWIDGET_H
