#pragma once
#include <stddef.h>
#include <stdint.h>

class CharacterDevice 
{
public:
    virtual ~CharacterDevice() { }

    virtual size_t Read(uint8_t* data, size_t size) = 0;
    virtual size_t Write(const uint8_t* data, size_t size) = 0;
};
