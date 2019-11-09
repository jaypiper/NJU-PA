#include "common.h"

static _Context* do_event(_Event e, _Context* c) {
 // Log("reach");
  //printf("after reach");
  switch (e.event) {
    case 0: printf("EVENT NULL"); break;
    case 1: printf("EVENT ERROR"); break;
    case 2: printf("EVENT IRQ TIMER"); break;
    case 3: printf("EVENT IQR IODEV"); break;
    case 4: printf("EVENT PAGEFAULT"); break;
    case 5: printf("EVENT YIELD"); break;
    case 6: printf("EVENT SYSCALL"); break;
    default: panic("Unhandled event ID = %d", e.event);
  }
  //printf("the end");
  return NULL;
}

void init_irq(void) {
  Log("Initializing interrupt/exception handler...");
  _cte_init(do_event);
}
