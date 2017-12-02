#ifndef _INCLUDE_SFS_API_H_
#define _INCLUDE_SFS_API_H_

#include <stdint.h>
#define SUCCESS 1
#define MAX_FILE_NAME 20
#define MAX_EXTENSION_NAME 3
#define SFS_NAME "MonorinaMSFS"
#define BLOCK_SIZE 1024
#define NUM_BLOCKS 1024
#define IN_USE 1
#define NOT_INUSE 0
#define NUM_INODE_BLOCKS 32 //randomly picked
#define SB_MAGIC 0xACBD0005
#define INITNULL 0
#define DIR_BLOCK_START 1 + NUM_INODE_BLOCKS //starts at 33
#define INIT_INODE_VAL -1
#define FAILURE -1
#define FILE_ERR1 -1 //if new file cannot be created
#define FILE_ERR2 -2 //if existing file cannot be opened
#define FILE_ERR3 -3
//#define NUM_BITMAP_BLOCKS 16 //randomly picked
//#define BITMAP_BLOCK_START NUM_BLOCKS - NUM_BITMAP_BLOCKS  
#define NUM_INODES 256 
#define NUM_FILES NUM_INODES//NUM_INODES < NUM_BLOCKS ? NUM_INODES : NUM_BLOCKS //minimum of number of blocks or number of inodes
typedef struct superblock_t{
    uint64_t magic;
    uint64_t block_size;
    uint64_t fs_size;
    uint64_t inode_table_len;
    uint64_t root_dir_inode;
} superblock_t;

typedef struct inode_t {
    unsigned int mode;
    unsigned int link_cnt;
    unsigned int uid;
    unsigned int gid;
    unsigned int size;
    int inuse; //added
    unsigned int data_ptrs[12];
    unsigned int indirectPointer; // points to a data block that points to other data blocks (Single indirect)
} inode_t;

/*
 * inodeIndex    which inode this entry describes
 * inode  pointer towards the inode in the inode table
 *rwptr    where in the file to start   
 */
typedef struct file_descriptor {
    int64_t inodeIndex;
    inode_t* inode; // 
    uint64_t rwptr;
} file_descriptor;


typedef struct directory_entry{
    int num; // represents the inode number of the entery. 
    char name[1+MAX_FILE_NAME]; // represents the name of the entery. 
}directory_entry;

typedef struct file_descriptor_table{
    file_descriptor fildes[NUM_FILES];
    int use[NUM_FILES]; // IN_USE or NOT_INUSE
}file_descriptor_table;



void mksfs(int fresh);
int sfs_getnextfilename(char *fname);
int sfs_getfilesize(const char* path);
int sfs_fopen(char *name);
int sfs_fclose(int fileID);
int sfs_fread(int fileID, char *buf, int length);
int sfs_fwrite(int fileID, const char *buf, int length);
int sfs_fseek(int fileID, int loc);
int sfs_remove(char *file);

#endif //_INCLUDE_SFS_API_H_
