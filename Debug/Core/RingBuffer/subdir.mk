################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (10.3-2021.10)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Core/RingBuffer/RingBuffer.c 

OBJS += \
./Core/RingBuffer/RingBuffer.o 

C_DEPS += \
./Core/RingBuffer/RingBuffer.d 


# Each subdirectory must supply rules for building sources it contributes
Core/RingBuffer/%.o Core/RingBuffer/%.su: ../Core/RingBuffer/%.c Core/RingBuffer/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F429xx -c -I../Core/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Core-2f-RingBuffer

clean-Core-2f-RingBuffer:
	-$(RM) ./Core/RingBuffer/RingBuffer.d ./Core/RingBuffer/RingBuffer.o ./Core/RingBuffer/RingBuffer.su

.PHONY: clean-Core-2f-RingBuffer

