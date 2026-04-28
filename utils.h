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
