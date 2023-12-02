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

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <assert.h>
#include <string.h>

// this should be enough
static char buf[65536] = {};
static char code_buf[65536 + 128] = {}; // a little larger than `buf`
static char *code_format =
"#include <stdio.h>\n"
"int main() { \n"
"  unsigned result = %s; \n"
"  printf(\"%%u\", result); \n"
"  return 0; \n"
"}";

//产生小于n的随机数
static uint32_t choose(uint32_t n){
//srand函数初始化随机数生成器，产生rand需要的种子，种子相同，产生的随机数也会相同
//    srand(time(0));
    return rand() % n;
}


static void gen_rand_op(){
    char op[4] = {'+', '-', '*', '/'};
//    printf("!!!!!!!!!!!!!!!!!!!!!! %c\n",op[4]);
    char op_str[2] = {op[choose(4)], '\0'};
    strcat(buf,op_str);
// printf("buf after add op is %s\n",buf);

//    sprintf(buf, "%c", op[choose(4)]);
}

static void gen_num(){
    char num[5];
    //数字转为字符型，保存在参数1中
    snprintf(num,sizeof(num),"%d",choose(10));
    //printf("gen_num is %s\n",num);
    strcat(buf,num);
    //printf("buf after add num is %s\n",buf);
}


static void insert_null(){
    uint32_t buf_len;
    int null_op;//position of ' '
    int null_num;//num of ' '
    int i;
    int j;
    buf_len=strlen(buf);
    null_num=choose(5)+1;
//    printf("null_num is %d\n",null_num);

    if(buf_len > 3){
        null_op=choose(buf_len-1)+1;
        if(strchr("+-*/()",buf[null_op-1])){
//        printf("null op is %d\n",null_op);
        for(j=0;j<null_num;j++){
//            strcat(buf," ");
            buf_len=strlen(buf);
//    printf("buf len is %d\n",buf_len);
            for(i=buf_len;i>=null_op;i--){
//    printf("i is %d,buf[i] is %c\n",i,buf[i]);
                buf[i+1]=buf[i];
            }
            buf[null_op]=' ';
//    printf("!!!!!!!!!!!!!!!!%s\n",buf);
        }
    }
    }
}


static void gen_rand_expr() {
    assert(strlen(buf)<65536);
    switch(choose(3)) {
        case 0:  
                if(buf[strlen(buf)-1] != ')' || strlen(buf)==0)
                    gen_num();
                else{
                    gen_rand_expr();
                }
                break;
        case 1:  
                 //strchr在参数1中查找参数2,若有则返回第一次出现的位置信息，否则返回NULL
                 if(buf[0]!='\0' && strchr("+-*/", buf[strlen(buf)-1])){///////////////////////////TODO
                    strcat(buf,"(");
                    gen_rand_expr(); 
//                    if(buf[strlen(buf)-2] == '('){
                        gen_rand_op();
                        gen_num();
//                    }
                     strcat(buf,")");
                 }
                 else { 
                    gen_rand_expr();
                 }
                 break;

        default: 
                 gen_rand_expr(); 
                 gen_rand_op(); 
                 gen_rand_expr(); 
                 break;
    }
    if(strlen(buf)==1) gen_rand_expr();
  
//      printf("expr is %s\n",buf);
}

//static void division_by_zero_detect(){
//    int buf_len;
//    buf_len=strlen(buf);
//    int i;
//    for(i=0;i<buf_len;i++){
//        if(buf[i]=='/'){
//            if(buf[i+1]=='0') {};
//            else if(

int main(int argc, char *argv[]) {
  int seed = time(0);//获取时间值
  srand(seed);
  int loop =1;
  if (argc > 1) {
//从字符串读格式化的数据到内存
    sscanf(argv[1], "%d", &loop);
  }

  int i;
  for (i = 0; i < loop; i ++) {
    gen_rand_expr();
    assert(strlen(buf)<65530);
    insert_null();
// printf("final expr is %s\n",buf);
 

//buf内容以code_format格式给到code_buf，格式化方式把数据写到字符串
    sprintf(code_buf, code_format, buf);

//popen执行shell命令，参数1指令，参数2只能是r或w; fopen打开文件，参数1文件路径，参数2打开模式
    FILE *fp = fopen("./tmp/.code.c", "w");
    assert(fp != NULL);
//向文件写
    fputs(code_buf, fp);
    fclose(fp);

    int ret = system("gcc ./tmp/.code.c -o ./tmp/.expr");
    if (ret != 0) continue;

    fp = popen("./tmp/.expr", "r");
    assert(fp != NULL);

    uint32_t result;
//从文件中读格式化的数据到内存中
    ret = fscanf(fp, "%d", &result);
    pclose(fp);

//%u 无符号整数
//    printf("%u %s\n %s\n", result, buf, code_buf);
    printf("%s", buf);
  }
  return 0;
}
