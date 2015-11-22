#-------------------------------------------------
#
# Project created by QtCreator 2015-09-13T14:27:49
#
#-------------------------------------------------

QT       += core gui opengl
CONFIG   += console

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets printsupport

QMAKE_LFLAGS += -fopenmp
QMAKE_CXXFLAGS += -fopenmp
LIBS += -fopenmp
#TARGET = MyOpenGL

TARGET = ray_casting_2
TEMPLATE = app

SOURCES += main.cpp\
           window.cpp \
           myglwidget.cpp \
    qcustomplot.cpp


HEADERS  += window.h \
            myglwidget.h \
    qcustomplot.h

FORMS    += window.ui

DISTFILES += \
    shader/backface.frag \
    shader/raycasting.frag \
    shader/backface.vert \
    shader/raycasting.vert \
    shader/raycast_iso_super.frag

 win32:INCLUDEPATH += "C:\Users\walter\Desktop\Qt\ray_casting_2\include"
 win32:INCLUDEPATH += "C:\Libraries\glew-1.13.0\include"
 win32:LIBS        += -L"C:\Libraries\glew-1.13.0\lib" -lglew32
 win32:LIBS        += -lopengl32
 win32:LIBS        += -lglu32
 win32:OBJECTS_DIR = .\obj
 win32:MOC_DIR = .\moc
 win32:RCC_DIR = .\rcc
 win32:DESTDIR = .\exec
