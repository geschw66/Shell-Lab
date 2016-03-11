#include "BasicTasks.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <cstring>
#include <string>
using namespace std;


//Possible blank-space delimiters.
#define XSH_TOKEN_DELIMS " \t\r\n\a"
/**
 * BasicTasks: Constructor.
 */
BasicTasks::BasicTasks()
{

}
/**
 * BasicTasks: Destructor.
 */
BasicTasks::~BasicTasks()
{

}

/*
 * readLine(void):
 *    Reads the line typed in from the user.
 */
const char * BasicTasks::readLine(void)
{
      char *line = NULL;
      size_t sizeOfBuffer = 0; //
      getline(&line, &sizeOfBuffer, stdin);
      return (const char *) line;
}

/**
 * parseLine():
 *   Parses the line typed in by user or provided by batch script.
 */
int BasicTasks::parseLine( char * line,vector<string>& tokens)
{
    char *token;

    //Use strtok to grab the next token from the line.
    token = strtok(line, XSH_TOKEN_DELIMS);
    while (token != NULL)
    {
        tokens.push_back(token);
        token = strtok(NULL, XSH_TOKEN_DELIMS);
     }

    return 0;
}

int BasicTasks::executeLine(vector<string>  args,InternalCommands &intCmd)
{
   //int i;

   return 1;
}

int BasicTasks::is_number(const string& s){
    string::const_iterator it = s.begin();
    while(it != s.end() && (isdigit(*it) || (*it == '-' && it == s.begin()))) ++it;
    return (it == s.end());
}
