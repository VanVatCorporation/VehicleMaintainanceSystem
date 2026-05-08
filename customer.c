#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "customer.h"
#include "utils.h"

#define INPUT_LENGTH 100
#define MAX_UPDATE_ATTEMPTS 5

static int isBlank(char value[])
{
    return value[0] == '\0';
}

static int equalsIgnoreCase(char first[], char second[])
{
    int i = 0;

    while (first[i] != '\0' && second[i] != '\0')
    {
        if (toupper((unsigned char)first[i]) != toupper((unsigned char)second[i]))
        {
            return 0;
        }

        i++;
    }

    return first[i] == '\0' && second[i] == '\0';
}

// Display options after a failed search.
static int askSearchOption(void)
{
    int choice;

    printSectionTitle("SEARCH OPTIONS");
    printMenuOption(1, "Try again");
    printMenuOption(2, "Exit search");
    printf("\nSelect option: ");

    if (!readInt(&choice))
    {
        printError("Invalid choice. Exit search.");
        return 0;
    }

    if (choice == 1)
    {
        return 1;
    }

    if (choice == 2)
    {
        return 0;
    }

    printError("Invalid choice. Exit search.");
    return 0;
}

static int isDuplicateCarPlateExcept(Customer customers[], int customerCount, char carPlate[], int exceptIndex)
{
    int i;

    for (i = 0; i < customerCount; i++)
    {
        if (i != exceptIndex && equalsIgnoreCase(customers[i].carPlate, carPlate))
        {
            return 1;
        }
    }

    return 0;
}

static int inputFullName(char fullName[], char prompt[], int allowEmpty)
{
    char input[INPUT_LENGTH];

    while (1)
    {
        printf("%s", prompt);

        if (!readLine(input, sizeof(input)))
        {
            printError("Input error. Please try again.");
            continue;
        }

        if (allowEmpty && input[0] == '\0')
        {
            return 0;
        }

        if (strlen(input) >= NAME_LENGTH)
        {
            printError("Full name is too long. Please try again.");
        }
        else if (!isValidFullName(input))
        {
            printError("Full name cannot be empty or contain digits. Please try again.");
        }
        else
        {
            strcpy(fullName, input);
            return 1;
        }
    }
}

static int inputPhoneNumber(char phoneNumber[], Customer customers[], int customerCount)
{
    char input[INPUT_LENGTH];

    while (1)
    {
        printf("Enter phone number: ");

        if (!readLine(input, sizeof(input)))
        {
            printError("Input error. Please try again.");
            continue;
        }

        // Validate phone number format and check for duplicates.
        if (!isValidPhoneNumber(input))
        {
            printError("Invalid phone number. Please try again.");
        }
        else if (isDuplicatePhoneNumber(customers, customerCount, input))
        {
            printError("Phone number already exists. Please enter another one.");
        }
        else
        {
            strcpy(phoneNumber, input);
            return 1;
        }
    }
}

static int inputCarPlate(char carPlate[], char prompt[], int allowEmpty, Customer customers[], int customerCount, int exceptIndex)
{
    char input[INPUT_LENGTH];

    while (1)
    {
        printf("%s", prompt);

        if (!readLine(input, sizeof(input)))
        {
            printError("Input error. Please try again.");
            continue;
        }

        if (allowEmpty && input[0] == '\0')
        {
            return 0;
        }

        normalizeCarPlate(input);

        if (strlen(input) >= PLATE_LENGTH)
        {
            printError("Car plate is too long. Please try again.");
        }
        else if (isBlank(input))
        {
            printError("Car plate cannot be empty. Please try again.");
        }
        else if (!isValidCarPlate(input))
        {
            printError("Invalid car plate. Use format like 30A-123.45 or 29-AA 123.45.");
        }
        else if (isDuplicateCarPlateExcept(customers, customerCount, input, exceptIndex))
        {
            printError("Car plate already exists. Please enter another one.");
        }
        else
        {
            strcpy(carPlate, input);
            return 1;
        }
    }
}

static int inputCarType(char carType[], char prompt[], int allowEmpty)
{
    char input[INPUT_LENGTH];

    while (1)
    {
        printf("%s", prompt);

        if (!readLine(input, sizeof(input)))
        {
            printError("Input error. Please try again.");
            continue;
        }

        if (allowEmpty && input[0] == '\0')
        {
            return 0;
        }

        if (strlen(input) >= TYPE_LENGTH)
        {
            printError("Car type is too long. Please try again.");
        }
        else if (isBlank(input))
        {
            printError("Car type cannot be empty. Please try again.");
        }
        else
        {
            strcpy(carType, input);
            return 1;
        }
    }
}

static int findCustomerForUpdate(Customer customers[], int customerCount)
{
    char phoneNumber[INPUT_LENGTH];
    int attempts = 0;
    int index;

    // Allow the user up to 5 attempts to enter a valid phone number.
    while (attempts < MAX_UPDATE_ATTEMPTS)
    {
        printf("Enter phone number to update: ");

        if (!readLine(phoneNumber, sizeof(phoneNumber)))
        {
            printError("Input error. Please try again.");
            continue;
        }

        if (!isValidPhoneNumber(phoneNumber))
        {
            attempts++;
            printError("Invalid phone number. Please try again.");
        }
        else
        {
            index = findCustomerIndexByPhone(customers, customerCount, phoneNumber);

            if (index != -1)
            {
                return index;
            }

            attempts++;
            printError("Customer not found.");
        }

        if (attempts < MAX_UPDATE_ATTEMPTS)
        {
            printf("[INFO] Remaining attempts: %d\n", MAX_UPDATE_ATTEMPTS - attempts);
        }
    }

    return -1;
}

static void displayUpdateMenu(void)
{
    printSectionTitle("UPDATE CUSTOMER");
    printMenuOption(1, "Full name");
    printMenuOption(2, "Car plate");
    printMenuOption(3, "Car type");
    printMenuOption(4, "Finish update");
    printf("\nSelect option: ");
}

static void printCustomerDivider(void)
{
    printf("+------------------+-------------------------------------------------+\n");
}

static void printCustomerTextRow(const char field[], const char value[])
{
    printf("| %-16s | %-47.47s |\n", field, value);
}

static void printCustomerIntRow(const char field[], int value)
{
    printf("| %-16s | %-47d |\n", field, value);
}

// Display customer information
void displayCustomer(Customer c)
{
    printBoxTitle("CUSTOMER PROFILE", 70);
    printCustomerDivider();
    printCustomerTextRow("Customer ID", c.customerId);
    printCustomerTextRow("Full name", c.fullName);
    printCustomerTextRow("Phone", c.phoneNumber);
    printCustomerTextRow("Car plate", c.carPlate);
    printCustomerTextRow("Car type", c.carType);
    printCustomerIntRow("Orders", c.orderCount);
    printCustomerDivider();
}

// Find customer index by phone number
int findCustomerIndexByPhone(Customer customers[], int customerCount, char phoneNumber[])
{
    int i;

    for (i = 0; i < customerCount; i++)
    {
        if (strcmp(customers[i].phoneNumber, phoneNumber) == 0)
        {
            return i;
        }
    }

    return -1;
}

// Find customer index by car plate
int findCustomerIndexByPlate(Customer customers[], int customerCount, char carPlate[])
{
    int i;

    for (i = 0; i < customerCount; i++)
    {
        if (equalsIgnoreCase(customers[i].carPlate, carPlate))
        {
            return i;
        }
    }

    return -1;
}

// Check for duplicate phone numbers
int isDuplicatePhoneNumber(Customer customers[], int customerCount, char phoneNumber[])
{
    int i;

    for (i = 0; i < customerCount; i++)
    {
        if (strcmp(customers[i].phoneNumber, phoneNumber) == 0)
        {
            return 1;
        }
    }

    return 0;
}

int isDuplicateCarPlate(Customer customers[], int customerCount, char carPlate[])
{
    return isDuplicateCarPlateExcept(customers, customerCount, carPlate, -1);
}

// Generate a unique customer ID based on the current customer count
void generateCustomerId(char customerId[], int customerCount)
{
    int nextId = customerCount + 1;

    if (nextId < 0)
    {
        nextId = 0;
    }
    else if (nextId > 999999)
    {
        nextId = 999999;
    }

    snprintf(customerId, ID_LENGTH, "CU%06d", nextId);
}

// Search and display customer by phone number
void searchCustomerByPhone(Customer customers[], int customerCount)
{
    char phoneNumber[INPUT_LENGTH];
    int index;

    printSectionTitle("SEARCH BY PHONE");

    while (1)
    {
        printf("Enter phone number: ");

        if (!readLine(phoneNumber, sizeof(phoneNumber)))
        {
            printError("Input error. Exit search.");
            return;
        }

        if (!isValidPhoneNumber(phoneNumber))
        {
            printError("Invalid phone number.");
        }
        else
        {
            index = findCustomerIndexByPhone(customers, customerCount, phoneNumber);

            if (index != -1)
            {
                displayCustomer(customers[index]);
                return;
            }

            printError("Customer not found.");
        }

        if (!askSearchOption())
        {
            return;
        }
    }
}

// Search and display customer by car plate
void searchCustomerByPlate(Customer customers[], int customerCount)
{
    char carPlate[INPUT_LENGTH];
    int index;

    printSectionTitle("SEARCH BY CAR PLATE");

    while (1)
    {
        printf("Enter car plate: ");

        if (!readLine(carPlate, sizeof(carPlate)))
        {
            printError("Input error. Exit search.");
            return;
        }

        normalizeCarPlate(carPlate);

        if (!isValidCarPlate(carPlate))
        {
            printError("Invalid car plate. Use format like 30A-123.45 or 29-AA 123.45.");
        }
        else
        {
            index = findCustomerIndexByPlate(customers, customerCount, carPlate);

            if (index != -1)
            {
                displayCustomer(customers[index]);
                return;
            }

            printError("Customer not found.");
        }

        if (!askSearchOption())
        {
            return;
        }
    }
}

// Add a new customer to the system
void addCustomer(Customer customers[], int *customerCount)
{
    Customer newCustomer;

    printSectionTitle("ADD CUSTOMER");

    // Check if the customer list is full before adding a new customer
    if (*customerCount >= MAX_CUSTOMERS)
    {
        printError("Customer list is full. Cannot add more customers.");
        return;
    }

    // Get customer information
    inputFullName(newCustomer.fullName, "Enter full name: ", 0);
    inputPhoneNumber(newCustomer.phoneNumber, customers, *customerCount);
    inputCarPlate(newCustomer.carPlate, "Enter car plate: ", 0, customers, *customerCount, -1);
    inputCarType(newCustomer.carType, "Enter car type: ", 0);

    // Generate a unique customer ID and initialize order count
    generateCustomerId(newCustomer.customerId, *customerCount);
    newCustomer.orderCount = 0;

    customers[*customerCount] = newCustomer;
    (*customerCount)++;

    printSuccess("Customer added successfully.");
    displayCustomer(newCustomer);
}

// Update existing customer information
void updateCustomer(Customer customers[], int *customerCount)
{
    int index;
    int choice;
    int updated;

    printSectionTitle("FIND CUSTOMER TO UPDATE");

    while (1)
    {
        index = findCustomerForUpdate(customers, *customerCount);

        if (index != -1)
        {
            break;
        }

        printSectionTitle("CUSTOMER NOT FOUND");
        printMenuOption(1, "Try again");
        printMenuOption(2, "Exit");
        printMenuOption(3, "Add new customer");
        printf("\nSelect option: ");

        if (!readInt(&choice))
        {
            printError("Invalid choice. Try again.");
            continue;
        }

        if (choice == 1)
        {
            continue;
        }
        else if (choice == 2)
        {
            return;
        }
        else if (choice == 3)
        {
            addCustomer(customers, customerCount);
            return;
        }
        else
        {
            printError("Invalid choice. Try again.");
        }
    }

    printSuccess("Customer found. Proceeding to update.");
    displayCustomer(customers[index]);

    while (1)
    {
        displayUpdateMenu();

        if (!readInt(&choice))
        {
            printError("Invalid choice. Try again.");
            continue;
        }

        if (choice == 1)
        {
            printf("Current full name: %s\n", customers[index].fullName);
            updated = inputFullName(customers[index].fullName, "Enter new full name (press Enter to keep current): ", 1);

            if (updated)
            {
                printSuccess("Full name updated.");
            }
        }
        else if (choice == 2)
        {
            printf("Current car plate: %s\n", customers[index].carPlate);
            updated = inputCarPlate(customers[index].carPlate, "Enter new car plate (press Enter to keep current): ", 1, customers, *customerCount, index);

            if (updated)
            {
                printSuccess("Car plate updated.");
            }
        }
        else if (choice == 3)
        {
            printf("Current car type: %s\n", customers[index].carType);
            updated = inputCarType(customers[index].carType, "Enter new car type (press Enter to keep current): ", 1);

            if (updated)
            {
                printSuccess("Car type updated.");
            }
        }
        else if (choice == 4)
        {
            break;
        }
        else
        {
            printError("Invalid choice. Try again.");
        }
    }

    printSuccess("Customer updated successfully.");
    displayCustomer(customers[index]);
}
