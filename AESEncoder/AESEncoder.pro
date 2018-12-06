CONFIG -= flat
TEMPLATE = app
TARGET = AESEncoder
BINDIR = $$OUT_PWD/bin
DESTDIR = $$BINDIR
QT += core gui widgets

CONFIG += precompile_header suppress_vcproj_warnings
QMAKE_CXXFLAGS += /std:c++latest

PRECOMPILED_HEADER = Source/pch.h

INCLUDEPATH += $$PWD/Source $$PWD/Include

Debug:LIBS += -L$$PWD/Libraries -lAESAsm_d
Release:LIBS += -L$$PWD/Libraries -lAESAsm

EXTRA_BINFILES += \
        $$PWD/Binaries/AESAsm.dll \
        $$PWD/Binaries/AESAsm_d.dll
EXTRA_BINFILES_WIN = $${EXTRA_BINFILES}
EXTRA_BINFILES_WIN ~= s,/,\\,g
DESTDIR_WIN = $${BINDIR}
DESTDIR_WIN ~= s,/,\\,g
for(FILE,EXTRA_BINFILES_WIN){
    QMAKE_POST_LINK +=$$quote(cmd /c copy /y $${FILE} $${DESTDIR_WIN}$$escape_expand(\n\t))
    }

SOURCES += \
    Source/main.cpp                                                                 \     
    Source/MainWindow/MainWindow.cpp                                                \
    Source/AESEncoder/AESEncoder.cpp                                                \
    Source/AESEncoder/AESEncoderBase.cpp                                            \
    Source/AESEncoder/AESCppEncoder.cpp                                             \
    Source/AESEncoder/AESAsmEncoder.cpp                                             \
    Source/AESEncoder/AESEncoderController.cpp                                      \
    Source/AESEncoder/AESEncoderView.cpp                                            \
    Source/AESEncoder/Widgets/AESEncoderViewWidget.cpp                              \

HEADERS += \
    Source/Pointers.h                                                               \
    Source/MainWindow/MainWindow.h                                                  \
    Source/Encoder/Encoder.h                                                        \
    Source/Encoder/EncoderController.h                                              \
    Source/Encoder/EncoderView.h                                                    \
    Source/AESEncoder/AESEncoder.h                                                  \
    Source/AESEncoder/AESEncoderBase.h                                              \
    Source/AESEncoder/AESCppEncoder.h                                               \
    Source/AESEncoder/AESAsmEncoder.h                                               \
    Source/AESEncoder/AESBasicOperations.h                                          \
    Source/AESEncoder/AESEncoderController.h                                        \
    Source/AESEncoder/AESEncoderView.h                                              \
    Source/AESEncoder/Widgets/AESEncoderViewWidget.h                                \
    

FORMS += \
    Source/MainWindow/UI/MainWindow.ui                                              \
    Source/AESEncoder/UI/AESEncoderView.ui                                          \