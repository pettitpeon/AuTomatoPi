################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/Extras/dictionary.c \
../src/Extras/iniparser.c 

OBJS += \
./src/Extras/dictionary.o \
./src/Extras/iniparser.o 

C_DEPS += \
./src/Extras/dictionary.d \
./src/Extras/iniparser.d 


# Each subdirectory must supply rules for building sources it contributes
src/Extras/%.o: ../src/Extras/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	arm-linux-gnueabihf-gcc -I"D:\Programming\Repos\rpiapi\BaseApi\src" -I"D:\Programming\Repos\rpiapi\cppunit-1.12.1\include" -I"D:\Programming\Repos\rpiapi\BaseApi" -O3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


