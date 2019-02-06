#include <stdlib.h>
#include <string.h>

#include "events.h"
#include "lcd.h"
#include "menu.h"
#include "menu_config.h"
#include "utils.h"

void menu_init() {
	clear_bit(MENU_ENTER_BUTTON, &MENU_BUTTONS_DDR);
	clear_bit(MENU_NEXT_BUTTON, &MENU_BUTTONS_DDR);
	clear_bit(MENU_PREV_BUTTON, &MENU_BUTTONS_DDR);
	clear_bit(MENU_ESC_BUTTON, &MENU_BUTTONS_DDR);
	
	set_bit(MENU_ENTER_BUTTON, &MENU_BUTTONS_PORT);
	set_bit(MENU_NEXT_BUTTON, &MENU_BUTTONS_PORT);
	set_bit(MENU_PREV_BUTTON, &MENU_BUTTONS_PORT);
	set_bit(MENU_ESC_BUTTON, &MENU_BUTTONS_PORT);
	
	lcd_init();
	lcd_on();

	extern MenuItem *top_item;
	extern MenuItem *bottom_item;
	extern MenuItem *current_item;
	top_item = NULL;
	bottom_item = NULL;
	current_item = NULL;
	
	extern int value_cursor_position;
	value_cursor_position = 0;
	
	extern char *proper_password;
	extern char *current_password;
	proper_password = NULL;
	current_password = "0000";
}

MenuItem *menu_add_item(char *text, MenuItem *parent, MenuItem *prev) {
	return _menu_init_item(text, parent, prev, NULL, NULL);
}

MenuItem *menu_add_dir(char *text, MenuItem *parent, MenuItem *prev) {
	return _menu_init_item(text, parent, prev, NULL, NULL);
}

MenuItem *menu_add_action(char *text, MenuItem *parent, MenuItem *prev,
								 void (*action)()) {
	return _menu_init_item(text, parent, prev, action, NULL);
}

MenuItem *menu_add_config(char *text, MenuItem *parent, MenuItem *prev,
								 char *value) {
	return _menu_init_item(text, parent, prev, NULL, value);
}

int menu_action_running() {
	// returns 1 if button isn't pushed and 0 otherwise
	return bit_status(MENU_ESC_BUTTON, &MENU_BUTTONS_PIN) == 1;
}


char *menu_get_config_value(char *path ) {
	MenuItem *config_item = _menu_item_malloc();
	config_item = _get_config_item(path);
	return config_item->value;
}

void menu_set_config_value(char *path, char *value) {
	MenuItem *config_item = _menu_item_malloc();
	config_item = _get_config_item(path);
	config_item->value = calloc(strlen(value), sizeof (char));
	config_item->value = value;
}

void menu_set_password(char *password) {
	extern char *proper_password;
	proper_password = password;
}

void menu_finalize() {
	extern char *proper_password;
	extern char *current_password;
	if (proper_password != NULL) {
		lcd_clear();
		lcd_set_cursor(0, 0);
		lcd_puts("Password:");
		lcd_set_cursor(0, 1);
		lcd_puts(current_password);
		lcd_enable_cursor();
		lcd_set_cursor(0, 1);
		
		extern int value_loop_running;
		value_loop_running = 1;
		while (value_loop_running) {
			button_pushed_event(MENU_NEXT_BUTTON, &MENU_BUTTONS_PIN, password_next);
			button_pushed_event(MENU_PREV_BUTTON, &MENU_BUTTONS_PIN, password_prev);
			button_pushed_event(MENU_ESC_BUTTON, &MENU_BUTTONS_PIN, password_escape);
			button_pushed_event(MENU_ENTER_BUTTON, &MENU_BUTTONS_PIN, password_enter);
		};
		lcd_disable_cursor();
	}
	
	extern MenuItem *top_item;
	extern MenuItem *bottom_item;
	extern MenuItem *current_item;
	
	top_item = current_item;
	bottom_item = current_item->next;
	
	lcd_clear();
	
	lcd_set_cursor(0, 0);
	lcd_puts(_get_formatted_text(top_item));
	
	if (bottom_item != NULL) {
		lcd_set_cursor(0, 1);
		lcd_puts(_get_formatted_text(bottom_item));
	}
	
	extern int menu_loop_running;
	menu_loop_running = 1;
	while (menu_loop_running) {
		button_pushed_event(MENU_NEXT_BUTTON, &MENU_BUTTONS_PIN, menu_next);
		button_pushed_event(MENU_PREV_BUTTON, &MENU_BUTTONS_PIN, menu_prev);
		button_pushed_event(MENU_ENTER_BUTTON, &MENU_BUTTONS_PIN, menu_enter);
		button_pushed_event(MENU_ESC_BUTTON, &MENU_BUTTONS_PIN, menu_escape);
	}
}

MenuItem *_get_config_item(char *path) {
	extern MenuItem *current_item;
	MenuItem *looking_item = _menu_item_malloc();
	looking_item = current_item;
	
	while (looking_item->parent != NULL)
	looking_item = looking_item->parent;
	
	while (looking_item->prev != NULL)
	looking_item = looking_item->prev;
	
	int i = 0, j = 0, k = 0;
	while (j < strlen(path)) {
		if (path[j] == '/') {
			looking_item = looking_item->first_child;
			i = 0;
			j++;
			k = j;
			continue;
		}
		if (path[j] != looking_item->text[i]) {
			if (looking_item->next == NULL) {
				return NULL;
				} else {
				looking_item = looking_item->next;
				j = k;
				i = 0;
				continue;
			}
		}
		i++;
		j++;
	}
	return looking_item;
}

char *_get_formatted_text(MenuItem *item) {
	extern MenuItem *current_item;
	int i = 0, j = 0;
	char *formatted_text = calloc(DISPLAY_ROW_SIZE, sizeof (char));
	formatted_text[i++] = (item == current_item) ? '>' : ' ';
	formatted_text[i++] = (item->parent == NULL) ? item->text[j++] : '/';
	while (i < DISPLAY_ROW_SIZE && j < strlen(item->text))
		formatted_text[i++] = item->text[j++];
	if (item->first_child != NULL) {
		if (strlen(item->text) > DISPLAY_ROW_SIZE - 1)
			formatted_text[DISPLAY_ROW_SIZE - 1] = '/';
		else
			formatted_text[i] = '/';	
	}
	return formatted_text;
}

MenuItem *_menu_init_item(char *text, MenuItem *parent, MenuItem *prev,
							     void (*action)(), char *value) {
	// Remember it. It will be next for the new_item
	MenuItem *next = NULL;
	if (prev != NULL && prev->next != NULL)
		next = prev->next;
	
	// Initialize new_item
	MenuItem *new_item = _menu_item_malloc();
	new_item->text = text;
	new_item->parent = parent;
	new_item->prev = prev;
	new_item->next = next;
	new_item->action = action;
	new_item->value = value;
	new_item->first_child = NULL;

	// Change links in previous and next items of the new_item
	if (prev != NULL)
		prev->next = new_item;
	if (next != NULL)
		next->prev = new_item;
	
	// Change link in the parent of the new_item
	if (parent->first_child == NULL)
		parent->first_child = new_item;
	
	// If new_item is first in the menu (have no parent and no items before it)
	// set it as current_item
	if (parent == NULL && prev == NULL)
		current_item = new_item;
	
	return new_item;
}

MenuItem *_menu_item_malloc() {
	return (MenuItem *) malloc(sizeof (MenuItem));
}