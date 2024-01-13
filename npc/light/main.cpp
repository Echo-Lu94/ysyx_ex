#include <nvboard.h>
#include <Vlight.h>

//#include <stdio.h>
//#include <stdlib.h>
//#include <assert.h>
//#include "verilated.h"
//#include "verilated_vcd_h"

//#define MAX_SIM_TIME 20
//vluint64_t sim_time=0;

static TOP_NAME dut;

void nvboard_bind_all_pins(Vlight *light);

static void single_cycle() {
  dut.clk = 0; dut.eval();
  dut.clk = 1; dut.eval();
}

static void reset(int n) {
  dut.rst = 1;
  while (n -- > 0) single_cycle();
  dut.rst = 0;
}

int main(){
    nvboard_bind_all_pins(&dut);
    nvboard_init();

    reset(10);
    while(1){
        nvboard_update();
        single_cycle();
        
    }
//    nvboard_quit();
}

