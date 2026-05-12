#ifndef UTILS_H
#define UTILS_H

#include <stddef.h>
#include <stdio.h>

// Nhóm hàm xử lý input dùng chung cho toàn bộ chương trình.
void removeTrailingNewline(char value[]);
void trimWhitespace(char value[]);
int readLine(char value[], size_t size);
int readInt(int *value);

// Nhóm hàm hỗ trợ giao diện console và căn bảng UTF-8.
void printBoxTitle(const char title[], int width);
void printSectionTitle(const char title[]);
void printPaddedText(const char value[], int width, int alignRight);
void fprintPaddedText(FILE *stream, const char value[], int width, int alignRight);
void printMenuOption(int number, const char label[]);
void printMenuBack(void);

// Nhóm hàm in thông báo có màu ANSI.
void printSuccess(const char message[]);
void printError(const char message[]);
void printInfo(const char message[]);

#endif
