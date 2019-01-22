#include <stdint.h>

void set_bit(char bit, volatile uint8_t *bit_register);
void clear_bit(char bit, volatile uint8_t *bit_register);
int bit_status(char bit, volatile uint8_t *bit_register);