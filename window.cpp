
#include <iostream>
#include "glwidget.h"
#include "window.h"

Window::Window()
{

    /// Specify an OpenGL 3.3 format using the Core profile.
    /// That is, no old-school fixed pipeline functionality
    QGLFormat glFormat;

    glFormat.setVersion( 3, 3 );
    glFormat.setProfile( QGLFormat::CoreProfile ); /**< Requires >=Qt-4.8.0 */
    glFormat.setSampleBuffers( true );
    glFormat.setAlpha(true);
    glFormat.setAccum(true);
    glFormat.setDepth(true);
    glFormat.setDoubleBuffer(true);
    glFormat.setRgba(true);

    /// Create a GLWidget requesting our format
    glWidget = new GLWidget(glFormat);

    /// Add the GLWidget em box Layout
    QHBoxLayout *mainLayout = new QHBoxLayout;
    mainLayout->addWidget(glWidget);
    setLayout(mainLayout);
    setWindowTitle(tr("Ray Casting"));
}

void Window::keyPressEvent(QKeyEvent *e)
{
    if (e->key() == Qt::Key_Escape)
        close();
    else
        QWidget::keyPressEvent(e);
}
