################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/BaCom.cpp \
../src/BaCore.cpp \
../src/BaGpio.cpp \
../src/BaIniParse.cpp \
../src/BaIpc.cpp \
../src/BaLog.cpp \
../src/BaMsg.cpp \
../src/BaProc.cpp \
../src/BaRPi.cpp \
../src/BaSwOsci.cpp \
../src/BaseApi.cpp \
../src/CtrlPT1.cpp 

OBJS += \
./src/BaCom.o \
./src/BaCore.o \
./src/BaGpio.o \
./src/BaIniParse.o \
./src/BaIpc.o \
./src/BaLog.o \
./src/BaMsg.o \
./src/BaProc.o \
./src/BaRPi.o \
./src/BaSwOsci.o \
./src/BaseApi.o \
./src/CtrlPT1.o 

CPP_DEPS += \
./src/BaCom.d \
./src/BaCore.d \
./src/BaGpio.d \
./src/BaIniParse.d \
./src/BaIpc.d \
./src/BaLog.d \
./src/BaMsg.d \
./src/BaProc.d \
./src/BaRPi.d \
./src/BaSwOsci.d \
./src/BaseApi.d \
./src/CtrlPT1.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: Cross G++ Compiler'
	arm-linux-gnueabihf-g++ -std=c++0x -D_USE_MATH_DEFINES=1 -D_NDEBUG=1 -I"D:\Programming\Repos\rpiapi\BaseApi\src\impl" -I"D:\Programming\Repos\rpiapi\BaseApi\src" -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


