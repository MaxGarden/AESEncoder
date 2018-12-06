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

    protected:
        virtual bool EncodeDataChunk(uint8_t* dataChunk, size_t dataChunkSize, const ExpandedKey& expandedKey) const noexcept override final;
    };
}

#endif //__AES_ASM_ENCODER_H__