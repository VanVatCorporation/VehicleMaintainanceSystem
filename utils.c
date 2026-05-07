#include <ctype.h>
#include <errno.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "utils.h"  

void printColor(const char* color, const char* text) {
    printf("%s%s%s", color, text, ANSI_COLOR_RESET);
}

void printInfo(const char* message) {
    printf(ANSI_COLOR_CYAN "[INFO] %s\n" ANSI_COLOR_RESET, message);
}

void printSuccess(const char* message) {
    printf(ANSI_COLOR_GREEN "[SUCCESS] %s\n" ANSI_COLOR_RESET, message);
}

void printError(const char* message) {
    printf(ANSI_COLOR_RED "[ERROR] %s\n" ANSI_COLOR_RESET, message);
}

void printSectionTitle(const char* title) {
    printf("\n");
    printf(ANSI_COLOR_CYAN "=== %s ===\n" ANSI_COLOR_RESET, title);
}

void printBoxTitle(const char* title, int width) {
    int titleLength = (int)strlen(title);
    int innerWidth = width - 2;
    int leftPadding;
    int rightPadding;
    int i;

    if (width < titleLength + 4) {
        width = titleLength + 4;
        innerWidth = width - 2;
    }

    leftPadding = (innerWidth - titleLength) / 2;
    rightPadding = innerWidth - titleLength - leftPadding;

    printf("\n");
    for (i = 0; i < width; i++) {
        printf("=");
    }

    printf("\n");
    printf("%*s%s%*s\n", leftPadding, "", title, rightPadding, "");

    for (i = 0; i < width; i++) {
        printf("=");
    }

    printf("\n");
}

void printMenuOption(int option, const char* label) {
    printf("  %d. %s\n", option, label);
}

void printMenuBack(void) {
    printf("  0. Back\n");
}

int readLine(char buffer[], int size) {
    int length;
    int ch;

    if (buffer == NULL || size <= 0) {
        return 0;
    }

    if (fgets(buffer, size, stdin) == NULL) {
        return 0;
    }

    length = (int)strlen(buffer);
    if (length > 0 && buffer[length - 1] == '\n') {
        buffer[length - 1] = '\0';
    } else {
        while ((ch = getchar()) != '\n' && ch != EOF) {
        }
    }

    return 1;
}

int readInt(int* value) {
    char input[100];
    char* end;
    char* start;
    long parsed;

    if (value == NULL || !readLine(input, sizeof(input))) {
        return 0;
    }

    start = input;
    while (isspace((unsigned char)*start)) {
        start++;
    }

    if (*start == '\0') {
        return 0;
    }

    errno = 0;
    parsed = strtol(start, &end, 10);

    if (start == end || errno == ERANGE || parsed < INT_MIN || parsed > INT_MAX) {
        return 0;
    }

    while (*end != '\0') {
        if (!isspace((unsigned char)*end)) {
            return 0;
        }

        end++;
    }

    *value = (int)parsed;
    return 1;
}
