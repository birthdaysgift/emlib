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
			//top_item->text[0] = ' ';
			//bottom_item->text[0] = '>';
		} else {
			current_item = current_item->next;
			//top_item->text[0] = ' ';
			//bottom_item->text[0] = '>';
		}
		lcd_clear();
		lcd_set_cursor(0, 0);
		lcd_puts(_get_formatted_text(top_item));
		lcd_set_cursor(0, 1);
		lcd_puts(_get_formatted_text(bottom_item));
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
			//top_item->text[0] = '>';
			//bottom_item->text[0] = ' ';
		} else {
			current_item = current_item->prev;
			//top_item->text[0] = '>';
			//bottom_item->text[0] = ' ';
		}
		lcd_clear();
		lcd_set_cursor(0, 0);
		lcd_puts(_get_formatted_text(top_item));
		lcd_set_cursor(0, 1);
		lcd_puts(_get_formatted_text(bottom_item));
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
		//top_item->text[0] = '>';
		bottom_item = current_item->next;
		
		lcd_clear();
		
		lcd_set_cursor(0, 0);
		lcd_puts(_get_formatted_text(top_item));
		
		if (bottom_item != NULL) {
			lcd_set_cursor(0, 1);
			lcd_puts(_get_formatted_text(bottom_item));
		}
		return;
	}
	
	if (_menu_has_value()) {
		lcd_clear();
		lcd_set_cursor(0, 0);
		lcd_puts(_get_formatted_text(current_item));
		lcd_set_cursor(0, 1);
		lcd_puts(current_item->value);
		lcd_set_cursor(0, 1);
		lcd_enable_cursor();
		while(bit_status(MENU_ENTER_BUTTON, &MENU_BUTTONS_PIN) == 0) {};
			
		extern int value_loop_running;
		value_loop_running = 1;
		while (value_loop_running) {
			button_pushed_event(MENU_NEXT_BUTTON, &MENU_BUTTONS_PIN, value_next);
			button_pushed_event(MENU_PREV_BUTTON, &MENU_BUTTONS_PIN, value_prev);
			button_pushed_event(MENU_ENTER_BUTTON, &MENU_BUTTONS_PIN, value_enter);
			button_pushed_event(MENU_ESC_BUTTON, &MENU_BUTTONS_PIN, value_escape);
		}
		lcd_disable_cursor();
		lcd_clear();
		lcd_set_cursor(0, 0);
		lcd_puts(_get_formatted_text(top_item));
		if (bottom_item != NULL) {
			lcd_set_cursor(0, 1);
			lcd_puts(_get_formatted_text(bottom_item));
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
		//top_item->text[0] = '>';
		bottom_item = current_item->next;
		
		lcd_clear();
		
		lcd_set_cursor(0, 0);
		lcd_puts(_get_formatted_text(current_item));
		
		if (bottom_item != NULL) {
			lcd_set_cursor(0, 1);
			lcd_puts(_get_formatted_text(bottom_item));
		}
	}
}

void value_next() {
	extern int value_cursor_position;
	current_item->value[value_cursor_position]--;
	if (current_item->value[value_cursor_position] < '0')
		current_item->value[value_cursor_position] = '9';
	lcd_set_cursor(0, 1);
	lcd_puts(current_item->value);
	lcd_set_cursor(value_cursor_position, 1);	
}

void value_prev() {
	extern int value_cursor_position;
	current_item->value[value_cursor_position]++;
	if (current_item->value[value_cursor_position] > '9')
		current_item->value[value_cursor_position] = '0';
	lcd_set_cursor(0, 1);
	lcd_puts(current_item->value);
	lcd_set_cursor(value_cursor_position, 1);
}

void value_enter() {
	extern int value_cursor_position;
	extern int value_loop_running;
	value_cursor_position++;
	while (1) {
		if (value_cursor_position > DISPLAY_ROW_SIZE - 1) {
			value_cursor_position = 0;
			value_loop_running = 0;
			break;
		}
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
		if (value_cursor_position < 0) {
			value_cursor_position = 0;
			value_loop_running = 0;
			break;
		}
		if (isdigit(current_item->value[value_cursor_position])) {
			lcd_set_cursor(value_cursor_position, 1);
			break;
		}
		value_cursor_position--;
	}
}

void password_next() {
	extern int value_cursor_position;
	extern char *current_password;
	current_password[value_cursor_position]--;
	if (current_password[value_cursor_position] < '0')
		current_password[value_cursor_position] = '9';
	lcd_set_cursor(0, 1);
	lcd_puts(current_password);
	lcd_set_cursor(value_cursor_position, 1);
}

void password_prev() {
	extern int value_cursor_position;
	extern char *current_password;
	current_password[value_cursor_position]++;
	if (current_password[value_cursor_position] > '9')
		current_password[value_cursor_position] = '0';
	lcd_set_cursor(0, 1);
	lcd_puts(current_password);
	lcd_set_cursor(value_cursor_position, 1);
}

void password_enter() {
	extern char *current_password;
	extern char *proper_password;
	extern int value_cursor_position;
	extern int value_loop_running;
	value_cursor_position++;
	while (1) {
		if (value_cursor_position > strlen(current_password) - 1) {
			if (strcmp(current_password, proper_password) == 0) {
				value_cursor_position = 0;
				value_loop_running = 0;
				break;				
			} else {
				lcd_clear();
				lcd_set_cursor(0, 0);
				lcd_puts("Password:");
				lcd_set_cursor(0, 1);
				lcd_puts("wrong password");
				lcd_disable_cursor();
				_delay_ms(1000);
				lcd_clear();
				lcd_set_cursor(0, 0);
				lcd_puts("Password:");
				lcd_set_cursor(0, 1);
				lcd_puts(current_password);
				lcd_enable_cursor();
				value_cursor_position--;
				continue;
			}
		}
		if (isdigit(current_password[value_cursor_position])) {
			lcd_set_cursor(value_cursor_position, 1);
			break;
		}
		value_cursor_position++;
	}
}

void password_escape() {
	extern int value_cursor_position;
	extern char *current_password;
	value_cursor_position--;
	while (1) {
		if (value_cursor_position < 0) {
			if (strcmp(current_password, proper_password) == 0) {
				value_cursor_position = 0;
				value_loop_running = 0;
				break;
			} else {
				lcd_clear();
				lcd_set_cursor(0, 0);
				lcd_puts("Password:");
				lcd_set_cursor(0, 1);
				lcd_puts("wrong password");
				lcd_disable_cursor();
				_delay_ms(1000);
				lcd_clear();
				lcd_set_cursor(0, 0);
				lcd_puts("Password:");
				lcd_set_cursor(0, 1);
				lcd_puts(current_password);
				lcd_enable_cursor();
				value_cursor_position++;
				continue;
			}
		}
		if (isdigit(current_password[value_cursor_position])) {
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