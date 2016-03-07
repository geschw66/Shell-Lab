#include <stdio.h>
#include <iostream>
#include <string>
#include <stdio.h>
#include <string.h>
#include "BasicTasks.h"
#include "InternalCommands.h"
#include <ctype.h>
#include <unistd.h>

using namespace std;
void xshLoop();
int HandleInput(char* line, BasicTasks* bt, InternalCommands* ic);
bool ValidateCommandLine(int argc, char *argv[]);

int main(int argc, char * argv[])
{

    ValidateCommandLine(argc, argv);
    //command loop:
    xshLoop();

return 0;
}
/*
 * Control Loop for the shell xsh
 */
void xshLoop(void)
{
  BasicTasks bt;
  InternalCommands ic;
  char *line = NULL;
  int status;
  

  do {
     cout <<"xsh >> ";
     //Reading line user inputs:
     line =(char *) bt.readLine();

     //Not likely but if this is true; BAIL!
     if(line == NULL){
       cout<<"line is equal to NULL"<<endl;
      return;
     }

	 status = HandleInput(line, &bt, &ic);
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

int HandleInput(char* line, BasicTasks* bt, InternalCommands* ic)
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
        return -1;
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
		 ic->exportCmd(preservedLine);
         ic->addCmdToHistory(preservedLine);
	 }
	 else if(args.at(0) =="unexport")
	 {
		 ic->unexportCmd(preservedLine);
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
		 status = HandleInput((char *)ic->getHistoryCommand(historyItem).c_str(), bt, ic);
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
     }


     status = bt->executeLine(args,*ic);
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
