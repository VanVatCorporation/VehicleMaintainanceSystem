// Revenue and popular-service reports.

#include <stdio.h>
#include <time.h>
#include <string.h>
#include "report.h"
#include "repair.h"
#include "utils.h"

long long calculateDailyRevenue(time_t date, RepairOrder orders[], int orderCount)
{
    long long totalRevenue = 0;
    struct tm *targetDateInfo = localtime(&date);

    if (targetDateInfo == NULL)
    {
        return 0;
    }

    struct tm targetDate = *targetDateInfo;

    for (int i = 0; i < orderCount; i++)
    {
        struct tm *orderDateInfo = localtime(&orders[i].createdDate);

        if (orders[i].status != COMPLETE)
        {
            continue;
        }

        if (orderDateInfo == NULL)
        {
            continue;
        }

        struct tm orderDate = *orderDateInfo;

        if (orderDate.tm_year == targetDate.tm_year &&
            orderDate.tm_mon == targetDate.tm_mon &&
            orderDate.tm_mday == targetDate.tm_mday)
        {
            totalRevenue += calculateTotal(orders[i]);
        }
    }

    return totalRevenue;
}

void getPopularServices(RepairOrder orders[], int orderCount)
{
    ServiceCount serviceCounts[MAX_SERVICES];
    int serviceCount = 0;

    for (int i = 0; i < MAX_SERVICES; i++)
    {
        serviceCounts[i].count = 0;
        serviceCounts[i].serviceName[0] = '\0';
    }

    for (int i = 0; i < orderCount; i++)
    {
        if (orders[i].status != COMPLETE)
        {
            continue;
        }

        for (int j = 0; j < orders[i].itemCount; j++)
        {
            char *serviceName = orders[i].items[j].name;
            int found = 0;

            for (int k = 0; k < serviceCount; k++)
            {
                if (strcmp(serviceCounts[k].serviceName, serviceName) == 0)
                {
                    serviceCounts[k].count++;
                    found = 1;
                    break;
                }
            }

            if (!found && serviceCount < MAX_SERVICES)
            {
                strcpy(serviceCounts[serviceCount].serviceName, serviceName);
                serviceCounts[serviceCount].count = 1;
                serviceCount++;
            }
        }
    }

    for (int i = 0; i < serviceCount - 1; i++)
    {
        for (int j = i + 1; j < serviceCount; j++)
        {
            if (serviceCounts[i].count < serviceCounts[j].count)
            {
                ServiceCount temp = serviceCounts[i];
                serviceCounts[i] = serviceCounts[j];
                serviceCounts[j] = temp;
            }
        }
    }

    printf("Popular Services:\n");
    if (serviceCount == 0)
    {
        printf("- No service data available.\n");
        return;
    }

    for (int i = 0; i < serviceCount; i++)
    {
        printf("- %s: %d\n", serviceCounts[i].serviceName, serviceCounts[i].count);
    }
}

void listAllCustomers(Customer customers[], int customerCount)
{
    printf("Customer List:\n");
    if (customerCount == 0)
    {
        printError("No customers available.");
        return;
    }

    for (int i = 0; i < customerCount; i++)
    {
        displayCustomer(customers[i]);
    }
}
