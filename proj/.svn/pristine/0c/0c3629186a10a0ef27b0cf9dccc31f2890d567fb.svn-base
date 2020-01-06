#ifndef I8042_H_
#define I8042_H_

/** @defgroup i8042 i8042
 * @{
 * Constants for programming the i8042 keyboard and mouse controller driver.
 * 
 * \author João Carlos Carreira Martins
 * \author João Francisco de Pinho Brandão
 */

#define KBD_IRQ 1 /**< @brief Keyboard IRQ line */
#define MOUSE_IRQ 12 /**< @brief mouse IRQ line */

#define STAT_REG 0x64 /**< @brief Status Register Address */
#define OUT_BUF 0x60 /**< @brief Output Buffer Address */
#define IN_BUF_KBC_CMD 0x64 /**< @brief Input Buffer Address to write commands to the KBC */
#define IN_BUF_CMD_BYTE 0x60 /**< @brief Input Buffer Address to write arguments of KBC commands */

#define ESC_BREAK 0x81 /**< @brief ESC makecode */

#define DELAY_US 20000 /**< @brief Keyboard maximum delay to respond to a command */

#define PARITY_ERROR BIT(7) /**< @brief Parity Error - Invalid Data */
#define TIMEOUT_ERROR BIT(6) /**< @brief Timeout Error - Invalid Data*/
#define OBF BIT(0) /**< @brief Output Buffer full - data available for reading */
#define IBF BIT(1) /**< @brief Input Buffer full - don't write commands or arguments */
#define AUX BIT(5) /**< @brief Mouse Data */

#define WRITE_MOUSE_CMD 0xD4 /**< @brief Write byte to mouse */

#define ENABLE_DATA_RPT 0xF4 /**< @brief Enable Data Reporting - In stream mode only */
#define DISABLE_DATA_RPT 0xF5 /**< @brief Disable Data Reporting - In stream mode, should be sent before any other command */
#define SET_STREAM_MODE 0xEA /**< @brief Set Stream Mode - send data on events */
#define SET_REMOTE_MODE 0xF0 /**< @brief Set Remote Mode - send data on request only */
#define READ_DATA 0xEB /**< @brief Read Data - send data packet request */

#define ACK 0xFA /**< @brief Acknowledgment byte if everything OK */
#define NACK 0xFE /**< @brief Acknowledgment byte if invalid byte */
#define ERROR 0xFC /**< @brief Acknowledgment byte if second consecutive invalid byte */


#define UP_ARROW_MAKE 0x48 /**< @brief Up Arrow makecode */
#define DOWN_ARROW_MAKE 0x50 /**< @brief Down Arrow makecode */
#define LEFT_ARROW_MAKE 0x4b /**< @brief Left Arrow makecode */
#define RIGHT_ARROW_MAKE 0x4d /**< @brief Right Arrow makecode */

#endif
