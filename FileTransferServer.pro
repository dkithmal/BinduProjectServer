#-------------------------------------------------
#
# Project created by QtCreator 2013-08-30T17:40:02
#
#-------------------------------------------------

QT       += core
QT       += network
QT       -= gui
QT       += xml

TARGET = FileTransferServer
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app


SOURCES += main.cpp \
    myserver.cpp \
    mythread.cpp

HEADERS += \
    myserver.h \
    mythread.h
