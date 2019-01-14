#if !defined(__AES_ENCODER_H__)
#define __AES_ENCODER_H__
#pragma once

#include "Encoder/Encoder.h"

namespace AES
{
    enum class EKeyType
    {
        Bits128,
        Bits192,
        Bits256,

        __Count
    };

    using Key = std::string;

    class IAESEncoder : public IEncoder
    {
    public:
        virtual ~IAESEncoder() = default;

        virtual void SetKeyType(EKeyType keyType) noexcept = 0;
        virtual EKeyType GetKeyType() const noexcept = 0;

        virtual void SetKey(const Key& key) noexcept = 0;
        virtual const Key& GetKey() const noexcept = 0;
    };
}

#endif //__AES_ENCODER_H__