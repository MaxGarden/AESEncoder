#if !defined(__AES_ENCODER_H__)
#define __AES_ENCODER_H__
#pragma once

#include "Encoder/Encoder.h"

namespace AES
{
    enum class EKeyType
    {
        Bits128,
        Bits196,
        Bits256,

        __Count
    };

    using Key = std::string;

    class IAESEncoder : public IEncoder
    {
    public:
        virtual ~IAESEncoder() = default;

        virtual bool SetKeyType(EKeyType keyType) noexcept = 0;
        virtual EKeyType GetKeyType() const noexcept = 0;

        virtual bool SetKey(const Key& key) noexcept = 0;
        virtual const Key& GetKey() const noexcept = 0;

        static IAESEncoderUniquePtr Create();
    };
}

#endif //__AES_ENCODER_H__