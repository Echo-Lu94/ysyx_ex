TOP=npc
TEST_NAME=dummy
#INC_PATH ?= ./include/ 
DEFINE=
BUILD_DIR=$(NPC_HOME)/build

OBJ_DIR = $(BUILD_DIR)
BINARY = $(BUILD_DIR)
#------------------------------------------------------------
#                    verilator flags
#------------------------------------------------------------
#Generate C++ in executable form
VERILATOR_FLAGS += -cc --exe
#Create .d dependencies
VERILATOR_FLAGS += -MMD -MP
#Optimize,assign non-initial Xs to this value
VERILATOR_FLAGS += --x-assign fast --x-initial fast
#Enable all styles warnings
#VERILATOR_FLAGS += -Wall 
#Enable all assertions
VERILATOR_FLAGS += --assert 
# Generate coverage analysis
# VERILATOR_FLAGS += --coverage
# Run Verilator in debug mode
#VERILATOR_FLAGS += --debug
# Add this trace to get a backtrace in gdb
#VERILATOR_FLAGS += --gdbbt
#VERILATOR_FLAGS += --top-module top +incdir+vsrc/include
VERILATOR_FLAGS += --build -Mdir $(BUILD_DIR)

#--------------------------------------------------
#                     SRCS
#--------------------------------------------------
#project source
VSRCS = $(shell find $(abspath ./vsrc) -name "*.v")
CSRCS = $(shell find $(abspath ./csrc) -name "*.c" -or -name "*.cc" -or -name "*.cpp")
#CSRCS = $(shell find -name "*.c" -or -name "*.cpp")

INC_VPATH += $(NPC_HOME)/vsrc/include/
INC_CPATH := $(NPC_HOME)/csrc/include/ $(INC_CPATH)
#rules for verilator
VINCLUDES = $(addprefix -I, $(INC_VPATH))
CINCLUDES = $(addprefix -I, $(INC_CPATH))

-include $(NPC_HOME)/csrc/include/config/auto.conf
-include $(NPC_HOME)/csrc/include/config/auto.conf.cmd

#CFLAGS += -CFLAGS -DTOP_NAME="\"V$(TOP)\"" "${CINCLUDES}"
CFLAGS += ${CINCLUDES} -D__GUEST_ISA__="risc32"
LDFLAGS += $(if $(CONFIG_TARGET_NATIVE_ELF), -lreadline -ldl -pie,)

ifneq ($(CONFIG_ITRACE)$(CONFIG_IQUEUE),)
CXXSRC = csrc/utils/disasm.cc
#$(info "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!")
#LIBS += $(shell llvm-config --libs) 
#CXXFLAGS += $(shell llvm-config --cxxflags) -fPIE
LDFLAGS += $(shell llvm-config --libs) 
LDFLAGS += $(shell llvm-config --cxxflags) -fPIE
#LDFLAGS += -L.
endif

#OBJS = $(CSRCS:%.c=$(OBJ_DIR)/%.o) $(CXXSRC:%.cc=$(OBJ_DIR)/%.o)
#OBJS = $(CXXSRC:%.cc=$(BUILD_DIR)/%.o)
#$(info "@@@@@@@@@@$(OBJS)")

#CFLAGS += -D$(DEFINE)
IMAGE ?= $(YSYX_HOME)/am-kernels/tests/cpu-tests/build/$(TEST_NAME)-riscv32e-npc.bin
NPC_ARGS :=  -l ${BUILD_DIR}/npc-log.txt
NPC_ARGS += -b
NPC_ARGS += -e $(shell dirname $(IMAGE).elf)/$(TEST_NAME)-riscv32e-npc.elf
#NPC_ARGS +=--log ./run.log --diff=$(NEMU_HOME)/build/riscv32e-nemu-interpreter-so

#useless, need function main()
#comrtl:clean
#	@echo "#------------------------------------"
#	@echo "#        MODULE VERILATING           "
#	@echo "#------------------------------------"
#	verilator ${VERILATOR_FLAGS} -I${INC_VPATH} ${VSRCS} -top ${TOP}

#$(OBJ_DIR)/%.o: %.c
#	@echo + CC $<
#	@mkdir -p $(dir $@)
#	@gcc $(CFLAGS) -c -o $@ $<
#	$(call call_fixdep, $(@:.o=.d), $@)
#
#$(OBJ_DIR)/%.o: %.cc
#	@echo + CXX $<
#	@mkdir -p $(dir $@)
#	@g++ $(CFLAGS) $(CXXFLAGS) -c -o $@ $<
#	$(call call_fixdep, $(@:.o=.d), $@)
#
#$(BINARY): $(OBJS) 
#	@echo + LD $@
#	@g++ -o $@ $(OBJS) $(LDFLAGS) $(LIBS)

#$(info "!!!!!!!!!!!!!!!$(CFLAGS)")
comtb:clean 
	@echo "#------------------------------------"
	@echo "#     TESTBENCH VERILATING           "
	@echo "#------------------------------------" 
#	@mkdir -p $(dir $(BUILD_DIR)/%.c)
#	g++ ${CFLAGS}  ${CXXFLAGS} $(LIBS) -c -o $(BUILD_DIR)/disasm.o $(CXXSRC)
#	$(call call_fixdep, $(@:.o=.d), $(BUILD_DIR)/disasm.o)
	verilator --trace  ${VERILATOR_FLAGS} ${VINCLUDES} ${VSRCS} -top ${TOP} \
		$(addprefix -CFLAGS , ${CFLAGS}) $(addprefix -LDFLAGS , ${LDFLAGS}) \
		${CSRCS} 

sim:comtb
	@echo "#------------------------------------"
	@echo "#        	BUILDING SIM            "
	@echo "#------------------------------------"
	$(BUILD_DIR)/V$(TOP) $(NPC_ARGS) $(IMAGE)

wave:
	gtkwave waveform.vcd

clean:
	rm -rf $(BUILD_DIR) ./waveform.vcd


