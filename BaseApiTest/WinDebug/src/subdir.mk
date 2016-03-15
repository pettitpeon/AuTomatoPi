################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/BaComTest.cpp \
../src/BaCoreTest.cpp \
../src/BaGpioTest.cpp \
../src/BaIniParseTest.cpp \
../src/BaLogTest.cpp \
../src/BaseApiTest.cpp \
../src/ProgressListener.cpp \
../src/TestTemplate.cpp 

OBJS += \
./src/BaComTest.o \
./src/BaCoreTest.o \
./src/BaGpioTest.o \
./src/BaIniParseTest.o \
./src/BaLogTest.o \
./src/BaseApiTest.o \
./src/ProgressListener.o \
./src/TestTemplate.o 

CPP_DEPS += \
./src/BaComTest.d \
./src/BaCoreTest.d \
./src/BaGpioTest.d \
./src/BaIniParseTest.d \
./src/BaLogTest.d \
./src/BaseApiTest.d \
./src/ProgressListener.d \
./src/TestTemplate.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: Cross G++ Compiler'
	g++ -std=c++0x -I"D:\Programming\Repos\rpiapi\cppunit-1.12.1" -I"D:\Programming\Repos\rpiapi\BaseApi" -I"D:\Programming\Repos\rpiapi\BaseApi\src" -I"D:\Programming\Repos\rpiapi\cppunit-1.12.1\include" -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


