// IMPORTANT: you must include the following line in all your C files
#include <lcom/lcf.h>

#include <stdint.h>
#include <stdio.h>

// Any header files included below this line should have been created by you
#include "mouse.h"

extern struct packet pp;
extern int size;
extern int counter;

int main(int argc, char *argv[]) {
  // sets the language of LCF messages (can be either EN-US or PT-PT)
  lcf_set_language("EN-US");

  // enables to log function invocations that are being "wrapped" by LCF
  // [comment this out if you don't want/need/ it]
  lcf_trace_calls("/home/lcom/labs/lab4/trace.txt");

  // enables to save the output of printf function calls on a file
  // [comment this out if you don't want/need it]
  lcf_log_output("/home/lcom/labs/lab4/output.txt");

  // handles control over to LCF
  // [LCF handles command line arguments and invokes the right function]
  if (lcf_start(argc, argv))
    return 1;

  // LCF clean up tasks
  // [must be the last statement before return]
  lcf_cleanup();

  return 0;
}

int(mouse_test_packet)(uint32_t cnt) {

  int packet_counter = cnt;

  uint8_t bit_no;

  if (mouse_subscribe_int(&bit_no) != 0) {
    return 1;
  }

  if (mouse_disable_irq() != 0) {
    return 1;
  }

  //ENABLE DATA REPORTING
  if (mouse_command(ENABLE_DATA_RPT) != ACK) {
    return 1;
  }

  if (mouse_enable_irq() != 0) {
    return 1;
  }

  int r, ipc_status;
  message msg;

  uint32_t irq_set = BIT(bit_no);

  while (packet_counter > 0) {
    /* Get a request message. */
    if ((r = driver_receive(ANY, &msg, &ipc_status)) != 0) {
      printf("driver_receive failed with: %d", r);
      continue;
    }
    if (is_ipc_notify(ipc_status)) { /* received notification */
      switch (_ENDPOINT_P(msg.m_source)) {
        case HARDWARE: /* hardware interrupt notification */
          if (msg.m_notify.interrupts & irq_set) {
            mouse_ih();
            if (size == 3) {
              packet_parser(&pp);
              mouse_print_packet(&pp);
              size = 0;
              --packet_counter;
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

  if (mouse_disable_irq() != 0) {
    return 1;
  }

  //DISABLE DATA REPORTING
  mouse_command(DISABLE_DATA_RPT);

  if (mouse_enable_irq() != 0) {
    return 1;
  }

  if (mouse_unsubscribe_int() != 0) {
    return 1;
  }

  return 0;
}

int(mouse_test_remote)(uint16_t period, uint8_t cnt) {

  int packet_counter = cnt;

  while (packet_counter > 0) {

    //SEND DATA PACKET REQUEST
    mouse_command(READ_DATA);

    mouse_polling();

    packet_parser(&pp);
    mouse_print_packet(&pp);
    size = 0;
    --packet_counter;

    tickdelay(micros_to_ticks(period * 1000));
  }

  //SET STREAM MODE
  mouse_command(SET_STREAM_MODE);

  //DISABLE DATA REPORTING
  mouse_command(DISABLE_DATA_RPT);

  //Reset the KBC command byte to Minix's default value
  if (reset_KBC_cmd_byte()) {
    return 1;
  }

  return 0;
}

int(mouse_test_async)(uint8_t idle_time) {

  if (idle_time <= 0)
    return 1;

  uint8_t mouse_bit_no;

  if (mouse_subscribe_int(&mouse_bit_no) != 0) {
    return 1;
  }

  if (mouse_disable_irq() != 0) {
    return 1;
  }
  //ENABLE DATA REPORTING
  if (mouse_command(ENABLE_DATA_RPT) != ACK) {
    return 1;
  }
  if (mouse_enable_irq() != 0) {
    return 1;
  }

  uint8_t timer_bit_no, idle_seconds;
  idle_seconds = idle_time;

  if (timer_subscribe_int(&timer_bit_no) != 0) {
    return 1;
  }

  int r, ipc_status;
  message msg;

  uint32_t irq_timer0 = BIT(timer_bit_no);
  uint32_t irq_mouse = BIT(mouse_bit_no);

  while (idle_seconds > 0) {
    /* Get a request message. */
    if ((r = driver_receive(ANY, &msg, &ipc_status)) != 0) {
      printf("driver_receive failed with: %d", r);
      continue;
    }
    if (is_ipc_notify(ipc_status)) { /* received notification */
      switch (_ENDPOINT_P(msg.m_source)) {
        case HARDWARE: /* hardware interrupt notification */
          if (msg.m_notify.interrupts & irq_timer0) {
            timer_int_handler();
            if (counter % 60 == 0)
              --idle_seconds;
          }
          if (msg.m_notify.interrupts & irq_mouse) {
            counter = 0;
            idle_seconds = idle_time;
            mouse_ih();
            if (size == 3) {
              packet_parser(&pp);
              mouse_print_packet(&pp);
              size = 0;
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

  if (mouse_disable_irq()) {
    return 1;
  }
  //DISABLE DATA REPORTING
  mouse_command(DISABLE_DATA_RPT);

  if (mouse_enable_irq()) {
    return 1;
  }

  if (mouse_unsubscribe_int() != 0) {
    return 1;
  }

  return 0;
}

int(mouse_test_gesture)(uint8_t x_len, uint8_t tolerance) {

  uint8_t bit_no;

  if (mouse_subscribe_int(&bit_no) != 0) {
    return 1;
  }

  if (mouse_disable_irq() != 0) {
    return 1;
  }

  //ENABLE DATA REPORTING
  if (mouse_command(ENABLE_DATA_RPT) != ACK) {
    return 1;
  }

  if (mouse_enable_irq() != 0) {
    return 1;
  }

  int r, ipc_status;
  message msg;

  uint32_t irq_set = BIT(bit_no);

  struct mouse_ev *mouse_event;
  enum states logicalAndState = INITIAL_STATE;

  while (logicalAndState != FINAL_STATE) {
    /* Get a request message. */
    if ((r = driver_receive(ANY, &msg, &ipc_status)) != 0) {
      printf("driver_receive failed with: %d", r);
      continue;
    }
    if (is_ipc_notify(ipc_status)) { /* received notification */
      switch (_ENDPOINT_P(msg.m_source)) {
        case HARDWARE: /* hardware interrupt notification */
          if (msg.m_notify.interrupts & irq_set) {
            mouse_ih();
            if (size == 3) {
              packet_parser(&pp);
              mouse_print_packet(&pp);
              mouse_event = mouse_detect_evt(&pp);
              logicalAndState = update_gesture_state(mouse_event, logicalAndState, x_len, tolerance);
              size = 0;
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

  if (mouse_disable_irq()) {
    return 1;
  }

  //DISABLE DATA REPORTING
  mouse_command(DISABLE_DATA_RPT);

  if (mouse_enable_irq()) {
    return 1;
  }

  if (mouse_unsubscribe_int() != 0) {
    return 1;
  }

  return 0;
}
