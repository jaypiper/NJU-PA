#include "common.h"

static _Context* do_event(_Event e, _Context* c) {
  Log("reach!");
  printf("after reach");
  switch (e.event) {
    case 0: Log("EVENT NULL"); break;
    case 1: Log("EVENT ERROR"); break;
    case 2: Log("EVENT IRQ TIMER"); break;
    case 3: Log("EVENT IQR IODEV"); break;
    case 4: Log("EVENT PAGEFAULT"); break;
    case 5: Log("EVENT YIELD"); break;
    case 6: Log("EVENT SYSCALL"); break;
    default: panic("Unhandled event ID = %d", e.event);
  }
  printf("the end");
  return NULL;
}

void init_irq(void) {
  Log("Initializing interrupt/exception handler...");
  _cte_init(do_event);
}
