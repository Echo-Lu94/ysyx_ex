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

#include "sdb.h"

#define NR_WP 32

typedef struct watchpoint {
  int NO;
  struct watchpoint *next;

  /* Add more members if necessary */
  //x char* expr;
  char expr[512];//expression of wp
  int value;//last value of wp

} WP;

static WP wp_pool[NR_WP] = {};
//head: head of wp in use; free_: free wp
static WP *head=NULL, *free_ = NULL;

void init_wp_pool() {
  int i;
  for (i = 0; i < NR_WP; i++) {
    wp_pool[i].NO = i;
    wp_pool[i].next = (i == NR_WP - 1 ? NULL : &wp_pool[i + 1]);
  }

  head = NULL;
  free_ = wp_pool;
}

/* Implement the functionality of watchpoint */

WP* new_wp(){
    Assert(free_ != NULL, "No extra free listnode!");
    WP *New_wp = free_;
    free_ = free_->next;//or free_++;
        New_wp->next = head;//New_wp->head;
        head = New_wp;//head->old_wp
        return New_wp;
}

void free_wp(WP *wp){
    if(wp == head){   
        head = head->next;
    }
    else{
        WP *wp_det = head;
        if(wp_det->next != wp){
            wp_det = wp_det->next; 
            Assert(wp_det,"The wp is not in the head");
        }else{
            wp_det->next = wp->next;
        }
    }
    wp->next = free_;
    free_ = wp;
}

void info_wp(){
//    int i;
//    for(i=0;i<NR_WP;i++){
//        printf("%s   %s\n", "No", "Expression");
//        printf("%d    %s\n", wp_pool[i].NO, wp_pool[i].expr);
//    }
    WP *wp = head;
    if(!wp){
        printf("No watchpoints.\n");
        return;
    }
        printf("%-8s%-15s%-8s\n", "No", "Expression", "Value");
        while(wp){
//            int len = strlen(wp->expr);
//            printf("strlen is %d\n",len);
            printf("%-8d%-15s%-8d\n", wp->NO, wp->expr,wp->value);
            wp = wp->next;
    }
}

void delete_wp(int NUM){
    
    Assert(NUM < NR_WP && NUM >= 0, "No watchpoint number %d", NUM);
    WP* wp = &wp_pool[NUM];
    free_wp(wp);
}

void set_wp(char *args){
    WP* wp = new_wp();
//    printf("args in set_wp is %s\n",args);
//    char* arg = strtok(args,"$");
//    if(strcmp(args,"$0")==0)
        strcpy(wp->expr, args);
//    else

//    wp->expr = args;
//    head=wp;
//    printf("wp->expr in set_wp is %s\n",wp->expr);
//    printf("head->expr in set_wp is %s, NO is %d\n",head->expr,head->NO);
    bool success = true;
    word_t res = expr(wp->expr,&success);
//TODO:tmp assert
    assert(success);
    wp->value = res;
//    printf("wp->value is %d\n", wp->value);
    printf("Watchpoint %d: %s\n", wp->NO, wp-> expr);
}

void diff_wp(){
    WP* wp = head;
    while(wp){
        bool *success = false;
        word_t res = expr(wp->expr, success);
        if(wp->value != res){
            printf("Watchpoint %d: %s\n"
            "Old value = %d\n"
            "New value = %d\n",
            wp->NO, wp->expr, wp->value, res);
            wp->value = res;
            nemu_state.state = NEMU_STOP;
        }
        wp = wp->next;
    }
}


