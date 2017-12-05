#ifndef MAINDEF
#define MAINDEF
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>
#define SUCCESS 1
#define FAILURE -1
#define TURNP '0'
#define TURNC '1'
#define TURNFAIL -3
extern int producer(void);
extern int consumer(void);
#endif
