#include <unistd.h>
#include <string>
#include <vector>
#include <cstdlib>
#include <stdio.h>
#include <iostream>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdlib.h>
using namespace std;

#ifndef COMMAND_LINE
#define COMMAND_LINE

struct cmdLine
{
 char** arguments;
 cmdLine * next;
};
#endif

void runPipedCommands(cmdLine* command, char* userInput);
int  countPipes(char * userInput);
void  sigtstp_handler(int sig);
//const std::vector<char*> stringToCharVector(std::vector<cmdLine>::iterator & it);

int main(int argc, char *argv[])
{

 cout<<"IN MAIN(): does this work??"<<endl;
 string userInput = "ls -l | tr a-z A-Z"; //used to count the pipe characters ('|').

    cmdLine * cmdStruct = new cmdLine;

    cmdStruct->arguments = (char**)malloc(sizeof *cmdStruct->arguments * 3);
    if (cmdStruct->arguments)
    {
    	cmdStruct->arguments[0] =(char*)malloc(sizeof *cmdStruct->arguments[0] * 8);
        strcpy(cmdStruct->arguments[0], "/bin/ls");

    	cmdStruct->arguments[1] = (char*)malloc(sizeof *cmdStruct->arguments[1] * 3);
        strcpy(cmdStruct->arguments[1], "-l");

    	cmdStruct->arguments[2] = NULL;
    }

	cmdLine * cmdStruct2 = new cmdLine;
	cmdStruct->next = cmdStruct2;

    cmdStruct2->arguments = (char**)malloc(sizeof *cmdStruct2->arguments * 3);
     if (cmdStruct2->arguments)
     {
     	cmdStruct2->arguments[0] =(char*) malloc(sizeof *cmdStruct2->arguments[0]  * 12);
        strcpy(cmdStruct2->arguments[0], "/usr/bin/tr");

     	cmdStruct2->arguments[1] = (char*)malloc(sizeof *cmdStruct2->arguments[1] * 4);
        strcpy(cmdStruct2->arguments[1], "a-z");

      	cmdStruct2->arguments[2] =(char*) malloc(sizeof *cmdStruct2->arguments[2] * 4);
        strcpy(cmdStruct2->arguments[2], "A-Z");

     	cmdStruct2->arguments[3] = NULL;
     }

    runPipedCommands( cmdStruct,(char *)userInput.c_str());

    cout<<"free memory for first array";

//    for(int i = 0; i < 2 ; i++){
//
//    	delete[] cmdStruct->arguments[i];
//    	cmdStruct->arguments[i] = NULL;
//    }
//
//
//
//    delete[]cmdStruct->arguments;
//    cmdStruct->arguments = NULL;
//
////    free(cmdStruct->next);
////    cmdStruct->next = NULL;
//
//    delete []cmdStruct;
//    cmdStruct = NULL;
//
//    cout<<"free memory for second array";
//    for(int i = 0; i < 3 ; i++){
//    	delete [] cmdStruct2->arguments[i];
//    	cmdStruct2->arguments[i] = NULL;
//    }
//
//    delete []cmdStruct2->arguments;
//    cmdStruct2->arguments = NULL;
//
//    delete cmdStruct2;
//    cmdStruct2 = NULL;

}


void runPipedCommands(cmdLine* command, char* userInput) {

	int numPipes = countPipes(userInput);
    int status;
    int i = 0;
    pid_t pid;

    int pipefds[2*numPipes];

    for(i = 0; i < (numPipes); i++){
        if(pipe(pipefds + i*2) < 0) {
            perror("couldn't pipe");
            exit(EXIT_FAILURE);
        }
    }

    int j = 0;
    while(command) {
        pid = fork();
        if(pid == 0)
        {

            //if not last command
            if(command->next)
            {
                if(dup2(pipefds[j + 1], 1) < 0)
                {
                    perror("dup2");
                    exit(EXIT_FAILURE);
                }
            }

            //if not first command&& j!= 2*numPipes
            if(j != 0 ){
                if(dup2(pipefds[j-2], 0) < 0){
                    perror(" dup2");///j-2 0 j+1 1
                    exit(EXIT_FAILURE);

                }
            }


            for(i = 0; i < 2*numPipes; i++){
                    close(pipefds[i]);
            }

            if( execvp(*command->arguments, command->arguments) < 0 )
            {
                    perror(*command->arguments);
                    exit(EXIT_FAILURE);
            }

        } else if(pid < 0)
        {
            perror("error");
            exit(EXIT_FAILURE);
        }
        command = command->next;
        j+=2;
    }

    //Parent closes the pipes and
    for(i = 0; i < 2 * numPipes; i++)
    {
        close(pipefds[i]);
    }

//    //wait for children...
//    for(i = 0; i < numPipes + 1; i++)
//    {
//       //sleep(10000);
//       wait(&status);
//    }


     while(( pid = waitpid(-1,&status, WNOHANG|WUNTRACED)) == -1)
	{
             if(WIFSTOPPED(status))
             {
            	 sigtstp_handler(20);

            	 write(0,"child stopped", 15);

             }else if (WIFEXITED(status)){

                 write(0,"child exited",13);
	         }else{
	        	 perror("waitpid error");
	         }
	}

}

/**
 * Counts the number of pipes being used.
 */
int  countPipes(char * userInput)
{
    int count = 0;

    int length = strlen(userInput);
    for(int i=0; i < length; i++)
    {
       if(userInput[i]== '|')
       {
          count++;
       }
    }
   return count;
}

void  sigtstp_handler(int sig)
{
	signal(SIGTSTP, sigtstp_handler);

}
