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
#include <cpu/difftest.h>
#include "../local-include/reg.h"

bool isa_difftest_checkregs(CPU_state *ref_r, vaddr_t pc) {
//cpu.pc== s->dnpc in exec_once
    printf("cpu.pc=%x, pc=%x, ref_pc=%x\n",cpu.pc,pc,ref_r->pc);
  if(cpu.pc == ref_r->pc){
      for(int i=0;i<NR_GPR;){
          if(ref_r->gpr[i] == cpu.gpr[i]){ 
            i++;
          }
          else{
            return difftest_check_reg(reg_name(i), pc, ref_r->gpr[i], cpu.gpr[i]);
          }

      }
      return true;
  }else return false;
//  if(cpu.pc != ref_r->pc) return false;
//	for(int i = 0; i < NR_GPR; i++) {
//		if(cpu.gpr[i] != ref_r->gpr[i]) return false;
//	}
//  return true;
}

void isa_difftest_attach() {
}
