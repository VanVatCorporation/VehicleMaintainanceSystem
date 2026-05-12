#ifndef REPORT_H
#define REPORT_H
#include "customer.h"
#include "repair.h"

typedef struct ServiceCount {
    char serviceName[50];
    int count;
} ServiceCount;

long long calculateDailyRevenue(time_t date, RepairOrder orders[], int orderCount);
void getPopularServices(RepairOrder orders[], int orderCount);

void listAllCustomers(Customer customers[], int customerCount);
//TODO: list all services (import file? or hardcode? or load from orders?)

#endif // REPORT_H
