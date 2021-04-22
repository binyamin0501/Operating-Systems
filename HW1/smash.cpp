/*	smash.c
main file. This file contains the main function of smash
*******************************************************************/
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h> 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <string>
#include <iostream>
#include <ostream>
#include <list>
#include "commands.h"
#include "signals.h"

using namespace std;

#define MAX_LINE_SIZE 80
#define MAXARGS 20
#define MAX_HIST 50

// declering functions
void Cmd_History_Update(string cmdString);

void* jobs = NULL; //This represents the list of jobs. Please change to a preferred type (e.g array of char*)
char lineSize[MAX_LINE_SIZE];
struct sigaction C_act;
struct sigaction Z_act;
list <string> Cmd_History;

//**************************************************************************************
// function name: main
// Description: main function of smash. get command from user and calls command functions
//**************************************************************************************
int main(int argc, char *argv[])
{
    char cmdString[MAX_LINE_SIZE]; 	   
	Cmd_History.clear();

	// taking care of the ctrl-c and ctrl-z like in recition 2
	sigset_t mask_c, mask_z;
	sigfillset(&mask_c);
	sigfillset(&mask_z);

	C_act.sa_handler = &catch_C_int;
	C_act.sa_flags = 0;
	C_act.sa_mask = mask_c;
	
	Z_act.sa_handler = &catch_Z_int;
	Z_act.sa_flags = 0;
	Z_act.sa_mask = mask_z;

	// setting up the handlers and checking if the system call failed
	if ((sigaction(SIGINT, &C_act, NULL)) || sigaction(SIGTSTP, &Z_act, NULL))
	{
		perror("smash");
		return 1;
	}
	
    while (1)
	{
	 	printf("smash > ");
		fgets(lineSize, MAX_LINE_SIZE, stdin);
		strcpy(cmdString, lineSize);    	
		cmdString[strlen(lineSize)-1]='\0';
		Cmd_History_Update(cmdString);
		// background command	
	 	if(!BgCmd(lineSize, jobs)) continue; 
		// built in commands
		ExeCmd(jobs, lineSize, cmdString);
		
		/* initialize for next line read*/
		lineSize[0]='\0';
		cmdString[0]='\0';
	}
    return 0;
}

void Cmd_History_Update(string cmdString)
{
	if ((cmdString == "") || (cmdString == "\0"))
		return;
	string new_cmd(cmdString);
	if (Cmd_History.size() == MAX_HIST)
	{
		Cmd_History.pop_front();
	}
	Cmd_History.push_back(new_cmd);
}