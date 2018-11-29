#if !defined(__AES_BASIC_OPERATIONS_H__)
#define __AES_BASIC_OPERATIONS_H__
#pragma once

#include "Encoder/Encoder.h"

namespace AES
{
    class IAESBasicOperations
    {
    public:
        virtual ~IAESBasicOperations() = default;

        virtual void AddRoundKey(uint8_t* state, const uint8_t* expandedKey) const noexcept = 0;
        virtual void SubBytes(uint8_t* state) const noexcept = 0;
        virtual void ShiftRows(uint8_t* state) const noexcept = 0;
        virtual void MixColumns(uint8_t* state) const noexcept = 0;

        virtual uint8_t GMultiplication(uint8_t first, uint8_t second) const noexcept = 0;
        virtual uint8_t GetSBoxValue(uint8_t index) const noexcept = 0;
    };
}

#endif //__AES_BASIC_OPERATIONS_H__