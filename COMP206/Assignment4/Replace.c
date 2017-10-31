#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define MAX_LEN 1000 //max length of sentence
#define ERROR -1 //error code
#define SUCCESS 0
#define MAX_TRY 5
void duplicate_str(char *orig, char *new)
{
  int i = 0;
  while(i < MAX_LEN)
  {
    if(*(orig + i) != '\n')
      *(new + i) = *(orig + i);
    else
      *(new + i) = '\0';
    i = i + 1;
  }
}
void FindRecord(char *filename, char *name, char record[])
{
    //find length of file
    FILE *fileID;
    int eof_flag = 0;
    char *line = NULL;
    size_t linecap = 0;
    ssize_t nread;

    //open file
    fileID = fopen(filename, "r");
    if (fileID == NULL)
      perror("The file could not be opened \n");
    else
    {
      //read file line by line and 
      //find the name to match with char *name
      while(eof_flag == 0)
      {
          nread = getline(&line, &linecap, fileID);
          if(nread <= 0)
            eof_flag = 1; //reached end of file
          else
          {
            //find the name in the record
            int i = 0;
            int compareflag = 1;
            while(*(line + i) != ',' && compareflag == 1)
            {
              //Still in name section of line
              if(*(line + i) != *(name + i) )
                compareflag = 0;
              i = i + 1;
            }

            if(compareflag == 1)
            {
              //strings are equal
              //duplicate the string
              duplicate_str(line, record);
            }// end of strcopy if

          }// end of valid line if
      } //end of eof while
    } //end of file opened if
} //end of function

void Replace(char *name, char *newname, char record[])
{

    //replace the name with the new name
    char *temp_record = malloc(MAX_LEN*sizeof(char));
    //First duplicate the temporary record
    duplicate_str(record,temp_record);
    //check the names are same: TODO

    int i = 0;
    while(*(newname + i) != '\n')
    {
      //add name to record
      *(record + i) = *(newname + i);
      i = i + 1;
    }
    int j = 0;
    while(*(temp_record + j) != ',')
    {
      //accelerate past the first name
      j = j + 1;
    }
    while(*(temp_record + j) != '\0')
    {
      *(record + i) = *(temp_record + j);
      i = i + 1;
      j = j + 1;
    }
    //*(record + i) = '\n';
    
}
void SaveRecord(char *filename, char *name, char record[])
{

    FILE *fileID = fopen(filename, "r");
    char *line = malloc(MAX_LEN*sizeof(char));
    int fileLength;
    int temp;
    fseek(fileID, 0, SEEK_END);
    fileLength = ftell(fileID);
    fseek(fileID, 0, SEEK_SET);
    char *fileBufferBefore = malloc(fileLength*sizeof(char));
    char *fileBufferAfter = malloc(fileLength*sizeof(char));
    int switch_buffers = 0;
    if (fileID == NULL)
      perror("The file could not be opened \n");
    else
    {
      //if the line matches
      //replace with new record
      int j = 0;
      while(fgets(line, MAX_LEN - 1, fileID) != NULL)
      {
        int equal_flag = 0;
        int i = 0;
        while(*(line + i) != ',' && equal_flag == 0)
        {
            if(*(line + i) != *(name + i))
              equal_flag = 1;
            i = i + 1;
        } //end of while
        if(equal_flag == 0)
        {
          switch_buffers = 1;
          j = 1;
          *fileBufferAfter='\n';
        }
        if(!switch_buffers)
        {
          *(fileBufferBefore + j) = *line;
          temp = j;
          while(*(fileBufferBefore + j) != '\n')
          {
            j = j + 1;
            *(fileBufferBefore + j) = *(line + j - temp);
          }
          j = j + 1;
        }
        else if (equal_flag == 1)
        {
          temp = j;
          *(fileBufferAfter + j) = *(line);
          while(*(fileBufferAfter + j) != '\n')
          {
            j = j + 1;
            *(fileBufferAfter + j) = *(line + j - temp);
          }
          j = j + 1;
        }
        //j = j + 1;
      }
      //Rewrite file
      //Reopen file for writing
      fileID = fopen(filename, "w");
      fputs(fileBufferBefore, fileID);
      fputs(record, fileID);
      //fputs('\n', fileID);
      fputs(fileBufferAfter, fileID);
    }
}
int main(void)
{
    char name[MAX_LEN];
    char replacement_name[MAX_LEN];
    char *filename;// = NULL;
    char record[MAX_LEN];
    int trial;

    // Get the sentence
    trial = 0;
    printf("Enter a name\n");
    while(fgets(name, MAX_LEN, stdin) == NULL && trial <= MAX_TRY)
    {
       printf("Error in getting the name, please try again \n");
       trial = trial + 1;
    }

    // Now get replacement name
    trial = 0;
    printf("Enter a replacement name\n");
    while(fgets(replacement_name, MAX_LEN, stdin) == NULL && trial <= MAX_TRY)
    {
       printf("Error in getting the name, please try again \n");
       trial = trial + 1;
    }
    // Find the record
    filename = "phonebook.csv";
    FindRecord(filename, name, record);
    printf("Record %s \n",record);
    Replace(name, replacement_name, record);
    printf("Record %s \n", record);
    SaveRecord(filename, name, record);

}
