#include "pch.h"
#include "AESEncoderController.h"
#include "AESEncoder.h"

using namespace AES;

class CAESEncoderController final : public IAESEncoderController
{
public:
    CAESEncoderController() = default;
    virtual ~CAESEncoderController() override final = default;

    virtual bool SetEncoder(const IEncoderSharedPtr& encoder) override final;

    virtual bool SetKeyType(EKeyType keyType) noexcept override final;
    virtual EKeyType GetKeyType() const noexcept override final;

    virtual bool SetKey(const Key& key) noexcept override final;
    virtual const Key& GetKey() const noexcept override final;

    virtual bool SetMaximumThreadsWorkers(size_t threadsNumber) noexcept override final;
    virtual size_t GetMaximumThreadsWorkers() const noexcept override final;

private:
    IAESEncoderSharedPtr m_Encoder;
};

bool CAESEncoderController::SetEncoder(const IEncoderSharedPtr& encoder)
{
    if (!encoder)
    {
        m_Encoder.reset();
        return true;
    }

    const auto aesEncoder = std::dynamic_pointer_cast<IAESEncoder>(encoder);
    if (!aesEncoder)
        return false;

    m_Encoder = aesEncoder;
    return true;
}

bool CAESEncoderController::SetKeyType(EKeyType keyType) noexcept
{
    EDITOR_ASSERT(m_Encoder);
    if (!m_Encoder)
        return false;

    m_Encoder->SetKeyType(keyType);
    return true;
}

EKeyType CAESEncoderController::GetKeyType() const noexcept
{
    EDITOR_ASSERT(m_Encoder);
    return m_Encoder ? m_Encoder->GetKeyType() : EKeyType::Bits128;
}

bool CAESEncoderController::SetKey(const Key& key) noexcept
{
    EDITOR_ASSERT(m_Encoder);
    if (!m_Encoder)
        return false;

    m_Encoder->SetKey(key);
    return true;
}

const Key& CAESEncoderController::GetKey() const noexcept
{
    static const auto emptyKey = Key{};

    EDITOR_ASSERT(m_Encoder);
    return m_Encoder ? m_Encoder->GetKey() : emptyKey;
}

bool CAESEncoderController::SetMaximumThreadsWorkers(size_t threadsNumber) noexcept
{
    EDITOR_ASSERT(m_Encoder);
    if (!m_Encoder)
        return false;

    return m_Encoder->SetMaximumThreadsWorkers(threadsNumber);
}

size_t CAESEncoderController::GetMaximumThreadsWorkers() const noexcept
{
    EDITOR_ASSERT(m_Encoder);
    return m_Encoder ? m_Encoder->GetMaximumThreadsWorkers() : 1u;
}

IAESEncoderControllerUniquePtr IAESEncoderController::Create()
{
    return std::make_unique<CAESEncoderController>();
}