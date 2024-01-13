
include $(YSYX_HOME)/Makefile

TOP=light
NXDC_FILES =light.nxdc
INC_PATH ?=

VERILATOR_CFLAGS += -MMD --build -cc  \
				-O3 --x-assign fast --x-initial fast --noassert

BUILD_DIR = ./build
OBJ_DIR = $(BUILD_DIR)/obj_dir
BIN = $(BUILD_DIR)/$(TOP)

#default: $(BIN)
#终极目标default，依赖./build/${TOP}
$(shell mkdir -p $(BUILD_DIR))

#constraint file
SRC_AUTO_BIND = $(abspath $(BUILD_DIR)/auto_bind.cpp)
$(SRC_AUTO_BIND): $(NXDC_FILES)
	python3 $(NVBOARD_HOME)/scripts/auto_pin_bind.py $^ $@
	#python3 $(NVBOARD_HOME)/scripts/auto_pin_bind.py $(NXDC_FILES) $(SRC_AUTO_BIND) 
	#用来生成auto_bin.cpp

#project source
VSRCS = $(shell find $(abspath ./) -name "*.v")
CSRCS = $(shell find $(abspath ./) -name "*.c" -or -name "*.cc" -or -name "*.cpp")
CSRCS += $(SRC_AUTO_BIND)

#rules for NVBoard
include $(NVBOARD_HOME)/scripts/nvboard.mk

#rules for verilator
INCFLAGS = $(addprefix -I, $(INC_PATH))
#INCFLAGS = -I
CFLAGS += $(INCFLAGS) -DTOP_NAME="\"V$(TOP)\""
#CFLAGS = -I -DTOP_NAME="\"Vlight\""
LDFLAGS += -lSDL2 -lSDL2_image

$(BIN):$(VSRCS) $(CSRCS) $(NVBOARD_ARCHIVE)
#./build/light文件的建立依赖于 .v .cpp文件以及verilator仿真可执行文件
	@rm -rf $(OBJ_DIR)
	verilator $(VERILATOR_CFLAGS) \
		--top-module $(TOP) $^ \
		$(addprefix -CFLAGS , $(CFLAGS)) $(addprefix -LDFLAGS , $(LDFLAGS)) \
		--Mdir $(OBJ_DIR) --exe -o $(abspath $(BIN))
#--Mdir 将生成的文件都放入obj_dir目录中，--exe -o ./build/${TOP}产生可执行文件并指定名字为./build/${TOP}
	$(call git_commit, "sim RTL") # DO NOT REMOVE THIS LINE!!!

all:$(BIN)

run: $(BIN)
	@$^
#执行可执行文件

clean:
	rm -rf $(BUILD_DIR)
	rm -rf waveform.vcd

.PHONY: default all clean run
