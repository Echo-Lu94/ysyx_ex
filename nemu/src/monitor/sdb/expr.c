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

/* We use the POSIX regex functions to process regular expressions.
 * Type 'man regex' for more information about POSIX regex functions.
 */
#include <regex.h>
#include <memory/paddr.h>
//c语言中使用正则表达式一般分为三步
//  编译regcomp
//  匹配regexec
//  释放regfree
enum {
    TK_NOTYPE = 256, TK_EQ, TK_NUM, TK_NOTEQ, TK_AND, TK_HEX, TK_REG, TK_DEREF 
   

  /* TODO: Add more token types */
};

static struct rule {
  const char *regex;
  int token_type;
} rules[] = {

  /* TODO: Add more rules.
   * Pay attention to the precedence level of different rules.
   */

  {"0[xX][0-9a-fA-F]+", TK_HEX},
  {"\\$[a-zA-Z]*[0-9]*", TK_REG},
  {" +", TK_NOTYPE},    // spaces
  {"\\+", '+'},         // plus
  {"==", TK_EQ},        // equal
  {"\\-", '-'},         //minus
  {"\\*", '*'},         //multi
  {"\\/", '/'},         //division
  {"\\(", '('},
  {"\\)", ')'},
  {"[0-9]+", TK_NUM},  //num
  {"!=", TK_NOTEQ},
  {"&&", TK_AND},
//  {"\\*.+", TK_DEREF},
};

#define NR_REGEX ARRLEN(rules)

static regex_t re[NR_REGEX] = {};

/* Rules are used for many times.
 * Therefore we compile them only once before any usage.
 */
void init_regex() {
  int i;
  char error_msg[128];
  int ret;

  for (i = 0; i < NR_REGEX; i ++) {
//把指定的正则表达式格式编译成特定的数据格式，使匹配更有效
//第一个参数，结构体数据类型，存放编译后的正则表达式
//第二个参数，指向写好的正则表达式的指针
//第三个参数，REG_EXTEND 以功能更强大的扩展方式进行匹配
//            REG_ICASE 匹配字母时忽略大小写
//            REG_NOSUB 不用存储匹配后的结果，只返回是否成功匹配。此时忽略regexec中的nmatch和pmatch
//            REG_NEWLINE 识别换行符，使$从行尾开始匹配，^从行头开始匹配
    ret = regcomp(&re[i], rules[i].regex, REG_EXTENDED);
    if (ret != 0) {
//执行regcomp或regexec产生错误时，调用该函数返回一个错误信息字符串
//  参数1 错误代号
//  参数2 编译好的正则表达式，可以为NULL
//  参数3 指向存放错误信息的字符串的内存空间
//  参数4 buffer长度，限制打印信息长度
      regerror(ret, &re[i], error_msg, 128);
      panic("regex compilation failed: %s\n%s", error_msg, rules[i].regex);
    }
  }
}

typedef struct token {
  int type;
  char str[32];
} Token;

static Token tokens[32] __attribute__((used)) = {};
static int nr_token __attribute__((used))  = 0;

//recognize token
static bool make_token(char *e) {
  int position = 0;
  int i;
  regmatch_t pmatch;

  nr_token = 0;


  while (e[position] != '\0') {
    /* Try all rules one by one. */
    for (i = 0; i < NR_REGEX; i ++) {
//模式匹配
//  参数1 编译好的正则表达式
//  参数2 目标文本串
//  参数3 regmatch_t结构体数组长度
//  参数4 regmatch_t {rm_so, rm_eo}，rm_so存放文本串在目标串的开始位置，rm_eo存放结束位置
//  参数5 REG_NOTBOL 让^无作用
//        REG_NOTEOL 让$无作用
      if (regexec(&re[i], e + position, 1, &pmatch, 0) == 0 && pmatch.rm_so == 0) {
        char *substr_start = e + position;
        int substr_len = pmatch.rm_eo;

        // considering limitation of tokens.str[32]
        Assert(substr_len < 32, "substr_len should less than 32 !");
//        Log("substr is %s", substr_start);

//%s  :打印字符串，遇到0停止
//%*s :打印字符串，至少substr_len个字节，不够左侧补0，超过则按实际长度
//%.*s:最多substr_len个字节，不够按实际长度，超过只打印substr_len个
//%-*s:原本左侧对齐改为右侧对齐
        Log("match rules[%d] = \"%s\" at position %d with len %d: %.*s",
            i, rules[i].regex, position, substr_len, substr_len, substr_start);

        position += substr_len;

//assert,括号内为真，则继续；括号内为假，则failed
//test assert
//        assert(rules[i].token_type!='+');

        /* Now a new token is recognized with rules[i]. Add codes
         * to record the token in the array `tokens'. For certain types
         * of tokens, some extra actions should be performed.
         */
        switch (rules[i].token_type) {
            case TK_NOTYPE: continue;
            case TK_HEX:
            case TK_REG:
                        memset(tokens[nr_token].str, 0, sizeof(tokens[nr_token].str)); 
                        tokens[nr_token].type = rules[i].token_type;
                        strncpy(tokens[nr_token].str , substr_start,substr_len) ;
            default : tokens[nr_token].type =  rules[i].token_type;
                      strncpy(tokens[nr_token].str , substr_start,substr_len) ;
                      Log("tokens[%d].type = %d", nr_token,tokens[nr_token].type);
                      Log("tokens[%d].str = %s\n", nr_token,tokens[nr_token].str);
                      nr_token++;
                      // considering limitation of tokens[32]
                      Assert(nr_token<32,"nr_token should less than 32 !");
        }
        break;
      }
    }

    if (i == NR_REGEX) {
      printf("no match at position %d\n%s\n%*.s^\n", position, e, position, "");
      return false;
    }
  }

  return true; 
}

static bool check_parentheses(int p, int q){
    int i;
    int n=0;

    if(tokens[p].type != '(' || tokens[q].type != ')')
        return false;

    for(i=p; i<=q; i++){
        if(tokens[i].type == '(')
            n++;
        else if(tokens[i].type == ')')
            n--;
        if(n == 0 && i<q) return false;
    }
    Assert(n == 0, "Parentheses are not pair!");

    return true;

}

static int min(int a, int b){
    if(a<b) return a;
    else return b;
}

static int find_prio(int p,int q){
    int i;
    int prio = 3;
    int result = 0;    
    int p_num = 0;

    for(i=p; i<=q; i++){

        //continue只能用于循环语句，不可用于switch语句；break可用于循环和switch语句
        switch(tokens[i].type){
            case '(': p_num++; break;
            case ')': p_num--; break;
            case '+':
            case '-': 
//                    Log("prio is %d, op is %d,pnum is %d\n",prio,op,p_num);
                      if(p_num == 0) {//out of parentheses
                          if(prio == 1) result = i;
                          else {
                              prio = min(1, prio);
                              result = i;
                          }
                          break;
                      }
                      else break;//continue;// in the parentheses
                    
            case '*':
            case '/': if(p_num == 0) {//out of parentheses
                          if(prio == 1) break;//continue;
                          else if(prio == 2) result = i;
                          else{
                              prio = min(2, prio);
                              result = i;
                          }
                          break;
                      }
                      else break;//continue;// in the parentheses
            case TK_EQ:
            case TK_NOTEQ:
            case TK_AND  :
                      prio = 3; result = i; break;
//            case TK_AND: prio = 3; result = i; break;
            case TK_DEREF: prio = 4; result=i;break;
            default : break;//continue;
        }
    }
//    Log("find the prio is %d",result);
 return result;
}

static uint32_t eval(int p, int q){
    int op;
//    int op_type;
    uint32_t val1, val2;

    if (p > q) {
    /* Bad expression */
        Log("Bad expression");
        assert(0);
        return -1;
  }
  else if (p == q) {
    /* Single token.
     * For now this token should be a number.
     * Return the value of the number.
     */

      word_t ret;
      if(tokens[p].type == TK_HEX){
        sscanf(tokens[p].str, "%X", &ret);
        printf("TK_HEX is %#x\n",ret);
        return ret;
      }
      else if(tokens[p].type == TK_REG){
          bool success;
          
          if(strcmp(tokens[p].str, "$0") == 0)
            ret = isa_reg_str2val(tokens[p].str, &success);
          else{
              char *str=strtok(tokens[p].str, "$");
              //printf("str is %s\n",str);
            ret = isa_reg_str2val(str, &success);
          }
//         printf("ret is %d\n", ret);
         return ret;
      }
      else if(tokens[p].type==TK_NUM){
//      Assert(tokens[p].type == TK_NUM, "Single token, but it's not a number!"); 
        return atoi(tokens[p].str);
      }
  }
  else if (check_parentheses(p, q) == true) {
    /* The expression is surrounded by a matched pair of parentheses.
     * If that is the case, just throw away the parentheses.
     */
    return eval(p + 1, q - 1);
  }

  else {
      if(tokens[p].type == TK_DEREF){
        Assert(p < nr_token-1, "ERROR: There is no parameter after TK_DEREF!");
        if(check_parentheses(p+1,q)){
            word_t ret = eval(p+2,q-1);
            return paddr_read(ret,4);
        }
        else if(p+1 == q){
            return paddr_read(q,4);
        }
      }
        else{


        //    op = the position of 主运算符 in the token expression;
            op = find_prio(p,q);
            val1 = eval(p, op - 1);
            val2 = eval(op + 1, q);
        //    switch (op_type) {
            switch (tokens[op].type){
              case '+':         return val1 + val2; break;
              case '-':         return val1 - val2; break;
              case '*':         return val1 * val2; break;
              case '/':         Assert(val2 != 0, "Dividend is zero!");
                                return val1 / val2; break;
              case TK_EQ:       return val1 == val2; break;
              case TK_NOTEQ:    return val1 != val2; break;
              case TK_AND:      return val1 && val2; break;
        //      case TK_DEREF:    return *(tokens[op].str); break;
              default: assert(0);
            }
        }   
  }
return 0;
}


word_t expr(char *e, bool *success) {
  if (!make_token(e)) {
    *success = false;
    return 0;
  }
 
  int i;
  for(i=0;i<nr_token;i++){
      if(tokens[i].type=='*' && (i==0 || (tokens[i-1].type != TK_NUM && tokens[i-1].type != ')' && tokens[i-1].type != TK_HEX && tokens[i-1].type != TK_REG))){
          tokens[i].type = TK_DEREF;
      }
  }

//--
//if(tokens[i].type=='-' && (i==0 || (tokens[i-1].type != TK_NUM
//  }

  
  /* TODO: Insert codes to evaluate the expression. */
//  *success = true;
  return eval(0, nr_token-1);
//  return 0;
}
