#include "pch.h"
#include "AESEncoderBase.h"
#include <future>

using namespace AES;

const std::array<uint8_t, 256> CAESEncoderBase::s_Sbox = {
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

EncoderData CAESEncoderBase::Encode(const EncoderData& data)
{
    EncoderData result;

    if (!Setup())
        return result;

    const auto paddingSize = s_StateSize - (data.size() % s_StateSize);
    result = data;
    result.resize(result.size() + paddingSize, 0);

    const auto expandedKey = ExpandKey();

    const auto possibleThreadsWorkersCount = result.size() / s_StateSize;
    const auto threadsWorkersCount = std::max(1u, std::min(possibleThreadsWorkersCount, GetMaximumThreadsWorkers()));

    const auto launchWorker = [this, &result, &expandedKey](size_t chunkOffset, size_t chunkSize)
    {
        return std::async(std::launch::async, [this, &result, &expandedKey, chunkOffset, chunkSize]()
        {
            return EncodeDataChunk(&result[chunkOffset], chunkSize, expandedKey);
        });
    };

    std::vector<std::future<bool>> asyncWorkers;
    asyncWorkers.reserve(threadsWorkersCount);

    auto dataOffset = 0u;
    const auto dataSize = result.size();
    const auto dataChunkSize = (dataSize / s_StateSize) / threadsWorkersCount * s_StateSize;

    for (auto i = 1u; i < threadsWorkersCount; ++i)
    {
        asyncWorkers.emplace_back(launchWorker(dataOffset, dataChunkSize));
        dataOffset += dataChunkSize;
    }

    asyncWorkers.emplace_back(launchWorker(dataOffset, dataSize - dataOffset));

    auto workersResult = true;
    for (auto& worker : asyncWorkers)
        workersResult &= worker.get();

    if (!workersResult)
        result.clear();

    return result;
}

void CAESEncoderBase::SetKeyType(EKeyType keyType) noexcept
{
    m_KeyType = keyType;
}

EKeyType CAESEncoderBase::GetKeyType() const noexcept
{
    return m_KeyType;
}

void CAESEncoderBase::SetKey(const Key& key) noexcept
{
    m_Key = key;
}

const AES::Key& CAESEncoderBase::GetKey() const noexcept
{
    return m_Key;
}

bool CAESEncoderBase::SetMaximumThreadsWorkers(size_t threadsNumber) noexcept
{
    if (threadsNumber == 0)
        return false;

    m_MaximumThreadsWorkers = threadsNumber;
    return true;
}

size_t CAESEncoderBase::GetMaximumThreadsWorkers() const noexcept
{
    return m_MaximumThreadsWorkers;
}

bool CAESEncoderBase::Setup() noexcept
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

CAESEncoderBase::ExpandedKey CAESEncoderBase::ExpandKey() const noexcept
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

            for (; index - 1 > 0; --index)
                result[0] = GMultiplication(result[0], 0x02);
        }

        return result;
    };

    ExpandedKey result((m_RoundsNumber + 1) * 128u / 8u);
    std::array<uint8_t, 4> buffer;

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

        if (i % m_ExpandKeyRoundsNumber == 0) {

            rotateWord(buffer);
            subWord(buffer);
            xor (buffer, rcon(i / m_ExpandKeyRoundsNumber), buffer);

        }
        else if (m_ExpandKeyRoundsNumber > 6 && i%m_ExpandKeyRoundsNumber == 4)
            subWord(buffer);

        for (auto x = 0u; x < buffer.size(); ++x)
            result[4 * i + x] = result[4 * (i - m_ExpandKeyRoundsNumber) + x] ^ buffer[x];
    }

    return result;
}

uint8_t CAESEncoderBase::GMultiplication(uint8_t first, uint8_t second) const noexcept
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

unsigned int AES::CAESEncoderBase::GetRoundsNumber() const noexcept
{
    return m_RoundsNumber;
}