cmake_minimum_required(VERSION 3.10)
project(OS_LAB_3)

set(CMAKE_CXX_STANDARD 17)

# Основной исполняемый файл
add_executable(OS_LAB_3 src/main.cpp src/marker.cpp)

# Тесты
add_executable(tests 
    test/test.cpp 
    src/marker.cpp
    include/catch_amalgamated.cpp  
)

# Подключение заголовков
target_include_directories(tests PRIVATE 
    ${CMAKE_SOURCE_DIR}/include  # Для catch_amalgamated.hpp
    ${CMAKE_SOURCE_DIR}/src      # Для marker.h
)