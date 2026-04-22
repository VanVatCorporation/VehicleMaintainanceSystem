#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "customer.h"

// Display customer information
void displayCustomer(Customer customer)
{
    printf("\n===== Customer Information =====\n");
    printf("Customer ID : %s\n", customer.customerId);
    printf("Full Name   : %s\n", customer.fullName);
    printf("Phone Number: %s\n", customer.phoneNumber);
    printf("Car Plate   : %s\n", customer.carPlate);
    printf("Car Type    : %s\n", customer.carType);
    printf("Order Count : %d\n", customer.orderCount);
}

// Find customer index by phone number
int findCustomerIndexByPhone(Customer customers[], int customerCount, char phoneNumber[])
{
    for (int i = 0; i < customerCount; i++)
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
    for (int i = 0; i < customerCount; i++)
    {
        if (strcmp(customers[i].carPlate, carPlate) == 0)
        {
            return i;
        }
    }

    return -1;
}

// Validate phone number format (simple validation)
int isValidPhoneNumber(char phoneNumber[])
{

    size_t length = strlen(phoneNumber);
    if (length != 10)
    {
        return 0; // Phone number must be exactly 10 digits
    }

    if (phoneNumber[0] != '0')
    {             // Phone number must start with '0'
        return 0; //
    }

    for (size_t i = 0; i < length; i++)
    {
        if (!isdigit((unsigned char)(phoneNumber[i])))
        {
            return 0;
        }
    }

    return 1; // Valid phone number
}

// Check for duplicate phone numbers
int isDuplicatePhoneNumber(Customer customers[], int customerCount, char phoneNumber[])
{

    for (int i = 0; i < customerCount; i++)
    {
        if (strcmp(customers[i].phoneNumber, phoneNumber) == 0)
        {
            return 1; // Return 1 if phone number already exists, otherwise 0
        }
    }

    return 0;
}

// Generate a unique customer ID based on the current customer count
void generateCustomerId(char customerId[], int customerCount)
{
    snprintf(customerId, ID_LENGTH, "CU%06d", customerCount + 1); // Generate ID in format CU000001, CU000002, etc.
}

// Search and display customer by phone number
void searchCustomerByPhone(Customer customers[], int customerCount)
{
    char phoneNumber[PHONE_LENGTH];

    printf("Enter phone number: ");
    scanf("%10s", phoneNumber);

    int index = findCustomerIndexByPhone(customers, customerCount, phoneNumber);

    if (index == -1)
    {
        printf("Customer not found:\n");
    }
    else
    {
        displayCustomer(customers[index]);
    }
}

// Search and display customer by car plate
void searchCustomerByPlate(Customer customers[], int customerCount)
{
    char carPlate[PLATE_LENGTH];

    printf("Enter car plate: ");
    scanf("%19s", carPlate);

    int index = findCustomerIndexByPlate(customers, customerCount, carPlate);

    if (index == -1)
    {
        printf("Customer not found.\n");
    }
    else
    {
        displayCustomer(customers[index]);
    }
}

// Add a new customer to the system
void addCustomer(Customer customers[], int *customerCount)
{
    // Check if the customer list is full before adding a new customer
    if (*customerCount >= MAX_CUSTOMERS)
    {
        printf("Customer list is full. Cannot add more customers.\n");
        return;
    }

    Customer newCustomer;

    // Full name
    while (1)
    {
        int i;

        printf("Enter full name: ");
        scanf(" %49[^\n]", newCustomer.fullName);

        // Validate that the full name is not empty or contains only whitespace
        for (i = 0; newCustomer.fullName[i] != '\0'; i++)
        {
            if (!isspace((unsigned char)(newCustomer.fullName[i])))
            {
                break;
            }
        }

        if (newCustomer.fullName[i] == '\0')
        {
            printf("Full name cannot be empty. Please try again.\n");
        }
        else
        {
            break;
        }
    }

    // Phone number
    while (1)
    {

        printf("Enter phone number: ");
        scanf("%10s", newCustomer.phoneNumber);

        // Validate phone number format and check for duplicates
        if (!isValidPhoneNumber(newCustomer.phoneNumber))
        {
            printf("Invalid phone number. Please try again.\n");
        }
        else if (isDuplicatePhoneNumber(customers, *customerCount, newCustomer.phoneNumber))
        {
            printf("Phone number already exists. Please enter another one.\n");
        }
        else
        {
            break;
        }
    }

    // Car plate
    while (1)
    {
        int i;

        printf("Enter car plate: ");
        scanf(" %19[^\n]", newCustomer.carPlate);

        // Validate that the car plate is not empty or contains only whitespace
        for (i = 0; newCustomer.carPlate[i] != '\0'; i++)
        {
            if (!isspace((unsigned char)(newCustomer.carPlate[i])))
            {
                break;
            }
        }

        if (newCustomer.carPlate[i] == '\0')
        {
            printf("Car plate cannot be empty. Please try again.\n");
        }
        else
        {
            break;
        }
    }

    // Car type
    while (1) {
        int i;

        printf("Enter car type: ");
        scanf(" %30[^\n]", newCustomer.carType);
        
        // Validate that the car type is not empty or contains only whitespace
        for (i = 0; newCustomer.carType[i] != '\0'; i++) {
            if (!isspace((unsigned char)(newCustomer.carType[i]))) {
                break;
            }
        }

        if (newCustomer.carType[i] == '\0') {
            printf("Car type cannot be empty. Please try again.\n");
        } else {
            break;
        }
    }

    // Generate a unique customer ID and initialize order count
    generateCustomerId(newCustomer.customerId, *customerCount);
    newCustomer.orderCount = 0;

    customers[*customerCount] = newCustomer;
    (*customerCount)++;

    printf("Customer added successfully.\n");

}

// 
