#-------------------------------------------------
#
# Project created by QtCreator 2016-10-28T12:11:23
#
#-------------------------------------------------

QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = CubeSolver
TEMPLATE = app


SOURCES += main.cpp\
        widget.cpp \
    solverthread.cpp

HEADERS  += widget.h \
    solverthread.h

FORMS    += widget.ui
