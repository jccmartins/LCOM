#include <lcom/lcf.h>
#include <lcom/timer.h>

#include <stdint.h>

#include "i8254.h"

int counter = 0;
static int hook_id = 0;

int(timer_subscribe_int)(uint8_t *bit_no)
{

  *bit_no = hook_id;

  if (sys_irqsetpolicy(TIMER0_IRQ, IRQ_REENABLE, &hook_id) != 0)
  {
    return 1;
  }

  return 0;
}

int(timer_unsubscribe_int)()
{

  if (sys_irqrmpolicy(&hook_id) != 0)
  {
    return 1;
  }

  return 0;
}

void(timer_int_handler)()
{
  counter++;
}
