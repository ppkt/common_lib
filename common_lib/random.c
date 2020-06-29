#include "random.h"

static uint32_t x = 123456789, y = 234567891, z = 345678912, w = 456789123,
                c = 0;

uint32_t rand_kiss32(void) {
  uint32_t t;

  y ^= (y << 5u);
  y ^= (y >> 7u);
  y ^= (y << 22u);

  t = z + w + c;
  z = w;
  c = t < 0;
  w = t & 2147483647u;

  x += 1411392427;

  return x + y + w;
}
