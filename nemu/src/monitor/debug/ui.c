#include "monitor/monitor.h"
#include "monitor/expr.h"
#include "monitor/watchpoint.h"
#include "nemu.h"

#include <stdlib.h>
#include <readline/readline.h>
#include <readline/history.h>

void cpu_exec(uint64_t);
void isa_reg_display(void);
uint32_t expr(char*,bool*);
/* We use the `readline' library to provide more flexibility to read from stdin. */
static char* rl_gets() {
  static char *line_read = NULL;

  if (line_read) {
    free(line_read);
    line_read = NULL;
  }

  line_read = readline("(nemu) ");

  if (line_read && *line_read) {
    add_history(line_read);
  }

  return line_read;
}

static int cmd_c(char *args) {
  cpu_exec(-1);
  return 0;
}

static int cmd_q(char *args) {
  return -1;
}

static int cmd_help(char *args);

static int cmd_si(char *args){
  if(args==NULL) {cpu_exec(1);}
  else {cpu_exec(atoi(args));}
  return 0;
}

static int cmd_info(char *args){
  if(args[0]=='r') {isa_reg_display(); }
  else if(args[0]=='w'){all_watchpoint(); }
  else {printf("No such command, maybe you want to using info r"); }
  return 0;
}

static int cmd_x(char *args){
  int a=0,i;
 
  for(i=0;args[i]!=' '&&i<strlen(args);i++){
    a*=10;
    a+=args[i]-48;
  }
  i=i+3;
  unsigned int ad=0;
  for( ;i<strlen(args);i++){
     ad*=16;
     if(args[i]<='9') {ad+=args[i]-'0';}
     else if(args[i]<='Z') {ad+=args[i]-'A';}
     else {ad+=args[i]-'a';}
  }
  for(int j=0;j<a;j++){
    if(j%4==0) printf("\n%x:  ",ad);
    printf("%08x  ",paddr_read(ad,4));
    ad=ad+4;
  }
  printf("\n");
  return 0;
}

static int cmd_p(char* args){
  bool judge = true;
  
  printf("%u\n",expr(args,&judge));
  return 0;
}
/*  打开文件  */
static int cmd_p_file(char* args){
  FILE *fp = fopen(args, "r");
  uint32_t num1; char str2[512];
  
  while(fscanf(fp,"%u",&num1) != EOF){
    char* return_val = fgets(str2,512,fp);
    if(return_val) ;
    if(num1 == expr_val(str2)) ;//printf("true\n");
    else printf("false: str1: %u,val_compute:%u str2: %s\n",num1,expr_val(str2),str2);
  }
  return 0;
}

static int cmd_check(char* args){
  bool success_set_check = false;
  uint32_t set_value = expr(args,&success_set_check);
  if(!success_set_check) { printf("set checkpoint filed: wrong expression!\n"); return 0;}
  WP* set_wp = new_wp();
  for(int i = 0; i < strlen(args); i++) set_wp->expr[i] = args[i];
  set_wp->expr[strlen(args)] = '\0';
  wp_value[set_wp->NO] = set_value;
  printf("set checkpoint %d: %s\n",set_wp->NO,args);
  return 0;
}

static int cmd_delete_check(char* i){
  int wp_id = 0;
  for(int j = 0; j < strlen(i); j++){
    wp_id *= 10;
    wp_id += i[j] - '0';
  }
  free_wp(wp_id);
  return 0;
}

static struct {
  char *name;
  char *description;
  int (*handler) (char *);
} cmd_table [] = {
  { "help", "Display informations about all supported commands", cmd_help },
  { "c", "Continue the execution of the program", cmd_c },
  { "q", "Exit NEMU", cmd_q },
  { "si", "Executes n instructions,default n=0", cmd_si}, 
  { "info", "print the condition of the program, 'r' for regester and 'w' for watching", cmd_info},
  { "x", "scan the memory, two extra arguments are needed, one for the expected byte number of the address, and one for expression which indicator the begging of the address", cmd_x},
  { "p", "expression evalueation", cmd_p},
  { "p_file", "expression evaluation in file", cmd_p_file},
  { "check", "set checkpoint", cmd_check},
  { "d", "delete the checkpoint i", cmd_delete_check},

  /* TODO: Add more commands */

};

#define NR_CMD (sizeof(cmd_table) / sizeof(cmd_table[0]))

static int cmd_help(char *args) {
  /* extract the first argument */
  char *arg = strtok(NULL, " ");
  int i;

  if (arg == NULL) {
    /* no argument given */
    for (i = 0; i < NR_CMD; i ++) {
      printf("%s - %s\n", cmd_table[i].name, cmd_table[i].description);
    }
  }
  else {
    for (i = 0; i < NR_CMD; i ++) {
      if (strcmp(arg, cmd_table[i].name) == 0) {
        printf("%s - %s\n", cmd_table[i].name, cmd_table[i].description);
        return 0;
      }
    }
    printf("Unknown command '%s'\n", arg);
  }
  return 0;
}

void ui_mainloop(int is_batch_mode) {
  if (is_batch_mode) {
    cmd_c(NULL);
    return;
  }

  for (char *str; (str = rl_gets()) != NULL; ) {
    /*读取到一条指令*/
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

#ifdef HAS_IOE
    extern void sdl_clear_event_queue(void);
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
