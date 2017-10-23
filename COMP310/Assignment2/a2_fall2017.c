/**
Assignment #2
COMP 310: Operating Systems
Name: Monorina Mukhopadhyay (260364335)
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <sys/mman.h>
#define FAILURE 1
#define SUCCESS 0
#define MAX_OPTIONS 50
#define TOTALTABLES 20
#define AVAILABLE 1
#define UNAVAILABLE 2
// Struct to store the reservations
typedef struct reservation{
char *name[TOTALTABLES];    // Names of persons getting tables, is NULL when table is available
int table_no[TOTALTABLES];  // Goes from 100 to 110 and then 200 to 210
int status[TOTALTABLES]; //"AVAILABLE or UNAVAILABLE"

}reservation;
sem_t mutex;
sem_t db;
int rc = 0;
int wc = 0;
//Shared memory functions

//Command functions
//Initialize the database
// Create the shared memory (or read it)
//
int init_database()
{
	//initialize the shared memory
	int shm_fd;
	char *name = "ReservationDBMM";

	/* create the shared memory segment */
	shm_fd = shm_open(name, O_CREAT | O_RDWR, 0666); //Should it be just O_CREAT?
	if(shm_fd < 0)
	{
		printf("Error in creating or opening shared memory..");
		return FAILURE;
	}

	/* configure the size of the shared memory segment */
	ftruncate(shm_fd,sizeof(reservation));

	// Map shared memory into address space of process

	reserve_db_ptr = mmap(0,sizeof(reservation), PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
	if (reserve_db_ptr == MAP_FAILED) {
		printf("Map failed\n");
		exit(-1);
	}

	// Create mutex and db variables to control access to shared memory
	mutex = sem_open("mutex",O_CREAT,0666,1);
	db = sem_open("db",O_CREAT,0666,1);

	// Now initialize the database
	//down db
	sem_wait(db);
	printf("Initializing database.....\n");
	for(int i = 0;i<TOTALTABLES;i++)
	{
		reservation->name[i] = NULL;
		reservation->status[i] = AVAILABLE;
		if(i<10){
			reservation->table_no[i] = 100+(i+1);
		}
		else{
			reservation->table_no[i]= 200+(i-10 + 1);
		}
	}
	sem_post(db);
	return SUCCESS;
	//up db

}
//Write the the database
int reserve(char *args[], int cnt)
{
	//Check to see arguments are valid
	char* name;
	int table;
	int section;
	int table_flag = 0;
	if(cnt !=3 && cnt != 4)
	{
		printf("Wrong arguments, please use form reserve <name> <section> <table number(optional)> ");
		return FAILURE;
	}
	else
	{
		//Protected read
		// de-reference the cmd pointer to prevent the command name in the linked list
		// from being overwritten
		// This segment is reused from Assignment 1 submission shell.c
		char** arg_copy;
		arg_copy = malloc(cmd_length*sizeof(args));
		for(int i = 0; i < cmd_length; i++)
		{
			arg_copy[i] = strdup(args[i]);
		}
		name = arg_copy[1];
		if(!strcmp(arg_copy[2],"A"))
			section = 1;
		else
			section = 2;
		if(cnt == 4)
		{
			table = section*100 + atoi(arg_copy[3]);
		}
		else
			table = 0;
		//Write region
		sem_wait(db);
		for(int i = (section-1)*10;i<section*10;i++)
		{
			//check if any table in section is available
			if(table = 0 && reservation->status[i] = AVAILABLE)
			{
				reservation->name[i] = name;
				reservation->status = UNAVAILABLE;
				table = reservation->table_no[i];
				table_flag = 1;
				break;
			}
			else if (reservation->table_no[i] = table)
			{
				reservation->name[i] = name;
				reservation->status = UNAVAILABLE;
				table_flag = 1;
				break;
			}
		}
		sem_post(db);
		if(table_flag == 0)
		{
			printf("The table is unavailable, cannot be reserved\n Please enter another table number or section to reserve table \n");
			return FAILURE;
		}
		printf("The table was reserved for you \n");
		return SUCCESS;

	}
}
int status(char *args[])
{
	
}
// This function is a modified version of the getcmd function
// given to us in Assignment 1, COMP 310 2017.
int getcmd(char *args[])
{
	char *line = NULL;
	size_t linecap = 0;
	printf(">>");
	if(getline(&line,&linecap,stdin) <= 0)
	{
		exit(-1);
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
int main(void)
{
	pid_t process_child_id;
	while(1)
	{
		// Get command here
		char *args[MAX_OPTIONS];
		int cnt;
		cnt = getcmd(args);
		if(cnt >= 1)
		{
			if(!strcmp(args[0],"init"))
				init_database();
			else if(!strcmp(args[0],"reserve"))
				reserve(args);
			else if(!strcmp(args[0],"status"))
				show_status(args);
			else if(!strcmp(args[0],"exit"))
			{	
				printf("Exiting..\n");
				exit(EXIT_SUCCESS);
			}
		}
	}
}