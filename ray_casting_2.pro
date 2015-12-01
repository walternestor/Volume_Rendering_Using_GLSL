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
    shader/backface.vert \
    shader/raycasting.vert \
    shader/raycasting_gray.frag \
    shader/raycasting_rgba.frag \
    shader/raycasting_direct_surface_render.frag \
    shader/raycasting_direct_surface_vector_color_render.frag

 INCLUDEPATH += "C:\Users\walter\Desktop\Qt\ray_casting_2\include"
 INCLUDEPATH += "C:\Libraries\glew-1.13.0\include"
 LIBS        += -L"C:\Libraries\glew-1.13.0\lib" -lglew32
 LIBS        += -lopengl32
 LIBS        += -lglu32
 LIBS += -fopenmp
 OBJECTS_DIR = .\obj
 MOC_DIR = .\moc
 RCC_DIR = .\rcc
 DESTDIR = .\exec
