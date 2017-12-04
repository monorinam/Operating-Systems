#include "main.h"
int producer()
{
    FILE* fileTurn;
    FILE* fileWrite;
    FILE* data = fopen("mydata.txt", "r");
    char readval = fgetc(data);
    while((readval = fgetc(data)) != EOF)//as long as char read is not null
    {
        //check for turn continuously
        while(1){
            fileTurn = fopen("TURN.txt", "r+");
            if(fileTurn == NULL)
            {
                printf("Error opening shared file TURN.txt \n");
                return FAILURE;
            }
            char turnval = fgetc(fileTurn);
            fclose(fileTurn);
            if(turnval == '0')
                break;

        }
        //now open the DATA.txt file and write to it
        // because we have the turn permission
       if( (fileWrite = fopen("DATA.txt","w")) == NULL)
       {
           printf("Error opening file DATA.txt\n");
           return FAILURE;
       }
       fputc(readval, fileWrite);
       fclose(fileWrite);
       //give the turn to the consumer to write
      if((fileTurn = fopen("Turn.txt","w")) == NULL)
      {
          printf("Error opening TURN.txt to write \n");
          return FAILURE;
      }
     fputc('1',fileTurn);
     fclose(fileTurn);

    }
    fclose(data);
}

