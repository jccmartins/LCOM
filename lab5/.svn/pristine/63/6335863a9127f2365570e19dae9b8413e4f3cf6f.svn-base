#include <lcom/lcf.h>

#include <stdint.h>

uint32_t sys_inb_counter;

int(util_get_LSB)(uint16_t val, uint8_t *lsb) {
  *lsb = val;
  return 0;
}

int(util_get_MSB)(uint16_t val, uint8_t *msb) {
  *msb = val >> 8;
  return 0;
}

int(util_sys_inb)(int port, uint8_t *value) {
  uint32_t addr;
  int ret;
  ret = sys_inb(port, &addr);
#ifdef LAB3
  ++sys_inb_counter;
#endif
  *value = addr;
  return ret;
}
