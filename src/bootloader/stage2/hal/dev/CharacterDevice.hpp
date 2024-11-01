#pragma once
#include <stdint.h>
#include <stddef.h>
#include <etl/expected.h>
#include <util/ErrorChain.hpp>

namespace hal::dev
{

class CharacterDevice
{
public:
    virtual size_t Read(uint8_t* data, size_t size, ErrorChain& err) = 0;
    virtual size_t Write(const uint8_t* data, size_t size, ErrorChain& err) = 0;
};

}