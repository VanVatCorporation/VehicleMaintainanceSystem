#ifndef FILEIO_H
#define FILEIO_H

#include "customer.h"
#include "repair.h"

// File paths
#define CUSTOMER_FILE "data/customers.csv"
#define REPAIR_FILE "data/repair_orders.csv"

// Persistence (CSV format)
void saveCustomers(Customer customers[], int count);
void loadCustomers(Customer customers[], int *count);
void saveRepairOrders(RepairOrder orders[], int count);
void loadRepairOrders(RepairOrder orders[], int *count);

// Invoice Export (.txt format)
void exportInvoice(RepairOrder order, Customer customer);

#endif
