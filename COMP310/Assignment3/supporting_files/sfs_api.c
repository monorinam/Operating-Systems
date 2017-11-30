
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
//this function empties out blocks
// (when file is either deleted or blocks that were
// assigned and then not needed)
int make_block_empty()
{

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
				inode_array[i].data_ptrs = -1; //not used yet
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
			fildest.fildes[filedestable_index].rwptr = 0;
			fildest.use[filedestable_index] = IN_USE;
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
	fildest.fildes[fd_index]->inode = &inode_array[inode_num];
	fildest.fildes[fd_index].inodeIndex = inode_num;
	fildest.fildes[fd_index].rwptr = 0;
	return fd_index;

}
int sfs_fclose(int fileID) {
	if(fileID >=0 && fileID < NUM_FILES)
	{
		//file is valid

		if(fildest.fildes[fileID].inodeIndex >= 0)
		{
			//valid inode
			//update the file descriptor values
			fildest.fildes[fileID].rwptr = 0;
			fildest.use[fileID] = NOT_INUSE;
		}
		else
			return FILE_ERR1;

	}
	else
		return FILE_ERR1; //fileID not valid

}
int sfs_fread(int fileID, char *buf, int length) {
	
}
void free_blocks_list(int start, int end, int[] block_array)
{
	for(int i = start; i < end; i++)
	{
		release_block(block_array[i]);
	}
}
int sfs_fwrite(int fileID, const char *buf, int length) {
	//error checking
	// is the fileID valid
	if(fileID >= 0 && fileID < NUM_FILES)
	{
		if(fildest.fildes[fileID].inodeIndex >= 0)
		{
			if(fildest.use[fileID] == IN_USE)
			{
				//file is open for writing
				// find the number of blocks that need to be added to the file (if needed)
				// the addition begins at rwptr and the length of buf is length
				// so the end of the file will be at rwptr + length
				// the current size is size. If size > (rwptr + length) then file needs more blocks
				inode_t * node = &(fildest.fildes[fileID].inode);
				if(&fildest.fildes[fileID].inode->size >( &fildest.fildes[fileID].inode->rwptr + length))
				{
					//needs more blocks
					//Num blocks = (rwptr + length - size)/BLOCK_SIZE
					int indirect_first = (node->indirectPointer==-1)?0:-1;
					int blocks_to_assign = ((&(fildest.fildes[fileID].inode->rwptr) + length) - (&(fildest.fildes[fileID].inode->size)))/BLOCK_SIZE;
					//find if there is a series of such blocks possible
					if(blocks_to_assign > BLOCK_SIZE)
						return FILE_ERR3;
					int block_list[blocks_to_assign];
					int release_flag = 1;
					int i = 0;
					for(; i < blocks_to_assign; i ++)
					{
						block_list[i] = fill_block();
						if(block_list[i] < 0)
						{
							release_flag = -1;
							break;
						}
					}
					//release blocks if needed
					if(release_flag == -1)
					{
						free_blocks_list(0,i,block_list);
						return FILE_ERR3;
					}
					

					// assign the blocks to the inodes
					int k = 0; //j is the reference for the data pointers inside the node
					while(k < 12)
					{
						if(node->data_ptrs[k] != -1) //it is occupied
							k++;
						else
							break;
					}
					//check if the indirect block will be needed
					// if we need it, then assign a block to it
					// this block will contain an array filled with pointers to the other blocks
					// if the data size is more than that can be accomodated in one indirect pointer
					// release the block and terminate
					if(blocks_to_assign > 12-k)
					{
						//indirect block needed
						int indirect_ptr_array[BLOCK_SIZE];
						if(node->indirectPointer == -1) //indirect pointer block does not exist
							node->indirectPointer = fill_block();
						if(node->indirectPointer < 0 || sizeof(indirect_ptr_array) > BLOCK_SIZE)
						{
							//free all the blocks assigned so far
								free_blocks_list(0,blocks_to_assign,block_list);
								if(node->indirectPointer >=0)
									release_block(node->indirectPointer);
								return FILE_ERR3;
						}

						//load pointer to memory if this is not first indirect block write
						if(indirect_first == -1){
							read_blocks(node->indirectPointer,1,indirect_ptr_array);
							//check indirect pointer is not full and has enough space for new stuff
							int m;
							for(m=0; m < BLOCK_SIZE; m++)
							{
								if(indirect_ptr_array[m] == NOT_INUSE)
								{
									//break;
									indirect_first = m;
									break;
								}
							}
							if(BLOCK_SIZE - m < blocks_to_assign - (12-k))
							{
								//release all blocks and return
								free_blocks_list(0,blocks_to_assign,block_list);
								return FILE_ERR3;

							}
						}
						else{
							//first indirect pointer block written here, so mark them all as unused
							for(int m = 0; m <BLOCK_SIZE;m++)
							{
								indirect_ptr_array[m] = NOT_INUSE; //mark as unused
							}
						}
						
						

					}
					int j = indirect_first;
					for(i = 0; i < blocks_to_assign; i ++)
					{
						//assign to direct pointer
						if((k+i) < 12)
						{
							node->data_ptrs[k+i] = blocks_to_assign[i];
						}
						else
						{
							indirect_ptr_array[j] = blocks_to_assign[i];
							j++;
							
						}
						
					}
					//save indirect pointer to block
					write_blocks(node->indirectPointer,1,&indirect_ptr_array);
					//save inode table
					write_blocks(1, NUM_INODE_BLOCKS,&inode_array);
				}
				//blocks added; file can be written now
				// first block to write is rwptr/BLOCK_SIZE (this is also k-1)
				// and write starts at position k - 1 + rwptr
				// So for each block, find which block is being written to from the 
				// data pointers or indirect pointers. Then read the block (i=0) and
				// add new data and write the block.
				int total_written = 0;
				int temp_read_array[BLOCK_SIZE];
				int indirect_index = indirect_first-1s;
				

				for(int i = k-1; i < (node->rwptr+length)/BLOCK_SIZE + 1;i++)
				{
					int this_block = -1;
					int written_this_block = BLOCK_SIZE - ((node->rwptr)%BLOCK_SIZE);
					//if the amount to write is less than the space left in block, then just write length
					written_this_block = written_this_block>length?length:written_this_block;
					if(i == k-1)
					{
						//the first block, this is partially written
						// check if block is full
						if(written_this_block == 0)
							continue;
						
						else
						{
							//now find the block
							//direct pointer
							//k is the first empty data pointer
							//if k = 12, then data_ptr[11] is partially empty
							if(k <= 12)
							{
								this_block  = node->data_ptrs[i];
							}
							else{
								//indirect pointer points to a partially empty block
								//find which indirect pointer
								this_block = node->indirect_ptr_array[indirect_index];
							}
							//read the block
							read_blocks(this_block,1,temp_read_array);

							//then write to it 
							int j = 0;
							int m = node->rwptr%BLOCK_SIZE;
							for(j<written_this_block && m < BLOCK_SIZE)
							{
								temp_read_array[m] = buf[j];
								j++;
								m++;

							}
							//write_blocks(this_block,1,&temp_read_array);
							
						}
					}
					else{
						if(i < 12)
						{
							this_block = node->data_ptrs[i];
						}
						else{
							this_block = indirect_ptr_array[indirect_index];
						}
						memcpy(temp_read_array,buf+total_written);
						
						//just write to the block 1024 length
					}
					write_blocks(this_block,1,&temp_read_array);
					indirect_index++;
					total_written += written_this_block;
					length -= written_this_block;
					node->size += written_this_block;
					node->rwptr += written_this_block;
					//update all the variables

				}


			}
			else 
				return FILE_ERR3; //file is not open
		}
		else 
			return FILE_ERR2; //file does not exist

	}
	else
		return FILE_ERR1; //file ID does not exist
	// does the file exist
	// is the file open for writing


}
int sfs_fseek(int fileID, int loc) {

}
int sfs_remove(char *file) {


}



