#include "pch.h"
#include "AESAsmEncoder.h"
#include "AESAsm.h"

using namespace AES;

EncoderData CAESAsmEncoder::Encode(const EncoderData& data)
{
    EncoderData result;

    if (!Setup())
        return result;

    const auto paddingSize = s_StateSize - (data.size() % s_StateSize);
    result = data;
    result.resize(result.size() + paddingSize, 0);

    const auto expandedKey = ExpandKey();

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


    const auto size = result.size();
    for (auto offset = 0u; offset < size; offset += s_StateSize)
        (*encodeFunction)(&result[offset], &result[offset], expandedKey.data());

    return result;
}