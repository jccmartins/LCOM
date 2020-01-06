#ifndef I8042_H_
#define I8042_H_

#define KBD_IRQ 1
#define MOUSE_IRQ 12

#define STAT_REG 0x64
#define OUT_BUF 0x60
#define IN_BUF_KBC_CMD 0x64
#define IN_BUF_CMD_BYTE 0x60

#define ESC_BREAK 0x81

#define DELAY_US 20000

#define PARITY_ERROR BIT(7)
#define TIMEOUT_ERROR BIT(6)
#define OBF BIT(0)
#define KBC_CMD_BYTE_OBF BIT(0)
#define IBF BIT(1)
#define AUX BIT(5)

#define WRITE_MOUSE_CMD 0xD4

#define ENABLE_DATA_RPT 0xF4
#define DISABLE_DATA_RPT 0xF5
#define SET_STREAM_MODE 0xEA
#define SET_REMOTE_MODE 0xF0
#define READ_DATA 0xEB

#define ACK 0xFA
#define NACK 0xFE
#define ERROR 0xFC

#endif
