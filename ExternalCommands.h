#ifndef EXTERNALCOMMANDS_H
#define EXTERNALCOMMANDS_H

using namespace std;
//int bg_pid;
//int fg_pid;

class ExternalCommands {

public:
	ExternalCommands();
	~ExternalCommands();
	void callExternal(int fg, vector<string> args);
};

#endif
