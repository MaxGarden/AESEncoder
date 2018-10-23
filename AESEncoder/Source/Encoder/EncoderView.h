#if !defined(__ENCODER_VIEW_H__)
#define __ENCODER_VIEW_H__
#pragma once

#include <QWidget>

class IEncoderView
{
public:
    virtual ~IEncoderView() = default;

    virtual bool SetController(const IEncoderControllerSharedPtr& controller) = 0;

    virtual QWidget* GetViewWidget() const noexcept = 0;
};

#endif //__ENCODER_VIEW_H__