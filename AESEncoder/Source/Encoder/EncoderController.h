#if !defined(__ENCODER_CONTROLLER_H__)
#define __ENCODER_CONTROLLER_H__
#pragma once

class IEncoderController
{
public:
    virtual ~IEncoderController() = default;

    virtual bool SetEncoder(const IEncoderSharedPtr& encoder) = 0;
};

#endif //__ENCODER_CONTROLLER_H__