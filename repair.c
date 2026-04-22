// Tạo phiếu, thêm dịch vụ vào phiếu, cập nhật trạng thái, tính tổng tiền

#include <stdio.h>
<<<<<<< Updated upstream
#include <time.h>
#include <string.h>
#include "repair.h"
#include "customer.h"

void generateOrderId(char orderId[], int counter){
	sprintf(orderId, "RO%06d", counter);
}

int main(){
	RepairOrder order;
	
	generateOrderId(order.orderId, 1); // mỗi lần tạo phiếu sẽ tăng lên 1

    printf("Nhap so dien thoai: ");
    fgets(inputPhone, sizeof(inputPhone), stdin);
    inputPhone[strcspn(inputPhone, "\n")] = 0;
	
	printf("Nhap tinh trang xe:");
	fgets(order.symptom, sizeof(order.symptom), stdin);
	order.symptom[strcspn(order.symptom, "\n")] = 0;
	
	order.createdDate=time(NULL); // thời gian thực khi tạo phiếu 
	
	printf("\n=== THONG TIN PHIEU ===\n");
	printf("Order ID: %s\n", order.orderId);
    printf("Phone: %s\n", )
	printf("Symptom: %s\n", order.symptom);
	printf("Created: %s", ctime(&order.createdDate));
	
	return 0;
=======
#include <string.h>
#include <time.h>
#include "repair.h"

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

    // copy vào order
    strcpy(order.customerPhone, inputPhone);

    // tìm khách
    int index = findCustomerIndexByPhone(customers, customerCount, inputPhone);

    if(index == -1){
        printf("Khong tim thay khach hang!\n");
    } else {
        printf("Da tim thay khach:\n");
        displayCustomer(customers[index]);

        // (nếu muốn lấy biển số)
        printf("Bien so: %s\n", customers[index].carPlate);
    }

    // nhập tình trạng
    printf("Nhap tinh trang xe: ");
    fgets(order.symptom, sizeof(order.symptom), stdin);
    order.symptom[strcspn(order.symptom, "\n")] = 0;

    // nhập số lượng dịch vụ
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

    return order;
}

// tổng tiền
int calculateTotal(RepairOrder order){
    int total = 0;
    for(int i = 0; i < order.itemCount; i++){
        total += order.items[i].price;
    }
    return total;
}

// in phiếu
void printRepairOrder(RepairOrder order){
    printf("\n=== THONG TIN PHIEU ===\n");
    printf("Order ID: %s\n", order.orderId);
    printf("Phone: %s\n", order.customerPhone);
    printf("Symptom: %s\n", order.symptom);
    printf("Created: %s", ctime(&order.createdDate));

    printf("\nDanh sach dich vu:\n");
    for(int i = 0; i < order.itemCount; i++){
        printf("%d. %s - %d VND\n",
            i + 1,
            order.items[i].name,
            order.items[i].price);
    }

    printf("Tong tien: %d VND\n", calculateTotal(order));
>>>>>>> Stashed changes
}