#include "pch.h"
#include "AESCppEncoder.h"

using namespace AES;

bool CAESCppEncoder::EncodeDataChunk(uint8_t* dataChunk, size_t dataChunkSize, const ExpandedKey& expandedKey) const noexcept
{
    const auto expandedKeyData = expandedKey.data();
    const auto roundsNumber = GetRoundsNumber();

    for (auto offset = 0u; offset < dataChunkSize; offset += s_StateSize)
        EncodeState(&dataChunk[offset], expandedKeyData, roundsNumber);

    return true;
}

void CAESCppEncoder::EncodeState(uint8_t* state, const uint8_t* expandedKey, unsigned int roundsNumber) const noexcept
{
    AddRoundKey(state, expandedKey);

    for (auto i = 1u; i < roundsNumber; ++i)
    {
        SubBytes(state);
        ShiftRows(state);
        MixColumns(state);
        AddRoundKey(state, expandedKey + s_StateSize * i);
    }

    SubBytes(state);
    ShiftRows(state);
    AddRoundKey(state, expandedKey + s_StateSize * roundsNumber);
}

void CAESCppEncoder::AddRoundKey(uint8_t* state, const uint8_t* expandedKey) const noexcept
{
    for (auto i = 0u; i < s_StateSize; ++i)
        state[i] ^= expandedKey[i];
}

void CAESCppEncoder::SubBytes(uint8_t* state) const noexcept
{
    for (auto i = 0u; i < s_StateSize; ++i)
    {
        const auto row = (state[i] & 0xf0) >> 4;
        const auto column = state[i] & 0x0f;
        state[i] = s_Sbox[16 * row + column];
    }
}

void CAESCppEncoder::ShiftRows(uint8_t* state) const noexcept
{
    for (auto i = 1u; i < s_StateDimension; ++i)
    {
        for (auto s = 0u; s < i; ++s)
        {
            const auto buffer = state[i];

            for (auto k = 1u; k < s_StateDimension; ++k)
                state[i + (k - 1) * s_StateDimension] = state[i + k * s_StateDimension];

            state[i + (s_StateDimension - 1) * s_StateDimension] = buffer;
        }
    }
}

void CAESCppEncoder::MixColumns(uint8_t* state) const noexcept
{
    const auto xorMultiplication = [this](auto& destination, const auto& first, const auto& second)
    {
        destination[0] = GMultiplication(first[0], second[0]) ^ GMultiplication(first[3], second[1]) ^ GMultiplication(first[2], second[2]) ^ GMultiplication(first[1], second[3]);
        destination[1] = GMultiplication(first[1], second[0]) ^ GMultiplication(first[0], second[1]) ^ GMultiplication(first[3], second[2]) ^ GMultiplication(first[2], second[3]);
        destination[2] = GMultiplication(first[2], second[0]) ^ GMultiplication(first[1], second[1]) ^ GMultiplication(first[0], second[2]) ^ GMultiplication(first[3], second[3]);
        destination[3] = GMultiplication(first[3], second[0]) ^ GMultiplication(first[2], second[1]) ^ GMultiplication(first[1], second[2]) ^ GMultiplication(first[0], second[3]);
    };

    static_assert(s_StateDimension == 4, "Mixing columns of matrix that dimension is other than 4 is not supported.");

    static const std::array<uint8_t, 4> magicArray{ 0x02, 0x01, 0x01, 0x03 };
    static uint8_t column[s_StateDimension];
    static uint8_t result[s_StateDimension];

    for (auto j = 0u; j < s_StateDimension; ++j)
    {
        for (auto i = 0; i < s_StateDimension; ++i)
            column[i] = state[s_StateDimension * j + i];

        xorMultiplication(result, magicArray, column);

        for (auto i = 0u; i < s_StateDimension; ++i)
            state[s_StateDimension * j + i] = result[i];
    }
}