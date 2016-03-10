#ifndef BASICTASKS_H_
#define BASICTASKS_H_

#include <string>
#include <vector>
#include "InternalCommands.h"

using namespace std;

class BasicTasks
{
   public:
     BasicTasks();
    ~BasicTasks();

     //Basic tasks for the shell xsh:
     const char * readLine(void);
     int parseLine(char * line,vector<string>& tokens);
     int  executeLine(vector<string>  args,InternalCommands &intCmd);
     int is_number(const string& s);

   private:


};
#endif
