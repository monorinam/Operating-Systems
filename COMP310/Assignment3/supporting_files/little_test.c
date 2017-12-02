
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
    int f = sfs_fopen("some_name.txt");    
    char my_data[] ="The quick brown fox jumps over the lazy dog Poor old Lucy lost her shoe";
    char my_data2[]="I forgot the rest of this stupid poem Potatoes";
    char out_data[1024];
    sfs_fwrite(f, my_data, sizeof(my_data));
    sfs_fseek(f, 0);
    sfs_fread(f, out_data, sizeof(out_data)+1);
    sfs_fwrite(f, my_data2, sizeof(my_data2));
    sfs_fseek(f, 0); 
    printf("%s\n", out_data);
    sfs_fread(f, out_data, sizeof(out_data)+1);
    printf("%s\n", out_data + sizeof(my_data));

    sfs_fclose(f);
}
