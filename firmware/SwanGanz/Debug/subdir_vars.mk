################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
SYSCFG_SRCS += \
../empty.syscfg 

S_SRCS += \
../osasm.s 

C_SRCS += \
../DAS.c \
../Display.c \
../FIFO.c \
../OS.c \
./ti_msp_dl_config.c \
/Applications/ti/mspm0_sdk_2_09_00_01/source/ti/devices/msp/m0p/startup_system_files/ticlang/startup_mspm0g350x_ticlang.c \
../main.c 

S_DEPS += \
./osasm.d 

GEN_CMDS += \
./device_linker.cmd 

GEN_FILES += \
./device_linker.cmd \
./device.opt \
./ti_msp_dl_config.c 

C_DEPS += \
./DAS.d \
./Display.d \
./FIFO.d \
./OS.d \
./ti_msp_dl_config.d \
./startup_mspm0g350x_ticlang.d \
./main.d 

GEN_OPTS += \
./device.opt 

OBJS += \
./DAS.o \
./Display.o \
./FIFO.o \
./OS.o \
./ti_msp_dl_config.o \
./startup_mspm0g350x_ticlang.o \
./main.o \
./osasm.o 

GEN_MISC_FILES += \
./device.cmd.genlibs \
./ti_msp_dl_config.h 

OBJS__QUOTED += \
"DAS.o" \
"Display.o" \
"FIFO.o" \
"OS.o" \
"ti_msp_dl_config.o" \
"startup_mspm0g350x_ticlang.o" \
"main.o" \
"osasm.o" 

GEN_MISC_FILES__QUOTED += \
"device.cmd.genlibs" \
"ti_msp_dl_config.h" 

C_DEPS__QUOTED += \
"DAS.d" \
"Display.d" \
"FIFO.d" \
"OS.d" \
"ti_msp_dl_config.d" \
"startup_mspm0g350x_ticlang.d" \
"main.d" 

GEN_FILES__QUOTED += \
"device_linker.cmd" \
"device.opt" \
"ti_msp_dl_config.c" 

S_DEPS__QUOTED += \
"osasm.d" 

C_SRCS__QUOTED += \
"../DAS.c" \
"../Display.c" \
"../FIFO.c" \
"./ti_msp_dl_config.c" \
"/Applications/ti/mspm0_sdk_2_09_00_01/source/ti/devices/msp/m0p/startup_system_files/ticlang/startup_mspm0g350x_ticlang.c" 

SYSCFG_SRCS__QUOTED += \
"../empty.syscfg" 

S_SRCS__QUOTED += \
"../osasm.s" 


