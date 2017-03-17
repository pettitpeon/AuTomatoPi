################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/impl/CBaIpcRegistry.cpp \
../src/impl/CBaIpcSvr.cpp \
../src/impl/CBaLog.cpp \
../src/impl/CBaMsg.cpp \
../src/impl/CBaSwOsci.cpp \
../src/impl/CCtrlPT1.cpp 

OBJS += \
./src/impl/CBaIpcRegistry.o \
./src/impl/CBaIpcSvr.o \
./src/impl/CBaLog.o \
./src/impl/CBaMsg.o \
./src/impl/CBaSwOsci.o \
./src/impl/CCtrlPT1.o 

CPP_DEPS += \
./src/impl/CBaIpcRegistry.d \
./src/impl/CBaIpcSvr.d \
./src/impl/CBaLog.d \
./src/impl/CBaMsg.d \
./src/impl/CBaSwOsci.d \
./src/impl/CCtrlPT1.d 


# Each subdirectory must supply rules for building sources it contributes
src/impl/%.o: ../src/impl/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: Cross G++ Compiler'
	arm-linux-gnueabihf-g++ -std=c++0x -D_USE_MATH_DEFINES=1 -D_NDEBUG=1 -I"D:\Programming\Repos\rpiapi\BaseApi\src\impl" -I"D:\Programming\Repos\rpiapi\BaseApi\src" -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


