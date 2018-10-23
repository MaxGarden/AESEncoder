#if !defined(__MAIN_WINDOW_H__)
#define __MAIN_WINDOW_H
#pragma once

#include <QMainWindow>
#include "ui_MainWindow.h"

class MainWindow final : public QMainWindow, public Ui::MainWindow
{
    Q_OBJECT

public:
    MainWindow();
    virtual ~MainWindow() override final = default;
};

#endif //__MAIN_WINDOW_H__