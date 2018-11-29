#include "pch.h"
#include <QApplication>
#include "MainWindow/MainWindow.h"

#include "AESEncoder/AESEncoder.h"
#include "AESEncoder/AESEncoderController.h"
#include "AESEncoder/AESEncoderView.h"

#include "AESEncoder/BasicOperationsImplementation/AESCppBasicOperations.h"
#include "AESEncoder/BasicOperationsImplementation/AESAsmBasicOperations.h"

#include <fstream>

int main(int argc, char** argv)
{
    QApplication app(argc, argv);
    CMainWindow mainWindow;

    auto registrationResult = true;
    registrationResult &= mainWindow.RegisterEncoderController(AES::IAESEncoderController::Create());
    registrationResult &= mainWindow.RegisterEncoderView(AES::IAESEncoderView::Create());

    registrationResult &= mainWindow.RegisterEncoder("AES-cpp", AES::IAESEncoder::Create(std::make_unique<AES::CAESCppBasicOperations>()));
    registrationResult &= mainWindow.RegisterEncoder("AES-asm", AES::IAESEncoder::Create(std::make_unique<AES::CAESAsmBasicOperations>()));

    EDITOR_ASSERT(registrationResult);
    if (!registrationResult)
        return -1;

    const auto initializeResult = mainWindow.Initialize();
    EDITOR_ASSERT(initializeResult);
    if (!initializeResult)
        return -2;

    mainWindow.show();
    return app.exec();
}