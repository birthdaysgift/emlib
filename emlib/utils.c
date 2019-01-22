#include <stdint.h>

#include "utils.h"

void set_bit(char bit, volatile uint8_t *bit_register) {
	*bit_register |= (1 << bit);
}

void clear_bit(char bit, volatile uint8_t *bit_register) {
	*bit_register &= ~(1 << bit);
}

int bit_status(char bit, volatile uint8_t *bit_register) {
	return (*bit_register & (1 << bit)) != 0;
}