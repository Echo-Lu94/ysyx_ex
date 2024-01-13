PROJ_PATH = $(shell pwd)/../

DESIGN ?= encode83
SDC_FILE ?= $(PROJ_PATH)/DC/shift.sdc
RTL_FILES ?=$(PROJ_PATH)/*.v
export CLK_FREQ_MHZ ?= 500

RESULT_DIR = $(PROJ_PATH)/DC/result/$(DESIGN)-$(CLK_FREQ_MHZ)MHz
NETLIST_V  = $(RESULT_DIR)/$(DESIGN).netlist.v
TIMING_RPT = $(RESULT_DIR)/$(DESIGN).rpt

init:
	bash -c "$$(wget -O - https://ysyx.oscc.cc/slides/resources/scripts/init-yosys-sta.sh)"

syn: $(NETLIST_V)
$(NETLIST_V): $(RTL_FILES) yosys.tcl
	mkdir -p $(@D)
	echo tcl yosys.tcl $(DESIGN) \"$(RTL_FILES)\" $(NETLIST_V) | yosys -l $(@D)/yosys.log -s -

sta: $(TIMING_RPT)
$(TIMING_RPT): $(SDC_FILE) $(NETLIST_V)
	LD_LIBRARY_PATH=bin/ ./bin/iSTA $(PROJ_PATH)/DC/sta.tcl $(DESIGN) $(SDC_FILE) $(NETLIST_V)

clean:
	-rm -rf result/ bin/ *.dot *.pdf *.ps

.PHONY: init syn sta clean
