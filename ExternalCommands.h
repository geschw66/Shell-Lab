#ifndef EXTERNALCOMMANDS_H
#define EXTERNALCOMMANDS_H

int bg_pid;
int fg_pid;

class ExternalCommands {

public:
	ExternalCommands();
	~ExternalCommands();
	void callExternal(int fg, char *cmd);
	void createProcess(char *cmd);
};

#endif
