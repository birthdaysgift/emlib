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
	// Remember it. It will be next for the new_item
	struct MenuItem *next = NULL;
	if (prev != NULL && prev->next != NULL)
		next = prev->next;
	
	// Initialize new_item
	struct MenuItem *new_item = _menu_item_malloc();
	new_item->text = _get_item_text(text);
	new_item->parent = parent;
	new_item->prev = prev;
	new_item->next = next;
	new_item->first_child = NULL;
	new_item->action = NULL;

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

struct MenuItem *menu_add_dir(char *text, struct MenuItem *parent, struct MenuItem *prev) {
	// Remember it. It will be next for the new_dir
	struct MenuItem *next = NULL;
	if (prev != NULL && prev->next != NULL)
		next = prev->next;
	
	// Initialize new_dir
	struct MenuItem *new_dir = _menu_item_malloc();
	new_dir->text = _get_dir_text(text);
	new_dir->parent = parent;
	new_dir->prev = prev;
	new_dir->next = next;
	new_dir->first_child = NULL;
	new_dir->action = NULL;
	
	// Change links in previous and next items of the new_dir
	if (prev != NULL)
		prev->next = new_dir;
	if (next != NULL)
		next->prev = new_dir;
	
	// Change link in the parent of the new_dir
	if (parent->first_child == NULL)
		parent->first_child = new_dir;
	
	// If new_item is first in the menu (have no parent and no items before it)
	// set it as current_item
	if (parent == NULL && prev == NULL)
		current_item = new_dir;
	
	return new_dir;
}

struct MenuItem *menu_add_action(char *text, struct MenuItem *parent, struct MenuItem *prev,
void (*action)()) {
	// Remember it. It will be next for the new_item
	struct MenuItem *next = NULL;
	if (prev != NULL && prev->next != NULL)
		next = prev->next;
	
	// Initialize new_item
	struct MenuItem *new_item = _menu_item_malloc();
	new_item->text = (parent != NULL) ? _get_action_text(text) : text;
	new_item->parent = parent;
	new_item->prev = prev;
	new_item->next = next;
	new_item->first_child = NULL;
	new_item->action = action;

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

char *_get_dir_text(char *source_text) {
	int source_len = strlen(source_text);
	int result_len = (source_len > 13) ? 16 : source_len+3;
	char *result_text;
	result_text = calloc(result_len, sizeof (char));
	result_text[0] = ' ';
	result_text[1] = '/';
	for (int i = 2; i <= result_len-2; i++)
		result_text[i] = source_text[i-2];
	result_text[result_len-1] = '/';
	return result_text;
}

char *_get_item_text(char *source_text) {
	int source_len = strlen(source_text);
	int result_len = (source_len > 14) ? 16 : source_len+2;
	char *result_text;
	result_text = calloc(result_len, sizeof (char));
	result_text[0] = ' ';
	result_text[1] = '/';
	for (int i = 2; i <= result_len-1; i++)
		result_text[i] = source_text[i-2];
	return result_text;
}

char *_get_action_text(char *source_text) {
	return _get_item_text(source_text);
}

struct MenuItem *_menu_item_malloc() {
	return (struct MenuItem *) malloc(sizeof (struct MenuItem));
}