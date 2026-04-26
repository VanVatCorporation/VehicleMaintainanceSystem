// Thống kê doanh thu, xuất hóa đơn phiếu sửa ra file .txt

#include <stdio.h>
#include <time.h>
#include <string.h>
#include "report.h"
#include "repair.h"

int calculateDailyRevenue(time_t date, RepairOrder orders[], int orderCount) {
    int totalRevenue = 0;
    
    // Convert target date to struct tm for comparison
    struct tm targetDate = *localtime(&date);
    
    // Iterate through all orders
    for (int i = 0; i < orderCount; i++) {

        // Convert order date to struct tm
        struct tm orderDate = *localtime(&orders[i].createdDate);
        
        // Check if the order was created on the same day
        if (orderDate.tm_year == targetDate.tm_year &&
            orderDate.tm_mon == targetDate.tm_mon &&
            orderDate.tm_mday == targetDate.tm_mday) {
            
            // Add this order's total to the revenue
            totalRevenue += calculateTotal(orders[i]);
        }
    }
    
    return totalRevenue;
}

void getPopularServices(RepairOrder orders[], int orderCount) {
    ServiceCount serviceCounts[MAX_SERVICES];
    int serviceCount = 0;

    // Initialize the service counts
    for (int i = 0; i < MAX_SERVICES; i++) {
        serviceCounts[i].count = 0;
    }

    // Count each service
    for (int i = 0; i < orderCount; i++) {
        for (int j = 0; j < orders[i].itemCount; j++) {
            char *serviceName = orders[i].items[j].name;
            int found = 0;

            // Check if the service is already in our list
            for (int k = 0; k < serviceCount; k++) {
                if (strcmp(serviceCounts[k].serviceName, serviceName) == 0) {
                    serviceCounts[k].count++;
                    found = 1;
                    break;
                }
            }

            // If not found, add it to the list
            if (!found && serviceCount < MAX_SERVICES) {
                strcpy(serviceCounts[serviceCount].serviceName, serviceName);
                serviceCounts[serviceCount].count = 1;
                serviceCount++;
            }
        }
    }

    // Sort services by count (descending)
    for (int i = 0; i < serviceCount - 1; i++) {
        for (int j = i + 1; j < serviceCount; j++) {
            if (serviceCounts[i].count < serviceCounts[j].count) {
                ServiceCount temp = serviceCounts[i];
                serviceCounts[i] = serviceCounts[j];
                serviceCounts[j] = temp;
            }
        }
    }

    // Print the popular services
    printf("Popular Services:\n");
    for (int i = 0; i < serviceCount; i++) {
        printf("- %s: %d\n", serviceCounts[i].serviceName, serviceCounts[i].count);
    }
}

void listAllCustomers(Customer customers[], int customerCount) {
    printf("Customer List:\n");
    for (int i = 0; i < customerCount; i++) {
        displayCustomer(customers[i]);
    }
}