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

#include "local-include/reg.h"
#include <cpu/cpu.h>
#include <cpu/ifetch.h>
#include <cpu/decode.h>

#define R(i) gpr(i)
#define Mr vaddr_read
#define Mw vaddr_write
#define LEN MUXDEF(CONFIG_RV64, 64, 32)
#define SLEN MUXDEF(CONFIG_RV64, 6, 5)
enum {
  TYPE_I, TYPE_U, TYPE_S,
  TYPE_J, TYPE_R, TYPE_B,
  TYPE_N, // none
};

#define src1R() do { *src1 = R(rs1); } while (0)
#define src2R() do { *src2 = R(rs2); } while (0)
//SEXT符号扩展
#define immI() do { *imm = SEXT(BITS(i, 31, 20), 12); } while(0)
#define immU() do { *imm = SEXT(BITS(i, 31, 12), 20) << 12; } while(0)
#define immS() do { *imm = (SEXT(BITS(i, 31, 25), 7) << 5) | BITS(i, 11, 7); } while(0)
//#define immJ() do { *imm = (SEXT(BITS(i, 31, 31), 1) << 20) | (SEXT(BITS(i, 19, 12), 8) << 12) | (SEXT(BITS(i, 20, 20), 1) << 11) | (SEXT(BITS(i, 30, 21), 10) << 1);} while(0)
#define immJ() do { *imm = (SEXT(BITS(i, 31, 31), 1) << 20) | BITS(i, 19, 12) << 12 | BITS(i, 20, 20) << 11 | BITS(i, 30, 21) << 1;} while(0)
#define immB() do { *imm = (SEXT(BITS(i, 31, 31), 1) << 12) | BITS(i, 7,   7) << 11 | BITS(i, 30, 25) << 5  | BITS(i, 11,  8) << 1;} while(0)
 


static void decode_operand(Decode *s, int *rd, sword_t *src1, sword_t *src2, sword_t *imm, int type) {
  uint32_t i = s->isa.inst.val;
  //位抽取
  int rs1 = BITS(i, 19, 15);
  int rs2 = BITS(i, 24, 20);
  *rd     = BITS(i, 11, 7);
//  int bits1=BITS(i,30,21);
//  int bits2=BITS(i,30,21)<<1;
  switch (type) {
    case TYPE_I: src1R();          immI(); 
                 Log("Type_I: rs1: %d, src1: %#x, imm: %#x",rs1,*src1,*imm); 
                 break;//rd
    case TYPE_U:                   immU(); 
                 Log("Type_U: rd: %d, imm: 0x%08x",*rd, *imm); 
                 break;//rd
    case TYPE_S: src1R(); src2R(); immS(); 
                 Log("Type_S: rs1: %d, src1: 0x%08x, rs2: %d, src2: 0x%08x, imm: 0x%08x",rs1,*src1,rs2, *src2,*imm);
                 break;
    case TYPE_J:                   immJ();
                 Log("Type_J: rd: %d, imm: 0x%08x",*rd, *imm); 
                 break;//rd
    case TYPE_B: src1R(); src2R(); immB();
                 Log("Type_B: rs1: %d, src1: 0x%08x, rs2: %d, src2: 0x%08x, imm: 0x%08x",rs1,*src1,rs2, *src2,*imm); 
                 break;
    case TYPE_R: src1R(); src2R();         
                 Log("Type_R: rs1: %d, src1: 0x%08x, rs2: %d, src2: 0x%08x,rd:%d",rs1,*src1,rs2, *src2, *rd);
                 break;//rd
    case TYPE_N:if(i == 0x00100073 )       break;//ebreak
                    else printf("404, Not Found!\n");
    default: printf("404, Not Found!\n");    break;
  }
}

static int decode_exec(Decode *s) {
  int rd = 0;
#ifdef CONFIG_FTRACE
  int rs1 = BITS(s->isa.inst.val, 19, 15);
#endif
  sword_t src1 = 0, src2 = 0, imm = 0;
//  word_t t;
//  vaddr_t pc_tmp = s->pc;

  s->dnpc = s->snpc;


#define INSTPAT_INST(s) ((s)->isa.inst.val)
#define INSTPAT_MATCH(s, name, type, ... /* execute body */ ) { \
  decode_operand(s, &rd, &src1, &src2, &imm, concat(TYPE_, type)); \
  __VA_ARGS__ ; \
}

  INSTPAT_START();
  //INSTPAT(模式字符串，指令名称，指令类型，指令执行操作);
  INSTPAT("??????? ????? ????? ??? ????? 00101 11", auipc  , U, R(rd) = s->pc + imm);
  INSTPAT("??????? ????? ????? 100 ????? 00000 11", lbu    , I, R(rd) = Mr(src1 + imm, 1));
  INSTPAT("??????? ????? ????? 000 ????? 01000 11", sb     , S, Mw(src1 + imm, 1, src2));
  INSTPAT("0000000 00001 00000 000 00000 11100 11", ebreak , N, NEMUTRAP(s->pc, R(10))); // R(10) is $a0
                                                                
  ////////////////////////pa2_2.2 by halo/////////////////////
  //      31    25    20    15  12     7     2  0
  //dummy
  INSTPAT("??????? ????? ????? ??? ????? 01101 11", lui    , U, R(rd) = imm);//li
  INSTPAT("??????? ????? ????? 000 ????? 00100 11", addi   , I, R(rd) = src1 + imm);//mv 
  INSTPAT("??????? ????? ????? 000 ????? 11001 11", jalr   , I, R(rd) = s->pc + 4; s->dnpc = (src1 + imm) & (0xfffffffe); 
                                                                IFDEF(CONFIG_FTRACE, func_state = (rs1 == 1) ? RET : CALL));//ret
  INSTPAT("??????? ????? ????? ??? ????? 11011 11", jal    , J, R(rd) = s->pc + 4; s->dnpc =s->pc + imm;
                                                                IFDEF(CONFIG_FTRACE, func_state = CALL));//j
  INSTPAT("??????? ????? ????? 010 ????? 01000 11", sw     , S, Mw(src1 + imm, 4, src2));
  //add 
  INSTPAT("??????? ????? ????? 010 ????? 00000 11", lw     , I, R(rd) = SEXT(Mr(src1 + imm, 4),32));//
  INSTPAT("0000000 ????? ????? 000 ????? 01100 11", add    , R, R(rd) = src1 + src2);
  INSTPAT("0100000 ????? ????? 000 ????? 01100 11", sub    , R, R(rd) = src1 - src2);//neg
  INSTPAT("??????? ????? ????? 011 ????? 00100 11", sltiu  , I, R(rd) = ((word_t)src1 < (word_t)imm));//seqz
  INSTPAT("??????? ????? ????? 001 ????? 11000 11", bne    , B, if(src1 != src2) s->dnpc =s->pc + imm);//bnez
//add-longlong
  INSTPAT("0000000 ????? ????? 011 ????? 01100 11", sltu   , R, R(rd) = (word_t)src1 < (word_t)src2);//snez
  INSTPAT("0000000 ????? ????? 100 ????? 01100 11", xor    , R, R(rd) = src1 ^ src2);
  INSTPAT("0000000 ????? ????? 110 ????? 01100 11", or     , R, R(rd) = src1 | src2);
//bit
  INSTPAT("??????? ????? ????? 000 ????? 11000 11", beq    , B, if(src1 == src2) s->dnpc = s->pc +imm);//beqz 
  INSTPAT("010000? ????? ????? 101 ????? 00100 11", srai   , I, R(rd) = src1 >> BITS(imm, 5, 0) );//TODO: sext//默认有符号数右移补1
  INSTPAT("??????? ????? ????? 111 ????? 00100 11", andi   , I, R(rd) = src1 & imm);//zext.b(zero extend byte)
  INSTPAT("0000000 ????? ????? 001 ????? 01100 11", sll    , R, R(rd) = src1 << src2);
  INSTPAT("0000000 ????? ????? 111 ????? 01100 11", and    , R, R(rd) = src1 & src2);
  INSTPAT("??????? ????? ????? 100 ????? 00100 11", xori   , I, R(rd) = src1 ^ imm);//not
  INSTPAT("??????? ????? ????? 001 ????? 01000 11", sh     , S, Mw(src1 + imm, 2, src2));
//bubble-sort
  INSTPAT("??????? ????? ????? 101 ????? 11000 11", bge    , B, if(src1 >= src2) s->dnpc =s->pc + imm);//blez
//crc32
  INSTPAT("0000000 ????? ????? 101 ????? 00100 11", srli   , I, R(rd) = (word_t)src1 >> BITS(imm, 5, 0));
  INSTPAT("??????? ????? ????? 111 ????? 11000 11", bgeu   , B, if((word_t)src1 >= (word_t)src2) s->dnpc =s->pc + imm);
  INSTPAT("0000000 ????? ????? 001 ????? 00100 11", slli   , I, R(rd) = src1 << BITS(imm, 5, 0));
//div
  INSTPAT("0000001 ????? ????? 000 ????? 01100 11", mul    , R, R(rd) = src1 * src2);
  INSTPAT("0000001 ????? ????? 100 ????? 01100 11", div    , R, R(rd) = src1 / src2);
//goldbach  
  INSTPAT("0000001 ????? ????? 110 ????? 01100 11", rem    , R, R(rd) = src1 % src2);
//if-else
  INSTPAT("??????? ????? ????? 100 ????? 11000 11", blt    , B, if(src1 < src2) s->dnpc =s->pc + imm);
  INSTPAT("0000000 ????? ????? 010 ????? 01100 11", slt    , R, R(rd) = (src1 < src2));
//load-store
  INSTPAT("??????? ????? ????? 001 ????? 00000 11", lh     , I, R(rd) = SEXT(Mr(src1 + imm, 2), 16));//sext
  INSTPAT("??????? ????? ????? 101 ????? 00000 11", lhu    , I, R(rd) = (word_t)Mr(src1 + imm, 2));
//mersenne
  INSTPAT("0000001 ????? ????? 001 ????? 01100 11", mulh   , R, R(rd) = (int64_t)src1 * (int64_t)src2 >> LEN );//最高位为符号位
  INSTPAT("0000001 ????? ????? 111 ????? 01100 11", remu   , R, R(rd) = (word_t)src1 % (word_t)src2);
  INSTPAT("0000001 ????? ????? 101 ????? 01100 11", divu   , R, R(rd) = (word_t)src1 / (word_t)src2);
//shift
  INSTPAT("0100000 ????? ????? 101 ????? 01100 11", sra    , R, R(rd) = src1 >> BITS(src2, SLEN-1, 0));
  INSTPAT("0000000 ????? ????? 101 ????? 01100 11", srl    , R, R(rd) = (word_t)src1 >> BITS((word_t)src2, SLEN-1, 0));
//switch
  INSTPAT("??????? ????? ????? 110 ????? 11000 11", bltu   , B, if((word_t)src1 < (word_t)src2) s->dnpc =s->pc + imm);
//hello-str
  INSTPAT("??????? ????? ????? 110 ????? 00100 11", ori    , I, R(rd) = src1 | imm);

//模式匹配失败,非法指令 
  INSTPAT("??????? ????? ????? ??? ????? ????? ??", inv    , N, INV(s->pc));
  INSTPAT_END();
//  printf("!!!!!!!!%#x\n",R(0));

  //to deside which value of s->dnpc should be
//  if(pc_tmp != s->pc) s->dnpc = s->pc;

  R(0) = 0; // reset $zero to 0

  return 0;
}

int isa_exec_once(Decode *s) {
//取指
  s->isa.inst.val = inst_fetch(&s->snpc, 4);
//译码、执行
  return decode_exec(s);
}
