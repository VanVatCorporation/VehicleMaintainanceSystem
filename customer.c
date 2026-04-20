#include <stdio.h>
#include "customer.h"

void displayCustomer(Customer customer) { // Display customer information
    printf("\n===== Customer Information =====\n");
    printf("Customer ID : %s\n", customer.customerId);
    printf("Full Name   : %s\n", customer.fullName);
    printf("Phone Number: %s\n", customer.phoneNumber);
    printf("Car Plate   : %s\n", customer.carPlate);
    printf("Car Type    : %s\n", customer.carType);
    printf("Order Count : %d\n", customer.orderCount);
}