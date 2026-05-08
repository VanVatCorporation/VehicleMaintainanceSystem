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

// create service Id
void generateServiceId(char id[], int counter){
    sprintf(id, "SV%06d", counter);
}

// create order
RepairOrder createRepairOrder(int counter, Customer customers[], int customerCount, Service services[], int serviceCount){
    RepairOrder order;
    char inputPhone[11];
    char id[10];

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
    printf("Enter problems of car:\n");
    fgets(order.symptom, sizeof(order.symptom), stdin);
    order.symptom[strcspn(order.symptom, "\n")] = 0;

    // Enter service 
    printf("Enter number of services: ");
    scanf("%d", &order.itemCount);
    getchar();

    if(order.itemCount > MAX_ITEMS){
        order.itemCount = MAX_ITEMS;
    }

    printf("\n=== SERVICE LIST ===\n");
    for(int i = 0; i < serviceCount; i++){
        printf("%s | %s | %d VND\n",
            services[i].serviceId,
            services[i].name,
            services[i].price);
    }

    for(int i = 0; i < order.itemCount; i++){
        printf("\nService %d\n", i + 1);

        printf("Enter service ID: ");
        scanf("%s", id);
        getchar();

        int idx = findServiceIndexById(services, serviceCount, id);

        if(idx == -1){
            printf("Service not found! Try again.\n");
            i--; // nhập lại
            continue;
        }

        strcpy(order.items[i].name, services[idx].name);
        order.items[i].price = services[idx].price;
    }

    order.createdDate = time(NULL);

    order.status = RECEIVED;// default status

    return order;
}

// add service to the list 
void addService(Service services[], int *serviceCount){
    if(*serviceCount >= MAX_SERVICES){
        printf("Service list is full!\n");
        return;
    }

    Service s;

    generateServiceId(s.serviceId, *serviceCount);
    printf("Service ID: %s\n", s.serviceId);

    printf("Enter service name: ");
    getchar();
    fgets(s.name, sizeof(s.name), stdin);
    s.name[strcspn(s.name, "\n")] = 0;

    printf("Enter price: ");
    scanf("%d", &s.price);

    services[*serviceCount] = s;
    (*serviceCount)++;

    printf("Add service successfully!\n");
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

// find service id
int findServiceIndexById(Service services[], int count, char id[]){
    for(int i = 0; i < count; i++){
        if(strcmp(services[i].serviceId, id) == 0){
            return i;
        }
    }
    return -1;
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
        case 1: newStatus = RECEIVED; break;
        case 2: newStatus = UNDER_REPAIRED; break;
        case 3: newStatus = COMPLETE; break;
        default: 
            printf("Error!\n");
            return order;
    }

    // check status
    // TODO: lỗi vẫn đảo ngược được
    if(newStatus < order.status){
        printf("Cannot go back original status!\n");
    }else if(newStatus == order.status){
        printf("Status is the same!\n");
    } else {
        order.status = newStatus;
        printf("Update complete!\n");
    }
    return order;
}

// update service in list
void updateService(Service services[], int serviceCount){
    char id[10];
    int index;

    printf("Enter service ID to update: ");
    scanf("%9s", id);

    index = findServiceIndexById(services, serviceCount, id);

    if(index == -1){
        printf("Service not found!\n");
        return;
    }

    printf("\n--- OLD INFORMATION ---\n");
    printf("Name: %s\n", services[index].name);
    printf("Price: %d\n", services[index].price);

    printf("\n--- UPDATE ---\n");

    printf("Enter new name: ");
    getchar(); // clear buffer
    fgets(services[index].name, sizeof(services[index].name), stdin);
    services[index].name[strcspn(services[index].name, "\n")] = 0;

    printf("Enter new price: ");
    scanf("%d", &services[index].price);

    printf("Update service successfully!\n");
}

// view history of order
void viewRepairOrderDetail(RepairOrder orders[], int orderCount, Customer customers[], int customerCount){
    char phone[11];

    printf("Enter phone number: ");
    scanf("%10s", phone);

    int found = 0;

    for(int i = 0; i < orderCount; i++){
        if(strcmp(orders[i].customerPhone, phone) == 0){

            printf("\n===== ORDER %d =====\n", i + 1);
            printRepairOrder(orders[i], customers, customerCount);

            found = 1;
        }
    }

    if(!found){
        printf("No history found for this phone number!\n");
    }
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
// list ALL order
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

// view details order
void printRepairOrder(RepairOrder order, Customer customers[], int customerCount){
    printf("\n=== ORDER INFORMATION ===\n");

    int index = findCustomerIndexByPhone(customers, customerCount, order.customerPhone);

    if(index != -1){
        Customer c = customers[index];

        printf("\n--- CUSTOMER INFORMATION ---\n");
        printf("Name: %s\n", c.fullName);
        printf("Phone: %s\n", c.phoneNumber);
        printf("Plate: %s\n", c.carPlate);
        printf("Car type: %s\n", c.carType);
    } else {
        printf("\n--- CUSTOMER INFORMATION ---\n");
        printf("UNKNOWN!\n");
    }

    printf("\n--- ORDER INFORMATION ---\n");
    printf("Order ID: %s\n", order.orderId);
    printf("Symptom: %s\n", order.symptom);
    printf("Status: %s\n", getStatusText(order.status));
    printf("Created: %s", ctime(&order.createdDate));

    printf("\n--- SERVICES ---\n");
    for(int i = 0; i < order.itemCount; i++){
        printf("%d. %s - %d VND\n",
            i + 1,
            order.items[i].name,
            order.items[i].price);
    }

    printf("\nTotal: %d VND\n", calculateTotal(order));
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