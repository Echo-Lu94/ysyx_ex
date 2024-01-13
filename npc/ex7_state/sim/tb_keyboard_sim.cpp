#include "verilated.h"
#include "verilated_vcd_c.h"
#include "Vkeyboard_sim.h"

VerilatedContext* contextp = NULL;
VerilatedVcdC* tfp = NULL;

static Vkeyboard_sim* top;

void step_and_dump_wave(){
    //刷新电路状态
    top->eval();
    //增加仿真时间
    contextp->timeInc(1);
    //dump数据
    tfp->dump(contextp->time());
}

void sim_init(){
    contextp = new VerilatedContext;
    tfp = new VerilatedVcdC;
    top=new Vkeyboard_sim;
    contextp->traceEverOn(true);
    top->trace(tfp,0);
    tfp->open("dump.vcd");
}

void sim_exit(){
    step_and_dump_wave();
    tfp->close();
}

int main(){
    sim_init();

while(!contextp->gotFinish()){
   step_and_dump_wave();
}

    sim_exit();
    delete top;
    return 0;
}
