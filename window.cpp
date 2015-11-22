
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

//void Window::plot()
//{
//    QCustomPlot *customPlot = ui->customPlot;

//    // prepare data:
//    QVector<double> x1(20), y1(20);
//    QVector<double> x2(100), y2(100);
//    QVector<double> x3(20), y3(20);
//    QVector<double> x4(20), y4(20);
//    for (int i=0; i<x1.size(); ++i)
//    {
//      x1[i] = i/(double)x1.size()*10;
//      y1[i] = qCos(x1[i]*0.8+qSin(x1[i]*0.16+1.0))*qSin(x1[i]*0.54)+1.4;
//    }
//    for (int i=0; i<x2.size(); ++i)
//    {
//      x2[i] = i/(double)x2.size()*10;
//      y2[i] = qCos(x2[i]*0.85+qSin(x2[i]*0.165+1.1))*qSin(x2[i]*0.50)+1.7;
//    }
//    for (int i=0; i<x3.size(); ++i)
//    {
//      x3[i] = i/(double)x3.size()*10;
//      y3[i] = 0.05+3*(0.5+qCos(x3[i]*x3[i]*0.2+2)*0.5)/(double)(x3[i]+0.7)+qrand()/(double)RAND_MAX*0.01;
//    }
//    for (int i=0; i<x4.size(); ++i)
//    {
//      x4[i] = x3[i];
//      y4[i] = (0.5-y3[i])+((x4[i]-2)*(x4[i]-2)*0.02);
//    }

////    // create and configure plottables:
////    // Linha com Pontos
////    QCPGraph *graph1 = customPlot->addGraph();
////    graph1->setData(x1, y1);
////    graph1->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCircle, QPen(Qt::black, 1.5), QBrush(Qt::white), 9));
////    graph1->setPen(QPen(QColor(120, 120, 120), 2));

//    // Area esbranquiçada acima da linha com pontos
////    QCPGraph *graph2 = customPlot->addGraph();
////    graph2->setData(x2, y2);
////    graph2->setPen(Qt::NoPen);
////    graph2->setBrush(QColor(200, 200, 200, 20));
////    graph2->setChannelFillGraph(graph1);

//    // Barras da frente
//    QCPBars *bars1 = new QCPBars(customPlot->xAxis, customPlot->yAxis);
//    customPlot->addPlottable(bars1);
//    bars1->setWidth(9/(double)x3.size());
//    bars1->setData(x3, y3);
//    bars1->setPen(Qt::NoPen);
//    bars1->setBrush(QColor(10, 140, 70, 160));

//    // Barras apagadas atras
////    QCPBars *bars2 = new QCPBars(customPlot->xAxis, customPlot->yAxis);
////    customPlot->addPlottable(bars2);
////    bars2->setWidth(9/(double)x4.size());
////    bars2->setData(x4, y4);
////    bars2->setPen(Qt::NoPen);
////    bars2->setBrush(QColor(10, 100, 50, 70));
////    bars2->moveAbove(bars1);

////    // move bars above graphs and grid below bars:
////    customPlot->addLayer("abovemain", customPlot->layer("main"), QCustomPlot::limAbove);
////    customPlot->addLayer("belowmain", customPlot->layer("main"), QCustomPlot::limBelow);
////    graph1->setLayer("abovemain");
////    customPlot->xAxis->grid()->setLayer("belowmain");
////    customPlot->yAxis->grid()->setLayer("belowmain");

////    // set some pens, brushes and backgrounds:
////    customPlot->xAxis->setBasePen(QPen(Qt::white, 1));
////    customPlot->yAxis->setBasePen(QPen(Qt::white, 1));
////    customPlot->xAxis->setTickPen(QPen(Qt::white, 1));
////    customPlot->yAxis->setTickPen(QPen(Qt::white, 1));
////    customPlot->xAxis->setSubTickPen(QPen(Qt::white, 1));
////    customPlot->yAxis->setSubTickPen(QPen(Qt::white, 1));
////    customPlot->xAxis->setTickLabelColor(Qt::white);
////    customPlot->yAxis->setTickLabelColor(Qt::white);
////    customPlot->xAxis->grid()->setPen(QPen(QColor(140, 140, 140), 1, Qt::DotLine));
////    customPlot->yAxis->grid()->setPen(QPen(QColor(140, 140, 140), 1, Qt::DotLine));
////    customPlot->xAxis->grid()->setSubGridPen(QPen(QColor(80, 80, 80), 1, Qt::DotLine));
////    customPlot->yAxis->grid()->setSubGridPen(QPen(QColor(80, 80, 80), 1, Qt::DotLine));
////    customPlot->xAxis->grid()->setSubGridVisible(true);
////    customPlot->yAxis->grid()->setSubGridVisible(true);
////    customPlot->xAxis->grid()->setZeroLinePen(Qt::NoPen);
////    customPlot->yAxis->grid()->setZeroLinePen(Qt::NoPen);
////    customPlot->xAxis->setUpperEnding(QCPLineEnding::esSpikeArrow);
////    customPlot->yAxis->setUpperEnding(QCPLineEnding::esSpikeArrow);
////    QLinearGradient plotGradient;
////    plotGradient.setStart(0, 0);
////    plotGradient.setFinalStop(0, 350);
////    plotGradient.setColorAt(0, QColor(80, 80, 80));
////    plotGradient.setColorAt(1, QColor(50, 50, 50));
////    customPlot->setBackground(plotGradient);
////    QLinearGradient axisRectGradient;
////    axisRectGradient.setStart(0, 0);
////    axisRectGradient.setFinalStop(0, 350);
////    axisRectGradient.setColorAt(0, QColor(80, 80, 80));
////    axisRectGradient.setColorAt(1, QColor(30, 30, 30));
////    customPlot->axisRect()->setBackground(axisRectGradient);

////    customPlot->rescaleAxes();
////    customPlot->yAxis->setRange(0, 2);
//}

//#undef QT_NO_OPENGL



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


