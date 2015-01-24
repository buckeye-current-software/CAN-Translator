################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../source/LinkedList.c \
../source/MessageAVL.c \
../source/SignalAVL.c \
../source/avl.c \
../source/can_interceptor.c \
../source/datalogger.c \
../source/lib.c \
../source/main.c \
../source/parser.c \
../source/translator.c 

OBJS += \
./source/LinkedList.o \
./source/MessageAVL.o \
./source/SignalAVL.o \
./source/avl.o \
./source/can_interceptor.o \
./source/datalogger.o \
./source/lib.o \
./source/main.o \
./source/parser.o \
./source/translator.o 

C_DEPS += \
./source/LinkedList.d \
./source/MessageAVL.d \
./source/SignalAVL.d \
./source/avl.d \
./source/can_interceptor.d \
./source/datalogger.d \
./source/lib.d \
./source/main.d \
./source/parser.d \
./source/translator.d 


# Each subdirectory must supply rules for building sources it contributes
source/%.o: ../source/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -I"/home/cancorder/workspace/CAN_Translator/headers" -I/usr/include/mysql -I"/home/cancorder/workspace/CAN_Translator/include" -I"/home/cancorder/workspace/CAN_Translator/include/socketcan" -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


