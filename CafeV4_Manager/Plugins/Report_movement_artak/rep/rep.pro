#-------------------------------------------------
#
# Project created by QtCreator 2015-04-09T01:27:15
#
#-------------------------------------------------

QT       += gui widgets sql

TARGET = rep
TEMPLATE = lib

SOURCES += rep.cpp

HEADERS += rep.h

CONFIG += static dll

QMAKE_LFLAGS += -static-libgcc -static-libstdc++ -static -lpthread
