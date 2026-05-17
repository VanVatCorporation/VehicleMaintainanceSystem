#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "customer.h"

static int isBlank(char value[])
{
    return value[0] == '\0';
}

static int isUpperLetter(char value)
{
    return value >= 'A' && value <= 'Z';
}

static int isValidProvinceCode(char carPlate[])
{
    int code;

    if (!isdigit((unsigned char)carPlate[0]) || !isdigit((unsigned char)carPlate[1]))
    {
        return 0;
    }

    code = (carPlate[0] - '0') * 10 + (carPlate[1] - '0');
    return code >= 11 && code <= 99;
}

static int hasDigitRun(char value[], int startIndex, int count)
{
    int i;

    for (i = 0; i < count; i++)
    {
        if (!isdigit((unsigned char)value[startIndex + i]))
        {
            return 0;
        }
    }

    return value[startIndex + count] == '\0';
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

    /*
     * Regex-style rules after normalization:
     * [1-9][0-9][A-Z][0-9]{5} or [1-9][0-9][A-Z]{2}[0-9]{5},
     * with the province code constrained to 11..99.
     */
    if (length != 8 && length != 9)
    {
        return 0;
    }

    if (!isValidProvinceCode(carPlate))
    {
        return 0;
    }

    if (length == 8)
    {
        if (!isUpperLetter(carPlate[2]))
        {
            return 0;
        }

        return hasDigitRun(carPlate, 3, 5);
    }

    if (!isUpperLetter(carPlate[2]) || !isUpperLetter(carPlate[3]))
    {
        return 0;
    }

    return hasDigitRun(carPlate, 4, 5);
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
