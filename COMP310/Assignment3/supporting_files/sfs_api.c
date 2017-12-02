#include "sfs_api.h"
//#include "bitmap.h"
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
int inode_blocks;
///Helper functions
int fill_block()
{
	//find the first free block
	int first_free = -1;
	for(int i=0;i<BITMAP_ROW_SIZE;i++)
	{
		for(int j = 0; j < 8; j++)
		{
			if(IS_FREE(free_bit_map[i],j))
			{
				first_free = i*8 + j;
				//fill the block
				USE_BIT(free_bit_map[i],j);
                break;
			}
		}
        if(first_free >= 0)
            break;
	}
	if(first_free == -1)
		return -1; //failure
	else
		return first_free; //success

}
//this function empties out blocks
// (when file is either deleted or blocks that were
// assigned and then not needed)
void empty_block(int block_num)
{
	//find this block in the free bitmap
	//then mark the block as free
	int i = block_num/8;
	int j = block_num - 8*i;
	FREE_BIT(free_bit_map[i],j);

}

//Check if a file name already exists
//if so, find it in the inode array and the file descriptor table
// return inode_index from the pointer passed, and the return value is the file 
//descriptor table
int check_file_exists(char* fname, int *filenum, int *inode_num)
{
	*filenum = -1;
	*inode_num = -1;//initial default values
	for(int i = 0; i < NUM_FILES; i++)
	{
		//char *temp = root[i].name; //needs to be assigned to a pointer for strcmp to work
		//strncpy(temp,fname,strlen(fname));
		if(strcmp(fname,root[i].name) == 0)
        {	*filenum = i;
            break;}
	}
	if(*filenum == -1)
		return FAILURE; //not found
	//now search inode
	*inode_num = root[*filenum].num;
	for(int i = 0; i < NUM_FILES; i++)
	{
		if(fildest.fildes[i].inodeIndex == *inode_num)
			return i;

	}
	return FAILURE;
}

int check_file_id(int fileID)
{
	if(fileID >= 0 && fileID < NUM_FILES)
	{
		if (fildest.fildes[fileID].inodeIndex >= 0)
		{
			if(fildest.use[fileID] == IN_USE)
				return SUCCESS;
			else
				return FILE_ERR3;
		}
		else 
			return FILE_ERR2;
	}
	else
		return FILE_ERR1;
}

void mksfs(int fresh) {
	root_position = 0;
	// Declare and malloc
	//fildest = (file_descriptor_table*)malloc(sizeof(file_descriptor_table));
	//initialize the file descriptor table (to zero it out and init all values
	for(int i = 0; i < NUM_FILES; i++)
	{
		fildest.fildes[i].inodeIndex = INIT_INODE_VAL;
		fildest.fildes[i].inode = NULL;
		fildest.fildes[i].rwptr = INIT_INODE_VAL;
		fildest.use[i] = NOT_INUSE;
	}
	//file_descriptor fildes;

	if(fresh)
	{
		inode_t zero_node;// = (inode_t*)malloc(sizeof(inode_t));
		int root_blocks = sizeof(root)/BLOCK_SIZE + 1;
        inode_blocks = sizeof(inode_array)/BLOCK_SIZE + 1;
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
		if(fill_block() > -1)//should always be zero
			write_blocks(0,1,&super_block);
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
				zero_node.mode = 755;
				zero_node.link_cnt = 1;
				zero_node.uid = 0;
				zero_node.gid = 0;
				zero_node.indirectPointer = -1;
                zero_node.inuse = IN_USE;
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
				inode_array[i].inuse = NOT_INUSE; //mark as unused
				inode_array[i].size = 0;
				inode_array[i].indirectPointer = 0; //not used yet, also cannot be zero since superblock
				for(int j = 0; j < 12;j++)
				{
					inode_array[i].data_ptrs[j] = 0;
				}
				//inode_array[i].data_ptrs[0] = 0; //this is not possible, since the first block is filled by the superblock
								//inode_array[i].inuse = NOT_INUSE
			}


		}
		// write inodes to directory (inodes start at 1s)
		for(int i = 0; i < inode_blocks; i ++)
			fill_block();
		write_blocks(1,inode_blocks,&inode_array);
		
		//save the bitmap
		//initialize data bitmap
		//Already done at declare time
		//save the bitmap to last block of the file system
		write_blocks(NUM_BLOCKS-1, 1, &free_bit_map);
		//initialize root, 
		for(int i = 0; i < NUM_FILES; i ++)
		{
			root[i].num = INIT_INODE_VAL;
            root[i].name[0] = '\0';
	//		strncpy(root[i].name,"",sizeof(root[i].name)); 
		}
		//write root
		write_blocks(zero_node.data_ptrs[0],root_blocks,&root);
		

	}
	else{
		if(init_disk(SFS_NAME, BLOCK_SIZE, NUM_BLOCKS) < 0)
			exit(0);//Failure
		//read the superblock
		read_blocks(0,1,&super_block);
		// read the inode table
		read_blocks(1,inode_blocks,&inode_array);
		//read the bitmap
		read_blocks(NUM_BLOCKS-1,1, &free_bit_map);
		//read the root directory
		read_blocks(inode_array[0].data_ptrs[0],1 + sizeof(root)/BLOCK_SIZE,&root);



	}
	
}
int sfs_getnextfilename(char *fname){
	int i = 0;
	while (root_position >= 0 && root_position < NUM_FILES)
	{
		if(root[root_position].num >= 0) //the file exists
		{
			strcpy(fname, root[i].name); //copy the file 
			root_position++;
			if(root_position == NUM_FILES) //last file in directory
				return 0; //so return zero
			return 1; //else non zero return
		}
		i++;
	}
	return 0;
}
int sfs_getfilesize(const char* path){
	int filenum,inode_num;
	check_file_exists((char *)path,&filenum,&inode_num);
	if(filenum >= 0)
	{
		//file exists
		//get size
		return inode_array[inode_num].size;
	}
	else
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
		write_blocks(1,inode_blocks,&inode_array);
		return index;
		// and save the inode table
	}

}
int sfs_fopen(char *name){
	int filenum;
	int inode_num;
	int fileID = check_file_exists(name,&filenum,&inode_num);
	if(filenum >= 0)
	{
		//file already exists
		//open and append
		if(fileID != FAILURE)
		{
			//found file; open 
			fildest.fildes[fileID].rwptr = 0;
			fildest.use[fileID] = IN_USE;
			//fildest.fildes[fileID].inodeIndex = inode_num;
			return fileID;
		}
		//then check if it is in the file directory
	}
	

	//file does not exist, assign new inode to file, if inode cannot be assigned errno 1 
	else{
		inode_num = find_free_inode();
		if(inode_num < 0)
			return FILE_ERR1;
	}
	// this region is catch all for either if the inode was not found in the fildestable 
	// or if the file did not exist

	// create new file descriptor here
	//find the first free file descriptor
	fileID = -1;
	for(int i = 0; i < NUM_FILES; i++)
	{
		if(fildest.use[i] == NOT_INUSE){
			fileID = i;
			break;
		}
	}
	if(fileID == -1)
		return FILE_ERR2;
	int i = 0;
	while(i<NUM_FILES)
	{
		if(root[i].num == INIT_INODE_VAL)
			break;
		i++;
	}
	if(i == NUM_FILES)
		return FILE_ERR3; //should never happen because root must have free space if files are free
						  // thats why I dont 
	fildest.fildes[fileID].inode = &(inode_array[inode_num]);
	fildest.fildes[fileID].inodeIndex = inode_num;
	fildest.use[fileID] = IN_USE;
	fildest.fildes[fileID].rwptr = 0;
	//add the file to root
	//find the first free root directory element

	//add file
	root[i].num = inode_num;
	strncpy(root[i].name,name,1+strlen(name));
	//save root to SFS disk block
	write_blocks(inode_array[0].data_ptrs[0],1,&root);
	return fileID;
 
}
int sfs_fclose(int fileID) {
	if(fileID >=0 && fileID < NUM_FILES)
	{
		//file is valid

		if(fildest.fildes[fileID].inodeIndex >= 0 && fildest.use[fileID] == IN_USE)
		{
			//valid inode
			//update the file descriptor values
			fildest.fildes[fileID].rwptr = 0;
			fildest.use[fileID] = NOT_INUSE;
			return 0;
		}
		else
			return FILE_ERR1;

	}
	else
		return FILE_ERR1; //fileID not valid

}
int sfs_fread(int fileID, char *buf, int length) {
	int check_fid = check_file_id(fileID);
	if(check_fid == SUCCESS)
	{
		inode_t *node = fildest.fildes[fileID].inode;
		file_descriptor *thisfildes = &(fildest.fildes[fileID]);
		int total_read = 0;
		char read_arry[BLOCK_SIZE];
		//can only read to size
		if(length+thisfildes->rwptr > node->size)
			length = node->size - thisfildes->rwptr;
		int where_in_block = -1;
		int which_block;
		int i = thisfildes->rwptr/BLOCK_SIZE;
		int i_0 = i;
        int indirect_read[BLOCK_SIZE];
		read_blocks(node->indirectPointer,1,indirect_read);

		//i = i_0;
		while(length > 0 && i <1+(thisfildes->rwptr + length)/BLOCK_SIZE)
		{
			//for first block, have to read from rwptr to end of block.
			// after first block read the whole block
			//find the block
			if(i < 12)
			{
				which_block = node->data_ptrs[i];
			}
			else
			{
			    which_block = indirect_read[i - 12];
			}

			if(i == i_0) //first block find read position
				where_in_block = thisfildes->rwptr - NUM_BLOCKS*(thisfildes->rwptr/BLOCK_SIZE);
			else
				where_in_block = 0;
			read_blocks(which_block,1,read_arry);
            int read_size = (length > BLOCK_SIZE? BLOCK_SIZE : length)-where_in_block;
			memcpy(buf+total_read,read_arry+where_in_block,read_size);
			
			// if(where_in_block == 0)
			// {
			// 	//read whole block, add to buffer
			// 	memcpy(buf+total_read,read_arry,BLOCK_SIZE);

			// }
			// else
			// {
			// 	//find position in block and read from there
			// 	memcpy(buf+total_read,read_arry+where_in_block,BLOCK_SIZE-where_in_block);
			// }
			//update variables
			thisfildes->rwptr += read_size;
			total_read += read_size;
			length -= read_size;
			i++;
			
		}
		return total_read;

	}
	else
		return check_fid;
	
}
// void free_blocks_list(int start, int end, int[] block_array)
// {
// 	for(int i = start; i < end; i++)
// 	{
// 		empty_block(block_array[i]);
// 	}
// }

int sfs_fwrite(int fileID, const char *buf, int length)
{
	int check_fid = check_file_id(fileID);
	if(check_fid == SUCCESS)
	{
		//get the inode and the file descriptor
		inode_t *node = fildest.fildes[fileID].inode;
		file_descriptor *thisfildes = &(fildest.fildes[fileID]);
		//first need to assign the blocks before we write to them
		// make sure all the needed blocks are available to assign
		//calculate the first block to write to
		if(node->size < thisfildes->rwptr+length)
		{
			//need more blocks
			int indirect_ptr_array[BLOCK_SIZE/sizeof(int)];
			int first_ptr_index = -1;// index of the first direct pointer we need
			//int direct_needed;
			int indirect_needed;
			int first_indirect = -1; //flag to indicate this is the first time indirect block is used 
			int num_blocks_needed = 1  + (thisfildes->rwptr + length - node->size)/BLOCK_SIZE;//round up on int operations
			int release_flag = -1;//flag to show all assigned blocks have to be released
			if(num_blocks_needed > NUM_BLOCKS)
				return FILE_ERR3;
			
			//check if all direct pointers are filled up
			//if so, we have to start at an indirect pointer block
			for(int i = 0; i < 12; i++)
			{
				if(node->data_ptrs[i] == NOT_INUSE)
				{
					first_ptr_index = i;
                    break;
				}

			}
			if(first_ptr_index == -1) //no free data blocks
				indirect_needed = num_blocks_needed;
			else
				indirect_needed = num_blocks_needed -(12-first_ptr_index);
			if(indirect_needed > 0)
			{
				//indirect pointers needed
				if(node->indirectPointer == NOT_INUSE)
				{
					node->indirectPointer = fill_block();
					first_indirect = 0;
					for(int k=0;k<BLOCK_SIZE/sizeof(int);k++)
						indirect_ptr_array[k] = NOT_INUSE;
				}
				//first load the inode pointer block
				else{

					read_blocks(node->indirectPointer,1,indirect_ptr_array);
					//how much space is left in indirect pointer?
					for(int i = 0; i < BLOCK_SIZE/sizeof(int);i++)
					{
						if(indirect_ptr_array[i] == NOT_INUSE){
							first_indirect = i;
							break;
						}

					}

				}
				if(first_indirect == -1)
					return FILE_ERR3;//no space in indirect pointer
			}
			//Check if there is enough space
			//to write the file
			if(indirect_needed >(int)(BLOCK_SIZE/sizeof(int)))
				return FILE_ERR3;//not enough space to write
			//Now assign blocks
			for(int i = 0; i < num_blocks_needed; i++)
			{
				if(i + first_ptr_index < 12)
				{
					//direct block assignment
					node->data_ptrs[i+first_ptr_index] = fill_block();
					if(node->data_ptrs[i+first_ptr_index] == 0)
						release_flag = i;
				}
				else
				{
					//indirect block assignment
					indirect_ptr_array[i - (12-first_ptr_index)] = fill_block();
					if(indirect_ptr_array[i] < 0)
						release_flag = i;
				}
			}
			if(release_flag > 0)
			{
				//blocks need to be released, something went wrong will filling them
				for(int i = release_flag - 1; i >= 0; i--)
				{
					if(i + first_ptr_index < 12)
					{
						empty_block(node->data_ptrs[i+first_ptr_index]);
					}
					else
						empty_block(indirect_ptr_array[i-(12-first_ptr_index)]);
				}
				return FILE_ERR3;

			}
			//save the inode table with indirect pointers etc
			if(indirect_needed > 0)
				write_blocks(node->indirectPointer,1,&indirect_ptr_array);
			node->size+=(BLOCK_SIZE*num_blocks_needed);
			write_blocks(1,inode_blocks,&inode_array);
		}

		// all blocks assigned and enough space, so now write the blocks
		int total_written =0;
		char write_array[BLOCK_SIZE];
		int which_block;
		int where_in_block = -1;
		int i = thisfildes->rwptr/BLOCK_SIZE;
		int i_0 = i;//to save
		while(length > 0)
		{
			//find the block
			if(i<12)
			{
				which_block = node->data_ptrs[i];
			}
			else{
				int temp_read[BLOCK_SIZE];
				read_blocks(node->indirectPointer,1,temp_read);
				which_block = temp_read[i - 12];
			}
			if(i == i_0)
			{
				where_in_block = thisfildes->rwptr - NUM_BLOCKS*(thisfildes->rwptr/BLOCK_SIZE);
				read_blocks(which_block,1,write_array);
			}
			else
				where_in_block = 0; //at the start
			memcpy(write_array+where_in_block,buf+total_written,BLOCK_SIZE-where_in_block);
			write_blocks(which_block,1,&write_array);
			if(length - (BLOCK_SIZE - where_in_block) < 0)
			{ 
				where_in_block = BLOCK_SIZE - length;
			}
			thisfildes->rwptr +=BLOCK_SIZE - where_in_block;
			total_written += BLOCK_SIZE - where_in_block;
			length -= BLOCK_SIZE - where_in_block;
			
			i++;
		}
		write_blocks(1,inode_blocks,&inode_array);
		return total_written;


	}
	else
		return check_fid;
}

int sfs_fseek(int fileID, int loc) {
	int check_fid = check_file_id(fileID);
	if(check_fid == SUCCESS)
	{
		inode_t *node = fildest.fildes[fileID].inode;
		file_descriptor *thisfildes = &(fildest.fildes[fileID]);
		if(loc > node->size)
			return FILE_ERR3;
		else
		{
			thisfildes->rwptr = loc;
			return SUCCESS;
		}
	}
	else
		return check_fid;
}
int sfs_remove(char *file) {
	//find the file index
	int filenum,inode_num;
	int indirect_ptr_array[BLOCK_SIZE/sizeof(int)];
	int fileID = check_file_exists(file,&filenum,&inode_num);
	if(fileID == FAILURE)
		return FILE_ERR1;//file does not exist
	//deallocate the blocks
	//calculate how many blocks the file contains
	inode_t *node = &inode_array[inode_num];
	int total_blocks = (node->size)/BLOCK_SIZE;
	if(total_blocks > 12)
		read_blocks(node->indirectPointer,1,indirect_ptr_array);
	for(int i = 0; i < total_blocks;i++)
	{
		if(i < 12){
			empty_block(node->data_ptrs[i]);
		}
		else{
			empty_block(indirect_ptr_array[i-12]);
		}
	}
	if(node->indirectPointer > 0)
		empty_block(node->indirectPointer);

	//remove from root directory, and shift all the elements in the root array
	for(int i = filenum; i < NUM_FILES; i++)
	{
		if(root[filenum].num == INIT_INODE_VAL)
			break;
		else
			root[filenum] = root[filenum + 1];
	}
	//write root block
	write_blocks(inode_array[super_block.root_dir_inode].data_ptrs[0],sizeof(root)/BLOCK_SIZE,&root);
	//remove from inode
	for(int i = inode_num;i < NUM_INODES;i++)
	{
		inode_array[i] = inode_array[i+1];
	}
	//save inode array
	write_blocks(1,inode_blocks,&inode_array);
	return SUCCESS;
}



