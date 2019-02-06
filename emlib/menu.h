struct _MenuItem {
	struct _MenuItem *parent;
	struct _MenuItem *prev;
	struct _MenuItem *next;
	struct _MenuItem *first_child;
	void (*action)();
	char *value;
	char *text;
};

typedef struct _MenuItem MenuItem;

MenuItem *top_item;
MenuItem *bottom_item;
MenuItem *current_item;

char *proper_password;
char *current_password;

int value_cursor_position;

void menu_init();

MenuItem *menu_add_item(char *text, MenuItem *parent, MenuItem *prev);
MenuItem *menu_add_dir(char *text, MenuItem *parent, MenuItem *prev);
MenuItem *menu_add_action(char *text, MenuItem *parent, MenuItem *prev, void (*action)());
MenuItem *menu_add_config(char *text, MenuItem *parent, MenuItem *prev, char *value);

int menu_action_running();

MenuItem *_get_config_item(char *path);
char *menu_get_config_value(char *path);
void menu_set_config_value(char *path, char *value);

void menu_set_password(char *password);

MenuItem *_menu_init_item(char *text, MenuItem *parent, MenuItem *prev,
							     void (*action)(), char *value);

char *_get_formatted_text(MenuItem *item);
							   
void _format_item_text(MenuItem *item);
void _format_dir_text(MenuItem *item);
void _format_action_text(MenuItem *item);

void menu_finalize();

MenuItem *_menu_item_malloc();