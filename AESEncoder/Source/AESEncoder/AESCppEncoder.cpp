#include "pch.h"
#include "AESCppEncoder.h"

using namespace AES;

EncoderData CAESCppEncoder::Encode(const EncoderData& data)
{
    EncoderData result;

    if (!Setup())
        return result;

    const auto paddingSize = s_StateSize - (data.size() % s_StateSize);
    result.resize(data.size() + paddingSize);

    const auto expandedKey = ExpandKey();

    const auto roundsNumber = GetRoundsNumber();

    const auto dataSize = data.size();
    for (auto offset = 0u; offset < dataSize; offset += s_StateSize)
    {
        State state;

        for (auto i = 0u; i < s_StateDimension; ++i)
        {
            for (auto j = 0u; j < s_StateDimension; ++j)
            {
                const auto dataIndex = offset + i + s_StateDimension * j;
                state[s_StateDimension * i + j] = dataIndex < data.size() ? data[dataIndex] : 0;
            }
        }

        EncodeState(state, expandedKey, roundsNumber);

        for (auto i = 0u; i < s_StateDimension; ++i)
        {
            for (auto j = 0u; j < s_StateDimension; ++j)
                result[offset + i + s_StateDimension * j] = state[s_StateDimension * i + j];
        }
    }

    return result;
}

void CAESCppEncoder::EncodeState(State& state, const ExpandedKey& expandedKey, unsigned int roundsNumber) const noexcept
{
    AddRoundKey(state.data(), expandedKey.data());

    for (auto i = 1u; i < roundsNumber; ++i)
    {
        SubBytes(state.data());
        ShiftRows(state.data());
        MixColumns(state.data());
        AddRoundKey(state.data(), expandedKey.data() + s_StateSize * i);
    }

    SubBytes(state.data());
    ShiftRows(state.data());
    AddRoundKey(state.data(), expandedKey.data() + s_StateSize * roundsNumber);
}

void CAESCppEncoder::AddRoundKey(uint8_t* state, const uint8_t* expandedKey) const noexcept
{
    for (auto i = 0u; i < s_StateDimension; ++i)
    {
        for (auto x = 0u; x < s_StateDimension; ++x)
            state[s_StateDimension * x + i] = state[s_StateDimension * x + i] ^ expandedKey[s_StateDimension * i + x];
    }
}

void CAESCppEncoder::SubBytes(uint8_t* state) const noexcept
{
    for (auto i = 0u; i < s_StateDimension; ++i)
    {
        for (auto j = 0u; j < s_StateDimension; ++j)
        {
            const auto row = (state[s_StateDimension*i + j] & 0xf0) >> 4;
            const auto column = state[s_StateDimension*i + j] & 0x0f;
            state[s_StateDimension*i + j] = s_Sbox[16 * row + column];
        }
    }
}

void CAESCppEncoder::ShiftRows(uint8_t* state) const noexcept
{
    for (auto i = 1u; i < s_StateDimension; ++i)
    {
        for (auto s = 0u; s < i; ++s)
        {
            const auto buffer = state[s_StateDimension * i + 0];

            for (auto k = 1u; k < s_StateDimension; ++k)
                state[s_StateDimension * i + k - 1] = state[s_StateDimension * i + k];

            state[s_StateDimension * i + s_StateDimension - 1] = buffer;
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

    std::array<uint8_t, 4> a{ 0x02, 0x01, 0x01, 0x03 };
    uint8_t col[s_StateDimension], res[s_StateDimension];

    for (auto j = 0u; j < s_StateDimension; ++j)
    {
        for (auto i = 0u; i < s_StateDimension; ++i)
            col[i] = state[s_StateDimension*i + j];

        xorMultiplication(res, a, col);

        for (auto i = 0u; i < s_StateDimension; ++i)
            state[s_StateDimension*i + j] = res[i];
    }
}