#if !defined(__AES_ASM_BASIC_OPERATIONS_H__)
#define __AES_ASM_BASIC_OPERATIONS_H__
#pragma once

#include "AESEncoder/AESBasicOperations.h"
#include "AESCppBasicOperations.h"

namespace AES
{
    class CAESAsmBasicOperations final : public IAESBasicOperations
    {
    public:
        CAESAsmBasicOperations() = default;
        virtual ~CAESAsmBasicOperations() override final = default;

        virtual void AddRoundKey(uint8_t* state, const uint8_t* expandedKey) const noexcept override final;
        virtual void SubBytes(uint8_t* state) const noexcept override final;
        virtual void ShiftRows(uint8_t* state) const noexcept override final;
        virtual void MixColumns(uint8_t* state) const noexcept override final;

        virtual uint8_t GMultiplication(uint8_t first, uint8_t second) const noexcept override final;
        virtual uint8_t GetSBoxValue(uint8_t index) const noexcept override final;

    private:
        CAESCppBasicOperations m_MockupOperations;
    };
}

#endif //__AES_ASM_BASIC_OPERATIONS_H__