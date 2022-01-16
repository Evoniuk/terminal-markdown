#ifndef tmd_h
#define tmd_h

#include <stdbool.h>

char* substitute_escapes(char* text, bool format);
void  format_and_print(char* text, bool format);

#endif
