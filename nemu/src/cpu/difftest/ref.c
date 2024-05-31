/***************************************************************************************
* Copyright (c) 2014-2022 Zihao Yu, Nanjing University
*
* NEMU is licensed under Mulan PSL v2.
* You can use this software according to the terms and conditions of the Mulan PSL v2.
* You may obtain a copy of Mulan PSL v2 at:
*          http://license.coscl.org.cn/MulanPSL2
*
* THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
* EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
* MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
*
* See the Mulan PSL v2 for more details.
***************************************************************************************/

#include <isa.h>
#include <cpu/cpu.h>
#include <difftest-def.h>
#include <memory/paddr.h>

#define NR_GPR MUXDEF(CONFIG_RVE, 16, 32)
//在DUT host memory 的'buf'和REF guest memory的'addr'之间拷贝n字节
//direction指定拷贝方向，difftest_to_dut表示往DUT拷贝，difftest_to_ref表示往REF拷贝
__EXPORT void difftest_memcpy(paddr_t addr, void *buf, size_t n, bool direction) {
    if(direction == DIFFTEST_TO_REF){
        memcpy(guest_to_host(addr), buf, n);
    }else {
        assert(0);
    }
}

//direction为difftest_to_dut时，获取REF的寄存器状态到dut，，往dut拷贝
//direction为difftest_to_ref时，设置REF的寄存器状态为dut，，往ref拷贝
__EXPORT void difftest_regcpy(void *dut, bool direction) {
    CPU_state *diff_context = (CPU_state *)dut;//CPU_state cpu{pc, gpr[32]}
    //cpu 特指nemu
    if(direction == DIFFTEST_TO_REF){//dut->ref
        for(int i=0; i< NR_GPR;i++){
            cpu.gpr[i] = diff_context->gpr[i];
        }
        cpu.pc = diff_context->pc;
    }else{
        for(int i=0;i< NR_GPR;i++){
           diff_context->gpr[i]=cpu.gpr[i];
        }
       diff_context->pc=cpu.pc;
    }
}

//ref执行n条指令
__EXPORT void difftest_exec(uint64_t n) {
    cpu_exec(n);
//  assert(0);
}

__EXPORT void difftest_raise_intr(word_t NO) {
  assert(0);
}

__EXPORT void difftest_init(int port) {
  void init_mem();
  init_mem();
  /* Perform ISA dependent initialization. */
  init_isa();
}
