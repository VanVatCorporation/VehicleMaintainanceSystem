#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "customer.h"

// Display customer information
void displayCustomer(Customer customer) { 
    printf("\n===== Customer Information =====\n");
    printf("Customer ID : %s\n", customer.customerId);
    printf("Full Name   : %s\n", customer.fullName);
    printf("Phone Number: %s\n", customer.phoneNumber);
    printf("Car Plate   : %s\n", customer.carPlate);
    printf("Car Type    : %s\n", customer.carType);
    printf("Order Count : %d\n", customer.orderCount);
}

// Find customer index by phone number
int findCustomerIndexByPhone(Customer customers[], int customerCount, char phoneNumber[]) { 
    for (int i = 0; i < customerCount; i++) {
        if (strcmp(customers[i].phoneNumber, phoneNumber) == 0) {
            return i;
        }
    }

    return -1;
}

// Find customer index by car plate
int findCustomerIndexByPlate(Customer customers[], int customerCount, char carPlate[]) { 
    for (int i = 0; i < customerCount; i++) {
        if (strcmp(customers[i].carPlate, carPlate) == 0) {
            return i;
        }
    }

    return -1;
}

// Validate phone number format (simple validation)
int isValidPhoneNumber(char phoneNumber[]) {
    
    size_t length = strlen(phoneNumber);
    if (length != 10) {
        return 0; // Phone number must be exactly 10 digits 
    }

    if (phoneNumber[0] != '0') {  // Phone number must start with '0'
        return 0; // 
    }

    for (int i = 0; i < length; i++) {
        if (!isdigit((unsigned char)(phoneNumber[i]))) {
            return 0; 
        }
    }

    return 1; // Valid phone number
}

// Check for duplicate phone numbers
int isDuplicatePhoneNumber(Customer customers[], int customerCount, char phoneNumber[]) {
    
    for (int i = 0; i < customerCount; i++) {
        if (strcmp(customers[i].phoneNumber, phoneNumber) == 0) {
            return 1; // Return 1 if phone number already exists, otherwise 0
        }
    }

    return 0; 
}


// Generate a unique customer ID based on the current customer count
void generateCustomerId(char customerId[], int customerCount) {
    snprintf(customerId, ID_LENGTH, "C%06d", customerCount + 1); // Generate ID in format C0001, C0002, etc.
}

// Find customer index by phone number
void searchCustomerByPhone(Customer customers[], int customerCount) {
    char phoneNumber[PHONE_LENGTH];

    printf("Enter phone number: ");
    scanf("%s", phoneNumber);

    int index = findCustomerIndexByPhone(customers, customerCount, phoneNumber);

    if (index != -1) {
        printf("Customer not found:\n");
    } else {
        displayCustomer(customers[index]);
    }
}