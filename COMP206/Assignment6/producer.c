#include "main.h"
int producer(void)
{
    FILE* fileTurn;
    FILE* fileWrite;
    FILE* data;
    if((data = fopen("mydata.txt", "r")) == NULL)
    {
      printf("Error in opening mydata.txt\n");
      return FAILURE;
    }

    char readval, turnval;
    do//as long as char read is not null
    {
        readval = fgetc(data);
       
        //check for turn continuously, and wait until done
        do{
            if((fileTurn = fopen("TURN.txt", "r")) == NULL)
            {
                printf("Error opening shared file TURN.txt \n");
                fclose(data);
                return FAILURE;
            }
            turnval = fgetc(fileTurn);
            fclose(fileTurn);
           

        }while(turnval != TURNP);
        //now open the DATA.txt file and write to it
        // because we have the turn permission
       if( (fileWrite = fopen("DATA.txt","w")) == NULL)
       {
           printf("Error opening file DATA.txt\n");
       }
       else{
            fputc(readval, fileWrite);
            fclose(fileWrite);
        }
       //give the turn to the consumer to write
      if((fileTurn = fopen("TURN.txt","w")) == NULL)
      {
          printf("Error opening TURN.txt to write \n");
          fclose(data);
          return TURNFAIL;
          //exit(EXIT_FAILURE);
      }
     fprintf(fileTurn,"%c \n",TURNC);
     fclose(fileTurn);

    }while(readval != EOF);
    fclose(data);
    return SUCCESS;
}

