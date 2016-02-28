# Copyright 2015 Florent Rotenberg.
#
# Author: Florent Rotenberg (florent.rotenberg@gmail.com)
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in
# all copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
# THE SOFTWARE.
#
# See http://creativecommons.org/licenses/MIT/ for more information.

# ------------------------------------------------------------------------------
# toolchain definitions
# ------------------------------------------------------------------------------
TOOLCHAIN_PATH 				?= $(HOME)/opt/bin/gcc-arm-none-eabi/
TOOLCHAIN_BIN 				= $(TOOLCHAIN_PATH)bin/

#+ PGM_SERIAL_PORT 			= /dev/tty.usbserial-A100S09O
#+ PGM_SERIAL_BAUD_RATE 	= 115200
#+ PGM_SERIAL_VERIFY 		= -v

BUILD_ROOT 						= build/
BUILD_DIR 						= $(BUILD_ROOT)$(TARGET)/
BUILD_OBJ 						= $(BUILD_ROOT)$(TARGET)/obj/

LIBDIR								= $(BUILD_ROOT)lib/

CC 										= $(TOOLCHAIN_BIN)arm-none-eabi-gcc
CXX 									= $(TOOLCHAIN_BIN)arm-none-eabi-g++
AS 										= $(TOOLCHAIN_BIN)arm-none-eabi-as
OBJCOPY 							= $(TOOLCHAIN_BIN)arm-none-eabi-objcopy
DB 										= $(TOOLCHAIN_BIN)arm-none-eabi-gdb
OBJDUMP 							= $(TOOLCHAIN_BIN)arm-none-eabi-objdump
AR 										= $(TOOLCHAIN_BIN)arm-none-eabi-ar
RANLIB								= $(TOOLCHAIN_BIN)arm-none-eabi-ranlib
SIZE						 			= $(TOOLCHAIN_BIN)arm-none-eabi-size
NM 										= $(TOOLCHAIN_BIN)arm-none-eabi-nm
HEXDUMP								= hexdump
DDD										= ddd
CTAGS									= ctags
MAKE									= make

REMOVE 								= rm -f
CAT 									= cat
DOXYGEN								= doxygen

FORMAT_SIZE    				= | xargs -I {} echo -ne "Flash:\n {} bytes" | figlet | cowsay -e o~ -n -f moose
FORMAT_RAMSIZE 				= | xargs -I {} echo -ne "Ram:\n {} bytes" | figlet | cowsay -e ~o -n -f koala

OPENOCD_PATH					= $(HOME)/opt/bin/
STM32LOADER_PATH 			= libstm32f4/programming/serial/
OPENOCD_SCRIPTS_PATH 	= libstm32f4/programming/jtag/

TAGS									= tags

# ------------------------------------------------------------------------------
# Platform specific flags
# ------------------------------------------------------------------------------
FAMILY							= f4xx
MODEL_DEFINE 				= STM32F407xx
FLASH_BASE_ADDRESS	= 0x08000000
FLASH_SIZE					= 0x00100000

ARCHFLAGS 					= -mcpu=cortex-m4 \
											-mthumb \
											-mfloat-abi=hard \
											-mfpu=fpv4-sp-d16 \
											-mlittle-endian

DEFS								= -D$(MODEL_DEFINE) \
											$(APPLICATION_DEFINE) \
											$(COPTDEFS) \
											$(CSEMIHOSTDEFS) \
											-DUSE_HAL_DRIVER \
											-DUSE_STM32F4_DISCOVERY \
											-D__VFP_FP__ \
											-D__FPU_PRESENT=1U \
											-DARM_MATH_CM4 \
											-DGCC_ARMCM4 \
											-DHSE_VALUE=$(F_CRYSTAL) \
											-DUSE_FULL_ASSERT

											#+ -DF_CPU=$(F_CPU) \
											#+ -DF_CRYSTAL=$(F_CRYSTAL) \
											#+ -D$(SYSCLOCK)=$(F_CPU) \
											#$(PROJECT_CONFIGURATION) \
											-DUSE_STDPERIPH_DRIVER \

								#+ -DUSE_OTG_MODE \
								#+ -DUSE_HOST_MODE \
								#+ -DUSE_USB_OTG_FS

ifeq ($(APPLICATION),TRUE)
APPLICATION_DEFINE 		= -DAPPLICATION
LD_SUFFIX 						= _application
BASE_ADDRESS 					= 0x08004000
else ifeq ($(APPLICATION_SMALL),TRUE)
APPLICATION_DEFINE 		= -DAPPLICATION
LD_SUFFIX 						= _application_small
BASE_ADDRESS					= 0x08001000
else ifeq ($(APPLICATION_LARGE),TRUE)
APPLICATION_DEFINE 		= -DAPPLICATION
LD_SUFFIX 						= _application_large
BASE_ADDRESS 					= 0x08008000
else
APPLICATION_DEFINE 		= -DNO_APPLICATION
LD_SUFFIX 						=
BASE_ADDRESS 					= 0x08000000
endif

# ------------------------------------------------------------------------------
# Flags for gcc/binutils
# ------------------------------------------------------------------------------
CONFIG								?= debug

ifeq ($(CONFIG), debug)
COPTFLAGS							= -g3 -O0 
COPTDEFS							= -DDEBUG 
ASOPTFLAGS						= -g3
endif
ifeq ($(CONFIG), release)
COPTFLAGS							= -O2
ASOPTFLAGS						= 
endif

## TODO how to make -flto option work ??

SEMIHOST							?= disable
ifeq ($(SEMIHOST), disable)
LDSEMIHOSTFLAGS				=  
CSEMIHOSTDEFS					=  
OPENOCDSEMIHOSTCMD		=
endif
ifeq ($(SEMIHOST), enable)
LDSEMIHOSTFLAGS				= --specs=rdimon.specs
CSEMIHOSTDEFS					= -DSEMIHOST
OPENOCDSEMIHOSTCMD		= -c "arm semihosting enable"
endif


INCFLAGS 							= -I. 

WARNINGS							= -Wall \
												-Werror \
												-Wpointer-arith \
												-Waggregate-return \
												-Wno-unused-local-typedefs \
												-Wunused

												#-Wextra \
												#-Wcast-align \
												#-Wsign-compare \
												#-Wshadow \
												#-Wfloat-equal \
												#-Wmissing-prototypes \
												#-Wmissing-declarations \

CWARNINGS							= -Wbad-function-cast \
												-Wstrict-prototypes 


FLAGS								=		$(WARNINGS) \
												$(DEFS) \
												$(ARCHFLAGS) \
												$(INCFLAGS) \
												$(COPTFLAGS) \
												-fasm \
												-finline \
												-finline-functions-called-once \
												-fdata-sections \
												-ffunction-sections \
												-funroll-loops \
												-fshort-enums \
												-fmessage-length=0 \
												-fno-move-loop-invariants

CFLAGS 							= 	$(FLAGS) \
												$(CWARNINGS) \
												-std=gnu99 \

CXXFLAGS 							= $(FLAGS) \
												-fno-exceptions \
												-fno-rtti \
												-std=c++11

ASFLAGS 							= $(ARCHFLAGS) \
												$(ASOPTFLAGS) \
												-funroll-loops \

#+ LINKER_SCRIPT 				= libstm32f4/linker_scripts/stm32f4xx_flash$(LD_SUFFIX).ld
LINKER_SCRIPT 				=	libstm32f4/linker_scripts/stm32f407vg_flash.ld

LDFLAGS 							= $(CFLAGS) \
												$(LDSEMIHOSTFLAGS) \
												--specs=nano.specs \
												-Wl,--gc-sections \
												-Wl,--relax \
												-Wl,-Map=$(BUILD_DIR)$(TARGET).map \
												-T $(LINKER_SCRIPT) \
												-L$(LIBDIR) \
												-L$(FW_AUDIO_PDM_DIR)

# ------------------------------------------------------------------------------
# libraries definition
# ------------------------------------------------------------------------------
# standard peripheral lib
LIBSTDPERIPH 		:= $(LIBDIR)libstdperiph.a
LIBS						+= $(LIBSTDPERIPH)
LDLIBS					+= -lstdperiph

# stm32f4 discovery board lib
LIBDISCOVERY		:= $(LIBDIR)libdiscovery.a
LIBS						+= $(LIBDISCOVERY)
LDLIBS					+= -ldiscovery

# cmsis stm32f4
LIBCMSISSTM32F4	:= $(LIBDIR)libcmsisstm32f4.a
LIBS						+= $(LIBCMSISSTM32F4)
LDLIBS					+= -lcmsisstm32f4

# cmsis DSP_Lib
LIBCMSISDSP 		:= $(LIBDIR)libcmsisdsp.a
LIBS						+= $(LIBCMSISDSP)
LDLIBS					+= -lcmsisdsp

# bsp components lib
# LIBCOMPONENT 		:= $(LIBDIR)libcomponent.a
# LIBS						+= $(LIBCOMPONENT)
# LDLIBS					+= -lcomponent

LDLIBS					+= -lPDMFilter_CM4F_GCC

# ------------------------------------------------------------------------------
# Files and directories for the user code
# ------------------------------------------------------------------------------
VPATH 								= $(PACKAGES)
CC_FILES_FULL					= $(wildcard $(patsubst %,%/*.cc,$(PACKAGES)))
CC_FILES 							= $(notdir $(CC_FILES_FULL))
C_FILES_FULL 					= $(wildcard $(patsubst %,%/*.c,$(PACKAGES)))
C_FILES 							= $(notdir $(C_FILES_FULL))
AS_FILES_FULL					= $(wildcard $(patsubst %,%/*.S,$(PACKAGES)))
AS_FILES 							= $(notdir $(AS_FILES_FULL))

TARGET_BIN 						= $(BUILD_DIR)$(TARGET).bin
TARGET_ELF 						= $(BUILD_DIR)$(TARGET).elf
TARGET_IHEX 					= $(BUILD_DIR)$(TARGET).ihex
TARGET_HEX 						= $(BUILD_DIR)$(TARGET).hex
TARGET_DIS 						= $(BUILD_DIR)$(TARGET).dis
TARGET_SIZE 					= $(BUILD_DIR)$(TARGET).size
TARGET_SYM 						= $(BUILD_DIR)$(TARGET).sym
TARGET_LSS 						= $(BUILD_DIR)$(TARGET).lss
TARGET_TOPSYM 				= $(BUILD_DIR)$(TARGET).top_symbols
TARGETS 							= $(BUILD_DIR)$(TARGET).*
DEP_FILE 							= $(BUILD_DIR)depends.mk

# ------------------------------------------------------------------------------
# Files and directories for the system firmware
# ------------------------------------------------------------------------------
LIBSTM32F4_PATH 			= libstm32f4/
STM32_PATH 						= libstm32f4/third_party/STM32CubeF4/

FW_STDDRIVER_DIR 					= $(STM32_PATH)STM32F4xx_HAL_Driver/
FW_STDDRIVER_SRCDIR 			= $(FW_STDDRIVER_DIR)src
FW_STDDRIVER_INCDIR 			= $(FW_STDDRIVER_DIR)inc
FW_STDDRIVER_INCDIR_LEGACY= $(FW_STDDRIVER_INCDIR)/Legacy

FW_CMSIS_DIR 							= $(STM32_PATH)CMSIS/
FW_CMSIS_CORE_DIR					= $(FW_CMSIS_DIR)inc
FW_CMSIS_STM32F4_DIR			= $(FW_CMSIS_DIR)Device
FW_CMSIS_RTOS_DIR					= $(FW_CMSIS_DIR)RTOS

FW_CMSIS_DSP_DIR 					= $(FW_CMSIS_DIR)DSP_Lib/
FW_CMSIS_DSP_BASIC_DIR		= $(FW_CMSIS_DSP_DIR)BasicMathFunctions
FW_CMSIS_DSP_COMMON_DIR		= $(FW_CMSIS_DSP_DIR)CommonTables
FW_CMSIS_DSP_COMPLEX_DIR	= $(FW_CMSIS_DSP_DIR)ComplexMathFunctions
FW_CMSIS_DSP_CONTROL_DIR	= $(FW_CMSIS_DSP_DIR)ControllerFunctions
FW_CMSIS_DSP_FAST_DIR			= $(FW_CMSIS_DSP_DIR)FastMathFunctions
FW_CMSIS_DSP_FILTER_DIR		= $(FW_CMSIS_DSP_DIR)FilteringFunctions
FW_CMSIS_DSP_MATRIX_DIR		= $(FW_CMSIS_DSP_DIR)MatrixFunctions
FW_CMSIS_DSP_STAT_DIR			= $(FW_CMSIS_DSP_DIR)StatisticsFunctions
FW_CMSIS_DSP_SUPPORT_DIR	= $(FW_CMSIS_DSP_DIR)SupportFunctions
FW_CMSIS_DSP_TRANSFORM_DIR= $(FW_CMSIS_DSP_DIR)TransformFunctions

FW_DISCOVERY_COMPONENTS_DIR					= $(STM32_PATH)BSP/Components/
FW_CS43L22_DIR											= $(FW_DISCOVERY_COMPONENTS_DIR)cs43l22
FW_LIS3DSH_DIR											= $(FW_DISCOVERY_COMPONENTS_DIR)lis3dsh
FW_LIS302DL_DIR											= $(FW_DISCOVERY_COMPONENTS_DIR)lis302dl
FW_COMMON_COMPONENTS_DIR						= $(FW_DISCOVERY_COMPONENTS_DIR)Common
FW_DISCOVERY_DIR										= $(STM32_PATH)BSP/STM32F4-Discovery

FW_MIDDLEWARE_DIR					= $(STM32_PATH)MiddleWares/
FW_AUDIO_PDM_DIR					= $(FW_MIDDLEWARE_DIR)STM32_Audio/PDM

FW_COMPONENT_DIR				  = $(LIBSTM32F4_PATH)bsp/components

#+ FW_USB_DIR 						= $(STM32_PATH)STM32_USB-FS-Device_Driver/
#+ FW_USB_SRCDIR 				= $(FW_USB_DIR)src
#+ FW_USB_INCDIR 				= $(FW_USB_DIR)inc

INCFLAGS 							+= -I$(LIBSTM32F4_PATH) \
												 -I$(STM32_PATH) \
												 -I$(FW_STDDRIVER_INCDIR) \
												 -I$(FW_STDDRIVER_INCDIR_LEGACY) \
												 -I$(FW_CMSIS_STM32F4_DIR) \
												 -I$(FW_CMSIS_CORE_DIR) \
												 -I$(FW_CMSIS_RTOS_DIR) \
												 -I$(FW_DISCOVERY_DIR) \
												 -I$(FW_COMMON_COMPONENTS_DIR) \
												 -I$(FW_CS43L22_DIR) \
												 -I$(FW_LIS3DSH_DIR) \
												 -I$(FW_LIS302DL_DIR) \
												 -I$(FW_AUDIO_PDM_DIR) \
												 -I$(FW_COMPONENT_DIR)

VPATH 								+= $(FW_CMSIS_STM32F4_DIR) \
												 $(FW_STDDRIVER_SRCDIR) \
												 $(FW_DISCOVERY_DIR) \
												 $(FW_COMMON_COMPONENTS_DIR) \
												 $(FW_CS43L22_DIR) \
												 $(FW_LIS3DSH_DIR) \
												 $(FW_LIS302DL_DIR) \
												 $(FW_CMSIS_DSP_BASIC_DIR) \
													$(FW_CMSIS_DSP_COMMON_DIR) \
													$(FW_CMSIS_DSP_COMPLEX_DIR) \
													$(FW_CMSIS_DSP_CONTROL_DIR) \
													$(FW_CMSIS_DSP_FAST_DIR) \
													$(FW_CMSIS_DSP_FILTER_DIR) \
													$(FW_CMSIS_DSP_MATRIX_DIR) \
													$(FW_CMSIS_DSP_STAT_DIR) \
													$(FW_CMSIS_DSP_SUPPORT_DIR) \
													$(FW_CMSIS_DSP_TRANSFORM_DIR) \
													$(FW_COMPONENT_DIR)

ALL_H_FILES_FULL			= $(wildcard $(patsubst %,%/*.h, $(PACKAGES) \
																											 $(LIBSTM32F4_PATH) \
																											 $(STM32_PATH) \
																											 $(FW_STDDRIVER_INCDIR) \
																											 $(FW_STDDRIVER_INCDIR_LEGACY) \
																											 $(FW_CMSIS_STM32F4_DIR) \
																											 $(FW_CMSIS_CORE_DIR) \
																											 $(FW_CMSIS_RTOS_DIR) \
																											 $(FW_DISCOVERY_DIR) \
																											 $(FW_COMMON_COMPONENTS_DIR) \
																											 $(FW_CS43L22_DIR) \
																											 $(FW_LIS3DSH_DIR) \
																											 $(FW_LIS302DL_DIR) \
																											 $(FW_AUDIO_PDM_DIR) \
																											 $(FW_COMPONENT_DIR)))

CORE_C_FILES_FULL 		= $(wildcard $(patsubst %,%/*.c,$(FW_CMSIS_STM32F4_DIR)))
CORE_C_FILES 					= $(notdir $(CORE_C_FILES_FULL))
CORE_S_FILES_FULL			= $(wildcard $(patsubst %,%/*.S,$(FW_CMSIS_STM32F4_DIR)))
CORE_S_FILES 					= $(notdir $(CORE_S_FILES_FULL))
DRIVERS_C_FILES_FULL	= $(wildcard $(patsubst %,%/*.c,$(FW_STDDRIVER_SRCDIR)))
DRIVERS_C_FILES 			= $(notdir $(DRIVERS_C_FILES_FULL))
DISCOVERY_C_FILES_FULL= $(wildcard $(patsubst %,%/*.c,	$(FW_DISCOVERY_DIR) \
																												$(FW_COMMON_COMPONENTS_DIR) \
																												$(FW_CS43L22_DIR) $(FW_LIS3DSH_DIR) \
																												$(FW_LIS302DL_DIR)))
DISCOVERY_C_FILES			= $(notdir $(DISCOVERY_C_FILES_FULL))
CMSISDSP_C_FILES_FULL	= $(wildcard $(patsubst %,%/*.c,	$(FW_CMSIS_DSP_BASIC_DIR) \
																												$(FW_CMSIS_DSP_COMMON_DIR) \
																												$(FW_CMSIS_DSP_COMPLEX_DIR) \
																												$(FW_CMSIS_DSP_CONTROL_DIR) \
																												$(FW_CMSIS_DSP_FAST_DIR) \
																												$(FW_CMSIS_DSP_FILTER_DIR) \
																												$(FW_CMSIS_DSP_MATRIX_DIR) \
																												$(FW_CMSIS_DSP_STAT_DIR) \
																												$(FW_CMSIS_DSP_SUPPORT_DIR) \
																												$(FW_CMSIS_DSP_TRANSFORM_DIR)))
CMSISDSP_C_FILES			= $(notdir $(CMSISDSP_C_FILES_FULL))

COMPONENT_C_FILES_FULL	= $(wildcard $(patsubst %,%/*.c,	$(FW_COMPONENT_DIR)))
COMPONENT_C_FILES				= $(notdir $(COMPONENT_C_FILES_FULL))
COMPONENT_CC_FILES_FULL = $(wildcard $(patsubst %,%/*.cc,	$(FW_COMPONENT_DIR)))
COMPONENT_CC_FILES			= $(notdir $(COMPONENT_CC_FILES_FULL))

#+ USB_C_FILES 					= $(notdir $(wildcard $(patsubst %,%/*.c,$(FW_USB_SRCDIR))))

#C_FILES 							+= 	

#+ ifeq ($(USB),enabled)
#+ INCFLAGS 							+= -I$(FW_USB_INCDIR)
#+ VPATH 								+= $(FW_USB_SRCDIR)
#+ C_FILES 							+= $(USB_C_FILES)
#+ endif

# ------------------------------------------------------------------------------
# What to build
# ------------------------------------------------------------------------------
OBJ_CMSISDSP_FILES = $(CMSISDSP_C_FILES:.c=.o)
OBJS_CMSISDSP = $(patsubst %,$(BUILD_OBJ)%,$(OBJ_CMSISDSP_FILES))

OBJ_CORE_FILES = $(CORE_C_FILES:.c=.o) $(CORE_S_FILES:.S=.o)
OBJS_CORE = $(patsubst %,$(BUILD_OBJ)%,$(OBJ_CORE_FILES))

OBJ_DISCOVERY_FILES = $(DISCOVERY_C_FILES:.c=.o)
OBJS_DISCOVERY = $(patsubst %,$(BUILD_OBJ)%,$(OBJ_DISCOVERY_FILES))

OBJ_DRIVERS_FILES = $(DRIVERS_C_FILES:.c=.o) $(COMPONENT_C_FILES:.c=.o) $(COMPONENT_CC_FILES:.cc=.o)
OBJS_DRIVERS = $(patsubst %,$(BUILD_OBJ)%,$(OBJ_DRIVERS_FILES))

# OBJ_COMPONENT_FILES = $(COMPONENT_C_FILES:.c=.o) $(COMPONENT_CC_FILES:.cc=.o)
# OBJS_COMPONENT = $(patsubst %,$(BUILD_OBJ)%,$(OBJ_COMPONENT_FILES))

OBJ_FILES = $(CC_FILES:.cc=.o) $(C_FILES:.c=.o) $(AS_FILES:.S=.o)
OBJS = $(patsubst %,$(BUILD_OBJ)%,$(OBJ_FILES))

DEPS = 	$(OBJS:.o=.d) \
				$(OBJS_DRIVERS:.o=.d) \
				$(OBJS_DISCOVERY:.o=.d) \
				$(OBJS_CMSISDSP:.o=.d) \
				$(OBJS_CORE:.o=.d) \
				$(OBJS_COMPONENT:.o=.d) 


.PHONY: all clean clean_libs clean_all libs

all: $(BUILD_DIR) $(BUILD_OBJ) $(TARGET_HEX) $(TARGET_IHEX) $(TARGET_BIN) $(TARGET_SYM) $(TARGET_LSS) $(TARGET_DIS) $(TARGET_TOPSYM) size ramsize ctags

libs: $(LIBDIR) $(LIBS) | $(BUILD_DIR) 

# ------------------------------------------------------------------------------
# Main rules
# ------------------------------------------------------------------------------
$(BUILD_DIR):
	@echo "Creating directory: $@"
	@mkdir -p $(BUILD_DIR)

$(BUILD_OBJ):
	@echo "Creating directory: $@"
	@mkdir -p $(BUILD_OBJ)

$(LIBDIR):
	@echo "Creating directory: $@"
	@mkdir -p $(LIBDIR)

$(DEP_FILE): $(BUILD_DIR) $(BUILD_OBJ) $(LIBDIR) $(DEPS)
	@echo "Creating file: $@"
	@cat $(DEPS) > $(DEP_FILE)

$(BUILD_DIR)%.elf: $(OBJS)
	@echo "[LD] $@"
	@$(CC) $(LDFLAGS) $^ $(LDLIBS) -o $@

$(BUILD_DIR)%.map: $(BUILD_DIR)%.elf

.PHONY: ctags
ctags: $(CC_FILES_FULL) $(C_FILES_FULL) $(AS_FILES_FULL) $(CORE_C_FILES_FULL) $(CORE_S_FILES_FULL) $(DRIVERS_C_FILES_FULL) $(DISCOVERY_C_FILES_FULL) $(CMSISDSP_C_FILES_FULL) $(ALL_H_FILES_FULL) $(COMPONENT_C_FILES_FULL)
	@$(CTAGS) --c-kinds=+cdefglmnpstuvx -f $(TAGS) $(CC_FILES_FULL) $(C_FILES_FULL) $(AS_FILES_FULL) $(CORE_C_FILES_FULL) $(CORE_S_FILES_FULL) $(DRIVERS_C_FILES_FULL) $(DISCOVERY_C_FILES_FULL) $(CMSISDSP_C_FILES_FULL) $(ALL_H_FILES_FULL) $(COMPONENT_C_FILES_FULL)

clean_libs:
	$(REMOVE) $(LIBS)
 
clean_all:
	$(MAKE) -f $(TARGET)/Makefile clean clean_libs
 
clean:
	$(REMOVE) $(OBJS) $(OBJS_DRIVERS) $(OBJS_DISCOVERY) $(OBJS_CORE) $(OBJS_CMSISDSP) $(TARGETS) $(DEP_FILE) $(DEPS) $(LIBS) $(TAGS)
 
size: $(TARGET_SIZE)
	@cat $(TARGET_SIZE) | awk '{ print $$1+$$2 }' | tail -n1 $(FORMAT_SIZE)

ramsize: $(TARGET_SIZE)
	@cat $(TARGET_SIZE) | awk '{ print $$2+$$3 }' | tail -n1 $(FORMAT_RAMSIZE)

$(LIBSTDPERIPH): $(OBJS_DRIVERS)
	@echo "[AR] $@"
	@$(AR) -cru $@ $^
	@$(RANLIB) $@

$(LIBDISCOVERY): $(OBJS_DISCOVERY)
	@echo "[AR] $@"
	@$(AR) -cru $@ $^
	@$(RANLIB) $@

$(LIBCMSISSTM32F4): $(OBJS_CORE)
	@echo "[AR] $@"
	@$(AR) -cru $@ $^
	@$(RANLIB) $@

$(LIBCMSISDSP): $(OBJS_CMSISDSP)
	@echo "[AR] $@"
	@$(AR) -cru $@ $^
	@$(RANLIB) $@

# $(LIBCOMPONENT): $(OBJS_COMPONENT)
# 	@echo "[AR] $@"
# 	@$(AR) -cru $@ $^
# 	@$(RANLIB) $@

include $(DEP_FILE)

# ------------------------------------------------------------------------------
# Source compiling and dependency analysis
# ------------------------------------------------------------------------------
$(BUILD_OBJ)%.o: %.S
	@echo "[ASM] $<"
	@$(CC) -c -x assembler-with-cpp $(ASFLAGS) $< -o $@

$(BUILD_OBJ)%.o: %.c
	@echo "[CC] $<"
	@$(CC) -c $(CFLAGS) $< -o $@

$(BUILD_OBJ)%.o: %.cc
	@echo "[CXX] $<"
	@$(CXX) -c $(CXXFLAGS) $< -o $@

$(BUILD_OBJ)%.d: %.S
	@echo "[ASM] $@"
	@$(CC) -c -x assembler-with-cpp -MM $(ASFLAGS) $< -MF $@ -MT $(@:.d=.o)

$(BUILD_OBJ)%.d: %.c
	@echo "[CC] $@"
	@$(CC) -MM $(CFLAGS) $< -MF $@ -MT $(@:.d=.o)

$(BUILD_OBJ)%.d: %.cc
	@echo "[CXX] $@"
	@$(CXX) -MM $(CXXFLAGS) $< -MF $@ -MT $(@:.d=.o)

# ------------------------------------------------------------------------------
# Object file conversion
# ------------------------------------------------------------------------------
$(BUILD_DIR)%.ihex: $(BUILD_DIR)%.elf
	@echo "[OBJCOPY] $@"
	@$(OBJCOPY) -O ihex $< $@

$(BUILD_DIR)%.hex: $(BUILD_DIR)%.elf
	@echo "[HEXDUMP] $@"
	@$(HEXDUMP) -e '1/2 "%04x" "\n"' $< -v > $@

$(BUILD_DIR)%.bin: $(BUILD_DIR)%.elf
	@echo "[OBJCOPY] $@"
	@$(OBJCOPY) -O binary $< $@

$(BUILD_DIR)%.lss: $(BUILD_DIR)%.elf
	@echo "[OBJDUMP] $@"
	@$(OBJDUMP) -d -h -S $< > $@

$(BUILD_DIR)%.sym: $(BUILD_DIR)%.elf
	@echo "[NM] $@"
	@$(NM) -n $< > $@

$(BUILD_DIR)%.dis: $(BUILD_DIR)%.elf
	@echo "[OBJDUMP] $@"
	@$(OBJDUMP) -h -d $^ > $@

$(BUILD_DIR)$(TARGET).top_symbols: $(TARGET_ELF)
	@echo "[NM] $@"
	@$(NM) $< --size-sort -C -f bsd -r > $@

$(TARGET_SIZE): $(TARGET_ELF)
	@echo "[SIZE] $@"
	@$(SIZE) $(TARGET_ELF) > $(TARGET_SIZE)

# ------------------------------------------------------------------------------
# Bootloader merging
# ------------------------------------------------------------------------------
#$(COMBO_BIN): $(TARGET_HEX) $(BOOTLOADER_HEX)
#	cat $(TARGET_HEX) $(BOOTLOADER_HEX) | \
#	awk -f libstm32f4/programming/merge_hex.awk > $(COMBO_HEX)
#	$(OBJCOPY) -I ihex -O binary $(COMBO_HEX) $(COMBO_BIN)

# ------------------------------------------------------------------------------
# Firmware flashing
# ------------------------------------------------------------------------------
upload: $(UPLOAD_COMMAND)

### serial upload needs to boot in system memory via USART1
#image_serial:
#	python $(STM32LOADER_PATH)stm32loader.py \
#	-p $(PGM_SERIAL_PORT) \
#	-b $(PGM_SERIAL_BAUD_RATE) \
#	-r -l 131072 image.bin
#
#upload_serial: $(TARGET_BIN)
#	python $(STM32LOADER_PATH)stm32loader.py \
#	-p $(PGM_SERIAL_PORT) \
#	-b $(PGM_SERIAL_BAUD_RATE) \
#	-e $(PGM_SERIAL_VERIFY) \
#	-a $(BASE_ADDRESS) \
#	-w $(TARGET_BIN)
#
#upload_serial_no_erase: $(TARGET_BIN)
#	python $(STM32LOADER_PATH)stm32loader.py \
#	-p $(PGM_SERIAL_PORT) \
#	-b $(PGM_SERIAL_BAUD_RATE) $(PGM_SERIAL_VERIFY)\
#	-a $(BASE_ADDRESS) \
#	-w $(TARGET_BIN)
#
#upload_combo_serial: $(COMBO_BIN)
#	python $(STM32LOADER_PATH)stm32loader.py \
#	-p $(PGM_SERIAL_PORT) \
#	-b $(PGM_SERIAL_BAUD_RATE) \
#	-e $(PGM_SERIAL_VERIFY) \
#	-w $(COMBO_BIN)
#
#upload_combo_serial_no_erase: $(COMBO_BIN)
#	python $(STM32LOADER_PATH)stm32loader.py \
#	-p $(PGM_SERIAL_PORT) \
#	-b $(PGM_SERIAL_BAUD_RATE) $(PGM_SERIAL_VERIFY)\
#	-w $(COMBO_BIN)
#
FLASH_INFO_JTAG_CMD = $(OPENOCD_PATH)openocd \
													-f board/stm32f4discovery.cfg \
													-f $(OPENOCD_SCRIPTS_PATH)prelude_$(FAMILY).cfg \
													-c "flash info 0" \
													-f $(OPENOCD_SCRIPTS_PATH)postlude.cfg

DUMP_FLASH_JTAG_CMD = $(OPENOCD_PATH)openocd \
													-f board/stm32f4discovery.cfg \
													-f $(OPENOCD_SCRIPTS_PATH)prelude_$(FAMILY).cfg \
													-c "dump_image dump_flash.hex $(FLASH_BASE_ADDRESS) $(FLASH_SIZE)" \
													-f $(OPENOCD_SCRIPTS_PATH)postlude.cfg

UPLOAD_JTAG_CMD = $(OPENOCD_PATH)openocd \
													-f board/stm32f4discovery.cfg \
													-c "program $(TARGET_BIN) verify reset exit $(FLASH_BASE_ADDRESS)"

# Because of a weird OpenOCD bug, this command sometimes fails with an
# "Error: error executing stm32x flash write algorithm" error. Retrying
# immediately always solves it. for this pout a ||  with previous command
dump_flash_jtag:
	$(DUMP_FLASH_JTAG_CMD)

flash_info_jtag:
	$(FLASH_INFO_JTAG_CMD)

upload_jtag: $(TARGET_BIN)
	$(UPLOAD_JTAG_CMD)

# .PHONY:  upload_bundle_serial upload_serial upload_serial_no_erase 
.PHONY: upload upload_jtag flash_info_jtag dump_flash_jtag

# ------------------------------------------------------------------------------
# Firmware debug
# ------------------------------------------------------------------------------
debug_server: $(TARGET_BIN)
	$(OPENOCD_PATH)openocd \
	-f board/stm32f4discovery.cfg \
	-c "init" \
	-c "reset halt" \
	$(OPENOCDSEMIHOSTCMD) \
	-c "poll"

debug_client:
	$(DB) --tui --eval-command="target remote localhost:3333" \
							--eval-command="b main" \
							$(TARGET_ELF)

ddd_client:
	$(DDD) --eval-command="target remote localhost:3333" --eval-command="b main" --debugger $(DB) $(TARGET_ELF)

debug_gdb:
	@gnome-terminal -e "make -f $(TARGET)/Makefile debug_server" 
	sleep 8s
	@gnome-terminal -e "make -f $(TARGET)/Makefile debug_client" 

.PHONY: debug_client debug_server ddd_client debug_gdb
