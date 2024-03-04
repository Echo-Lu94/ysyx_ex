#电路名xxx.v，testbench名tb_xxx.cpp，使用make MODULE='xxx' '执行模式‘进行自动化仿真
#执行模式
#1. sim: 执行仿真流程，并生成波形文件
#2. build: 生成仿真所需文件和波形文件
#3. wave: 查看波形文件
#4. module_verilate: 仅生成电路模块的C++文件
#5. testbench_verilate: 生成电路模块和testbench的C++文件

TOP=npc
NXDC_FILES = 
INC_PATH ?= ./include/ 
MODULE=npc
DEFINE=
VERILATOR_CFLAGS += -Wall -MMD --build -cc  \
				-O3 --x-assign fast --x-initial fast --noassert

VERILATOR_FLAGS += -sv -cc +verilog2001ext+ --x-assign fast --x-initial fast --noassert
#project source
VSRCS = $(shell find $(abspath ./) -name "*.v")
CSRCS = $(shell find $(abspath ./) -name "*.c" -or -name "*.cc" -or -name "*.cpp")
CSRCS += $(SRC_AUTO_BIND)


#rules for verilator
INCFLAGS = $(addprefix -I, $(INC_PATH))
#INCFLAGS = -I
CFLAGS += $(INCFLAGS) -DTOP_NAME="\"V$(TOP)\""
LDFLAGS += -lSDL2 -lSDL2_image
#CFLAGS += -D$(DEFINE)


#default:
#ifndef TOP
#	@echo "Set TOP name"
#else
#	@echo "Select the execution mode: sim, build, wave, module_verilate, testbench_verilate"
#endif
#
#.PHONY: sim
sim:waveform.vcd
#
#.PHONY: build
build: ./obj_dir/V${MODULE}
#
#.PHONY: module_verilate
module_verilate: .module.verilate
#
#.PHONY: testbench_verilate
testbench_verilate: .testbench.verilate
#
#.PHONY: wave
wave: waveform.vcd
	@echo "### SIMULATING ###"
	gtkwave waveform.vcd
#
waveform.vcd: ./obj_dir/V${MODULE}
	@echo "### WAVE ###"
	./obj_dir/V${MODULE} | tee sim.log
#
##生成仿真可执行文件“
./obj_dir/V${MODULE}: .testbench.verilate
	@echo "### BUILDING SIM ###"
	make -C obj_dir -f V${MODULE}.mk V${MODULE}
#
#.testbench.verilate: ${MODULE}.v tb_${MODULE}.cpp
.testbench.verilate: ${VSRCS} ${CSRCS}
	@echo "### TESTBENCH VERILATING ###"
	verilator --trace ${VERILATOR_FLAGS} -I${INC_PATH} ${VSRCS} -top ${TOP} --exe ${CSRCS}
#
.module.verilate: ${VSRCS}
	@echo "### MODULE VERILATING ###"
	verilator ${VERILATOR_FLAGS} -I${INC_PATH} ${VSRCS} -top ${TOP}
#
.PHONY: clean
clean:
	rm -rf $(BUILD_DIR)
	rm -rf waveform.vcd
	rm -rf ./obj_dir
.PHONY: default all clean run
