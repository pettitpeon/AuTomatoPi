################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/dictionary.c 

CPP_SRCS += \
../src/BaCom.cpp \
../src/BaCore.cpp \
../src/BaGpio.cpp \
../src/BaIniParse.cpp \
../src/BaLog.cpp \
../src/CBaGpio.cpp \
../src/CBaLog.cpp 

OBJS += \
./src/BaCom.o \
./src/BaCore.o \
./src/BaGpio.o \
./src/BaIniParse.o \
./src/BaLog.o \
./src/CBaGpio.o \
./src/CBaLog.o \
./src/dictionary.o 

C_DEPS += \
./src/dictionary.d 

CPP_DEPS += \
./src/BaCom.d \
./src/BaCore.d \
./src/BaGpio.d \
./src/BaIniParse.d \
./src/BaLog.d \
./src/CBaGpio.d \
./src/CBaLog.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: Cross G++ Compiler'
	g++ -std=c++0x -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

src/%.o: ../src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	gcc -std=c11 -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


