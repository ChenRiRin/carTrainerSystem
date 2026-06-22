cmake_minimum_required(VERSION 3.20)
project(carTrainerSystem)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

enable_testing()

find_package(FLTK REQUIRED)
find_package(SQLite3 REQUIRED)

add_subdirectory(src)
add_subdirectory(test)
