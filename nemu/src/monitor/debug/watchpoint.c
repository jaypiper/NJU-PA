#include "monitor/watchpoint.h"
#include "monitor/expr.h"

#define NR_WP 32

static WP wp_pool[NR_WP] = {};
static WP *head = NULL, *free_ = NULL;
uint32_t wp_value[NR_WP] = {};
int wp_num = 0;

void init_wp_pool() {
  int i;
  for (i = 0; i < NR_WP; i ++) {
    wp_pool[i].NO = i;
    wp_pool[i].next = &wp_pool[i + 1];
  }
  wp_pool[NR_WP - 1].next = NULL;

  head = NULL;
  free_ = wp_pool;
}

/* TODO: Implement the functionality of watchpoint */
WP* new_wp(){
  if(free_ == NULL) assert(0);
  WP* ret_wp = free_;
  free_ = free_ -> next;
  ret_wp -> next = head;
  head = ret_wp;
  ret_wp -> expr[0] = '\0';
  wp_num ++;

  return ret_wp;
}

void free_wp(int i){
  wp_pool[i].next = free_; 
  free_ = &wp_pool[i];
  wp_num --;
  for(WP* p = head; p != NULL; p = p -> next){
    if(p -> next == &wp_pool[i]) p-> next = (p -> next)->next;
  }
  return;
}

bool check_watchpoint(){
  
  bool changed = false;
  for(WP* p = head; p != NULL; p = p -> next){
    uint32_t new_val = expr_val(p -> expr);
    if(new_val != wp_value[p->NO]) {
      printf("checkpoint %d: \n old value: %u  ,  new value: %u\n",p -> NO,wp_value[p->NO],new_val);
      changed = true;
    }
  }
  return changed;
}
/*输出所有监视点信息*/
void all_watchpoint(){
  for(WP* p = head; p != NULL; p = p -> next){
    printf("checkpoint %d: expr: %s , now value: %u\n",p->NO,p->expr,expr_val(p->expr));
  }
  return;
}