#ifndef UTILS_H
#define UTILS_H


#define ANSI_COLOR_RESET   "\x1b[0m"
#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_YELLOW  "\x1b[33m"
#define ANSI_COLOR_CYAN    "\x1b[36m"

// Khai báo prototype hàm (để file khác có thể gọi được)
void printColor(const char* color, const char* text);
void printInfo(const char* message);
void printSuccess(const char* message);
void printError(const char* message);
void printSectionTitle(const char* title);
void printBoxTitle(const char* title, int width);
void printMenuOption(int option, const char* label);
void printMenuBack(void);
int readLine(char buffer[], int size);
int readInt(int* value);

#endif
