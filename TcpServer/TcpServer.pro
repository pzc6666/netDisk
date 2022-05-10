#-------------------------------------------------
#
# Project created by QtCreator 2022-03-19T23:31:36
#
#-------------------------------------------------

QT       += core gui network sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = TcpServer
TEMPLATE = app


SOURCES += main.cpp\
        tcpserver.cpp \
    mytcpserver.cpp \
    mytcpsocket.cpp \
    protocol.cpp \
    opendb.cpp \
    log.cpp \
    logworker.cpp \
    mythreadpool.cpp

HEADERS  += tcpserver.h \
    mytcpserver.h \
    mytcpsocket.h \
    protocol.h \
    opendb.h \
    log.h \
    logworker.h \
    mythreadpool.h

FORMS    += tcpserver.ui

RESOURCES += \
    config.qrc
