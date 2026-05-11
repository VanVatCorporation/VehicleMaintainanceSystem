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
    snprintf(orderId, 10, "RO%06d", counter + 1);
}

// create service Id 
void generateServiceId(char serviceId[], int counter)
{
    snprintf(serviceId, 10, "SV%06d", counter + 1);
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

    printSectionTitle("SERVICE DETAILS");
    order.itemCount = inputServiceCount();

    if (serviceCount <= 0)
    {
        printError("No services available.");
        order.itemCount = 0;
        return order;
    }

    printBoxTitle("AVAILABLE SERVICES", 64);
    printOrderDivider();
    printf("| %-10s | %-25s | %-12s |\n",
           "Service ID",
           "Service Name",
           "Price");
    printOrderDivider();

    for (i = 0; i < serviceCount; i++)
    {
        printf("| %-10s | %-25.25s | %10d VND |\n",
               services[i].serviceId,
               services[i].name,
               services[i].price);
    }

    printOrderDivider();

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

    // service
    order = inputOrderServices(order, services, serviceCount);

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
    printf("+-----+----------------------------------------+--------------+\n");
}

 void printOrderSummaryRow(const char label[], const char value[])
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
