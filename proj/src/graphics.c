#include "graphics.h"

#include "mouse.h"
#include "snake.h"

#include "../res/0.xpm"
#include "../res/1.xpm"
#include "../res/2.xpm"
#include "../res/3.xpm"
#include "../res/4.xpm"
#include "../res/5.xpm"
#include "../res/6.xpm"
#include "../res/7.xpm"
#include "../res/8.xpm"
#include "../res/9.xpm"
#include "../res/apple.xpm"
#include "../res/mouse-pointer.xpm"
#include "../res/trophy.xpm"
#include "../res/two_dots.xpm"

//snake.c
extern uint16_t pixels_per_unit;
extern struct Snake *snake;
extern struct Apple *apple;

//mouse.c
extern struct Mouse *mouse;

xpm_image_t zero_img;
xpm_image_t one_img;
xpm_image_t two_img;
xpm_image_t three_img;
xpm_image_t four_img;
xpm_image_t five_img;
xpm_image_t six_img;
xpm_image_t seven_img;
xpm_image_t eight_img;
xpm_image_t nine_img;
xpm_image_t apple_img;
xpm_image_t mouse_pointer_img;
xpm_image_t trophy_img;
xpm_image_t two_dots_img;

void load_all_xpm()
{
  enum xpm_image_type type = XPM_8_8_8;

  xpm_load(zero_xpm, type, &zero_img);
  xpm_load(one_xpm, type, &one_img);
  xpm_load(two_xpm, type, &two_img);
  xpm_load(three_xpm, type, &three_img);
  xpm_load(four_xpm, type, &four_img);
  xpm_load(five_xpm, type, &five_img);
  xpm_load(six_xpm, type, &six_img);
  xpm_load(seven_xpm, type, &seven_img);
  xpm_load(eight_xpm, type, &eight_img);
  xpm_load(nine_xpm, type, &nine_img);
  xpm_load(apple_xpm, type, &apple_img);
  xpm_load(mouse_pointer_xpm, type, &mouse_pointer_img);
  xpm_load(trophy_xpm, type, &trophy_img);
  xpm_load(two_dots_xpm, type, &two_dots_img);
}

static void *video_mem, *aux_video_mem;
static uint16_t xRes, yRes;
static uint8_t bitsPerPixel;
static uint8_t memoryModel;
static uint8_t redMaskSize, greenMaskSize, blueMaskSize;
static uint8_t redFieldPosition, greenFieldPosition, blueFieldPosition;

int(set_VBE_mode)(uint16_t mode)
{

  //Allocate lowest 1MB of memory
  mmap_t mmap;
  lm_alloc(BIT(20), &mmap);

  //Get Mode Info
  vbe_mode_info_t vmi_p;
  vbe_get_mode_info(mode, &vmi_p);

  lm_free(&mmap);

  xRes = vmi_p.XResolution;
  yRes = vmi_p.YResolution;
  bitsPerPixel = vmi_p.BitsPerPixel;
  memoryModel = vmi_p.MemoryModel;
  redMaskSize = vmi_p.RedMaskSize;
  greenMaskSize = vmi_p.GreenMaskSize;
  blueMaskSize = vmi_p.BlueMaskSize;
  redFieldPosition = vmi_p.RedFieldPosition;
  greenFieldPosition = vmi_p.GreenFieldPosition;
  blueFieldPosition = vmi_p.BlueFieldPosition;

  // Allow memory mapping
  int r;
  struct minix_mem_range mr; /*physical memory range*/
  unsigned int vram_base;    /*VRAM’s physical addresss*/
  unsigned int vram_size;    /*VRAM’s size, but you can usethe frame-buffer size, instead*/

  vram_base = vmi_p.PhysBasePtr;
  vram_size = vmi_p.XResolution * vmi_p.YResolution * ((vmi_p.BitsPerPixel + 7) / 8);

  aux_video_mem = malloc(vram_size);

  mr.mr_base = (phys_bytes)vram_base;
  mr.mr_limit = mr.mr_base + vram_size;

  if (OK != (r = sys_privctl(SELF, SYS_PRIV_ADD_MEM, &mr)))
  {
    panic("sys_privctl (ADD_MEM) failed: %d\n", r);
  }

  /* Map memory */
  video_mem = vm_map_phys(SELF, (void *)mr.mr_base, vram_size);
  if (video_mem == MAP_FAILED)
    panic("couldn't map video memory");

  // Set VBE Mode
  struct reg86 reg86;

  memset(&reg86, 0, sizeof(reg86));

  reg86.ax = 0x4F02;
  reg86.bx = 1 << 14 | mode; //Bit 14 = 1 - Use linear/flat frame buffer model
  reg86.intno = 0x10;
  if (sys_int86(&reg86) != OK)
  {
    printf("set_VBE_mode: sys_int86() failed \n");
    return -1;
  }

  return reg86.ax;
}

int(vg_draw_pixel)(bool vramDirectly, int x, int y, uint32_t color)
{

  if (x < xRes && y < yRes)
  {
    uint8_t *address = NULL;

    if (vramDirectly == true)
    {
      address = (uint8_t *)video_mem + (y * xRes + x) * ((bitsPerPixel + 7) / 8);
    }
    else
    {
      address = (uint8_t *)aux_video_mem + (y * xRes + x) * ((bitsPerPixel + 7) / 8);
    }

    for (size_t i = 0; i < ((bitsPerPixel + 7) / 8); i++)
    {
      *address = color >> (i * 8);
      ++address;
    }
  }

  return 0;
}

int(vg_draw_hline)(uint16_t x, uint16_t y, uint16_t len, uint32_t color)
{

  for (size_t xPos = 0; xPos < len; xPos++)
  {
    vg_draw_pixel(false, xPos + x, y, color);
  }

  return 0;
}

int(vg_draw_rectangle)(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint32_t color)
{

  for (size_t i = 0; i < height; i++)
  {
    vg_draw_hline(x, y + i, width, color);
  }

  return 0;
}

int(display_xpm)(bool vramDirectly, xpm_image_t img, uint16_t x, uint16_t y)
{

  uint8_t *sprite = img.bytes;

  if (sprite == NULL)
  {
    printf("ERROR: sprite NULL\n  ");
    return 1;
  }

  uint32_t color;

  for (size_t j = 0; j < img.height; j++)
  {
    for (size_t i = 0; i < img.width; i++)
    {
      if (memoryModel == 0x04)
      { //0x04 - packed pixel/indexed color
        color = *sprite;
      }
      else if (memoryModel == 0x06)
      { //0x06 - direct color
        uint8_t b = *sprite % (1 << blueMaskSize);
        ++sprite;
        uint8_t g = *sprite % (1 << greenMaskSize);
        ++sprite;
        uint8_t r = *sprite % (1 << redMaskSize);
        color = (r << redFieldPosition) + (g << greenFieldPosition) + (b << blueFieldPosition);
      }
      if (color != 0x00b140)
      {
        vg_draw_pixel(vramDirectly, x + i, y + j, color);
      }
      ++sprite;
    }
  }

  return 0;
}

// ---------------------------------------------------SNAKE-------------------------------------------

uint16_t(get_xRes)()
{
  return xRes;
}

uint16_t(get_yRes)()
{
  return yRes;
}

void(draw_map)()
{
  vg_draw_rectangle(0, 0, xRes, yRes, 0x47d147);
}

void(draw_snake)()
{

  struct Node *node = snake->head;

  vg_draw_rectangle(node->x * pixels_per_unit, node->y * pixels_per_unit, pixels_per_unit, pixels_per_unit, 0xadd8e9);
  node = node->next;

  while (node != NULL)
  {
    vg_draw_rectangle(node->x * pixels_per_unit, node->y * pixels_per_unit, pixels_per_unit, pixels_per_unit, 0x0000ff);
    node = node->next;
  }
}

void(draw_mouse)()
{
  display_xpm(false, mouse_pointer_img, mouse->x, mouse->y);
}

void(draw_apple)()
{
  if (apple != NULL)
  {
    display_xpm(false, apple_img, apple->x * pixels_per_unit, apple->y * pixels_per_unit);
  }
}

void(draw_score)()
{
  int score = snake->score;
  int digit;

  int xPos = get_xRes() - 20;
  int y = 5;

  int n_digits = 0;

  // trophy.xpm width is 22px, 
  // 3*20 is the space needed for 3 digits for the value of the score, 
  // 10 is a a little space to separate this xpm from the actual score
  int trophy_xPos = get_xRes() - 22 - 3 * 20 - 10;

  display_xpm(false, trophy_img, trophy_xPos, y);

  if (score == 0)
  {
    display_xpm(false, zero_img, xPos, y);
  }
  else
  {
    while (score > 0)
    {

      ++n_digits;
      xPos = get_xRes() - 20 * n_digits;

      digit = score % 10;

      switch (digit)
      {
      case 0:
        display_xpm(false, zero_img, xPos, y);
        break;
      case 1:
        display_xpm(false, one_img, xPos, y);
        break;
      case 2:
        display_xpm(false, two_img, xPos, y);
        break;
      case 3:
        display_xpm(false, three_img, xPos, y);
        break;
      case 4:
        display_xpm(false, four_img, xPos, y);
        break;
      case 5:
        display_xpm(false, five_img, xPos, y);
        break;
      case 6:
        display_xpm(false, six_img, xPos, y);
        break;
      case 7:
        display_xpm(false, seven_img, xPos, y);
        break;
      case 8:
        display_xpm(false, eight_img, xPos, y);
        break;
      case 9:
        display_xpm(false, nine_img, xPos, y);
        break;
      }

      score = score / 10;
    }
  }
}

void(draw_two_digit_number)(uint32_t number, uint16_t *x, uint16_t *y)
{
  uint32_t remainder;
  for (int i = 1; i <= 2; i++)
  {
    remainder = number % 10;
    number = number / 10;
    switch (remainder)
    {
    case 0:
      *x -= zero_img.width;
      display_xpm(false, zero_img, *x, *y);
      break;
    case 1:
      *x -= one_img.width;
      display_xpm(false, one_img, *x, *y);
      break;
    case 2:
      *x -= two_img.width;
      display_xpm(false, two_img, *x, *y);
      break;
    case 3:
      *x -= three_img.width;
      display_xpm(false, three_img, *x, *y);
      break;
    case 4:
      *x -= four_img.width;
      display_xpm(false, four_img, *x, *y);
      break;
    case 5:
      *x -= five_img.width;
      display_xpm(false, five_img, *x, *y);
      break;
    case 6:
      *x -= six_img.width;
      display_xpm(false, six_img, *x, *y);
      break;
    case 7:
      *x -= seven_img.width;
      display_xpm(false, seven_img, *x, *y);
      break;
    case 8:
      *x -= eight_img.width;
      display_xpm(false, eight_img, *x, *y);
      break;
    case 9:
      *x -= nine_img.width;
      display_xpm(false, nine_img, *x, *y);
      break;
    default:
      break;
    }
  }
}

void(draw_time)(struct Time *rtc_time)
{
  printf("gg\n");
  if (rtc_time != NULL)
  {
    printf("%d %d %d \n", rtc_time->hours, rtc_time->minutes, rtc_time->seconds);
    uint16_t x = xRes - 10;
    uint16_t y = yRes - 10;
    y -= 22; //every number image height

    draw_two_digit_number(rtc_time->seconds, &x, &y);
    x -= two_dots_img.width;
    display_xpm(false, two_dots_img, x, y);
    draw_two_digit_number(rtc_time->minutes, &x, &y);
    x -= two_dots_img.width;
    display_xpm(false, two_dots_img, x, y);
    draw_two_digit_number(rtc_time->hours, &x, &y);
  }
}

void(draw_scene)(struct Time *rtc_time)
{
  aux_video_mem = memcpy(aux_video_mem, video_mem, xRes * yRes * ((bitsPerPixel + 7) / 8));
  memset(aux_video_mem, 0, xRes * yRes * ((bitsPerPixel + 7) / 8));
  draw_map();
  draw_snake();
  draw_apple();
  draw_score();
  draw_time(rtc_time);
  draw_mouse();
  video_mem = memcpy(video_mem, aux_video_mem, xRes * yRes * ((bitsPerPixel + 7) / 8));
}
