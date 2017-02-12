#-------------------------------------------------
#
# Project created by QtCreator 2017-01-24T18:38:06
#
#-------------------------------------------------

QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Temp
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    hostdialog.cpp \
    hinfo.cpp \
    debugform.cpp

HEADERS  += mainwindow.h \
    hostdialog.h \
    hinfo.h \
    debugform.h

FORMS    += mainwindow.ui \
    hostdialog.ui \
    debugform.ui
