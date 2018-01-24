#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <dirent.h>
#include <sys/types.h>
#include <signal.h>
#include <fcntl.h>
#include <time.h>
#include <sys/wait.h>
#define SUCCESS 0
#define FAILURE 1
#define MAX_OPTIONS 20 // As defined in the example code given to us

// This code is given for illustration purposes. You need not include or follow this
// strictly. Feel free to writer better or bug free code. This example code block does not
// worry about deallocating memory. You need to ensure memory is allocated and deallocated
// properly so that your shell works without leaking memory.
// This is implemented with stderr as stdout, and so all error is displayed on the screen
// This struct holds all the background processes
typedef struct process_node {
    char** process_cmd;
    pid_t process_id;
    int cmd_length;
    struct process_node * next_process; // pointer to previous process in stack
 } process_node;
 process_node* head_process_node; // The head process , starts with shell

// these are global so they can be shared with the signal handler
// for SIGINT. This is not a very elegant solution.
 /* I use this method since the foreground process is always the last child
 process that is running */
int bg;
pid_t process_child_id;

 // Linked List functions
// Push
void push_process_to_stack(char** cmd, pid_t p_id, int cmd_length)
{
	process_node* temp_node = (process_node*)malloc(sizeof(process_node));
	// de-reference the cmd pointer to prevent the command name in the linked list
	// from being overwritten
	char** cmd_copy;
	cmd_copy = malloc(cmd_length*sizeof(cmd));
	for(int i = 0; i < cmd_length; i++)
	{
		cmd_copy[i] = strdup(cmd[i]);
	}
	temp_node->process_cmd = cmd_copy;
	temp_node->process_id = p_id;
	temp_node->cmd_length = cmd_length;
	if(head_process_node == NULL){
		temp_node->next_process = NULL;
	}
	else{
		temp_node->next_process = head_process_node;
	}
	
	head_process_node = temp_node;
}

int pop_process_from_stack(pid_t p_id)
{
	process_node* previous_node;
	process_node* current_node; // since we need to rejoin list when modifying it
	current_node = head_process_node;
	previous_node = NULL; // to start with, since the head node has no previous node

	// Assume each process id is distinct, and terminate while loop when the process id 
	// is found
	int flag = 1;
	while(flag && current_node != NULL)
	{
		if(current_node->process_id == p_id)
		{
			flag = 0; // found the right process in background
			if(previous_node == NULL)
			{
				//if first node is being removed
				head_process_node = current_node->next_process;
			}
			else
			{
				previous_node->next_process = current_node->next_process;
				
			}
		}
		if(flag == 1)
		{
			previous_node = current_node;
			current_node = current_node->next_process;

		}
			
	}
	if(flag == 1)
	{
		// Process was not in background
		return FAILURE;
	}
	else
	{
		for(int i = 0; i < current_node->cmd_length; i++)
		{
			free(current_node->process_cmd[i]);
		}
		free(current_node->process_cmd);
		free(current_node);
		return SUCCESS;
	}
}
// Function to count processes in queue
int count_processes()
{
	int count = 0;
	// To prevent any memory leaks save initial malloced pointer
	process_node* temp_node_to_free = malloc(sizeof(process_node));
	process_node* temp_node = temp_node_to_free;
	temp_node = head_process_node;
	// traverse through queue and find functions
	while(temp_node != NULL)
	{	
		count++;
		temp_node = temp_node->next_process;

	}
	free(temp_node_to_free);
	return count;
}

// Command functions
int getcmd(char *prompt, char *args[], int *background)
{
    int length, i = 0;
    char *token, *loc;
    char *line = NULL;
    size_t linecap = 0;
    printf("%s", prompt);
    length = getline(&line, &linecap, stdin);
    if (length <= 0) {
        exit(-1);
    }
    // Check if background is specified..
    if ((loc = index(line, '&')) != NULL) {
        *background = 1;
        *loc = ' ';
    } else {
        *background = 0;
    }
    char *l = line;
    while ((token = strsep(&line, " \t\n")) != NULL) {
        for (int j = 0; j < strlen(token); j++)
            if (token[j] <= 32)
                token[j] = '\0';
        if (strlen(token) > 0)
            args[i++] = token;
    }
    if(i == 0) {
    	free(l);
    }
    return i;
}

// Implements the built in scandir 
// using only system calls
int scan_direct(char* dirname)
{
	struct dirent *file_name;
	DIR *dir_val;
	int nFiles;
	// Opens the directory
	dir_val = opendir(dirname);
	// No files in directory
	if(dir_val == NULL)
	{
		return -1;
	}
	else
	{
		// Reads all files in directory
		nFiles = 0;
		file_name = readdir (dir_val); 
		while(file_name != NULL)
		{		
		write(1,file_name->d_name, strlen(file_name->d_name));
		write(1,"\n",1);
		file_name = readdir (dir_val);
		nFiles++;
		}
		closedir(dir_val);
		free(file_name);
		return nFiles;
	}
}
// Implements ls, and calls scan_direct
int file_display(char** args, int cnt, int redir_flag)
{
    int numFiles;
    int i;

    if(cnt == 1 || redir_flag > 0){
        numFiles = scan_direct(".");
    }
    else{
        numFiles = scan_direct(args[1]);
    }

    if(numFiles == -1){
        fprintf(stderr,"Scandir: Error"); 
        return FAILURE;
    }
    return SUCCESS;
}
int change_dir(char**args, int cnt){
    if(cnt > 1)
    {
        if(!chdir(args[1]))
        {
            return SUCCESS;
        }
        else{
            fprintf(stderr,"cd: No such file or directory \n"); 
            return FAILURE;
        }
    }
    else{
        printf("cd : Please enter name of directory \n"); 
        return FAILURE;
    }


}
void exit_shell()
{
    printf("Are you sure you want to exit program? Enter [y]");      
    size_t confchar = 0;
    char *confirm = NULL;
    int conflen = 0;
    conflen = getline(&confirm, &confchar, stdin); 
    if(!strcmp("y\n", confirm))
    { 
        printf("Terminating ....\n"); 
        exit(EXIT_SUCCESS);
    }
}

int cat(char** args, int cnt)
{
    int file;
    int fileLength;
    //One declaration on line 270
    // since char array size is unknown
    if(cnt==1)
    {
        write(STDERR_FILENO,"No file to cat\n",15); //TODO Change
        return FAILURE;
    }
    else
    { 
        
        file = open(args[1], O_RDONLY);
        if (file < 0)
        {
            write(STDERR_FILENO,"Error opening file \n",20); //TODO Change
            return FAILURE;
        }
        /** lseek finds the end of the file and 
        // lets us know the entire filelength 
        This prevents us from having to loop through the
        file or guess at buffer lengths
        */
        //go to end of file
        fileLength = lseek(file, 0, SEEK_END);
        int buffer[fileLength];
        // go to beginning of file
        lseek(file, 0, SEEK_SET);
        // if there is something in the file then write it
        // to standard output
        if(read(file, buffer, fileLength))
        	write(STDOUT_FILENO, buffer, fileLength);
        close(file); 
        return SUCCESS;
    }
}


int file_copy(char** args, int cnt)
{
    if(cnt == 1 || cnt == 2){
        write(STDERR_FILENO,"No file to copy\n", 16); 
        return FAILURE;
    }
    else{
        int file1;
        int file2;
        int fileLength;
        file1 = open(args[1], O_RDONLY); 
        file2 = open(args[2], O_RDWR | O_APPEND | O_CREAT,0777); 
        if (file1 < 0)
        {
            write(STDERR_FILENO, "Error opening file \n", 20); 
            return FAILURE;
        }
        /** lseek finds the end of the file and 
        // lets us know the entire filelength 
        This prevents us from having to loop through the
        file or guess at buffer lengths
        */
        //go to end of file
        fileLength = lseek(file1, 0, SEEK_END);
        char buffer[fileLength];
        // go to beginning of file
        lseek(file1, 0, SEEK_SET);
        if(read(file1, buffer, fileLength))
        {
        	// write to the second file
        	write(file2, buffer, fileLength);
        }
        close(file1);
        close(file2);
        return SUCCESS;
    }
}

// Implement "fg" function
int file_fg(char** args, int cnt)
{
	int w;
	int process_found;
	if(cnt == 2)
	{
		//correct format for fg command
		// first remove the process from the stack
		int w;
		int num_process;
		num_process = count_processes();
		// convert to int from args string
		pid_t process_id = (pid_t) ((int) args[1]);

		//make sure process exists in background:
		if(num_process)
		{

			process_found = pop_process_from_stack(process_id);
			if(process_found == SUCCESS)
			{	
				// process is now foreground process
				w = waitpid(process_id, NULL, 0);
				if (w == -1)
				{
		            return FAILURE;
		        }
		        else
		        	return SUCCESS;
		    }
		    else{
		    	printf("No process with the specified process ID found in background\n");
		    	return FAILURE;
		    }
		}

    	else
    	{
    		printf( "No background processes at this time\n");
    		return SUCCESS;
    	}
	}
	else{
		printf("Error executing fg command, no process specified. Format should be \" fg <pid> \" \n");
		return FAILURE;
	}
}

void file_jobs()
{
	int num_process;
	num_process = count_processes();
	process_node* temp_node_to_free = malloc(sizeof(process_node));
	process_node* temp_node = temp_node_to_free;
	process_node* temp_next;
	temp_node = head_process_node;

	//Check to make sure there are jobs in the background
	// Traverse the stack to print all jobs in background
	if(num_process)
	{
		
		printf("Printing all jobs in background\n");
		while(temp_node != NULL)
		{	
			// Check that a process is done here, if a process is done
			// right before jobs executes, it will show as "Jobs done"
			if(waitpid(temp_node->process_id,NULL,WNOHANG))
			{
				temp_next = temp_node->next_process; //so we can reassign temp_node after to step through the stack
				printf("Job done\t:Process ID\t%d\t%s\t%s%s", (int) temp_node->process_id,"Process commands", temp_node->process_cmd[0],"\n");
				pop_process_from_stack(temp_node->process_id);
				temp_node = temp_next;
			}
			else
			{
				printf("Jobs running in background\t:Process ID\t%d\t%s\t%s%s", (int) temp_node->process_id,"Process commands", temp_node->process_cmd[0],"\n");
				temp_node = temp_node->next_process;
			}
			
		
		}
	}
	else
	{
		printf("No jobs in background\n");
	}
	free(temp_node_to_free);

}
// Function to determine if a job has been done
// At each iteration of the shell
// Removed completed jobs from the queue and displays them
// The flag is SUCCESS when exit calls it (to indicate that
// all background jobs will terminate)
void check_bg_done(int flag) // the flag determines if exit is calling this
{
	int count;
	int i;
	process_node* temp_node_to_free = malloc(sizeof(process_node));
	process_node* temp_node = temp_node_to_free;
	process_node* temp_next;
	temp_node = head_process_node;
	count = count_processes();
	for(i = 0; i < count; i++)
	{
	// Check that a process is done here
		temp_next = temp_node->next_process; //so we can reassign temp_node after to step through the stack
		if(waitpid(temp_node->process_id,NULL,WNOHANG))
		{
			printf("\nJob done\t:Process ID\t%d\t%s\t%s%s", (int) temp_node->process_id,"Process commands", temp_node->process_cmd[0],"\n");
			pop_process_from_stack(temp_node->process_id);
			temp_node = temp_next;
		}
		else if(flag == SUCCESS)
		{
			printf("\nJobs running in background, will be terminated \t:Process ID\t%d\t%s\t%s%s", (int) temp_node->process_id,"Process commands", temp_node->process_cmd[0],"\n");
			pop_process_from_stack(temp_node->process_id);
			temp_node = temp_next;
		}
		else{
			temp_node = temp_node->next_process;
		}
	}
	free(temp_node_to_free);

}
// signal handling function
static void sigHandler(int sig)
{
	
	if(sig == SIGINT && !bg && ! (int) (process_child_id))
	{
		fprintf(stderr,"Termination signal received from keyboard\n");
		// Kill the foreground child process by exiting it
		exit(EXIT_SUCCESS);
	}
	else if(sig == SIGTSTP)
	{
		fprintf(stderr, "Ignoring keyboard signal CTRL-Z, please press ENTER key \n");
		//do nothing
	}
}


//Main function
int main(void)
{
    char* args[MAX_OPTIONS];
    int w, redirect,redir_fd;
    pid_t process_parent_id=getpid(); //current process ID is the parent ID
    head_process_node = NULL;
    int loop_count; // for the for loop to free args at the end
    /* Signal handling here for SIGINT and SIGTSTP
    */
    if (signal(SIGINT, sigHandler) == SIG_ERR || signal(SIGTSTP, sigHandler))
    {
		printf("Error! Could not bind signal handler\n!");
		exit(EXIT_FAILURE);
	}
    while(1) 
    {
        bg = 0;
        redir_fd = 1;
        // check jobs that have been done already
        check_bg_done(FAILURE);
        int cnt = getcmd("\n>> ", args, &bg);
        time_t now;
		srand((unsigned int) (time(&now)));
    	// if no token entered, continue
        if(cnt > 0)
        {
            
	        // Fork a child process to execute the commands
	        // fg, jobs and exit are always shell processes
	        // check output redirection and set write flag accordingly
	        // The redirect flag also holds the position where the redirection command
	        // appears in args
	        redirect = 0;
        	for (loop_count = 0; loop_count < cnt; loop_count ++)
        	{
        		if(!strcmp(args[loop_count],">"))
        		{
        			redirect = loop_count;
        		}
        	}
        	
	        
	        process_child_id = fork();
	        if(process_child_id < 0)
	        {
	            printf("Error in forking new process, terminating...\n");
	            free(args[0]);
	            exit(EXIT_SUCCESS);
	            continue;
	        }
	        else if(!process_child_id){
	            // This is the child process
	            // For the child process, fork always returns zero (it returns <-1 if fork fails and +ve to the parent)
	            // Take care of output redirection first
	            if(redirect)
            	{
            		close(1);
            		redir_fd = open(args[redirect + 1], O_RDWR | O_APPEND | O_CREAT,0777);
            	}
            	// Execute custom jobs
		        if(!strcmp("ls", args[0]))
		        {
		            //execute ls function here
		           if(bg)
		        	{
		        		int w,rem;
						w = rand() % 10;
						rem=sleep(w);
						//handles interruption by signal
						while(rem!=0)
						 {
						 rem=sleep(rem);
						 }
		        	}
		            file_display(args, cnt, redirect);
		        }
		        else if(!strcmp("cd", args[0]))
		        {
		        	/**"cd" has to be done in the parent process to change
		        	the parent directory
		        	*/
		        	if(bg)
		        	{
		        		int w,rem;
						w = rand() % 10;
						rem=sleep(w);
						//handles interruption by signal
						while(rem!=0)
						 {
						 rem=sleep(rem);
						 }
		        	}
		            change_dir(args, cnt);
		            //continue;
		        }
		        else if(!strcmp("jobs", args[0]))
		        {
		        	// do nothing
		        }
		                
		        else if(!strcmp("cat", args[0])){
		        	if(bg)
		        	{
		        		int w,rem;
						w = rand() % 10;
						rem=sleep(w);
						//handles interruption by signal
						while(rem!=0)
						 {
						 rem=sleep(rem);
						 }
		        	}
		            cat(args, cnt);
		            //continue;
		        }

		        else if(!strcmp("cp", args[0])){
		        	if(bg)
		        	{
		        		int w,rem;
						w = rand() % 10;
						rem=sleep(w);
						//handles interruption by signal
						while(rem!=0)
						 {
						 rem=sleep(rem);
						 }
		        	}
		            file_copy(args, cnt);
		            //continue;
		        }
		        else if(!strcmp("fg", args[0])){
		        	// do nothing here, fg happens only in background
		        }
		        
		        else if(!strcmp("exit", args[0]) || !strcmp("quit",args[0]) ||
		            !strcmp("quit()",args[0]) || !strcmp("exit()",args[0]))
		        {
		            // Exit process, not shell
		            
		            free(args[0]);
		            exit(EXIT_SUCCESS);
		            //continue;
		        }
		        
	            else{
	            	
	            	args[cnt++] = NULL;
					if(bg)
		            {
			            int w;
						w = rand() % 10;
						sleep(w);
					}
	            	if(execvp(args[0], args) == -1)
	            	{
	                	printf("Error in executing command...\n");
	                	exit(EXIT_FAILURE);
	                	//continue;
	            	}
	            }
	            // Close the redirection file, and transfer STDOUT to console
	            if(redirect)
            	{
            		close(redir_fd);
            	}
            	// exit the child process after command is done
	            exit(EXIT_SUCCESS);
	            
	            // else{
	            //     //execvp worked, do nothing else
	                    // 
	            // }
	        }

	        else
	        {
	            //This is the parent process
	           
	            if(bg && strcmp("jobs", args[0]) && strcmp("fg", args[0])){
	                //run process in background
	                // cannot run fg or  jobs in a child process
	                push_process_to_stack(args, process_child_id, cnt);
	
	                kill(process_parent_id, SIGCONT);
	               
	            }
	            
	            else
	            {
	                //wait for child process foreground to finish, 
	                w = waitpid(process_child_id, NULL , 0);
	                if (w == -1){
	                    printf("Error in waiting for child method");
	                    continue;
	                }
	            }
	            // in case the command is cd, need to implement in parent as well
	            if(!strcmp("cd", args[0]))
		        {
		            change_dir(args, cnt);
		        }
		        // command to quit the shell
		        if(!strcmp("exit", args[0]) || !strcmp("quit",args[0]) ||
		            !strcmp("quit()",args[0]) || !strcmp("exit()",args[0]))
		        {
		            check_bg_done(SUCCESS);
		            free(args[0]);
		            exit_shell();
		        }
		    	// fg has to go outside the shell to avoid IPC
			    else if(!strcmp("fg", args[0]))
			    {
			        file_fg(args, cnt);			        
			    }
			    else if(!strcmp("jobs", args[0]))
			    {
		        	file_jobs();
			    }
	            
	        }
	        // re-initialize args to all null
	        // and free the head
	        free(args[0]);
	        for(loop_count = 0; loop_count < cnt; loop_count++)
	        {
	        	args[loop_count] = NULL;
	        } 
	 	}      
    } //end while

     /* the steps can be..:
	    (1) fork a child process using fork()
	    (2) the child process will invoke execvp()
	    (3) if background is not specified, the parent will wait,
	    otherwise parent starts the next command... */
} //end main
