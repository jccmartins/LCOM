#include "keyboard.h"

static int kbd_hook_id = 1;

uint8_t byte;

int(kbd_subscribe_int)(uint8_t *bit_no) {

  *bit_no = kbd_hook_id;

  if (sys_irqsetpolicy(KBD_IRQ, IRQ_REENABLE | IRQ_EXCLUSIVE, &kbd_hook_id) != 0) {
    return 1;
  }

  return 0;
}

int(kbd_unsubscribe_int)() {
  if (sys_irqrmpolicy(&kbd_hook_id) != 0) {
    return 1;
  }

  return 0;
}

void(kbc_ih)() {

  uint8_t st;

  util_sys_inb(STAT_REG, &st);
  if ((st & (PARITY_ERROR | TIMEOUT_ERROR)) == 0) {
    if (st & OBF) //if output buffer is full
      util_sys_inb(OUT_BUF, &byte);
  }
}
