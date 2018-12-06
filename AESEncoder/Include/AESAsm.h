#if !defined(__AES_ASM_H__)
#define __AES_ASM_H__
#pragma once

#if defined(AESASM_EXPORTS)
#define DLLFUNCTION __declspec(dllexport)
#else
#define DLLFUNCTION __declspec(dllimport)
#endif

namespace AESAsm
{
    using byte = unsigned char;

    void DLLFUNCTION Aes128bEncryption(byte* state, const byte* expandedKey);
    void DLLFUNCTION Aes192bEncryption(byte* state, const byte* expandedKey);
    void DLLFUNCTION Aes256bEncryption(byte* state, const byte* expandedKey);
}

#endif //__AES_ASM_H__