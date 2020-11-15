cmake_minimum_required(VERSION 3.10)  # CMake version check
project(automatopi)                   # Create project
set(CMAKE_CXX_STANDARD 17)            # Enable c++14 standard

list(APPEND CROSS_LINKER_FLAGS "-v")

find_package(Threads)

add_subdirectory(BaseApi)
add_subdirectory(AuTomatoes)
add_subdirectory(cppunit-1.12.1)
