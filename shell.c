/*
  CMSC 421
  Homework 1 - Simple Shell
  Author: Yousuf Asfari
*/

#include "shell.h"

char** parseLine(char* line){
  int index = 0;
  int firstSpace = 0;
  int buffSize = 2;
  //int newBuffSize = buffSize;
  char** argArr = malloc(buffSize * sizeof(char*));


  for (int i=0; i<buffSize; i++)
  {
    argArr[i] = NULL;
  }

  do {

    //Get the index for the first space
    firstSpace = first_unquoted_space(line);
    //Replace the space with \0
    line[firstSpace] = '\0';
    //Place the string with the endline taken out in the char** array
    //This array is the argument array that will be passed to exec
    argArr[index] = unescape(line,stderr);
    //Proceed to the next argument in the command
    line += firstSpace+1;
    //Increment argument array index counter
    index++;

    //We have to resize the token array buffer size

    buffSize += 1;
    argArr = realloc(argArr,buffSize*sizeof(char*));
    if(!argArr){
      fprintf(stderr, "Allocation Error\n");
      exit(EXIT_FAILURE);
    }

    argArr[buffSize - 1] = NULL;
    
    
  } while(first_unquoted_space(line) != -1);  //While we still have arguments in the string 

  return argArr;
}

void executeFile(char* fileName){

  FILE *stream;
  char *line = NULL;
  size_t len = 0;
  char **tokenArr;

   stream = fopen(fileName, "r");
   if (stream == NULL) {
      fprintf(stderr, "Could not open file\n" );
      fclose(stream);
      exit(1);
   }

   while ((getline(&line, &len, stream)) != -1) {

      tokenArr = parseLine(line);
      
      if (strcmp(tokenArr[0], "exit") == 0){      /* is it an "exit"?     */
          fclose(stream);
          if (tokenArr[1] == NULL){
            free(line);
            clearMem(tokenArr);
            exit(0);
          }
          free(line);
          clearMem(tokenArr);
          exit(1);                      
      } 

      execArgs(tokenArr, stream, line);
    }
    free(line);
    fclose(stream);

    return;
}

// Function where the system command is executed 
void execArgs(char** args, FILE* stream, char* line) 
{ 
    int status, numbytes, fd[2];
    char    readbuffer[7];
    char pipedString[] = "failed";
    pipe(fd);
    
    // Forking a child 
    pid_t pid = fork();  
  
    if (pid == -1) { 
        fprintf(stderr, "Error: forking process failed\n");
        if (stream != NULL){
          fclose(stream);
        }
        exit(1); 
    } 

    else if (pid == 0) {      
      if (execvp(args[0], args) < 0) { 
          close(fd[0]);
          write(fd[1], pipedString, (strlen(pipedString)+1));
          exit(1);
      } 
    } 
    else { 
        // waiting for child to terminate 
        wait(&status);  

        close(fd[1]);
        numbytes = read(fd[0], &readbuffer, sizeof(readbuffer));
        if (numbytes > 0 && strcmp(readbuffer, pipedString) == 0){
            fprintf(stderr, "Error: unable to run command\n");
            clearMem(args);
            if (stream != NULL){
              free(line);
              fclose(stream);
              exit(1);
            }     
            return;
        }
    } 

    clearMem(args);
    return;
}

void clearMem(char** args){
    int i = 0;
    while(args[i] != NULL){
      free(args[i]);
      i++;
    }
    free(args);
    return;
}

int main(int argc,char **argv){
  char* command;

  if (argc == 1){
    char *entries = NULL;
    char **tokenArr = NULL;
    size_t entriesLen = 0;
    FILE* dummy = NULL;
    while(1){
          printf("HW1 Shell -> ");     //   display prompt             

          //Deal with the input
          getline(&entries, &entriesLen, stdin);
          //printf("\n");
          tokenArr = parseLine(entries);
      
          if (strcmp(tokenArr[0], "exit") == 0 && tokenArr[1] == NULL){      /* is it an "exit"?     */
              
              free(entries);
              clearMem(tokenArr);
              exit(0);
                          
          }
          else{ 
            execArgs(tokenArr, dummy, entries);
          }
          free(entries);
         
          entries = NULL;
          tokenArr = NULL;
          
    }
  }
  else if (argc == 2){
    int returnValue;
    int argLen = strlen(argv[1]) + 1;
    char argument[argLen];
    strcpy(argument, argv[1]);
    command = argument;
    executeFile(command);
    exit(0);
  }
  //More than 1 argument, we print to stderr and exit
  else{
    fprintf(stderr, "%s", "Only 0 or 1 command line arguments are allowed!\n" );
    exit(1);
  }

  return 0;
}

