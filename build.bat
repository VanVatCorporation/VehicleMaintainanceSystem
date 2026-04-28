@echo off
gcc -Wall -g -o motorbike_shop.exe main.c customer.c customer_validation.c repair.c fileio.c utils.c report.c
if errorlevel 1 (
    echo Build failed.
    exit /b 1
)
echo Build successful: motorbike_shop.exe
