#ifndef __WATCHPOINT_H__
#define __WATCHPOINT_H__

#include "common.h"


typedef struct watchpoint {
  int NO;
  struct watchpoint *next;
  char expr[128];   //应该够用了吧 QAQ
  /* TODO: Add more members if necessary */


} WP;

extern uint32_t wp_value[];
WP* new_wp();
void free_wp(int i);
bool check_watchpoint();
void all_watchpoint();

#endif
