################################################################################
# Automatically-generated file. Do not edit!
################################################################################

SHELL = cmd.exe

# Each subdirectory must supply rules for building sources it contributes
%.o: ../%.c $(GEN_OPTS) | $(GEN_FILES) $(GEN_MISC_FILES)
	@echo 'Building file: "$<"'
	@echo 'Invoking: Arm Compiler'
	"C:/ti/ccs2040/ccs/tools/compiler/ti-cgt-armllvm_4.0.4.LTS/bin/tiarmclang.exe" -c @"device.opt"  -march=thumbv6m -mcpu=cortex-m0plus -mfloat-abi=soft -mlittle-endian -mthumb -O0 -I"C:/Users/ashto/Dev/SeniorDesign_SwanGanz/firmware/SwanGanz" -I"C:/Users/ashto/Dev/SeniorDesign_SwanGanz/firmware/SwanGanz/Debug" -I"C:/ti/mspm0_sdk_2_09_00_01/source/third_party/CMSIS/Core/Include" -I"C:/ti/mspm0_sdk_2_09_00_01/source" -gdwarf-3 -MMD -MP -MF"$(basename $(<F)).d_raw" -MT"$(@)"  $(GEN_OPTS__FLAG) -o"$@" "$<"
	@echo 'Finished building: "$<"'
	@echo ' '

OS.o: ../OS.c $(GEN_OPTS) | $(GEN_FILES) $(GEN_MISC_FILES)
	@echo 'Building file: "$<"'
	@echo 'Invoking: Arm Compiler'
	"C:/ti/ccs2040/ccs/tools/compiler/ti-cgt-armllvm_4.0.4.LTS/bin/tiarmclang.exe" -c @"device.opt"  -march=thumbv6m -mcpu=cortex-m0plus -mfloat-abi=soft -mlittle-endian -mthumb -O0 -I"C:/Users/ashto/Dev/SeniorDesign_SwanGanz/firmware/SwanGanz" -I"C:/Users/ashto/Dev/SeniorDesign_SwanGanz/firmware/SwanGanz/Debug" -I"C:/ti/mspm0_sdk_2_09_00_01/source/third_party/CMSIS/Core/Include" -I"C:/ti/mspm0_sdk_2_09_00_01/source" -D__MSPM0G3507__ -gdwarf-3 -MMD -MP -MF"$(basename $(<F)).d_raw" -MT"$(basename\ $(<F)).o"  $(GEN_OPTS__FLAG) -o"$@" "$<"
	@echo 'Finished building: "$<"'
	@echo ' '

build-1361750188: ../empty.syscfg
	@echo 'Building file: "$<"'
	@echo 'Invoking: SysConfig'
	"C:/ti/ccs2040/ccs/utils/sysconfig_1.26.0/sysconfig_cli.bat" -s "C:/ti/mspm0_sdk_2_09_00_01/.metadata/product.json" --script "C:/Users/ashto/Dev/SeniorDesign_SwanGanz/firmware/SwanGanz/empty.syscfg" -o "." --compiler ticlang
	@echo 'Finished building: "$<"'
	@echo ' '

device_linker.cmd: build-1361750188 ../empty.syscfg
device.opt: build-1361750188
device.cmd.genlibs: build-1361750188
ti_msp_dl_config.c: build-1361750188
ti_msp_dl_config.h: build-1361750188

%.o: ./%.c $(GEN_OPTS) | $(GEN_FILES) $(GEN_MISC_FILES)
	@echo 'Building file: "$<"'
	@echo 'Invoking: Arm Compiler'
	"C:/ti/ccs2040/ccs/tools/compiler/ti-cgt-armllvm_4.0.4.LTS/bin/tiarmclang.exe" -c @"device.opt"  -march=thumbv6m -mcpu=cortex-m0plus -mfloat-abi=soft -mlittle-endian -mthumb -O0 -I"C:/Users/ashto/Dev/SeniorDesign_SwanGanz/firmware/SwanGanz" -I"C:/Users/ashto/Dev/SeniorDesign_SwanGanz/firmware/SwanGanz/Debug" -I"C:/ti/mspm0_sdk_2_09_00_01/source/third_party/CMSIS/Core/Include" -I"C:/ti/mspm0_sdk_2_09_00_01/source" -gdwarf-3 -MMD -MP -MF"$(basename $(<F)).d_raw" -MT"$(@)"  $(GEN_OPTS__FLAG) -o"$@" "$<"
	@echo 'Finished building: "$<"'
	@echo ' '

startup_mspm0g350x_ticlang.o: C:/ti/mspm0_sdk_2_09_00_01/source/ti/devices/msp/m0p/startup_system_files/ticlang/startup_mspm0g350x_ticlang.c $(GEN_OPTS) | $(GEN_FILES) $(GEN_MISC_FILES)
	@echo 'Building file: "$<"'
	@echo 'Invoking: Arm Compiler'
	"C:/ti/ccs2040/ccs/tools/compiler/ti-cgt-armllvm_4.0.4.LTS/bin/tiarmclang.exe" -c @"device.opt"  -march=thumbv6m -mcpu=cortex-m0plus -mfloat-abi=soft -mlittle-endian -mthumb -O0 -I"C:/Users/ashto/Dev/SeniorDesign_SwanGanz/firmware/SwanGanz" -I"C:/Users/ashto/Dev/SeniorDesign_SwanGanz/firmware/SwanGanz/Debug" -I"C:/ti/mspm0_sdk_2_09_00_01/source/third_party/CMSIS/Core/Include" -I"C:/ti/mspm0_sdk_2_09_00_01/source" -gdwarf-3 -MMD -MP -MF"$(basename $(<F)).d_raw" -MT"$(@)"  $(GEN_OPTS__FLAG) -o"$@" "$<"
	@echo 'Finished building: "$<"'
	@echo ' '

main.o: ../main.c $(GEN_OPTS) | $(GEN_FILES) $(GEN_MISC_FILES)
	@echo 'Building file: "$<"'
	@echo 'Invoking: Arm Compiler'
	"C:/ti/ccs2040/ccs/tools/compiler/ti-cgt-armllvm_4.0.4.LTS/bin/tiarmclang.exe" -c @"device.opt"  -march=thumbv6m -mcpu=cortex-m0plus -mfloat-abi=soft -mlittle-endian -mthumb -O0 -I"C:/Users/ashto/Dev/SeniorDesign_SwanGanz/firmware/SwanGanz" -I"C:/Users/ashto/Dev/SeniorDesign_SwanGanz/firmware/SwanGanz/Debug" -I"C:/ti/mspm0_sdk_2_09_00_01/source/third_party/CMSIS/Core/Include" -I"C:/ti/mspm0_sdk_2_09_00_01/source" -D__MSPM0G3507__ -gdwarf-3 -MMD -MP -MF"$(basename $(<F)).d_raw" -MT"$(basename\ $(<F)).o" -std=c99 $(GEN_OPTS__FLAG) -o"$@" "$<"
	@echo 'Finished building: "$<"'
	@echo ' '

%.o: ../%.s $(GEN_OPTS) | $(GEN_FILES) $(GEN_MISC_FILES)
	@echo 'Building file: "$<"'
	@echo 'Invoking: Arm Compiler'
	"C:/ti/ccs2040/ccs/tools/compiler/ti-cgt-armllvm_4.0.4.LTS/bin/tiarmclang.exe" -c @"device.opt"  -march=thumbv6m -mcpu=cortex-m0plus -mfloat-abi=soft -mlittle-endian -mthumb -O0 -I"C:/Users/ashto/Dev/SeniorDesign_SwanGanz/firmware/SwanGanz" -I"C:/Users/ashto/Dev/SeniorDesign_SwanGanz/firmware/SwanGanz/Debug" -I"C:/ti/mspm0_sdk_2_09_00_01/source/third_party/CMSIS/Core/Include" -I"C:/ti/mspm0_sdk_2_09_00_01/source" -gdwarf-3 $(GEN_OPTS__FLAG) -o"$@" "$<"
	@echo 'Finished building: "$<"'
	@echo ' '


