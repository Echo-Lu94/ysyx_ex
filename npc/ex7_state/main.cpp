#include <nvboard.h>
#include <Vps2_keyboard.h>

static TOP_NAME dut;

void nvboard_bind_all_pins(Vps2_keyboard *ps2_keyboard);

static void single_cycle(){
    dut.clk =0; dut.eval();
    dut.clk =1; dut.eval();
}

static void reset(int n){
    dut.rst_n = 0;
    while(n-- > 0) single_cycle();
    dut.rst_n = 1;
}

int main(){
    nvboard_bind_all_pins(&dut);
    nvboard_init();
    
    reset(10);
    
    while(1){
        nvboard_update();
        single_cycle();
    }
}
        
