#include <stdio.h>
#include <iostream>
#include <string>
#include <signal.h>
#include <stdio.h>
#include <string.h>
#include "BasicTasks.h"
#include "InternalCommands.h"
#include "ExternalCommands.h"
#include <ctype.h>
#include <unistd.h>

using namespace std;
void xshLoop();
int HandleInput(char* line, BasicTasks* bt, InternalCommands* ic, ExternalCommands* ec);
bool ValidateCommandLine(int argc, char *argv[]);

int pgid; //process group id

int main(int argc, char * argv[])
{
    //set session ID, this allows for killing all child processes at the end, more easily
    setsid();
    //get process group id
    pgid = getpgid(0);

    ValidateCommandLine(argc, argv);
    //command loop:
    xshLoop();

return 0;
}

/**
 * sigHandler(int signum)
 * handles all appropriate signals and ignores the rest
 */
void sigHandler(int signum){
    
    switch(signum){
        case SIGINT:
            //if foreground process
                //kill(pid_foreground, SIGINT)
            break;
        case SIGQUIT:
            //if foreground process
                //kill(pid_foreground, SIGQUIT)
            break;
        case SIGCONT:
            //if foreground process
                //kill(pid_foreground, SIGCONT)
            break;
        case SIGTSTP:
            //if foreground process
                //kill(pid_foreground, SIGSTP)
            break;
        default:
            //ignore
            break;
    }
}
    



/*
 * Control Loop for the shell xsh
 */
void xshLoop(void)
{
  BasicTasks bt;
  InternalCommands ic;
  ExternalCommands ec;
  char *line = NULL;
  int status;

  //catch signals
  signal(SIGINT, sigHandler);
  signal(SIGQUIT, sigHandler);
  signal(SIGCONT, sigHandler);
  signal(SIGTSTP, sigHandler);
  signal(SIGABRT, sigHandler);
  signal(SIGALRM, sigHandler);
  signal(SIGHUP, sigHandler);
  signal(SIGTERM, sigHandler);
  signal(SIGUSR1, sigHandler);
  signal(SIGUSR2, sigHandler);
  
  //ic.setEnvVars();

  do {
     cout <<"xsh >> ";
     //Reading line user inputs:
     line =(char *) bt.readLine();

     //Not likely but if this is true; BAIL!
     if(line == NULL){
       cout<<"line is equal to NULL"<<endl;
      return;
     }

	 status = HandleInput(line, &bt, &ic, &ec);
	 //if -1, exit command was sent
	 if(status == -1)
	 {
		 return;
	 }

     delete [] line ;
     line = NULL;

  } while (status);

}

/*
 * HandleInput():
 *   designed to handle the parsing of input line
 *   this is created so that it can be recalled when the repeat command is issued
 */

int HandleInput(char* line, BasicTasks* bt, InternalCommands* ic, ExternalCommands* ec)
{
	  //Variables for handling input
      vector<string> args;
	  char * preservedLine;
	  int status;

	  //Need a deep copy.
      preservedLine = new char[strlen(line) + 1];
      strcpy(preservedLine, line);

     bt->parseLine(line,args);

     if(args.empty())
     {
#ifdef _VDEBUG
          cout <<"the command line is empty" << endl;
#endif
       
     }
     else if(args.at(0) =="exit")
     {
        if(args.size() > 1 && bt->is_number(args.at(1)))
	{
		//kill all child processes
		kill(pgid, 15);
		//report back whatever is given
		exit(stoi(args.at(1)));
	}
	else if(args.size() > 1)
	{
		cout << "Usage: exit [n]" << endl;
		//else report back an error
		exit(-1);
	}
	else
	{
		//else exit
		//kill all child processes
		kill(pgid, 15);
		//exit normally
		exit(0);
	}
     }
     else if(args.at(0)=="clr")
     {
         ic->clearScreen();
         ic->addCmdToHistory(preservedLine);
     }
	 else if(args.at(0) =="echo")
     {
         ic->echoCommand(preservedLine);
         ic->addCmdToHistory(preservedLine);
     }
     else if(args.at(0) == "set"){
         ic ->setCmd(preservedLine, args);
         ic->addCmdToHistory(preservedLine);
     }
     else if(args.at(0)== "unset"){
         ic->unsetCmd(preservedLine);
         ic->addCmdToHistory(preservedLine);
     }
	 else if(args.at(0)=="show")
     {
         ic->showCommand(args);
         ic->addCmdToHistory(preservedLine);
     }
	 else if(args.at(0) =="history")
     {
         ic->addCmdToHistory(preservedLine);
		 ic->historyCommand();
     } 
	 else if(args.at(0) =="export")
	 {
		 ic->exportCmd(preservedLine, args);
         ic->addCmdToHistory(preservedLine);
	 }
	 else if(args.at(0) =="unexport")
	 {
		 ic->unexportCmd(args);
         ic->addCmdToHistory(preservedLine);
	 }
	 else if(args.at(0) =="environ")
	 {
		 ic->environCmd();
         ic->addCmdToHistory(preservedLine);
	 }
     else if(args.at(0) == "dir"){
         ic->dirCmd();
         ic->addCmdToHistory(preservedLine);
     }
     else if(args.at(0) == "chdir"){
         ic-> chdirCommand(args);
         ic ->addCmdToHistory(preservedLine);
     }
     else if (args.at(0) == "pause"){
         ic->pauseCmd();
         ic ->addCmdToHistory(preservedLine);
     }
     else if(args.at(0) == "kill"){
         ic->killCmd(args);
         ic->addCmdToHistory(preservedLine);
     }
	 else if(args.at(0) =="repeat")
	 {
		 int historyItem = -1;

		 //get argument number if there are more than one argument
		 if(args.size() >= 2)
		 {
			// try
			// {
				 sscanf(args.at(1).c_str(), "%d", &historyItem);
				 historyItem--;
			// }
			// catch(Exception *e)
			// {
			//	 //if it failed, just roll with last command for repeating
			//	 cout << "Unable to interpret argument. Repeating last command." << endl;
			//	 historyItem = -1;
			// }
		 }
		 //get history command, recall this function
		 status = HandleInput((char *)ic->getHistoryCommand(historyItem).c_str(), bt, ic, ec);
         args.clear();

		 //do not save repeat command to history list, this mimicks the bang command in linux

		 //handle memory
         delete [] preservedLine;
         preservedLine = NULL;

	     return status;
	 }
     else
     {
#ifdef _VDEBUG
          cout <<"args are:"<<endl;
          for(unsigned int i= 0; i < args.size(); i++)
          cout <<"["<<i<<"]: "<<args.at(i)<<endl;
#endif
         
         //Call External
         int fg = (args.at(args.size()-1).at(0)!='&');
         ec->callExternal(fg, args);
         
     }


     status = 1;//bt->executeLine(args,*ic);
     args.clear();

	 //handle memory
     delete [] preservedLine;
     preservedLine = NULL;

	 return status;
}

/**
 *ValidateCommandLine():
 *   validates the commandline for valid combinations of option
 *   flags and their arguments.
 */
bool ValidateCommandLine(int argc, char *argv[])
{
    char opt;
    bool  status = true;
    string filename;

     if(argc == 1)
     {  
       //return false;
     } 

     while((opt=getopt(argc,argv,"f:w:h"))!=-1)
     {  
        switch(opt)
        {
            case 'h':
  
               //show help and exit:
               status = false;
               break;
           case 'f':
               filename = optarg;
               break;
           case 'w':
              filename = optarg;

              break;
          default:
             break;
 
        } 
     } 

 
 return status;
}


