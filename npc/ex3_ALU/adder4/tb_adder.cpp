#include "verilated.h"
#include "verilated_vcd_c.h"
#include "Vadder.h"

VerilatedContext* contextp = NULL;
VerilatedVcdC* tfp = NULL;

static Vadder* top;

void step_and_dump_wave(){
    top->eval();
    contextp->timeInc(1);
    tfp->dump(contextp->time());
}

void sim_init(){
    contextp = new VerilatedContext;
    tfp = new VerilatedVcdC;
    top=new Vadder;
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

//add
    top->fun_sel=0b0; top->a =0b0111; top->b=0b0111; step_and_dump_wave();//7+7=0b1110,c0,o1,z0
                      top->a =0b0111; top->b=0b1000; step_and_dump_wave();//7+(-8)=0b1111,c0,o0,z0
                      top->a =0b1111; top->b=0b1000; step_and_dump_wave();//(-1)+(-8)=0b0111,c1,o1,z0
                      top->a =0b1111; top->b=0b1001; step_and_dump_wave();//(-1)+(-7)=0b1000,c1,o0,z0
                      top->a =0b1111; top->b=0b0001; step_and_dump_wave();//(-1)+1=0b0000,c1,o0,z1
//sub
    top->fun_sel=0b1; top->a =0b1111; top->b=0b1000; step_and_dump_wave();//(-1)-(-8)=0b0111,c1,o1,z0
                      top->a =0b0001; top->b=0b0111; step_and_dump_wave();//1-7=0b1010,c0,o0,z0
                      top->a =0b0000; top->b=0b0010; step_and_dump_wave();//0-2=0b1110,c0,o0,z0
                      top->a =0b0111; top->b=0b0010; step_and_dump_wave();//7-2=0b0101,c1,o0,z0
                      top->b =0b1111; top->a=0b1000; step_and_dump_wave();//(-8)-(-1)=0b1001,c0,o0,z0
                      top->a =0b1000; top->b=0b0001; step_and_dump_wave();//(-8)-1=0b0111,c1,o1,z0
                      top->a =0b1000; top->b=0b1000; step_and_dump_wave();//(-8)-(-8)=0b0000,c1,o1,z1
                      top->a =0b1111; top->b=0b1001; step_and_dump_wave();//(-1)-(-7)=0b0110,c1,o0,z0
//减数涉及到-8都会显示溢出              

    sim_exit();
}
