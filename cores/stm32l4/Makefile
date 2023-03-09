TARGET = ProffieLite_BoardPack

# debug build?
DEBUG = 1
# optimization
OPT = -Os

BUILD_DIR = board_build
SYSTEMPACK = ../../system/STM32L4xx/Source
TOOLS    = C:\Users\Cosmin\AppData\Local\Arduino15\packages\UltraProffie\tools\arm-none-eabi-gcc\9-2020-q2-update
# TOOLS    = C:\Users\user\AppData\Local\Arduino15\packages\proffieboard\tools\arm-none-eabi-gcc\9-2020-q2-update
CPP      = $(TOOLS)/bin/arm-none-eabi-cpp
CC       = $(TOOLS)/bin/arm-none-eabi-gcc
CXX      = $(TOOLS)/bin/arm-none-eabi-g++
AS       = $(TOOLS)/bin/arm-none-eabi-as
AR       = $(TOOLS)/bin/arm-none-eabi-ar
LD       = $(TOOLS)/bin/arm-none-eabi-ld
SZ		 = $(TOOLS)/bin/arm-none-eabi-size
CFLAGS   = -mcpu=cortex-m4 -mthumb -c -g3 $(OPT) $(WARNINGS) -std=gnu11 -ffunction-sections -fdata-sections -nostdlib --param max-inline-insns-single=500 -MMD $(EXTRAS) $(DEFINES) $(INCLUDES)
CXXFLAGS = -mcpu=cortex-m4 -mthumb -c -g3 $(OPT) $(WARNINGS) -std=gnu++11 -ffunction-sections -fdata-sections -fno-threadsafe-statics -nostdlib --param max-inline-insns-single=500 -fno-rtti -fno-exceptions -MMD $(EXTRAS) $(DEFINES) $(INCLUDES)
ASFLAGS  = -c -g -x assembler-with-cpp $(EXTRAS) $(DEFINES) $(INCLUDES)
#LDFLAGS  = -L../../system/CMSIS/Lib -Lboard_build/ $(OPT) -Wl,--gc-sections -save-temps $(EXTRAS) -T../../variants/STM32L431CC-ProffieLite/linker_scripts/STM32L431CC_FLASH.ld --specs=nano.specs -mcpu=cortex-m4 -mthumb -Wl,--cref -Wl,--check-sections -Wl,--gc-sections -Wl,--unresolved-symbols=report-all -Wl,--warn-common -Wl,--warn-section-align
LDFLAGS  = -L../../system/STM32L4xx/Lib -L../../system/CMSIS/Lib $(OPT) -Wl,--gc-sections -save-temps $(EXTRAS) -T../../variants/STM32L431CC-ProffieLite/linker_scripts/STM32L431CC_FLASH.ld --specs=nano.specs -mcpu=cortex-m4 -mthumb -Wl,--cref -Wl,--check-sections -Wl,--gc-sections -Wl,--unresolved-symbols=report-all -Wl,--warn-common -Wl,--warn-section-align

WARNINGS = -Wall -Wextra -Wno-unused-parameter
EXTRAS   = -DSTM32L431xx -D__FPU_PRESENT=1 -march=armv7e-m -mthumb -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mabi=aapcs -mslow-flash-data
EXTRAS +=-DUSB_VID=0x90 -DUSB_PID=0x80 -DUSB_DID=0x90 -DUSB_MANUFACTURER=0x90 -DUSB_PRODUCT=0x90 -DUSB_TYPE=0x00
# EXTRAS +=-DDOSFS_SDCARD=0 -DDOSFS_SFLASH=1
EXTRAS +=-DDOSFS_SDCARD=1 -DDOSFS_SFLASH=0

DEFINES  = -D_SYSTEM_CORE_CLOCK_=80000000L -DARDUINO=10606 -D_ARDUINO_STM32L4 -DARDUINO_ARCH_STM32L4
DEFINES +=  \
	-DUSE_HAL_DRIVER \
	-DSTM32L431xx

INCLUDES = -I../../system/CMSIS/Include -I../../system/CMSIS/Device/ST/STM32L4xx/Include -I../../system/STM32L4xx/Include -I../../variants/STM32L431CC-ProffieLite -I../../libraries/RTC/src  -I../../libraries/I2S/src -I../../libraries/SPI/src -I../../libraries/Wire/src -I../../libraries/Servo/src -I. 
INCLUDES += \
	-I../../system/CMSIS/Include \
	-I../../system/CMSIS/Device/ST/STM32L4xx/Include \
	-I../../system/STM32L4xx/Source/DAP \
	-I../../system/STM32L4xx/Source/USB/HAL/Inc \
	-I../../system/STM32L4xx/Source/USB/Core/Inc \
	-I../../system/STM32L4xx/Source/USB/Class/CDC/Inc \
	-I../../system/STM32L4xx/Source/USB/Class/MSC/Inc \
	-I../../system/STM32L4xx/Source/USB/Class/HID/Inc \
	-I../../system/STM32L4xx/Source/USB/Class/WEBUSB/Inc \
	-I../../system/STM32L4xx/Source/USB \
	-I../../system/STM32L4xx/Include \
	-I.
# USB Flags
# ---------

C_SRCS_CORE = \
	./avr/dtostrf.c \
	./avr/eeprom.c \
	./avr/fdevopen.c \
	hooks.c \
	itoa.c \
	stm32l4_wiring.c \
	stm32l4_wiring_analog.c \
	stm32l4_wiring_digital.c \
	stm32l4_wiring_interrupts.c \
	stm32l4_wiring_pulse.c \
	stm32l4_wiring_shift.c \
	stm32l4_wiring_tone.c
# 	HardwareSerial.cpp 

CPP_SRCS_CORE = \
	../../libraries/I2S/src/I2S.cpp \
	../../libraries/RTC/src/RTC.cpp \
	../../libraries/Servo/src/Servo.cpp \
	../../libraries/SPI/src/SPI.cpp \
	../../libraries/Wire/src/Wire.cpp \
	../../variants/STM32L431CC-ProffieLite/variant.cpp \
	CDC.cpp \
	FS.cpp \
	IPAddress.cpp \
	Print.cpp \
	RingBuffer.cpp \
	STM32.cpp \
	Stream.cpp \
	USBCore.cpp \
	Uart.cpp \
	WMath.cpp \
	WString.cpp \
	abi.cpp \
	main.cpp \
	new.cpp \
	SerialStub.cpp \
	

# C sources
C_SRCS_SYSTEM =  \
	$(SYSTEMPACK)/DAP/DAP.c \
	$(SYSTEMPACK)/DAP/SW_DP.c \
	$(SYSTEMPACK)/armv7m_atomic.c \
	$(SYSTEMPACK)/armv7m_core.c \
	$(SYSTEMPACK)/armv7m_orchid.c \
	$(SYSTEMPACK)/armv7m_pendsv.c \
	$(SYSTEMPACK)/armv7m_svcall.c \
	$(SYSTEMPACK)/armv7m_systick.c \
	$(SYSTEMPACK)/armv7m_timer.c \
	$(SYSTEMPACK)/dosfs_core.c \
	$(SYSTEMPACK)/dosfs_device.c \
	$(SYSTEMPACK)/dosfs_sflash.c \
	$(SYSTEMPACK)/dosfs_storage.c \
	$(SYSTEMPACK)/stm32l4_adc.c \
	$(SYSTEMPACK)/stm32l4_clib.c \
	$(SYSTEMPACK)/stm32l4_dac.c \
	$(SYSTEMPACK)/stm32l4_dma.c \
	$(SYSTEMPACK)/stm32l4_exti.c \
	$(SYSTEMPACK)/stm32l4_flash.c \
	$(SYSTEMPACK)/stm32l4_i2c.c \
	$(SYSTEMPACK)/stm32l4_iwdg.c \
	$(SYSTEMPACK)/stm32l4_gpio.c \
	$(SYSTEMPACK)/stm32l4_nvic.c \
	$(SYSTEMPACK)/stm32l4_qspi.c \
	$(SYSTEMPACK)/stm32l4_rtc.c \
	$(SYSTEMPACK)/stm32l4_sai.c \
	$(SYSTEMPACK)/stm32l4_sdmmc.c \
	$(SYSTEMPACK)/stm32l4_sdspi.c \
	$(SYSTEMPACK)/stm32l4_spi.c \
	$(SYSTEMPACK)/stm32l4_servo.c \
	$(SYSTEMPACK)/stm32l4_system.c \
	$(SYSTEMPACK)/stm32l4_timer.c \
	$(SYSTEMPACK)/stm32l4_uart.c \
	$(SYSTEMPACK)/stm32l4_usbd_cdc.c \
	$(SYSTEMPACK)/stm32l4_usbd_dap.c \
	$(SYSTEMPACK)/stm32l4_usbd_hid.c \
	$(SYSTEMPACK)/stm32l4_crc.c \
	# $(SYSTEMPACK)/boot_stm32l4xx.c

BSRCS = $(SYSTEMPACK)/boot_stm32l4xx.c

# ASM sources
ASM_SRCS_SYSTEM =  \
	$(SYSTEMPACK)/armv7m_rtlib.S \
	$(SYSTEMPACK)/startup_stm32l431xx.S

all:: $(BUILD_DIR)/$(TARGET).elf $(BUILD_DIR)/$(TARGET).bin
#$(BUILD_DIR)/boot_stm32l431.o 

# list of ASM program objects
OBJECTS = $(addprefix $(BUILD_DIR)/,$(notdir $(ASM_SRCS_SYSTEM:.S=.o)))
vpath %.S $(sort $(dir $(ASM_SRCS_SYSTEM)))
# list of objects
OBJECTS += $(addprefix $(BUILD_DIR)/,$(notdir $(C_SRCS_SYSTEM:.c=.o)))
vpath %.c $(sort $(dir $(C_SRCS_SYSTEM)))

OBJECTS += $(addprefix $(BUILD_DIR)/,$(notdir $(C_SRCS_CORE:.c=.o)))
vpath %.c $(sort $(dir $(C_SRCS_CORE)))

OBJECTS += $(addprefix $(BUILD_DIR)/,$(notdir $(CPP_SRCS_CORE:.cpp=.o)))
vpath %.cpp $(sort $(dir $(CPP_SRCS_CORE)))

BOBJS_L431 = $(addprefix $(BUILD_DIR)/,$(notdir $(BSRCS:.c=.o)))
vpath %.c $(sort $(dir $(BSRCS)))

$(info    Im on $(OBJECTS))
$(info    Im on $(BOBJS_L431))


# OBJECTS = $(patsubst %.c, $(BUILD_DIR)/%.o,$(filter-out %.cpp,$(SRCS))) $(patsubst %.cpp,$(BUILD_DIR)/%.o,$(filter-out %.c,$(SRCS)))
# $(info    Im on $(OBJECTS))

$(BUILD_DIR)/boot_stm32l431.o: $(BOBJS_L431)  $(BUILD_DIR)
	$(LD) -r -o $@ $^

$(BUILD_DIR)/%.o: %.c $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/%.o: %.cpp $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(BUILD_DIR)/%.o: %.S $(BUILD_DIR)
	$(CC) $(ASFLAGS) -c $< -o $@

$(BUILD_DIR)/%.o: %.s $(BUILD_DIR)
	$(CC) $(ASFLAGS) -c $< -o $@

$(BUILD_DIR)/$(TARGET).elf: $(OBJECTS) 
# $(CC) $(LDFLAGS) -Wl,-Map,$(BUILD_DIR)/$(TARGET).map -Wl,--start-group $(OBJECTS) -Wl,--end-group -lstm32l431 -larm_cortexM4lf_math -lc -lm -o $(BUILD_DIR)/$(TARGET).elf
	$(CC) $(LDFLAGS) -Wl,-Map,$(BUILD_DIR)/$(TARGET).map -Wl,--start-group $(OBJECTS) -Wl,--end-group -larm_cortexM4lf_math -lc -lm -o $(BUILD_DIR)/$(TARGET).elf
	$(SZ) $@

$(BUILD_DIR)/$(TARGET).bin: $(BUILD_DIR)/$(TARGET).elf
	arm-none-eabi-objcopy -O binary $(BUILD_DIR)/$(TARGET).elf $(BUILD_DIR)/$(TARGET).bin

$(BUILD_DIR):
	$(info    Ive created bourd_build dir)
	mkdir $@

clean::
	rm -f $(TARGET).elf $(TARGET).bin $(TARGET).map *~ $(OBJS) $(OBJS:.o=.d)
	-rm -fR $(BUILD_DIR)

-include $(OBJS:.o=.d)
# -include $(wildcard $(BUILD_DIR)/*.d)

