#if !defined(__AES_ASM_ENCODER_H__)
#define __AES_ASM_ENCODER_H__
#pragma once

#include "AESEncoderBase.h"

namespace AES
{
    class CAESAsmEncoder final : public CAESEncoderBase
    {
    public:
        CAESAsmEncoder() = default;
        virtual ~CAESAsmEncoder() override final = default;

        virtual EncoderData Encode(const EncoderData& data) override final;
    };
}

#endif //__AES_ASM_ENCODER_H__