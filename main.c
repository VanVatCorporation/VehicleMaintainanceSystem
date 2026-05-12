#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include "customer.h"
#include "repair.h"
#include "fileio.h"
#include "report.h"
#include "utils.h"

#define MAX_REPAIR_ORDERS 50000
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
    printMenuOption(3, "Reports");
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
    printMenuOption(3, "Add service");
    printMenuOption(4, "Update service");
    printMenuOption(5, "List all repair orders");
    printMenuOption(6, "Filter repair orders by status");
    printMenuOption(7, "View repair order history");
    printMenuOption(8, "Search order by ID or car plate");
    printMenuOption(9, "List services");
    printMenuBack();
    printf("\nSelect option: ");
}

static void printReportMenu(void)
{
    printSectionTitle("REPORTS");
    printMenuOption(1, "Daily revenue");
    printMenuOption(2, "Popular services");
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
                addService(services, &serviceCount);
                saveAllData();
                break;
            case 4:
                updateService(services, serviceCount);
                saveAllData();
                break;
            case 5:
                listRepairOrders(orders, orderCount);
                break;
            case 6:
                filterRepairOrdersByStatus(orders, orderCount);
                break;
            case 7:
                viewRepairOrderHistory(orders, orderCount, customers, customerCount);
                break;
            case 8:
                searchRepairOrder();
                break;
            case 9:
                listServices(services, serviceCount);
                break;
            case 0:
                return;
            default:
                printError("Please choose an option from the menu.");
        }
    }
}

static int inputReportDate(time_t *date)
{
    char input[32];
    int day;
    int month;
    int year;
    time_t now;
    struct tm dateInfo;

    printf("Enter date (dd/mm/yyyy, press Enter for today): ");
    if (!readLine(input, sizeof(input)))
    {
        printError("Input error.");
        return 0;
    }

    now = time(NULL);
    dateInfo = *localtime(&now);

    if (input[0] != '\0')
    {
        if (sscanf(input, "%d/%d/%d", &day, &month, &year) != 3 ||
            day < 1 || day > 31 ||
            month < 1 || month > 12 ||
            year < 1900)
        {
            printError("Invalid date format.");
            return 0;
        }

        dateInfo.tm_mday = day;
        dateInfo.tm_mon = month - 1;
        dateInfo.tm_year = year - 1900;
        dateInfo.tm_hour = 0;
        dateInfo.tm_min = 0;
        dateInfo.tm_sec = 0;
        dateInfo.tm_isdst = -1;
    }

    *date = mktime(&dateInfo);
    return *date != (time_t)-1;
}

static void openReportMenu(void)
{
    int choice;
    time_t date;
    long long revenue;

    while (1)
    {
        printReportMenu();

        if (!readInt(&choice))
        {
            printError("Please enter a valid number.");
            continue;
        }

        switch (choice)
        {
            case 1:
                if (inputReportDate(&date))
                {
                    revenue = calculateDailyRevenue(date, orders, orderCount);
                    printf("Daily revenue: %lld VND\n", revenue);
                }
                break;
            case 2:
                getPopularServices(orders, orderCount);
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
                openReportMenu();
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
