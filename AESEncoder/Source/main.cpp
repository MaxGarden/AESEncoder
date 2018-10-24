#include "pch.h"
#include <QApplication>
#include "MainWindow/MainWindow.h"

#include "AESEncoder/AESEncoder.h"
#include "AESEncoder/AESEncoderController.h"
#include "AESEncoder/AESEncoderView.h"

int main(int argc, char** argv)
{
    QApplication app(argc, argv);
    CMainWindow mainWindow;

    auto registrationResult = true;

    registrationResult &= mainWindow.RegisterEncoder("AES", AES::IAESEncoder::Create());
    registrationResult &= mainWindow.RegisterEncoderController(AES::IAESEncoderController::Create());
    registrationResult &= mainWindow.RegisterEncoderView(AES::IAESEncoderView::Create());

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