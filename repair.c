#include <stdio.h>
#include <string.h>
#include <time.h>
#include <ctype.h>
#include "repair.h"
#include "customer.h"
#include "utils.h"

#define INPUT_LENGTH 100
#define ORDER_NO_WIDTH 3
#define ORDER_SERVICE_WIDTH 38
#define ORDER_PRICE_WIDTH 16
#define ORDER_TABLE_WIDTH (ORDER_NO_WIDTH + ORDER_SERVICE_WIDTH + ORDER_PRICE_WIDTH + 10)
#define LIST_NO_WIDTH 4
#define LIST_ID_WIDTH 10
#define LIST_PHONE_WIDTH 12
#define LIST_STATUS_WIDTH 16
#define LIST_TOTAL_WIDTH 16
#define COLOR_RESET "\033[0m"
#define COLOR_GREEN "\033[32m"
#define COLOR_YELLOW "\033[33m"
#define COLOR_GRAY "\033[90m"


// create order Id
void generateOrderId(char orderId[], int counter)
{
    int nextId = counter + 1;

    if (nextId < 0)
    {
        nextId = 0;
    }
    else if (nextId > 999999)
    {
        nextId = 999999;
    }

    snprintf(orderId, 10, "RO%06d", nextId);
}

// find service by Id
int findServiceIndexById(Service services[], int count, char serviceId[])
{
    int i;

    for (i = 0; i < count; i++)
    {
        if (strcmp(services[i].serviceId, serviceId) == 0)
        {
            return i;
        }
    }

    return -1;
}


static int inputServiceCount(void)
{
    int itemCount;

    while (1)
    {
        printf("Enter number of services: ");

        if (!readInt(&itemCount))
        {
            printError("Invalid number. Please try again.");
        }
        else if (itemCount < 1 || itemCount > MAX_ITEMS)
        {
            printf("[ERROR] Number of services must be from 1 to %d.\n", MAX_ITEMS);
        }
        else
        {
            return itemCount;
        }
    }
}


static int inputCustomerPhone(char phoneNumber[], Customer customers[], int customerCount)
{
    char input[INPUT_LENGTH];
    int index;

    while (1)
    {
        printf("Enter phone number (0 to cancel): ");

        if (!readLine(input, sizeof(input)))
        {
            printError("Input error. Please try again.");
            continue;
        }

        if (strcmp(input, "0") == 0)
        {
            return 0;
        }

        if (!isValidPhoneNumber(input))
        {
            printError("Invalid phone number. Please try again.");
        }
        else if ((index = findCustomerIndexByPhone(customers, customerCount, input)) == -1)
        {
            printError("Customer not found. Please add the customer before creating an order.");
        }
        else
        {
            strcpy(phoneNumber, input);
            printSuccess("Customer found.");
            displayCustomer(customers[index]);
            return 1;
        }
    }
}

static RepairOrder inputOrderServices(RepairOrder order, Service services[], int serviceCount)
{
    int i;
    char serviceId[10];
    int index;
    char priceText[32];

    printSectionTitle("SERVICE DETAILS");

    if (serviceCount <= 0)
    {
        printError("No services available.");
        order.itemCount = 0;
        return order;
    }

    order.itemCount = inputServiceCount();

    printBoxTitle("AVAILABLE SERVICES", 64);
    printf("+------------+---------------------------+------------------+\n");
    printf("| ");
    printPaddedText("Service ID", 10, 0);
    printf(" | ");
    printPaddedText("Service Name", 25, 0);
    printf(" | ");
    printPaddedText("Price", 16, 0);
    printf(" |\n");
    printf("+------------+---------------------------+------------------+\n");

    for (i = 0; i < serviceCount; i++)
    {
        snprintf(priceText, sizeof(priceText), "%d VND", services[i].price);
        printf("| ");
        printPaddedText(services[i].serviceId, 10, 0);
        printf(" | ");
        printPaddedText(services[i].name, 25, 0);
        printf(" | ");
        printPaddedText(priceText, 16, 1);
        printf(" |\n");
    }

    printf("+------------+---------------------------+------------------+\n");

    for (i = 0; i < order.itemCount; i++)
    {
        while (1)
        {
            printf("\nEnter Service ID for item %d: ", i + 1);

            if (!readLine(serviceId, sizeof(serviceId)))
            {
                printError("Input error.");
                continue;
            }

            index = findServiceIndexById(services, serviceCount, serviceId);

            if (index == -1)
            {
                printError("Service not found. Please try again.");
            }
            else
            {
                strcpy(order.items[i].name, services[index].name);
                order.items[i].price = services[index].price;
                break;
            }
        }
    }
    return order;
}

// create order
RepairOrder createRepairOrder(int counter, Customer customers[], int customerCount, Service services[], int serviceCount)
{
    RepairOrder order;

    memset(&order, 0, sizeof(order));

    printSectionTitle("CREATE REPAIR ORDER");
    generateOrderId(order.orderId, counter);
    printf("[INFO] New order ID: %s\n", order.orderId);

    if (!inputCustomerPhone(order.customerPhone, customers, customerCount))
    {
        printInfo("Repair order creation cancelled.");
        return order;
    }

    // problems
    printf("Enter vehicle problem: ");
    if (!readLine(order.symptom, sizeof(order.symptom)))
    {
        strcpy(order.symptom, "");
    }

    // service
    order = inputOrderServices(order, services, serviceCount);

    order.createdDate = time(NULL);
    order.status = RECEIVED; // default status

    return order;
}

// caculate money when print order
long long calculateTotal(RepairOrder order)
{
    long long total = 0;
    int i;

    for (i = 0; i < order.itemCount; i++)
    {
        total += order.items[i].price;
    }

    return total;
}

// update status
const char* getStatusText(Status status)
{
    switch (status)
    {
        case RECEIVED: return "Received";
        case UNDER_REPAIRED: return "Under repaired";
        case COMPLETE: return "Complete";
        default: return "Unknown";
    }
}

static const char* getStatusColor(Status status)
{
    switch (status)
    {
        case RECEIVED: return COLOR_GRAY;
        case UNDER_REPAIRED: return COLOR_YELLOW;
        case COMPLETE: return COLOR_GREEN;
        default: return COLOR_RESET;
    }
}

static void printStatusValue(Status status)
{
    printf("%s%s%s", getStatusColor(status), getStatusText(status), COLOR_RESET);
}

// update status when creating a order
RepairOrder updateStatus(RepairOrder order)
{
    int choice;
    Status nextStatus;

    while (1)
    {
        printSectionTitle("UPDATE ORDER STATUS");
        printf("  Current status: %s\n\n", getStatusText(order.status));
        printMenuOption(1, "Received");
        printMenuOption(2, "Under repaired");
        printMenuOption(3, "Complete");
        printf("\nSelect option: ");

        if (!readInt(&choice))
        {
            printError("Invalid choice. Please try again.");
            continue;
        }
        switch (choice)
        {
            case 1: nextStatus = RECEIVED; break;
            case 2: nextStatus = UNDER_REPAIRED; break;
            case 3: nextStatus = COMPLETE; break;
            default:
                printError("Invalid choice. Please try again.");
                continue;
        }

        if (nextStatus == order.status + 1)
        {
            order.status = nextStatus;
            return order;
        }
        else if (nextStatus == order.status)
        {
            printInfo("Status unchanged.");
            return order;
        }
        else
        {
            printError("Status must move forward one step at a time.");
        }
    }   
}



// view history of order
void viewRepairOrderHistory(RepairOrder orders[], int orderCount, Customer customers[], int customerCount)
{
    char phone[11];
    int i;
    int found = 0;
    int customerIndex;

    printSectionTitle("REPAIR ORDER HISTORY");
    
    printf("Enter phone number: ");
    if (!readLine(phone, sizeof(phone)))
    {
        printError("Input error.");
        return;
    }

    customerIndex = findCustomerIndexByPhone(customers, customerCount, phone);
    if (customerIndex != -1)
    {
        displayCustomer(customers[customerIndex]);
    }

    for (i = 0; i < orderCount; i++)
    {
        if (strcmp(orders[i].customerPhone, phone) == 0)
        {
            printf("\n===== ORDER %d =====\n", i + 1);
            printRepairOrder(orders[i]);
            found = 1;
        }
    }

    if (!found)
    {
        printError("No history found for this phone number.");
    }
}

static void printOrderListDivider(void)
{
    int i;

    printf("+");
    for (i = 0; i < LIST_NO_WIDTH + 2; i++) printf("-");
    printf("+");
    for (i = 0; i < LIST_ID_WIDTH + 2; i++) printf("-");
    printf("+");
    for (i = 0; i < LIST_PHONE_WIDTH + 2; i++) printf("-");
    printf("+");
    for (i = 0; i < LIST_STATUS_WIDTH + 2; i++) printf("-");
    printf("+");
    for (i = 0; i < LIST_TOTAL_WIDTH + 2; i++) printf("-");
    printf("+\n");
}

static void printOrderListHeader(void)
{
    printOrderListDivider();
    printf("| ");
    printPaddedText("No", LIST_NO_WIDTH, 1);
    printf(" | ");
    printPaddedText("Order ID", LIST_ID_WIDTH, 0);
    printf(" | ");
    printPaddedText("Phone", LIST_PHONE_WIDTH, 0);
    printf(" | ");
    printPaddedText("Status", LIST_STATUS_WIDTH, 0);
    printf(" | ");
    printPaddedText("Total", LIST_TOTAL_WIDTH, 1);
    printf(" |\n");
    printOrderListDivider();
}

static void printOrderListRow(int number, RepairOrder order)
{
    char numberText[16];
    char totalText[32];

    snprintf(numberText, sizeof(numberText), "%d", number);
    snprintf(totalText, sizeof(totalText), "%lld VND", calculateTotal(order));

    printf("| ");
    printPaddedText(numberText, LIST_NO_WIDTH, 1);
    printf(" | ");
    printPaddedText(order.orderId, LIST_ID_WIDTH, 0);
    printf(" | ");
    printPaddedText(order.customerPhone, LIST_PHONE_WIDTH, 0);
    printf(" | ");
    printPaddedText(getStatusText(order.status), LIST_STATUS_WIDTH, 0);
    printf(" | ");
    printPaddedText(totalText, LIST_TOTAL_WIDTH, 1);
    printf(" |\n");
}

static char readPagingCommand(void)
{
    char input[INPUT_LENGTH];

    printf("\n[n] Next | [p] Prev | [q] Quit: ");

    if (!readLine(input, sizeof(input)) || input[0] == '\0')
    {
        return 'q';
    }

    return (char)tolower((unsigned char)input[0]);
}

// filter order by status
void filterRepairOrdersByStatus(RepairOrder orders[], int orderCount)
{
    int choice;
    int i;
    int totalItems = 0;
    int totalPages;
    int page = 0;
    char cmd;
    Status filterStatus;

    printSectionTitle("FILTER BY STATUS");
    printMenuOption(1, "Received");
    printMenuOption(2, "Under repaired");
    printMenuOption(3, "Complete");
    printf("\nChoice: ");

    if (!readInt(&choice))
    {
        printError("Invalid choice.");
        return;
    }

    switch (choice)
    {
        case 1: filterStatus = RECEIVED; break;
        case 2: filterStatus = UNDER_REPAIRED; break;
        case 3: filterStatus = COMPLETE; break;
        default:
            printError("Unknown status.");
            return;
    }

    for (i = 0; i < orderCount; i++)
    {
        if (orders[i].status == filterStatus)
        {
            totalItems++;
        }
    }

    if (totalItems == 0)
    {
        printError("No data.");
        return;
    }

    totalPages = (totalItems + 9) / 10;

    do
    {
        int count = 0;
        int shown = 0;

        printf("\n===== FILTER RESULTS (Page %d/%d) =====\n",
               page + 1,
               totalPages);
        printOrderListHeader();

        for (i = 0; i < orderCount; i++)
        {
            if (orders[i].status == filterStatus)
            {
                if (count >= page * 10 && shown < 10)
                {
                    printOrderListRow(i + 1, orders[i]);
                    shown++;
                }

                count++;
            }
        }

        printOrderListDivider();
        cmd = readPagingCommand();

        if (cmd == 'n' && page < totalPages - 1)
        {
            page++;
        }
        else if (cmd == 'p' && page > 0)
        {
            page--;
        }
        else if (cmd == 'n' || cmd == 'p')
        {
            printInfo("No more pages in that direction.");
        }
        else if (cmd != 'q')
        {
            printError("Invalid paging command.");
        }

    } while (cmd != 'q');
}

// list all orders
void listRepairOrders(RepairOrder orders[], int orderCount)
{
    int page = 0;
    int totalPages;
    char cmd;
    int i;

    if (orderCount == 0)
    {
        printError("No orders available.");
        return;
    }

    totalPages = (orderCount + 9) / 10;

    do
    {
        int start = page * 10;
        int end = start + 10;

        if (end > orderCount)
        {
            end = orderCount;
        }

        printf("\n===== LIST OF ORDERS (Page %d/%d) =====\n",
               page + 1,
               totalPages);
        printOrderListHeader();

        for (i = start; i < end; i++)
        {
            printOrderListRow(i + 1, orders[i]);
        }

        printOrderListDivider();
        cmd = readPagingCommand();

        if (cmd == 'n' && page < totalPages - 1)
        {
            page++;
        }
        else if (cmd == 'p' && page > 0)
        {
            page--;
        }
        else if (cmd == 'n' || cmd == 'p')
        {
            printInfo("No more pages in that direction.");
        }
        else if (cmd != 'q')
        {
            printError("Invalid paging command.");
        }

    } while (cmd != 'q');
}

void printOrderDivider(void)
{
    int i;

    printf("+");
    for (i = 0; i < ORDER_NO_WIDTH + 2; i++)
    {
        printf("-");
    }
    printf("+");
    for (i = 0; i < ORDER_SERVICE_WIDTH + 2; i++)
    {
        printf("-");
    }
    printf("+");
    for (i = 0; i < ORDER_PRICE_WIDTH + 2; i++)
    {
        printf("-");
    }
    printf("+\n");
}

void printOrderSummaryRow(const char label[], const char value[])
{
    printf("  %-12s : ", label);
    printText(value);
    printf("\n");
}

static void formatCurrency(char output[], size_t size, long long amount)
{
    snprintf(output, size, "%lld VND", amount);
}

// print repair order
void printRepairOrder(RepairOrder order)
{
    int i;
    char createdText[32];
    char numberText[16];
    char priceText[32];

    if (strftime(createdText, sizeof(createdText), "%Y-%m-%d %H:%M", localtime(&order.createdDate)) == 0)
    {
        strcpy(createdText, "Unknown");
    }

    printBoxTitle("REPAIR ORDER DETAILS", ORDER_TABLE_WIDTH);
    printOrderSummaryRow("Order ID", order.orderId);
    printOrderSummaryRow("Phone", order.customerPhone);
    printOrderSummaryRow("Created", createdText);
    printf("  %-12s : ", "Status");
    printStatusValue(order.status);
    printf("\n");
    printOrderSummaryRow("Problem", order.symptom);

    printf("\n");
    printOrderDivider();
    printf("| ");
    printPaddedText("No", ORDER_NO_WIDTH, 0);
    printf(" | ");
    printPaddedText("Service", ORDER_SERVICE_WIDTH, 0);
    printf(" | ");
    printPaddedText("Price", ORDER_PRICE_WIDTH, 0);
    printf(" |\n");
    printOrderDivider();

    if (order.itemCount == 0)
    {
        printf("| ");
        printPaddedText("", ORDER_NO_WIDTH, 0);
        printf(" | ");
        printPaddedText("No services selected", ORDER_SERVICE_WIDTH, 0);
        printf(" | ");
        printPaddedText("0 VND", ORDER_PRICE_WIDTH, 1);
        printf(" |\n");
    }

    for (i = 0; i < order.itemCount; i++)
    {
        snprintf(numberText, sizeof(numberText), "%d", i + 1);
        formatCurrency(priceText, sizeof(priceText), order.items[i].price);
        printf("| ");
        printPaddedText(numberText, ORDER_NO_WIDTH, 1);
        printf(" | ");
        printPaddedText(order.items[i].name, ORDER_SERVICE_WIDTH, 0);
        printf(" | ");
        printPaddedText(priceText, ORDER_PRICE_WIDTH, 1);
        printf(" |\n");
    }

    printOrderDivider();
    formatCurrency(priceText, sizeof(priceText), calculateTotal(order));
    printf("| ");
    printPaddedText("", ORDER_NO_WIDTH, 0);
    printf(" | ");
    printPaddedText("Total", ORDER_SERVICE_WIDTH, 0);
    printf(" | ");
    printPaddedText(priceText, ORDER_PRICE_WIDTH, 1);
    printf(" |\n");
    printOrderDivider();
}
