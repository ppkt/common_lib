#include "gfx.h"
#include "fonts/adafruit_gfx_library/glcdfont.c"

static error_t _gfx_min_max_point(const gfx_point *p1, const gfx_point *p2,
                                  gfx_point *out_1, gfx_point *out_2) {
  if (!out_1 || !out_2) {
    return E_NULL_PTR;
  }

  uint16_t min_x = min(p1->x, p2->x);
  uint16_t max_x = max(p1->x, p2->x);
  uint16_t min_y = min(p1->y, p2->y);
  uint16_t max_y = max(p1->y, p2->y);

  out_1->x = min_x;
  out_1->y = min_y;
  out_2->x = max_x;
  out_2->y = max_y;
  return E_SUCCESS;
}

error_t gfx_init(gfx_context *ctx, uint8_t *buffer, uint16_t width,
                 uint16_t height) {
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

error_t gfx_draw_pixel(gfx_context *ctx, uint16_t x, uint16_t y) {
  if (x >= ctx->width || y >= ctx->height)
    return E_SUCCESS;

  ctx->buffer[x + (y / 8) * ctx->width] |= (1u << (y & 7u));
  return E_SUCCESS;
}

error_t gfx_clear_pixel(gfx_context *ctx, uint16_t x, uint16_t y) {
  if (x >= ctx->width || y >= ctx->height)
    return E_SUCCESS;

  ctx->buffer[x + (y / 8) * ctx->width] &= ~(1u << (y & 7u));
  return E_SUCCESS;
}

error_t gfx_draw_gfx_pixel(gfx_context *ctx, const gfx_point *pt) {
  check_error(gfx_draw_pixel(ctx, pt->x, pt->y));
  return E_SUCCESS;
}

error_t gfx_draw_char(gfx_context *ctx, uint16_t x, uint16_t y, char c) {

  //  extern uint8_t font[];
  for (uint8_t i = 0; i < 5; i++) { // Char bitmap = 5 columns
    uint8_t line = font[c * 5 + i];
    for (uint8_t j = 0; j < 8; j++, line >>= 1u) {
      if (line & 1u) {
        check_error(gfx_draw_pixel(ctx, x + i, y + j));
      }
    }
  }

  return E_SUCCESS;
}

error_t gfx_draw_text(gfx_context *ctx, uint16_t x, uint16_t y,
                      const char *string) {
  uint16_t _x = x;
  uint16_t _y = y;
  uint8_t a = 0;
  while (string[a]) {
    if ((_x + 5) >= ctx->width) {
      _x = 0;
      _y += 8;
    }
    check_error(gfx_draw_char(ctx, _x, _y, string[a++]));
    _x += 5;
  }
  return E_SUCCESS;
}

static error_t _gfx_draw_line_low(gfx_context *ctx, const gfx_point *p1,
                                  const gfx_point *p2) {
  int16_t d_x = p2->x - p1->x;
  int16_t d_y = p2->y - p1->y;
  int8_t y_i = 1;

  if (d_y < 0) {
    y_i = -1;
    d_y = -d_y;
  }
  int16_t d = 2 * d_y - d_x;
  int16_t y = p1->y;

  for (uint16_t x = p1->x; x < p2->x; ++x) {
    check_error(gfx_draw_gfx_pixel(ctx, &(gfx_point){.x = x, .y = y}));

    if (d > 0) {
      y += y_i;
      d -= 2 * d_x;
    }
    d += 2 * d_y;
  }

  return E_SUCCESS;
}

static error_t _gfx_draw_line_high(gfx_context *ctx, const gfx_point *p1,
                                   const gfx_point *p2) {
  int16_t d_x = p2->x - p1->x;
  int16_t d_y = p2->y - p1->y;
  int8_t x_i = 1;

  if (d_x < 0) {
    x_i = -1;
    d_x = -d_x;
  }
  int16_t d = 2 * d_x - d_y;
  int16_t x = p1->x;

  for (uint16_t y = p1->y; y < p2->y; ++y) {
    check_error(gfx_draw_gfx_pixel(ctx, &(gfx_point){.x = x, .y = y}));

    if (d > 0) {
      x += x_i;
      d -= 2 * d_y;
    }
    d += 2 * d_x;
  }

  return E_SUCCESS;
}

static error_t _gfx_draw_line_horizontal(gfx_context *ctx, const gfx_point *p1,
                                         const gfx_point *p2) {
  if (p1->y != p2->y) {
    return E_VALUE_INVALID;
  }

  for (uint16_t x = p1->x; x <= p2->x; ++x) {
    check_error(gfx_draw_gfx_pixel(ctx, &(gfx_point){.x = x, .y = p1->y}));
  }

  return E_SUCCESS;
}

static error_t _gfx_draw_line_vertical(gfx_context *ctx, const gfx_point *p1,
                                       const gfx_point *p2) {
  if (p1->x != p2->x) {
    return E_VALUE_INVALID;
  }

  for (uint16_t y = p1->y; y <= p2->y; ++y) {
    check_error(gfx_draw_gfx_pixel(ctx, &(gfx_point){.x = p1->x, .y = y}));
  }

  return E_SUCCESS;
}

error_t gfx_draw_line(gfx_context *ctx, const gfx_point *p1,
                      const gfx_point *p2) {

  gfx_point _p1, _p2;
  check_error(_gfx_min_max_point(p1, p2, &_p1, &_p2));

  // Special case for horizontal / vertical lines
  if (_p1.y == _p2.y) {
    return _gfx_draw_line_horizontal(ctx, &_p1, &_p2);
  } else if (_p1.x == _p2.x) {
    return _gfx_draw_line_vertical(ctx, &_p1, &_p2);
  }

  // Use Bresenham algorithm
  if (abs(p2->y - p1->y) < abs(p2->x - p1->x)) {
    if (p1->x > p2->x) {
      return _gfx_draw_line_low(ctx, p2, p1);
    } else {
      return _gfx_draw_line_low(ctx, p1, p2);
    }
  } else {
    if (p1->y > p2->y) {
      return _gfx_draw_line_high(ctx, p2, p1);
    } else {
      return _gfx_draw_line_high(ctx, p1, p2);
    }
  }
}

error_t gfx_draw_rectangle(gfx_context *ctx, const gfx_point *p1,
                           const gfx_point *p2) {
  gfx_point _p1, _p2;
  check_error(_gfx_min_max_point(p1, p2, &_p1, &_p2));

  gfx_point corner_1 = {.x = _p2.x, .y = _p1.y};
  gfx_point corner_2 = {.x = _p1.x, .y = _p2.y};

  check_error(_gfx_draw_line_horizontal(ctx, &_p1, &corner_1));
  check_error(_gfx_draw_line_horizontal(ctx, &corner_2, &_p2));

  check_error(_gfx_draw_line_vertical(ctx, &_p1, &corner_2));
  check_error(_gfx_draw_line_vertical(ctx, &corner_1, &_p2));

  return E_SUCCESS;
}
error_t gfx_draw_circle(gfx_context *ctx, const gfx_point *center,
                        uint16_t radius) {
  int16_t f = 1 - radius;
  int16_t d_x = 0;
  int16_t d_y = -2 * radius;
  int16_t x = 0;
  int16_t y = radius;

  gfx_draw_gfx_pixel(ctx,
                     &(gfx_point){.x = center->x, .y = center->y + radius});
  gfx_draw_gfx_pixel(ctx,
                     &(gfx_point){.x = center->x, .y = center->y - radius});
  gfx_draw_gfx_pixel(ctx,
                     &(gfx_point){.x = center->x + radius, .y = center->y});
  gfx_draw_gfx_pixel(ctx,
                     &(gfx_point){.x = center->x - radius, .y = center->y});

  while (x < y) {
    if (f >= 0) {
      --y;
      d_y += 2;
      f += d_y;
    }
    ++x;
    d_x += 2;
    f += d_x + 1;

    check_error(gfx_draw_gfx_pixel(
        ctx, &(gfx_point){.x = center->x + x, .y = center->y + y}));
    check_error(gfx_draw_gfx_pixel(
        ctx, &(gfx_point){.x = center->x - x, .y = center->y + y}));
    check_error(gfx_draw_gfx_pixel(
        ctx, &(gfx_point){.x = center->x + x, .y = center->y - y}));
    check_error(gfx_draw_gfx_pixel(
        ctx, &(gfx_point){.x = center->x - x, .y = center->y - y}));

    check_error(gfx_draw_gfx_pixel(
        ctx, &(gfx_point){.x = center->x + y, .y = center->y + x}));
    check_error(gfx_draw_gfx_pixel(
        ctx, &(gfx_point){.x = center->x - y, .y = center->y + x}));
    check_error(gfx_draw_gfx_pixel(
        ctx, &(gfx_point){.x = center->x + y, .y = center->y - x}));
    check_error(gfx_draw_gfx_pixel(
        ctx, &(gfx_point){.x = center->x - y, .y = center->y - x}));
  }

  return E_SUCCESS;
}

error_t gfx_fill_rectangle(gfx_context *ctx, const gfx_point *p1,
                           const gfx_point *p2) {
  gfx_point _p1, _p2;
  check_error(_gfx_min_max_point(p1, p2, &_p1, &_p2));

  for (uint16_t y = _p1.y; y <= _p2.y; ++y) {
    check_error(_gfx_draw_line_horizontal(ctx, &(gfx_point){.x = _p1.x, .y = y},
                                          &(gfx_point){.x = _p2.x, .y = y}));
  }

  return E_SUCCESS;
}
