
cmake_minimum_required(VERSION 3.10)


project(MotorbikeShop C)


set(CMAKE_C_STANDARD 99)
add_compile_options(-Wall -g)

set(SOURCES 
    main.c 
    customer.c 
    customer_validation.c 
    repair.c 
    fileio.c 
    utils.c 
    report.c
)


add_executable(motorbike_shop ${SOURCES})
