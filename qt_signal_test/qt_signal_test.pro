QT += core
QT += widgets
QT += quick
QT -= gui

TARGET = qt_signal_test
CONFIG += console
CONFIG -= app_bundle
CONFIG += c++11

TEMPLATE = app

SOURCES += main.cpp \
    mainapplication.cpp \
    myobject.cpp

HEADERS += \
    mainapplication.h \
    myobject.h

DISTFILES +=

