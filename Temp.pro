#-------------------------------------------------
#
# Project created by QtCreator 2017-01-24T18:38:06
#
#-------------------------------------------------

VERSION = 1.2.0

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

RESOURCES += \
    qttemp.qrc

DEFINES += APP_VERSION=\\\"$$VERSION\\\"

win32:RC_ICONS += \
    c.ico

win32:QMAKE_TARGET_COMPANY += \
    "JFO Soft"

win32:QMAKE_TARGET_COPYRIGHT += \
    "Fredrik Olausson <fredrik@bluppfisk.org>"

win32:RC_LANG += \
    0x041D
