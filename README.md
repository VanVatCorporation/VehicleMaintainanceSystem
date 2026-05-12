# Vehicle Repair Shop Management System

Chương trình quản lý tiệm sửa xe chạy trên terminal, viết bằng ngôn ngữ C. Project mô phỏng quy trình làm việc của một tiệm sửa xe nhỏ: quản lý khách hàng, tạo phiếu sửa chữa, quản lý dịch vụ, cập nhật trạng thái sửa xe, lưu dữ liệu vào file và xuất hóa đơn.

Project được xây dựng theo yêu cầu CLB F-Code - Train-C: không dùng giao diện đồ họa, không dùng cơ sở dữ liệu, không dùng thư viện ngoài C chuẩn.

## Trạng thái project

- Đã hoàn thiện các chức năng bắt buộc.
- Có thêm chức năng nâng cao: thống kê doanh thu, thống kê dịch vụ bán chạy, xuất hóa đơn `.txt`, giao diện console có màu ANSI.
- Dữ liệu được tự động lưu sau các thao tác thay đổi để hạn chế mất dữ liệu khi thoát chương trình.

## Tính năng chính

### Quản lý khách hàng

- Thêm khách hàng mới với họ tên, số điện thoại, biển số xe và loại xe.
- Số điện thoại là định danh duy nhất, không cho phép trùng.
- Cập nhật thông tin khách hàng: họ tên, biển số xe, loại xe.
- Không cho sửa số điện thoại vì đây là khóa định danh khách hàng.
- Tìm kiếm khách hàng theo số điện thoại.
- Tìm kiếm khách hàng theo biển số xe.
- Xem danh sách toàn bộ khách hàng.

### Quản lý phiếu sửa chữa

- Tạo phiếu sửa chữa mới cho khách hàng đã tồn tại.
- Sinh mã phiếu tự động theo dạng `RO000001`.
- Nhập triệu chứng xe.
- Chọn dịch vụ từ danh mục dịch vụ.
- Tối đa 10 dịch vụ trong một phiếu sửa chữa.
- Tự động tính tổng tiền từ các dịch vụ đã chọn.
- Xem chi tiết phiếu sửa chữa kèm thông tin khách hàng.
- Xem danh sách toàn bộ phiếu sửa chữa.
- Lọc phiếu sửa chữa theo trạng thái.
- Xem lịch sử sửa chữa của một khách hàng theo số điện thoại.
- Tìm kiếm phiếu theo mã phiếu hoặc biển số xe.

### Quản lý trạng thái

Phiếu sửa chữa có 3 trạng thái:

1. `Received` - đã tiếp nhận.
2. `Under repaired` - đang sửa.
3. `Complete` - hoàn thành.

Chương trình chỉ cho phép cập nhật trạng thái theo đúng chiều:

```text
Received -> Under repaired -> Complete
```

Không cho phép nhảy trạng thái hoặc đảo ngược trạng thái.

### Quản lý dịch vụ

- Thêm dịch vụ mới vào danh mục.
- Sinh mã dịch vụ tự động theo dạng `SV000001`.
- Cập nhật tên dịch vụ và đơn giá.
- Xem danh sách toàn bộ dịch vụ.
- Dịch vụ được lưu vào file riêng để lần chạy sau vẫn sử dụng lại được.

### Báo cáo và hóa đơn

- Thống kê doanh thu trong ngày.
- Thống kê dịch vụ được sử dụng nhiều nhất.
- Tự động xuất hóa đơn `.txt` khi phiếu được cập nhật sang trạng thái `Complete`.
- Hóa đơn gồm thời gian, mã phiếu, thông tin khách hàng, thông tin xe, triệu chứng, danh sách dịch vụ và tổng tiền.

### Giao diện console

- Menu được chia theo từng nhóm chức năng.
- Bảng hiển thị được căn cột cố định để tránh lệch layout khi có tiếng Việt hoặc số tiền dài.
- Thông báo thành công, lỗi và thông tin được in màu bằng ANSI escape code.
- Trạng thái phiếu có màu riêng để dễ quan sát khi demo.

## Cấu trúc project

| File / thư mục | Vai trò |
| --- | --- |
| `main.c` | Khởi động chương trình, load dữ liệu, hiển thị menu chính và điều hướng chức năng. |
| `customer.h` / `customer.c` | Khai báo cấu trúc khách hàng, thêm/sửa/tìm kiếm/hiển thị khách hàng. |
| `customer_validation.c` | Validate họ tên, số điện thoại và biển số xe. |
| `repair.h` / `repair.c` | Quản lý phiếu sửa chữa, dịch vụ, trạng thái và bảng hiển thị phiếu. |
| `fileio.h` / `fileio.c` | Đọc/ghi dữ liệu CSV và xuất hóa đơn `.txt`. |
| `report.h` / `report.c` | Thống kê doanh thu, thống kê dịch vụ phổ biến và liệt kê khách hàng. |
| `utils.h` / `utils.c` | Hàm dùng chung: đọc input an toàn, trim chuỗi, in menu, in bảng, in màu. |
| `data/` | Thư mục lưu dữ liệu khi chương trình chạy. |
| `build.bat` | Script build nhanh trên Windows bằng GCC. |
| `Makefile` | Build project bằng `make`. |
| `CMakeLists.txt` | Build project bằng CMake. |

## Dữ liệu lưu trữ

Chương trình lưu dữ liệu dưới dạng CSV:

| File | Nội dung |
| --- | --- |
| `data/customers.csv` | Danh sách khách hàng. |
| `data/repair_orders.csv` | Danh sách phiếu sửa chữa. |
| `data/services.csv` | Danh mục dịch vụ. |
| `invoice_ROxxxxxx.txt` | Hóa đơn được xuất khi phiếu hoàn thành. |

Các file dữ liệu được load lại khi khởi động chương trình. Sau mỗi thao tác thêm/sửa/cập nhật quan trọng, chương trình tự động ghi lại dữ liệu xuống file.

## Yêu cầu môi trường

- GCC hoặc MinGW-w64 nếu chạy trên Windows.
- Không cần cài thư viện ngoài.
- CMake hoặc Make là tùy chọn, chỉ dùng nếu muốn build theo các công cụ đó.

## Cách build và chạy

### Windows

Build bằng script có sẵn:

```powershell
.\build.bat
.\motorbike_shop.exe
```

Hoặc build thủ công bằng GCC:

```powershell
gcc -Wall -Wextra -g -o motorbike_shop.exe main.c customer.c customer_validation.c repair.c fileio.c utils.c report.c
.\motorbike_shop.exe
```

### Linux / macOS

Build bằng Makefile:

```bash
make
./motorbike_shop
```

Hoặc build thủ công:

```bash
gcc -Wall -Wextra -g -o motorbike_shop main.c customer.c customer_validation.c repair.c fileio.c utils.c report.c
./motorbike_shop
```

### CMake

```bash
cmake -S . -B build
cmake --build build
```

Sau khi build xong, chạy file thực thi trong thư mục `build`.

## Luồng demo gợi ý

1. Mở chương trình và kiểm tra số lượng khách hàng, phiếu sửa chữa, dịch vụ đã load.
2. Vào `Repair order management` -> `Add service` để thêm một vài dịch vụ mẫu.
3. Vào `Customer management` -> `Add customer` để thêm khách hàng mới.
4. Tạo phiếu sửa chữa cho khách hàng vừa thêm.
5. Chọn dịch vụ từ danh mục và kiểm tra tổng tiền tự động.
6. Thử cập nhật trạng thái sai thứ tự để thấy chương trình chặn lỗi.
7. Cập nhật đúng thứ tự đến `Complete`.
8. Kiểm tra hóa đơn `.txt` được xuất tự động.
9. Xem lịch sử sửa chữa theo số điện thoại.
10. Chạy báo cáo doanh thu và dịch vụ phổ biến.

## Các rule validate quan trọng

- Số điện thoại phải có đúng 10 chữ số và bắt đầu bằng `0`.
- Số điện thoại không được trùng giữa các khách hàng.
- Họ tên không được rỗng và không được chứa chữ số.
- Biển số xe được chuẩn hóa trước khi lưu, ví dụ `30A-123.45` và `30a 12345` sẽ được đưa về cùng một dạng để so sánh.
- Biển số xe không được trùng giữa các khách hàng.
- Giá dịch vụ không được âm.
- Không thể tạo phiếu sửa chữa nếu chưa có dịch vụ trong danh mục.
- Không thể cập nhật trạng thái phiếu ngược chiều hoặc nhảy cóc.

## Ghi chú kỹ thuật

- Project dùng mảng tĩnh theo yêu cầu môn học: tối đa 10.000 khách hàng và 50.000 phiếu sửa chữa.
- `RepairOrder` chứa mảng `RepairItem`, đúng yêu cầu dùng struct lồng nhau.
- CSV được xử lý có hỗ trợ dấu phẩy và dấu nháy kép trong dữ liệu text.
- Các hàm in bảng dùng logic tính độ rộng UTF-8 để hạn chế lệch cột khi có tiếng Việt.
- `main.c` chỉ giữ vai trò điều hướng chính; logic khách hàng, phiếu sửa, file và báo cáo được tách sang module riêng.

## Checklist trước khi nộp

- Build không warning nghiêm trọng với `gcc -Wall -Wextra`.
- Chạy được chương trình từ terminal.
- Thêm/sửa/tìm kiếm khách hàng hoạt động đúng.
- Chặn trùng số điện thoại và trùng biển số.
- Tạo phiếu sửa chữa và tính tổng tiền đúng.
- Cập nhật trạng thái đúng quy trình.
- Lưu file và load lại dữ liệu sau khi mở chương trình.
- Xuất hóa đơn khi phiếu hoàn thành.
- Báo cáo doanh thu và dịch vụ phổ biến hoạt động.

## License

Project sử dụng license trong file `LICENSE`.
