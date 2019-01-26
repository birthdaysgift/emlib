#include <stdint.h>

void button_pushed_event(char, volatile uint8_t *, void (*handler)(void));

void menu_next();
void menu_prev();
void menu_enter();
void menu_escape();

void value_next();
void value_prev();
void value_enter();
void value_escape();

int _menu_has_next();
int _menu_has_prev();
int _menu_has_parent();
int _menu_has_child();
int _menu_has_action();
int _menu_has_value();