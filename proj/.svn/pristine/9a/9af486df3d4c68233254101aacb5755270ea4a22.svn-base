#include <lcom/lcf.h>
#include <lcom/timer.h>

#include <stdint.h>

#include "i8254.h"

int counter = 0;
static int hook_id = 0;

int(timer_set_frequency)(uint8_t timer, uint32_t freq) {

  if (freq <= 18) {
    printf("1193182 / %d = %d > 65535 (biggest number represented by uint16_t)\n",
           freq, 1193182 / freq);
    return 1;
  }

  uint8_t lsb, msb, status, control_word;
  uint16_t divisor = TIMER_FREQ / freq;

  util_get_LSB(divisor, &lsb);
  util_get_MSB(divisor, &msb);

  timer_get_conf(timer, &status);

  int timer_bits = 0x00;
  if (timer == 1)
    timer_bits = BIT(6);
  if (timer == 2)
    timer_bits = BIT(7);

  control_word = (status & 0x0f) | TIMER_LSB_MSB | timer_bits;
  sys_outb(TIMER_CTRL, control_word);
  sys_outb(TIMER_0 + timer, lsb);
  sys_outb(TIMER_0 + timer, msb);

  return 0;
}

int(timer_subscribe_int)(uint8_t *bit_no) {

  *bit_no = hook_id;

  if (sys_irqsetpolicy(TIMER0_IRQ, IRQ_REENABLE, &hook_id) != 0)
  {
    return 1;
  }
  
  return 0; 
}

int(timer_unsubscribe_int)() {

  if (sys_irqrmpolicy(&hook_id) != 0) {
    return 1;
  }

  return 0;
}

void(timer_int_handler)() { 
  counter++; 
  }

int(timer_get_conf)(uint8_t timer, uint8_t *st) {
  uint8_t rbc = TIMER_RB_CMD | TIMER_RB_COUNT_ | TIMER_RB_SEL(timer);
  sys_outb(TIMER_CTRL, rbc);
  util_sys_inb(TIMER_0 + timer, st);
  return 0;
}

int(timer_display_conf)(uint8_t timer, uint8_t st,
                        enum timer_status_field field) {
  union timer_status_field_val conf;

  switch (field) {
  case tsf_all:
    conf.byte = st;
    break;
  case tsf_initial:
    conf.in_mode = (st & (BIT(5) | BIT(4))) >> 4;
    break;
  case tsf_mode:
    conf.count_mode = (st & (BIT(3) | BIT(2) | BIT(1))) >> 1;
    break;
  case tsf_base:
    conf.bcd = st & BIT(0);
    break;
  }

  timer_print_config(timer, field, conf);
  return 0;
}
