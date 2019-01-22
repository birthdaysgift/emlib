#include <stdint.h>

void button_pushed_event(char, volatile uint8_t *, void (*handler)(void));
