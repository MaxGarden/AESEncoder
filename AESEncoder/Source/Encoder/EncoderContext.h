#if !defined(__ENCODER_CONEXT_H__)
#define __ENCODER_CONEXT_H__
#pragma once

#include <QString>

class IEncoderContext
{
public:
    virtual ~IEncoderContext() = default;

    virtual bool SetEncoder(IEncoderUniquePtr&& encoder) = 0;
    virtual const IEncoderSharedPtr& GetEncoder() const noexcept = 0;

    virtual bool SetController(IEncoderControllerUniquePtr&& controller) = 0;
    virtual const IEncoderControllerSharedPtr& GetController() const noexcept = 0;

    virtual bool SetView(IEncoderViewUniquePtr&& view) = 0;
    virtual const IEncoderViewSharedPtr& GetView() const noexcept = 0;

    virtual const QString& GetName() const noexcept = 0;

    static IEncoderContextUniquePtr Create(QString&& name);
};

#endif //__ENCODER_CONEXT_H__