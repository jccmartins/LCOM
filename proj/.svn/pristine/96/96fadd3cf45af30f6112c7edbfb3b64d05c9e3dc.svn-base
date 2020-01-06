#include <lcom/lcf.h>

#include <stdint.h>

int(util_sys_inb)(int port, uint8_t *value) {
  uint32_t addr;
  int ret;
  ret = sys_inb(port, &addr);
  *value = addr;
  return ret;
}
