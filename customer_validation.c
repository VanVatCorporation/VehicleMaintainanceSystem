#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "customer.h"

static int isBlank(char value[])
{
    return value[0] == '\0';
}

static int isAllowedSeriesLetter(char value)
{
    // Regular Vietnamese car plate series letters.
    return strchr("ABCDEFGHKLMNPSTUVXYZ", value) != NULL;
}

static int isSpecialOneLetterSeries(char value)
{
    // R is used for trailers and semi-trailers.
    return value == 'R';
}

static int isSpecialTwoLetterSeries(char first, char second)
{
    char series[3];

    series[0] = first;
    series[1] = second;
    series[2] = '\0';

    // MD is used as ASCII input for Vietnamese electric motorbike series.
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

// Validate phone number format.
int isValidPhoneNumber(char phoneNumber[])
{
    size_t length = strlen(phoneNumber);
    size_t i;

    if (length != 10)
    {
        return 0; // Phone number must be exactly 10 digits.
    }

    if (phoneNumber[0] != '0')
    {
        return 0; // Phone number must start with '0'.
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

int isValidCarPlate(char carPlate[])
{
    size_t length = strlen(carPlate);

    // Accepted after normalization:
    // 30A12345  = car format 30A-123.45
    // 29AA12345 = motorbike format 29-AA 123.45
    // 30LD12345 = special series such as LD, NG, CV, NN.
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
        if (!isAllowedSeriesLetter(carPlate[2]) && !isSpecialOneLetterSeries(carPlate[2]))
        {
            return 0;
        }

        return hasFiveOrderDigits(carPlate, 3);
    }

    if (!isSpecialTwoLetterSeries(carPlate[2], carPlate[3]) &&
        (!isAllowedSeriesLetter(carPlate[2]) || !isAllowedSeriesLetter(carPlate[3])))
    {
        return 0;
    }

    return hasFiveOrderDigits(carPlate, 4);
}

void normalizeCarPlate(char carPlate[])
{
    int readIndex;
    int writeIndex = 0;

    // Remove common separators and convert letters to uppercase.
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
