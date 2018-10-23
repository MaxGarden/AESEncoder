#if !defined(__ENCODER_H__)
#define __ENCODER_H__
#pragma once

#include "EncoderSettings.h"

using EncoderData = std::vector<byte>;

class IEncoder
{
public:
    virtual ~IEncoder() = default;

    virtual bool SetSettings(IEncoderSettingsSharedPtr settings) = 0;
    virtual EncoderData Encode(const EncoderData& data) = 0;
};

#endif //__ENCODER_H__