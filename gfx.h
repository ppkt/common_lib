#pragma once
#include "utils.h"
#include <stdint-gcc.h>
#include <string.h>

typedef struct {
  uint8_t width;
  uint8_t height;
  bool word_wrap;
  uint8_t *buffer;
} gfx_context;

error_t gfx_init(gfx_context *ctx, uint8_t *buffer, uint8_t width,
                 uint8_t height);

error_t gfx_demo_checker(gfx_context *ctx);

error_t gfx_clear(gfx_context *ctx);

error_t gfx_draw_char(gfx_context *ctx, uint8_t x, uint8_t y, char c);

error_t gfx_draw_text(gfx_context *ctx, uint8_t x, uint8_t y,
                      const char *string);

error_t gfx_draw_pixel(gfx_context *ctx, uint8_t x, uint8_t y);

error_t gfx_clear_pixel(gfx_context *ctx, uint16_t x, uint16_t y);
