#include "fileio.h"
#include "utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// Persistence (CSV format)

static void writeCsvField(FILE *file, const char value[]) {
  int needsQuotes = 0;

  if (value == NULL) {
    value = "";
  }

  for (const char *current = value; *current != '\0'; current++) {
    if (*current == ',' || *current == '"' || *current == '\n' ||
        *current == '\r') {
      needsQuotes = 1;
      break;
    }
  }

  if (!needsQuotes) {
    fprintf(file, "%s", value);
    return;
  }

  fputc('"', file);
  for (const char *current = value; *current != '\0'; current++) {
    if (*current == '"') {
      fputc('"', file);
    }
    fputc(*current, file);
  }
  fputc('"', file);
}

static const char *readCsvField(const char *cursor, char output[],
                                size_t size) {
  size_t length = 0;
  int quoted = 0;

  if (size > 0) {
    output[0] = '\0';
  }

  if (cursor == NULL) {
    return NULL;
  }

  if (*cursor == '"') {
    quoted = 1;
    cursor++;
  }

  while (*cursor != '\0') {
    if (quoted) {
      if (*cursor == '"') {
        if (*(cursor + 1) == '"') {
          if (length + 1 < size) {
            output[length++] = '"';
          }
          cursor += 2;
          continue;
        }

        cursor++;
        break;
      }
    } else if (*cursor == ',' || *cursor == '\n' || *cursor == '\r') {
      break;
    }

    if (length + 1 < size) {
      output[length++] = *cursor;
    }
    cursor++;
  }

  if (size > 0) {
    output[length] = '\0';
  }

  while (*cursor != '\0' && *cursor != ',' && *cursor != '\n' &&
         *cursor != '\r') {
    cursor++;
  }

  if (*cursor == ',') {
    cursor++;
  }

  return cursor;
}

static int readRequiredCsvField(const char **cursor, char output[],
                                size_t size) {
  if (*cursor == NULL || **cursor == '\0' || **cursor == '\n' ||
      **cursor == '\r') {
    return 0;
  }

  *cursor = readCsvField(*cursor, output, size);
  return *cursor != NULL;
}

void saveCustomers(Customer customers[], int count) {
  FILE *file = fopen(CUSTOMER_FILE, "w");
  if (file == NULL) {
    printf("Error: Could not open %s for writing.\n", CUSTOMER_FILE);
    return;
  }

  for (int i = 0; i < count; i++) {
    writeCsvField(file, customers[i].customerId);
    fputc(',', file);
    writeCsvField(file, customers[i].fullName);
    fputc(',', file);
    writeCsvField(file, customers[i].phoneNumber);
    fputc(',', file);
    writeCsvField(file, customers[i].carPlate);
    fputc(',', file);
    writeCsvField(file, customers[i].carType);
    fprintf(file, ",%d\n", customers[i].orderCount);
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

    Customer parsedCustomer;
    const char *cursor = line;
    char orderCountText[16];

    memset(&parsedCustomer, 0, sizeof(parsedCustomer));

    if (readRequiredCsvField(&cursor, parsedCustomer.customerId,
                             sizeof(parsedCustomer.customerId)) &&
        readRequiredCsvField(&cursor, parsedCustomer.fullName,
                             sizeof(parsedCustomer.fullName)) &&
        readRequiredCsvField(&cursor, parsedCustomer.phoneNumber,
                             sizeof(parsedCustomer.phoneNumber)) &&
        readRequiredCsvField(&cursor, parsedCustomer.carPlate,
                             sizeof(parsedCustomer.carPlate)) &&
        readRequiredCsvField(&cursor, parsedCustomer.carType,
                             sizeof(parsedCustomer.carType)) &&
        readRequiredCsvField(&cursor, orderCountText, sizeof(orderCountText))) {
      repairTextEncoding(parsedCustomer.fullName, sizeof(parsedCustomer.fullName));
      repairTextEncoding(parsedCustomer.carType, sizeof(parsedCustomer.carType));
      parsedCustomer.orderCount = atoi(orderCountText);
      customers[*count] = parsedCustomer;
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
    writeCsvField(file, orders[i].orderId);
    fputc(',', file);
    writeCsvField(file, orders[i].customerPhone);
    fputc(',', file);
    writeCsvField(file, orders[i].symptom);
    fprintf(file, ",%ld,%d,%d", (long)orders[i].createdDate, orders[i].status,
            orders[i].itemCount);

    // Nested items: ServiceName,Price
    for (int j = 0; j < orders[i].itemCount; j++) {
      fputc(',', file);
      writeCsvField(file, orders[i].items[j].name);
      fprintf(file, ",%d", orders[i].items[j].price);
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
    if (*count >= MAX_REPAIR_ORDERS) {
      break;
    }

    RepairOrder parsedOrder;
    const char *cursor = line;
    char createdText[32];
    char statusText[16];
    char itemCountText[16];

    memset(&parsedOrder, 0, sizeof(parsedOrder));

    if (!readRequiredCsvField(&cursor, parsedOrder.orderId,
                              sizeof(parsedOrder.orderId)) ||
        !readRequiredCsvField(&cursor, parsedOrder.customerPhone,
                              sizeof(parsedOrder.customerPhone)) ||
        !readRequiredCsvField(&cursor, parsedOrder.symptom,
                              sizeof(parsedOrder.symptom)) ||
        !readRequiredCsvField(&cursor, createdText, sizeof(createdText)) ||
        !readRequiredCsvField(&cursor, statusText, sizeof(statusText)) ||
        !readRequiredCsvField(&cursor, itemCountText, sizeof(itemCountText))) {
      continue;
    }

    parsedOrder.createdDate = (time_t)atol(createdText);
    parsedOrder.status = (Status)atoi(statusText);
    if (parsedOrder.status < RECEIVED || parsedOrder.status > COMPLETE) {
      parsedOrder.status = RECEIVED;
    }
    repairTextEncoding(parsedOrder.symptom, sizeof(parsedOrder.symptom));

    parsedOrder.itemCount = atoi(itemCountText);

    if (parsedOrder.itemCount < 0) {
      parsedOrder.itemCount = 0;
    }
    if (parsedOrder.itemCount > MAX_ITEMS) {
      parsedOrder.itemCount = MAX_ITEMS;
    }

    for (int j = 0; j < parsedOrder.itemCount; j++) {
      char priceText[16];

      if (!readRequiredCsvField(&cursor, parsedOrder.items[j].name,
                                sizeof(parsedOrder.items[j].name)) ||
          !readRequiredCsvField(&cursor, priceText, sizeof(priceText))) {
        parsedOrder.itemCount = j;
        break;
      }

      parsedOrder.items[j].price = atoi(priceText);
      repairTextEncoding(parsedOrder.items[j].name, sizeof(parsedOrder.items[j].name));
    }

    orders[*count] = parsedOrder;
    (*count)++;
  }

  fclose(file);
}

void saveServices(Service services[], int count) {
  FILE *file = fopen(SERVICE_FILE, "w");
  if (file == NULL) {
    printf("Error: Could not open %s for writing.\n", SERVICE_FILE);
    return;
  }

  for (int i = 0; i < count; i++) {
    writeCsvField(file, services[i].serviceId);
    fputc(',', file);
    writeCsvField(file, services[i].name);
    fprintf(file, ",%d\n", services[i].price);
  }

  fclose(file);
}

void loadServices(Service services[], int *count) {
  FILE *file = fopen(SERVICE_FILE, "r");
  if (file == NULL) {
    *count = 0;
    return;
  }

  *count = 0;
  char line[512];
  while (fgets(line, sizeof(line), file)) {
    Service parsedService;
    const char *cursor = line;
    char priceText[16];

    if (*count >= MAX_SERVICES) {
      break;
    }

    memset(&parsedService, 0, sizeof(parsedService));

    if (readRequiredCsvField(&cursor, parsedService.serviceId,
                             sizeof(parsedService.serviceId)) &&
        readRequiredCsvField(&cursor, parsedService.name,
                             sizeof(parsedService.name)) &&
        readRequiredCsvField(&cursor, priceText, sizeof(priceText))) {
      repairTextEncoding(parsedService.name, sizeof(parsedService.name));
      parsedService.price = atoi(priceText);

      if (parsedService.price >= 0) {
        services[*count] = parsedService;
        (*count)++;
      }
    }
  }

  fclose(file);
}

// Invoice Export (.txt format)
// Code structures are mine, but the bill graphic (fprintf) are AI generated.

static void formatInvoiceCurrency(char output[], size_t size,
                                  long long amount) {
  snprintf(output, size, "%lld VND", amount);
}

void exportInvoice(RepairOrder order, Customer customer) {
  char filename[160];
  char fallbackFilename[80];
  char timestamp[32];
  char priceText[32];
  long long total = 0;
  time_t now = time(NULL);
  char dateText[32];
  struct tm *nowInfo = localtime(&now);

  if (nowInfo == NULL ||
      strftime(timestamp, sizeof(timestamp), "%Y%m%d_%H%M%S", nowInfo) == 0) {
    strcpy(timestamp, "unknown_time");
  }

  snprintf(filename, sizeof(filename), "output/invoice_%s_%s.txt", order.orderId, timestamp);
  snprintf(fallbackFilename, sizeof(fallbackFilename), "invoice_%s_%s.txt", order.orderId, timestamp);

  FILE *file = fopen(filename, "w");
  if (file == NULL) {
    strcpy(filename, fallbackFilename);
    file = fopen(filename, "w");
    if (file == NULL) {
      printf("Error: Could not create invoice file %s.\n", filename);
      return;
    }
  }

  fprintf(file, "==========================================\n");
  fprintf(file, "=          VEHICLE REPAIR INVOICE        =\n");
  fprintf(file, "==========================================\n");

  // Timestamp
  if (nowInfo == NULL ||
      strftime(dateText, sizeof(dateText), "%d/%m/%Y %H:%M:%S", nowInfo) == 0) {
    strcpy(dateText, "Unknown");
  }

  fprintf(file, "Date: %s\n", dateText);
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
  fprintPaddedText(file, "Service Name", 30, 0);
  fprintf(file, " ");
  fprintPaddedText(file, "Price", 14, 1);
  fprintf(file, "\n");
  for (int i = 0; i < order.itemCount; i++) {
    formatInvoiceCurrency(priceText, sizeof(priceText), order.items[i].price);
    fprintPaddedText(file, order.items[i].name, 30, 0);
    fprintf(file, " ");
    fprintPaddedText(file, priceText, 14, 1);
    fprintf(file, "\n");
    total += order.items[i].price;
  }
  fprintf(file, "------------------------------------------\n");

  // Total
  formatInvoiceCurrency(priceText, sizeof(priceText), total);
  fprintf(file, "TOTAL AMOUNT: ");
  fprintPaddedText(file, priceText, 18, 1);
  fprintf(file, "\n");
  fprintf(file, "==========================================\n");
  fprintf(file, "=      Thank you for your business!      =\n");
  fprintf(file, "==========================================\n");

  fclose(file);
  printf("Invoice exported successfully: %s\n", filename);
}
