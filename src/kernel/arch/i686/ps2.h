#pragma once

#include <stdint.h>
#include <stdbool.h>

typedef struct {
    const char* Name;
    int KeyboardPortNumber;
    int MousePortNumber;
} PS2Driver;
