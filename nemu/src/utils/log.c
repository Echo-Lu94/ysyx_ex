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

#include <common.h>
#include <utils.h>
#include <stdlib.h>
#include <string.h>
#include <elf.h>

extern uint64_t g_nr_guest_inst;
FILE *log_fp = NULL;

void init_log(const char *log_file) {
  log_fp = stdout;
  if (log_file != NULL) {
    FILE *fp = fopen(log_file, "w");
    Assert(fp, "Can not open '%s'", log_file);
    log_fp = fp;
  }
  Log("Log is written to %s", log_file ? log_file : "stdout");
}

bool log_enable() {
  return MUXDEF(CONFIG_TRACE, (g_nr_guest_inst >= CONFIG_TRACE_START) &&
         (g_nr_guest_inst <= CONFIG_TRACE_END), false);
}

//------------- iringbuf ------------------
IRBuffer* IRBuffer_create(){
    IRBuffer* irbuf=malloc(sizeof(IRBuffer));
    irbuf->wr_ptr = 0;
    irbuf->rd_ptr = 0;
    irbuf->buffer = malloc(IRBUFFER_SIZE * sizeof(char *));
    for(int i=0; i< IRBUFFER_SIZE; i++){
        irbuf->buffer[i] = malloc(ITRACE_LEN * sizeof(char));
    }
    return irbuf;
}

void IRBuffer_free(IRBuffer* irbuf){
        for(int i=0; i<IRBUFFER_SIZE; i++){
            free(irbuf->buffer[i]);
        }
        free(irbuf->buffer);
        free(irbuf);
        return;
}

void IRBuffer_wr(IRBuffer* irbuf, char* itrace){
    memcpy(irbuf->buffer[irbuf->wr_ptr],itrace, strlen(itrace)+1);
//    irbuf->buffer[irbuf->wr_ptr] = itrace;
//    printf("wr_ptr:%d, buf: %s\n",irbuf->wr_ptr,irbuf->buffer[irbuf->wr_ptr]);
    irbuf->wr_ptr = (irbuf->wr_ptr+1) % IRBUFFER_SIZE;
    return;

}

void IRBuffer_display(IRBuffer* irbuf){
    while(irbuf->rd_ptr !=irbuf->wr_ptr - 1){
            printf("    %s\n",irbuf->buffer[irbuf->rd_ptr]);
            irbuf->rd_ptr = (irbuf->rd_ptr+1) % IRBUFFER_SIZE;
    }
    if(irbuf->rd_ptr==irbuf->wr_ptr-1){
        printf("--->%s\n",irbuf->buffer[irbuf->rd_ptr]);
    } 
}

// ------------------------ ftrace ------------------------
Elf32_Sym *elf_sym = NULL;//symbol table
Elf32_Shdr *elf_shdr = NULL;//section headers
char* strtab = NULL;//.strtab
int sym_num=0;
int func_state = 0;
static uint8_t space_num = 0;
static uint8_t ret_space_num = 0;

//
//  elf_header:
//      {e_ident
//      e_shoff---------------------|
//      e_shentsize//sec size       |
//      e_shnum                     |
//      e_shstrndx}--|              |
//                   |              |
//  elf_shdr:<-------|--------------|
//      ...          |
//      .shstrtab <----|
//      .symtab
//      .strtab
//      {sh_name
//       sh_type
//       sh_offset
//       sh_size
//       sh_link//.strtab的索引值
//       }
//   
//  .strtab & .symtab
//      elf_sym
//      {st_name
//       st_value//addr
//       st_size
//       st_info=STT_FUNC}
//
//
void init_elf(const char* elf_file){
    Elf32_Ehdr elf_header;
    FILE *fp;
    fp = fopen(elf_file,"r");
    Assert(fp, "Open elf_file Error!");
    int readf = fread(&elf_header, sizeof(Elf32_Ehdr), 1, fp);
    Assert(readf,"Read elf_header Failed");
    int is_elf = (strncmp((char*)elf_header.e_ident, ELFMAG,SELFMAG));
    Assert(!is_elf,"Not a elf file");

    elf_shdr = malloc(sizeof(Elf32_Shdr)*elf_header.e_shnum);
    fseek(fp, elf_header.e_shoff, SEEK_SET);//section header
    readf = fread(elf_shdr, sizeof(Elf32_Shdr)*elf_header.e_shnum,1,fp);
    Assert(readf, "Read section headers failed!");
    rewind(fp);

    fseek(fp, elf_shdr[elf_header.e_shstrndx].sh_offset, SEEK_SET);//.shstrtab
    char shstrtab[elf_shdr[elf_header.e_shstrndx].sh_size];
    readf = fread(shstrtab, elf_shdr[elf_header.e_shstrndx].sh_size,1,fp);
    assert(readf);
    rewind(fp);
    
    for(int i=0; i<elf_header.e_shnum; i++){
        if(!strcmp(shstrtab+elf_shdr[i].sh_name, ".strtab")){
            strtab = malloc(sizeof(char )*elf_shdr[i].sh_size);
            fseek(fp,elf_shdr[i].sh_offset,SEEK_SET);//.strtab
            readf=fread(strtab,elf_shdr[i].sh_size,1,fp);
            Assert(readf, "Read .strtab failed!");
            rewind(fp);
        }

        if(elf_shdr[i].sh_type == SHT_SYMTAB){
            sym_num=elf_shdr[i].sh_size / sizeof(Elf32_Sym);
            elf_sym=malloc(sizeof(char)*elf_shdr[i].sh_size);
            fseek(fp,elf_shdr[i].sh_offset,SEEK_SET);//.symtab
            readf=fread(elf_sym,elf_shdr[i].sh_size,1,fp);
            Assert(readf, "Read .symtab failed!");
            rewind(fp);
        }
    }
}

void func_detect(vaddr_t dnpc, vaddr_t pc){
    char* func;
    for(int i = 0;i<sym_num;i++){
        if(dnpc>=elf_sym[i].st_value && dnpc<elf_sym[i].st_value + elf_sym[i].st_size){
            func = strtab + elf_sym[i].st_name;//offset in strtab
            Assert(func, "Failed to detect the fuction");
        }
    }
    if(func_state != 0){
        log_write("0x%x:", pc);//cpu.pc
        if(func_state == CALL){
            space_num = space_num >= 0 ? space_num : 0;
            int tmp = space_num;
            log_write("space_num:%d",space_num);
            while(tmp > 0){log_write(" "); tmp--;}
            space_num++;
            ret_space_num = space_num -1;
            log_write("call [%s@0x%x]\n", func, dnpc);//s->dnpc
        }
        if(func_state == RET){
            ret_space_num = ret_space_num >= 0 ? ret_space_num : 0;
            int ret_tmp = ret_space_num;
            log_write("  ret_num:%d",ret_tmp);
            while(ret_tmp){log_write(" "); ret_tmp--;}
            ret_space_num--;
            space_num = ret_space_num+1;
            log_write("ret [%s]\n", func);
        }
        func_state=0;
    }
}


