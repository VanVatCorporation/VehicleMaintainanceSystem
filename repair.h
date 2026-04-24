#ifndef REPAIR_H
#define REPAIR_H

#include <time.h>
#include "customer.h"

#define MAX_ITEMS 20

// repair status
typedef enum {
    RECEIVED,
    UNDER_REPAIRED,
    COMPLETE
} Status;

// service
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

// prototype
void generateOrderId(char orderId[], int counter);
RepairOrder createRepairOrder(int counter, Customer customers[], int customerCount);
void printRepairOrder(RepairOrder order);
int calculateTotal(RepairOrder order);

// status
const char* getStatusText(Status status);
RepairOrder updateStatus(RepairOrder order);

#endif
