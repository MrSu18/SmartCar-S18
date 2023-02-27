################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../code/ImageBasic.c \
../code/ImageConversion.c \
../code/ImageTrack.c \
../code/ZwPlus_lib.c \
../code/adc.c \
../code/bluetooth.c \
../code/filter.c \
../code/key.c \
../code/motor.c \
../code/pid.c 

OBJS += \
./code/ImageBasic.o \
./code/ImageConversion.o \
./code/ImageTrack.o \
./code/ZwPlus_lib.o \
./code/adc.o \
./code/bluetooth.o \
./code/filter.o \
./code/key.o \
./code/motor.o \
./code/pid.o 

COMPILED_SRCS += \
./code/ImageBasic.src \
./code/ImageConversion.src \
./code/ImageTrack.src \
./code/ZwPlus_lib.src \
./code/adc.src \
./code/bluetooth.src \
./code/filter.src \
./code/key.src \
./code/motor.src \
./code/pid.src 

C_DEPS += \
./code/ImageBasic.d \
./code/ImageConversion.d \
./code/ImageTrack.d \
./code/ZwPlus_lib.d \
./code/adc.d \
./code/bluetooth.d \
./code/filter.d \
./code/key.d \
./code/motor.d \
./code/pid.d 


# Each subdirectory must supply rules for building sources it contributes
code/%.src: ../code/%.c code/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: TASKING C/C++ Compiler'
	cctc -D__CPU__=tc26xb "-fE:/nodeanddata/studio/FSL/Complete/S18/software/Seekfree_TC264_Opensource_Library/Debug/TASKING_C_C___Compiler-Include_paths.opt" --iso=99 --c++14 --language=+volatile --exceptions --anachronisms --fp-model=3 -O0 --tradeoff=4 --compact-max-size=200 -g -Wc-w544 -Wc-w557 -Ctc26xb -o "$@"  "$<"  -cs --dep-file="$(@:.src=.d)" --misrac-version=2012 -N0 -Z0 -Y0 2>&1;
	@echo 'Finished building: $<'
	@echo ' '

code/%.o: ./code/%.src code/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: TASKING Assembler'
	astc -Og -Os --no-warnings= --error-limit=42 -o  "$@" "$<" --list-format=L1 --optimize=gs
	@echo 'Finished building: $<'
	@echo ' '


