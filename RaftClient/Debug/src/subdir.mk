################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/CANBus.cpp \
../src/Client.cpp \
../src/ClientRequest.cpp \
../src/Communication.cpp \
../src/IPManager.cpp \
../src/RaftClient.cpp \
../src/RaftUtils.cpp \
../src/ServoMotor.cpp \
../src/TimeManager.cpp \
../src/Tracer.cpp 

C_SRCS += \
../src/candump.c \
../src/cangen.c \
../src/canplayer.c \
../src/cansend.c \
../src/cansniffer.c \
../src/lib.c 

OBJS += \
./src/CANBus.o \
./src/Client.o \
./src/ClientRequest.o \
./src/Communication.o \
./src/IPManager.o \
./src/RaftClient.o \
./src/RaftUtils.o \
./src/ServoMotor.o \
./src/TimeManager.o \
./src/Tracer.o \
./src/candump.o \
./src/cangen.o \
./src/canplayer.o \
./src/cansend.o \
./src/cansniffer.o \
./src/lib.o 

CPP_DEPS += \
./src/CANBus.d \
./src/Client.d \
./src/ClientRequest.d \
./src/Communication.d \
./src/IPManager.d \
./src/RaftClient.d \
./src/RaftUtils.d \
./src/ServoMotor.d \
./src/TimeManager.d \
./src/Tracer.d 

C_DEPS += \
./src/candump.d \
./src/cangen.d \
./src/canplayer.d \
./src/cansend.d \
./src/cansniffer.d \
./src/lib.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

src/%.o: ../src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


