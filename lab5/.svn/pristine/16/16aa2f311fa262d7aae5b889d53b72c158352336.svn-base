// IMPORTANT: you must include the following line in all your C files
#include <lcom/lcf.h>

#include <lcom/lab5.h>

#include <stdint.h>
#include <stdio.h>

#include "graphics.h"
#include "keyboard.h"

extern uint8_t byte;
extern int counter;

// Any header files included below this line should have been created by you

int main(int argc, char *argv[]) {
  // sets the language of LCF messages (can be either EN-US or PT-PT)
  lcf_set_language("EN-US");

  // enables to log function invocations that are being "wrapped" by LCF
  // [comment this out if you don't want/need it]
  lcf_trace_calls("/home/lcom/labs/lab5/trace.txt");

  // enables to save the output of printf function calls on a file
  // [comment this out if you don't want/need it]
  lcf_log_output("/home/lcom/labs/lab5/output.txt");

  // handles control over to LCF
  // [LCF handles command line arguments and invokes the right function]
  if (lcf_start(argc, argv))
    return 1;

  // LCF clean up tasks
  // [must be the last statement before return]
  lcf_cleanup();

  return 0;
}

int(video_test_init)(uint16_t mode, uint8_t delay) {

  if (delay <= 0) {
    return 1;
  }

  set_VBE_mode(mode);

  sleep(delay);

  if (vg_exit() != 0) {
    printf("video_test_init: vg_exit() failed \n");
    return -1;
  }

  return 0;
}

int(video_test_rectangle)(uint16_t mode, uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint32_t color) {
  uint8_t bit_no;

  set_VBE_mode(mode);

  vg_draw_rectangle(x, y, width, height, color);

  if (kbd_subscribe_int(&bit_no) != 0) {
    return 1;
  }

  int r, ipc_status;
  message msg;

  uint32_t irq_set = BIT(bit_no);

  uint8_t bytes[2], size = 0;
  bool done = false;
  bool make;

  while (!(byte == ESC_BREAK)) {
    /* Get a request message. */
    if ((r = driver_receive(ANY, &msg, &ipc_status)) != 0) {
      printf("driver_receive failed with: %d", r);
      continue;
    }
    if (is_ipc_notify(ipc_status)) { /* received notification */
      switch (_ENDPOINT_P(msg.m_source)) {
        case HARDWARE: /* hardware interrupt notification */
          if (msg.m_notify.interrupts & irq_set) {
            kbc_ih();
            bytes[size] = byte;
            ++size;
            if (bytes[size - 1] != 0xE0) {
              make = !(bytes[size - 1] >> 7);
              done = true;
            }

            if (done) {
              kbd_print_scancode(make, size, bytes);
              size = 0;
              done = false;
            }
          }
          break;
        default:
          break; /* no other notifications expected: do nothing */
      }
    }
    else {
      return 1;
    }
  }

  if (kbd_unsubscribe_int() != 0) {
    return 1;
  }

  if (vg_exit() != 0) {
    printf("video_test_init: vg_exit() failed \n");
    return -1;
  }

  return 0;
}

int(video_test_pattern)(uint16_t mode, uint8_t no_rectangles, uint32_t first, uint8_t step) {

  uint8_t bit_no;

  set_VBE_mode(mode);

  if (no_rectangles != 0) {
    vg_draw_pattern(no_rectangles, first, step);
  }

  if (kbd_subscribe_int(&bit_no) != 0) {
    return 1;
  }

  int r, ipc_status;
  message msg;

  uint32_t irq_set = BIT(bit_no);

  uint8_t bytes[2], size = 0;
  bool done = false;
  bool make;

  while (!(byte == ESC_BREAK)) {
    /* Get a request message. */
    if ((r = driver_receive(ANY, &msg, &ipc_status)) != 0) {
      printf("driver_receive failed with: %d", r);
      continue;
    }
    if (is_ipc_notify(ipc_status)) { /* received notification */
      switch (_ENDPOINT_P(msg.m_source)) {
        case HARDWARE: /* hardware interrupt notification */
          if (msg.m_notify.interrupts & irq_set) {
            kbc_ih();
            bytes[size] = byte;
            ++size;
            if (bytes[size - 1] != 0xE0) {
              make = !(bytes[size - 1] >> 7);
              done = true;
            }

            if (done) {
              kbd_print_scancode(make, size, bytes);
              size = 0;
              done = false;
            }
          }
          break;
        default:
          break; /* no other notifications expected: do nothing */
      }
    }
    else {
      return 1;
    }
  }

  if (kbd_unsubscribe_int() != 0) {
    return 1;
  }

  if (vg_exit() != 0) {
    printf("video_test_init: vg_exit() failed \n");
    return -1;
  }

  return 0;
}

int(video_test_xpm)(xpm_map_t xpm, uint16_t x, uint16_t y) {
  uint8_t bit_no;

  set_VBE_mode(0x105);

  display_xpm(true, xpm, x, y);

  if (kbd_subscribe_int(&bit_no) != 0) {
    return 1;
  }

  int r, ipc_status;
  message msg;

  uint32_t irq_set = BIT(bit_no);

  uint8_t bytes[2], size = 0;
  bool done = false;
  bool make;

  while (!(byte == ESC_BREAK)) {
    /* Get a request message. */
    if ((r = driver_receive(ANY, &msg, &ipc_status)) != 0) {
      printf("driver_receive failed with: %d", r);
      continue;
    }
    if (is_ipc_notify(ipc_status)) { /* received notification */
      switch (_ENDPOINT_P(msg.m_source)) {
        case HARDWARE: /* hardware interrupt notification */
          if (msg.m_notify.interrupts & irq_set) {
            kbc_ih();
            bytes[size] = byte;
            ++size;
            if (bytes[size - 1] != 0xE0) {
              make = !(bytes[size - 1] >> 7);
              done = true;
            }

            if (done) {
              kbd_print_scancode(make, size, bytes);
              size = 0;
              done = false;
            }
          }
          break;
        default:
          break; /* no other notifications expected: do nothing */
      }
    }
    else {
      return 1;
    }
  }

  if (kbd_unsubscribe_int() != 0) {
    return 1;
  }

  if (vg_exit() != 0) {
    printf("video_test_init: vg_exit() failed \n");
    return -1;
  }

  return 0;
}

int(video_test_move)(xpm_map_t xpm, uint16_t xi, uint16_t yi, uint16_t xf, uint16_t yf, int16_t speed, uint8_t fr_rate) {

  set_VBE_mode(0x105);

  display_xpm(true, xpm, xi, yi);

  uint8_t bytes[2], size = 0;
  bool done = false;
  bool make;

  uint8_t timer_bit_no, kbd_bit_no;

  uint8_t interruptions_per_frame;

  if (speed > 0) {
    interruptions_per_frame = 60 / fr_rate;
  }
  else if (speed < 0) {
    interruptions_per_frame = abs(speed) * 60 / fr_rate;
    speed = 1;
  }

  bool horizontal = !(yf - yi);

  uint16_t x = xi;
  uint16_t y = yi;

  if (timer_subscribe_int(&timer_bit_no) != 0)
    return 1;

  if (kbd_subscribe_int(&kbd_bit_no) != 0)
    return 1;

  int r, ipc_status;
  message msg;

  uint32_t irq_timer0 = BIT(timer_bit_no);
  uint32_t irq_kbd = BIT(kbd_bit_no);

  bool end = false;

  while (byte != ESC_BREAK) {
    /* Get a request message. */
    if ((r = driver_receive(ANY, &msg, &ipc_status)) != 0) {
      printf("driver_receive failed with: %d", r);
      continue;
    }
    if (is_ipc_notify(ipc_status)) { /* received notification */
      switch (_ENDPOINT_P(msg.m_source)) {
        case HARDWARE: /* hardware interrupt notification */
          if (msg.m_notify.interrupts & irq_timer0) {
            if (!end) {
              timer_int_handler();
              if (counter % interruptions_per_frame == 0) {
                if (horizontal) {
                  if ((x + speed) > xf) {
                    x = xf;
                    end = true;
                  }
                  else {
                    x += speed;
                  }
                }
                else {
                  if ((y + speed) > yf) {
                    y = yf;
                    end = true;
                  }
                  else {
                    y += speed;
                  }
                }
                double_buffer_xpm(xpm, x, y);
              }
            }
          }
          if (msg.m_notify.interrupts & irq_kbd) {
            kbc_ih();
            bytes[size] = byte;
            ++size;
            if (bytes[size - 1] != 0xE0) {
              make = !(bytes[size - 1] >> 7);
              done = true;
            }

            if (done) {
              kbd_print_scancode(make, size, bytes);
              size = 0;
              done = false;
            }
          }
          break;
        default:
          break; /* no other notifications expected: do nothing */
      }
    }
    else {
      return 1;
    }
  }

  if (timer_unsubscribe_int() != 0) {
    return 1;
  }

  if (kbd_unsubscribe_int() != 0) {
    return 1;
  }

  if (vg_exit() != 0) {
    printf("video_test_init: vg_exit() failed \n");
    return -1;
  }

  return 0;
}

int(video_test_controller)() {
  return 0;
}
