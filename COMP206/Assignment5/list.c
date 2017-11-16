#include <stdio.h>
#include <stdlib.h>
#include "list.h"

//sets the head node to null
//global head pointer
linkedlist* head_node;
void newList(void)
{
	head_node = NULL;
}

void addNode(int val)
{
	linkedlist* temp_node = (linkedlist*)malloc(sizeof(linkedlist));
	temp_node->value = val;
	temp_node->next = head_node;
	head_node = temp_node;

}

void prettyPrint(void)
{
	linkedlist* temp_node_to_free = (linkedlist*)malloc(sizeof(linkedlist));
	linkedlist* temp_node = temp_node_to_free;
	temp_node = head_node;
	//traverse the list in reverse order from head (head points to most recent number)

	while(temp_node != NULL)
	{
		printf("%d \t",temp_node->value);
		temp_node = temp_node->next;
	}
	free(temp_node_to_free);
	printf("\n");
}
