#ifndef WINDOW_H
#define WINDOW_H

#include <QWidget>
#include <QHBoxLayout>

class GLWidget;

class Window : public QWidget
{
    Q_OBJECT

public:

    Window();

protected:

    void keyPressEvent(QKeyEvent *event);

private:
    GLWidget *glWidget;
};

#endif // WINDOW_H
