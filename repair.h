#ifndef REPAIR_H
#define REPAIR_H

#include <time.h>
#include "customer.h"

#define MAX_ITEMS 10
#define MAX_SERVICES 100

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

typedef struct {
    char serviceId[10];
    char name[50];
    int price;
} Service;

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

// ======prototype======

// order
void generateOrderId(char orderId[], int counter);
RepairOrder createRepairOrder(int counter, Customer customers[], int customerCount, Service services[], int serviceCount);
void printRepairOrder(RepairOrder order, Customer customers[], int customerCount);
int calculateTotal(RepairOrder order);

// status
const char* getStatusText(Status status);
RepairOrder updateStatus(RepairOrder order);

//service
void generateServiceId(char id[], int counter);
void addService(Service services[], int *serviceCount);
void updateService(Service services[], int serviceCount);
int findServiceIndexById(Service services[], int count, char id[]);

// view and filter
void viewRepairOrderDetail(RepairOrder orders[], int orderCount, Customer customers[], int customerCount);
void listRepairOrders(RepairOrder orders[], int orderCount);
void filterRepairOrdersByStatus(RepairOrder orders[], int orderCount);

#endif
