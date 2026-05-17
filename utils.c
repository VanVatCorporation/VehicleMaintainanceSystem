#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "utils.h"
#include <time.h>
#include <locale.h>

#ifdef _WIN32
#include <windows.h>
#endif

#define INPUT_LENGTH 100
#define DEFAULT_UI_WIDTH 64
#define UI_COLOR_RESET "\033[0m"
#define UI_COLOR_BLUE "\033[34m"
#define UI_COLOR_GREEN "\033[32m"
#define UI_COLOR_RED "\033[31m"
#define UI_COLOR_CYAN "\033[36m"

void configureConsoleEncoding(void)
{
    setlocale(LC_ALL, "");

#ifdef _WIN32
    SetConsoleCP(CP_UTF8);
    SetConsoleOutputCP(CP_UTF8);
#endif
}

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

static void fprintUtf8Bytes(FILE *stream, const char value[], int bytes)
{
#ifdef _WIN32
    HANDLE outputHandle = NULL;
    DWORD mode;
    wchar_t wideBuffer[4096];
    int wideLength;
    DWORD written;

    if (stream == stdout)
    {
        outputHandle = GetStdHandle(STD_OUTPUT_HANDLE);
    }
    else if (stream == stderr)
    {
        outputHandle = GetStdHandle(STD_ERROR_HANDLE);
    }

    if (outputHandle != NULL &&
        outputHandle != INVALID_HANDLE_VALUE &&
        GetConsoleMode(outputHandle, &mode))
    {
        fflush(stream);
        wideLength = MultiByteToWideChar(CP_UTF8, 0, value, bytes, wideBuffer, 4096);
        if (wideLength > 0)
        {
            WriteConsoleW(outputHandle, wideBuffer, (DWORD)wideLength, &written, NULL);
            return;
        }
    }
#endif

    fwrite(value, 1, (size_t)bytes, stream);
}

#ifdef _WIN32
static int readConsoleUtf8Line(char value[], size_t size)
{
    HANDLE inputHandle;
    DWORD mode;
    DWORD charsRead;
    wchar_t wideInput[1024];
    size_t outputLength = 0;
    int i;

    if (size == 0)
    {
        return 0;
    }

    inputHandle = GetStdHandle(STD_INPUT_HANDLE);
    if (inputHandle == INVALID_HANDLE_VALUE || !GetConsoleMode(inputHandle, &mode))
    {
        return -1;
    }

    if (!ReadConsoleW(inputHandle, wideInput, 1023, &charsRead, NULL))
    {
        value[0] = '\0';
        return 0;
    }

    if (charsRead >= 1023)
    {
        charsRead = 1023;
    }
    wideInput[charsRead] = L'\0';

    for (i = 0; wideInput[i] != L'\0'; i++)
    {
        char utf8Char[8];
        int byteCount;

        byteCount = WideCharToMultiByte(CP_UTF8, 0, &wideInput[i], 1, utf8Char, sizeof(utf8Char), NULL, NULL);
        if (byteCount <= 0)
        {
            continue;
        }

        if (outputLength + (size_t)byteCount >= size)
        {
            break;
        }

        memcpy(value + outputLength, utf8Char, (size_t)byteCount);
        outputLength += (size_t)byteCount;
    }

    value[outputLength] = '\0';
    return 1;
}
#endif

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

static int decodeUtf8Codepoint(const char value[], int *bytes, unsigned int *codepoint)
{
    unsigned char first = (unsigned char)value[0];

    if (first < 0x80)
    {
        *bytes = 1;
        *codepoint = first;
        return 1;
    }

    if ((first & 0xE0) == 0xC0 &&
        isUtf8Continuation((unsigned char)value[1]))
    {
        *bytes = 2;
        *codepoint = ((unsigned int)(first & 0x1F) << 6) |
                     ((unsigned int)value[1] & 0x3F);
        return 1;
    }

    if ((first & 0xF0) == 0xE0 &&
        isUtf8Continuation((unsigned char)value[1]) &&
        isUtf8Continuation((unsigned char)value[2]))
    {
        *bytes = 3;
        *codepoint = ((unsigned int)(first & 0x0F) << 12) |
                     (((unsigned int)value[1] & 0x3F) << 6) |
                     ((unsigned int)value[2] & 0x3F);
        return 1;
    }

    if ((first & 0xF8) == 0xF0 &&
        isUtf8Continuation((unsigned char)value[1]) &&
        isUtf8Continuation((unsigned char)value[2]) &&
        isUtf8Continuation((unsigned char)value[3]))
    {
        *bytes = 4;
        *codepoint = ((unsigned int)(first & 0x07) << 18) |
                     (((unsigned int)value[1] & 0x3F) << 12) |
                     (((unsigned int)value[2] & 0x3F) << 6) |
                     ((unsigned int)value[3] & 0x3F);
        return 1;
    }

    return 0;
}

static int cp1252ByteFromCodepoint(unsigned int codepoint, unsigned char *byte)
{
    if (codepoint <= 0x7F || (codepoint >= 0xA0 && codepoint <= 0xFF))
    {
        *byte = (unsigned char)codepoint;
        return 1;
    }

    switch (codepoint)
    {
        case 0x20AC: *byte = 0x80; return 1;
        case 0x201A: *byte = 0x82; return 1;
        case 0x0192: *byte = 0x83; return 1;
        case 0x201E: *byte = 0x84; return 1;
        case 0x2026: *byte = 0x85; return 1;
        case 0x2020: *byte = 0x86; return 1;
        case 0x2021: *byte = 0x87; return 1;
        case 0x02C6: *byte = 0x88; return 1;
        case 0x2030: *byte = 0x89; return 1;
        case 0x0160: *byte = 0x8A; return 1;
        case 0x2039: *byte = 0x8B; return 1;
        case 0x0152: *byte = 0x8C; return 1;
        case 0x017D: *byte = 0x8E; return 1;
        case 0x2018: *byte = 0x91; return 1;
        case 0x2019: *byte = 0x92; return 1;
        case 0x201C: *byte = 0x93; return 1;
        case 0x201D: *byte = 0x94; return 1;
        case 0x2022: *byte = 0x95; return 1;
        case 0x2013: *byte = 0x96; return 1;
        case 0x2014: *byte = 0x97; return 1;
        case 0x02DC: *byte = 0x98; return 1;
        case 0x2122: *byte = 0x99; return 1;
        case 0x0161: *byte = 0x9A; return 1;
        case 0x203A: *byte = 0x9B; return 1;
        case 0x0153: *byte = 0x9C; return 1;
        case 0x017E: *byte = 0x9E; return 1;
        case 0x0178: *byte = 0x9F; return 1;
        default:
            if (codepoint >= 0x80 && codepoint <= 0x9F)
            {
                *byte = (unsigned char)codepoint;
                return 1;
            }
            return 0;
    }
}

static int isValidUtf8Bytes(const unsigned char value[], size_t length, int *hasNonAscii)
{
    size_t i = 0;

    *hasNonAscii = 0;

    while (i < length)
    {
        unsigned char first = value[i];

        if (first < 0x80)
        {
            i++;
        }
        else if ((first & 0xE0) == 0xC0 &&
                 i + 1 < length &&
                 isUtf8Continuation(value[i + 1]))
        {
            *hasNonAscii = 1;
            i += 2;
        }
        else if ((first & 0xF0) == 0xE0 &&
                 i + 2 < length &&
                 isUtf8Continuation(value[i + 1]) &&
                 isUtf8Continuation(value[i + 2]))
        {
            *hasNonAscii = 1;
            i += 3;
        }
        else if ((first & 0xF8) == 0xF0 &&
                 i + 3 < length &&
                 isUtf8Continuation(value[i + 1]) &&
                 isUtf8Continuation(value[i + 2]) &&
                 isUtf8Continuation(value[i + 3]))
        {
            *hasNonAscii = 1;
            i += 4;
        }
        else
        {
            return 0;
        }
    }

    return 1;
}

static void repairMojibakeUtf8(char value[], size_t size)
{
    unsigned char repaired[4096];
    size_t readIndex = 0;
    size_t writeIndex = 0;
    size_t length = strlen(value);
    int hasNonAscii;

    if (length == 0 || length >= sizeof(repaired))
    {
        return;
    }

    while (readIndex < length)
    {
        int bytes;
        unsigned int codepoint;
        unsigned char byte;

        if (!decodeUtf8Codepoint(value + readIndex, &bytes, &codepoint) ||
            !cp1252ByteFromCodepoint(codepoint, &byte))
        {
            return;
        }

        repaired[writeIndex++] = byte;
        readIndex += (size_t)bytes;
    }

    if (writeIndex + 1 > size ||
        !isValidUtf8Bytes(repaired, writeIndex, &hasNonAscii) ||
        !hasNonAscii)
    {
        return;
    }

    repaired[writeIndex] = '\0';

    if (strcmp(value, (char *)repaired) != 0)
    {
        strcpy(value, (char *)repaired);
    }
}

void repairTextEncoding(char value[], size_t size)
{
    repairMojibakeUtf8(value, size);
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

    if (value != NULL && bytes > 0)
    {
        fprintUtf8Bytes(stream, value, bytes);
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

void fprintText(FILE *stream, const char value[])
{
    if (value == NULL)
    {
        return;
    }

    fprintUtf8Bytes(stream, value, (int)strlen(value));
}

void printText(const char value[])
{
    fprintText(stdout, value);
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

#ifdef _WIN32
    int consoleResult = readConsoleUtf8Line(value, size);
    if (consoleResult == 1)
    {
        removeTrailingNewline(value);
        trimWhitespace(value);
        repairMojibakeUtf8(value, size);
        return 1;
    }
    if (consoleResult == 0)
    {
        return 0;
    }
#endif

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
    repairMojibakeUtf8(value, size);
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

static int buildDate(int day, int month, int year, time_t *out)
{
    time_t parsed;
    struct tm tmDate = {0};
    struct tm *normalized;

    if (day < 1 || day > 31 || month < 1 || month > 12 || year < 1900)
    {
        return 0;
    }

    tmDate.tm_mday = day;
    tmDate.tm_mon = month - 1;
    tmDate.tm_year = year - 1900;
    tmDate.tm_hour = 0;
    tmDate.tm_min = 0;
    tmDate.tm_sec = 0;
    tmDate.tm_isdst = -1;

    parsed = mktime(&tmDate);
    if (parsed == (time_t)-1)
    {
        return 0;
    }

    normalized = localtime(&parsed);
    if (normalized == NULL ||
        normalized->tm_mday != day ||
        normalized->tm_mon != month - 1 ||
        normalized->tm_year != year - 1900)
    {
        return 0;
    }

    *out = parsed;
    return 1;
}

static int buildToday(time_t *out)
{
    time_t now = time(NULL);
    struct tm *nowInfo = localtime(&now);

    if (nowInfo == NULL)
    {
        return 0;
    }

    return buildDate(nowInfo->tm_mday, nowInfo->tm_mon + 1, nowInfo->tm_year + 1900, out);
}

int promptDateToTime(time_t *out)
{
    char dateInput[32];
    int d;
    int m;
    int y;
    char extra;

    while (1)
    {
        printf("Enter date (dd/mm/yyyy, Enter for today, 0 to cancel): ");
        if (!readLine(dateInput, sizeof(dateInput)))
        {
            return 0;
        }

        if (strcmp(dateInput, "0") == 0)
        {
            return 0;
        }

        if (dateInput[0] == '\0')
        {
            if (buildToday(out))
            {
                return 1;
            }

            printError("Could not read today's date.");
            return 0;
        }

        if (sscanf(dateInput, "%d/%d/%d %c", &d, &m, &y, &extra) != 3 ||
            !buildDate(d, m, y, out))
        {
            printError("Invalid date. Use dd/mm/yyyy, Enter for today, or 0 to cancel.");
            continue;
        }

        return 1;
    }
}
