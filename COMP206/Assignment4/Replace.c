#include <stdio.h>
#include <stdlib.h>
#define MAX_LEN 1000 //max length of sentence
#define ERROR -1 //error code
#define SUCCESS 0
#define MAX_TRY 5
void duplicate_str(char *orig, char *new)
{
  int i = 0;
  while(i < MAX_LEN)
  {
    if(orig[i] != '\n')
      new[i] = orig[i];
    else
      new[i] = '\0';
    i = i + 1;
  }
}
void FindRecord(char *filename, char *name, char record[])
{
    //find length of file
    int fileLength;
    FILE *fileID;
    int eof_flag = 0;
    char *line = NULL;
    size_t linecap = 0;
    ssize_t nread;

    //open file
    printf("The name of the file is %s \n",filename);
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
            while(line[i] != ',' && compareflag == 1)
            {
              //Still in name section of line
              if(line[i] != name[i] )
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
    char temp_record[MAX_LEN];
    //First duplicate the temporary record
    duplicate_str(record,temp_record);
    //check the names are same

    int i = 0;
    while(temp_record != ',')
    {
      //this part contains the name, before the first comma
      record[i] = newname[i];

    }

    
}
void SaveRecord(char *filename, char *name, char record[])
{
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

    // Get name of file
    trial = 0;
    printf("Enter the name of the file \n");
    while(fgets(filename, MAX_LEN, stdin) == NULL && trial <= MAX_TRY)
    {
       printf("Error in getting the name, please try again \n");
       trial = trial + 1;
    }
    // Find the record
    filename = "phonebook.csv";
    FindRecord(filename, name, record);
    printf("Record %s \n",record);
    Replace(name, replacement_name, record);
    SaveRecord(filename, replacement_name, record);

}
