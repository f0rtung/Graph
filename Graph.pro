#-------------------------------------------------
#
# Project created by QtCreator 2015-02-13T17:46:29
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets printsupport

TARGET = Graph
TEMPLATE = app


SOURCES += main.cpp\
        widget.cpp \
    qcustomplot.cpp

HEADERS  += widget.h \
    qcustomplot.h

FORMS    += widget.ui
CONFIG += c++11

RESOURCES += \
    graphres.qrc
