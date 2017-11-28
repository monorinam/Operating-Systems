
#include "sfs_api.h"
#include "bitmap.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fuse.h>
#include <strings.h>
#include "disk_emu.h"
#define LASTNAME_FIRSTNAME_DISK "mukhopadhyay_monorina.disk"
//#define NUM_BLOCKS 1024  //maximum number of data blocks on the disk.
#define BITMAP_ROW_SIZE (NUM_BLOCKS/8) // this essentially mimcs the number of rows we have in the bitmap. we will have 128 rows. 

/* macros */
#define FREE_BIT(_data, _which_bit) \
    _data = _data | (1 << _which_bit)

#define USE_BIT(_data, _which_bit) \
    _data = _data & ~(1 << _which_bit)
#define IS_FREE(_data, _which_bit) _data & (1 << _which_bit)

//initialize all bits to high
uint8_t free_bit_map[BITMAP_ROW_SIZE] = { [0 ... BITMAP_ROW_SIZE - 1] = UINT8_MAX };
//global variables
superblock_t super_block;
file_descriptor_table fildest;
inode_t inode_array[NUM_INODES];
directory_entry root[NUM_FILES]; //the only directory in the SFS
int bitmap_start_free;//this stores the first block in the bitmap that is free
int root_position; //this keeps track of the position in sfs_getnextfilename()
///Helper functions
int fill_block()
{
	//find the first free block
	int first_free = 129;
	for(int i=0;i<BITMAP_ROW_SIZE;i++)
	{
		for(int j = 0; j < 8; j++)
		{
			if(IS_FREE(BITMAP_ROW_SIZE[i],j))
			{
				first_free = i*8 + j;
				//fill the block
				USE_BIT(free_bit_map[i],j);
			}
		}
	}
	if(first_free == 129)
		return -1; //failure
	else
		return first_free; //success

}

//Check if a file name already exists
int check_file_exists(char* fname)
{
	for(int i = 0; i < NUM_FILES; i++)
	{
		if(strcmp(fname,root[i].name) == 0)
			return i;
	}
	return FAILURE;
}
void mksfs(int fresh) {
	root_position = 0;
	// Declare and malloc
	fildest = malloc(sizeof(file_descriptor_table));
	//initialize the file descriptor table (to zero it out and init all values
	for(int i = 0; i < NUM_FILES; i++)
	{
		fildest.fildes[i].inodeIndex = INIT_INODE_VAL;
		fildes.fildes[i].inode = NULL;
		fildest.fildes[i].rwptr = INIT_INODE_VAL;
		fildest.inuse[i] = NOT_INUSE
	}
	//file_descriptor fildes;

	if(fresh)
	{
		inode_t zero_node = malloc(sizeof(inode_t));
		int root_blocks = sizeof(root)/BLOCK_SIZE + 1;
		//remove the filesystem if it exists
		remove(SFS_NAME);
		//then initialize it again
		init_fresh_disk(SFS_NAME, BLOCK_SIZE, NUM_BLOCKS);

		//write super block
		//initialize super block first
		super_block.magic = SB_MAGIC;
		super_block.block_size = BLOCK_SIZE;
		super_block.fs_size = NUM_BLOCKS*BLOCK_SIZE; //the file system size
		super_block.inode_table_len = NUM_INODES;
		super_block.root_dir_inode = 0;
		if(fill_block() > -1)
			write_block(0,1,&super_block);
		else
			exit(0);
		// initialize inodes
		for(int i = 0; i < NUM_INODES; i++)
		{
			if(i==0)
			{
				//the first inode has info about root directory
				//inode_t zero_node = malloc(sizeof(inode_t));
				zero_node.size = 0;
				zero_node.mode = x0755;
				zero_node.link_cnt = 1;
				zero_node.uid = 0;
				zero_node.gid = 0;
				//int max_root_blocks = 12 < sizeof(root)/BLOCK_SIZE 
				//fill the blocks for the direct pointers of the root
				for(int i = 0; i < root_blocks ; i++)
				{
					zero_node.data_ptrs[i] = fill_block();
					
				}
				inode_array[0] = zero_node;
				//memset(&zero_node,INITNULL,sizeof(zero_node));

			}
			else
			{
				//inode_array[i] = malloc(sizeof(inode_t));
				inode_array[i].inuse = NOT_INUSE
			}


		}
		// write inodes to directory (inodes start at 1s)
		for(int i = 0; i < NUM_INODE_BLOCKS; i ++)
			fill_block();
		write_block(1,NUM_INODE_BLOCKS,&inode_array);
		
		//save the bitmap
		//initialize data bitmap
		//Already done at declare time
		//save the bitmap to last block of the file system
		write_block(NUM_BLOCKS-1, 1, &free_bit_map);
		//initialize root, 
		for(int i = 0; i < NUM_FILES; i ++)
		{
			root[i].num = INIT_INODE_VAL;
		}
		//write root
		write(zero_node.data_ptrs[0],root_blocks,&root);
		

	}
	else{
		if(init_disk.(SFS_NAME, BLOCK_SIZE, NUM_BLOCKS) < 0)
			exit(0);//Failure
		//read the superblock
		read_blocks(0,1,&super_block);
		// read the inode table
		read_blocks(1,NUM_INODE_BLOCKS,&inode_array);
		//read the bitmap
		read_blocks(NUM_BLOCKS-1,1, &free_bit_map);
		//read the root directory
		read_blocks(inode_array[0].data_ptrs[0],1 + sizeof(root)/BLOCK_SIZE),&root)



	}
	
}
int sfs_getnextfilename(char *fname){
	while (root_position > = 0 && root_position < NUM_FILES)
	{
		if(root[root_position].num > = 0) //the file exists
		{
			strcpy(fname, root[i].name); //copy the file 
			root_position++;
			if(root_position == NUM_FILES) //last file in directory
				return 0; //so return zero
			return 1; //else non zero return
		}
	}
}
int sfs_getfilesize(const char* path){
	int filenum;
	if(filenum = check_file_exists(path) >= 0)
	{
		//file exists
		//get size
		return inode_array[root[filenum].num].size;
	}
	else
		return FAILURE;

}
int search_file_dir_table(inode_num)
{
	for(int i = 0; i < NUM_FILES; i++)
	{
		if(fildest.fildes[i].inodeIndex == inode_num)
			return i;

	}
	return FAILURE;
}
//finds and creates an inode in the inode table
int find_free_inode()
{
	//first search for the free inode in the table
	int index = -1;
	for(int i = 0; i < NUM_INODES; i++)
	{
		if(inode_array[i].inuse == NOT_INUSE)
		{
			index = i;
			break;
		}

	}
	if(index == -1)
		return FAILURE;
	else
	{
		//get a new inode
		//inode_t node = inode_array[index];
		//mark as used
		inode_array[index].inuse = IN_USE;
		inode_array[index].size = 0; //empty for now
		write_blocks(1,NUM_INODE_BLOCKS,&inode_array);
		return index;
		// and save the inode table
	}

}
int sfs_fopen(char *name){
	int filenum = check_file_exists(name);
	int inode_num;
	if(filenum > = 0)
	{
		//file already exists
		//open and append
		//the inode # is root[filenum].num
		inode_num = root[filenum].num;
		int filedestable_index = search_file_dir_table(inode_num);
		if(filedestable_index != FAILURE)
		{
			//found file; open 
			return filedestable_index;
		}
		//then check if it is in the file directory
	}
	// this region is catch all for either if the inode was not found in the fildestable 
	// or if the file did not exist

	//file does not exist, assign new inode to file, if inode cannot be assigned errno 1 
	else{
		int inode_num = find_free_inode();
		if(inode_num < 0)
			return FILE_ERR1;
	}
	// create new file descriptor here
	//find the first free file descriptor
	int fd_index = -1;
	for(int i = 0; i < NUM_FILES; i++)
	{
		if(fildest.use[i] == NOT_INUSE)
			fd_index = i;
	}
	if(fd_index == -1)
		return FILE_ERR2;
	return fd_index;


	//if filenum >= 0 and new file descriptor could not be created error code 2
	//if filenum <=0 and new file descriptor could not be created error code 1


}
int sfs_fclose(int fileID) {

}
int sfs_fread(int fileID, char *buf, int length) {
	
}
int sfs_fwrite(int fileID, const char *buf, int length) {

}
int sfs_fseek(int fileID, int loc) {

}
int sfs_remove(char *file) {


}



