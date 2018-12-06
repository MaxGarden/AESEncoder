#if !defined(__AES_CPP_ENCODER_H__)
#define __AES_CPP_ENCODER_H__
#pragma once

#include "AESEncoderBase.h"

namespace AES
{
    class CAESCppEncoder final : public CAESEncoderBase
    {
    public:
        CAESCppEncoder() = default;
        virtual ~CAESCppEncoder() override final = default;

        virtual EncoderData Encode(const EncoderData& data) override final;

    private:
        using State = std::array<uint8_t, s_StateSize>;
        void EncodeState(State& state, const ExpandedKey& expandedKey, unsigned int roundNumber) const noexcept;

        void AddRoundKey(uint8_t* state, const uint8_t* expandedKey) const noexcept;
        void SubBytes(uint8_t* state) const noexcept;
        void ShiftRows(uint8_t* state) const noexcept;
        void MixColumns(uint8_t* state) const noexcept;
    };
}

#endif //__AES_CPP_ENCODER_H__