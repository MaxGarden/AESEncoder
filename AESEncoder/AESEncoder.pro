CONFIG -= flat
TEMPLATE = app
TARGET = AESEncoder
BINDIR = $$OUT_PWD/bin
DESTDIR = $$BINDIR
QT += core gui widgets

CONFIG += precompile_header suppress_vcproj_warnings
QMAKE_CXXFLAGS += /std:c++latest

PRECOMPILED_HEADER = Source/pch.h

INCLUDEPATH += $$PWD/Source

SOURCES += \
    Source/main.cpp                                                                 \

HEADERS += \
    

FORMS += \