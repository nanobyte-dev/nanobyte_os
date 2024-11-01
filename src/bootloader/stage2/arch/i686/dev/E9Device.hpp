#include <hal/dev/CharacterDevice.hpp>

namespace arch::i686
{
    
class E9Device : public hal::dev::CharacterDevice
{
public:
    virtual size_t Read(uint8_t* data, size_t size, ErrorChain& err) override;
    virtual size_t Write(const uint8_t* data, size_t size, ErrorChain& err) override;
};
    
}

