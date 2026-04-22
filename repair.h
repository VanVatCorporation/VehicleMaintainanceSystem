<<<<<<< Updated upstream
// Tạo phiếu, thêm dịch vụ vào phiếu, cập nhật trạng thái, tính tổng tiền
=======
// Tạo phiếu, thêm dịch vụ vào phiếu, cập nhật trạng thái, tính tổng
// tiền
>>>>>>> Stashed changes

#ifndef REPAIR_H
#define REPAIR_H

#include <time.h>
<<<<<<< Updated upstream

typedef enum {
    TIEP_NHAN,
    DANG_SUA,
    HOAN_THANH
} Status;
=======
#include "customer.h"

#define MAX_ITEMS 10

typedef struct RepairItem {
    char name[50];
    int price;
} RepairItem;
>>>>>>> Stashed changes

typedef struct RepairOrder {
    char orderId[10];
    char customerPhone[11];
<<<<<<< Updated upstream
    char symptom[100];3
    time_t createdDate;
    Status status;
}RepairOrder;

void createOrder(struct RepairOrder orders[], int *m, struct Customer customers[], int n);
=======
    char symptom[100];
    time_t createdDate;
    RepairItem items[MAX_ITEMS];
    int itemCount;
} RepairOrder;

// Prototype
void generateOrderId(char orderId[], int counter);
RepairOrder createRepairOrder(int counter, Customer customers[], int customerCount);
void printRepairOrder(RepairOrder order);
int calculateTotal(RepairOrder order);
>>>>>>> Stashed changes

#endif