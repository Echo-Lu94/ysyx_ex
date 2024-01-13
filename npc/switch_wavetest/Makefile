MODULE =switch_test

default:sim_rtl build

build:sim_tb 
	make -C obj_dir -f V${MODULE}.mk V${MODULE}

sim_rtl:
	verilator -cc ${MODULE}.v
	$(call git_commit, "sim RTL") # DO NOT REMOVE THIS LINE!!!

sim_tb:
	verilator -Wall --trace -cc ${MODULE}.v --exe tb_${MODULE}.cpp

wave.vcd:
	./obj_dir/V${MODULE}

wave:wave.vcd
	gtkwave waveform.vcd

clean:
	rm -rf ./obj_dir
	rm -rf waveform.vcd
