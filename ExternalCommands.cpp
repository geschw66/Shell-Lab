#include <string.h>
#include <iostream>
#include <unistd.h>
#include <stdlib.h>
#include <vector>
#include <algorithm>
#include "ExternalCommands.h"
#include <sys/wait.h>

using namespace std;

//extern char ** environ;

ExternalCommands::ExternalCommands() {}

ExternalCommands::~ExternalCommands() {}

void ExternalCommands::callExternal(int fg, vector<string> args) {
	int pid = 0;
    
	//handle arguments
	//check for I/O redirection ( < or > )
    int inPipe = 0;
    int outPipe = 0;
    for(int i = 1; i < args.size(); ++i){
        if(!args.at(i).compare("<")){
            inPipe = i;
        }
        else if (!args.at(i).compare(">")){
            outPipe = i;
        }
        
    }
    char * filename = &(args.at(0))[0u];
    
    //find input/output files if any
    char * infile, *outfile;

    if(inPipe != 0){
        if(inPipe+1 != args.size()){
            infile = &(args.at(inPipe+1))[0u];
        }
    }
    if(outPipe != 0){
        if(outPipe+1 != args.size()){
            outfile = &(args.at(outPipe+1))[0u];
        }
    }

    //create argv
    int n;
    if ((n = min(outPipe, inPipe)) == 0){
        n= args.size();
    }
    char * argv[n];
    for(int i = 0; i<n; ++i){
        argv[i] = &(args.at(i))[0u];
    }

	pid = fork();
	if (pid == -1) {
		//forking error
        cout << "forking error" <<endl;
        return;
	}
	if (pid == 0) {
		//child process, piping
//        if((execve(filename,argv, environ)) == -1){
//            return;
//        }
        exit(0);
        
	}
	else {
		//parent process
		//foreground/background
		if (fg) {
	//		fg_pid = pid;
			waitpid(pid, NULL, WUNTRACED);
		}
		else {
	//		bg_pid = pid;
		}
	}
}
