// Consumer.c from Ruth Silcoff
// Comment added by Monorina
#include <stdlib.h>
#include <stdio.h>
#include "main.h"

int consumer(void)
{
	FILE* turn;
	FILE* read;
	char turnvalue;
	char c = ' ';
	//runs until end of file
	while(c != EOF)
	{
		//check for turn
		do
		{
			turn = fopen("TURN.txt","r");
			if(turn == NULL)
			{
				printf("Error opening TURN.txt in consumer \n");
			}
			turnvalue = fgetc(turn);
			fclose(turn);
		}while(turnvalue != '1');
		//open file and read data
		read = fopen("DATA.txt", "r");
		if(read == NULL)
		{
			printf("Error opening DATA.txt in consumer \n");
			return FAILURE;
		}
		else
		{
			c = fgetc(read);
			fclose(read);
			//print character to screen
			if(c != EOF)
			{
				printf("%c",c);
			}
		}
		turn = fopen("TURN.txt","w");
		if(turn == NULL)
		{
			printf("Error opening TURN.txt in consumer");
			return FAILURE;
		}
		fputc('0',turn);
		fclose(turn);
	}
	return SUCCESS;
}
