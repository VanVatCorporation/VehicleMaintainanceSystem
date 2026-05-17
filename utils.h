#ifndef UTILS_H
#define UTILS_H

#include <stddef.h>
#include <stdio.h>
#include <time.h>

// Common input helper functions
void removeTrailingNewline(char value[]);
void trimWhitespace(char value[]);
int readLine(char value[], size_t size);
int readInt(int *value);

// Prompt user for a date in dd/mm/yyyy and convert to time_t. Returns 1 on success.
// Prompt user for a date in dd/mm/yyyy and convert to time_t.
// Repeats until valid date entered or user cancels by entering `0`.
// Returns: 1 = success (sets *out), 0 = cancelled
int promptDateToTime(time_t *out);

// Console UI helper functions
void printBoxTitle(const char title[], int width);
void printSectionTitle(const char title[]);
void printPaddedText(const char value[], int width, int alignRight);
void fprintPaddedText(FILE *stream, const char value[], int width, int alignRight);
void printMenuOption(int number, const char label[]);
void printMenuBack(void);
void printSuccess(const char message[]);
void printError(const char message[]);
void printInfo(const char message[]);

#endif
