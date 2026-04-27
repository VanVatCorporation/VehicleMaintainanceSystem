#include "fileio.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// Persistence (CSV format)

void saveCustomers(Customer customers[], int count) {
  FILE *file = fopen(CUSTOMER_FILE, "w");
  if (file == NULL) {
    printf("Error: Could not open %s for writing.\n", CUSTOMER_FILE);
    return;
  }

  for (int i = 0; i < count; i++) {
    fprintf(file, "%s,%s,%s,%s,%s,%d\n", customers[i].customerId,
            customers[i].fullName, customers[i].phoneNumber,
            customers[i].carPlate, customers[i].carType,
            customers[i].orderCount);
  }

  fclose(file);
}

void loadCustomers(Customer customers[], int *count) {
  FILE *file = fopen(CUSTOMER_FILE, "r");
  if (file == NULL) {
    // File might not exist yet, which is fine for the first run
    *count = 0;
    return;
  }

  *count = 0;
  char line[512];
  while (fgets(line, sizeof(line), file)) {
    if (*count >= MAX_CUSTOMERS)
      break;

    Customer *c = &customers[*count];
    // Parse CSV line: ID,Name,Phone,Plate,Type,OrderCount
    // Using sscanf with [^,] to handle spaces within fields
    if (sscanf(line, "%[^,],%[^,],%[^,],%[^,],%[^,],%d", c->customerId,
               c->fullName, c->phoneNumber, c->carPlate, c->carType,
               &c->orderCount) == 6) {
      (*count)++;
    }
  }

  fclose(file);
}

void saveRepairOrders(RepairOrder orders[], int count) {
  FILE *file = fopen(REPAIR_FILE, "w");
  if (file == NULL) {
    printf("Error: Could not open %s for writing.\n", REPAIR_FILE);
    return;
  }

  for (int i = 0; i < count; i++) {
    // Basic info: OrderID,Phone,Symptom,Date,Status,ItemCount
    fprintf(file, "%s,%s,%s,%ld,%d,%d", orders[i].orderId,
            orders[i].customerPhone, orders[i].symptom,
            (long)orders[i].createdDate, orders[i].status, orders[i].itemCount);

    // Nested items: ServiceName,Price
    for (int j = 0; j < orders[i].itemCount; j++) {
      fprintf(file, ",%s,%d", orders[i].items[j].name,
              orders[i].items[j].price);
    }
    fprintf(file, "\n");
  }

  fclose(file);
}

void loadRepairOrders(RepairOrder orders[], int *count) {
  FILE *file = fopen(REPAIR_FILE, "r");
  if (file == NULL) {
    *count = 0;
    return;
  }

  *count = 0;
  char line[1024];
  while (fgets(line, sizeof(line), file)) {
    // For RepairOrders, parsing is more complex due to variable number of
    // items. Use strtok to parse the comma-separated values.
    char *token = strtok(line, ",");
    if (token == NULL)
      continue;

    RepairOrder *o = &orders[*count];

    // OrderID
    strncpy(o->orderId, token, 10);

    // Phone
    token = strtok(NULL, ",");
    if (token)
      strncpy(o->customerPhone, token, 11);

    // Symptom
    token = strtok(NULL, ",");
    if (token)
      strncpy(o->symptom, token, 100);

    // Date
    token = strtok(NULL, ",");
    if (token)
      o->createdDate = (time_t)atol(token);

    // Status
    token = strtok(NULL, ",");
    if (token)
      o->status = (Status)atoi(token);

    // ItemCount
    token = strtok(NULL, ",");
    if (token)
      o->itemCount = atoi(token);

    // Items
    for (int j = 0; j < o->itemCount; j++) {
      token = strtok(NULL, ",");
      if (token)
        strncpy(o->items[j].name, token, 50);

      token = strtok(NULL, ",");
      if (token)
        o->items[j].price = atoi(token);
    }

    (*count)++;
  }

  fclose(file);
}

// Invoice Export (.txt format)
// Code structures are mine, but the bill graphic (fprintf) are AI generated.

void exportInvoice(RepairOrder order, Customer customer) {
  char filename[100];
  snprintf(filename, sizeof(filename), "invoice_%s.txt", order.orderId);

  FILE *file = fopen(filename, "w");
  if (file == NULL) {
    printf("Error: Could not create invoice file %s.\n", filename);
    return;
  }

  fprintf(file, "==========================================\n");
  fprintf(file, "=          VEHICLE REPAIR INVOICE        =\n");
  fprintf(file, "==========================================\n");

  // Timestamp
  time_t now = time(NULL);
  fprintf(file, "Date: %s", ctime(&now));
  fprintf(file, "Order ID: %s\n", order.orderId);
  fprintf(file, "------------------------------------------\n");

  // Customer Info
  fprintf(file, "CUSTOMER INFORMATION:\n");
  fprintf(file, "Name: %s\n", customer.fullName);
  fprintf(file, "Phone: %s\n", customer.phoneNumber);
  fprintf(file, "Car Plate: %s\n", customer.carPlate);
  fprintf(file, "Car Type: %s\n", customer.carType);
  fprintf(file, "------------------------------------------\n");

  // Symptoms
  fprintf(file, "Symptoms: %s\n", order.symptom);
  fprintf(file, "------------------------------------------\n");

  // Services
  fprintf(file, "SERVICES & PARTS:\n");
  fprintf(file, "%-30s %10s\n", "Service Name", "Price");
  int total = 0;
  for (int i = 0; i < order.itemCount; i++) {
    fprintf(file, "%-30s %10d VND\n", order.items[i].name,
            order.items[i].price);
    total += order.items[i].price;
  }
  fprintf(file, "------------------------------------------\n");

  // Total
  fprintf(file, "TOTAL AMOUNT: %18d VND\n", total);
  fprintf(file, "==========================================\n");
  fprintf(file, "=      Thank you for your business!      =\n");
  fprintf(file, "==========================================\n");

  fclose(file);
  printf("Invoice exported successfully: %s\n", filename);
}
