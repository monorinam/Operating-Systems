#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define MAX_LEN 1000 //max length of sentence
#define ERROR -1 //error code
#define SUCCESS 0
#define MAX_TRY 5
// This is a helper function 
// It replaces the string in orig with the string in new
// It assumes both strings are a max of MAX_LEN
void duplicate_str(char *orig, char *new)
{
  int i = 0;
  //Find the newline, add all characters before the newline to the new string
  // Add null after the newline
  while(i < MAX_LEN)
  {
    if(*(orig + i) != '\n')
      *(new + i) = *(orig + i);
    else
      *(new + i) = '\0';
    i = i + 1;
  }
}
// This is the find record function
// Inputs are filename, the name to be replaced, and the record array
// with the record where name will be replaced.
void FindRecord(char *filename, char *name, char record[])
{
    FILE *fileID;
    char *line = malloc(MAX_LEN*sizeof(char));
    int emptyflag = 0;

    //open file
    fileID = fopen(filename, "r");
    if (fileID == NULL)
      perror("The file could not be opened \n");
    else
    {
      //read file line by line and 
      //find the name to match with char *name
      while(fgets(line, MAX_LEN, fileID) != NULL)
      {
            //find the name in the record
            int i = 0;
            int compareflag = 1;
            // The name is before the first comma, so step through line
            // until the first comma
            while(*(line + i) != ',' && compareflag == 1)
            {
              //Still in name section of line
              // compare the two strings char by char
              if(*(line + i) != *(name + i) )
                compareflag = 0;
              i = i + 1;
            }

            if(compareflag == 1)
            {
              //strings are equal
              //duplicate the string
              emptyflag = 1;
              duplicate_str(line, record);
            }// end of strcopy if
      } //end of eof while
    } //end of file opened if
    // If the line is not found record is just nulls
    if(emptyflag == 0)
    {
      for(int i = 0; i < MAX_LEN; i ++)
      {
        *(record + i) = '\0';
      }
    }
} //end of function
// This is the function that replacenes name in record with newname
// This function assumes that the record contains the line with the name
void Replace(char *name, char *newname, char record[])
{

    //replace the name with the new name
    // Stores the old record
    char *temp_record = malloc(MAX_LEN*sizeof(char));
    //First duplicate the temporary record
    duplicate_str(record,temp_record);
    //check the names are same: TODO

    int i = 0;
    // replace the name in record
    while(*(newname + i) != '\n')
    {
      //add name to record
      *(record + i) = *(newname + i);
      i = i + 1;
    }
    // now shift the rest of the line after the name
    int j = 0;
    while(*(temp_record + j) != ',')
    {
      //accelerate past the first name
      j = j + 1;
    }
    //now replace the rest from old line to new line
    while(*(temp_record + j) != '\0')
    {
      *(record + i) = *(temp_record + j);
      i = i + 1;
      j = j + 1;
    }
    //terminate with newline
    *(record + i) = '\n';
    //add null to other elements
    for(i = i + 1;i < MAX_LEN;i++)
      *(record + i) = '\0';
    
}
// This function writes the record to the file
void SaveRecord(char *filename, char *name, char record[])
{

    FILE *fileID = fopen(filename, "r");
    char *line = malloc(MAX_LEN*sizeof(char));
    int fileLength;
    int temp;
    // find the file length using fseek to go to end of file
    // and back again
    // to load the file into buffer
    fseek(fileID, 0, SEEK_END);
    fileLength = ftell(fileID);
    fseek(fileID, 0, SEEK_SET);
    // this buffer stores all data before the line to be replaced
    char *fileBufferBefore = malloc(fileLength*sizeof(char));
    // this buffer stores all data after the line to be replaced
    char *fileBufferAfter = malloc(fileLength*sizeof(char));
    // this is the flag that shows that the given line is found in
    // the file and the rest of the lines are stored in the fileBufferAfter buffer
    int switch_buffers = 0;

    //initialize the buffer befores and afters to null
    *fileBufferBefore = '\0';
    *fileBufferAfter = '\0';
    // error check that file can be opened.
    if (fileID == NULL)
      perror("The file could not be opened \n");
    else
    {
      int j = 0;
      // go through each line of the file
      while(fgets(line, MAX_LEN - 1, fileID) != NULL)
      {
        int equal_flag = 0;
        int i = 0;
        // step through the line until the first comma
        // name is before the first comma
        while(*(line + i) != ',' && equal_flag == 0)
        {
            // check if the names are identical character by character
            if(*(line + i) != *(name + i))
              equal_flag = 1;
            i = i + 1;
        } //end of while
        if(equal_flag == 0)
        {
          // the names are equal for this line and the record
          // so set the flag and reset the pointer counter
          switch_buffers = 1;
          j = 0;
        }
        if(!switch_buffers)
        {
          // before the line is found
          // add to before buffer
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
          // if the line has already been found and is not the current line
          // (skips the current line), add to after buffer
          temp = j;
          *(fileBufferAfter + j) = *(line);
          while(*(fileBufferAfter + j) != '\n')
          {
            j = j + 1;
            *(fileBufferAfter + j) = *(line + j - temp);
          }
          j = j + 1;
        }
      }
      //Rewrite file
      //Reopen file for writing
      fileID = fopen(filename, "w");
      // write the section before the line
      fputs(fileBufferBefore, fileID);
      // write the line
      fputs(record, fileID);
      // write the section after the line
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
    if(*record != '\0')
    {
      Replace(name, replacement_name, record);
      SaveRecord(filename, name, record);

    }

}
