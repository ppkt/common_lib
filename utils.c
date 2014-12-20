#include "utils.h"

static __IO uint32_t _delay;

void delay(__IO uint32_t nTime)
{
  _delay = nTime;

  while(_delay != 0);
}

void delay_decrement(void)
{
  if (_delay--) {}
}
