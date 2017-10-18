#-------------------------------------------------
#
# Project created by QtCreator 2017-08-22T15:10:13
#
#-------------------------------------------------

QT       += core gui serialbus

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets
CONFIG += -std=c++11 console
TARGET = CanSimulator
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
    anLogger/src/anlogger.cpp \
    CanProtocol/src/canprotocol.cpp \
    shared/abstractstatemachinebasis.cpp \
    shared/commonthings.cpp \
    shared/directtransition.cpp \
    CanBusWorker/src/canbusworker.cpp \
    CanBusWorker/src/canbusworkerbasis.cpp \
    CanBusWorker/src/errorcanbusworker.cpp \
    CanBusWorker/src/frameiswritten.cpp \
    CanBusWorker/src/idlecanbusworker.cpp \
    CanBusWorker/src/runningcanbusworker.cpp \
    CanBusWorker/src/uninitiatedcanbusworker.cpp \
    CanBusWorker/src/writeframecanbusworker.cpp

HEADERS += \
        mainwindow.h \
    anLogger/src/anlogger.h \
    CanProtocol/src/canprotocol.h \
    shared/abstractstatemachinebasis.h \
    shared/commonthings.h \
    shared/directtransition.h \
    CanBusWorker/src/canbusworker.h \
    CanBusWorker/src/canbusworkerbasis.h \
    CanBusWorker/src/errorcanbusworker.h \
    CanBusWorker/src/frameiswritten.h \
    CanBusWorker/src/idlecanbusworker.h \
    CanBusWorker/src/runningcanbusworker.h \
    CanBusWorker/src/uninitiatedcanbusworker.h \
    CanBusWorker/src/writeframecanbusworker.h

FORMS += \
        mainwindow.ui
