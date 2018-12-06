#if !defined(__ENCODER_H__)
#define __ENCODER_H__
#pragma once

using EncoderData = std::vector<byte>;

class IEncoder
{
public:
    virtual ~IEncoder() = default;

    virtual EncoderData Encode(const EncoderData& data) = 0;

    virtual bool SetMaximumThreadsWorkers(size_t threadsNumber) noexcept = 0;
    virtual size_t GetMaximumThreadsWorkers() const noexcept = 0;
};

#endif //__ENCODER_H__