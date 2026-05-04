
#ifndef UTILS_H
#define UTILS_H
#define COLOR_TITLE   "\033[1;33m" // Vàng đậm cho tiêu đề
#define COLOR_HEADER  "\033[1;36m" // Xanh dương sáng cho bảng
#define COLOR_SUCCESS "\033[1;32m" // Xanh lá cho kết quả tìm thấy
#define COLOR_ERROR   "\033[1;31m" // Đỏ cho thông báo không thấy
#define COLOR_RESET   "\033[0m"    // Reset về màu mặc định
// Các hàm tiện ích in màu
void printColor(const char* color, const char* text);

#endif  