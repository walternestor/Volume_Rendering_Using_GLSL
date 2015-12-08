
#include "window.h"
#include "ui_window.h"

#include "myglwidget.h"

#include <fstream>

using namespace std;


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

    // Intensity Control
    connect(ui->intensityMaxSlider, SIGNAL(valueChanged(int)), ui->myGLWidget, SLOT(intensityMaxSliderUniform(int)));
    connect(ui->intensityMinSlider, SIGNAL(valueChanged(int)), ui->myGLWidget, SLOT(intensityMinSliderUniform(int)));

    // TFF
    connect(ui->redWidthSlider, SIGNAL(valueChanged(int)), ui->myGLWidget, SLOT(redWidthUniform(int)));
    connect(ui->redCenterSlider, SIGNAL(valueChanged(int)), ui->myGLWidget, SLOT(redCenterUniform(int)));
    connect(ui->greenWidthSlider, SIGNAL(valueChanged(int)), ui->myGLWidget, SLOT(greenWidthUniform(int)));
    connect(ui->greenCenterSlider, SIGNAL(valueChanged(int)), ui->myGLWidget, SLOT(greenCenterUniform(int)));
    connect(ui->blueWidthSlider, SIGNAL(valueChanged(int)), ui->myGLWidget, SLOT(blueWidthUniform(int)));
    connect(ui->blueCenterSlider, SIGNAL(valueChanged(int)), ui->myGLWidget, SLOT(blueCenterUniform(int)));
    connect(ui->alphaWidthSlider, SIGNAL(valueChanged(int)), ui->myGLWidget, SLOT(alphaWidthUniform(int)));
    connect(ui->alphaCenterSlider, SIGNAL(valueChanged(int)), ui->myGLWidget, SLOT(alphaCenterUniform(int)));

    //    ui->customPlot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectAxes |
    //                                      QCP::iSelectLegend | QCP::iSelectPlottables);

    //    ui->histogramPlot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectAxes |
    //                                      QCP::iSelectLegend | QCP::iSelectPlottables);

    ui->histogramPlot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectAxes |
                                       QCP::iSelectPlottables);


    //    connect(ui->myGLWidget, SIGNAL(keyValue(QVector<double>)), this, SLOT(takeKeyValue(QVector<double>)));

    //    plot();

}
QVector<double> key;
QVector<double> nonEq;
QVector<double> eqHistValue;
QVector<double> luTableValue;
QString file1Name;

void Window::on_loadPushButton_clicked()
{
    QString widthLineEdit;
    QString heightLineEdit;
    QString depthLineEdit;
    bool ok;

    widthLineEdit = ui->widthLineEdit->text();
    heightLineEdit = ui->heightLineEdit->text();
    depthLineEdit = ui->depthLineEdit->text();

    ui->myGLWidget->initVol3DTex(file1Name.toStdString().c_str(),
                                 widthLineEdit.toInt(&ok, 10),
                                 heightLineEdit.toInt(&ok, 10),
                                 depthLineEdit.toInt(&ok, 10));
}

void Window::on_loadRAWButton_clicked()
{
    file1Name = QFileDialog::getOpenFileName(this, tr("Open RAW Data"), ".", tr("RAW File (*.raw)"));
    //ui->File1Path->setText(file1Name);

    //    QFile myFile(file1Name);
    //    int fileSize = 0;
    //    fileSize = (myFile.size() / 16.0) * 8.0;  //when file does open.
    //    myFile.close();
    //    qDebug() << "Size  is: " << fileSize;
    //    ui->myGLWidget->initVol3DTex(file1Name.toStdString().c_str(), fileSize);
}

void Window::on_histPushButton_clicked()
{
    eqHistogram();
    nonEqHistogram();

    plotHistogram(key, nonEq, eqHistValue, luTableValue);
}

void Window::eqHistogram()
{
    luTableValue = ui->myGLWidget->lutEqualizedValue();
    eqHistValue = ui->myGLWidget->equalizedValue();
}

void Window::nonEqHistogram()
{
    key = ui->myGLWidget->keyValue();
    nonEq = ui->myGLWidget->nonEqualizedValue();
}


void Window::plotHistogram(QVector<double> key, QVector<double> nonEq, QVector<double> eqValue, QVector<double> lutValue)
{
    QCustomPlot *histogramPlot = ui->histogramPlot;

    // Non Equalized
    QCPBars *nonEqHistBars = new QCPBars(histogramPlot->xAxis, histogramPlot->yAxis);
    histogramPlot->addPlottable(nonEqHistBars);
    nonEqHistBars->setWidth(1);
    nonEqHistBars->setData(key, nonEq);
    nonEqHistBars->setPen(Qt::NoPen);
    nonEqHistBars->setBrush(QColor(10, 140, 70, 160));

    //    // Equalized
    //    QCPBars *eqHistBars = new QCPBars(histogramPlot->xAxis, histogramPlot->yAxis);
    //    histogramPlot->addPlottable(eqHistBars);
    //    eqHistBars->setWidth(1);
    //    eqHistBars->setData(key, eqValue);
    //    eqHistBars->setPen(Qt::NoPen);
    //    eqHistBars->setBrush(QColor(10, 100, 50, 70));
    ////    eqHistBars->moveAbove(eqHistBars);

    //    // LUT
    //    QCPGraph *lut = histogramPlot->addGraph();
    //    lut->setData(key, lutValue);
    //    lut->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssNone, QPen(Qt::black, 1.5), QBrush(Qt::white), 9));
    //    lut->setLineStyle(QCPGraph::lsStepCenter);
    //    lut->setPen(QPen(QColor(120, 120, 120), 2));

    histogramPlot->replot();
    histogramPlot->rescaleAxes();
}

void Window::keyPressEvent(QKeyEvent * e)
{
    switch (e->key())
    {
    case Qt::Key_Escape: close(); break;
    case Qt::Key_1: ui->myGLWidget->keyPressed(1); break;
    case Qt::Key_2: ui->myGLWidget->keyPressed(2); break;
    case Qt::Key_3: ui->myGLWidget->keyPressed(3); break;
    case Qt::Key_4: ui->myGLWidget->keyPressed(4); break;
    case Qt::Key_5: ui->myGLWidget->keyPressed(5); break;
    }
}

Window::~Window()
{
    delete ui;
}
