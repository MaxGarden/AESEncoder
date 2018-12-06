#if !defined(__AES_ENCODER_VIEW_WIDGET_H__)
#define __AES_ENCODER_VIEW_WIDGET_H__
#pragma once

#include "ui_AESEncoderView.h"
#include "AESEncoder/AESEncoderController.h"

namespace AES
{
    class CAESEncoderViewWidget final : public QWidget, public Ui::AESEncoderView
    {
        Q_OBJECT

    public:
        CAESEncoderViewWidget(QWidget* parent);
        virtual ~CAESEncoderViewWidget() override final = default;

        void SetController(IAESEncoderController* controller);

    private:
        void ResetView();
        bool TryApplySettingsToController();

    private slots:
        void OnKeyTypeChanged(int index);
        void OnKeyChanged(const QString& key);

    private:
        IAESEncoderController * m_Controller = nullptr;
    };
}

#endif //__AES_ENCODER_VIEW_WIDGET_H__