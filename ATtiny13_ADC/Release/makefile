################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(ASM_DEPS)),)
-include $(ASM_DEPS)
endif
ifneq ($(strip $(S_DEPS)),)
-include $(S_DEPS)
endif
ifneq ($(strip $(S_UPPER_DEPS)),)
-include $(S_UPPER_DEPS)
endif
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 
LSS += \
ATtiny13_ADC.lss \

FLASH_IMAGE += \
ATtiny13_ADC.hex \

EEPROM_IMAGE += \
ATtiny13_ADC.eep \

SIZEDUMMY += \
sizedummy \


# All Target
all: ATtiny13_ADC.elf secondary-outputs

# Tool invocations
ATtiny13_ADC.elf: $(OBJS) $(USER_OBJS)
	@echo 'Building target: $@'
	@echo 'Invoking: AVR C Linker'
	avr-gcc -Wl,-Map,ATtiny13_ADC.map -mmcu=attiny13 -o "ATtiny13_ADC.elf" $(OBJS) $(USER_OBJS) $(LIBS)
	@echo 'Finished building target: $@'
	@echo ' '

ATtiny13_ADC.lss: ATtiny13_ADC.elf
	@echo 'Invoking: AVR Create Extended Listing'
	-avr-objdump -h -S ATtiny13_ADC.elf  >"ATtiny13_ADC.lss"
	@echo 'Finished building: $@'
	@echo ' '

ATtiny13_ADC.hex: ATtiny13_ADC.elf
	@echo 'Create Flash image (ihex format)'
	-avr-objcopy -R .eeprom -R .fuse -R .lock -R .signature -O ihex ATtiny13_ADC.elf  "ATtiny13_ADC.hex"
	@echo 'Finished building: $@'
	@echo ' '

ATtiny13_ADC.eep: ATtiny13_ADC.elf
	@echo 'Create eeprom image (ihex format)'
	-avr-objcopy -j .eeprom --no-change-warnings --change-section-lma .eeprom=0 -O ihex ATtiny13_ADC.elf  "ATtiny13_ADC.eep"
	@echo 'Finished building: $@'
	@echo ' '

sizedummy: ATtiny13_ADC.elf
	@echo 'Invoking: Print Size'
	-avr-size --format=avr --mcu=attiny13 ATtiny13_ADC.elf
	@echo 'Finished building: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) $(FLASH_IMAGE)$(ELFS)$(OBJS)$(ASM_DEPS)$(EEPROM_IMAGE)$(S_DEPS)$(SIZEDUMMY)$(S_UPPER_DEPS)$(LSS)$(C_DEPS) ATtiny13_ADC.elf
	-@echo ' '

secondary-outputs: $(LSS) $(FLASH_IMAGE) $(EEPROM_IMAGE) $(SIZEDUMMY)

.PHONY: all clean dependents
.SECONDARY:

-include ../makefile.targets
