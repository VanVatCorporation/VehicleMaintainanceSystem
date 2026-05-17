#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include "customer.h"
#include "repair.h"
#include "fileio.h"
#include "utils.h"
#include "service.h"
#include "report.h"
#include <time.h>

#define APP_HEADER_WIDTH 68

// Global variables for signal handler access
Customer customers[MAX_CUSTOMERS];
int customerCount = 0;

RepairOrder orders[MAX_REPAIR_ORDERS];
int orderCount = 0;

Service services[MAX_SERVICES];
int serviceCount = 0;

static void saveAllData(void)
{
    saveCustomers(customers, customerCount);
    saveRepairOrders(orders, orderCount);
    saveServices(services, serviceCount);
}

static void rebuildCustomerOrderCounts(void)
{
    int i;
    int index;

    for (i = 0; i < customerCount; i++)
    {
        customers[i].orderCount = 0;
    }

    for (i = 0; i < orderCount; i++)
    {
        index = findCustomerIndexByPhone(customers, customerCount, orders[i].customerPhone);
        if (index != -1)
        {
            customers[index].orderCount++;
        }
    }
}

static void printAppHeader(void)
{
    printBoxTitle("MOTORBIKE REPAIR SHOP MANAGEMENT", APP_HEADER_WIDTH);
    printf("  Customers loaded     : %d\n", customerCount);
    printf("  Repair orders loaded : %d\n", orderCount);
    printf("  Services loaded      : %d\n", serviceCount);
    printInfo("Data is saved automatically after each change.");
}

static void printMainMenu(void)
{
    printSectionTitle("MAIN MENU");
    printMenuOption(1, "Customer management");
    printMenuOption(2, "Repair order management");
    printMenuOption(3, "Service Management");
    printMenuOption(4, "Save and exit");
    printf("\nSelect option: ");
}

static void printCustomerMenu(void)
{
    printSectionTitle("CUSTOMER MANAGEMENT");
    printMenuOption(1, "Add customer");
    printMenuOption(2, "Update customer");
    printMenuOption(3, "Search by phone");
    printMenuOption(4, "Search by car plate");
    printMenuOption(5, "List all customers");
    printMenuBack();
    printf("\nSelect option: ");
}

static void printRepairMenu(void)
{
    printSectionTitle("REPAIR ORDER MANAGEMENT");
    printMenuOption(1, "Create repair order");
    printMenuOption(2, "View or update order status");
    printMenuOption(3, "List all repair orders");
    printMenuOption(4, "Filter repair orders by status");
    printMenuOption(5, "View repair order history");
    printMenuOption(6, "Search order by ID or car plate");
    printMenuBack();
    printf("\nSelect option: ");
}

static void printServiceManagement(void)
{
    printSectionTitle("SERVICE MANAGEMENT");
    printMenuOption(1, "Add service");
    printMenuOption(2, "Update service");
    printMenuOption(3, "List of service");
    printMenuOption(4, "Popular services");
    printMenuOption(5, "Daily revenue");
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
                saveAllData();
                break;
            case 2:
                updateCustomer(customers, &customerCount);
                rebuildCustomerOrderCounts();
                saveAllData();
                break;
            case 3:
                searchCustomerByPhone(customers, customerCount);
                break;
            case 4:
                searchCustomerByPlate(customers, customerCount);
                break;
            case 5:
                listAllCustomers(customers, customerCount);
                break;
            case 0:
                return;
            default:
                printError("Please choose an option from the menu.");
        }
    }
}

static void printRepairOrderWithCustomer(RepairOrder order)
{
    int customerIndex = findCustomerIndexByPhone(customers, customerCount, order.customerPhone);

    if (customerIndex != -1)
    {
        displayCustomer(customers[customerIndex]);
    }
    else
    {
        printInfo("Customer profile is not available for this order.");
    }

    printRepairOrder(order);
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

    printRepairOrderWithCustomer(orders[found]);
    Status previousStatus = orders[found].status;
    orders[found] = updateStatus(orders[found]);

    if (orders[found].status == previousStatus)
    {
        printInfo("Order status unchanged.");
    }
    else
    {
        printSuccess("Order status updated.");
        saveAllData();
    }

    // Mission 14: Export invoice if complete
    if (previousStatus != COMPLETE && orders[found].status == COMPLETE)
    {
        int cIndex = findCustomerIndexByPhone(customers, customerCount, orders[found].customerPhone);

        if (cIndex != -1)
        {
            exportInvoice(orders[found], customers[cIndex]);
        }
    }
}

static void searchRepairOrder(void)
{
    int choice;
    char input[100];
    int found = 0;

    printSectionTitle("SEARCH REPAIR ORDER");
    printMenuOption(1, "By order ID");
    printMenuOption(2, "By car plate");
    printf("\nSelect option: ");

    if (!readInt(&choice))
    {
        printError("Invalid choice.");
        return;
    }

    if (choice == 1)
    {
        printf("Enter order ID: ");
        if (!readLine(input, sizeof(input)))
        {
            printError("Input error.");
            return;
        }

        for (int i = 0; i < orderCount; i++)
        {
            if (strcmp(orders[i].orderId, input) == 0)
            {
                printRepairOrderWithCustomer(orders[i]);
                found = 1;
                break;
            }
        }
    }
    else if (choice == 2)
    {
        int customerIndex;

        printf("Enter car plate: ");
        if (!readLine(input, sizeof(input)))
        {
            printError("Input error.");
            return;
        }

        normalizeCarPlate(input);
        if (!isValidCarPlate(input))
        {
            printError("Invalid car plate.");
            return;
        }

        customerIndex = findCustomerIndexByPlate(customers, customerCount, input);
        if (customerIndex == -1)
        {
            printError("No customer found for this car plate.");
            return;
        }

        for (int i = 0; i < orderCount; i++)
        {
            if (strcmp(orders[i].customerPhone, customers[customerIndex].phoneNumber) == 0)
            {
                printRepairOrderWithCustomer(orders[i]);
                found = 1;
            }
        }
    }
    else
    {
        printError("Invalid choice.");
        return;
    }

    if (!found)
    {
        printError("Repair order not found.");
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
                if (customerCount <= 0)
                {
                    printError("Please add at least one customer before creating a repair order.");
                    break;
                }
                if (serviceCount <= 0)
                {
                    printError("Please add at least one service before creating a repair order.");
                    break;
                }

                orders[orderCount] = createRepairOrder(orderCount, customers, customerCount, services, serviceCount);
                if (orders[orderCount].itemCount <= 0)
                {
                    printError("Repair order was not created because it has no services.");
                    break;
                }
                {
                    int customerIndex = findCustomerIndexByPhone(customers, customerCount, orders[orderCount].customerPhone);

                    if (customerIndex != -1)
                    {
                        customers[customerIndex].orderCount++;
                    }
                }
                orderCount++;
                printSuccess("Repair order created.");
                saveAllData();
                break;
            case 2:
                viewOrUpdateOrderStatus();
                break;
            case 3:
                listRepairOrders(orders, orderCount);
                break;
            case 4:
                filterRepairOrdersByStatus(orders, orderCount);
                break;
            case 5:
                viewRepairOrderHistory(orders, orderCount, customers, customerCount);
                break;
            case 6:
                searchRepairOrder();
                break;
            case 0:
                return;
            default:
                printError("Please choose an option from the menu.");
        }
    }
}

static void openServiceManagement(void)
{
    int subChoice;

    while(1)
    {
        printServiceManagement();

        if (!readInt(&subChoice))
        {
            printError("Please enter a valid number.");
            continue;
        }
        switch(subChoice)
        {
            case 1:
                addService(services, &serviceCount);
                saveAllData();
                break;
            case 2:
                updateService(services, serviceCount);
                saveAllData();
                break;
            case 3:
                listServices(services, serviceCount);
                break;
            case 4:
                getPopularServices(orders, orderCount);
                break;
            case 5:
                displayDailyRevenue(orders, orderCount);
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

    saveAllData();
    printSuccess("Data saved. Goodbye!");
    exit(0);
}

int main()
{
    configureConsoleEncoding();

    // Register signals
    signal(SIGINT, handleExit);  // Ctrl+C
    signal(SIGTERM, handleExit); // Termination request
#ifdef SIGHUP
    signal(SIGHUP, handleExit);  // Terminal closed
#endif

    // Mission 11: Read data on startup
    loadCustomers(customers, &customerCount);
    loadRepairOrders(orders, &orderCount);
    loadServices(services, &serviceCount);
    rebuildCustomerOrderCounts();

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
                openServiceManagement();
                break;
            case 4:
                handleExit(0); // Normal exit
                break;
            default:
                printError("Please choose an option from the menu.");
        }
    }

    return 0;
}
