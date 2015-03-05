#-------------------------------------------------
#
# Project created by QtCreator 2015-03-05T14:37:11
#
#-------------------------------------------------

QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = BaseStation
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    Protocols/rest_network.cpp \
    Joystick/joystick.cc

HEADERS  += mainwindow.h \
    Protocols/rest_network.h \
    Joystick/joystick.hh

FORMS    += mainwindow.ui
