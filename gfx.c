#include "gfx.h"
#include "fonts/adafruit_gfx_library/glcdfont.c"

error_t gfx_init(gfx_context *ctx, uint8_t *buffer, uint8_t width,
                 uint8_t height) {
  if (!ctx || !buffer) {
    return E_NULL_PTR;
  }
  ctx->buffer = buffer;
  ctx->width = width;
  ctx->height = height;
  ctx->word_wrap = true;

  return E_SUCCESS;
}

// print checkers on display
error_t gfx_demo_checker(gfx_context *ctx) {
  // fill buffer
  for (uint16_t i = 0; i < (ctx->height * (ctx->width / 8)); ++i) {
    if (i % 2 == 0)
      ctx->buffer[i] = 0x55;
    else
      ctx->buffer[i] = 0xAA;
  }

  return E_SUCCESS;
}

error_t gfx_clear(gfx_context *ctx) {
  memset(ctx->buffer, 0, ctx->width * ctx->height / 8);
  return E_SUCCESS;
}

error_t gfx_draw_pixel(gfx_context *ctx, uint8_t x, uint8_t y) {
  if (x >= ctx->width || y >= ctx->height)
    return E_VALUE_TOO_BIG;

  ctx->buffer[x + (y / 8) * ctx->width] |= (1u << (y & 7u));
  return E_SUCCESS;
}

error_t gfx_clear_pixel(gfx_context *ctx, uint16_t x, uint16_t y) {
  if (x >= ctx->width || y >= ctx->height)
    return E_VALUE_TOO_BIG;

  ctx->buffer[x + (y / 8) * ctx->width] &= ~(1u << (y & 7u));
  return E_SUCCESS;
}

error_t gfx_draw_char(gfx_context *ctx, uint8_t x, uint8_t y, char c) {

  //  extern uint8_t font[];
  for (uint8_t i = 0; i < 5; i++) { // Char bitmap = 5 columns
    uint8_t line = font[c * 5 + i];
    for (uint8_t j = 0; j < 8; j++, line >>= 1u) {
      if (line & 1u) {
        gfx_draw_pixel(ctx, x + i, y + j);
      }
    }
  }

  return E_SUCCESS;
}

error_t gfx_draw_text(gfx_context *ctx, uint8_t x, uint8_t y,
                      const char *string) {
  uint8_t _x = x;
  uint8_t _y = y;
  uint8_t a = 0;
  while (string[a]) {
    if ((_x + 5) >= ctx->width) {
      _x = 0;
      _y += 8;
    }
    gfx_draw_char(ctx, _x, _y, string[a++]);
    _x += 5;
  }
  return E_SUCCESS;
}
