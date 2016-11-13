#-------------------------------------------------
#
# Project created by QtCreator 2016-11-13T06:22:18
#
#-------------------------------------------------

QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = QMultisweeper
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    multiplayer.cpp

HEADERS  += mainwindow.h \
    board.h \
    multiplayer.h

FORMS    += mainwindow.ui \
    multiplayer.ui
