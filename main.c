#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include "customer.h"
#include "repair.h"
#include "fileio.h"

// Global variables for signal handler access
Customer customers[MAX_CUSTOMERS];
int customerCount = 0;

RepairOrder orders[1000]; 
int orderCount = 0;

// Signal handler to save data on abrupt closure
void handleExit(int sig) {
    printf("\nDetected signal %d. Saving data and exiting...\n", sig);
    saveCustomers(customers, customerCount);
    saveRepairOrders(orders, orderCount);
    exit(0);
}

int main() {
    // Register signals
    signal(SIGINT, handleExit);  // Ctrl+C
    signal(SIGTERM, handleExit); // Termination request
    signal(SIGHUP, handleExit);  // Terminal closed

    // Mission 11: Read data on startup
    loadCustomers(customers, &customerCount);
    loadRepairOrders(orders, &orderCount);

    printf("Welcome to Motorbike Repair Shop Management System\n");
    printf("Loaded %d customers and %d repair orders.\n", customerCount, orderCount);

    int choice;
    while (1) {
        printf("\n=== MAIN MENU ===\n");
        printf("1. Customer Management\n");
        printf("2. Repair Order Management\n");
        printf("3. Exit\n");
        printf("Choose: ");
        if (scanf("%d", &choice) != 1) {
            // Handle non-numeric input to prevent infinite loop
            while (getchar() != '\n');
            continue;
        }
        getchar();

        if (choice == 1) {
            printf("\n--- CUSTOMER MANAGEMENT ---\n");
            printf("1. Add Customer\n");
            printf("2. Update Customer\n");
            printf("3. Search by Phone\n");
            printf("4. Search by Plate\n");
            printf("Back: 0\n");
            int subChoice;
            if (scanf("%d", &subChoice) != 1) {
                while (getchar() != '\n');
                continue;
            }
            getchar();

            switch (subChoice) {
                case 1: addCustomer(customers, &customerCount); break;
                case 2: updateCustomer(customers, &customerCount); break;
                case 3: searchCustomerByPhone(customers, customerCount); break;
                case 4: searchCustomerByPlate(customers, customerCount); break;
                default: break;
            }
        } else if (choice == 2) {
            printf("\n--- REPAIR ORDER MANAGEMENT ---\n");
            printf("1. Create Repair Order\n");
            printf("2. View/Update Order Status\n");
            printf("Back: 0\n");
            int subChoice;
            if (scanf("%d", &subChoice) != 1) {
                while (getchar() != '\n');
                continue;
            }
            getchar();

            if (subChoice == 1) {
                RepairOrder newOrder = createRepairOrder(orderCount + 1, customers, customerCount);
                orders[orderCount++] = newOrder;
            } else if (subChoice == 2) {
                char orderId[10];
                printf("Enter Order ID: ");
                scanf("%9s", orderId);
                int found = -1;
                for (int i = 0; i < orderCount; i++) {
                    if (strcmp(orders[i].orderId, orderId) == 0) {
                        found = i;
                        break;
                    }
                }
                if (found != -1) {
                    printRepairOrder(orders[found]);
                    orders[found] = updateStatus(orders[found]);
                    
                    // Mission 14: Export invoice if complete
                    if (orders[found].status == COMPLETE) {
                        int cIndex = findCustomerIndexByPhone(customers, customerCount, orders[found].customerPhone);
                        if (cIndex != -1) {
                            exportInvoice(orders[found], customers[cIndex]);
                        }
                    }
                } else {
                    printf("Order not found.\n");
                }
            }
        } else if (choice == 3) {
            handleExit(0); // Normal exit
        }
    }

    return 0;
}