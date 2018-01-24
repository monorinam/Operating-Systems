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
			//save inode table
			write_blocks(1,NUM_INODE_BLOCKS,&inode_array);
			return total_written;