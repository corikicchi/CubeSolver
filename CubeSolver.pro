#-------------------------------------------------
#
# Project created by QtCreator 2016-10-28T12:11:23
#
#-------------------------------------------------

QT       += core gui network opengl

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = CubeSolver
TEMPLATE = app


SOURCES += main.cpp\
        widget.cpp \
    solverthread.cpp \
    solver/calculateordinal.cpp \
    solver/cube.cpp \
    solver/cubeparser.cpp \
    solver/groupcube.cpp \
    solver/idastarsearch.cpp \
    solver/movetable.cpp \
    solver/ordinalcube.cpp \
    solver/printvector.cpp \
    solver/pruningtable.cpp \
    opengl/glwidget.cpp

HEADERS  += widget.h \
    solverthread.h \
    solver/cube.h \
    solver/cubeparser.h \
    solver/groupcube.h \
    solver/movetable.h \
    solver/printvector.h \
    solver/pruningtable.h \
    solver/submovetable.h \
    solver/timer.h \
    solver/calculateordinal.h \
    solver/idastarsearch.h \
    solver/ordinalcube.h \
    opengl/glwidget.h

FORMS    += widget.ui

LIBS += opengl32.lib glu32.lib
