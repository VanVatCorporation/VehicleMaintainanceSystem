#ifndef CUSTOMER_H
#define CUSTOMER_H

// === Hằng số cấu hình cho module khách hàng ===
#define MAX_CUSTOMERS 10000
#define ID_LENGTH 10
#define NAME_LENGTH 50
#define PHONE_LENGTH 11
#define PLATE_LENGTH 20
#define TYPE_LENGTH 31

// === Cấu trúc lưu thông tin một khách hàng ===
typedef struct {
    char customerId[ID_LENGTH];      // Mã khách hàng tự sinh, ví dụ CU000001.
    char fullName[NAME_LENGTH];      // Họ tên đầy đủ của khách hàng.
    char phoneNumber[PHONE_LENGTH];  // SĐT định danh duy nhất, không được trùng.
    char carPlate[PLATE_LENGTH];     // Biển số xe đã được chuẩn hóa.
    char carType[TYPE_LENGTH];       // Loại xe hoặc tên xe khách mang đến sửa.
    int orderCount;                  // Số phiếu sửa đã tạo cho khách hàng này.
} Customer;

// === Khai báo hàm dùng ở các file khác ===

// Nhóm hàm quản lý khách hàng.
void addCustomer(Customer customers[], int *customerCount);
void updateCustomer(Customer customers[], int *customerCount);
void searchCustomerByPhone(Customer customers[], int customerCount);
void searchCustomerByPlate(Customer customers[], int customerCount);
void displayCustomer(Customer customer);

// Nhóm hàm tìm vị trí khách hàng trong mảng.
int findCustomerIndexByPhone(Customer customers[], int customerCount, char phoneNumber[]);
int findCustomerIndexByPlate(Customer customers[], int customerCount, char carPlate[]);


// Nhóm hàm kiểm tra dữ liệu đầu vào của khách hàng.
int isValidFullName(char fullName[]);
int isValidPhoneNumber(char phoneNumber[]);
int isValidCarPlate(char carPlate[]);
int isDuplicatePhoneNumber(Customer customers[], int customerCount, char phoneNumber[]);
int isDuplicateCarPlate(Customer customers[], int customerCount, char carPlate[]);

// Nhóm hàm tiện ích riêng cho dữ liệu khách hàng.
void generateCustomerId(char customerId[], int customerCount);
void normalizeCarPlate(char carPlate[]);

#endif
