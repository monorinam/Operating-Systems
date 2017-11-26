#ifndef LINKEDLIST_DEF //if not defined to protect multiple definitions
#define LINKEDLIST_DEF
// define linked list struct
typedef struct linkedlist {
	int value;
	struct linkedlist * next;
}linkedlist;
//extern functions
extern void newList(void);
extern void addNode(int);
extern void prettyPrint(void);
#endif