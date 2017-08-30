#-------------------------------------------------
#
# Project created by QtCreator 2017-08-22T15:10:13
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets serialbus
CONFIG += -std=c++11 console
TARGET = CanSimulatorExec
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


SOURCES += \
        main.cpp \
        mainwindow.cpp \
    canprotocol.cpp \
    CanBusWorker/canbusworker.cpp \
    CanBusWorker/canbusworkerdb.cpp \
    CanBusWorker/connectdevice.cpp \
    CanBusWorker/createdevice.cpp \
    CanBusWorker/deviceconnected.cpp \
    CanBusWorker/directtransition.cpp \
    CanBusWorker/framesent.cpp \
    CanBusWorker/readframe.cpp \
    CanBusWorker/waitforerrorhandler.cpp \
    CanBusWorker/waitforpluginandinterface.cpp \
    CanBusWorker/writeframe.cpp \
    CanBusWorker/writeaframe.cpp

HEADERS += \
        mainwindow.h \
    anlogger.h \
    canprotocol.h \
    commonthings.h \
    CanBusWorker/canbusworker.h \
    CanBusWorker/canbusworkerdb.h \
    CanBusWorker/connectdevice.h \
    CanBusWorker/createdevice.h \
    CanBusWorker/deviceconnected.h \
    CanBusWorker/directtransition.h \
    CanBusWorker/framesent.h \
    CanBusWorker/readframe.h \
    CanBusWorker/waitforerrorhandler.h \
    CanBusWorker/waitforpluginandinterface.h \
    CanBusWorker/writeframe.h \
    CanBusWorker/writeaframe.h

FORMS += \
        mainwindow.ui
