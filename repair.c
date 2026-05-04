#include <stdio.h>
#include <string.h>
#include <time.h>
#include "repair.h"
#include "customer.h"
#include "utils.h"

// create order Id
void generateOrderId(char orderId[], int counter){
    sprintf(orderId, "RO%06d", counter);
}

// create order
RepairOrder createRepairOrder(int counter, Customer customers[], int customerCount){
    RepairOrder order;
    char inputPhone[11];

    generateOrderId(order.orderId, counter);

    // enter number phone
    printf("Enter phone number: ");
    fgets(inputPhone, sizeof(inputPhone), stdin);
    inputPhone[strcspn(inputPhone, "\n")] = 0;

    strcpy(order.customerPhone, inputPhone);

    // find customer by phone number and car plate
    int index = findCustomerIndexByPhone(customers, customerCount, inputPhone);

    if(index == -1){
        printf("Customer not found!\n");
    } else {
        printf("The customer has found:\n");
        displayCustomer(customers[index]);
        printf("Enter car plate: %s\n", customers[index].carPlate);
    }

    // Problems 
    printf("Enter problems of car: ");
    fgets(order.symptom, sizeof(order.symptom), stdin);
    order.symptom[strcspn(order.symptom, "\n")] = 0;

    // Enter service 
    printf("Enter number of service: ");
    scanf("%d", &order.itemCount);
    getchar();

    if(order.itemCount > MAX_ITEMS){
        order.itemCount = MAX_ITEMS;
    }

    for(int i = 0; i < order.itemCount; i++){
        printf("\nService %d\n", i + 1);

        printf("Name of service: ");
        fgets(order.items[i].name, sizeof(order.items[i].name), stdin);
        order.items[i].name[strcspn(order.items[i].name, "\n")] = 0;

        printf("Price: ");
        scanf("%d", &order.items[i].price);
        getchar();
    }

    order.createdDate = time(NULL);

    order.status = RECEIVED;// default status

    return order;
}

// caculate money when print order
int calculateTotal(RepairOrder order){
    int total = 0;
    for(int i = 0; i < order.itemCount; i++){
        total += order.items[i].price;
    }
    return total;
}

// update status
const char* getStatusText(Status status){
    switch(status){
        case RECEIVED: return "Received";
        case UNDER_REPAIRED: return "Under repaired";
        case COMPLETE: return "Complete";
        default: return "Unknown";
    }
}

// update status when creating a order
RepairOrder updateStatus(RepairOrder order){
    int choice;

    printf("\nUpdate status:\n");
    printf("1. Received\n");
    printf("2. Under repaired\n");
    printf("3. Complete\n");
    printf("Choose: ");
    scanf("%d", &choice);

    switch(choice){
        case 1: order.status = RECEIVED; break;
        case 2: order.status = UNDER_REPAIRED; break;
        case 3: order.status = COMPLETE; break;
        default: printf("Error!\n");
    }

    return order;
}

// in phiếu
void printRepairOrder(RepairOrder order){
    printf("\n=== ORDER INFORMATION ===\n");
    printf("Order ID: %s\n", order.orderId);
    printf("Phone: %s\n", order.customerPhone);
    printf("Symptom: %s\n", order.symptom);
    printf("Created: %s", ctime(&order.createdDate));
    printf("Status: %s\n", getStatusText(order.status));
    printf("\nList of service:\n");
    for(int i = 0; i < order.itemCount; i++){
        printf("%d. %s - %d VND\n",
            i + 1,
            order.items[i].name,
            order.items[i].price);
    }

    printf("Total of money: %d VND\n", calculateTotal(order));
}

void searchOrder(RepairOrder orders[], int orderCount){
    char searchTerm[50];
    int found = 0;

    printf(ANSI_COLOR_YELLOW "\n=== SEARCH REPAIR ORDER ===\n" ANSI_COLOR_RESET);
    printf("Enter Order ID or Customer Phone: ");
    scanf("%s", searchTerm);
    while(getchar() != '\n');

    printf(ANSI_COLOR_CYAN "\n%-10s | %-15s | %-20s | %-15s\n" ANSI_COLOR_RESET, "ID", "Phone", "Symptom", "Status");
    printf("------------------------------------------------------------\n");

    for (int i = 0; i < orderCount; i++) {
        if (strstr(orders[i].orderId, searchTerm) || strstr(orders[i].customerPhone, searchTerm)) {
            printf(ANSI_COLOR_GREEN "%-10s | %-15s | %-20s | %-15s\n" ANSI_COLOR_RESET, 
                   orders[i].orderId, 
                   orders[i].customerPhone, 
                   orders[i].symptom, 
                   getStatusText(orders[i].status)); 
            found++;
        }
    }

    if (found == 0) {
        printf(ANSI_COLOR_RED "No orders found matching: %s\n" ANSI_COLOR_RESET, searchTerm);
    } else {
        printf("\nFound %d result(s).\n", found);
    }
}