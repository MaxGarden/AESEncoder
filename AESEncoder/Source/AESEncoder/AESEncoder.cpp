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
    CAESEncoder(IAESBasicOperationsUniquePtr basicOperations);
    virtual ~CAESEncoder() override final = default;

    virtual bool SetKeyType(EKeyType keyType) noexcept override final;
    virtual EKeyType GetKeyType() const noexcept override final;

    virtual bool SetKey(const Key& key) noexcept override final;
    virtual const Key& GetKey() const noexcept override final;

    virtual EncoderData Encode(const EncoderData& data) override final;

private:
    bool Setup() noexcept;
    ExpandedKey ExpandKey() const noexcept;
    uint8_t xtime(uint8_t x) const noexcept;
    void XorWithIv(uint8_t* buf, uint8_t* iv) const noexcept;
    void EncodeState(State& state, const ExpandedKey& expandedKey) const noexcept;

private:
    IAESBasicOperationsUniquePtr m_BasicOperations;

    EKeyType m_KeyType = EKeyType::Bits128;
    Key m_Key = {};

    unsigned int m_KeyLenght = 32u;

    const unsigned int m_BlockLength = 16u;

    unsigned int m_ExpandKeyRoundsNumber = 8u;
    unsigned int m_RoundsNumber = 14u;
};

CAESEncoder::CAESEncoder(IAESBasicOperationsUniquePtr basicOperations) :
    m_BasicOperations(std::move(basicOperations))
{
    EDITOR_ASSERT(m_BasicOperations);
}

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

    EDITOR_ASSERT(m_BasicOperations);
    if (!m_BasicOperations)
        return result;

    if (!Setup())
        return result;

    const auto paddingSize = s_StateSize - (data.size() % s_StateSize);
    result.resize(data.size() + paddingSize);

    const auto expandedKey = ExpandKey();

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

    case EKeyType::Bits192:
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

    static const auto subWord = [this](auto& word)
    {
        for (auto& byte : word)
            byte = m_BasicOperations->GetSBoxValue(16 * ((byte & 0xf0) >> 4) + (byte & 0x0f));
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
                result[0] = m_BasicOperations->GMultiplication(result[0], 0x02);
        }

        return result;
    };

    ExpandedKey result((m_RoundsNumber + 1) * 128u / 8u);
    std::array<uint8_t, 4> buffer;

    EDITOR_ASSERT(m_BasicOperations);
    if (!m_BasicOperations)
        return result;

    for (auto i = 0u; i < m_ExpandKeyRoundsNumber; ++i)
    {
        for (auto x = 0u; x < buffer.size(); ++x)
        {
            const auto index = 4 * i + x;
            result[index] = index < m_Key.size() ? m_Key[index] : 0;
        }
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

uint8_t CAESEncoder::xtime(uint8_t x) const noexcept
{
    return ((x << 1) ^ (((x >> 7) & 1) * 0x1b));
}

void CAESEncoder::XorWithIv(uint8_t* buf, uint8_t* iv) const noexcept
{
    for (auto i = 0u; i < m_BlockLength; ++i)
        buf[i] ^= iv[i];
}

void CAESEncoder::EncodeState(State& state, const ExpandedKey& expandedKey) const noexcept
{
    EDITOR_ASSERT(m_BasicOperations);
    if (!m_BasicOperations)
        return;

    m_BasicOperations->AddRoundKey(state.data(), expandedKey.data());

    for (auto i = 1u; i < m_RoundsNumber; ++i)
    {
        m_BasicOperations->SubBytes(state.data());
        m_BasicOperations->ShiftRows(state.data());
        m_BasicOperations->MixColumns(state.data());
        m_BasicOperations->AddRoundKey(state.data(), expandedKey.data() + s_StateSize * i);
    }

    m_BasicOperations->SubBytes(state.data());
    m_BasicOperations->ShiftRows(state.data());
    m_BasicOperations->AddRoundKey(state.data(), expandedKey.data() + s_StateSize * m_RoundsNumber);
}

IAESEncoderUniquePtr IAESEncoder::Create(IAESBasicOperationsUniquePtr&& basicOperations)
{
    EDITOR_ASSERT(basicOperations);
    if (!basicOperations)
        return nullptr;

    return std::make_unique<CAESEncoder>(std::move(basicOperations));
}