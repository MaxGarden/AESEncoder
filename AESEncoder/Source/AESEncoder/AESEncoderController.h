#if !defined(__AES_ENCODER_CONTROLLER_H__)
#define __AES_ENCODER_CONTROLLER_H__
#pragma once

#include "AESEncoder.h"
#include "Encoder/EncoderController.h"

namespace AES
{
    class IAESEncoderController : public IEncoderController
    {
    public:
        virtual ~IAESEncoderController() = default;

        virtual bool SetKeyType(EKeyType keyType) noexcept = 0;
        virtual EKeyType GetKeyType() const noexcept = 0;

        virtual bool SetKey(const Key& key) noexcept = 0;
        virtual const Key& GetKey() const noexcept = 0;

        static IAESEncoderControllerUniquePtr Create();
    };
}

#endif //__AES_ENCODER_CONTROLLER_H__