struct MenuItem {
	struct MenuItem *parent;
	struct MenuItem *prev;
	struct MenuItem *next;
	struct MenuItem *first_child;
	void (*action)();
	char *value;
	char *text;
};

struct MenuItem *top_item;
struct MenuItem *bottom_item;
struct MenuItem *current_item;

void menu_init();

struct MenuItem *menu_add_item(char *text, struct MenuItem *parent, struct MenuItem *prev);
struct MenuItem *menu_add_dir(char *text, struct MenuItem *parent, struct MenuItem *prev);
struct MenuItem *menu_add_action(char *text, struct MenuItem *parent, struct MenuItem *prev, void (*action)());
struct MenuItem *menu_add_config(char *text, struct MenuItem *parent, struct MenuItem *prev, char *value);

struct MenuItem *_menu_init_item(char *text, struct MenuItem *parent, struct MenuItem *prev,
							     void (*action)(), char *value);
							   
void _format_item_text(struct MenuItem *item);
void _format_dir_text(struct MenuItem *item);
void _format_action_text(struct MenuItem *item);

void menu_finalize();

struct MenuItem *_menu_item_malloc();