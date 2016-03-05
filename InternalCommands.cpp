// InternalCommands.cpp
//
// Class responsible for simple internal commands:
// 	clr
// 	history
//	show
//	echo
//
//- - - - - - - - - - - - - - - - - - - - - -
#include <locale>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <unistd.h>
#include <term.h>
#include "InternalCommands.h"

using namespace std;

/**
 * Constructor.
 */
InternalCommands::InternalCommands():longestIndex(0)
{}
/**
 * Destructor.
 */
InternalCommands::~InternalCommands(){}

/*
* clearScreen():
* 	clr: Clear the screen and display a new 
* 	command line prompt at the top.
*
*/
void InternalCommands::clearScreen()
{
    if(!cur_term)
    {
       int result;
        //Access the terminfo database, so we can tell the terminal to clear.
        //STDOUT_FILENO-> file number/discriptor of standard out.
        //result tests to see if an error was returned if yes..then bail.
        setupterm(NULL, STDOUT_FILENO, &result);
       if(result <= 0)
       {
           return;
       }
    }
    
    char clear[6] = "clear";
    //Tells terminal to clear the screen.
    putp(tigetstr(clear));
}

/**
 * showCommand(vector <string> args)
 * 	show W1 W2 ...: display the word(s)
 * 	followed by a newline
 */
 void InternalCommands::showCommand(vector<string> args)
 {
     vector<string>::iterator it = args.begin();
     //start with second element
     it++;
     //iterate until end
     for (; it != args.end(); ++it)
     {
     	cout << *it << endl;
     }
 }

/**
 * echoCommand(vector <string> cmd)
 *	echo <comment>: display <comment> on the 
 *	sdout followed by a new line.
 */
void InternalCommands::echoCommand(char * eCmd)
{
   //change it to a string so as to take advantage of erase
   string eCommand = eCmd;
   int firstElement = eCommand.find("echo");
   //Remove the subsring "echoc"
   eCommand.erase(firstElement, 4);
   cout <<eCommand;

}

/**
 * historyCommand()
 * 	history: displays up to the last 100 commands.
 */
void InternalCommands::historyCommand()
{
  
  for(unsigned int i = 0; i < historyList.size();i++)
  {  
      cout<<" "<<right<<setw(longestIndex)<< i+1 <<left<<" "<< historyList.at(i);
  }
  
}

/*
 * getHistoryCommand(int n)
 * returns item n from history list
 *
 */

string InternalCommands::getHistoryCommand(int n)
{
	//if negative, get length - n 
	if(n < 0)
	{
		return getHistoryCommand(historyList.size() + n);
	}
	//else just return nth element
	else
	{
		return historyList.at(n);
	}
}

/**
 *  addCmdToHistory(char * cmd);
 * 	This function adds the most recent command to the historyList.
 * 	If the state of the list prior to adding the command is equal 
 * 	100 elements, then the oldest element is removed.
 */
 void InternalCommands::addCmdToHistory(char * cmd)
{
    string strCmd = cmd;
    string newStrIndex;
    //If the current size is equal to 100, then remove the oldest element.
    if(historyList.size() == 100)
    {
       historyList.erase(historyList.begin());
    }

    //add the commandline.   
    historyList.push_back(strCmd);
       
    //adjust the size of the index for formatting
    // to the width of the longest number string.
    newStrIndex = static_cast<ostringstream*>( &(ostringstream() << historyList.size()) )->str();
       
    if(newStrIndex.size() > longestIndex)
    {
          longestIndex = newStrIndex.size();
    }
}

 /**
 * exportCmd(vector <string> cmd)
 *	export <W1> <W2>: set (store) <W2> in map under index <W1>
 */
 void InternalCommands::exportCmd(char * cmd)
 {
	 string eCommand = cmd;
	 string delim = " ";
     //get rid of the export cmd
     eCommand.erase(eCommand.find("export"), 6);
	 //get W1 and W2
	 int pos = eCommand.find(delim);
	 string W1 = eCommand.substr(0, pos);
	 eCommand.erase(0, pos + delim.length());
	 pos = eCommand.find(delim);
	 string W2 = eCommand;

	 //make W1 all caps
	 transform(W1.begin(), W1.end(), W1.begin(), ::toupper);
	 //add W2 to index W1 of environMap
	 environMap[W1] = W2;
 }

  /**
 * unexportCmd(vector <string> cmd)
 *	export <W1>: unset (remove) <W1> from map
 */
 void InternalCommands::unexportCmd(char * cmd)
 {
	 string eCommand = cmd;
	 //get rid of the unexport cmd
     eCommand.erase(eCommand.find("unexport"), 8);
	 //Get W1
	 string W1 = eCommand;
	 
	 //make W1 all caps
	 transform(W1.begin(), W1.end(), W1.begin(), ::toupper);
     //remove W1 from map if it exists
	 environMap.erase(W1);
 }

 /**
 * environCmd()
 *	print every environment variable currently set
 */
 void InternalCommands::environCmd()
 {
	 //iterate through the map
	 for(map<string, string>::const_iterator it = environMap.begin(); it != environMap.end(); it++)
     {
		 //print out elements "FIRST=second"
		 cout << it->first << "=" << it->second << endl;
     }
 }

/**
 * chdir(vector<string> args)
 * 	chdir W: change the current directory
 */
int InternalCommands::chdirCommand(vector<string> args)
{
	if (args.size() != 2) {
		cout << "usage: chdir W"<< endl;
		return -2; //-2 for incorrect usage
	}
	else {
		return (chdir(args.at(1).c_str())); //returns 0 for success, -1 for failure
	}
}
