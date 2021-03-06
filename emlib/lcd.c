﻿#include "lcd.h"

#include <stdarg.h>
#include <stdio.h>
#include <util/delay.h>

static void lcd_send(uint8_t value, uint8_t mode);
static void lcd_write_nibble(uint8_t nibble);

static void lcd_command(uint8_t command);
static void lcd_write(uint8_t value);

static uint8_t lcd_displayparams;
static char lcd_buffer[LCD_COL_COUNT + 1];

void lcd_command (uint8_t command) {
	lcd_send(command, 0);
}

void lcd_write(uint8_t value) {
	lcd_send(value, 1);
}

void lcd_send(uint8_t value, uint8_t mode) {
	if (mode) {
		LCD_RS_PORT = LCD_RS_PORT | (1 << LCD_RS_BIT);
		} else {
		LCD_RS_PORT = LCD_RS_PORT & ~(1 << LCD_RS_BIT);
	}

	LCD_RW_PORT = LCD_RW_PORT & ~(1 << LCD_RW_BIT);

	lcd_write_nibble(value >> 4);
	lcd_write_nibble(value);
}

void lcd_write_nibble (uint8_t nibble) {

	LCD_DATA_PORT &= ~ ((1 << LCD_D3) | (1 << LCD_D2) | (1 << LCD_D1) | (1 << LCD_D0));

	if (nibble & 0x08) LCD_DATA_PORT |= (1 << LCD_D3);
	if (nibble & 0x04) LCD_DATA_PORT |= (1 << LCD_D2);
	if (nibble & 0x02) LCD_DATA_PORT |= (1 << LCD_D1);
	if (nibble & 0x01) LCD_DATA_PORT |= (1 << LCD_D0);
	
	LCD_EN_PORT = LCD_EN_PORT & ~(1 << LCD_EN_BIT);
	LCD_EN_PORT = LCD_EN_PORT | (1 << LCD_EN_BIT);
	_delay_ms(1);
	LCD_EN_PORT = LCD_EN_PORT & ~(1 << LCD_EN_BIT);
	_delay_ms(10); // If delay less than this value, the data is not correctly displayed
}

void lcd_init(void) {
	// Configure pins as output
	LCD_DATA_DDR |= (1<<LCD_D3)|(1<<LCD_D2)|(1<<LCD_D1)|(1<<LCD_D0);

	LCD_RS_DDR |= (1<<LCD_RS_BIT);
	LCD_RW_DDR |= (1<<LCD_RW_BIT);
	LCD_EN_DDR |= (1<<LCD_EN_BIT);

	// Wait for LCD to become ready (docs say 15ms+)
	_delay_ms(100);

	LCD_RS_PORT &= ~(1<<LCD_RS_BIT);
	LCD_RW_PORT &= ~(1<<LCD_RW_BIT);
	LCD_EN_PORT &= ~(1<<LCD_EN_BIT);

	_delay_ms(15);

	lcd_write_nibble(0x03); // Switch to 8 bit mode
	_delay_ms(15);

	lcd_write_nibble(0x03);// 2nd time
	_delay_ms(100);

	lcd_write_nibble(0x03);// 3rd time
	_delay_ms(15);

	lcd_write_nibble(0x02);// Set 4-bit mode

	_delay_ms(15);

	lcd_command(LCD_FUNCTIONSET|LCD_4BITMODE|LCD_2LINE|LCD_5x8DOTS);

	lcd_displayparams=LCD_CURSOROFF|LCD_BLINKOFF;
	lcd_command(LCD_DISPLAYCONTROL|lcd_displayparams);
}

void lcd_on(void) {
	lcd_displayparams|=LCD_DISPLAYON;
	lcd_command(LCD_DISPLAYCONTROL|lcd_displayparams);
}

void lcd_off(void) {
	lcd_displayparams &= ~LCD_DISPLAYON;
	lcd_command(LCD_DISPLAYCONTROL|lcd_displayparams);
}

void lcd_clear(void) {
	lcd_command(LCD_CLEARDISPLAY);
	_delay_ms(2);
}

void lcd_return_home(void) {
	lcd_command(LCD_RETURNHOME);
	_delay_ms(2);
}

void lcd_enable_blinking(void) {
	lcd_displayparams|=LCD_BLINKON;
	lcd_command(LCD_DISPLAYCONTROL|lcd_displayparams);
}

void lcd_disable_blinking(void) {
	lcd_displayparams&= ~LCD_BLINKON;
	lcd_command(LCD_DISPLAYCONTROL|lcd_displayparams);
}

void lcd_enable_cursor(void) {
	lcd_displayparams|=LCD_CURSORON;
	lcd_command(LCD_DISPLAYCONTROL|lcd_displayparams);
}

void lcd_disable_cursor(void) {
	lcd_displayparams&= ~LCD_CURSORON;
	lcd_command(LCD_DISPLAYCONTROL|lcd_displayparams);
}

void lcd_set_cursor(uint8_t col, uint8_t row) {
	static uint8_t offsets[] = {0x00, 0x40, 0x14, 0x54 };

	if (row > 1) {
		row = 1 ;
	}

	lcd_command(LCD_SETDDRAMADDR | (col + offsets[row]));
}

void lcd_puts (char *string) {

	char *it = string;

	for (; *it; it++) {
		lcd_write( *it);
	}
}

void lcd_printf(char *format, ...) {
	va_list args;

	va_start(args, format);
	vsnprintf(lcd_buffer, LCD_COL_COUNT + 1 , format, args);
	va_end(args);

	lcd_puts(lcd_buffer);
}