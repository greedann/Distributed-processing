cmake_minimum_required(VERSION 3.5.0)
project(lab1 VERSION 0.1.0 LANGUAGES C)

include(CTest)
enable_testing()

add_executable(lab1 main.c hashmap.c hashmap.h)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
