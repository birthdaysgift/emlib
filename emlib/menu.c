#include <stdlib.h>
#include <string.h>

#include "lcd.h"
#include "menu.h"

void menu_init() {
	lcd_init();
	lcd_on();

	extern struct MenuItem *top_item;
	extern struct MenuItem *bottom_item;
	extern struct MenuItem *current_item;
	top_item = NULL;
	bottom_item = NULL;
	current_item = NULL;
}

struct MenuItem *menu_add_item(char *text, struct MenuItem *parent, struct MenuItem *prev) {
	struct MenuItem *item = _menu_init_item(text, parent, prev, NULL, NULL);
	_format_item_text(item);
	return item;
}

struct MenuItem *menu_add_dir(char *text, struct MenuItem *parent, struct MenuItem *prev) {
	struct MenuItem *item = _menu_init_item(text, parent, prev, NULL, NULL);
	_format_dir_text(item);
	return item;
}

struct MenuItem *menu_add_action(char *text, struct MenuItem *parent, struct MenuItem *prev,
								 void (*action)()) {
	struct MenuItem *item = _menu_init_item(text, parent, prev, action, NULL);
	_format_action_text(item);
	return item;
}

struct MenuItem *menu_add_config(char *text, struct MenuItem *parent, struct MenuItem *prev,
								 char *value) {
	struct MenuItem *item = _menu_init_item(text, parent, prev, NULL, value);
	_format_item_text(item);
	return item;									 
}

void _format_item_text(struct MenuItem *item) {
	int display_row_size = 16;
	int raw_text_len = strlen(item->text);
	int i = 0;
	int j = 0;
	char *formatted_text = calloc(display_row_size, sizeof (char));
	formatted_text[i++] = ' ';
	formatted_text[i++] = (item->parent == NULL) ? item->text[j++] : '/';
	while (i < display_row_size && j < raw_text_len)
		formatted_text[i++] = item->text[j++];
	int result_text_len = i;
	item->text = calloc(result_text_len, sizeof (char));
	for (int k = 0; k < result_text_len && formatted_text[k] != 0; k++)
		item->text[k] = formatted_text[k];
}

void _format_dir_text(struct MenuItem *item) {
	_format_item_text(item);
	int display_row_size = 16;
	int item_text_len = strlen(item->text);
	if (item_text_len == display_row_size) {
		item->text[item_text_len-1] = '/';
	} else {
		// Add '/' in the end of item->text
		int dir_text_len = item_text_len + 1;
		char *dir_text = calloc(dir_text_len, sizeof (char));
		for (int i = 0; i < item_text_len; i++)
			dir_text[i] = item->text[i];
		dir_text[dir_text_len - 1] = '/';
		item->text = calloc(dir_text_len, sizeof (char));
		item->text = dir_text;
	}
}

void _format_action_text(struct MenuItem *item) {
	_format_item_text(item);
}

struct MenuItem *_menu_init_item(char *text, struct MenuItem *parent, struct MenuItem *prev,
							     void (*action)(), char *value) {
	// Remember it. It will be next for the new_item
	struct MenuItem *next = NULL;
	if (prev != NULL && prev->next != NULL)
		next = prev->next;
	
	// Initialize new_item
	struct MenuItem *new_item = _menu_item_malloc();
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

	if (_menu_has_action())
		current_item->action();
	
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

void menu_finalize() {
	extern struct MenuItem *top_item;
	extern struct MenuItem *bottom_item;
	extern struct MenuItem *current_item;
	
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

struct MenuItem *_menu_item_malloc() {
	return (struct MenuItem *) malloc(sizeof (struct MenuItem));
}