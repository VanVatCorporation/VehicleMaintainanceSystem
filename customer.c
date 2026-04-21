#include <stdio.h>
#include <string.h>
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