#include "pch.h"
#include "AESEncoder.h"

using namespace AES;

class CAESEncoder final : public IAESEncoder
{
public:
    CAESEncoder() = default;
    virtual ~CAESEncoder() override final = default;

    virtual bool SetKeyType(EKeyType keyType) noexcept override final;
    virtual EKeyType GetKeyType() const noexcept override final;

    virtual bool SetKey(const Key& key) noexcept override final;
    virtual const Key& GetKey() const noexcept override final;

    virtual EncoderData Encode(const EncoderData& data) override final;

private:
    EKeyType m_KeyType = EKeyType::Bits128;
    Key m_Key = {};
};

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
    return data;
}

IAESEncoderUniquePtr IAESEncoder::Create()
{
    return std::make_unique<CAESEncoder>();
}