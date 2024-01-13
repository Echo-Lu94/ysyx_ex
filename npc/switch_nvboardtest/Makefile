include $(YSYX_HOME)/Makefile
TOP=switch_test
NXDC_FILES =constr/switch_test.nxdc
INC_PATH ?=

VERILATOR_CFLAGS += -MMD --build -cc  \
				-O3 --x-assign fast --x-initial fast --noassert

BUILD_DIR = ./build
OBJ_DIR = $(BUILD_DIR)/obj_dir
BIN = $(BUILD_DIR)/$(TOP)

default: $(BIN)
$(shell mkdir -p $(BUILD_DIR))

#constraint file
SRC_AUTO_BIND = $(abspath $(BUILD_DIR)/auto_bind.cpp)
$(SRC_AUTO_BIND): $(NXDC_FILES)
	python3 $(NVBOARD_HOME)/scripts/auto_pin_bind.py $^ $@

#project source
#VSRCS = $(wildcard ./*.v)
#CSRCS = $(wildcard ./*.cpp)
#CSRCS += $(SRC_AUTO_BIND)
VSRCS = $(shell find $(abspath ./vsrc) -name "*.v")
CSRCS = $(shell find $(abspath ./csrc) -name "*.c" -or -name "*.cc" -or -name "*.cpp")
CSRCS += $(SRC_AUTO_BIND)

#rules for NVBoard
include $(NVBOARD_HOME)/scripts/nvboard.mk

#rules for verilator
INCFLAGS = $(addprefix -I, $(INC_PATH))
CFLAGS += $(INCFLAGS) -DTOP_NAME="\"V$(TOP)\""
LDFLAGS += -lSDL2 -lSDL2_image

$(BIN):$(VSRCS) $(CSRCS) $(NVBOARD_ARCHIVE)
	@rm -rf $(OBJ_DIR)
	verilator $(VERILATOR_CFLAGS) \
		--top-module $(TOP) $^ \
		$(addprefix -CFLAGS , $(CFLAGS)) $(addprefix -LDFLAGS , $(LDFLAGS)) \
		--Mdir $(OBJ_DIR) --exe -o $(abspath $(BIN))
	$(call git_commit, "sim RTL") # DO NOT REMOVE THIS LINE!!!

all:default

run: $(BIN)
	@$^

clean:
	rm -rf ./obj_dir
	rm -rf waveform.vcd
