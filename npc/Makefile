TOP=npc
#TEST_NAME=dummy
#INC_PATH ?= ./include/ 
DEFINE=
BUILD_DIR=$(NPC_HOME)/build
#ALL ?= dummy

TEST_HOME=$(YSYX_HOME)/am-kernels/tests/cpu-tests/
ALLS = $(basename $(notdir $(shell find $(TEST_HOME)tests/. -name "*.c")))
#$(info "$ALL")
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
VERILATOR_FLAGS += --public
VERILATOR_FLAGS += --timing
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
CPPFLAGS += -O2 -MMD -Wall -Werror 
LDFLAGS += $(if $(CONFIG_TARGET_NATIVE_ELF), -lreadline -ldl -pie,)
CFLAGS += $(if $(CONFIG_CC_ASAN), -fsanitize=address,)

#------------------------------------------------
#                   DIFFTEST
#------------------------------------------------
ifdef CONFIG_DIFFTEST
DIFF_REF_PATH = $(NEMU_HOME)/$(call remove_quote,$(CONFIG_DIFFTEST_REF_PATH))
#DIFF_REF_SO = $(DIFF_REF_PATH)/build/$(call remove_quote,$(CONFIG_ISA))-$(call remove_quote,$(CONFIG_DIFFTEST_REF_NAME))-$(call remove_quote,$(CONFIG_ENGINE))-so
DIFF_REF_SO = $(DIFF_REF_PATH)/build/riscv32-nemu-interpreter-so
#SHARE=$(if $(CONFIG_TARGET_SHARE),1,0)
#$(info "````````````$(call remove_quote,$(CONFIG_ISA))``````$(DIFF_REF_SO)")
NPC_ARGS = --diff=$(DIFF_REF_SO)
#ifeq ($(SHARE),1)
#$(info "``````````")
##SO = -so
#CFLAGS  += -fPIC -fvisibility=hidden
#LDFLAGS += -shared -fPIC
#endif
endif


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
IMAGE ?= $(YSYX_HOME)/am-kernels/tests/cpu-tests/build/$(ALL)-riscv32e-npc.bin
NPC_ARGS +=  -l ${BUILD_DIR}/npc-log.txt
NPC_ARGS += -b
NPC_ARGS += -e $(shell dirname $(IMAGE).elf)/$(ALL)-riscv32e-npc.elf
NPC_ARGS +=--log ${BUILD_DIR}/run.log 

#useless, need function main()
#comrtl:clean
#	@echo "#------------------------------------"
#	@echo "#        MODULE VERILATING           "
#	@echo "#------------------------------------"
#	verilator ${VERILATOR_FLAGS} -I${INC_VPATH} ${VSRCS} -top ${TOP}

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

sim:comtb $(DIFF_REF_SO)
	@echo "#------------------------------------"
	@echo "#        	BUILDING SIM            "
	@echo "#------------------------------------"
	$(BUILD_DIR)/V$(TOP) $(NPC_ARGS) $(IMAGE)
	@mv ./waveform.vcd ./build

wave:
	gtkwave ./build/waveform.vcd

clean:
	rm -rf $(BUILD_DIR) ./waveform.vcd

#all:run
#	@echo "" $(ALL)

run: 
	@$(foreach i, $(ALLS), \
		make sim ALL=$i; \
	)
#	grep -n 'npc:' > .result 

#run: 
#	@$(foreach i, $(ALLS), \
#		@if make sim ALL=$i; then \
#			printf "[%14s] $(COLOR_GREEN)PASS!$(COLOR_NONE)\n" $* >> .result; \
#		else \
#			printf "[%14s] $(COLOR_RED)FAIL!$(COLOR_NONE)\n" $* >> .result; \
#		fi)
#	@echo "" $(ALL)

all:run
	@cat .result

#	@for i in $(ALLS); do \
#		make sim ALL=$$i; \ 
#	done
