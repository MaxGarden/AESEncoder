#include "pch.h"
#include "AESAsmBasicOperations.h"

using namespace AES;

void AES::CAESAsmBasicOperations::AddRoundKey(uint8_t* state, const uint8_t* expandedKey) const noexcept
{
    m_MockupOperations.AddRoundKey(state, expandedKey);
}

void AES::CAESAsmBasicOperations::SubBytes(uint8_t* state) const noexcept
{
    m_MockupOperations.SubBytes(state);
}

void AES::CAESAsmBasicOperations::ShiftRows(uint8_t* state) const noexcept
{
    m_MockupOperations.ShiftRows(state);
}

void AES::CAESAsmBasicOperations::MixColumns(uint8_t* state) const noexcept
{
    m_MockupOperations.MixColumns(state);
}

uint8_t CAESAsmBasicOperations::GMultiplication(uint8_t first, uint8_t second) const noexcept
{
    return m_MockupOperations.GMultiplication(first, second);
}

uint8_t CAESAsmBasicOperations::GetSBoxValue(uint8_t index) const noexcept
{
    return m_MockupOperations.GetSBoxValue(index);
}