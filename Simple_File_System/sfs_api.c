#include "sfs_api.h"
//#include "bitmap.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fuse.h>
#include <strings.h>
#include "disk_emu.h"
#define LASTNAME_FIRSTNAME_DISK "SIMPLEFILESYS_DISK.disk"
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
int root_blocks;
//------------------------------------------------------------------------------
//                          SFS HELPER FUNCTIONS

// This function gets the index of the next free block in the free bitmap
// If there is an error, it returns -1
// Otherwise it returns the index of the first free block
// The block is marked as in use in the free bit map array
int get_block_if_free()
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
    {
         write_blocks(NUM_BLOCKS-1,1,&free_bit_map);

		return first_free; //success
           }

}
// This function calls get_block_if_free()
// If the return is -1, it converts to 0
// Otherwise returns the block number
// It is used to check for assignment errors
// After the first one (superblock) since
// the inode data pointers are unsigned integers
int fill_block()
{
    int check = get_block_if_free();
    if(check == -1)
        return 0;
    else
        return check;
}
//This function empties out blocks with given index (bloc_num)
// 
void empty_block(int block_num)
{
	//find this block in the free bitmap
	//then mark the block as free
	int i = block_num/8;
	int j = block_num - 8*i;
	FREE_BIT(free_bit_map[i],j);
    write_blocks(NUM_BLOCKS-1,1,&free_bit_map);

}
// This function checks the file name fname
// File names can have a max of 16 characters, 
// 1 dot and 3 char extensions
// If the file name is good, then it returns SUCCESS
// else it returns FAILURE
int check_filename(char *fname)
{
    int num_name_chars = 0;//stores number of names
    int num_dots = 0; //stores number of dots
    int num_extens = 0; //stores number of extension chars
    int i = 0;
    //Count the number of each type of characters
    //If character is after a dot, then it is an extension
    //Otherwise it is part of the name.
    //Null terminates the loop
    while(i < (1+MAX_FILE_NAME))
    {
        if(fname[i] == '\0')
            break;
        if(fname[i] == '.')
            num_dots += 1;
        else if(num_dots > 0)
            num_extens += 1;
        else
            num_name_chars += 1;
        i++;
    }
    if(num_dots > 1 || num_name_chars > 16 || num_extens > 3)
        return FAILURE;
    else
        return SUCCESS;

}


//This function checks if a file name already exists
//If so, finds it in the inode array and the file descriptor table
//It has three return valies: fileID, filenum and inode_num
//If a file is closed, it returns the filenum and inode_num
//But the fileID is FAILURE. 
//If a file is open, then it returns the filenum, inode_num and fileID
int check_file_exists(char* fname, int *filenum, int *inode_num)
{
	*filenum = -1;
	*inode_num = -1;
    //initial default values
	for(int i = 0; i < NUM_FILES; i++)
	{
		if(strcmp(fname,root[i].name) == 0)
        {	*filenum = i;
            break;}
	}
	if(*filenum == -1)
		return FAILURE; //not found
	//now search for inode
    //Then searching for the index in the file descriptor table
    //"fileID"
	*inode_num = root[*filenum].num;
	for(int i = 0; i < NUM_FILES; i++)
	{
		if(fildest.fildes[i].inodeIndex == *inode_num)
			return i;

	}
	return FAILURE;
}
// This function checks if a file with the given 
// fileID is in the file descriptor table
// If yes, it returns success
// It checks for validity of fileID's.
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
//finds and creates an inode in the inode table
//at the position of the first free inode
//Iterates through the inode table
//Checks for one that is unused, and then
//returns the index of the free inode
//The inode is marked as inuse, with zero size
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
        //printf("Writing inode array the root numbers are %d %d \n", inode_array[0].data_ptrs[8],inode_array[0].data_ptrs[10]);
		write_blocks(1,inode_blocks,&inode_array);
		return index;
		// and save the inode table
	}

}
//----------------------------------------------------------------------------------
//                          SFS API FUNCTIONS
//Function to initialize the file system
//fresh indicates whether it should be remade from scratch or read
//from an existing system
void mksfs(int fresh) {
    // This variable keeps track of the position in the directory listing
    // for sfs_getnextfilename()
    //
	root_position = 0;
    //Calculate size of root and inode blocks based on struct and array sizes
    //
    root_blocks = sizeof(root)/BLOCK_SIZE + 1;
	inode_blocks = sizeof(inode_array)/BLOCK_SIZE + 1;
    //zero out all global variables before reading them
    //to prevent any garbage from being stored in them by mistake
    for(int i = 0; i < NUM_FILES; i++)
	 {
	    fildest.fildes[i].inodeIndex = INIT_INODE_VAL;
	    fildest.fildes[i].inode = NULL;
		fildest.fildes[i].rwptr = INIT_INODE_VAL;
	    fildest.use[i] = NOT_INUSE;
	}
    for(int i = 0; i < NUM_FILES; i++)
     {
        root[i].num = -1;
        root[i].name[0] = '\0';
     }
    for(int i = 0; i < NUM_INODES; i++)
    {
        inode_array[i].size = 0;
        inode_array[i].indirectPointer = 0;
        for(int j=0;j<12;j++)
        {
            inode_array[i].data_ptrs[j] = 0;
        }
    }

	if(fresh)
	{
    	inode_t zero_node;
        //remove the filesystem if it exists
		remove(LASTNAME_FIRSTNAME_DISK);
		//then initialize it again from fresh
		init_fresh_disk(LASTNAME_FIRSTNAME_DISK, BLOCK_SIZE, NUM_BLOCKS);

		//write super block
		//initialize super block first
		super_block.magic = SB_MAGIC;
		super_block.block_size = BLOCK_SIZE;
		super_block.fs_size = NUM_BLOCKS*BLOCK_SIZE; //the file system size
		super_block.inode_table_len = NUM_INODES;
		super_block.root_dir_inode = 0;
        int superblock = get_block_if_free();
		if(superblock == 0)//should always be zero
			write_blocks(0,1,&super_block);
		else
			exit(0);
        //Fill inode blocks (to store the inode array)
        for(int i = 0; i < inode_blocks; i ++)
        {
		    int check =	fill_block();
            if(check <= 0)
            {
                printf("Error assigning inode blocks aborting..\n");
                exit(EXIT_FAILURE);
            }
        }

		// initialize inodes
		for(int i = 0; i < NUM_INODES; i++)
		{
			if(i==0)
			{
				//the first inode has info about root directory
                //So it is initialized here
				zero_node.size = 0;
				zero_node.mode = 755;
				zero_node.link_cnt = 1;
				zero_node.uid = 0;
				zero_node.gid = 0;
				zero_node.indirectPointer = 0;
                zero_node.inuse = IN_USE;
                //Fill the root blocks to store the root directory array
				for(int i = 0; i < 12 ; i++)
				{
                    if(i < root_blocks)
                    {
                        int check = fill_block();
                        if(check > 0)
				    	    zero_node.data_ptrs[i] = check;
                        else
                            exit(EXIT_FAILURE);
                    }
                    else
                        zero_node.data_ptrs[i] = 0;
					
				}
				inode_array[0] = zero_node;


			}
			else
			{
                //fill rest of the inodes
				inode_array[i].inuse = NOT_INUSE; //mark as unused
				inode_array[i].size = 0;
				inode_array[i].indirectPointer = 0; //not used yet, also cannot be zero since superblock is the only one with zero so for all other inodes, zero indicates
                //unused
				for(int j = 0; j < 12;j++)
				{
					inode_array[i].data_ptrs[j] = 0;
				}
			}
		}
		// write inodes to directory (inodes start at 1s)
		write_blocks(1,inode_blocks,&inode_array);
		
		//save the bitmap to last block of the file system
		write_blocks(NUM_BLOCKS-1, 1, &free_bit_map);
		//initialize root, and set all names to null
		for(int i = 0; i < NUM_FILES; i ++)
		{
			root[i].num = INIT_INODE_VAL;
            root[i].name[0] = '\0';
		}
		//write root to directory
		write_blocks(zero_node.data_ptrs[0],root_blocks,&root);
		

	}
	else{
        //open existing disk
		if(init_disk(LASTNAME_FIRSTNAME_DISK, BLOCK_SIZE, NUM_BLOCKS) < 0)
			exit(0);//Failure
		//read the superblock
		read_blocks(0,1,&super_block);
		// read the inode table
		read_blocks(1,inode_blocks,&inode_array);
		//read the bitmap
		read_blocks(NUM_BLOCKS-1,1, &free_bit_map);
		//read the root directory
		read_blocks(inode_array[0].data_ptrs[0],root_blocks,&root);
        read_blocks(1,inode_blocks,&inode_array);
       	}
	
}
// This function gets the filename of the next file in the root directory
// It uses global variable root_position to keep track of the position in the
// root array. Root is continuously filled (and removals are shifted to the left)
// So it just checks if a file exists at the position. If not, the last file
// is reached.
int sfs_getnextfilename(char *fname){
    if(check_filename(fname) == FAILURE)
        return FILE_ERR1;
		if(root[root_position].num > 0) //the file exists
		{
			strcpy(fname, root[root_position].name); //copy the file 
			root_position++;
			if(root_position == NUM_FILES) //last file in directory
				return 0; //so return zero
			return 1; //else non zero return
		}
        else
            return 0; //since root is continuous so end of directory
}
// Gets the filesize of the file specified by path
// It checks that the filename is valid
// Then finds the position in root and inode index
// If it exists in root, returns the size from the
// inode array element
// else returns FAILURE
int sfs_getfilesize(const char* path){
    if(check_filename((char *)path) == FAILURE)
        return FILE_ERR1;
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
// Opens a new file
// Checks file name is valid
// If file is already open, returns the fileID
// If file is not open (ie, not in the file descriptor table
// Then, creates a new file descriptor and returns it
// If file does not exist in root, returns error

int sfs_fopen(char *name){
    if(check_filename(name) == FAILURE)
        return FILE_ERR1;
   	int filenum;
	int inode_num;
	int fileID = check_file_exists(name,&filenum,&inode_num);
    //File exists in root
	if(filenum >= 0)
	{
		//File is open
		//So, return fileID
		if(fileID != FAILURE)
			return fileID;
	}
	

// File does not exist in root, so create new file. Assign an inode, add it to root
// And then create the open file in file descriptor table
    else{
		inode_num = find_free_inode();
		if(inode_num < 0)
			return FILE_ERR1;
	    }
    // Inode assigned and file in root, so now create a new entry 
    // in the file descriptor table
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
    //Could not find open fileID, so return error
	if(fileID == -1)
		return FILE_ERR2;
    //File ID found, now find space in the root directory for this file
    //If this is a new file (otherwise we already know root number
	int i = 0;
    if(filenum >= 0)
    {
        //file exists in root, just not open yet
        i = filenum;
    }
    else{
        //Find space in root
	    while(i<NUM_FILES)
    	{
	    	if(root[i].num == INIT_INODE_VAL)
		    	break;
	    	i++;
    	}
    }
	if(i == NUM_FILES)
		return FILE_ERR3; //should never happen because root must have free space if files are free
    // Now add element to the file descriptor table since it we have
    // All three elements (space in file, space in root and space in inode table)
    // File is opened in append mode (rwptr at size)
    fildest.fildes[fileID].inode = &(inode_array[inode_num]);
	fildest.fildes[fileID].inodeIndex = inode_num;
	fildest.use[fileID] = IN_USE;
    fildest.fildes[fileID].rwptr = inode_array[inode_num].size;
	//add the file to root
	root[i].num = inode_num;
	strncpy(root[i].name,name,1+strlen(name));
	//save root to SFS disk block
	write_blocks(inode_array[0].data_ptrs[0],root_blocks,&root);
	return fileID;
 
}
//Closes an open file and removes it from the file descriptor table
//Check validity of fileID
//Then if the file is marked as in use, mark it as not in use
//And clear the inode index to null, and rwptr to 0, so that
//this index of "fildes" can be reused
int sfs_fclose(int fileID) {
    if(check_file_id(fileID) != SUCCESS)
        return FILE_ERR1;

    if(fildest.fildes[fileID].inodeIndex >= 0 && fildest.use[fileID] == IN_USE)
    {
        //valid inode
        //update the file descriptor values
        fildest.fildes[fileID].rwptr = 0;
        fildest.use[fileID] = NOT_INUSE;
        fildest.fildes[fileID].inodeIndex = INIT_INODE_VAL;
        return 0;
    }
    else
        return FILE_ERR1;

}
// Read function
// Algorithm:
//      - Find the inode, and file descriptor table
//      - Then find the first block to be read
//      - Find how much of the block to read
//      - Copy the part of the block to buf
//      - Continue doing this for other blocks until length = 0
//      - Going through both direct and indirect pointers
//      - If size is less than length, then read to end of file
//
int sfs_fread(int fileID, char *buf, int length) {
	int check_fid = check_file_id(fileID);
    if(check_fid != SUCCESS)
        return check_fid;
    // Valid file here
    // So start the reading
    inode_t *node = fildest.fildes[fileID].inode;
    file_descriptor *thisfildes = &(fildest.fildes[fileID]);
    int total_read = 0;
    char read_arry[BLOCK_SIZE];
    //can only read to size, so curtail length if needed
    if(length+thisfildes->rwptr > node->size)
        length = node->size - thisfildes->rwptr;
    int where_in_block = -1;
    int which_block;
    //First block to read
    int i = thisfildes->rwptr/BLOCK_SIZE;
    //Variable to store the first block read (since rwptr changes in the loop)
    int i_0 = i;
    //Read the indirect blocks (if they are empty this reads garbage)
    //But they are only used if the indirectpointer is not empty
    int indirect_read[BLOCK_SIZE];
    read_blocks(node->indirectPointer,1,indirect_read);

    while(length > 0)    {
        //for first block, have to read from rwptr to end of block.
        // after first block read the whole block
        //
        //find the block
        if(i < 12)
        {
            which_block = node->data_ptrs[i];
        }
        else
        {
            which_block = indirect_read[i - 12];
        }
        //Find the read position in the first block
        //If not the first block, then starts reading
        //at the beginning of the block
        if(i == i_0) //first block find read position
            where_in_block = thisfildes->rwptr - NUM_BLOCKS*(thisfildes->rwptr/BLOCK_SIZE);
        else
            where_in_block = 0;
        // Read the block
        read_blocks(which_block,1,read_arry);
        // Find the size to be read in this block
        // If reading from the beginning of the block
        // Then either read to end of the block (BLOCK_SIZE)
        // or read to length if length is smaller
        // If reading from an offset, then either read to
        // length or BLOCKSIZE, whichever is smaller
        //
        int read_size;
        if(where_in_block == 0)
        {
            if(length > BLOCK_SIZE)
                read_size = BLOCK_SIZE;
            else
                read_size = length;
        }
        else
        {
            if(where_in_block + length > BLOCK_SIZE)
                read_size = BLOCK_SIZE - where_in_block;
            else
                read_size = length;
        }
        // Copy amount read_size to the buf
        memcpy(buf+total_read,read_arry+where_in_block,read_size);
        
        //update variables
        thisfildes->rwptr += read_size;
        total_read += read_size;
        length -= read_size;
        i++;
        
    }
    return total_read;	
}
//Function to write to a file
//      - First name sure file is open
//      - Then find the number of blocks needed to write
//      - If number of blocks needed is more than those left
//      - Then abort, otherwise assign blocks
//      - If a block cannot be assigned, release all blocks
//      - and abort.
//      - Once all blocks are assigned, write the same way
//      - as read was done
//      (This should be done in several functions, but I did not have time 
//      to modify it so it is quite long)
int sfs_fwrite(int fileID, const char *buf, int length)
{
	int check_fid = check_file_id(fileID);
	if(check_fid != SUCCESS)
        return check_fid;
	//get the inode and the file descriptor
    inode_t *node = fildest.fildes[fileID].inode;
    file_descriptor *thisfildes = &(fildest.fildes[fileID]);
    // STEP 1:
    // ASSIGN ALL NECESSARY BLOCKS
    //      - FIRST MAKE SURE ALL NEEDED BLOCKS ARE AVAILABLE
    //      - THEN DO THE ASSIGNMENT
    //Calculate the total blocks needed
    int num_blocks_needed = 1  + (thisfildes->rwptr + length)/BLOCK_SIZE;//round up on int operations
    //Find how many blocks are assigned
    int blocks_assigned;
    // If size of the file is zero, no blocks are assigned (edge case)
    if(node->size == 0)
        blocks_assigned = 0;
    else
        blocks_assigned = 1 + (node->size/BLOCK_SIZE);
    // Find number of blocks that needed to be assigned
    num_blocks_needed -= blocks_assigned;
    // ********* Block assignment ***********
    if(num_blocks_needed > 0)
    {
        int indirect_ptr_array[BLOCK_SIZE/sizeof(int)];
        int first_ptr_index = -1;// index of the first direct pointer we need
        int indirect_needed;
        //flag to indicate this is the first time indirect block is used 
        int first_indirect = -1; 
        int release_flag = -1;//flag to show all assigned blocks have to be released
        // Not enough blocks available
        if(num_blocks_needed > NUM_BLOCKS)
            return FILE_ERR3;
        
        //check if all direct pointers are filled up
        //if so, we have to start at an indirect pointer block
        for(int i = 0; i < 12; i++)
        {
            if(node->data_ptrs[i] == NOT_INUSE)
            {
                first_ptr_index = i;//the previous block may not be filled up;
                break;
            }

        }
        if(first_ptr_index == -1) //no free direct blocks
        {
            indirect_needed = num_blocks_needed;
            first_ptr_index = 12;//re-assign for consistent maths
        }
        else
            indirect_needed = num_blocks_needed -(12-first_ptr_index);
        // ****** Assign indirect blocks *******
        if(indirect_needed > 0)
        {
            //indirect pointers needed
            if(node->indirectPointer == NOT_INUSE)
            {
                node->indirectPointer = fill_block();
                if(node->indirectPointer < 1)
                    return FILE_ERR3;
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
        // in the free bit map
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
                int index = i - (12-first_ptr_index)+first_indirect;
                indirect_ptr_array[index] = fill_block();
                if(indirect_ptr_array[i] == 1)
                    release_flag = i;
            }
        }
        // Blocks need to be released
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
        write_blocks(1,inode_blocks,&inode_array);
    }

    // ********* WRITE BLOCK REGION ****************
    int total_written =0;
    char write_array[BLOCK_SIZE];
    int which_block;
    int where_in_block = -1;
    int i = thisfildes->rwptr/BLOCK_SIZE;
    // save the first block valie
    int i_0 = i;
    
    int temp_read[BLOCK_SIZE];
    // Read the data (same algorithm as fread)
    while(length > 0)
    {
        //find the block
        if(i<12)
        {
            which_block = node->data_ptrs[i];
        }
        else{
            read_blocks(node->indirectPointer,1,temp_read);
            which_block = temp_read[i - 12];
        }
        // For the first block, find the position in the block 
        // Otherwise start writing at beginning of block
        // For first block, also read the current block
        // So it is not overwritten
        if(i == i_0)
        {
            where_in_block = thisfildes->rwptr - NUM_BLOCKS*(thisfildes->rwptr/BLOCK_SIZE);
            read_blocks(which_block,1,write_array);
        }
        else
            where_in_block = 0; //at the start
        // Copy to write array
        memcpy(write_array+where_in_block,buf+total_written,BLOCK_SIZE-where_in_block);
        // Write the block
        // Update all variables
        write_blocks(which_block,1,&write_array);
        // Update the length, and factor in whether it is smaller than end of block
        if(length - (BLOCK_SIZE - where_in_block) < 0)
        { 
            where_in_block = BLOCK_SIZE - length;
        }
        thisfildes->rwptr +=BLOCK_SIZE - where_in_block;
        total_written += BLOCK_SIZE - where_in_block;
        length -= BLOCK_SIZE - where_in_block;
        if(thisfildes->rwptr > node->size)
           node->size = thisfildes->rwptr;
        i++;
    }
    //update inode array
    write_blocks(1,inode_blocks,&inode_array);
    return total_written;

}
// Function to move the rwptr to the desired location
//          - Checks the file is open
//          - If yes, then checks if the loc is within the size
//          - If yes, then moves rwptr to loc
//          - If no, returns an error
int sfs_fseek(int fileID, int loc) {
	int check_fid = check_file_id(fileID);
    if(check_fid != SUCCESS)
        return check_fid;
    inode_t *node = fildest.fildes[fileID].inode;
    file_descriptor *thisfildes = &(fildest.fildes[fileID]);
    // Cannot seek the given location
    if(loc > node->size)
        return FILE_ERR3;
    else
    {
        thisfildes->rwptr = loc;
        return SUCCESS;
    }
}
// Removes a file from the root directory
//      - Checks filename
//      - Find the file in root and inode arrays
//      - If the file is open, close it
//      - If it exists, then shifts the root left
//      - (Root is always continuous)
//      - Empties out the inode blocks that were attached
//      - To the file
//      - And zeroes out the given inode
int sfs_remove(char *file) {
    if(check_filename(file) == FAILURE)
        return FILE_ERR1;
	//find the file index
	int filenum,inode_num;
	int indirect_ptr_array[BLOCK_SIZE/sizeof(int)];
	int fileID = check_file_exists(file,&filenum,&inode_num);
    //file does not exist

	if(filenum == FAILURE)
		return FILE_ERR1;
    if(fileID >= 0)
        sfs_fclose(fileID);
    //deallocate the blocks
	//calculate how many blocks the file contains
	inode_t *node = &inode_array[inode_num];
	int total_blocks =1+ (node->size)/BLOCK_SIZE;
	if(total_blocks > 12)
		read_blocks(node->indirectPointer,1,&indirect_ptr_array);
	for(int i = 0; i < total_blocks;i++)
	{
		if(i < 12){
			empty_block(node->data_ptrs[i]);
            node->data_ptrs[i] = INIT_INODE_VAL;
		}
		else{
			empty_block(indirect_ptr_array[i-12]);
		}
	}
	if(node->indirectPointer > 0){
		empty_block(node->indirectPointer);
        node->indirectPointer = INIT_INODE_VAL;
    }
    node->size = 0;
    node->inuse = NOT_INUSE;
	//remove from root directory, and shift all the elements in the root array
	for(int i = filenum; i < NUM_FILES; i++)
	{
		if(root[i].num == INIT_INODE_VAL)
			break;
		else
			root[i] = root[i + 1];
	}
	//write root block
	write_blocks(inode_array[0].data_ptrs[0],root_blocks,&root);
    //mark the inode as usued and clear it out
    inode_array[inode_num].inuse = NOT_INUSE;
    inode_array[inode_num].size = 0;
    if(fileID >= 0)
        sfs_fclose(fileID);
	//save inode array
	write_blocks(1,inode_blocks,&inode_array);
	return SUCCESS;
}



