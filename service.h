#ifndef SERVICE_H
#define SERVICE_H

#define SERVICE_ID_LENGTH 20
#define MAX_SERVICES 100

typedef struct {
    char serviceId[SERVICE_ID_LENGTH];
    char name[50];
    int price;
} Service;

//======PROTOTYPE======

// Service management
void addService(Service services[], int *serviceCount);
void updateService(Service services[], int serviceCount);
void listServices(Service services[], int serviceCount);

// Utility
int findServiceIndexById(Service services[], int count, char serviceId[]);
void generateServiceId(char serviceId[], int counter);

#endif