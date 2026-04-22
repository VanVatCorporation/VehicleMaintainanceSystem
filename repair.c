#include <stdio.h>
#include <string.h>
#include <time.h>
#include "repair.h"
#include "customer.h"

// tạo mã phiếu
void generateOrderId(char orderId[], int counter){
    sprintf(orderId, "RO%06d", counter);
}

// tạo phiếu
RepairOrder createRepairOrder(int counter, Customer customers[], int customerCount){
    RepairOrder order;
    char inputPhone[11];

    // tạo ID
    generateOrderId(order.orderId, counter);

    // nhập sdt
    printf("Nhap so dien thoai: ");
    fgets(inputPhone, sizeof(inputPhone), stdin);
    inputPhone[strcspn(inputPhone, "\n")] = 0;

    strcpy(order.customerPhone, inputPhone);

    // tìm khách
    int index = findCustomerIndexByPhone(customers, customerCount, inputPhone);

    if(index == -1){
        printf("Khong tim thay khach hang!\n");
    } else {
        printf("Da tim thay khach:\n");
        displayCustomer(customers[index]);
        printf("Bien so: %s\n", customers[index].carPlate);
    }

    // nhập tình trạng xe
    printf("Nhap tinh trang xe: ");
    fgets(order.symptom, sizeof(order.symptom), stdin);
    order.symptom[strcspn(order.symptom, "\n")] = 0;

    // nhập số dịch vụ
    printf("Nhap so luong dich vu: ");
    scanf("%d", &order.itemCount);
    getchar();

    if(order.itemCount > MAX_ITEMS){
        order.itemCount = MAX_ITEMS;
    }

    for(int i = 0; i < order.itemCount; i++){
        printf("\nDich vu %d\n", i + 1);

        printf("Ten: ");
        fgets(order.items[i].name, sizeof(order.items[i].name), stdin);
        order.items[i].name[strcspn(order.items[i].name, "\n")] = 0;

        printf("Gia: ");
        scanf("%d", &order.items[i].price);
        getchar();
    }

    order.createdDate = time(NULL);

    order.status = TIEP_NHAN;// trạng thái mặc định

    return order;
}

// tính tổng tiền
int calculateTotal(RepairOrder order){
    int total = 0;
    for(int i = 0; i < order.itemCount; i++){
        total += order.items[i].price;
    }
    return total;
}

// update status
const char* getStatusText(Status status){
    switch(status){
        case TIEP_NHAN: return "Tiep nhan";
        case DANG_SUA: return "Dang sua";
        case HOAN_THANH: return "Hoan thanh";
        default: return "Khong ro";
    }
}

// cập nhật status
void updateStatus(RepairOrder *order){
    int choice;

    printf("\nCap nhat trang thai:\n");
    printf("1. Tiep nhan\n");
    printf("2. Dang sua\n");
    printf("3. Hoan thanh\n");
    printf("Chon: ");
    scanf("%d", &choice);

    switch(choice){
        case 1: order->status = TIEP_NHAN; break;
        case 2: order->status = DANG_SUA; break;
        case 3: order->status = HOAN_THANH; break;
        default: printf("Khong hop le!\n");
    }
}

// in phiếu
void printRepairOrder(RepairOrder order){
    printf("\n=== THONG TIN PHIEU ===\n");
    printf("Order ID: %s\n", order.orderId);
    printf("Phone: %s\n", order.customerPhone);
    printf("Symptom: %s\n", order.symptom);
    printf("Created: %s", ctime(&order.createdDate));
    printf("Status: %s\n", getStatusText(order.status));
    printf("\nDanh sach dich vu:\n");
    for(int i = 0; i < order.itemCount; i++){
        printf("%d. %s - %d VND\n",
            i + 1,
            order.items[i].name,
            order.items[i].price);
    }

    printf("Tong tien: %d VND\n", calculateTotal(order));
}
