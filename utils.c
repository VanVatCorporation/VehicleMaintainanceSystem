#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "utils.h"

#define INPUT_LENGTH 100
#define DEFAULT_UI_WIDTH 64
#define UI_COLOR_RESET "\033[0m"
#define UI_COLOR_BLUE "\033[34m"
#define UI_COLOR_GREEN "\033[32m"
#define UI_COLOR_RED "\033[31m"
#define UI_COLOR_CYAN "\033[36m"

// In lặp lại một ký tự ra stream bất kỳ, dùng chung cho console và file hóa đơn.
static void fprintRepeated(FILE *stream, char value, int count)
{
    int i;

    for (i = 0; i < count; i++)
    {
        fputc(value, stream);
    }
}

// Bản rút gọn để in ký tự lặp trực tiếp ra màn hình console.
static void printRepeated(char value, int count)
{
    fprintRepeated(stdout, value, count);
}

// Byte tiếp diễn trong UTF-8 luôn có dạng bit 10xxxxxx.
static int isUtf8Continuation(unsigned char value)
{
    return (value & 0xC0) == 0x80;
}

// Xác định một ký tự UTF-8 đang chiếm bao nhiêu byte để không cắt lỗi tiếng Việt.
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

// Lấy một đoạn chuỗi vừa đủ độ rộng hiển thị, tính theo số ký tự thay vì số byte.
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

// Giới hạn độ rộng khung UI để tiêu đề không quá hẹp hoặc quá dài.
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

// In tiêu đề trong khung màu xanh, tự căn giữa và hỗ trợ chuỗi UTF-8.
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

// In tiêu đề section với độ rộng mặc định của UI console.
void printSectionTitle(const char title[])
{
    printBoxTitle(title, DEFAULT_UI_WIDTH);
}

// In text có padding cố định ra stream; alignRight = 1 dùng cho cột số tiền.
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

// In text có padding cố định ra màn hình console.
void printPaddedText(const char value[], int width, int alignRight)
{
    fprintPaddedText(stdout, value, width, alignRight);
}

// In một lựa chọn menu theo format thống nhất.
void printMenuOption(int number, const char label[])
{
    printf("  [%d] %s\n", number, label);
}

// In lựa chọn quay lại để các menu con dùng chung.
void printMenuBack(void)
{
    printf("  [0] Back\n");
}

// In thông báo thao tác thành công bằng màu xanh lá.
void printSuccess(const char message[])
{
    printf("%s[OK] %s%s\n", UI_COLOR_GREEN, message, UI_COLOR_RESET);
}

// In thông báo lỗi bằng màu đỏ để người dùng dễ nhận ra.
void printError(const char message[])
{
    printf("%s[ERROR] %s%s\n", UI_COLOR_RED, message, UI_COLOR_RESET);
}

// In thông tin phụ bằng màu xanh cyan.
void printInfo(const char message[])
{
    printf("%s[INFO] %s%s\n", UI_COLOR_CYAN, message, UI_COLOR_RESET);
}

// Xóa ký tự xuống dòng do fgets để lại ở cuối input.
void removeTrailingNewline(char value[])
{
    value[strcspn(value, "\n")] = '\0';
}

// Xóa khoảng trắng đầu/cuối chuỗi nhưng giữ nguyên khoảng trắng ở giữa.
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

// Đọc trọn một dòng input để tránh scanf bỏ sót dữ liệu hoặc kẹt ký tự thừa.
int readLine(char value[], size_t size)
{
    int ch;

    if (fgets(value, size, stdin) == NULL)
    {
        return 0;
    }

    if (strchr(value, '\n') == NULL)
    {
        // Nếu người dùng nhập quá dài, đọc bỏ phần còn lại để lần nhập sau không bị ảnh hưởng.
        while ((ch = getchar()) != '\n' && ch != EOF)
            ;
    }

    removeTrailingNewline(value);
    trimWhitespace(value);
    return 1;
}

// Đọc lựa chọn số nguyên và loại các input sai như "1abc".
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
