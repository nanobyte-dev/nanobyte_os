#pragma once
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

#include "string/memcpy.h"
#include "string/memset.h"
#include "string/memcmp.h"
#include "string/memchr.h"
#include "string/memmove.h"

const char* strchr(const char* str, char chr);
char* strcpy(char* dst, const char* src);
unsigned strlen(const char* str);
int strcmp(const char* a, const char* b);

// todo: move
wchar_t* utf16_to_codepoint(wchar_t* string, int* codepoint);
char* codepoint_to_utf8(int codepoint, char* stringOutput);

#ifdef __cplusplus
}
#endif
