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
#include <readline/readline.h>
#include <readline/history.h>
#include "sdb.h"
#include <memory/paddr.h>

static int is_batch_mode = false;

void init_regex();
/////////////// watchpoint ////////////////
void init_wp_pool();
void info_wp();
void delete_wp(int NUM);
void set_wp(char *args);
void diff_wp();

//bool make_token(char *e);
word_t expr(char *e, bool *success);


/* We use the `readline' library to provide more flexibility to read from stdin. */
static char* rl_gets() {
    static char *line_read = NULL;

  if (line_read) {
    free(line_read);
    line_read = NULL;
  }

  line_read = readline("(nemu) ");
  if (line_read && *line_read) {
//      printf("%s-%d\n",line_read, *line_read);
    add_history(line_read);
  }

  return line_read;
}

static int cmd_c(char *args) {
  cpu_exec(-1);
  return 0;
}

////////////////// added by halo for PA1
static int cmd_si(char *args ){
    char *arg = strtok(NULL, " ");
//    extern void execute(uint64_t n);
    if(arg == NULL){
//        execute(1);
        cpu_exec(1);
    }
    else{
        cpu_exec(atoi(arg));
//        execute(atoi(arg));
    }    
    return 0;
}

static int cmd_info(char *args){
    char *arg = strtok(NULL, " ");
    if(strcmp(arg , "r") == 0){
        isa_reg_display();
    }
    else if(strcmp(arg, "w") == 0){
        info_wp();
    }
    return 0;
}

static int cmd_x(char *args){
    char *arg = strtok(NULL, " ");
    int x_num = (atoi(arg));
    arg = strtok(NULL, "\0");//paddr    

//strtoul将参数str指向的字符串根据给定base转换为无符号长整数
    paddr_t paddr = (uint32_t)strtoul(arg,NULL,16);
//    printf("x_num=%d,addr=%u, addr = %s\n", x_num,paddr,arg);
    word_t read_val;
    int i;
    for(i=0;i<x_num;i++){
    //case 4: return *(uint32_t *)addr;
        read_val = paddr_read(paddr, 4);
        printf("%#.8x:     %#.8x\n",paddr, read_val);//%#. 以0x的格式输出十六进制数据
        paddr+=4;
    }
    return 0;
}


static int cmd_p(char *args){
//static int cmd_p(){////////for test
//    char *context;
//
//    char *arg = strtok_r(args, " ", &context);
//    printf("the arg is %s, context is %s\n",arg, context);
    
  bool *success = false;
//  word_t result;

//    cmdp_val[cmdp_num] = expr(args,success);
//    result = expr(args,success);
//    printf("result = %d\n",*success);
//    assert(*success ==true);
 word_t cmdp_num=0;
 word_t cmdp_val[32];
    cmdp_num++;
cmdp_val[cmdp_num] = expr(args,success);
/////////////////////////////////////// for test
//    uint32_t gen_result;
//    char args[65536];
////popen执行shell指令，参数1指令，参数2只能是r或w; fopen参数1文件路径，参数2打开模式
//    FILE *fp=popen("~/ysyx_workspace/ysyx-workbench/nemu/tools/gen-expr/tmp/.expr","r");
//    assert(fp != NULL);
//    int ret=fscanf(fp, "%d", &gen_result);
//    assert(ret!=0);
//   printf("gen_result = %d\n",gen_result);
//    pclose(fp);
//
//    ret = system("cp ~/ysyx_workspace/ysyx-workbench/nemu/tools/gen-expr/input ./");
//    assert(ret == 0);
////    fp=fopen("~/ysyx_workspace/ysyx-workbench/nemu/tools/gen-expr/input","r");
//    fp=fopen("input","r");
//    assert(fp!=NULL);
////    ret=fscanf(fp, "%s", args);
////  printf("the args is %s\n", args);
//    
//    if(fgets(args, 65536, fp) != NULL) {
//        printf("%s\n", args);
//    }
//
//    fclose(fp);
//
//    result = expr(args,success);
//
//    if(result != gen_result){
//    Log( "result=%d, but gen_result=%d",result,gen_result);
//    }else
//    {
//        Log("expr success!");
//    }
    printf("$%d = %d\n", cmdp_num, cmdp_val[cmdp_num]);
  
    return 0;
}

static int cmd_w(char *args){
    char *arg = strtok(NULL, " ");
//    int32_t res = expr(
//printf("cmd_w arg is %s\n",arg);
    set_wp(arg);
    return 0;
}

static int cmd_d(char *args){
    Assert(args != NULL, "No segment to delete");
    char *arg = strtok(NULL, " ");
    delete_wp(atoi(arg));
    return 0;
}

static int cmd_q(char *args) {
  return -1;
}

static int cmd_help(char *args);

static struct {
  const char *name;
  const char *description;
  int (*handler) (char *);
} cmd_table [] = {
  { "help", "Display information about all supported commands", cmd_help },
  { "c", "Continue the execution of the program", cmd_c },
  { "q", "Exit NEMU", cmd_q },
  { "si", "Single step", cmd_si },
  { "info", "Print the information of r: registers/w: watchpoints", cmd_info},
  { "x", "Scan the memory", cmd_x},
  { "p", "Expression evaluation", cmd_p},
  { "w", "Set a watchpoint", cmd_w},
  { "d", "Delete a watchpoint", cmd_d},

};

#define NR_CMD ARRLEN(cmd_table)

static int cmd_help(char *args) {
  /* extract the first argument */
  char *arg = strtok(NULL, " ");
  int i;

  if (arg == NULL) {//help后未指定命令,则打印所有命令
    /* no argument given */
    for (i = 0; i < NR_CMD; i ++) {
      printf("%s - %s\n", cmd_table[i].name, cmd_table[i].description);
    }
  }
  else {
    for (i = 0; i < NR_CMD; i ++) {
      if (strcmp(arg, cmd_table[i].name) == 0) {//help xxx,匹配到命令，打印该命令
        printf("%s - %s\n", cmd_table[i].name, cmd_table[i].description);
        return 0;
      }
    }
    printf("Unknown command '%s'\n", arg);
  }
  return 0;
}

void sdb_set_batch_mode() {
  is_batch_mode = true;
}

void sdb_mainloop() {
  if (is_batch_mode) {
    cmd_c(NULL);
    return;
  }

  for (char *str; (str = rl_gets()) != NULL; ) {
    char *str_end = str + strlen(str);

    /* extract the first token as the command */
    char *cmd = strtok(str, " ");
    if (cmd == NULL) { continue; }

    /* treat the remaining string as the arguments,
     * which may need further parsing
     */
    char *args = cmd + strlen(cmd) + 1;
    if (args >= str_end) {
      args = NULL;
    }

#ifdef CONFIG_DEVICE
    extern void sdl_clear_event_queue();
    sdl_clear_event_queue();
#endif

    int i;
    for (i = 0; i < NR_CMD; i ++) {
      if (strcmp(cmd, cmd_table[i].name) == 0) {
        if (cmd_table[i].handler(args) < 0) { return; }
        break;
      }
    }

    if (i == NR_CMD) { printf("Unknown command '%s'\n", cmd); }
  }
}

void init_sdb() {
  /* Compile the regular expressions. */
  init_regex();

  /* Initialize the watchpoint pool. */
  init_wp_pool();
}
