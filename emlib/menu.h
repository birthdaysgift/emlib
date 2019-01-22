struct MenuItem {
	struct MenuItem *parent;
	struct MenuItem *prev;
	struct MenuItem *next;
	struct MenuItem *first_child;
	void (*action)();
	char *text;
};

struct MenuItem *top_item;
struct MenuItem *bottom_item;
struct MenuItem *current_item;

void menu_init();

struct MenuItem *menu_add_item(char *text, struct MenuItem *parent, struct MenuItem *prev);
struct MenuItem *menu_add_dir(char *text, struct MenuItem *parent, struct MenuItem *prev);
struct MenuItem *menu_add_action(char *text, struct MenuItem *parent, struct MenuItem *prev, void (*action)());

void menu_finalize();

void menu_next();
void menu_prev();
void menu_enter();
void menu_escape();

int _menu_has_next();
int _menu_has_prev();
int _menu_has_parent();
int _menu_has_child();
int _menu_has_action();

char *_get_dir_text(char *);
char *_get_item_text(char *);
char *_get_action_text(char *);

struct MenuItem *_menu_item_malloc();