// Tạo phiếu, thêm dịch vụ vào phiếu, cập nhật trạng thái, tính tổng
// tiền

#include <stdio.h>
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
}