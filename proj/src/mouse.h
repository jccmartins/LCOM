#ifndef MOUSE_H
#define MOUSE_H

#include "i8042.h"
#include <lcom/lcf.h>
#include <stdint.h>

/** @defgroup mouse mouse
 * @{
 * Functions for the mouse and its struct.
 * 
 * \author João Carlos Carreira Martins
 * \author João Francisco de Pinho Brandão
 */

/** @brief Stores mouse x and y position */
struct Mouse{
  int x, y;
};

/** @brief Create mouse struct with x and y position in the middle of the screen */
void(create_mouse)();
/** @brief Processes event from mouse */
void(mouse_process_event)(struct mouse_ev *mouse_event);
/** @brief Subscribes mouse interrupts */
int(mouse_subscribe_int)(uint8_t *bit_no);
/** @brief Disable mouse IRQ line */
int(mouse_disable_irq)();
/** @brief Enables mouse IRQ line */
int(mouse_enable_irq)();
/** @brief Unsubscribes mouse interrupts */
int(mouse_unsubscribe_int)();
/** @brief Mouse interrupts handler */
void(mouse_ih)();
/** @brief Parses mouse packets */
void(packet_parser)(struct packet *pp);
/** @brief Checks if IBF is set
 * \n Checks a maximum of 5 times and returns 0 if IBF is not set, otherwise returns 1 if after 5 tries IBF is always set
 */
int(is_IBF_set)();
/** @brief Checks if OBF is set
 * \n Checks a maximum of 5 times and returns 0 if OBF is set, otherwise returns 1 if after 5 tries OBF is never set
 */
int(is_OBF_set)();
/** @brief Writes a mouse command to the mouse */
uint8_t(mouse_command)(uint8_t value);

#endif
