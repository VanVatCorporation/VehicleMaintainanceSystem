#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include "customer.h"
#include "repair.h"
#include "fileio.h"
#include "utils.h"

#define MAX_REPAIR_ORDERS 1000
#define APP_HEADER_WIDTH 68

// Global variables for signal handler access
Customer customers[MAX_CUSTOMERS];
int customerCount = 0;

RepairOrder orders[MAX_REPAIR_ORDERS];
int orderCount = 0;

static void printAppHeader(void)
{
    printBoxTitle("MOTORBIKE REPAIR SHOP MANAGEMENT", APP_HEADER_WIDTH);
    printf("  Customers loaded     : %d\n", customerCount);
    printf("  Repair orders loaded : %d\n", orderCount);
    printInfo("Data will be saved automatically when you exit.");
}

static void printMainMenu(void)
{
    printSectionTitle("MAIN MENU");
    printMenuOption(1, "Customer management");
    printMenuOption(2, "Repair order management");
    printMenuOption(3, "Save and exit");
    printf("\nSelect option: ");
}

static void printCustomerMenu(void)
{
    printSectionTitle("CUSTOMER MANAGEMENT");
    printMenuOption(1, "Add customer");
    printMenuOption(2, "Update customer");
    printMenuOption(3, "Search by phone");
    printMenuOption(4, "Search by car plate");
    printMenuBack();
    printf("\nSelect option: ");
}

static void printRepairMenu(void)
{
    printSectionTitle("REPAIR ORDER MANAGEMENT");
    printMenuOption(1, "Create repair order");
    printMenuOption(2, "View or update order status");
    printMenuBack();
    printf("\nSelect option: ");
}

static void openCustomerMenu(void)
{
    int subChoice;

    while (1)
    {
        printCustomerMenu();

        if (!readInt(&subChoice))
        {
            printError("Please enter a valid number.");
            continue;
        }

        switch (subChoice)
        {
            case 1:
                addCustomer(customers, &customerCount);
                break;
            case 2:
                updateCustomer(customers, &customerCount);
                break;
            case 3:
                searchCustomerByPhone(customers, customerCount);
                break;
            case 4:
                searchCustomerByPlate(customers, customerCount);
                break;
            case 0:
                return;
            default:
                printError("Please choose an option from the menu.");
        }
    }
}

static void viewOrUpdateOrderStatus(void)
{
    char orderId[10];
    int found = -1;
    int i;

    printf("Enter order ID: ");
    if (!readLine(orderId, sizeof(orderId)))
    {
        printError("Input error. Please try again.");
        return;
    }

    for (i = 0; i < orderCount; i++)
    {
        if (strcmp(orders[i].orderId, orderId) == 0)
        {
            found = i;
            break;
        }
    }

    if (found == -1)
    {
        printError("Order not found.");
        return;
    }

    printRepairOrder(orders[found]);
    orders[found] = updateStatus(orders[found]);
    printSuccess("Order status updated.");

    // Mission 14: Export invoice if complete
    if (orders[found].status == COMPLETE)
    {
        int cIndex = findCustomerIndexByPhone(customers, customerCount, orders[found].customerPhone);

        if (cIndex != -1)
        {
            exportInvoice(orders[found], customers[cIndex]);
        }
    }
}

static void openRepairMenu(void)
{
    int subChoice;

    while (1)
    {
        printRepairMenu();

        if (!readInt(&subChoice))
        {
            printError("Please enter a valid number.");
            continue;
        }

        switch (subChoice)
        {
            case 1:
                if (orderCount >= MAX_REPAIR_ORDERS)
                {
                    printError("Repair order list is full.");
                    break;
                }

                orders[orderCount] = createRepairOrder(orderCount + 1, customers, customerCount);
                orderCount++;
                printSuccess("Repair order created.");
                break;
            case 2:
                viewOrUpdateOrderStatus();
                break;
            case 0:
                return;
            default:
                printError("Please choose an option from the menu.");
        }
    }
}

// Signal handler to save data on abrupt closure
void handleExit(int sig)
{
    printf("\n");
    if (sig == 0)
    {
        printInfo("Saving data before exit...");
    }
    else
    {
        printf("[INFO] Detected signal %d. Saving data before exit...\n", sig);
    }

    saveCustomers(customers, customerCount);
    saveRepairOrders(orders, orderCount);
    printSuccess("Data saved. Goodbye!");
    exit(0);
}

int main()
{
    // Register signals
    signal(SIGINT, handleExit);  // Ctrl+C
    signal(SIGTERM, handleExit); // Termination request
#ifdef SIGHUP
    signal(SIGHUP, handleExit);  // Terminal closed
#endif

    // Mission 11: Read data on startup
    loadCustomers(customers, &customerCount);
    loadRepairOrders(orders, &orderCount);

    printAppHeader();

    int choice;
    while (1)
    {
        printMainMenu();

        if (!readInt(&choice))
        {
            printError("Please enter a valid number.");
            continue;
        }

        switch (choice)
        {
            case 1:
                openCustomerMenu();
                break;
            case 2:
                openRepairMenu();
                break;
            case 3:
                handleExit(0); // Normal exit
                break;
            default:
                printError("Please choose an option from the menu.");
        }
    }

    return 0;
}
