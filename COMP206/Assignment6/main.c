#include<stdlib.h>
#include<stdio.h>
#include<unistd.h>
#include "main.h"

int main()
{
    FILE* fileID = fopen("TURN.txt", "w+");
    if(fileID == NULL)
    {
        printf("Error opening shared file TURN.txt\n");
        return -1;
    }
   fputc('0',fileID);
   fclose(fileID);

    int pid = fork();
    if(pid == -1)
    {
        printf("Could not fork process, terminating.. \n");
        exit(1);
    }
   if(pid != 0)
   {
       producer();
       waitpid(pid, NULL, 0); //wait for child to finish
   }
   else{
       consumer();
   }
   return SUCCESS;
    
