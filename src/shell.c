#include "parser.h"

#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <stdbool.h>


#define READ  0
#define WRITE 1




void fork_error() {
  perror("fork() failed)");
  exit(EXIT_FAILURE);
}


void fork_cmd(char* argv[]) {
  pid_t pid;

  switch (pid = fork()) {
    case -1:
      fork_error();
    case 0:
      execvp(argv[0], argv);
      perror("execvp");
      exit(EXIT_FAILURE);
    default:
      break;
  }
}

void fork_cmds(char* argvs[MAX_COMMANDS][MAX_ARGV], int n) {
// For storing the parents stdin & stdout
   int temporaryIn = dup(READ);
   int temporaryOut = dup(WRITE);
   int pipefds[2];
   
   //These varibles are used to temporary store file descriptors.
   int fdin = dup(temporaryIn); 
   int fdout = dup(temporaryOut);
  

    for (int i = 0; i < n; i++) {
        dup2(fdin, READ);
        //close(fdin);
        
       
       if(i == n-1){ // If last command redirect its output to the standard output of parrent process
       fdout= dup(temporaryOut);
       }
                          
       
        else{ // If not last command, create pipe for communication.
        if(pipe(pipefds)==-1){
        perror("PIPE\n");
         exit(EXIT_FAILURE); 
        };

       // fdout is now reffereing to the write end of the pipe
       fdout=pipefds[WRITE];
       //fdin is now reffereing to the read end of the pipe
       fdin=pipefds[READ];
       }

      //Redirect the standard output to the write end of the pipe
      dup2(fdout,WRITE);
        
      close(fdout);

    // Fork a new child
    fork_cmd(argvs[i]);
  }


    //Return the original stdin & stdout to the parrent.
    dup2(temporaryIn,READ);
    dup2(temporaryOut,WRITE);
    close(temporaryIn);
    close(temporaryOut); 

} 

void get_line(char* buffer, size_t size) {
        
if(  getline(&buffer, &size, stdin)== -1){
     printf("ERROR\n");
     exit(1);};
 


 buffer[strlen(buffer)-1] = '\0';
}

void wait_for_all_cmds(int n) {
  // Not implemented yet!

  for (int i = 0; i < n; i++) {
   printf("%i\n", wait(NULL));
  }
}

int main() {
  int n;
  char* argvs[MAX_COMMANDS][MAX_ARGV];
  size_t size = 128;
  char line[size];

  while(true) {
    printf(" >> ");
    get_line(line, size);

    n = parse(line, argvs);

    // Debug printouts.
    printf("%d commands parsed.\n", n);
    print_argvs(argvs);

    fork_cmds(argvs, n);
    wait_for_all_cmds(n);
  }

  exit(EXIT_SUCCESS);
}


