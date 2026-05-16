# Vehicle Repair Shop Management System

A terminal-based repair shop management system written in C. The application is designed for a small vehicle or motorbike repair shop that needs to manage customers, repair orders, service catalogs, order status, invoices, and basic business reports without using a database or external dependencies.

This project was built as a CLB F-Code Train-C assignment and follows the required constraints: standard C, file-based persistence, modular source files, and a console-only user interface.

## Project Status

- Core required features are implemented.
- Advanced features are included: daily revenue report, popular service report, invoice export, and colored console output.
- Data is loaded on startup and automatically saved after data-changing operations.
- The codebase is split into focused modules for customers, repair orders, file I/O, reports, and shared utilities.

## Key Features

### Customer Management

- Add a new customer with full name, phone number, vehicle plate, and vehicle type.
- Use phone number as the unique customer identifier.
- Prevent duplicate phone numbers and duplicate vehicle plates.
- Update customer name, vehicle plate, and vehicle type.
- Keep phone number immutable to preserve customer identity.
- Search customers by phone number.
- Search customers by vehicle plate.
- List all customers.

### Repair Order Management

- Create repair orders with automatically generated IDs such as `RO000001`.
- Attach repair orders to customers by phone number.
- Record vehicle symptoms or repair notes.
- Select repair services from the service catalog.
- Support up to 10 service items per repair order.
- Automatically calculate the total amount for each repair order.
- View detailed repair order information with customer data.
- List all repair orders.
- Filter repair orders by status.
- View a customer's repair history by phone number.
- Search repair orders by order ID or vehicle plate.

### Service Catalog

- Add new repair services.
- Generate service IDs such as `SV000001`.
- Update service name and price.
- List all available services.
- Persist the service catalog to a separate CSV file.

### Status Workflow

Each repair order follows a strict forward-only workflow:

```text
Received -> Under repaired -> Complete
```

The application prevents invalid transitions such as skipping a step or moving backward from `Complete` to a previous status.

### Reports and Invoices

- Calculate daily revenue from completed repair orders.
- Show the most frequently used services.
- Automatically export a `.txt` invoice when an order is marked as `Complete`.
- Include customer information, vehicle information, symptoms, service items, and total amount in the invoice.

### Console User Interface

- Menu-driven terminal interface.
- Colored status and feedback messages using ANSI escape codes.
- Fixed-width tables for customers, services, repair orders, and invoices.
- UTF-8-aware text padding to reduce table misalignment with Vietnamese text.

## Technical Highlights

- Written in C with no third-party libraries.
- Uses static arrays as required by the assignment.
- Supports up to 10,000 customers.
- Supports up to 50,000 repair orders.
- Supports up to 100 services.
- Uses nested structs: `RepairOrder` contains an array of `RepairItem`.
- Uses CSV files for persistence.
- CSV writer and parser support quoted fields, commas, and escaped double quotes.
- Automatically saves customer, repair order, and service data after important mutations.
- Handles `Ctrl+C` / termination signals by saving data before exit.

## Project Structure

| Path | Responsibility |
| --- | --- |
| `main.c` | Application entry point, menu flow, startup loading, auto-save coordination, and signal handling. |
| `customer.h` / `customer.c` | Customer data structure, customer CRUD operations, search, and display logic. |
| `customer_validation.c` | Validation for full name, phone number, and vehicle plate. |
| `repair.h` / `repair.c` | Repair order structure, service catalog, order status workflow, totals, and order UI. |
| `fileio.h` / `fileio.c` | CSV load/save logic and invoice export. |
| `report.h` / `report.c` | Daily revenue, popular service statistics, and customer listing support. |
| `utils.h` / `utils.c` | Shared input handling, string trimming, table padding, menus, and colored messages. |
| `data/` | Runtime data directory for CSV files. |
| `build.bat` | Windows build script using GCC. |
| `Makefile` | Make-based build configuration. |
| `CMakeLists.txt` | CMake build configuration. |

## Data Persistence

The application stores runtime data in CSV files:

| File | Description |
| --- | --- |
| `data/customers.csv` | Customer records. |
| `data/repair_orders.csv` | Repair order records and nested service items. |
| `data/services.csv` | Service catalog records. |
| `invoice_ROxxxxxx.txt` | Exported invoice for a completed order. |

Data is loaded when the program starts. After operations that change data, the application saves the latest customer, repair order, and service state back to disk.

## Requirements

- GCC or MinGW-w64 on Windows.
- A terminal that supports basic ANSI colors for the best UI experience.
- `make` or CMake is optional.
- No database server, network connection, or external C library is required.

## Build and Run

### Windows

Using the provided build script:

```powershell
.\build.bat
.\motorbike_shop.exe
```

Manual GCC build:

```powershell
gcc -Wall -Wextra -g -o motorbike_shop.exe main.c customer.c customer_validation.c repair.c fileio.c utils.c report.c
.\motorbike_shop.exe
```

### Linux / macOS

Using Make:

```bash
make
./motorbike_shop
```

Manual GCC build:

```bash
gcc -Wall -Wextra -g -o motorbike_shop main.c customer.c customer_validation.c repair.c fileio.c utils.c report.c
./motorbike_shop
```

### CMake

```bash
cmake -S . -B build
cmake --build build
```

Run the generated executable from the `build` directory.

## Suggested Demo Flow

1. Start the application and verify that existing customers, repair orders, and services are loaded.
2. Add a few services from `Repair order management -> Add service`.
3. Add a customer from `Customer management -> Add customer`.
4. Create a repair order for that customer.
5. Select services from the catalog and verify the calculated total.
6. Try an invalid status transition to confirm that the workflow is protected.
7. Update the status in the valid order until it reaches `Complete`.
8. Check the generated invoice file.
9. View repair history by phone number.
10. Run daily revenue and popular service reports.

## Validation and Business Rules

- Phone number must contain exactly 10 digits.
- Phone number must start with `0`.
- Phone number must be unique.
- Customer full name cannot be empty or contain digits.
- Vehicle plate is normalized before validation and comparison.
- Vehicle plate must be unique.
- Service price cannot be negative.
- A repair order cannot be created without at least one service item.
- Repair order status can only move forward one step at a time.
- Completed orders are used for revenue and popular service reports.

## Quality Checklist

- The program builds with GCC using `-Wall -Wextra`.
- The application runs entirely in the terminal.
- Customer add, update, search, and list flows work correctly.
- Duplicate phone numbers and duplicate vehicle plates are rejected.
- Repair orders calculate totals correctly.
- Status transitions follow the required workflow.
- Data is saved and loaded through files.
- Invoices are exported when orders are completed.
- Reports work with completed orders.
- Console tables remain aligned with long numbers and UTF-8 text.

## License

This project is distributed under the license included in `LICENSE`.
