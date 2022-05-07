#-------------------------------------------------
#
# Project created by QtCreator 2022-03-19T15:42:34
#
#-------------------------------------------------

QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = TcpClient
TEMPLATE = app


SOURCES += main.cpp\
        tcpclient.cpp \
    protocol.cpp \
    operatewidget.cpp \
    online.cpp \
    friend.cpp \
    book.cpp \
    privatechat.cpp \
    sharefile.cpp

HEADERS  += tcpclient.h \
    protocol.h \
    operatewidget.h \
    online.h \
    friend.h \
    book.h \
    privatechat.h \
    sharefile.h

FORMS    += tcpclient.ui \
    online.ui \
    privatechat.ui

RESOURCES += \
    config.qrc \
    res.qrc \
    ico.qrc

RC_ICONS = ./favicon.ico
