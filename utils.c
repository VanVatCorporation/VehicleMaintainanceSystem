#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "utils.h"
#include <time.h>

#define INPUT_LENGTH 100
#define DEFAULT_UI_WIDTH 64
#define UI_COLOR_RESET "\033[0m"
#define UI_COLOR_BLUE "\033[34m"
#define UI_COLOR_GREEN "\033[32m"
#define UI_COLOR_RED "\033[31m"
#define UI_COLOR_CYAN "\033[36m"

static void fprintRepeated(FILE *stream, char value, int count)
{
    int i;

    for (i = 0; i < count; i++)
    {
        fputc(value, stream);
    }
}

static void printRepeated(char value, int count)
{
    fprintRepeated(stdout, value, count);
}

static int isUtf8Continuation(unsigned char value)
{
    return (value & 0xC0) == 0x80;
}

static int getUtf8CharSize(const char value[], size_t remaining)
{
    unsigned char first = (unsigned char)value[0];

    if (remaining == 0 || first == '\0')
    {
        return 0;
    }

    if (first < 0x80)
    {
        return 1;
    }

    if (remaining >= 2 &&
        (first & 0xE0) == 0xC0 &&
        isUtf8Continuation((unsigned char)value[1]))
    {
        return 2;
    }

    if (remaining >= 3 &&
        (first & 0xF0) == 0xE0 &&
        isUtf8Continuation((unsigned char)value[1]) &&
        isUtf8Continuation((unsigned char)value[2]))
    {
        return 3;
    }

    if (remaining >= 4 &&
        (first & 0xF8) == 0xF0 &&
        isUtf8Continuation((unsigned char)value[1]) &&
        isUtf8Continuation((unsigned char)value[2]) &&
        isUtf8Continuation((unsigned char)value[3]))
    {
        return 4;
    }

    return 1;
}

static int getUtf8Slice(const char value[], int maxWidth, int *visibleWidth)
{
    int bytes = 0;
    int totalBytes;
    int width = 0;
    int charSize;

    if (value == NULL || maxWidth <= 0)
    {
        *visibleWidth = 0;
        return 0;
    }

    totalBytes = (int)strlen(value);

    while (bytes < totalBytes && width < maxWidth)
    {
        charSize = getUtf8CharSize(value + bytes, (size_t)(totalBytes - bytes));

        if (charSize <= 0)
        {
            break;
        }

        bytes += charSize;
        width++;
    }

    *visibleWidth = width;
    return bytes;
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
    int visibleLength;
    int titleBytes;
    int padding;
    int leftPadding;
    int rightPadding;

    if (title == NULL)
    {
        title = "";
    }

    titleBytes = getUtf8Slice(title, innerWidth - 2, &visibleLength);

    padding = innerWidth - visibleLength;
    leftPadding = padding / 2;
    rightPadding = padding - leftPadding;

    printf("\n%s+", UI_COLOR_BLUE);
    printRepeated('-', innerWidth);
    printf("+\n|");
    printRepeated(' ', leftPadding);
    printf("%.*s", titleBytes, title);
    printRepeated(' ', rightPadding);
    printf("|\n+");
    printRepeated('-', innerWidth);
    printf("+%s\n", UI_COLOR_RESET);
}

void printSectionTitle(const char title[])
{
    printBoxTitle(title, DEFAULT_UI_WIDTH);
}

void fprintPaddedText(FILE *stream, const char value[], int width, int alignRight)
{
    int visibleWidth;
    int bytes;
    int padding;

    if (width < 0)
    {
        width = 0;
    }

    bytes = getUtf8Slice(value, width, &visibleWidth);
    padding = width - visibleWidth;

    if (alignRight)
    {
        fprintRepeated(stream, ' ', padding);
    }

    if (value != NULL)
    {
        fprintf(stream, "%.*s", bytes, value);
    }

    if (!alignRight)
    {
        fprintRepeated(stream, ' ', padding);
    }
}

void printPaddedText(const char value[], int width, int alignRight)
{
    fprintPaddedText(stdout, value, width, alignRight);
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
    printf("%s[OK] %s%s\n", UI_COLOR_GREEN, message, UI_COLOR_RESET);
}

void printError(const char message[])
{
    printf("%s[ERROR] %s%s\n", UI_COLOR_RED, message, UI_COLOR_RESET);
}

void printInfo(const char message[])
{
    printf("%s[INFO] %s%s\n", UI_COLOR_CYAN, message, UI_COLOR_RESET);
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

int promptDateToTime(time_t *out)
{
    char dateInput[32];

    while (1) {
        printf("Enter date (dd/mm/yyyy) or 0 to cancel: ");
        if (!readLine(dateInput, sizeof(dateInput))) {
            return 0; /* treat EOF as cancel */
        }

        if (strcmp(dateInput, "0") == 0 || dateInput[0] == '\0') {
            return 0; /* cancelled */
        }

        int d, m, y;
        if (sscanf(dateInput, "%d/%d/%d", &d, &m, &y) != 3) {
            printError("Invalid date format. Use dd/mm/yyyy or 0 to cancel.");
            continue;
        }

        struct tm tmDate = {0};
        tmDate.tm_mday = d;
        tmDate.tm_mon = m - 1;
        tmDate.tm_year = y - 1900;

        time_t t = mktime(&tmDate);
        if (t == (time_t)-1) {
            printError("Invalid date. Try again or enter 0 to cancel.");
            continue;
        }

        *out = t;
        return 1;
    }
}
