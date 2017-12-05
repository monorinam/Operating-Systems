//Monorina M (260354335)
#include<stdlib.h>
#include<stdio.h>
#include<unistd.h>
#include "main.h"

int main(void)
{
    FILE* fileID = fopen("TURN.txt", "w+");
    if(fileID == NULL)
    {
        printf("Error opening shared file TURN.txt\n");
        return -1;
    }
   fputc(TURNP,fileID); //give turn to producer
   fclose(fileID);

    int pid = fork();
    if(pid < 0)
    {
        printf("Could not fork process, terminating.. \n");
        exit(1);
    }
   if(!pid) 
   {
      //this is the child
       int status = producer();
       if(status == TURNFAIL)
          printf("ALERT: Please press Control + C to abort, the consumer will hang since TURN.txt cannot be written \n");
      
   }
   else{
       consumer();
       waitpid(pid, NULL, 0); //wait for child to finish before next iteration
   }
   return SUCCESS;
}