cmake_minimum_required(VERSION 3.22)

project(test-libcrypto VERSION 1.0)

find_package(OpenSSL REQUIRED)

add_executable(${PROJECT_NAME} "main.c")
