#include "ctype.h"

bool islower(char ch) {
    return ((ch >= 'a') && (ch <= 'z'));
}

bool isupper(char ch) {
    return ((ch >= 'A') && (ch <= 'Z'));
}

bool isalpha(char ch) {
    return isupper(ch) || islower(ch);
}

char tolower(char ch) {
    return isupper(ch) ? (ch - 'A' + 'a') : ch;
}

char toupper(char ch) {
    return islower(ch) ? (ch - 'a' + 'A') : ch;
}
