#include <nvboard.h>
#include <Valu.h>

static TOP_NAME dut;

void nvboard_bind_all_pins(Valu *alu);

static void single_cycle(){
    dut.clk =0; dut.eval();
    dut.clk =1; dut.eval();
}

int main(){
    nvboard_bind_all_pins(&dut);
    nvboard_init();

    while(1){
        nvboard_update();
        single_cycle();
    }
}
        
