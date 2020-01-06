#include "mouse.h"

static int mouse_hook_id = 2;
struct packet pp;
int size = 0;

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
      util_sys_inb(OUT_BUF, &pp.bytes[size]);
      if ((pp.bytes[0] & BIT(3)) == BIT(3)) {
        ++size;
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

int(reset_KBC_cmd_byte)() {
  uint8_t cmd_byte = minix_get_dflt_kbc_cmd_byte();
  if (is_IBF_set()) {
    return 1;
  }
  sys_outb(IN_BUF_KBC_CMD, 0x60);
  if (is_IBF_set()) {
    return 1;
  }
  sys_outb(IN_BUF_CMD_BYTE, cmd_byte); //write command byte

  return 0;
}

int(mouse_polling)() {
  uint8_t st;
  while (size < 3) {
    do {
      util_sys_inb(STAT_REG, &st);
      tickdelay(micros_to_ticks(DELAY_US));
    } while ((st & (AUX | OBF)) != (BIT(5) | BIT(0)));

    if (util_sys_inb(OUT_BUF, &pp.bytes[size])) {
      return 1;
    }
    if ((pp.bytes[0] & BIT(3)) == BIT(3)) {
      ++size;
    }
  }
  return 0;
}

struct mouse_ev *(mouse_detect_evt)(struct packet *pp) {
  static struct packet *old_pp;
  struct mouse_ev *mouse_event = malloc(sizeof *mouse_event);

  if (old_pp == NULL) {
    old_pp = malloc(sizeof *old_pp);
    if (pp->lb == 0 && pp->mb == 0 && pp->rb == 0) {
      mouse_event->type = MOUSE_MOV;
      mouse_event->delta_x = pp->delta_x;
      mouse_event->delta_y = pp->delta_y;
    }
    else if (pp->lb == 1 && pp->mb == 0 && pp->rb == 0) {
      mouse_event->type = LB_PRESSED;
    }
    else if (pp->lb == 0 && pp->mb == 0 && pp->rb == 1) {
      mouse_event->type = RB_PRESSED;
    }
    else {
      mouse_event->type = BUTTON_EV;
    }
  }
  else {
    if (old_pp->lb == 1 && old_pp->mb == 0 && old_pp->rb == 0) {
      if (pp->lb == 0 && pp->mb == 0 && pp->rb == 0) {
        mouse_event->type = LB_RELEASED;
      }
      else if (pp->lb == 1 && pp->mb == 0 && pp->rb == 0) {
        mouse_event->type = MOUSE_MOV;
        mouse_event->delta_x = pp->delta_x;
        mouse_event->delta_y = pp->delta_y;
      }
      else {
        mouse_event->type = BUTTON_EV;
      }
    }
    else if (old_pp->lb == 0 && old_pp->mb == 0 && old_pp->rb == 1) {
      if (pp->lb == 0 && pp->mb == 0 && pp->rb == 0) {
        mouse_event->type = RB_RELEASED;
      }
      else if (pp->lb == 0 && pp->mb == 0 && pp->rb == 1) {
        mouse_event->type = MOUSE_MOV;
        mouse_event->delta_x = pp->delta_x;
        mouse_event->delta_y = pp->delta_y;
      }
      else {
        mouse_event->type = BUTTON_EV;
      }
    }
    else {
      if (pp->lb == 0 && pp->mb == 0 && pp->rb == 0) {
        mouse_event->type = MOUSE_MOV;
        mouse_event->delta_x = pp->delta_x;
        mouse_event->delta_y = pp->delta_y;
      }
      else if (pp->lb == 1 && pp->mb == 0 && pp->rb == 0) {
        mouse_event->type = LB_PRESSED;
      }
      else if (pp->lb == 0 && pp->mb == 0 && pp->rb == 1) {
        mouse_event->type = RB_PRESSED;
      }
      else {
        mouse_event->type = BUTTON_EV;
      }
    }
  }

  *old_pp = *pp;

  return mouse_event;
}

enum states(update_gesture_state)(struct mouse_ev *mouse_event, enum states logicalAndState, uint8_t x_len, uint8_t tolerance) {
  static int delta_x, delta_y;
  switch (logicalAndState) {
    case INITIAL_STATE:
      if (mouse_event->type == LB_PRESSED) {
        delta_x = 0;
        delta_y = 0;
        return LB_PRESSED_STATE;
      }
      return INITIAL_STATE;
      break;
    case LB_PRESSED_STATE:
      if (mouse_event->type == LB_RELEASED) {
        if (delta_x < 0 || delta_y < 0) {
          return INITIAL_STATE;
        }
        if (((double) delta_y / (double) delta_x) <= 1.0 || delta_x < x_len) {
          return INITIAL_STATE;
        }
        delta_x = 0;
        delta_y = 0;
        return FIRST_LINE_DRAWN_STATE;
      }

      if (mouse_event->type == MOUSE_MOV) {
        if (logicalAndState == LB_PRESSED_STATE) {
          delta_x += mouse_event->delta_x;
          delta_y += mouse_event->delta_y;
        }
        if (mouse_event->delta_x < 0) {
          if (abs(mouse_event->delta_x) > tolerance) {
            return INITIAL_STATE;
          }
        }
        if (mouse_event->delta_y < 0) {
          if (abs(mouse_event->delta_y) > tolerance) {
            return INITIAL_STATE;
          }
        }
        return LB_PRESSED_STATE;
      }

      return INITIAL_STATE;
      break;
    case FIRST_LINE_DRAWN_STATE:
      if (mouse_event->type == MOUSE_MOV) {
        delta_x += mouse_event->delta_x;
        delta_y += mouse_event->delta_y;

        if (abs(delta_x) > tolerance || abs(delta_y) > tolerance) {
          return INITIAL_STATE;
        }
        else {
          return FIRST_LINE_DRAWN_STATE;
        }
      }

      if (mouse_event->type == RB_PRESSED) {
        delta_x = 0;
        delta_y = 0;
        return RB_PRESSED_STATE;
      }
      if (mouse_event->type == LB_PRESSED) {
        return LB_PRESSED_STATE;
      }
      return INITIAL_STATE;
      break;
    case RB_PRESSED_STATE:

      if (mouse_event->type == RB_RELEASED) {
        if (delta_x < 0 || delta_y > 0) {
          return INITIAL_STATE;
        }
        if (((double) delta_y / (double) delta_x) >= -1.0 || delta_x < x_len) {
          return INITIAL_STATE;
        }
        delta_x = 0;
        delta_y = 0;
        return FINAL_STATE;
      }

      if (mouse_event->type == MOUSE_MOV) {
        if (logicalAndState == RB_PRESSED_STATE) {
          delta_x += mouse_event->delta_x;
          delta_y += mouse_event->delta_y;
        }
        if (mouse_event->delta_x < 0) {
          if (abs(mouse_event->delta_x) > tolerance) {
            return INITIAL_STATE;
          }
        }
        if (mouse_event->delta_y > 0) {
          if (abs(mouse_event->delta_y) > tolerance) {
            return INITIAL_STATE;
          }
        }
        return RB_PRESSED_STATE;
      }

      return INITIAL_STATE;
      break;
    case FINAL_STATE:
      return FINAL_STATE;
      break;
  }
}
