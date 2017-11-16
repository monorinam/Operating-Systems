#include <stdio.h>
#include <stdlib.h>
#include "list.h"
int main(void)
{
	int num;
	char *line;
	int flag = 1;
	newList();

	while(flag)
	{
		printf("Enter a positive number (greater than zero\n");
		if(fgets(line, 10000,stdin) != NULL)
		{
			num = atoi(line);
			//for a non-numeric value, atoi returns 0
			if(num <= 0)
			{
				printf("The number entered is not positive, printing all positive numbers entered \n");
				flag = 0;
			}
			else
				addNode(num);
		}// end if valid line input
	}// end while loop
	//print all the elements of the list
	prettyPrint();
}// end main loop
