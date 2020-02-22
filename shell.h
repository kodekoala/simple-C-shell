/*
  HW1 Shell Header File
*/


#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <limits.h>
#include <string.h>
#include <sys/wait.h>
#include "utils.h"
#include <ctype.h>
#include <pwd.h>
#include <sys/types.h>
#include <sys/types.h>

#define DELIMS " \n\r"
void execArgs(char** args, FILE* stream, char* line);
void executeFile(char* fileName);
char** parseLine(char*);
void clearMem(char** args);
