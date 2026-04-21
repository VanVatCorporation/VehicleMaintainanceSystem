// Tạo phiếu, thêm dịch vụ vào phiếu, cập nhật trạng thái, tính tổng tiền

#ifndef ORDER_H
#define ORDER_H

#include <time.h>

typedef enum {
    TIEP_NHAN,
    DANG_SUA,
    HOAN_THANH
} Status;

typedef struct RepairOrder {
    char orderId[10];
    char customerPhone[15];
    char symptom[100];3
    time_t createdDate;
    Status status;
}RepairOrder;

void createOrder(struct RepairOrder orders[], int *m, struct Customer customers[], int n);

#endif