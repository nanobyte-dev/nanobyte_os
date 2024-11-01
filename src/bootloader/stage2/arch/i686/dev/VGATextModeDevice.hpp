#include <hal/dev/CharacterDevice.hpp>

namespace arch::i686 {

class VGATextModeDevice : public hal::dev::CharacterDevice
{
public:
    VGATextModeDevice();
    size_t Read(uint8_t* data, size_t size, ErrorChain& err) override;
    size_t Write(const uint8_t* data, size_t size, ErrorChain& err) override;

    void Clear();

private:
    void PutChar(int x, int y, char c);
    void PutColor(int x, int y, uint8_t color);
    char GetChar(int x, int y);
    uint8_t GetColor(int x, int y);

    void SetCursor(int x, int y);
    void Scrollback(int lines);

    void PutChar(char c);

    int m_ScreenX, m_ScreenY;
    static uint8_t* g_ScreenBuffer;
};
    
}
