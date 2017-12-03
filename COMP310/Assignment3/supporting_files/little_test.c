
/* sfs_test.c 
 * 
 * 
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "sfs_api.h"

int main() {
    mksfs(1);
    int f = sfs_fopen("some_name_other.txt");
   // char my_data[] ="The quick brown fox jumps over the lazy dog Poor old Lucy lost her shoe";
  //  char my_data2[]="I forgot the rest of this stupid poem Potatoes";
    char mydata[42905];
    for(int i = 0; i < 42905; i ++)
    {
        mydata[i] = (char) i;
    }

   // char out_data[1024];
    //sfs_fwrite(f, my_data, sizeof(my_data));
    //sfs_fseek(f, 0);
   // sfs_fread(f, out_data, sizeof(out_data)+1);
    //sfs_fwrite(f, my_data2, sizeof(my_data2));
    //sfs_fseek(f, 0); 
   // printf("%s\n", out_data);
    //sfs_fread(f, out_data, sizeof(out_data)+1);
   // printf("%s\n", out_data + sizeof(my_data));
    sfs_fwrite(f,mydata,4095);
    sfs_fwrite(f,mydata+4095,sizeof(mydata)-4095);
    char out_data2[42905];
    char out1[42905];
    for(int k = 0;k<2;k++)
    {
        sfs_fseek(f, 0);
        sfs_fread(f, out1, 42905);
        for(int i=0; i< 42905; i++)
        {
            if(mydata[i]!=out1[i])
            {
                printf("My data is %d and outdata is %d \n",mydata[i],out1[i]);
            }
        }
    }
    printf("first test finished\n");
    sfs_fseek(f, 0);
    for (int j = 0; j < 41; j++) {
        int offset = j * 1024;
        sfs_fread(f, out_data2 + offset, 1024);
    }
    for(int i=0; i< 41*1024; i++)
    {
        if(mydata[i]!=out_data2[i])
        {
            printf("%d, My data is %d and outdata is %d \n",i, mydata[i],out_data2[i]);
        }
    }

    sfs_fclose(f);
}
