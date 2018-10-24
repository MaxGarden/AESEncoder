#if !defined(__AES_ENCODER_VIEW_H__)
#define __AES_ENCODER_VIEW_H__
#pragma once

#include "Encoder/EncoderView.h"

namespace AES
{
    class IAESEncoderView : public IEncoderView
    {
    public:
        virtual ~IAESEncoderView() = default;

        static IAESEncoderViewUniquePtr Create();
    };
}

#endif //__AES_ENCODER_VIEW_H__