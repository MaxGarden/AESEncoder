#include "pch.h"
#include "AESAsmEncoder.h"
#include "AESAsm.h"

using namespace AES;

bool AES::CAESAsmEncoder::EncodeDataChunk(uint8_t* dataChunk, size_t dataChunkSize, const ExpandedKey& expandedKey) const noexcept
{
    const auto encodeFunction = [keyType = GetKeyType()]()
    {
        switch (keyType)
        {
        case AES::EKeyType::Bits128:
            return &AESAsm::Aes128bEncryption;
        case AES::EKeyType::Bits192:
            return &AESAsm::Aes192bEncryption;
        default:
        case AES::EKeyType::Bits256:
            return &AESAsm::Aes256bEncryption;
        }
    }();

    const auto expandedKeyData = expandedKey.data();
    for (auto offset = 0u; offset < dataChunkSize; offset += s_StateSize)
        (*encodeFunction)(&dataChunk[offset], expandedKeyData);

    return true;
}