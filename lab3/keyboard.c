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

int(is_IBF_set)() {
  int n_tries = 5;
  uint8_t st;

  while (n_tries > 0) {
    if (util_sys_inb(STAT_REG, &st)) {
      return 1;
    }
    if ((st & IBF) != 0) { //Check if IBF is set
      tickdelay(micros_to_ticks(DELAY_US));
    }else{
      return 0;
    }
    --n_tries;
  }

  return 1;
}

int(is_OBF_set)() {
  int n_tries = 5;
  uint8_t st;

  while (n_tries > 0) {
    if (util_sys_inb(STAT_REG, &st)) {
      return 1;
    }
    if ((st & OBF) == 0) { //Check if OBF is set
      tickdelay(micros_to_ticks(DELAY_US));
    }
    else {
      return 0;
    }
    --n_tries;
  }

  return 1;
}



