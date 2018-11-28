#include "pch.h"
#include "AESEncoder.h"

using namespace AES;


class CAESEncoder final : public IAESEncoder
{
    static const auto s_StateDimension = 4u;
    static const auto s_StateSize = s_StateDimension * s_StateDimension;

    using ExpandedKey = std::vector<uint8_t>;
    using State = std::array<uint8_t, s_StateSize>;

public:
    CAESEncoder() = default;
    virtual ~CAESEncoder() override final = default;

    virtual bool SetKeyType(EKeyType keyType) noexcept override final;
    virtual EKeyType GetKeyType() const noexcept override final;

    virtual bool SetKey(const Key& key) noexcept override final;
    virtual const Key& GetKey() const noexcept override final;

    virtual EncoderData Encode(const EncoderData& data) override final;

private:
    bool Setup() noexcept;
    ExpandedKey ExpandKey() const noexcept;
    void AddRoundKey(uint8_t roundNumber, uint8_t* state, const uint8_t* expandedKey) const noexcept;
    void SubBytes(uint8_t* state) const noexcept;
    void ShiftRows(uint8_t* state) const noexcept;
    void MixColumns(uint8_t* state) const noexcept;
    uint8_t xtime(uint8_t x) const noexcept;
    void XorWithIv(uint8_t* buf, uint8_t* iv) const noexcept;
    uint8_t gMultiplication(uint8_t first, uint8_t second) const noexcept;
    void EncodeState(State& state, const ExpandedKey& expandedKey) const noexcept;

private:
    EKeyType m_KeyType = EKeyType::Bits128;
    Key m_Key = {};

    unsigned int m_KeyLenght = 32u;

    const unsigned int m_BlockLength = 16u;

    unsigned int m_ExpandKeyRoundsNumber = 8u;
    unsigned int m_RoundsNumber = 14u;

    static const uint8_t s_Sbox[256];
};

const uint8_t CAESEncoder::s_Sbox[256] = {
    0x63, 0x7c, 0x77, 0x7b, 0xf2, 0x6b, 0x6f, 0xc5, 0x30, 0x01, 0x67, 0x2b, 0xfe, 0xd7, 0xab, 0x76,
    0xca, 0x82, 0xc9, 0x7d, 0xfa, 0x59, 0x47, 0xf0, 0xad, 0xd4, 0xa2, 0xaf, 0x9c, 0xa4, 0x72, 0xc0,
    0xb7, 0xfd, 0x93, 0x26, 0x36, 0x3f, 0xf7, 0xcc, 0x34, 0xa5, 0xe5, 0xf1, 0x71, 0xd8, 0x31, 0x15,
    0x04, 0xc7, 0x23, 0xc3, 0x18, 0x96, 0x05, 0x9a, 0x07, 0x12, 0x80, 0xe2, 0xeb, 0x27, 0xb2, 0x75,
    0x09, 0x83, 0x2c, 0x1a, 0x1b, 0x6e, 0x5a, 0xa0, 0x52, 0x3b, 0xd6, 0xb3, 0x29, 0xe3, 0x2f, 0x84,
    0x53, 0xd1, 0x00, 0xed, 0x20, 0xfc, 0xb1, 0x5b, 0x6a, 0xcb, 0xbe, 0x39, 0x4a, 0x4c, 0x58, 0xcf,
    0xd0, 0xef, 0xaa, 0xfb, 0x43, 0x4d, 0x33, 0x85, 0x45, 0xf9, 0x02, 0x7f, 0x50, 0x3c, 0x9f, 0xa8,
    0x51, 0xa3, 0x40, 0x8f, 0x92, 0x9d, 0x38, 0xf5, 0xbc, 0xb6, 0xda, 0x21, 0x10, 0xff, 0xf3, 0xd2,
    0xcd, 0x0c, 0x13, 0xec, 0x5f, 0x97, 0x44, 0x17, 0xc4, 0xa7, 0x7e, 0x3d, 0x64, 0x5d, 0x19, 0x73,
    0x60, 0x81, 0x4f, 0xdc, 0x22, 0x2a, 0x90, 0x88, 0x46, 0xee, 0xb8, 0x14, 0xde, 0x5e, 0x0b, 0xdb,
    0xe0, 0x32, 0x3a, 0x0a, 0x49, 0x06, 0x24, 0x5c, 0xc2, 0xd3, 0xac, 0x62, 0x91, 0x95, 0xe4, 0x79,
    0xe7, 0xc8, 0x37, 0x6d, 0x8d, 0xd5, 0x4e, 0xa9, 0x6c, 0x56, 0xf4, 0xea, 0x65, 0x7a, 0xae, 0x08,
    0xba, 0x78, 0x25, 0x2e, 0x1c, 0xa6, 0xb4, 0xc6, 0xe8, 0xdd, 0x74, 0x1f, 0x4b, 0xbd, 0x8b, 0x8a,
    0x70, 0x3e, 0xb5, 0x66, 0x48, 0x03, 0xf6, 0x0e, 0x61, 0x35, 0x57, 0xb9, 0x86, 0xc1, 0x1d, 0x9e,
    0xe1, 0xf8, 0x98, 0x11, 0x69, 0xd9, 0x8e, 0x94, 0x9b, 0x1e, 0x87, 0xe9, 0xce, 0x55, 0x28, 0xdf,
    0x8c, 0xa1, 0x89, 0x0d, 0xbf, 0xe6, 0x42, 0x68, 0x41, 0x99, 0x2d, 0x0f, 0xb0, 0x54, 0xbb, 0x16 };

bool CAESEncoder::SetKeyType(EKeyType keyType) noexcept
{
    m_KeyType = keyType;
    return true;
}

EKeyType CAESEncoder::GetKeyType() const noexcept
{
    return m_KeyType;
}

bool CAESEncoder::SetKey(const Key& key) noexcept
{
    m_Key = key;
    return true;
}

const AES::Key& CAESEncoder::GetKey() const noexcept
{
    return m_Key;
}

EncoderData CAESEncoder::Encode(const EncoderData& data)
{
    EncoderData result;

    if (!Setup())
        return result;

    const auto paddingSize = s_StateSize - (data.size() % s_StateSize);
    result.resize(data.size() + paddingSize);

    const auto expandedKey = ExpandKey();

    for (auto offset = 0u; offset < data.size(); offset += s_StateSize)
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

        EncodeState(state, expandedKey);

        for (auto i = 0u; i < s_StateDimension; ++i)
        {
            for (auto j = 0u; j < s_StateDimension; ++j)
                result[offset + i + s_StateDimension * j] = state[s_StateDimension * i + j];
        }
    }

    return result;
}

bool CAESEncoder::Setup() noexcept
{
    switch (m_KeyType)
    {
    case EKeyType::Bits128:
        m_KeyLenght = 16u;
        m_ExpandKeyRoundsNumber = 4;
        m_RoundsNumber = 10;
        break;

    case EKeyType::Bits196:
        m_KeyLenght = 24u;
        m_ExpandKeyRoundsNumber = 6;
        m_RoundsNumber = 12;
        break;

    case EKeyType::Bits256:
        m_KeyLenght = 32u;
        m_ExpandKeyRoundsNumber = 8;
        m_RoundsNumber = 14;
        break;

    default:
        return false;
    }

    return true;
}

CAESEncoder::ExpandedKey CAESEncoder::ExpandKey() const noexcept
{
    static const auto rotateWord = [](auto& word)
    {
        const auto buffer = word[0];

        for (auto i = 0u; i < word.size() - 1; ++i)
            word[i] = word[i + 1];

        word[word.size() - 1] = buffer;
    };

    static const auto subWord = [](auto& word)
    {
        for (auto& byte : word)
            byte = s_Sbox[16 * ((byte & 0xf0) >> 4) + (byte & 0x0f)];
    };

    static const auto xor = [](auto& destination, const auto& first, const auto& second)
    {
        for (auto i = 0u; i < s_StateDimension; ++i)
            destination[i] = first[i] ^ second[i];
    };

    const auto rcon = [this](auto index)
    {
        std::array<uint8_t, 4> result{ 0x02, 0x00, 0x00, 0x00 };

        if (index == 1)
            result[0] = 0x01;
        else if (index > 1)
        {
            result[0] = 0x02;
            --index;

            for(;index - 1 > 0; --index)
                result[0] = gMultiplication(result[0], 0x02);
        }

        return result;
    };

    ExpandedKey result((m_RoundsNumber + 1) * 128u / 8u);
    std::array<uint8_t, 4> buffer;

    for (auto i = 0u; i < m_ExpandKeyRoundsNumber; ++i)
    {
        for (auto x = 0u; x < buffer.size(); ++x)
            result[4 * i + x] = m_Key[4 * i + x];
    }

    const auto length = s_StateDimension * (m_RoundsNumber + 1);
    for (auto i = m_ExpandKeyRoundsNumber; i < length; ++i)
    {
        for (auto x = 0u; x < buffer.size(); ++x)
            buffer[x] = result[4 * (i - 1) + x];

        if (i%m_ExpandKeyRoundsNumber == 0) {

            rotateWord(buffer);
            subWord(buffer);
            xor(buffer, rcon(i / m_ExpandKeyRoundsNumber), buffer);

        }
        else if (m_ExpandKeyRoundsNumber > 6 && i%m_ExpandKeyRoundsNumber == 4)
            subWord(buffer);

        for (auto x = 0u; x < buffer.size(); ++x)
            result[4 * i + x] = result[4 * (i - m_ExpandKeyRoundsNumber) + x] ^ buffer[x];
    }

    return result;
}

void CAESEncoder::AddRoundKey(uint8_t roundNumber, uint8_t* state, const uint8_t* expandedKey) const noexcept
{
    for (auto i = 0u; i < s_StateDimension; ++i)
    {
        for (auto x = 0u; x < s_StateDimension; ++x)
            state[s_StateDimension * x + i] = state[s_StateDimension * x + i] ^ expandedKey[4 * s_StateDimension * roundNumber + s_StateDimension * i + x];
    }
}

void CAESEncoder::SubBytes(uint8_t* state) const noexcept
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

void CAESEncoder::ShiftRows(uint8_t* state) const noexcept
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

void CAESEncoder::MixColumns(uint8_t* state) const noexcept
{
    const auto xorMultiplication = [this](auto& destination, const auto& first, const auto& second)
    {
        destination[0] = gMultiplication(first[0], second[0]) ^ gMultiplication(first[3], second[1]) ^ gMultiplication(first[2], second[2]) ^ gMultiplication(first[1], second[3]);
        destination[1] = gMultiplication(first[1], second[0]) ^ gMultiplication(first[0], second[1]) ^ gMultiplication(first[3], second[2]) ^ gMultiplication(first[2], second[3]);
        destination[2] = gMultiplication(first[2], second[0]) ^ gMultiplication(first[1], second[1]) ^ gMultiplication(first[0], second[2]) ^ gMultiplication(first[3], second[3]);
        destination[3] = gMultiplication(first[3], second[0]) ^ gMultiplication(first[2], second[1]) ^ gMultiplication(first[1], second[2]) ^ gMultiplication(first[0], second[3]);
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

uint8_t CAESEncoder::xtime(uint8_t x) const noexcept
{
    return ((x << 1) ^ (((x >> 7) & 1) * 0x1b));
}

void CAESEncoder::XorWithIv(uint8_t* buf, uint8_t* iv) const noexcept
{
    for (auto i = 0u; i < m_BlockLength; ++i)
        buf[i] ^= iv[i];
}

uint8_t CAESEncoder::gMultiplication(uint8_t first, uint8_t second) const noexcept
{
    uint8_t result = 0;

    for (auto i = 0u; i < 8u; ++i)
    {
        if (second & 1)
            result ^= first;

        const auto hbs = first & 0x80;

        first <<= 1;

        if (hbs)
            first ^= 0x1b;

        second >>= 1;
    }

    return result;
}

void CAESEncoder::EncodeState(State& state, const ExpandedKey& expandedKey) const noexcept
{
    AddRoundKey(0, state.data(), expandedKey.data());

    for (auto i = 1u; i < m_RoundsNumber; ++i)
    {
        SubBytes(state.data());
        ShiftRows(state.data());
        MixColumns(state.data());
        AddRoundKey(i, state.data(), expandedKey.data());
    }

    SubBytes(state.data());
    ShiftRows(state.data());
    AddRoundKey(m_RoundsNumber, state.data(), expandedKey.data());
}

IAESEncoderUniquePtr IAESEncoder::Create()
{
    return std::make_unique<CAESEncoder>();
}