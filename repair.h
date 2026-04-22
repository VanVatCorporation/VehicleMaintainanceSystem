// Tạo phiếu, thêm dịch vụ vào phiếu, cập nhật trạng thái, tính tổng
// tiền

#ifndef REPAIR_H
#define REPAIR_H

#include <time.h>
#include "customer.h"

#define MAX_ITEMS 10

// trạng thái sửa
typedef enum {
    TIEP_NHAN,
    DANG_SUA,
    HOAN_THANH
} Status;

// dịch vụ
typedef struct {
    char name[50];
    int price;
} RepairItem;

// phiếu sửa chữa
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

#endif