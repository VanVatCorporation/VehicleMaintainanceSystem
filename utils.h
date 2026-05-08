#ifndef UTILS_H
#define UTILS_H

#include <stddef.h>

// Common input helper functions
void removeTrailingNewline(char value[]);
void trimWhitespace(char value[]);
int readLine(char value[], size_t size);
int readInt(int *value);

// Console UI helper functions
void printBoxTitle(const char title[], int width);
void printSectionTitle(const char title[]);
void printMenuOption(int number, const char label[]);
void printMenuBack(void);
void printSuccess(const char message[]);
void printError(const char message[]);
void printInfo(const char message[]);

#endif

#define ANSI_COLOR_RESET   "\x1b[0m"
#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_YELLOW  "\x1b[33m"
#define ANSI_COLOR_CYAN    "\x1b[36m"

// Khai báo prototype hàm (để file khác có thể gọi được)
void printColor(const char* color, const char* text);

#endif
