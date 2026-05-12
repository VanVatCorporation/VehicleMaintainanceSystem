#include <stdio.h>
#include <string.h>
#include <time.h>
#include "repair.h"
#include "customer.h"
#include "utils.h"

#define INPUT_LENGTH 100
#define ORDER_NO_WIDTH 3
#define ORDER_SERVICE_WIDTH 38
#define ORDER_PRICE_WIDTH 16
#define ORDER_TABLE_WIDTH (ORDER_NO_WIDTH + ORDER_SERVICE_WIDTH + ORDER_PRICE_WIDTH + 10)

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

// create service Id 
void generateServiceId(char serviceId[], int counter)
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

    snprintf(serviceId, 10, "SV%06d", nextId);
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


static int inputPrice(void)
{
    int price;

    while (1)
    {
        printf("Price: ");

        if (!readInt(&price))
        {
            printError("Invalid price. Please try again.");
        }
        else if (price < 0)
        {
            printError("Price cannot be negative. Please try again.");
        }
        else
        {
            return price;
        }
    }
}

static void inputCustomerPhone(char phoneNumber[])
{
    char input[INPUT_LENGTH];

    while (1)
    {
        printf("Enter phone number: ");

        if (!readLine(input, sizeof(input)))
        {
            printError("Input error. Please try again.");
            continue;
        }

        if (!isValidPhoneNumber(input))
        {
            printError("Invalid phone number. Please try again.");
        }
        else
        {
            strcpy(phoneNumber, input);
            return;
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
    order.itemCount = inputServiceCount();

    if (serviceCount <= 0)
    {
        printError("No services available.");
        order.itemCount = 0;
        return order;
    }

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
    int index;

    printSectionTitle("CREATE REPAIR ORDER");
    generateOrderId(order.orderId, counter);
    printf("[INFO] New order ID: %s\n", order.orderId);

    // enter phone number
    inputCustomerPhone(order.customerPhone);

    // find customer by phone number
    index = findCustomerIndexByPhone(customers, customerCount, order.customerPhone);

    if (index == -1)
    {
        printError("Customer not found. The order will still be created with this phone number.");
    }
    else
    {
        printSuccess("Customer found.");
        displayCustomer(customers[index]);
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

        if (nextStatus > order.status)
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
            printError("Status is not reversible. Please try again.");
        }
    }
}

// add service to the list
void addService(Service services[], int *serviceCount)
{
    Service service;

    if (*serviceCount >= MAX_SERVICES)
    {
        printError("Service list is full.");
        return;
    }

    generateServiceId(service.serviceId, *serviceCount);
    printf("[INFO] New service ID: %s\n", service.serviceId);

    printf("Enter service name: ");
    if (!readLine(service.name, sizeof(service.name)))
    {
        strcpy(service.name, "");
    }

    service.price = inputPrice();

    services[*serviceCount] = service;
    (*serviceCount)++;

    printSuccess("Service added successfully.");
}

// update service in the list
void updateService(Service services[], int serviceCount)
{
    char id[10];
    int index;
    char priceText[20];
    
    printSectionTitle("UPDATE SERVICE");
    printf("Enter service ID to update: ");
    if (!readLine(id, sizeof(id)))
    {
        printError("Input error.");
        return;
    }

    index = findServiceIndexById(services, serviceCount, id);

    if (index == -1)
    {
        printError("Service not found.");
        return;
    }

    printOrderSummaryRow("Service ID", services[index].serviceId);
    printOrderSummaryRow("Name", services[index].name);
    snprintf(priceText,sizeof(priceText), "%d VND", services[index].price);
    printOrderSummaryRow("Price", priceText);

    printf("Enter new service name: ");
    if (!readLine(services[index].name, sizeof(services[index].name)))
    {
        strcpy(services[index].name, "");
    }

    services[index].price = inputPrice();

    printSuccess("Service updated successfully.");
}

// view history of order
void viewRepairOrderHistory(RepairOrder orders[], int orderCount, Customer customers[], int customerCount)
{
    char phone[11];
    int i;
    int found = 0;

    (void)customers;
    (void)customerCount;

    printSectionTitle("REPAIR ORDER HISTORY");
    
    printf("Enter phone number: ");
    if (!readLine(phone, sizeof(phone)))
    {
        printError("Input error.");
        return;
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

        for (i = 0; i < orderCount; i++)
        {
            if (orders[i].status == filterStatus)
            {
                if (count >= page * 10 && shown < 10)
                {
                    printf(" %-2d. %-10s | %-10s\n",
                           i + 1,
                           orders[i].orderId,
                           orders[i].customerPhone);
                    shown++;
                }

                count++;
            }
        }

        printf("\n[n] Next | [p] Prev | [q] Quit: ");

        if (!readLine(&cmd, 2))
        {
            break;
        }

        if (cmd == 'n' && page < totalPages - 1)
        {
            page++;
        }
        else if (cmd == 'p' && page > 0)
        {
            page--;
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

        for (i = start; i < end; i++)
        {
            printf("%d. %s | %s | %s\n",
                   i + 1,
                   orders[i].orderId,
                   orders[i].customerPhone,
                   getStatusText(orders[i].status));
        }

        printf("\n[n] Next | [p] Prev | [q] Quit: ");

        if (!readLine(&cmd, 2))
        {
            break;
        }

        if (cmd == 'n' && page < totalPages - 1)
        {
            page++;
        }
        else if (cmd == 'p' && page > 0)
        {
            page--;
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
    printf("  %-12s : %s\n", label, value);
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
    printOrderSummaryRow("Status", getStatusText(order.status));
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
