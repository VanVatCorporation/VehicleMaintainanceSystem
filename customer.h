#ifndef CUSTOMER_H
#define CUSTOMER_H

// === Constants ===
#define MAX_CUSTOMERS 10000
#define ID_LENGTH 10
#define NAME_LENGTH 50
#define PHONE_LENGTH 15
#define PLATE_LENGTH 20
#define TYPE_LENGTH 20

// === Data structure ===
typedef struct {
    char customerId[10];
    char fullName[50];
    char phoneNumber[15];
    char carPlate[20];
    char carType[20];
    int orderCount;
} Customer;


// === Function prototypes ===

// Customer management functions
void addCustomer(Customer customers[], int *customerCount);
void updateCustomer(Customer customers[], int customerCount);
void searchCustomerByPhone(Customer customers[], int customerCount);
void searchCustomerByPlate(Customer customers[], int customerCount);
void displayCustomer(Customer customer);

// Finding functions
void readCustomersFromFile(Customer customers[], int *customerCount);
int findCustomerIndexByPhone(Customer customers[], int customerCount, char phoneNumber[]);
int findCustomerIndexByPlate(Customer customers[], int customerCount, char carPlate[]);


// Validation functions
int isValidPhoneNumber(char phoneNumber[]);
int isDuplicatePhoneNumber(Customer customers[], int customerCount, char phoneNumber[]);
void generateCustomerId(char customerId[], int customerCount);

#endif