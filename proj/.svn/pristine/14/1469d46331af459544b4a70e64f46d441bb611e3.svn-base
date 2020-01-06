#ifndef GRAPHICS_H
#define GRAPHICS_H

#include "i8042.h"
#include <lcom/lcf.h>
#include <stdint.h>

#include "rtc.h"

/** @defgroup graphics graphics
 * @{
 * Functions for graphics.
 * 
 * \author João Carlos Carreira Martins
 * \author João Francisco de Pinho Brandão
 */

/** @brief Load all xpm files used for this project */
void(load_all_xpm)();

/** @brief Set a VBE mode
 * @param mode - Desired VBE mode to set
 */
int(set_VBE_mode)(uint16_t mode);
/** @brief Draw pixel to buffer
 * @param vramDirectly - If true pixel is drawn directly to the video memory, if false it is drawn to an auxiliar buffer
 * @param color - Pixel color
 */
int(vg_draw_pixel)(bool vramDirectly, int x, int y, uint32_t color);
/** @brief Draw line to buffer
 * @param x - Inital x coordinate
 * @param y - Initial y coordinate
 * @param len - Line length
 * @param color - Line color
 */
int(vg_draw_hline)(uint16_t x, uint16_t y, uint16_t len, uint32_t color);
/** @brief Draw rectangle to buffer
 * @param x - Upper left x coordinate
 * @param y - Upper left y coordinate
 * @param width - Rectangle width
 * @param height - Rectangle height
 * @param color - Rectangle color
 */
int(vg_draw_rectangle)(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint32_t color);
/** @brief Draw xpm to buffer
 * @param vramDirectly - If true draws xpm directly to video memory, if false it is drawn to an auxiliar buffer
 * @param img - Xpm image to be drawn
 * @param x - Upper left x coordinate
 * @param y - Upper left y coordinate
 */
int(display_xpm)(bool vramDirectly, xpm_image_t img, uint16_t x, uint16_t y);

// -----------------------------------------------SNAKE------------------------------------------

/** @brief Get X resolution of current VBE mode */
uint16_t(get_xRes)();
/** @brief Get Y resolution of current VBE mode */
uint16_t(get_yRes)();

/** @brief Draws map of the game */
void(draw_map)();
/** @brief Draws snake in the map */
void(draw_snake)();
/** @brief Draws mouse pointer */
void(draw_mouse)();
/** @brief Draws apple on the screen if it exists */
void(draw_apple)();
/** @brief Draws score on the board */
void(draw_score)();
/** @brief Draws a number with two digits (if less than 10 draws a 0 first) on the screen */
void(draw_two_digit_number)(uint32_t number, uint16_t *x, uint16_t *y);
/** @brief Draws the time on the screen */
void(draw_time)(struct Time *rtc_time);
/** @brief Draws the game scene with double buffer */
void(draw_scene)();

#endif

