#include "graphics.h"

static void *video_mem, *aux_video_mem;
static uint16_t xRes, yRes;
static uint8_t bitsPerPixel;
static uint8_t memoryModel;
static uint8_t redMaskSize, greenMaskSize, blueMaskSize;
static uint8_t redFieldPosition, greenFieldPosition, blueFieldPosition;

int(return_VBE_controller_info)() {

  struct reg86 reg86;

  memset(&reg86, 0, sizeof(reg86)); /* zero the structure */

  reg86.intno = 0x10; /* BIOS video services */
  reg86.ax = 0x4F00;  /* Set Video Mode function */
  reg86.al = 0x03;    /* 80x25 text mode*/
  if (sys_int86(&reg86) != OK) {
    printf("return_VBE_controller_info: sys_int86() failed \n");
    return -1;
  }
  return 0;
}

int(return_VBE_mode_info)() {
  struct reg86 reg86;

  memset(&reg86, 0, sizeof(reg86));

  reg86.ax = 0x4F01; //return VBE mode information
  reg86.intno = 0x10;
  if (sys_int86(&reg86) != OK) {
    printf("return_VBE_mode_info: sys_int86() failed \n");
    return -1;
  }
  return reg86.ax;
}

int(set_VBE_mode)(uint16_t mode) {

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

  mr.mr_base = (phys_bytes) vram_base;
  mr.mr_limit = mr.mr_base + vram_size;

  if (OK != (r = sys_privctl(SELF, SYS_PRIV_ADD_MEM, &mr))) {
    panic("sys_privctl (ADD_MEM) failed: %d\n", r);
  }

  /* Map memory */
  video_mem = vm_map_phys(SELF, (void *) mr.mr_base, vram_size);
  if (video_mem == MAP_FAILED)
    panic("couldn't map video memory");

  // Set VBE Mode
  struct reg86 reg86;

  memset(&reg86, 0, sizeof(reg86));

  reg86.ax = 0x4F02;
  reg86.bx = 1 << 14 | mode; //Bit 14 = 1 - Use linear/flat frame buffer model
  reg86.intno = 0x10;
  if (sys_int86(&reg86) != OK) {
    printf("set_VBE_mode: sys_int86() failed \n");
    return -1;
  }

  return reg86.ax;
}

int(vg_draw_pixel)(int xPos, int x, int y, uint32_t color) {

  uint8_t *address = (uint8_t *) video_mem + (y * xRes + x + xPos) * ((bitsPerPixel + 7) / 8);
  for (size_t i = 0; i < ((bitsPerPixel + 7) / 8); i++) {
    *address = color >> (i * 8);
    ++address;
  }

  return 0;
}

int(vg_draw_hline)(uint16_t x, uint16_t y, uint16_t len, uint32_t color) {

  for (size_t xPos = 0; xPos < len; xPos++) {
    vg_draw_pixel(xPos, x, y, color);
  }

  return 0;
}

int(vg_draw_rectangle)(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint32_t color) {

  for (size_t i = 0; i < height; i++) {
    vg_draw_hline(x, y + i, width, color);
  }

  return 0;
}

int(vg_draw_pattern)(uint8_t no_rectangles, uint32_t first, uint8_t step) {

  uint16_t width = xRes / no_rectangles;
  uint16_t height = yRes / no_rectangles;
  uint16_t x = 0;
  uint16_t y = 0;
  uint32_t color;

  for (int i = 0; i < no_rectangles; i++) {
    for (int j = 0; j < no_rectangles; j++) {
      if (memoryModel == 0x04) { //0x04 - packed pixel/indexed color
        color = (first + (i * no_rectangles + j) * step) % (1 << bitsPerPixel);
      }
      else if (memoryModel == 0x06) { //0x06 - direct color
        // uint8_t bFirst = (uint8_t)(first << (8 - blueMaskSize)) >> (8 - blueMaskSize);
        // uint8_t gFirst = ((uint8_t)(first >> 8) << (8 - greenMaskSize)) >> (8 - greenMaskSize);
        // uint8_t rFirst = ((uint8_t)(first >> 16) << (8 - redMaskSize)) >> (8 - redMaskSize);
        uint8_t bFirst = first;
        uint8_t gFirst = first >> blueMaskSize;
        uint8_t rFirst = first >> (greenMaskSize + blueMaskSize);
        uint8_t r = (rFirst + j * step) % (1 << redMaskSize);
        uint8_t g = (gFirst + i * step) % (1 << greenMaskSize);
        uint8_t b = (bFirst + (j + i) * step) % (1 << blueMaskSize);
        color = (r << redFieldPosition) + (g << greenFieldPosition) + (b << blueFieldPosition);
      }
      vg_draw_rectangle(x, y, width, height, color);
      x += width;
    }
    x = 0;
    y += height;
  }

  return 0;
}

int(vg_fill_address)(bool vramDirectly, uint16_t x, uint16_t y, uint8_t color) {
  
  if (x < xRes && y < yRes) {
    uint8_t *address = NULL;

    if (vramDirectly == true) {
      address = (uint8_t *) video_mem + (y * xRes + x) * ((bitsPerPixel + 7) / 8);
    }
    else {
      address = (uint8_t *) aux_video_mem + (y * xRes + x) * ((bitsPerPixel + 7) / 8);
    }

    *address = color;
  }

  return 0;
}

int(display_xpm)(bool vramDirectly, xpm_map_t xpm, uint16_t x, uint16_t y) {
  enum xpm_image_type type = XPM_INDEXED;
  xpm_image_t img;
  uint8_t *sprite = xpm_load(xpm, type, &img);

  if (sprite == NULL) {
    printf("ERROR: sprite NULL\n  ");
    return 1;
  }

  for (size_t j = 0; j < img.height; j++) {
    for (size_t i = 0; i < img.width; i++) {
      vg_fill_address(vramDirectly, x + i, y + j, *sprite);
      ++sprite;
    }
  }

  return 0;
}

void(double_buffer_xpm)(xpm_map_t xpm, uint16_t x, uint16_t y) {
  aux_video_mem = memcpy(aux_video_mem, video_mem, xRes * yRes * ((bitsPerPixel + 7) / 8));
  memset(aux_video_mem, 0, xRes * yRes * ((bitsPerPixel + 7) / 8));
  display_xpm(false, xpm, x, y);
  video_mem = memcpy(video_mem, aux_video_mem, xRes * yRes * ((bitsPerPixel + 7) / 8));
}
