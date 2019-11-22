#include "common.h"
_Context* do_syscall(_Context *c);

static _Context* do_event(_Event e, _Context* c) {
 // Log("reach");
  //printf("after reach");
  switch (e.event) {
    // case 0: printf("EVENT NULL\n"); break;
    // case 1: printf("EVENT ERROR\n"); break;
    // case 2: printf("EVENT IRQ TIMER\n"); break;
    // case 3: printf("EVENT IQR IODEV\n"); break;
    // case 4: printf("EVENT PAGEFAULT\n"); break;
    case _EVENT_YIELD: printf("EVENT YIELD\n"); break;
    case _EVENT_SYSCALL: do_syscall(c);break;
    default: panic("Unhandled event ID = %d", e.event);
  }
  //printf("the end");
  return NULL;
}

void init_irq(void) {
  Log("Initializing interrupt/exception handler...");
  _cte_init(do_event);
}
