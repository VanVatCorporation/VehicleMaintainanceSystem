#include <stdio.h>
#include <string.h>

#include "repair.h"
#include "service.h"
#include "utils.h"


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

    snprintf(serviceId, 10, "SV%06d", counter + 1);

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

// list of service
void listServices(Service services[], int serviceCount)
{
    char priceText[32];

    printSectionTitle("SERVICE CATALOG");

    if (serviceCount == 0)
    {
        printError("No services available.");
        return;
    }

    printf("+------------+---------------------------+------------------+\n");
    printf("| ");
    printPaddedText("Service ID", 10, 0);
    printf(" | ");
    printPaddedText("Service Name", 25, 0);
    printf(" | ");
    printPaddedText("Price", 16, 0);
    printf(" |\n");
    printf("+------------+---------------------------+------------------+\n");

    for (int i = 0; i < serviceCount; i++)
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
}