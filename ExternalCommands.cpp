#include <string.h>
#include <iostream>
#include <unistd.h>
#include <stdlib.h>
#include <vector>
#include <algorithm>
#include "ExternalCommands.h"
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

using namespace std;

extern char ** environ;

ExternalCommands::ExternalCommands() {}

ExternalCommands::~ExternalCommands() {}

void ExternalCommands::callExternal(int fg, vector<string> args) {
    int pid = 0;
    
    //handle arguments
    //check for I/O redirection ( < or > )
    int inPipe = 0;
    int outPipe = 0;
    int n = 0;
    for(int i = 1; i < args.size(); ++i){
        if(!args.at(i).compare("<")){
            inPipe = i;
            if(n ==0){
                n=i;
            }
        }
        else if (!args.at(i).compare(">")){
            outPipe = i;
            if(n==0){
                n=i;
            }
        }
        
    }
    //prevent user from using both input and output at same time
    if(inPipe && outPipe){
        cout << "simultaneous input/output not supported" <<endl;
        return;
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
    if (n== 0){
        n = args.size();
    }
    
    char * argv[n+1];
    for(int i = 0; i<n; ++i){
        argv[i] = &(args.at(i))[0u];
    }
    //char x = '$';
    //char x = '$';
    argv[n] = NULL;
    
    //debug
    //    for(int i = 0; i<n+1; ++i){
    //        if( argv[i]==NULL)
    //            cout << "$" <<endl;
    //        else
    //        cout << argv[i] <<endl;
    //    }
    
    
    //  for(int i = 0; i<2; ++i){
    pid = fork();
    if (pid == -1) {
        //forking error
        cout << "forking error" <<endl;
        return;
    }
    if (pid == 0) {
        //child process, piping
        if(outPipe){//&& i == 0){
            int out = open(outfile, O_WRONLY | O_CREAT, S_IRUSR | S_IWUSR);
            dup2(out,STDOUT_FILENO);
            close(out);
        }
        
        if(inPipe){// && i ==1){
            int in = open(infile, O_RDONLY);
            dup2(in, STDIN_FILENO);
            close(in);
        }
        
        if(execvp(filename,argv) == -1){
            cout << errno << endl;
        }
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
    // }
}
