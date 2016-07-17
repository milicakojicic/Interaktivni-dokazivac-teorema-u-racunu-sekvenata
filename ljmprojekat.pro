#-------------------------------------------------
#
# Project created by QtCreator 2016-07-15T17:13:47
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = ljmprojekat
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    lexer.cpp \
    parser.cpp \
    sequent.cpp \
    node.cpp

HEADERS  += mainwindow.h \
    parser.hpp \
    fol.hpp \
    sequent.h \
    node.h

FORMS    += mainwindow.ui

QMAKE_CXXFLAGS += -std=c++11



