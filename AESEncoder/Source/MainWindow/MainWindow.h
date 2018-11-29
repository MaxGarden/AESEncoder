#if !defined(__MAIN_WINDOW_H__)
#define __MAIN_WINDOW_H
#pragma once

#include <QMainWindow>
#include "Encoder/Encoder.h"
#include "ui_MainWindow.h"

class CMainWindow final : public QMainWindow, public Ui::MainWindow
{
    Q_OBJECT

public:
    CMainWindow();
    virtual ~CMainWindow() override final = default;

    bool RegisterEncoder(const QString& name, IEncoderUniquePtr&& encoder);
    bool UnregisterEncoder(const QString& name);

    bool RegisterEncoderController(IEncoderControllerUniquePtr&& controller);
    bool RegisterEncoderView(IEncoderViewUniquePtr&& view);

    bool Initialize();

private:
    void SelectEncoder(IEncoderSharedPtr encoder);

    void ResetControllers();
    void ResetViews();

    IEncoderControllerSharedPtr FindMatchingController(const IEncoderSharedPtr& encoder) const noexcept;
    IEncoderView* FindMatchingView(const IEncoderControllerSharedPtr& controller) const noexcept;

    void SetEncoderViewWidget(QWidget* viewWidget);

    void OnEncoderRegistered(const QString& name, IEncoder& encoder);
    void OnEncoderUnregistered(const QString& name, IEncoder& encoder);

    void EncodeFile(const QString& savePath) const;
    bool OpenFileToEncode(const QString& filename, EncoderData& dataToEncode) const;
    bool SaveEncodedDataToFile(const QString& filename, const EncoderData& encodedData) const;

private slots:
    void OnEncoderChanged(const QString& name);
    void OnEncodeButtonClicked();
    void OnEncodeAsButtonClicked();
    void OnBrowseFileToEncodeButtonClicked();

private:
    bool m_Initialized = false;

    IEncoderSharedPtr m_CurrentEncoder;
    QWidget* m_CurrentViewWidget = nullptr;

    std::map<QString, IEncoderSharedPtr> m_Encoders;

    std::vector<IEncoderControllerSharedPtr> m_ControllersPool;
    std::vector<IEncoderViewUniquePtr> m_ViewsPool;
};

#endif //__MAIN_WINDOW_H__