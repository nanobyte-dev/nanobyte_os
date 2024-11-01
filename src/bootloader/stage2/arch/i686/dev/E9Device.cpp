#include "E9Device.hpp"
#include <arch/i686/IO.hpp>

namespace arch::i686 {

size_t E9Device::Read(uint8_t* data, size_t size, ErrorChain& err)
{
    if (err.Failed())
        return 0;
    
    err.Fail(ResultCode::NotSupported, "Not supported");
    return 0;
}

size_t E9Device::Write(const uint8_t* data, size_t size, ErrorChain& err)
{
    if (err.Failed())
        return 0;

    for (size_t i = 0; i < size; i++)
        Out(0xE9, data[i]);

    return size;
}

}