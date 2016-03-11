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

}

/**
 * getCmdArrayLength():
 * Counts the number of substrings prior to the appearance of
 *     a "|"
 */
int Pipe::getCmdArrayLength(int i, vector<string>& cmd) {

    //Count up to and including the pipe | becuase we need
    //  # substrings + a NULL element for execvp().
    while (cmd.at(i) != "|" || cmd.at(i) != cmd.end())
    {
        i++;
    }

    return i;
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
    int currentStart = 0;
    for(vector<string>::const_iterator it = cmd.begin(); it != cmd.end(); it++)
    {
        newNode = new cmdLine;
        cout <<"The current command starts at element: "<<currentStart<<endl;
        int cmdLength = getCmdArrayLength(currentStart, cmd);

        cout <<"The next command will start at element: "<<currentStart;
        cout <<"There were "<<cmdLength<<" substrings in this command"<<endl;

        //create a new char * string array:
        newNode->arguments = new char*[cmdLength+1];
        int i = 0;
        while (it != "|")
        {
            //Dynamic allocation of memory:
            newNode->arguments[i] = new char[strlen(it->c_str()) + 1];
            strcpy(newNode->arguments[i], it->c_str());
            it++;
            i++;
        }

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
void Pipe::pipeCommand(vector<string> args, char * line) {

    setupPipe(args);    //sets up the pipe.
    runPipedCmds(line);    //runs the pipe.
    cleanUp();            //destroys the linked list and frees memory.

}
/**
 *  runPipedCmds():
 *  The meat of the running piped commands.
 */
void Pipe::runPipedCmds(char* userInput)
{

    cmdLine * cmd = head;
    int numPipes = countPipes(userInput);
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
