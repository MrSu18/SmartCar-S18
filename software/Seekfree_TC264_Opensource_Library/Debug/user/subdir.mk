################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../user/ImageGarage.c \
../user/cpu0_main.c \
../user/cpu1_main.c \
../user/isr.c 

OBJS += \
./user/ImageGarage.o \
./user/cpu0_main.o \
./user/cpu1_main.o \
./user/isr.o 

COMPILED_SRCS += \
./user/ImageGarage.src \
./user/cpu0_main.src \
./user/cpu1_main.src \
./user/isr.src 

C_DEPS += \
./user/ImageGarage.d \
./user/cpu0_main.d \
./user/cpu1_main.d \
./user/isr.d 


# Each subdirectory must supply rules for building sources it contributes
user/%.src: ../user/%.c user/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: TASKING C/C++ Compiler'
	cctc -D__CPU__=tc26xb "-fE:/github/smartcar/software/Seekfree_TC264_Opensource_Library/Debug/TASKING_C_C___Compiler-Include_paths.opt" --iso=99 --c++14 --language=+volatile --exceptions --anachronisms --fp-model=3 -O0 --tradeoff=4 --compact-max-size=200 -g -Wc-w544 -Wc-w557 -Ctc26xb -o "$@"  "$<"  -cs --dep-file="$(@:.src=.d)" --misrac-version=2012 -N0 -Z0 -Y0 2>&1;
	@echo 'Finished building: $<'
	@echo ' '

user/%.o: ./user/%.src user/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: TASKING Assembler'
	astc -Og -Os --no-warnings= --error-limit=42 -o  "$@" "$<" --list-format=L1 --optimize=gs
	@echo 'Finished building: $<'
	@echo ' '


