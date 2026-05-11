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
    snprintf(orderId, 10, "RO%06d", counter);
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

// create order
RepairOrder createRepairOrder(int counter, Customer customers[], int customerCount)
{
    RepairOrder order;
    int index;
    int i;

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

    // enter service
    printSectionTitle("SERVICE DETAILS");
    order.itemCount = inputServiceCount();

    for (i = 0; i < order.itemCount; i++)
    {
        printf("\n[Service %d]\n", i + 1);

        printf("Service name: ");
        if (!readLine(order.items[i].name, sizeof(order.items[i].name)))
        {
            strcpy(order.items[i].name, "");
        }

        order.items[i].price = inputPrice();
    }

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
            case 1: order.status = RECEIVED; return order;
            case 2: order.status = UNDER_REPAIRED; return order;
            case 3: order.status = COMPLETE; return order;
            default: printError("Invalid choice. Please try again.");
        }
    }
}

static void printOrderDivider(void)
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

static void printOrderSummaryRow(const char label[], const char value[])
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
