################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Each subdirectory must supply rules for building sources it contributes
inc/%.o: ../inc/%.c $(GEN_OPTS) | $(GEN_FILES) $(GEN_MISC_FILES)
	@echo 'Building file: "$<"'
	@echo 'Invoking: Arm Compiler'
	"/Applications/ti/ccs2040/ccs/tools/compiler/ti-cgt-armllvm_4.0.4.LTS/bin/tiarmclang" -c @"device.opt"  -march=thumbv6m -mcpu=cortex-m0plus -mfloat-abi=soft -mlittle-endian -mthumb -O0 -I"/Users/nathan/Dev/464H/SeniorDesign_SwanGanz/firmware/SwanGanz" -I"/Users/nathan/Dev/464H/SeniorDesign_SwanGanz/firmware/SwanGanz/Debug" -I"/Applications/ti/mspm0_sdk_2_09_00_01/source/third_party/CMSIS/Core/Include" -I"/Applications/ti/mspm0_sdk_2_09_00_01/source" -gdwarf-3 -MMD -MP -MF"inc/$(basename $(<F)).d_raw" -MT"$(@)"  $(GEN_OPTS__FLAG) -o"$@" "$<"
	@echo 'Finished building: "$<"'
	@echo ' '

inc/%.o: ../inc/%.s $(GEN_OPTS) | $(GEN_FILES) $(GEN_MISC_FILES)
	@echo 'Building file: "$<"'
	@echo 'Invoking: Arm Compiler'
	"/Applications/ti/ccs2040/ccs/tools/compiler/ti-cgt-armllvm_4.0.4.LTS/bin/tiarmclang" -c @"device.opt"  -march=thumbv6m -mcpu=cortex-m0plus -mfloat-abi=soft -mlittle-endian -mthumb -O0 -I"/Users/nathan/Dev/464H/SeniorDesign_SwanGanz/firmware/SwanGanz" -I"/Users/nathan/Dev/464H/SeniorDesign_SwanGanz/firmware/SwanGanz/Debug" -I"/Applications/ti/mspm0_sdk_2_09_00_01/source/third_party/CMSIS/Core/Include" -I"/Applications/ti/mspm0_sdk_2_09_00_01/source" -gdwarf-3 $(GEN_OPTS__FLAG) -o"$@" "$<"
	@echo 'Finished building: "$<"'
	@echo ' '


