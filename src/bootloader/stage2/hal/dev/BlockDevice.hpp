#pragma once
#include <stdint.h>
#include <stddef.h>
#include <etl/expected.h>
#include <util/ErrorChain.hpp>

namespace hal::dev
{

class BlockDevice
{
public:
    virtual size_t Read(size_t position, uint8_t* data, size_t size, ErrorChain& err) = 0;
    virtual size_t Write(size_t position, const uint8_t* data, size_t size, ErrorChain& err) = 0;
    virtual size_t Size() = 0;
};

}