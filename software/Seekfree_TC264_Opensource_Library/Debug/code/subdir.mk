################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../code/ZwPlus_lib.c \
../code/bluetooth.c \
../code/filter.c \
../code/key.c \
../code/motor.c 

OBJS += \
./code/ZwPlus_lib.o \
./code/bluetooth.o \
./code/filter.o \
./code/key.o \
./code/motor.o 

COMPILED_SRCS += \
./code/ZwPlus_lib.src \
./code/bluetooth.src \
./code/filter.src \
./code/key.src \
./code/motor.src 

C_DEPS += \
./code/ZwPlus_lib.d \
./code/bluetooth.d \
./code/filter.d \
./code/key.d \
./code/motor.d 


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


