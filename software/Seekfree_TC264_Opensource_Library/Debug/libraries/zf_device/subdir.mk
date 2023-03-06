################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../libraries/zf_device/zf_device_absolute_encoder.c \
../libraries/zf_device/zf_device_bluetooth_ch9141.c \
../libraries/zf_device/zf_device_camera.c \
../libraries/zf_device/zf_device_dl1a.c \
../libraries/zf_device/zf_device_icm20602.c \
../libraries/zf_device/zf_device_imu660ra.c \
../libraries/zf_device/zf_device_imu963ra.c \
../libraries/zf_device/zf_device_key.c \
../libraries/zf_device/zf_device_mpu6050.c \
../libraries/zf_device/zf_device_mt9v03x.c \
../libraries/zf_device/zf_device_oled.c \
../libraries/zf_device/zf_device_tft180.c \
../libraries/zf_device/zf_device_tsl1401.c \
../libraries/zf_device/zf_device_type.c \
../libraries/zf_device/zf_device_virtual_oscilloscope.c \
../libraries/zf_device/zf_device_wifi_uart.c \
../libraries/zf_device/zf_device_wireless_uart.c 

OBJS += \
./libraries/zf_device/zf_device_absolute_encoder.o \
./libraries/zf_device/zf_device_bluetooth_ch9141.o \
./libraries/zf_device/zf_device_camera.o \
./libraries/zf_device/zf_device_dl1a.o \
./libraries/zf_device/zf_device_icm20602.o \
./libraries/zf_device/zf_device_imu660ra.o \
./libraries/zf_device/zf_device_imu963ra.o \
./libraries/zf_device/zf_device_key.o \
./libraries/zf_device/zf_device_mpu6050.o \
./libraries/zf_device/zf_device_mt9v03x.o \
./libraries/zf_device/zf_device_oled.o \
./libraries/zf_device/zf_device_tft180.o \
./libraries/zf_device/zf_device_tsl1401.o \
./libraries/zf_device/zf_device_type.o \
./libraries/zf_device/zf_device_virtual_oscilloscope.o \
./libraries/zf_device/zf_device_wifi_uart.o \
./libraries/zf_device/zf_device_wireless_uart.o 

COMPILED_SRCS += \
./libraries/zf_device/zf_device_absolute_encoder.src \
./libraries/zf_device/zf_device_bluetooth_ch9141.src \
./libraries/zf_device/zf_device_camera.src \
./libraries/zf_device/zf_device_dl1a.src \
./libraries/zf_device/zf_device_icm20602.src \
./libraries/zf_device/zf_device_imu660ra.src \
./libraries/zf_device/zf_device_imu963ra.src \
./libraries/zf_device/zf_device_key.src \
./libraries/zf_device/zf_device_mpu6050.src \
./libraries/zf_device/zf_device_mt9v03x.src \
./libraries/zf_device/zf_device_oled.src \
./libraries/zf_device/zf_device_tft180.src \
./libraries/zf_device/zf_device_tsl1401.src \
./libraries/zf_device/zf_device_type.src \
./libraries/zf_device/zf_device_virtual_oscilloscope.src \
./libraries/zf_device/zf_device_wifi_uart.src \
./libraries/zf_device/zf_device_wireless_uart.src 

C_DEPS += \
./libraries/zf_device/zf_device_absolute_encoder.d \
./libraries/zf_device/zf_device_bluetooth_ch9141.d \
./libraries/zf_device/zf_device_camera.d \
./libraries/zf_device/zf_device_dl1a.d \
./libraries/zf_device/zf_device_icm20602.d \
./libraries/zf_device/zf_device_imu660ra.d \
./libraries/zf_device/zf_device_imu963ra.d \
./libraries/zf_device/zf_device_key.d \
./libraries/zf_device/zf_device_mpu6050.d \
./libraries/zf_device/zf_device_mt9v03x.d \
./libraries/zf_device/zf_device_oled.d \
./libraries/zf_device/zf_device_tft180.d \
./libraries/zf_device/zf_device_tsl1401.d \
./libraries/zf_device/zf_device_type.d \
./libraries/zf_device/zf_device_virtual_oscilloscope.d \
./libraries/zf_device/zf_device_wifi_uart.d \
./libraries/zf_device/zf_device_wireless_uart.d 


# Each subdirectory must supply rules for building sources it contributes
libraries/zf_device/%.src: ../libraries/zf_device/%.c libraries/zf_device/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: TASKING C/C++ Compiler'
	cctc -D__CPU__=tc26xb "-fE:/nodeanddata/studio/FSL/Complete/S18/software/Seekfree_TC264_Opensource_Library/Debug/TASKING_C_C___Compiler-Include_paths.opt" --iso=99 --c++14 --language=+volatile --exceptions --anachronisms --fp-model=3 -O0 --tradeoff=4 --compact-max-size=200 -g -Wc-w544 -Wc-w557 -Ctc26xb -o "$@"  "$<"  -cs --dep-file="$(@:.src=.d)" --misrac-version=2012 -N0 -Z0 -Y0 2>&1;
	@echo 'Finished building: $<'
	@echo ' '

libraries/zf_device/%.o: ./libraries/zf_device/%.src libraries/zf_device/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: TASKING Assembler'
	astc -Og -Os --no-warnings= --error-limit=42 -o  "$@" "$<" --list-format=L1 --optimize=gs
	@echo 'Finished building: $<'
	@echo ' '


