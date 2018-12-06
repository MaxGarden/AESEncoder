#if !defined(__AES_ENCODER_BASE_H__)
#define __AES_ENCODER_BASE_H__
#pragma once

#include "AESEncoder.h"

namespace AES
{
    class CAESEncoderBase : public IAESEncoder
    {
    public:
        CAESEncoderBase() = default;
        virtual ~CAESEncoderBase() override = default;

        virtual bool SetKeyType(EKeyType keyType) noexcept override final;
        virtual EKeyType GetKeyType() const noexcept override final;

        virtual bool SetKey(const Key& key) noexcept override final;
        virtual const Key& GetKey() const noexcept override final;

    protected:
        using ExpandedKey = std::vector<uint8_t>;

        bool Setup() noexcept;
        ExpandedKey ExpandKey() const noexcept;
        uint8_t GMultiplication(uint8_t first, uint8_t second) const noexcept;

        unsigned int GetRoundsNumber() const noexcept;

    protected:
        static const std::array<uint8_t, 256> s_Sbox;
        static const auto s_StateDimension = 4u;
        static const auto s_StateSize = s_StateDimension * s_StateDimension;

    private:
        EKeyType m_KeyType = EKeyType::Bits128;
        Key m_Key = {};

        unsigned int m_KeyLenght = 32u;

        const unsigned int m_BlockLength = 16u;

        unsigned int m_ExpandKeyRoundsNumber = 8u;
        unsigned int m_RoundsNumber = 14u;
    };
}

#endif //__AES_ENCODER_BASE_H__