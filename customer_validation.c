#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "customer.h"

// Kiểm tra chuỗi rỗng sau khi input đã được trim.
static int isBlank(char value[])
{
    return value[0] == '\0';
}

// Kiểm tra chữ cái seri biển số thông dụng ở Việt Nam.
static int isAllowedSeriesLetter(char value)
{
    // Các chữ cái seri biển số thông dụng ở Việt Nam.
    return strchr("ABCDEFGHKLMNPSTUVXYZ", value) != NULL;
}

// Kiểm tra seri một chữ cái thuộc nhóm đặc biệt.
static int isSpecialOneLetterSeries(char value)
{
    // R dùng cho rơ-moóc và sơ-mi rơ-moóc.
    return value == 'R';
}

// Kiểm tra các seri hai chữ cái đặc biệt như xe ngoại giao, quân đội, liên doanh...
static int isSpecialTwoLetterSeries(char first, char second)
{
    char series[3];

    series[0] = first;
    series[1] = second;
    series[2] = '\0';

    // MD được dùng thay cho MĐ để nhập bằng ký tự ASCII trong terminal.
    return strcmp(series, "CD") == 0 ||
           strcmp(series, "LD") == 0 ||
           strcmp(series, "DA") == 0 ||
           strcmp(series, "KT") == 0 ||
           strcmp(series, "MD") == 0 ||
           strcmp(series, "RM") == 0 ||
           strcmp(series, "HC") == 0 ||
           strcmp(series, "NG") == 0 ||
           strcmp(series, "QT") == 0 ||
           strcmp(series, "CV") == 0 ||
           strcmp(series, "NN") == 0;
}

// Kiểm tra phần số thứ tự cuối biển số có đúng 5 chữ số không.
static int hasFiveOrderDigits(char carPlate[], int startIndex)
{
    int i;

    for (i = startIndex; carPlate[i] != '\0'; i++)
    {
        if (!isdigit((unsigned char)carPlate[i]))
        {
            return 0;
        }
    }

    return strlen(carPlate + startIndex) == 5;
}

// Họ tên không được rỗng và không được chứa chữ số.
int isValidFullName(char fullName[])
{
    int i;

    if (isBlank(fullName))
    {
        return 0;
    }

    for (i = 0; fullName[i] != '\0'; i++)
    {
        if (isdigit((unsigned char)fullName[i]))
        {
            return 0;
        }
    }

    return 1;
}

// SĐT phải có đúng 10 chữ số và bắt đầu bằng số 0.
int isValidPhoneNumber(char phoneNumber[])
{
    size_t length = strlen(phoneNumber);
    size_t i;

    if (length != 10)
    {
        return 0; // SĐT phải có đúng 10 chữ số.
    }

    if (phoneNumber[0] != '0')
    {
        return 0; // SĐT Việt Nam trong project phải bắt đầu bằng '0'.
    }

    for (i = 0; i < length; i++)
    {
        if (!isdigit((unsigned char)phoneNumber[i]))
        {
            return 0;
        }
    }

    return 1;
}

// Validate biển số sau khi đã bỏ dấu phân cách và chuyển về chữ hoa.
int isValidCarPlate(char carPlate[])
{
    size_t length = strlen(carPlate);

    // Các dạng hợp lệ sau khi chuẩn hóa:
    // 30A12345  = dạng hiển thị 30A-123.45
    // 29AA12345 = dạng hiển thị 29-AA 123.45
    // 30LD12345 = seri đặc biệt như LD, NG, CV, NN.
    if (length != 8 && length != 9)
    {
        return 0;
    }

    if (!isdigit((unsigned char)carPlate[0]) || !isdigit((unsigned char)carPlate[1]))
    {
        return 0;
    }

    if (length == 8)
    {
        // Dạng 2 số tỉnh + 1 chữ seri + 5 số thứ tự.
        if (!isAllowedSeriesLetter(carPlate[2]) && !isSpecialOneLetterSeries(carPlate[2]))
        {
            return 0;
        }

        return hasFiveOrderDigits(carPlate, 3);
    }

    // Dạng 2 số tỉnh + 2 chữ seri + 5 số thứ tự.
    if (!isSpecialTwoLetterSeries(carPlate[2], carPlate[3]) &&
        (!isAllowedSeriesLetter(carPlate[2]) || !isAllowedSeriesLetter(carPlate[3])))
    {
        return 0;
    }

    return hasFiveOrderDigits(carPlate, 4);
}

// Chuẩn hóa biển số để mọi cách nhập như "30A-123.45" hay "30a 12345" đều so sánh được.
void normalizeCarPlate(char carPlate[])
{
    int readIndex;
    int writeIndex = 0;

    // Bỏ các dấu phân cách phổ biến và chuyển chữ cái về in hoa.
    for (readIndex = 0; carPlate[readIndex] != '\0'; readIndex++)
    {
        unsigned char current = (unsigned char)carPlate[readIndex];

        if (current == '-' || current == '.' || isspace(current))
        {
            continue;
        }

        carPlate[writeIndex] = (char)toupper(current);
        writeIndex++;
    }

    carPlate[writeIndex] = '\0';
}
