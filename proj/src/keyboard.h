#ifndef KEYBOARD_H
#define KEYBOARD_H

#include "i8042.h"
#include <lcom/lcf.h>
#include <stdint.h>

/** @defgroup keyboard keyboard
 * @{
 * Functions for the keyboard.
 * 
 * \author João Carlos Carreira Martins
 * \author João Francisco de Pinho Brandão
 */

/** @brief Subscribes keyboard interrupts */
int(kbd_subscribe_int)(uint8_t *bit_no);
/** @brief Unsubscribes keyboard interrupts */
int(kbd_unsubscribe_int)();
/** @brief Keyboard interrupts handler */
void(kbc_ih)();


#endif
