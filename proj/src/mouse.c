#include "mouse.h"

#include "graphics.h"
#include "snake.h"

static int mouse_hook_id = 2;
struct packet pp;
int packet_size = 0;

struct Mouse *mouse;

void(create_mouse)() {
  mouse = (struct Mouse *) malloc(sizeof(struct Mouse));
  mouse->x = get_xRes() / 2;
  mouse->y = get_yRes() / 2;
}

//snake.c
extern struct Apple *apple;
extern uint16_t pixels_per_unit;

void(mouse_process_event)(struct mouse_ev *mouse_event) {
  switch (mouse_event->type) {
    case MOUSE_MOV:
      mouse->x += mouse_event->delta_x;
      mouse->y -= mouse_event->delta_y;

      if (mouse->x < 0) {
        mouse->x = 0;
      }
      else if (mouse->x >= get_xRes()) {
        mouse->x = get_xRes() - 1;
      }

      if (mouse->y < 0) {
        mouse->y = 0;
      }
      else if (mouse->y >= get_yRes()) {
        mouse->y = get_yRes() - 1;
      }
      break;
    case LB_PRESSED:
      if (apple == NULL) {
        struct Apple *newApple = (struct Apple *) malloc(sizeof(struct Apple));
        newApple->x = mouse->x / pixels_per_unit;
        newApple->y = mouse->y / pixels_per_unit;
        apple = newApple;
        if(snake_apple_collision()){
          apple = NULL;
        }
      }
      break;
    default:
      break;
  }
}

int(mouse_subscribe_int)(uint8_t *bit_no) {

  *bit_no = mouse_hook_id;

  if (sys_irqsetpolicy(MOUSE_IRQ, IRQ_REENABLE | IRQ_EXCLUSIVE, &mouse_hook_id) != 0) {
    return 1;
  }

  return 0;
}

int(mouse_disable_irq)() {
  if (sys_irqdisable(&mouse_hook_id) != 0) {
    return 1;
  }

  return 0;
}

int(mouse_enable_irq)() {
  if (sys_irqenable(&mouse_hook_id) != 0) {
    return 1;
  }

  return 0;
}

int(mouse_unsubscribe_int)() {
  if (sys_irqrmpolicy(&mouse_hook_id) != 0) {
    return 1;
  }

  return 0;
}

void(mouse_ih)() {
  uint8_t st;

  util_sys_inb(STAT_REG, &st);
  if ((st & (PARITY_ERROR | TIMEOUT_ERROR)) == 0) {
    if ((st & (AUX | OBF)) == (BIT(5) | BIT(0))) { //if output buffer is full and data is from mouse
      util_sys_inb(OUT_BUF, &pp.bytes[packet_size]);
      if ((pp.bytes[0] & BIT(3)) == BIT(3)) {
        ++packet_size;
      }
    }
  }
}

void(packet_parser)(struct packet *pp) {
  int delta_x_sign, delta_y_sign;

  pp->lb = pp->bytes[0] & BIT(0);
  pp->rb = (pp->bytes[0] & BIT(1)) >> 1;
  pp->mb = (pp->bytes[0] & BIT(2)) >> 2;
  delta_x_sign = (pp->bytes[0] & BIT(4)) >> 4;
  delta_y_sign = (pp->bytes[0] & BIT(5)) >> 5;
  pp->x_ov = (pp->bytes[0] & BIT(6)) >> 6;
  pp->y_ov = (pp->bytes[0] & BIT(7)) >> 7;

  if (delta_x_sign) {
    int16_t doublebyte = pp->bytes[1] | BIT(8);
    //left-shift para colocar o sign bit no bit 15, dividir por 128 (2^7) para recuparar o valor mantendo o sinal
    pp->delta_x = (int16_t)(doublebyte << 7) / 128;
  }
  else {
    pp->delta_x = pp->bytes[1];
  }

  if (delta_y_sign) {
    int16_t doublebyte = pp->bytes[2] | BIT(8);
    //left-shift para colocar o sign bit no bit 15, dividir por 128 (2^7) para recuparar o valor mantendo o sinal
    pp->delta_y = (int16_t)(doublebyte << 7) / 128;
  }
  else {
    pp->delta_y = pp->bytes[2];
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
    }
    else {
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

uint8_t(mouse_command)(uint8_t value) {
  uint8_t ack;

  if (is_IBF_set()) {
    return 1;
  }
  sys_outb(IN_BUF_KBC_CMD, WRITE_MOUSE_CMD);
  if (is_IBF_set()) {
    return 1;
  }
  sys_outb(IN_BUF_CMD_BYTE, value);
  if (is_OBF_set()) {
    return 1;
  }
  if (util_sys_inb(OUT_BUF, &ack)) { //read acknowledgment
    return 1;
  }

  return ack;
}
