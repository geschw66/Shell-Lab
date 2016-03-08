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
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <iomanip>
#include <algorithm>
#include <unistd.h>
#include <term.h>
#include <dirent.h>
#include <stdlib.h>

#include <signal.h>
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

/**
 * setEnvVars();
 * set the environmental variables (exported vars) from external file storage
*/
void InternalCommands::setEnvVars(){
    ifstream envVars("./envVars.txt");
    if(envVars.is_open()){
        string line;
        while(getline(envVars, line)){
            if(!line.empty()){
                vector<string> kv;
                istringstream iss(line);
                string temp;
                while(iss >> temp){
                    kv.push_back(temp);
                }
                
                environMap[kv.at(0)]= kv.at(1);
                
                
            }
        }
        //environCmd();
    }
}
/**
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
 * pauseCmd()
 * pause the shell until the Enter Key is pressed;
 */
void InternalCommands::pauseCmd(){
    cout<< "Shell Paused. Press Enter to Continue" <<endl;
    cin.ignore();
}

/**
 * Searches through the environMap to find the given key value
 */
int InternalCommands::showEnvironValue(const string& arg)
{
	//Case of where argument is in the form $ and string of chars ex: $TEST:
	// will search value by the key TEST and return value.
	string key = arg.substr(1);

	int status = (environMap.find(key) != environMap.end());

	//If the key exists print the value
	if (status) {
		cout << environMap.at(key) << " ";
	}
	return status;
}

/**
 * Searches through localMap for a value, given a key
 */
int InternalCommands::showLocalValue(const string& arg)
{
	string key = arg.substr(1);
	int status = (localMap.find(key) != localMap.end());
	if (status) {
		cout << localMap.at(key) << " ";
	}
	return status;
}
/**
 * showCommand(vector <string> args)
 * 	show W1 W2 ...: display the word(s)
 * 	followed by a newline
 */
void InternalCommands::showCommand(vector<string> args) {
	vector<string>::iterator it = args.begin();

	//start with second element
	it++;

	//iterate until end
	for (; it != args.end(); ++it) {
		string arg = *it;

		char c = arg.at(0);

		if (c == '$')
		{
			//variable substitution as necessary
			if (arg.length() > 1)
			{
				if (arg.at(1) == '$')
				{
					//pid of shell
					cout << ::getpid() << " ";
				} else if (arg.at(1) == '?')
				{
					//Decimal value returned by last foreground command
					arg = "Return value of last foreground command goes here";
					cout << arg << " ";
				} else if (arg.at(1) == '!')
				{
					//pid of last background command
					arg = "PID of last background command goes here";
					cout << arg << " ";
				} else
				{
                    //Case of where argument is in the form $ and string of chars ex: $TEST:
					// will search value by the key TEST and return value.
					if (!showEnvironValue(arg)) {
						if (!showLocalValue(arg)) {
							cout << arg << " ";
						} 
					}
				}
			}
		}
		else {
			cout << arg << " ";
		}
	}
	cout << endl;
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
   eCommand.erase(firstElement, 5);
   cout <<eCommand;

}

/**
 * void setCmd()
 * set the value of local variables stored in localMap
 */
void InternalCommands::setCmd(char * cmd, vector<string> args){
    
    if(args.size() != 3){
        cout << "usage: set W1 W2" <<endl;
    }
    else {
    string eCommand = cmd;
    string delim = " ";
    
    //get rid of the set cmd
    eCommand.erase(eCommand.find("set"), 4);
    
    //get W1 and W2
    int pos = eCommand.find(delim);
    string W1 = eCommand.substr(0, pos);
    eCommand.erase(0, pos + delim.length());
    pos = eCommand.find(delim);
    string W2 = eCommand.substr(0, eCommand.length() - 1);
    
    //make W1 all caps
    transform(W1.begin(), W1.end(), W1.begin(), ::toupper);
    //add W2 to index W1 of localMap
    localMap[W1] = W2;
    }
}

void InternalCommands::unsetCmd(char * cmd){
    string eCommand = cmd;
    //get rid of the unset cmd
    eCommand.erase(eCommand.find("unset"), 6);
    //Get W1
    string W1 = eCommand;
    W1 = W1.substr(0, W1.length()-1);
    
    //make W1 all caps
    transform(W1.begin(), W1.end(), W1.begin(), ::toupper);
    //remove W1 from localMap if it exists
    localMap.erase(W1);
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
 void InternalCommands::exportCmd(char * cmd, vector<string> args)
 {   if(args.size() != 3){
         cout << "usage: Export W1 W2" <<endl;
     }
     else {
         string eCommand = cmd;
         string delim = " ";
    
         //get rid of the export cmd
         eCommand.erase(eCommand.find("export"), 7);

         //get W1 and W2
         int pos = eCommand.find(delim);
         string W1 = eCommand.substr(0, pos);
         eCommand.erase(0, pos + delim.length());
         pos = eCommand.find(delim);
         string W2 = eCommand.substr(0, eCommand.length() - 1);

         //make W1 all caps
         transform(W1.begin(), W1.end(), W1.begin(), ::toupper);
         //add W2 to index W1 of environMap
    
         environMap[W1] = W2;
    
         //export to outside file so variables are available to every shell instance
         ofstream envVars ("./envVars.txt");
         string out= "";
         
         for(map<string, string>::const_iterator it = environMap.begin(); it != environMap.end(); it++){
             //print out elements to file
             out += it->first + " " + it->second + "\n";
         }
    
         envVars << out;
     }
 }

  /**
 * unexportCmd(vector <string> cmd)
 *	export <W1>: unset (remove) <W1> from map
 */
 void InternalCommands::unexportCmd(char * cmd)
 {
	string eCommand = cmd;
	//get rid of the unexport cmd
  	eCommand.erase(eCommand.find("unexport"), 9);
	//Get W1
	string W1 = eCommand;
	W1 = W1.substr(0, W1.length()-1);
	 
	//make W1 all caps
	transform(W1.begin(), W1.end(), W1.begin(), ::toupper);
    //remove W1 from map if it exists
	environMap.erase(W1);
     
     //export to outside file so variables are available to every shell instance
     ofstream envVars ("./envVars.txt");
     string out= "";
     for(map<string, string>::const_iterator it = environMap.begin(); it != environMap.end(); it++)
     {
         //print out elements to file
         out += it->first + " " + it->second + "\n";
     }
     
     envVars << out;

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

/*
 * replaceEnvironCmds(string)
 * replace enviroment variables with their string counterparts
 */
string InternalCommands::replaceEnvironCmds(char * line)
{
	string eCommand = line;
	const char* delims = " \t\r\n\a";
	char* token;
	vector<string> tokens;

    	//Use strtok to grab the next token from the line.
    	token = strtok(line, delims);
    	while (token != NULL)
    	{
        	tokens.push_back(token);
        	token = strtok(NULL, delims);
    	}

	//scan through tokens, replace any environment variables with its value
	for(vector<string>::iterator it = tokens.begin(); it != tokens.end() ; it++)
	{
		string temp = *it;
		//if the first value is the character '$' search to see if the rest is there
		if(temp.at(0) == '$')
		{
			//strip off '$'
			cout << "Stripping '$' from " << temp;
			temp.erase(temp.begin());
			cout << "= " << temp << endl;
			//make all caps
			transform(temp.begin(), temp.end(), temp.begin(), ::toupper);
			//if it exists in the map, replace it
			if(environMap.count(temp) > 0)
			{
				it->replace(it->begin(), it->end(), environMap[temp]);
				cout << "temp: " << temp << " & environMap[temp]: " << environMap[temp] << endl;
			}
		}
	}
    
    //export to outside file so variables are available to every shell instance
    ofstream envVars ("./envVars.txt");
    string out= "";
    for(map<string, string>::const_iterator it = environMap.begin(); it != environMap.end(); it++)
    {
        //print out elements to file
        out += it->first + " " + it->second + "\n";
    }
    
    envVars << out;
}

/**
 * dir();
 * list the contents of the current directory
 */
void InternalCommands::dirCmd(){
    DIR *dirp;
    struct dirent *epd;
    
    dirp = opendir("./");
    if(dirp != NULL){
        while((epd = readdir(dirp)) != NULL){
            
        	//why printf and not cout?
           // printf("%s\n",epd->d_name);
        	cout << epd->d_name <<endl;
        }
        closedir(dirp);
    }
    else {
        cout << "Could not get Directory Listing" <<endl;
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


int InternalCommands::is_number(const string& s){
    string::const_iterator it = s.begin();
    while(it != s.end() && isdigit(*it)) ++it;
    return (it == s.end());
}

/**
 * kill(vector<string>args)
 * send signal W1 to process W2
 */
void InternalCommands::killCmd(vector<string>args){
    
    if(args.size() > 3 || args.size() == 1){
        cout << "usage: kill [-n] pid" <<endl;
    }
    else{
        string s = args.at(1);
        if(args.size() == 2){
            if(is_number(s)){
                kill(stoi(s),SIGTERM);
            }
        }
        else{
            string p = args.at(2);
            if(is_number(p)){
                if(s.at(0) =='-' && s.length() >1 && is_number(s.substr(1))){
                    kill(stoi(s), stoi(p));
                }
                else{
                    cout << "usage: kill [-n] pid" <<endl;
                }
            }
        }
    }
}

/*
 * wrapper for map.find(key)->value;
 */
string InternalCommands::findMapValue(string key)
{
	if(!key.empty())
	 cout << environMap.find(key)->second<<endl;

	return key;
}

    
