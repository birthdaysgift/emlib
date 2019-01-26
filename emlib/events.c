#define F_CPU 16000000

#include <ctype.h>
#include <stdint.h>
#include <string.h>
#include <util/delay.h>

#include "events.h"
#include "lcd.h"
#include "menu.h"
#include "menu_config.h"
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

void menu_next() {
	extern struct MenuItem *top_item;
	extern struct MenuItem *bottom_item;
	extern struct MenuItem *current_item;
	if (_menu_has_next()) {
		if (current_item == bottom_item) {
			top_item = bottom_item;
			bottom_item = current_item = current_item->next;
			top_item->text[0] = ' ';
			bottom_item->text[0] = '>';
			} else {
			current_item = current_item->next;
			top_item->text[0] = ' ';
			bottom_item->text[0] = '>';
		}
		lcd_clear();
		lcd_set_cursor(0, 0);
		lcd_puts(top_item->text);
		lcd_set_cursor(0, 1);
		lcd_puts(bottom_item->text);
	}
}

void menu_prev() {
	extern struct MenuItem *top_item;
	extern struct MenuItem *bottom_item;
	extern struct MenuItem *current_item;
	if (_menu_has_prev()) {
		if (current_item == top_item) {
			bottom_item = top_item;
			top_item = current_item = current_item->prev;
			top_item->text[0] = '>';
			bottom_item->text[0] = ' ';
			} else {
			current_item = current_item->prev;
			top_item->text[0] = '>';
			bottom_item->text[0] = ' ';
		}
		lcd_clear();
		lcd_set_cursor(0, 0);
		lcd_puts(top_item->text);
		lcd_set_cursor(0, 1);
		lcd_puts(bottom_item->text);
	}
}

void menu_enter() {
	extern struct MenuItem *top_item;
	extern struct MenuItem *bottom_item;
	extern struct MenuItem *current_item;

	if (_menu_has_action()) {
		current_item->action();
		return;
	}
	
	if (_menu_has_child())	{
		current_item = current_item->first_child;
		top_item = current_item;
		top_item->text[0] = '>';
		bottom_item = current_item->next;
		
		lcd_clear();
		
		lcd_set_cursor(0, 0);
		lcd_puts(top_item->text);
		
		if (bottom_item != NULL) {
			lcd_set_cursor(0, 1);
			lcd_puts(bottom_item->text);
		}
		return;
	}
	
	if (_menu_has_value()) {
		lcd_clear();
		lcd_set_cursor(0, 0);
		lcd_puts(current_item->text);
		lcd_set_cursor(0, 1);
		lcd_puts(current_item->value);
		lcd_set_cursor(0, 1);
		lcd_enable_cursor();
		while(bit_status(MENU_ENTER_BUTTON, &MENU_BUTTONS_PIN) == 0) {};
		while (1) {
			button_pushed_event(MENU_ENTER_BUTTON, &MENU_BUTTONS_PIN, value_enter);
			button_pushed_event(MENU_ESC_BUTTON, &MENU_BUTTONS_PIN, value_escape);
		}
		return;
	}
}

void menu_escape() {
	extern struct MenuItem *top_item;
	extern struct MenuItem *bottom_item;
	extern struct MenuItem *current_item;
	if (_menu_has_parent())	{
		current_item = current_item->parent;
		top_item = current_item;
		top_item->text[0] = '>';
		bottom_item = current_item->next;
		
		lcd_clear();
		
		lcd_set_cursor(0, 0);
		lcd_puts(current_item->text);
		
		if (bottom_item != NULL) {
			lcd_set_cursor(0, 1);
			lcd_puts(bottom_item->text);
		}
	}
}

void value_enter() {
	extern int value_cursor_position;
	value_cursor_position++;
	while (1) {
		if (value_cursor_position > DISPLAY_ROW_SIZE - 1)
			value_cursor_position = 0;
		if (isdigit(current_item->value[value_cursor_position])) {
			lcd_set_cursor(value_cursor_position, 1);
			break;
		}
		value_cursor_position++;
	}
}

void value_escape() {
	extern int value_cursor_position;
	value_cursor_position--;
	while (1) {
		if (value_cursor_position < 0)
			value_cursor_position = DISPLAY_ROW_SIZE - 1;
		if (isdigit(current_item->value[value_cursor_position])) {
			lcd_set_cursor(value_cursor_position, 1);
			break;
		}
		value_cursor_position--;
	}
}

int _menu_has_next() {
	extern struct MenuItem *current_item;
	return current_item->next != NULL;
}

int _menu_has_prev() {
	extern struct MenuItem *current_item;
	return current_item->prev != NULL;
}

int _menu_has_parent() {
	extern struct MenuItem *current_item;
	return current_item->parent != NULL;
}

int _menu_has_child() {
	extern struct MenuItem *current_item;
	return current_item->first_child != NULL;
}

int _menu_has_action() {
	extern struct MenuItem *current_item;
	return current_item->action != NULL;
}

int _menu_has_value() {
	extern struct MenuItem *current_item;
	return current_item->value != NULL;
}