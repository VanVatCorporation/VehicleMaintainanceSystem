#include <stdio.h>
#include <string.h>
#include <time.h>
#include "repair.h"
#include "customer.h"
#include "utils.h"

#define INPUT_LENGTH 100

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
int calculateTotal(RepairOrder order)
{
    int total = 0;
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
    printf("+-----+----------------------------------------+--------------+\n");
}

static void printOrderSummaryRow(const char label[], const char value[])
{
    printf("  %-12s : %s\n", label, value);
}

// print repair order
void printRepairOrder(RepairOrder order)
{
    int i;
    char createdText[32];

    if (strftime(createdText, sizeof(createdText), "%Y-%m-%d %H:%M", localtime(&order.createdDate)) == 0)
    {
        strcpy(createdText, "Unknown");
    }

    printBoxTitle("REPAIR ORDER DETAILS", 64);
    printOrderSummaryRow("Order ID", order.orderId);
    printOrderSummaryRow("Phone", order.customerPhone);
    printOrderSummaryRow("Created", createdText);
    printOrderSummaryRow("Status", getStatusText(order.status));
    printOrderSummaryRow("Problem", order.symptom);

    printf("\n");
    printOrderDivider();
    printf("| %-3s | %-38s | %-12s |\n", "No", "Service", "Price");
    printOrderDivider();

    for (i = 0; i < order.itemCount; i++)
    {
        printf("| %-3d | %-38.38s | %10d VND |\n",
            i + 1,
            order.items[i].name,
            order.items[i].price);
    }

    printOrderDivider();
    printf("| %-3s | %-38s | %10d VND |\n", "", "Total", calculateTotal(order));
    printOrderDivider();
}
