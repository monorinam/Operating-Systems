#ifndef LINKEDLIST_DEF
#define LINKEDLIST_DEF
typedef struct linkedlist {
	int value;
	struct linkedlist * next;
}linkedlist;
extern linkedlist* head;
extern void newList(void);
extern void addNode(int);
extern void prettyPrint(void);
#endif