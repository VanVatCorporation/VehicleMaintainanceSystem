#include <stdio.h>
#include <string.h>
#include <time.h>
#include "repair.h"
#include "customer.h"

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

    // TODO: xem lại customer để sửa  
    // find customer by phone number and car plate
    int index = findCustomerIndexByPhone(customers, customerCount, inputPhone); // 

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
        case UNDER_REPAIRED: return "Under repair";
        case COMPLETE: return "Completed";
        default: return "Unknown";
    }
}

// update status when creating a order
RepairOrder updateStatus(RepairOrder order){
    int choice;
    Status newStatus;

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

    // check status 
    if(newStatus < order.status){
        printf("cannot go back original status!\n");
    } else {
        order.status = newStatus;
        printf("Update complete!\n");
    }
    return order;
}

//View information of order
void viewRepairOrderDetail(RepairOrder order, Customer customers[], int customerCount){
    printf("\n===== INFORMATION =====\n");

    int index = findCustomerIndexByPhone(customers, customerCount, order.customerPhone);

    if(index != -1){
        Customer c = customers[index];

        printf("\n--- INFORMATION OF CUSTOMER ---\n");
        printf("NAME: %s\n", c.fullName);
        printf("PHONE NUMBER: %s\n", c.phoneNumber);
        printf("PLATE: %s\n", c.carPlate);
        printf("CAR TYPE: %s\n", c.carType);
    } else {
        printf("UNKNOWN!\n");
    }

    printf("\n--- INFORMATION OF  ORDER ---\n");
    printf("Order ID: %s\n", order.orderId);
    printf("Situation: %s\n", order.symptom);
    printf("Status: %s\n", getStatusText(order.status));
    printf("Created: %s", ctime(&order.createdDate));

    printf("\n--- SERVICE ---\n");
    for(int i = 0; i < order.itemCount; i++){
        printf("%d. %s - %d VND\n",
            i + 1,
            order.items[i].name,
            order.items[i].price);
    }

    printf("\nTotal: %d VND\n", calculateTotal(order));
}

// TODO: thêm paging cho filter khi lọc
// filter order by status
void filterRepairOrdersByStatus(RepairOrder orders[], int orderCount){
    int choice;
    Status filterStatus;

    printf("\nFilter by status:\n");
    printf("1. Received\n");
    printf("2. Under repaired\n");
    printf("3. Complete\n");
    printf("Choice: ");
    scanf("%d", &choice);

    switch(choice){
        case 1: filterStatus = RECEIVED; break;
        case 2: filterStatus = UNDER_REPAIRED; break;
        case 3: filterStatus = COMPLETE; break;
        default:
            printf("Unknown!\n");
            return;
    }

    // paging 
    int totalItems = 0;
    for(int i = 0; i < orderCount; i++){
        if(orders[i].status == filterStatus){
            totalItems++;
        }
    }

    int totalPages = (totalItems + 9) / 10;

    if(totalItems == 0){
        printf("\nNo data!\n");
        return;
    }

    int page = 0;
    char cmd;

    do{
        int count = 0;
        int shown = 0;

        printf("\n===== FILTER RESULTS (Page %d/%d) =====\n", page + 1, totalPages);

        for(int i = 0; i < orderCount; i++){
            if(orders[i].status == filterStatus){

                if(count >= page * 10 && shown < 10){
                    printf("%d. %s | %s\n",
                        i + 1,
                        orders[i].orderId,
                        orders[i].customerPhone);
                    shown++;
                }

                count++;
            }
        }

        printf("\n[n] Next | [p] Prev | [q] Quit: ");
        scanf(" %c", &cmd);

        if(cmd == 'n' && page < totalPages - 1){
            page++;
        }
        else if(cmd == 'p' && page > 0){
            page--;
        }

    } while(cmd != 'q');
}

// TODO: thêm paging cho list
// list of order
void listRepairOrders(RepairOrder orders[], int orderCount){
    int page = 0;
    int totalPages = (orderCount + 9) / 10;

    char choice;

    do{
        int start = page * 10;
        int end = start + 10;

        if(end > orderCount) end = orderCount;

        printf("\n===== LIST OF ORDER (Page %d/%d) =====\n", page + 1, totalPages);

        for(int i = start; i < end; i++){
            printf("%d. %s | %s | %s\n",
                i + 1,
                orders[i].orderId,
                orders[i].customerPhone,
                getStatusText(orders[i].status));
        }

        printf("\n[n] Next | [p] Prev | [q] Quit: ");
        scanf(" %c", &choice);

        if(choice == 'n' && page < totalPages - 1){
            page++;
        }
        else if(choice == 'p' && page > 0){
            page--;
        }

    } while(choice != 'q');
}

// Print order
void printRepairOrder(RepairOrder order){
    printf("\n=== ORDER INFORMATION ===\n");
    printf("Order ID: %s\n", order.orderId);
    printf("Phone: %s\n", order.customerPhone);
    printf("Symptom: %s\n", order.symptom);
    printf("Created: %s\n", ctime(&order.createdDate));
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
