#include "Assert.hpp"
#include <arch/i686/IO.hpp>
#include <Debug.hpp>

bool Assert_(const char* condition, const char* filename, int line, const char* function)
{
    Debug::Critical("Assert", "Assertion failed: %s", condition);
    Debug::Critical("Assert", "In file %s line %d function %s", filename, line, function);
    arch::i686::Panic();
    return false;
}
