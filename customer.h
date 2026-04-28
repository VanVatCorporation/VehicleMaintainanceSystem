#ifndef CUSTOMER_H
#define CUSTOMER_H

// === Constants ===
#define MAX_CUSTOMERS 10000
#define ID_LENGTH 10
#define NAME_LENGTH 50
#define PHONE_LENGTH 11
#define PLATE_LENGTH 20
#define TYPE_LENGTH 31

// === Data structure ===
typedef struct {
    char customerId[ID_LENGTH];
    char fullName[NAME_LENGTH];
    char phoneNumber[PHONE_LENGTH];
    char carPlate[PLATE_LENGTH];
    char carType[TYPE_LENGTH];
    int orderCount;
} Customer;

// === Function prototypes ===

// Customer management functions
void addCustomer(Customer customers[], int *customerCount);
void updateCustomer(Customer customers[], int *customerCount);
void searchCustomerByPhone(Customer customers[], int customerCount);
void searchCustomerByPlate(Customer customers[], int customerCount);
void displayCustomer(Customer customer);

// Finding functions
int findCustomerIndexByPhone(Customer customers[], int customerCount, char phoneNumber[]);
int findCustomerIndexByPlate(Customer customers[], int customerCount, char carPlate[]);


// Validation functions
int isValidFullName(char fullName[]);
int isValidPhoneNumber(char phoneNumber[]);
int isValidCarPlate(char carPlate[]);
int isDuplicatePhoneNumber(Customer customers[], int customerCount, char phoneNumber[]);
int isDuplicateCarPlate(Customer customers[], int customerCount, char carPlate[]);

// Utility functions
void generateCustomerId(char customerId[], int customerCount);
void normalizeCarPlate(char carPlate[]);

#endif
