#ifndef REPAIR_H
#define REPAIR_H

#include <time.h>
#include "customer.h"
#include "service.h"

#define MAX_ITEMS 10
#define MAX_SERVICES 100

// repair status
typedef enum {
    RECEIVED,
    UNDER_REPAIRED,
    COMPLETE
} Status;

// service stored inside a repair order
typedef struct {
    char name[50];
    int price;
} RepairItem;

// repair order
typedef struct {
    char orderId[10];
    char customerPhone[11];
    char symptom[100];
    time_t createdDate;
    Status status;

    RepairItem items[MAX_ITEMS];
    int itemCount;
} RepairOrder;

//=====prototype=====

// ID generators
void generateOrderId(char orderId[], int counter);

// repair order
RepairOrder createRepairOrder(int counter, Customer customers[], int customerCount, Service services[], int serviceCount);
void printRepairOrder(RepairOrder order);
long long calculateTotal(RepairOrder order);

// repair order management
void viewRepairOrderHistory(RepairOrder orders[],int orderCount, Customer customers[], int customerCount);
void listRepairOrders(RepairOrder orders[], int orderCount);
void filterRepairOrdersByStatus(RepairOrder orders[], int orderCount);

// status
const char* getStatusText(Status status);
RepairOrder updateStatus(RepairOrder order);

// output
void printOrderDivider(void);
void printOrderSummaryRow(const char label[], const char value[]);

#endif
