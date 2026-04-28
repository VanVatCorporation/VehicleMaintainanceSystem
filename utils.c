#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "utils.h"

#define INPUT_LENGTH 100
#define DEFAULT_UI_WIDTH 64

static void printRepeated(char value, int count)
{
    int i;

    for (i = 0; i < count; i++)
    {
        putchar(value);
    }
}

static int normalizeWidth(int width)
{
    if (width < 24)
    {
        return 24;
    }

    if (width > 100)
    {
        return 100;
    }

    return width;
}

void printBoxTitle(const char title[], int width)
{
    int normalizedWidth = normalizeWidth(width);
    int innerWidth = normalizedWidth - 2;
    int titleLength = (int)strlen(title);
    int visibleLength;
    int padding;
    int leftPadding;
    int rightPadding;

    if (titleLength > innerWidth - 2)
    {
        visibleLength = innerWidth - 2;
    }
    else
    {
        visibleLength = titleLength;
    }

    padding = innerWidth - visibleLength;
    leftPadding = padding / 2;
    rightPadding = padding - leftPadding;

    printf("\n+");
    printRepeated('-', innerWidth);
    printf("+\n|");
    printRepeated(' ', leftPadding);
    printf("%.*s", visibleLength, title);
    printRepeated(' ', rightPadding);
    printf("|\n+");
    printRepeated('-', innerWidth);
    printf("+\n");
}

void printSectionTitle(const char title[])
{
    printBoxTitle(title, DEFAULT_UI_WIDTH);
}

void printMenuOption(int number, const char label[])
{
    printf("  [%d] %s\n", number, label);
}

void printMenuBack(void)
{
    printf("  [0] Back\n");
}

void printSuccess(const char message[])
{
    printf("[OK] %s\n", message);
}

void printError(const char message[])
{
    printf("[ERROR] %s\n", message);
}

void printInfo(const char message[])
{
    printf("[INFO] %s\n", message);
}

void removeTrailingNewline(char value[])
{
    value[strcspn(value, "\n")] = '\0';
}

// Trim leading and trailing spaces but keep spaces inside the text.
void trimWhitespace(char value[])
{
    char *start = value;
    char *end;

    while (isspace((unsigned char)*start))
    {
        start++;
    }

    if (start != value)
    {
        memmove(value, start, strlen(start) + 1);
    }

    end = value + strlen(value);
    while (end > value && isspace((unsigned char)*(end - 1)))
    {
        end--;
    }

    *end = '\0';
}

// Read one full line to avoid scanf cutting long input.
int readLine(char value[], size_t size)
{
    int ch;

    if (fgets(value, size, stdin) == NULL)
    {
        return 0;
    }

    if (strchr(value, '\n') == NULL)
    {
        while ((ch = getchar()) != '\n' && ch != EOF)
            ;
    }

    removeTrailingNewline(value);
    trimWhitespace(value);
    return 1;
}

// Read a menu choice and reject input like "1abc".
int readInt(int *value)
{
    char input[INPUT_LENGTH];
    char extra;

    if (!readLine(input, sizeof(input)))
    {
        return 0;
    }

    return sscanf(input, "%d %c", value, &extra) == 1;
}
