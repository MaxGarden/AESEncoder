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
    Source/MainWindow/MainWindow.cpp                                                \
    Source/AESEncoder/AESEncoder.cpp                                                \
    Source/AESEncoder/AESEncoderController.cpp                                      \
    Source/AESEncoder/AESEncoderView.cpp                                            \
    Source/AESEncoder/Widgets/AESEncoderViewWidget.cpp                              \
    Source/AESEncoder/BasicOperationsImplementation/AESCppBasicOperations.cpp       \
    Source/AESEncoder/BasicOperationsImplementation/AESAsmBasicOperations.cpp       \

HEADERS += \
    Source/Pointers.h                                                               \
    Source/MainWindow/MainWindow.h                                                  \
    Source/Encoder/Encoder.h                                                        \
    Source/Encoder/EncoderController.h                                              \
    Source/Encoder/EncoderView.h                                                    \
    Source/AESEncoder/AESEncoder.h                                                  \
    Source/AESEncoder/AESBasicOperations.h                                          \
    Source/AESEncoder/AESEncoderController.h                                        \
    Source/AESEncoder/AESEncoderView.h                                              \
    Source/AESEncoder/Widgets/AESEncoderViewWidget.h                                \
    Source/AESEncoder/BasicOperationsImplementation/AESCppBasicOperations.h         \
    Source/AESEncoder/BasicOperationsImplementation/AESAsmBasicOperations.h         \
    

FORMS += \
    Source/MainWindow/UI/MainWindow.ui                                              \
    Source/AESEncoder/UI/AESEncoderView.ui                                          \