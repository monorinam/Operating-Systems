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
    //char readval = fgetc(data);
    do//as long as char read is not null
    {
        readval = fgetc(data);
       
        //check for turn continuously
        do{
            fileTurn = fopen("TURN.txt", "r+");
            if(fileTurn == NULL)
            {
                printf("Error opening shared file TURN.txt \n");
                return FAILURE;
            }
            turnval = fgetc(fileTurn);
            fclose(fileTurn);
           

        }while(turnval != '0');
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
          return FAILURE;
      }
     fputc('1',fileTurn);
     fclose(fileTurn);

    }while(readval != '\0' && readval != EOF);
    fclose(data);
    return SUCCESS;
}

