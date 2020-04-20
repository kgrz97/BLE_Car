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
Bluetooth_car.lss \

FLASH_IMAGE += \
Bluetooth_car.hex \

EEPROM_IMAGE += \
Bluetooth_car.eep \

SIZEDUMMY += \
sizedummy \


# All Target
all: Bluetooth_car.elf secondary-outputs

# Tool invocations
Bluetooth_car.elf: $(OBJS) $(USER_OBJS)
	@echo 'Building target: $@'
	@echo 'Invoking: AVR C Linker'
	avr-gcc -Wl,-Map,Bluetooth_car.map -mmcu=attiny2313 -o "Bluetooth_car.elf" $(OBJS) $(USER_OBJS) $(LIBS)
	@echo 'Finished building target: $@'
	@echo ' '

Bluetooth_car.lss: Bluetooth_car.elf
	@echo 'Invoking: AVR Create Extended Listing'
	-avr-objdump -h -S Bluetooth_car.elf  >"Bluetooth_car.lss"
	@echo 'Finished building: $@'
	@echo ' '

Bluetooth_car.hex: Bluetooth_car.elf
	@echo 'Create Flash image (ihex format)'
	-avr-objcopy -R .eeprom -R .fuse -R .lock -R .signature -O ihex Bluetooth_car.elf  "Bluetooth_car.hex"
	@echo 'Finished building: $@'
	@echo ' '

Bluetooth_car.eep: Bluetooth_car.elf
	@echo 'Create eeprom image (ihex format)'
	-avr-objcopy -j .eeprom --no-change-warnings --change-section-lma .eeprom=0 -O ihex Bluetooth_car.elf  "Bluetooth_car.eep"
	@echo 'Finished building: $@'
	@echo ' '

sizedummy: Bluetooth_car.elf
	@echo 'Invoking: Print Size'
	-avr-size --format=avr --mcu=attiny2313 Bluetooth_car.elf
	@echo 'Finished building: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) $(FLASH_IMAGE)$(ELFS)$(OBJS)$(ASM_DEPS)$(EEPROM_IMAGE)$(S_DEPS)$(SIZEDUMMY)$(S_UPPER_DEPS)$(LSS)$(C_DEPS) Bluetooth_car.elf
	-@echo ' '

secondary-outputs: $(LSS) $(FLASH_IMAGE) $(EEPROM_IMAGE) $(SIZEDUMMY)

.PHONY: all clean dependents
.SECONDARY:

-include ../makefile.targets
