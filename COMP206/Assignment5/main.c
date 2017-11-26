#include <stdio.h>
#include <stdlib.h>
#include "list.h"
int main(void)
{
	int num;
	char *line = malloc(10000*sizeof(char));
	//flag for loop termination
	int flag = 1;
	//create a new linked list
	newList();

	while(flag)
	{
		printf("Enter a positive number (greater than zero) \n");
		//read the number using fgets to avoid garbage collection with scanf
		if(fgets(line, 10000,stdin) != NULL)
		{
			//convert number to integer
			num = atoi(line);
			//for a non-numeric value, atoi returns 0
			if(num <= 0)
			{
				printf("The number entered is not positive, printing all positive numbers entered \n");
				flag = 0;
			}
			else
				addNode(num); //add to linked list
		}// end if valid line input
	}// end while loop
	//print all the elements of the list
	prettyPrint();
	free(line);
}// end main loop
