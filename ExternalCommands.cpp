#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include "ExternalCommands.h"
#include <sys/wait.h>

using namespace std;

ExternalCommands::ExternalCommands() {}

ExternalCommands::~ExternalCommands() {}

void callExternal(int fg, char *cmd) {
	int pid = 0;

	//handle arguments
	//check for I/O redirection ( < or > )
	
	pid = fork();
	if (pid == -1) {
		//forking error
	}
	if (pid == 0) {
		//child process, piping
	}
	else {
		//parent process
		//foreground/background
		if (fg) {
			fg_pid = pid;
			waitpid(pid, NULL, WUNTRACED);
		}
		else {
			bg_pid = pid;
		}
	}
}
