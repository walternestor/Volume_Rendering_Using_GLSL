// window.h

#ifndef WINDOW_H
#define WINDOW_H

#include <QWidget>
#include <QSlider>
#include <QColorDialog>
#include <stdio.h>

//#include "gdcmImageReader.h"

//#define QT_NO_OPENGL
//#include "qcustomplot.h"
//#undef QT_NO_OPENGL

namespace Ui {
class Window;
}

class Window : public QWidget
{
    Q_OBJECT

public:
    explicit Window(QWidget *parent = 0);
    ~Window();

    void plot();
    void eqHistogram();
    void nonEqHistogram();



public slots:
    void plotHistogram(QVector<double> key, QVector<double> nonEqValue, QVector<double> eqValue, QVector<double> lutValue);

//    QVector<double> takeKeyValue();


protected:
    void keyPressEvent(QKeyEvent *event);

private slots:


    void on_histPushButton_clicked();
    void on_loadRAWButton_clicked();    
    void on_loadPushButton_clicked();

private:
    Ui::Window *ui;

};

#endif // WINDOW_H
