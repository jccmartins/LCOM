#pragma once

#include "i8042.h"
#include <lcom/lcf.h>
#include <stdint.h>

int(return_VBE_controller_info)();

int(return_VBE_mode_info)();

int(set_VBE_mode)(uint16_t mode);

int(vg_draw_pixel)(int h, int x, int y, uint32_t color);

int(vg_draw_hline)(uint16_t x, uint16_t y, uint16_t len, uint32_t color);

int(vg_draw_rect)(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint32_t color);

int(vg_draw_pattern)(uint8_t no_rectangles, uint32_t first, uint8_t step);

int(vg_fill_address)(bool vramDirectly, uint16_t x, uint16_t y, uint8_t color);

int(display_xpm)(bool vramDirectly, xpm_map_t xpm, uint16_t x, uint16_t y);

void(aux_buffer_clear)();

void(double_buffer_xpm)(xpm_map_t xpm, uint16_t x, uint16_t y);

