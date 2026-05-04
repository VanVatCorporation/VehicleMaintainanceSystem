#include <stdio.h>
#include "utils.h"

void printColor(const char* color, const char* text) {
    printf("%s%s%s", color, text, ANSI_COLOR_RESET);
}