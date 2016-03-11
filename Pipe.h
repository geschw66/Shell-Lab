/*
 * Pipe.h
 *
 *  Created on: Mar 10, 2016
 *      Author: ggeschw
 */

#ifndef PIPE_H_
#define PIPE_H_

#ifndef COMMAND_LINE
#define COMMAND_LINE

//represents command and args to be piped.
struct cmdLine
{
 char** arguments;	//cmd and arguments
 int arraLength;  	//number of elements of the arguments array
 cmdLine * next;	//next Node in the cmdline linked list.
};
#endif

#include <iostream>
#include <string>
#include <cstring>
#include <vector>
#include <map>

using namespace std;

class Pipe {
public:
	//Constructor
	Pipe();

	//Destructor
	virtual ~Pipe();

	// public interface to pipe command.
    void pipeCommand(vector<string> args, char * line);

private:
	cmdLine * head;//for linked list of structs that contain a char** of the commands for execvp.
	//Counts the number of pipes in the usersInput.
	int countPipes(char * userInput);
	//Sets up the creation of the linklist, creating dynamic memory etc.
	void setupPipe(vector<string> cmd);
	//Runs the actual piped commands.
	void runPipedCmds(char* userInput);
	//Clean up (deallocate the dynamic memory of the linear linked list.
	void cleanUp();
	int getCmdArrayLength(int i, vector<string>& cmd);

	//pipeCount persists as long as the linked list persists.
	unsigned int pipeCount;

};

#endif /* PIPE_H_ */
