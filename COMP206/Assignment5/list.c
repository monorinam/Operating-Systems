#include <stdio.h>
#include <stdlib.h>
#include "list.h"

//global head pointer
// private, cannot be externed
static linkedlist* head_node;
//sets head to null
void newList(void)
{
	head_node = NULL;
}
// Adds a node
void addNode(int val)
{
	//creates a temporary node
	linkedlist* temp_node = (linkedlist*)malloc(sizeof(linkedlist));
	//assigns value to the temporary node
	temp_node->value = val;
	// this is now the head node (since adding to end of the list)
	temp_node->next = head_node;
	head_node = temp_node;

}

void prettyPrint(void)
{
	//To prevent memory leaks (create the original malloc and save it, because at the end
	// of the loop, temp node will point to null and cannot be freed)
	linkedlist* temp_node_to_free = (linkedlist*)malloc(sizeof(linkedlist));
	linkedlist* temp_node = temp_node_to_free;
	temp_node = head_node;
	//traverse the list in reverse order from head (head points to most recent number)
	// until end is reached
	while(temp_node != NULL)
	{
		//print the value and increment
		printf("%d \t",temp_node->value);
		temp_node = temp_node->next;
	}
	//free the temporary node
	free(temp_node_to_free);
	printf("\n");
}
