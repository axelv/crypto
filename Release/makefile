################################################################################
# Automatically-generated file. Do not edit!
################################################################################

SHELL = cmd.exe

CG_TOOL_ROOT := C:/ti/ccsv5/tools/compiler/c6000_7.4.4

ORDERED_OBJS += \
$(GEN_CMDS__FLAG) \
"./asymmetric/signatures.obj" \
"./asymmetric/monmult.obj" \
"./asymmetric/monexp.obj" \
"./bbs/bbs.obj" \
"./ciphers/sha2/sha2_wrapper.obj" \
"./ciphers/sha2/sha2.obj" \
"./ocb/ocb.obj" \
"./ocb/aes.obj" \
"./packman/packman_slave.obj" \
"./packman/packman_master.obj" \
"./sts/sts_slave.obj" \
"./sts/sts_master.obj" \
"./sts/slave_keys.obj" \
"./sts/master_keys.obj" \
"./sts/dh.obj" \
"./tools/tools.obj" \
"./main.obj" \
"./exit.obj" \
"./crypto_wrapper.obj" \
"../C6748.cmd" \
-l"libc.a" \

-include ../makefile.init

RM := DEL /F
RMDIR := RMDIR /S/Q

# All of the sources participating in the build are defined here
-include sources.mk
-include subdir_vars.mk
-include tools/subdir_vars.mk
-include sts/subdir_vars.mk
-include packman/subdir_vars.mk
-include ocb/subdir_vars.mk
-include ciphers/sha2/subdir_vars.mk
-include bbs/subdir_vars.mk
-include asymmetric/subdir_vars.mk
-include subdir_rules.mk
-include tools/subdir_rules.mk
-include sts/subdir_rules.mk
-include packman/subdir_rules.mk
-include ocb/subdir_rules.mk
-include ciphers/sha2/subdir_rules.mk
-include bbs/subdir_rules.mk
-include asymmetric/subdir_rules.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(S_DEPS)),)
-include $(S_DEPS)
endif
ifneq ($(strip $(S_UPPER_DEPS)),)
-include $(S_UPPER_DEPS)
endif
ifneq ($(strip $(S62_DEPS)),)
-include $(S62_DEPS)
endif
ifneq ($(strip $(C64_DEPS)),)
-include $(C64_DEPS)
endif
ifneq ($(strip $(ASM_DEPS)),)
-include $(ASM_DEPS)
endif
ifneq ($(strip $(CC_DEPS)),)
-include $(CC_DEPS)
endif
ifneq ($(strip $(S55_DEPS)),)
-include $(S55_DEPS)
endif
ifneq ($(strip $(C67_DEPS)),)
-include $(C67_DEPS)
endif
ifneq ($(strip $(C??_DEPS)),)
-include $(C??_DEPS)
endif
ifneq ($(strip $(CLA_DEPS)),)
-include $(CLA_DEPS)
endif
ifneq ($(strip $(CPP_DEPS)),)
-include $(CPP_DEPS)
endif
ifneq ($(strip $(S??_DEPS)),)
-include $(S??_DEPS)
endif
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
ifneq ($(strip $(C62_DEPS)),)
-include $(C62_DEPS)
endif
ifneq ($(strip $(CXX_DEPS)),)
-include $(CXX_DEPS)
endif
ifneq ($(strip $(C++_DEPS)),)
-include $(C++_DEPS)
endif
ifneq ($(strip $(ASM_UPPER_DEPS)),)
-include $(ASM_UPPER_DEPS)
endif
ifneq ($(strip $(K_DEPS)),)
-include $(K_DEPS)
endif
ifneq ($(strip $(C43_DEPS)),)
-include $(C43_DEPS)
endif
ifneq ($(strip $(S67_DEPS)),)
-include $(S67_DEPS)
endif
ifneq ($(strip $(SA_DEPS)),)
-include $(SA_DEPS)
endif
ifneq ($(strip $(S43_DEPS)),)
-include $(S43_DEPS)
endif
ifneq ($(strip $(OPT_DEPS)),)
-include $(OPT_DEPS)
endif
ifneq ($(strip $(S64_DEPS)),)
-include $(S64_DEPS)
endif
ifneq ($(strip $(C_UPPER_DEPS)),)
-include $(C_UPPER_DEPS)
endif
ifneq ($(strip $(C55_DEPS)),)
-include $(C55_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 

# All Target
all: DSP.out

# Tool invocations
DSP.out: $(OBJS) $(CMD_SRCS) $(GEN_CMDS)
	@echo 'Building target: $@'
	@echo 'Invoking: C6000 Linker'
	"C:/ti/ccsv5/tools/compiler/c6000_7.4.4/bin/cl6x" -mv6740 --abi=coffabi -O2 --display_error_number --diag_warning=225 --diag_wrap=off -z -m"DSP.map" -i"C:/ti/ccsv5/tools/compiler/c6000_7.4.4/lib" -i"C:/ti/ccsv5/tools/compiler/c6000_7.4.4/include" --reread_libs --warn_sections --display_error_number --diag_wrap=off --xml_link_info="DSP_linkInfo.xml" --rom_model -o "DSP.out" $(ORDERED_OBJS)
	@echo 'Finished building target: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) $(C6000_EXECUTABLE_OUTPUTS__QUOTED) "DSP.out"
	-$(RM) "crypto_wrapper.pp" "exit.pp" "main.pp" "tools\tools.pp" "sts\dh.pp" "sts\master_keys.pp" "sts\slave_keys.pp" "sts\sts_master.pp" "sts\sts_slave.pp" "packman\packman_master.pp" "packman\packman_slave.pp" "ocb\aes.pp" "ocb\ocb.pp" "ciphers\sha2\sha2.pp" "ciphers\sha2\sha2_wrapper.pp" "bbs\bbs.pp" "asymmetric\monexp.pp" "asymmetric\monmult.pp" "asymmetric\signatures.pp" 
	-$(RM) "crypto_wrapper.obj" "exit.obj" "main.obj" "tools\tools.obj" "sts\dh.obj" "sts\master_keys.obj" "sts\slave_keys.obj" "sts\sts_master.obj" "sts\sts_slave.obj" "packman\packman_master.obj" "packman\packman_slave.obj" "ocb\aes.obj" "ocb\ocb.obj" "ciphers\sha2\sha2.obj" "ciphers\sha2\sha2_wrapper.obj" "bbs\bbs.obj" "asymmetric\monexp.obj" "asymmetric\monmult.obj" "asymmetric\signatures.obj" 
	-@echo 'Finished clean'
	-@echo ' '

.PHONY: all clean dependents
.SECONDARY:

-include ../makefile.targets

