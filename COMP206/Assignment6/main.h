#ifndef MAINDEF
#define MAINDEF
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>
#define SUCCESS 1
#define FAILURE -1
extern int producer();
extern int consumer();
#endif
