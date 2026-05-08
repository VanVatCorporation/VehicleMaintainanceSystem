#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "customer.h"

// Display customer information
void displayCustomer(Customer c)
{
    printf("\n=========================================\n");
    printf("| %-15s | %-20s |\n", "Field", "Value");
    printf("=========================================\n");
    printf("| %-15s | %-20s |\n", "Customer ID", c.customerId);
    printf("| %-15s | %-20s |\n", "Full Name", c.fullName);
    printf("| %-15s | %-20s |\n", "Phone", c.phoneNumber);
    printf("| %-15s | %-20s |\n", "Car Plate", c.carPlate);
    printf("| %-15s | %-20s |\n", "Car Type", c.carType);
    printf("| %-15s | %-20d |\n", "Orders", c.orderCount);
    printf("=========================================\n");
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

int isDuplicateCarPlate(Customer customers[], int customerCount, char carPlate[])
{
    int i;

    for (i = 0; i < customerCount; i++)
    {
        if (strcmp(customers[i].carPlate, carPlate) == 0)
        {
            return 1;
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
    char choiceInput[10];
    int choice;
    int ch;

    while (1)
    {
        printf("Enter phone number: ");
        scanf("%10s", phoneNumber);
        while ((ch = getchar()) != '\n' && ch != EOF)
            ;

        if (!isValidPhoneNumber(phoneNumber))
        {
            printf("Invalid phone number.\n");
        }
        else
        {
            int index = findCustomerIndexByPhone(customers, customerCount, phoneNumber);

            if (index == -1)
            {
                printf("Customer not found.\n");
            }
            else
            {
                displayCustomer(customers[index]);
                return;
            }
        }

        printf("\nOptions:\n");
        printf("1. Try again\n");
        printf("2. Exit search\n");
        printf("Choose: ");

        if (fgets(choiceInput, sizeof(choiceInput), stdin) == NULL)
        {
            printf("Input error. Exit search.\n");
            return;
        }

        if (sscanf(choiceInput, "%d", &choice) != 1)
        {
            printf("Invalid choice. Exit search.\n");
            return;
        }

        if (choice == 1)
        {
            continue;
        }
        else if (choice == 2)
        {
            return;
        }
        else
        {
            printf("Invalid choice. Exit search.\n");
            return;
        }
    }
}

// Search and display customer by car plate
void searchCustomerByPlate(Customer customers[], int customerCount)
{
    char carPlate[PLATE_LENGTH];
    char choiceInput[10];
    int choice;
    int ch;

    while (1)
    {
        printf("Enter car plate: ");
        scanf("%19s", carPlate);
        while ((ch = getchar()) != '\n' && ch != EOF)
            ;

        int index = findCustomerIndexByPlate(customers, customerCount, carPlate);

        if (index == -1)
        {
            printf("Customer not found.\n");
        }
        else
        {
            displayCustomer(customers[index]);
            return;
        }

        printf("\nOptions:\n");
        printf("1. Try again\n");
        printf("2. Exit search\n");
        printf("Choose: ");

        if (fgets(choiceInput, sizeof(choiceInput), stdin) == NULL)
        {
            printf("Input error. Exit search.\n");
            return;
        }

        if (sscanf(choiceInput, "%d", &choice) != 1)
        {
            printf("Invalid choice. Exit search.\n");
            return;
        }

        if (choice == 1)
        {
            continue;
        }
        else if (choice == 2)
        {
            return;
        }
        else
        {
            printf("Invalid choice. Exit search.\n");
            return;
        }
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
    int ch;

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
        while ((ch = getchar()) != '\n' && ch != EOF)
            ;

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

        for (i = 0; newCustomer.carPlate[i] != '\0'; i++)
        {
            if (!isspace((unsigned char)newCustomer.carPlate[i]))
            {
                break;
            }
        }

        if (newCustomer.carPlate[i] == '\0')
        {
            printf("Car plate cannot be empty. Please try again.\n");
        }
        else if (isDuplicateCarPlate(customers, *customerCount, newCustomer.carPlate))
        {
            printf("Car plate already exists. Please enter another one.\n");
        }
        else
        {
            break;
        }
    }

    // Car type
    while (1)
    {
        int i;

        printf("Enter car type: ");
        scanf(" %30[^\n]", newCustomer.carType);

        // Validate that the car type is not empty or contains only whitespace
        for (i = 0; newCustomer.carType[i] != '\0'; i++)
        {
            if (!isspace((unsigned char)(newCustomer.carType[i])))
            {
                break;
            }
        }

        if (newCustomer.carType[i] == '\0')
        {
            printf("Car type cannot be empty. Please try again.\n");
        }
        else
        {
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

// Update existing customer information
void updateCustomer(Customer customers[], int *customerCount)
{
    char phoneNumber[PHONE_LENGTH];
    char choiceInput[10];
    int index = -1;
    int choice;
    int ch;

    while (1)
    {
        int attempts = 0;

        // Allow the user up to 5 attempts to enter a valid phone number
        while (attempts < 5)
        {
            printf("Enter phone number to update: ");
            scanf("%10s", phoneNumber);
            while ((ch = getchar()) != '\n' && ch != EOF)
                ;

            if (!isValidPhoneNumber(phoneNumber))
            {
                attempts++;
                printf("Invalid phone number. Please try again.\n");

                if (attempts < 5)
                {
                    printf("Remaining attempts: %d\n", 5 - attempts);
                }

                continue;
            }

            index = findCustomerIndexByPhone(customers, *customerCount, phoneNumber);

            if (index != -1)
            {
                break;
            }

            attempts++;
            printf("Customer not found.\n");

            if (attempts < 5)
            {
                printf("Remaining attempts: %d\n", 5 - attempts);
            }
        }

        if (index != -1)
        {
            break;
        }

        printf("\nOptions:\n");
        printf("1. Try again\n");
        printf("2. Exit\n");
        printf("3. Add new customer\n");
        printf("Choose: ");

        if (fgets(choiceInput, sizeof(choiceInput), stdin) == NULL)
        {
            printf("Input error. Try again.\n");
            continue;
        }

        if (sscanf(choiceInput, "%d", &choice) != 1)
        {
            printf("Invalid choice. Try again.\n");
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
            printf("Invalid choice. Try again.\n");
        }
    }

    printf("Customer found. Proceeding to update...\n");
    displayCustomer(customers[index]);
    
 while (1)
    {
    printf("\n=================================\n");
    printf("         UPDATE MENU\n");
    printf("=================================\n");
    printf("| %-2d | %-20s |\n", 1, "Full name");
    printf("| %-2d | %-20s |\n", 2, "Car plate");
    printf("| %-2d | %-20s |\n", 3, "Car type");
    printf("| %-2d | %-20s |\n", 4, "Finish update");
    printf("=================================\n");
    printf("Choose: ");
 
        if (fgets(choiceInput, sizeof(choiceInput), stdin) == NULL)
        {
            printf("Input error. Try again.\n");
            continue;
        }

        if (sscanf(choiceInput, "%d", &choice) != 1)
        {
            printf("Invalid choice. Try again.\n");
            continue;
        }

        // Update full name
        if (choice == 1)
        {
            char newFullName[NAME_LENGTH];
            int i;

            printf("Current full name: %s\n", customers[index].fullName);

            while (1) // Loop until a valid full name is entered or the user chooses to keep the current name
            {
                printf("Enter new full name (press Enter to keep current): ");

           
                if (fgets(newFullName, sizeof(newFullName), stdin) == NULL)
                {
                    printf("Input error. Try again.\n");
                    continue;
                }

               
                newFullName[strcspn(newFullName, "\n")] = '\0';

            
                if (strlen(newFullName) == 0)
                {
                    break;
                }

 
                for (i = 0; newFullName[i] != '\0'; i++)
                {
                    if (!isspace((unsigned char)newFullName[i]))
                    {
                        break;
                    }
                }

                if (newFullName[i] == '\0')
                {
                    printf("Full name cannot contain only spaces. Please try again.\n");
                }
                else
                {
                    strcpy(customers[index].fullName, newFullName);
                    printf("Full name updated.\n");
                    break;
                }
            }
        }

                // Update car plate
        else if (choice == 2)
        {
            char newCarPlate[PLATE_LENGTH];
            int i, j, duplicate;

            printf("Current car plate: %s\n", customers[index].carPlate);

            while (1)
            {
                printf("Enter new car plate (press Enter to keep current): ");

                if (fgets(newCarPlate, sizeof(newCarPlate), stdin) == NULL)
                {
                    printf("Input error. Try again.\n");
                    continue;
                }

                newCarPlate[strcspn(newCarPlate, "\n")] = '\0';

                if (strlen(newCarPlate) == 0)
                {
                    break;
                }

                for (i = 0; newCarPlate[i] != '\0'; i++)
                {
                    if (!isspace((unsigned char)newCarPlate[i]))
                    {
                        break;
                    }
                }

                if (newCarPlate[i] == '\0')
                {
                    printf("Car plate cannot contain only spaces. Please try again.\n");
                    continue;
                }

                duplicate = 0;
                for (j = 0; j < *customerCount; j++)
                {
                    if (j != index && strcmp(customers[j].carPlate, newCarPlate) == 0)
                    {
                        duplicate = 1;
                        break;
                    }
                }

                if (duplicate)
                {
                    printf("Car plate already exists. Please try again.\n");
                }
                else
                {
                    strcpy(customers[index].carPlate, newCarPlate);
                    printf("Car plate updated.\n");
                    break;
                }
            }
        }

        // Update car type
        else if (choice == 3)
        {
            char newCarType[TYPE_LENGTH];
            int i;

            printf("Current car type: %s\n", customers[index].carType);

            while (1) // Loop until a valid car type is entered or the user chooses to keep the current type
            {
                printf("Enter new car type (press Enter to keep current): ");

                if (fgets(newCarType, sizeof(newCarType), stdin) == NULL)
                {
                    printf("Input error. Try again.\n");
                    continue;
                }

                newCarType[strcspn(newCarType, "\n")] = '\0';

                if (strlen(newCarType) == 0)
                {
                    break;
                }

                for (i = 0; newCarType[i] != '\0'; i++)
                {
                    if (!isspace((unsigned char)newCarType[i]))
                    {
                        break;
                    }
                }

                if (newCarType[i] == '\0')
                {
                    printf("Car type cannot contain only spaces. Please try again.\n");
                }
                else
                {
                    strcpy(customers[index].carType, newCarType);
                    printf("Car type updated.\n");
                    break;
                }
            }
        }
        // Finish update
        else if (choice == 4)
        {
            break;
        }
        else
        {
            printf("Invalid choice. Try again.\n");
        }
    }
    
    printf("Customer updated successfully.\n");
    displayCustomer(customers[index]);
    printf("------------------------------\n");
}