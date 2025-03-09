#pragma once

#include <stdbool.h>
#include <stdint.h>

typedef struct {
    uint8_t scancode;
    bool pressed;
    char ascii;
} Keyboard_key;

extern bool Keyboard_keyStates[128];

void Keyboard_initialize();
void Keyboard_setLEDs(bool scrollLock, bool numLock, bool capsLock);
void Keyboard_setTypematicRateAndDelay(uint8_t repeatRate, uint8_t repeatDelay);
int Keyboard_getScancodeSet();
void Keyboard_setScancodeSet(uint8_t setNumber);
bool Keyboard_isKeyPressed(uint8_t scancode);
bool Keyboard_isShiftPressed();
Keyboard_key Keyboard_processKey();
bool Keyboard_hasKey();
Keyboard_key* Keyboard_popKey();
void Keyboard_clearBuffer();
