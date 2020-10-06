SET(CMAKE_SYSTEM_NAME Linux)
set(CMAKE_SYSTEM_PROCESSOR arm)

SET(COMPILER_TRIPLE "arm-linux-gnueabihf")

#SET(CMAKE_SYSROOT "${RPI_TOOLCHAIN_PATH}/${TRIPLE}/sysroot")
#LIST(APPEND CMAKE_PREFIX_PATH "/usr/arm-linux-gnueabihf")

SET(CMAKE_C_COMPILER "clang-9")
SET(CMAKE_CXX_COMPILER "clang++-9")

#SET(CMAKE_PREFIX_PATH /usr/arm-linux-gnueabihf)
#SET(CMAKE_INCLUDE_PATH "/usr/arm-linux-gnueabihf/include")

#SET(CMAKE_SYSROOT /usr/arm-linux-gnueabihf)

include_directories(
   /usr/arm-linux-gnueabihf/include 
   /usr/arm-linux-gnueabihf/include/c++/7/arm-linux-gnueabihf
   )

SET(CROSS_COMPILER_FLAGS "--target=${COMPILER_TRIPLE}")
SET(CMAKE_CXX_FLAGS_INIT "${CROSS_COMPILER_FLAGS}")
SET(CMAKE_C_FLAGS_INIT "${CROSS_COMPILER_FLAGS}")

#SET(CROSS_LINKER_FLAGS "-fuse-ld=lld -Wl,-z,notext -L/usr/arm-linux-gnueabihf/lib -lc++ -lc++abi -lpthread")
SET(CROSS_LINKER_FLAGS "-Wl,-z,notext -lpthread -L/usr/arm-linux-gnueabihf/lib -v")
SET(CMAKE_EXE_LINKER_FLAGS_INIT "${CROSS_LINKER_FLAGS}")
SET(CMAKE_SHARED_LINKER_FLAGS_INIT "${CROSS_LINKER_FLAGS}")

# Search for programs only in the build host directories
SET(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)

# Search for libraries and headers only in the target directories
SET(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
SET(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
SET(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)

SET(STDLIB "libstdc++")

add_definitions(-DRPI)

