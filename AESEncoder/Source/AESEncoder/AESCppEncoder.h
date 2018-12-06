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

    protected:
        virtual bool EncodeDataChunk(uint8_t* dataChunk, size_t dataChunkSize, const ExpandedKey& expandedKey) const noexcept override final;

    private:
        void EncodeState(uint8_t* state, const uint8_t* expandedKey, unsigned int roundNumber) const noexcept;

        void AddRoundKey(uint8_t* state, const uint8_t* expandedKey) const noexcept;
        void SubBytes(uint8_t* state) const noexcept;
        void ShiftRows(uint8_t* state) const noexcept;
        void MixColumns(uint8_t* state) const noexcept;
    };
}

#endif //__AES_CPP_ENCODER_H__