################################################################################
# Automatically-generated file. Do not edit!
################################################################################

CG_TOOL_ROOT := /Applications/ti/ccs2040/ccs/tools/compiler/ti-cgt-armllvm_4.0.4.LTS

GEN_OPTS__FLAG := @"./device.opt" 
GEN_CMDS__FLAG := -Wl,-l"./device_linker.cmd" 

ORDERED_OBJS += \
"./DAS.o" \
"./ti_msp_dl_config.o" \
"./startup_mspm0g350x_ticlang.o" \
"./main.o" \
"./inc/ADC.o" \
"./inc/ADCTimer.o" \
"./inc/Arabic.o" \
"./inc/Bump.o" \
"./inc/Clock.o" \
"./inc/DAC.o" \
"./inc/DAC5.o" \
"./inc/Dump.o" \
"./inc/EdgeTriggered.o" \
"./inc/FFT.o" \
"./inc/FIFO.o" \
"./inc/FIFO1.o" \
"./inc/I2C.o" \
"./inc/IRDistance.o" \
"./inc/InputCapture.o" \
"./inc/JoyStick.o" \
"./inc/Key.o" \
"./inc/LaunchPad.o" \
"./inc/Motor.o" \
"./inc/PWM.o" \
"./inc/PWM1.o" \
"./inc/SPI.o" \
"./inc/SSD1306.o" \
"./inc/ST7735.o" \
"./inc/SysTick.o" \
"./inc/Timer.o" \
"./inc/UART.o" \
"./inc/UARTints.o" \
"./inc/msp.o" \
$(GEN_CMDS__FLAG) \
-Wl,-ldevice.cmd.genlibs \
-Wl,-llibc.a \

-include ../makefile.init

RM := rm -rf
RMDIR := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include subdir_vars.mk
-include inc/subdir_vars.mk
-include inc/Arabic/subdir_vars.mk
-include subdir_rules.mk
-include inc/subdir_rules.mk
-include inc/Arabic/subdir_rules.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(C55_DEPS)),)
-include $(C55_DEPS)
endif
ifneq ($(strip $(C_UPPER_DEPS)),)
-include $(C_UPPER_DEPS)
endif
ifneq ($(strip $(S67_DEPS)),)
-include $(S67_DEPS)
endif
ifneq ($(strip $(S62_DEPS)),)
-include $(S62_DEPS)
endif
ifneq ($(strip $(S_DEPS)),)
-include $(S_DEPS)
endif
ifneq ($(strip $(OPT_DEPS)),)
-include $(OPT_DEPS)
endif
ifneq ($(strip $(C??_DEPS)),)
-include $(C??_DEPS)
endif
ifneq ($(strip $(ASM_UPPER_DEPS)),)
-include $(ASM_UPPER_DEPS)
endif
ifneq ($(strip $(S??_DEPS)),)
-include $(S??_DEPS)
endif
ifneq ($(strip $(C64_DEPS)),)
-include $(C64_DEPS)
endif
ifneq ($(strip $(CXX_DEPS)),)
-include $(CXX_DEPS)
endif
ifneq ($(strip $(S64_DEPS)),)
-include $(S64_DEPS)
endif
ifneq ($(strip $(CLA_DEPS)),)
-include $(CLA_DEPS)
endif
ifneq ($(strip $(S55_DEPS)),)
-include $(S55_DEPS)
endif
ifneq ($(strip $(SV7A_DEPS)),)
-include $(SV7A_DEPS)
endif
ifneq ($(strip $(C62_DEPS)),)
-include $(C62_DEPS)
endif
ifneq ($(strip $(C67_DEPS)),)
-include $(C67_DEPS)
endif
ifneq ($(strip $(K_DEPS)),)
-include $(K_DEPS)
endif
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
ifneq ($(strip $(CC_DEPS)),)
-include $(CC_DEPS)
endif
ifneq ($(strip $(C++_DEPS)),)
-include $(C++_DEPS)
endif
ifneq ($(strip $(C43_DEPS)),)
-include $(C43_DEPS)
endif
ifneq ($(strip $(S43_DEPS)),)
-include $(S43_DEPS)
endif
ifneq ($(strip $(ASM_DEPS)),)
-include $(ASM_DEPS)
endif
ifneq ($(strip $(S_UPPER_DEPS)),)
-include $(S_UPPER_DEPS)
endif
ifneq ($(strip $(CPP_DEPS)),)
-include $(CPP_DEPS)
endif
ifneq ($(strip $(SA_DEPS)),)
-include $(SA_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 
EXE_OUTPUTS += \
SwanGanz.out 

EXE_OUTPUTS__QUOTED += \
"SwanGanz.out" 


# All Target
all: $(OBJS) $(GEN_CMDS)
	@$(MAKE) --no-print-directory -Onone "SwanGanz.out"

# Tool invocations
SwanGanz.out: $(OBJS) $(GEN_CMDS)
	@echo 'Building target: "$@"'
	@echo 'Invoking: Arm Linker'
	"/Applications/ti/ccs2040/ccs/tools/compiler/ti-cgt-armllvm_4.0.4.LTS/bin/tiarmclang" @"device.opt"  -march=thumbv6m -mcpu=cortex-m0plus -mfloat-abi=soft -mlittle-endian -mthumb -O2 -gdwarf-3 -Wl,-m"SwanGanz.map" -Wl,-i"/Applications/ti/mspm0_sdk_2_09_00_01/source" -Wl,-i"/Users/nathan/Dev/464H/SeniorDesign_SwanGanz/firmware/SwanGanz" -Wl,-i"/Users/nathan/Dev/464H/SeniorDesign_SwanGanz/firmware/SwanGanz/Debug/syscfg" -Wl,-i"/Applications/ti/ccs2040/ccs/tools/compiler/ti-cgt-armllvm_4.0.4.LTS/lib" -Wl,--diag_wrap=off -Wl,--display_error_number -Wl,--warn_sections -Wl,--xml_link_info="SwanGanz_linkInfo.xml" -Wl,--rom_model -o "SwanGanz.out" $(ORDERED_OBJS)
	@echo 'Finished building target: "$@"'
	@echo ' '

# Other Targets
clean:
	-$(RM) $(GEN_MISC_FILES__QUOTED)$(GEN_FILES__QUOTED)$(EXE_OUTPUTS__QUOTED)
	-$(RM) "DAS.o" "ti_msp_dl_config.o" "startup_mspm0g350x_ticlang.o" "main.o" "inc/ADC.o" "inc/ADCTimer.o" "inc/Arabic.o" "inc/Bump.o" "inc/Clock.o" "inc/DAC.o" "inc/DAC5.o" "inc/Dump.o" "inc/EdgeTriggered.o" "inc/FFT.o" "inc/FIFO.o" "inc/FIFO1.o" "inc/I2C.o" "inc/IRDistance.o" "inc/InputCapture.o" "inc/JoyStick.o" "inc/Key.o" "inc/LaunchPad.o" "inc/Motor.o" "inc/PWM.o" "inc/PWM1.o" "inc/SPI.o" "inc/SSD1306.o" "inc/ST7735.o" "inc/SysTick.o" "inc/Timer.o" "inc/UART.o" "inc/UARTints.o" "inc/msp.o" 
	-$(RM) "DAS.d" "ti_msp_dl_config.d" "startup_mspm0g350x_ticlang.d" "main.d" "inc/ADC.d" "inc/ADCTimer.d" "inc/Arabic.d" "inc/Bump.d" "inc/Clock.d" "inc/DAC.d" "inc/DAC5.d" "inc/Dump.d" "inc/EdgeTriggered.d" "inc/FFT.d" "inc/FIFO.d" "inc/FIFO1.d" "inc/I2C.d" "inc/IRDistance.d" "inc/InputCapture.d" "inc/JoyStick.d" "inc/Key.d" "inc/LaunchPad.d" "inc/Motor.d" "inc/PWM.d" "inc/PWM1.d" "inc/SPI.d" "inc/SSD1306.d" "inc/ST7735.d" "inc/Timer.d" "inc/UART.d" "inc/UARTints.d" 
	-$(RM) "inc/SysTick.d" "inc/msp.d" 
	-@echo 'Finished clean'
	-@echo ' '

.PHONY: all clean dependents
.SECONDARY:

-include ../makefile.targets

