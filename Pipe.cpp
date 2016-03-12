/*
 * Pipe.cpp
 *
 *  Created on: Mar 10, 2016
 *      Author: ggeschw
 *
 * Purpose is to implement the pipe feature that works similar to the
 *   Linux/Unix flavor of the pipe mechanism.
 */

#include "Pipe.h"
#include <cstdlib>
#include <stdio.h>
#include <iostream>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <unistd.h>
#include <string>
#include <vector>
/**
 * Constructor
 */
Pipe::Pipe() : pipeCount(0),
               head(NULL)
{ }

/**
 * Destructor
 */
Pipe::~Pipe() {
 
   cleanUp();

}

/**
 * getCmdArrayLength():
 * Counts the number of substrings prior to the appearance of
 *     a "|"
 */
int Pipe::getCmdArrayLength(int i, vector<string> cmd) {

    //Count up to and including the pipe | becuase we need
    //  # substrings + a NULL element for execvp().
    int j = 0;
    while (  i  <  cmd.size() && cmd.at(i) != "|" )
    {
         i++;
         j++; // iterating the size of the array.
    }

    return j;
}

/**
 * setupPipe():
 * Sets up the creation of the linked-list,
 *    creating dynamic memory etc.
 */
void Pipe::setupPipe(vector<string> cmd)
{
    //separate commands... 1 | 2 | 3     into a char** of sub strings.
    //
    //create a dynamically allocated char** of N strings, each element is a command or
    // parameter of a command. Deep copy stuff HERE.
    // Each Node represents a separate command and its parameters
    cmdLine  * newNode;
    int cmdStart = 0;
    for(vector<string>::const_iterator it = cmd.begin(); it != cmd.end(); it++)
    {
        newNode = new cmdLine;

#ifdef _VDEBUG

        cout <<"The current command starts at element: "<<cmdStart<<endl;

#endif
        if(cmdStart >=cmd.size())
        {
           return;
        }
        int cmdLength = getCmdArrayLength(cmdStart, cmd);
        cmdStart += cmdLength +1;
#ifdef _VDEBUG

        cout <<"The next command will start at element: "<<cmdStart;
        //create a new char * string array:
        cout <<". There were "<<cmdLength<<" substrings in this command"<<endl;
#endif
        newNode->arguments = new char*[cmdLength+1];
        int i = 0;
        while (it != cmd.end() && *it != "|")
        {
            //Dynamic allocation of memory:
	    newNode->arguments[i] = new char[it->length() + 1];
            strcpy(newNode->arguments[i], it->c_str());
            
            //move the iterator one.
            it++;
            //iterate to the next element of the char ** array.
            i++;
        }
        newNode->arraLength = cmdLength +1;

#ifdef _VDEBUG
        cout <<"arraLength: "<<newNode->arraLength<<endl;
#endif
        newNode->arguments[i] = NULL; //last element for execvp needs to be NULL
        newNode->next = NULL;

        //Add to linked list:
        if(head == NULL)
        {
            head = newNode;
            head->next = NULL;

        }else
        {
            cmdLine * current = head;
            while(current->next)
            {
                current = current->next;
            }

            current->next = newNode;
        }
    }
}

/**
 *  pipeCommand():
 *  Public entrance to pipe command feature.
 */
void Pipe::pipeCommand(vector<string> &args, char * line) {

#ifdef _VDEBUG
    cout <<"before setupPipe call"<<endl;
#endif

    setupPipe(args);      //sets up the pipe.
    runPipedCmds(line);   //runs the pipe.
    cleanUp();            //destroys the linked list and frees memory.
}

/**
 *  runPipedCmds():
 *  The meat of the running piped commands.
 */
void Pipe::runPipedCmds(char* userInput)
{

#ifdef _VDEBUG
    printLinkedList();
#endif

    cmdLine * cmd = head;
    int numPipes = countPipes(userInput);

#ifdef _VDEBUG
    cout <<"Number of Pipes: "<< numPipes << endl;
#endif

    int status;
    int i = 0;
    pid_t pid;

    int pipefds[2*numPipes];

    for(i = 0; i < (numPipes); i++)
    {
        if(pipe(pipefds + i*2) < 0)
        {
            perror("couldn't pipe");
            exit(EXIT_FAILURE);
        }
    }

    int j = 0;
    while(cmd) {
        if((pid = fork())==0)
        {
            //if not last command
            if(cmd->next)
            {
                if(dup2(pipefds[j + 1], 1) < 0)
                {
                    perror("1st dup2");
                    exit(EXIT_FAILURE);
                }
            }

            //if not first command&& j!= 2*numPipes
            if(j != 0 ){
                if(dup2(pipefds[j-2], 0) < 0){
                    perror(" 2nd dup2");///j-2 0 j+1 1
                    exit(EXIT_FAILURE);

                }
            }


            for(i = 0; i < 2*numPipes; i++){
                    close(pipefds[i]);
            }

            if( execvp(*cmd->arguments, cmd->arguments) < 0 )
            {
                    perror(*cmd->arguments);
                    exit(EXIT_FAILURE);
            }

        } else if(pid < 0)
        {
            perror("error");
            exit(EXIT_FAILURE);
        }
        cmd = cmd->next;
        j+=2;
    }

    //clean up pipes.
    for(i = 0; i < 2 * numPipes; i++)
    {
        close(pipefds[i]);
    }

    //clean up child processes.
    for(i = 0; i < numPipes + 1; i++)
    {
        wait(&status);
    }
    
}

/**
 * countPipes():
 * Counts the number of pipes being used.
 */
int  Pipe::countPipes(char * userInput)
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

/**
 * cleanUp():
 * Performs the clean up; deallocate the dynamic
 *     memory of the linear linked list.
 *
 */
void Pipe::cleanUp()
{
    cmdLine * current;

    //if head is NULL we are done.
    while(head)
    {
       current = head->next;
       for(int i = 0; i < head->arraLength; i++)
       {
          if(head->arguments[i] != NULL)
          {
              delete[] head->arguments[i];
              head->arguments[i] = NULL;
          }

       }

       if(head->arguments != NULL)
       {
         delete []	 head->arguments;
         head->arguments = NULL;
       }

        head = current;
    }

}
/**
 * printLinkedList():
 *   Prints out the linked list for debugging purposes.
 */
void Pipe::printLinkedList()
{
       cmdLine * current = head;
       while(current)
       { 
          for(int i= 0; i < current->arraLength; i++)
          {
              if( current->arguments[i] !=NULL){
              	  int length =  strlen(current->arguments[i]);
              	  cout <<" ["<< i <<"] ";
             	  for(int j=0; j < length; j++)
             	  {
          		cout<<current->arguments[i][j];
             	  }
             
             } else{
                  
               cout <<" ["<< i <<"] NULL";
             }
          }
          cout<<endl;
          current = current->next;
      } 

}    
