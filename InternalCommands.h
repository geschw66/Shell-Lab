#ifndef INTERNALCOMMANDS_H_
#define INTERNALCOMMANDS_H_

#include <iostream>
#include <string>
#include <cstring>
#include <vector>
#include <map>

using namespace std;

class InternalCommands
{

   public:
   //Constructor
   InternalCommands();   
   //Destructor
   ~InternalCommands();
   //setEnvVars: set the enviromental variables (exported vars) from external file storage
   void setEnvVars();
   //clr: Clear the screen and display a new command line prompt at the top.
   void clearScreen();
   //pause: Pause the shell until the Enter button is pressed
   void pauseCmd();
   //show: show W1 W2 ... : display the word(s) followed by a newline
   void showCommand(vector<string> args);

  //echo: echo <comment>: displays <comment> on the stdout follwed by a
  //   new line.
  void  echoCommand(char *  eCmd);

  // set and unset
  // set and unset the value of local variables, stored in localMap
  void setCmd(char * cmd, vector<string>args);
  void unsetCmd(char * cmd);
    
  //HISTORY related methods:
  // history() prints the history
  // FIFO structure:
  // addCmdToHistory() adds the cmd line to the history structure.
  // rmvlastCmd() removes the oldest command.
  
  void  historyCommand();
  //Get command from history vector
  string getHistoryCommand(int n);
  //Add the command to the history vector.
  void  addCmdToHistory(char * cmd);

  //export: export <W1> <W2>:
  //	sets environment variable <W1>.toupper() to value <W2>
  void exportCmd(char * cmd, vector<string> args);
  
  //unexport: unexport <W1>:
  //	removes <W1> from set environment variables
  void unexportCmd(char * cmd);
  
  //environ: print out all set environment variables
  void environCmd();
  
  //replace enviroment variables with their string counterparts
  string replaceEnvironCmds(char * line);
  
  //dir
  // list the contents of the current directory
  void dirCmd();
    
  //chdir: change current directory to W
  int chdirCommand(vector<string> args);

  private:
  vector <string> historyList;
  map <string, string> environMap;
  map <string, string> localMap;
  unsigned int longestIndex;

  //private functions:
  string findMapValue(const string key);
	int showEnvironValue(const string& arg);
	int showLocalValue(const string& arg);
};

#endif

