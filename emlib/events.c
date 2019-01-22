#define F_CPU 16000000

#include <stdint.h>
#include <util/delay.h>

#include "events.h"
#include "lcd.h"
#include "utils.h"

void button_pushed_event(char button_bit, volatile uint8_t *button_register,
void (*handler)(void)) {
	int button_pushed = (bit_status(button_bit, button_register) == 0);
	if (button_pushed) {
		_delay_ms(50);
		handler();
		while (button_pushed)
		button_pushed = bit_status(button_bit, button_register) == 0;
	}
}
