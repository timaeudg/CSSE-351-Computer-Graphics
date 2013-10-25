#ifndef __LIST_H
#define __LIST_H
#define _CRT_SECURE_NO_WARNINGS //windows nonsense

typedef struct
{
	int item_count;
	int current_max_size;
	char growable;

	void **items;
	char **names;	
} list;

void list_make(list *listo, int size, char growable);
int list_add_item(list *listo, void *item, char *name);

void* list_get_name(list *listo, char *name);
void* list_get_index(list *listo, int indx);
void* list_get_item(list *listo, void *item_to_find);

void list_delete_index(list *listo, int indx);
void list_delete_name(list *listo, char *name);
void list_delete_item(list *listo, void *item);
void list_delete_all(list *listo);

void list_swap_index(list *listo, int to, int from);
void list_sub_sort(list *listo, int start, int end, int (*compare) (void *o1, void *o2) );
void list_sort(list *listo, int (*compare) (void *o1, void *o2) );
int list_max(list *listo, int (*compare) (void *o1, void *o2) );
int list_min(list *listo, int (*compare) (void *o1, void *o2) );
int list_find(list *listo, char *name_to_find);

char* list_print_items(list *listo);
void list_print_list(list *listo);
void list_free(list *listo);

#endif
