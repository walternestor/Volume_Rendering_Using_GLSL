#-------------------------------------------------
#
# Project created by QtCreator 2015-09-13T14:27:49
#
#-------------------------------------------------

QT       += core gui opengl

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = ray_casting_2
TEMPLATE = app


SOURCES += main.cpp\
            window.cpp \
            glwidget.cpp

HEADERS  += window.h \
            glwidget.h

win32:INCLUDEPATH += "C:\Users\walter\Desktop\Qt\ray_casting_2\include"
# win32:INCLUDEPATH += "C:\Qt\Libraries\glew-1.10.0\include"
 win32:INCLUDEPATH += "C:\Libraries\glew-1.13.0\include"
# win32:LIBS        += -L"C:\Qt\Libraries\glew-1.10.0\lib" -lglew32
 win32:LIBS        += -L"C:\Libraries\glew-1.13.0\lib" -lglew32
 win32:LIBS        += -lopengl32
 win32:LIBS        += -lglu32
 win32:OBJECTS_DIR = .\obj
 win32:MOC_DIR = .\moc
 win32:RCC_DIR = .\rcc
 win32:DESTDIR = .\exec

DISTFILES += \
    shader/backface.frag \
    shader/raycasting.frag \
    shader/backface.vert \
    shader/raycasting.vert
