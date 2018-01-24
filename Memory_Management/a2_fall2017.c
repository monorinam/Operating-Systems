/**
Memory Management Assignment: OS
*/

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <semaphore.h>
#include <time.h>

#define FAILURE 1
#define SUCCESS 0
#define MAX_OPTIONS 50
#define TOTALTABLES 20
#define AVAILABLE 1
#define UNAVAILABLE 2
#define MAX_NAME_LENGTH 1024
// Struct to store the reservations
typedef struct reservation{
char name[TOTALTABLES][MAX_NAME_LENGTH];    // Names of persons getting tables, is NULL when table is available
int table_no[TOTALTABLES];  // Goes from 100 to 110 and then 200 to 210
char section[TOTALTABLES]; // The section (slightly redundant since we can get from the table #)
int status[TOTALTABLES]; //"AVAILABLE or UNAVAILABLE"

}reservation; 
reservation* reserve_db;
sem_t *mutex_monorina;  // controls access to rc (the reader count)
sem_t *db_monorina;    // controls access to database
sem_t *order_monorina; // this is the mutex to remember order of requests for read/write
			 // this does fifo service for both readers and writers
			// to stop the writers from starving if an endless stream of readers arrive
int *rc_monorina; // the reader count
//int initdone = 0; // Indicates if the db is already initialized or not, also controlled by mutex
//Shared memory functions

//Command functions
//Initialize the database
// Create the shared memory (or read it)
//
int init_database()
{
	//initialize the shared memory
	int shm_fd, shm_rc;
	char *name = "ReservationDBMM";
	char *name_rc = "ReaderCountSharedMonorina";

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

	reserve_db = mmap(0,sizeof(reservation), PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
	if (reserve_db == MAP_FAILED) {
		printf("Map failed\n");
		exit(-1);
	}
	// open the readercount shared memory
	shm_rc = shm_open(name_rc, O_CREAT | O_RDWR, 0666);
	if(shm_rc < 0)
	{
		printf("Error in creating or opening shared memory");
		exit(EXIT_FAILURE);
	}
	ftruncate(shm_rc, sizeof(int));
	//map shared memory
	rc_monorina = mmap(0,sizeof(int), PROT_READ| PROT_WRITE, MAP_SHARED, shm_rc, 0);
	if(rc_monorina == MAP_FAILED)
	{
		printf("Map failed \n");
		exit(EXIT_FAILURE);
	}
	//Unlink mutexes to ensure
	// crashed code does not affect this
	sem_unlink("mutex_monorina");
	sem_unlink("db_monorina");
	sem_unlink("order_monorina");


	// Create mutex and db variables to control access to shared memory
	mutex_monorina = sem_open("mutex_monorina",O_CREAT,0666,1);
	db_monorina = sem_open("db_monorina",O_CREAT,0666,1);
	order_monorina = sem_open("order_monorina", O_CREAT,0666,1);
	// Now initialize the database
	//down db
	sem_wait(order_monorina);
	sem_wait(db_monorina);
	sem_post(order_monorina); // signal the order mutex since this write has been served and has db access
	/* DB ACCESS TO INIT
	*/
    printf("Initializing database.....\n");
	for(int i = 0;i<TOTALTABLES;i++)
	{
		strcpy(reserve_db->name[i] ,"INITIALIZED");
		reserve_db->status[i] = AVAILABLE;
		if(i<10){
			reserve_db->table_no[i] = 100+(i+1);
			reserve_db->section[i] = 'A';
		}
		else{
			reserve_db->table_no[i]= 200+(i-10 + 1);
			reserve_db->section[i] = 'B';
		}
	}
	sem_post(db_monorina);
	return SUCCESS;

	//up db

}
//Write the the database
int reserve(char **args, int cnt)
{
	//Check to see arguments are valid
	int table;
	int section;
	int table_flag = 0;
    time_t rawtime;
    struct tm * timeinfo;
	if(cnt !=3 && cnt != 4)
	{
		printf("Wrong arguments, please use form reserve <name> <section> <table number(optional)> ");
		return FAILURE;
	}
	else
	{
		//Protected read
		
		if(!strcmp(args[2],"A"))
			section = 1;
		else
			section = 2;
		if(cnt == 4)
		{
			table = atoi(args[3]);
			//Check if the table being requested is a valid table
			if(!((table>=101 && table <= 110) || (table >= 201 && table <= 210)))
			{
				printf("Table does not exist, please request again with a valid table number \n");
				//free(arg_copy);
				return FAILURE;
			}
		}
		else
			table = 0;
		
		//Write region
		sem_wait(order_monorina);
		sem_wait(db_monorina);
		sem_post(order_monorina);
        time(&rawtime);
        timeinfo = localtime (&rawtime);
        //printf("Lock Accessed at time : %s ",asctime(timeinfo));
        int w,rem;
        w = rand() % 10;
        rem = sleep(w);
		for(int i = (section-1)*10; i < section*10; i++)
		{
			//check if any table in section is available
			if(table == 0 && reserve_db->status[i] == AVAILABLE)
			{
				reserve_db->status[i] = UNAVAILABLE;
				strcpy(reserve_db->name[i],args[1]);
				table = reserve_db->table_no[i];
				table_flag = 1;
				break;
			}
			else if (reserve_db->table_no[i] == table && reserve_db->status[i] == AVAILABLE)
			{
				reserve_db->status[i] = UNAVAILABLE;
				strcpy(reserve_db->name[i],args[1]);
				table_flag = 1;
				break;
			}
		}
		sem_post(db_monorina);
		if(table_flag == 0)
		{
			printf("The table is unavailable, cannot be reserved\n");
			return FAILURE;
		}
		printf("The table %d was reserved for %s \n",table, args[1]);
		return SUCCESS;

	}
}
int status()
{
 // This is the reader function
 // Multiple readers can read from the same database
	//reservation reserve_temp = malloc(sizeof(reservation));
	char* section;
	sem_wait(order_monorina); //wait in queue to access reading
	sem_wait(mutex_monorina); // get access to rc
	if (*rc_monorina == 0)
		sem_wait(db_monorina);
	*rc_monorina += 1;
	sem_post(order_monorina); //now have access to db so get out of queue
	sem_post(mutex_monorina); // give up access to rc since done updating
	//Read database
    time_t rawtime;
    struct tm * timeinfo;
    time(&rawtime);
    timeinfo = localtime(&rawtime);
    //printf("Lock accessed at time : %s",asctime(timeinfo));
    int w,rem;
    w = rand() % 10;
    rem = sleep(w);
	// Ask: should I do a memcpy rather than directly reading memcpy(reserve_temp,reserve_db,)
	for(int i = 0; i<TOTALTABLES;i++)
	{
		if(reserve_db->status[i]==AVAILABLE)
			printf("Table %d in section %c is available\n",reserve_db->table_no[i],reserve_db->section[i]);
		else if(reserve_db->status[i] == UNAVAILABLE)
			printf("Table %d in section %c is already reserved by %s  \n", reserve_db->table_no[i],reserve_db->section[i], reserve_db->name[i]);
	}
	sem_wait(mutex_monorina); // need to change rc again
	// Since this reader is done
	*rc_monorina = *rc_monorina - 1;
	if(*rc_monorina == 0)  // all readers are done so give up db access
		sem_post(db_monorina);
	sem_post(mutex_monorina); //updated rc done
	return SUCCESS;



}

// This function is a modified version of the getcmd function
// given to us in Assignment 1, COMP 310 2017.
//Here it either reads from the command line or from the file with given fileID
// If fileflag is 0, it reads from CLI, else from file with FileID
int getcmd(char *args[], int fileflag, FILE *fileID)
{

	char *line = NULL;
	size_t linecap = 0;
	int i = 0;
	char *token;
	//read from CLI
	if(fileflag == 0)
	{
		printf(">>");
		if(getline(&line,&linecap,stdin) <= 0)
		{
			exit(-1);
		}	
	}
	else //read from file
	{
		if(getline(&line,&linecap,fileID) <= 0)
		{
			free(line);
			return -1;
		}
	}
	char *l = line;
    while ((token = strsep(&line, " \t\n")) != NULL) {
        for (int j = 0; j < strlen(token); j++)
            if (token[j] <= 32)
                token[j] = '\0';
        if (strlen(token) > 0)
            args[i++] = token;
    }
    //if(i == 0) {
    if(i == 0)
    {
    	free(l);
    }
    free(line);
    //free(fileID);
    //}
    return i;
}
//This method parses the sentences read from the CLI or file, and calls
// the required function (init, reserve status) If the shared memeory is not
// open, it opens the shared memory (memopen_flag indicates whether SHM is open or not)
int parse_sentence(int memopen_flag, char *args[], int cnt)
{
	
	char *name = "ReservationDBMM";
	char *name_rc = "ReaderCountSharedMonorina";//"
	// if this is the first run of this loop
	// for the process and if the first command
	// is not init, then open the memory map for
	if(!strcmp(args[0],"init"))
		init_database();
	else if(memopen_flag == 0)  //if command is not init and the shared memory is not open
	{
		int shm_fd, shm_rc;
		
		//sem_wait(db);
		shm_fd = shm_open(name, O_RDWR, 0666);
		if (shm_fd == -1) {
			printf("Database cannot be accessed\n");
			exit(-1);
		}

		/* now map the shared memory segment in the address space of the process */
		reserve_db = mmap(0,sizeof(reservation), PROT_READ|PROT_WRITE, MAP_SHARED, shm_fd, 0);
		if (reserve_db == MAP_FAILED) {
			printf("Database cannot be accessed \n");
			exit(-1);
		}
		
		shm_rc = shm_open(name_rc, O_RDWR, 0666);
		if (shm_rc == -1) {
			printf("Reader count cannot be accessed\n");
			exit(-1);
		}

		/* now map the shared memory segment in the address space of the process */
		rc_monorina = mmap(0,sizeof(int), PROT_READ|PROT_WRITE, MAP_SHARED, shm_rc, 0);
		if (rc_monorina == MAP_FAILED) {
			printf("reader count cannot be accessed \n");
			exit(-1);
		}


		mutex_monorina = sem_open("mutex_monorina",O_RDWR,0666,1);
		db_monorina = sem_open("db_monorina",O_RDWR,0666,1);
		order_monorina = sem_open("order_monorina",O_RDWR,0666,1);
		//sem_post(db);
		
	}
	// Call the required functions for the command given
	if(!strcmp(args[0],"reserve"))
		reserve(args, cnt);
	else if(!strcmp(args[0],"status"))
		status();
	else if(!strcmp(args[0],"exit"))
	{	
		printf("Exiting..\n");
		exit(EXIT_SUCCESS);
	}
	return SUCCESS;

}
// The main function
int main(int argc, char *argv[])
{
	int memopen_flag = 0;
	int fileflag = 0;
	FILE *fileID;
	//File reading
	if(argc > 1)
	{
		// file reading for getcmd
		fileflag = 1;
		//find number of lines in file
		fileID = fopen(argv[1],"r");
		//TODO:ERROR CHECKING
	} //end of file reading if
	
	//This is the command line interface 
	// infinite loop for reservation commands
	while(1)
	{
		// Get command here
		char *args[MAX_OPTIONS];
		int cnt;
        time_t now;
        srand((unsigned int) (time(&now)));
        //if file is given as argument, open the file to getcmd
		if(fileflag == 0)
			cnt = getcmd(args, fileflag , NULL);
		else
			cnt = getcmd(args, fileflag, fileID);
		if(cnt == -1 && argc > 1)
		{
			// file is done reading
			printf("Finished parsing file commands \n Exiting .... \n");
			exit(SUCCESS);
		}

		if(cnt >= 1)
		{
			parse_sentence(memopen_flag,args, cnt);
			if(memopen_flag == 0)
				memopen_flag = 1;
		} //end if if (cnt)
		free(args[0]);
		for (int i = 0; i < cnt; i++)
		{
			args[i] = NULL;
		}

	} //end of while
 //end of if
} //end of main
