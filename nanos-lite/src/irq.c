#include "common.h"

static _Context* do_event(_Event e, _Context* c) {
  //printf("reach!");
  switch (e.event) {
    case 0: Log("EVENT_NULL"); break;
    case 1: Log("EVENT_ERROR"); break;
    case 2: Log("EVENT_IRQ_TIMER"); break;
    case 3: Log("EVENT_IQR_IODEV"); break;
    case 4: Log("EVENT_PAGEFAULT"); break;
    case 5: Log("EVENT_YIELD"); break;
    case 6: Log("EVENT_SYSCALL"); break;
    default: panic("Unhandled event ID = %d", e.event);
  }
  printf("the end");
  return NULL;
}

void init_irq(void) {
  Log("Initializing interrupt/exception handler...");
  _cte_init(do_event);
}
