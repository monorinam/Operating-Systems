int sfs_fwrite(int fileID, const char *buf, int length)
{
	int check_fid = check_file_id(fileID);
	if(check_fid == SUCCESS)
	{
		//get the inode and the file descriptor
		inode_t * node = &(fildest.fildes[fileID].inode);
		file_descriptor *thisfildes = &(fildest.fildes[fileID]);
		//first need to assign the blocks before we write to them
		// make sure all the needed blocks are available to assign
		//calculate the first block to write to
		if(node->size < thisfildes->rwptr+length)
		{
			//need more blocks
			int indirect_ptr_array[BLOCK_SIZE/sizeof(int)];
			int first_ptr_index = -1;// index of the first direct pointer we need
			int direct_needed;
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
							break
						}

					}

				}
				if(first_indirect == -1)
					return FILE_ERR3;//no space in indirect pointer
			}
			//Check if there is enough space
			//to write the file
			if(indirect_needed > BLOCK_SIZE/sizeof(int))
				return FILE_ERR3;//not enough space to write
			//Now assign blocks
			for(int i = 0; i < num_blocks_needed; i++)
			{
				if(i + first_ptr_index < 12)
				{
					//direct block assignment
					node->data_ptrs[i+first_ptr_index] = fill_block();
					if(node->data_ptrs[i+first_ptr_index] < 0)
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
						release_block(node->data_ptrs[i+first_ptr_index]);
					}
					else
						release_block(indirect_ptr_array[i-(12-first_ptr_index)]);
				}
				return FILE_ERR3;

			}
			//save the inode table with indirect pointers etc
			if(indirect_needed > 0)
				write_blocks(node->indirectPointer,1,&indirect_ptr_array);
			write_blocks(1,NUM_INODE_BLOCKS,&inode_array);
		}

		// all blocks assigned and enough space, so now write the blocks
		int total_written =0;
		int write_array[BLOCK_SIZE];
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
			thisfildes->rwptr +=BLOCK_SIZE - where_in_block;
			total_written += BLOCK_SIZE - where_in_block
			length -= BLOCK_SIZE - where_in_block;
			i++;
		}
		return total_written;


	}
	else
		return check_fid;
}