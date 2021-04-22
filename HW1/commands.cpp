//		commands.c
//********************************************
#include "commands.h"
#define FAIL 1
#define SUCCESS 0
#define ZERO_ARG 0
#define ONE_ARG 1
#define TWO_ARG 2
//********************************************
// function name: ExeCmd
// Description: interperts and executes built-in commands
// Parameters: pointer to jobs, command string
// Returns: 0 - success,1 - failure
//**************************************************************************************
int ExeCmd(void* jobs, char* lineSize, char* cmdString)
{
	char* cmd; 
	char* args[MAX_ARG];
	char pwd[MAX_LINE_SIZE];
	char* delimiters = " \t\n";  
	int i = 0, num_arg = 0;
	Bool illegal_cmd = FALSE; // illegal command
    cmd = strtok(lineSize, delimiters);
	if (cmd == NULL)
		return 0; 
   	args[0] = cmd;
	for (i=1; i<MAX_ARG; i++)
	{
		args[i] = strtok(NULL, delimiters); 
		if (args[i] != NULL) 
			num_arg++; 
	}
/*************************************************/
// Built in Commands
// https://man7.org/linux/man-pages/man2/syscalls.2.html
/*************************************************/
	if (!strcmp(cmd, "pwd") ) 
	{
		if (num_arg != ZERO_ARG) {
			printf("smash error: > \"%s\"\n", cmdString);
			return FAIL;
		}

		char *path = NULL;
		path = get_current_dir_name();
		if (!path) {
			perror("smash");
			return FAIL;
		}

		printf("%s\n", path);
		free(path);
	} 
	
	/*************************************************/
	else if (!strcmp(cmd, "cd")) 
	{
		if (num_arg != ONE_ARG) {
			printf("smash error: > \"%s\"\n", cmdString);
			return FAIL;
		} 

		char *new_path = NULL;
		new_path = argv[1] == '-' ? old_path : argv[1];
		old_path_try = get_current_dir_name();

		if (!old_path_try) {
			if (errno == ENOENT) {
				printf("smash error: > “path” – No such file or directory\n");
				return FAIL;
			}
			perror("smash");
			return FAIL;
		}

	}
	
	/*************************************************/
	else if (!strcmp(cmd, "history"))
	{
 		
	}
	/*************************************************/
	
	else if (!strcmp(cmd, "jobs")) 
	{
 		
	}
	/*************************************************/
	else if (!strcmp(cmd, "kill")) 
	{
		
	}
	/*************************************************/
	else if (!strcmp(cmd, "showpid")) 
	{
		
	} 
	/*************************************************/
	else if (!strcmp(cmd, "fg")) 
	{
  		
	}
	/*************************************************/
	else if (!strcmp(cmd, "bg"))
	{
   		
	}
	/*************************************************/
	else if (!strcmp(cmd, "quit"))
	{
   		exit(1);
	}
	/*************************************************/
	else if (!strcmp(cmd, "cp"))
	{
   		
	} 
	/*************************************************/
	else if (!strcmp(cmd, "diff"))
	{
   		
	} 
	/*************************************************/
	else // external command
	{
 		ExeExternal(args, cmdString);
	 	return 0;
	}
	if (illegal_cmd == TRUE)
	{
		printf("smash error: > \"%s\"\n", cmdString);
		return 1;
	}
    return 0;
}
//**************************************************************************************
// function name: ExeExternal
// Description: executes external command
// Parameters: external command arguments, external command string
// Returns: void
//**************************************************************************************
void ExeExternal(char *args[MAX_ARG], char* cmdString)
{
	int pID;
    	switch(pID = fork()) 
	{
    		case -1: 
			break;

        	case 0 :
                // Child Process
               	setpgrp();
			break;
			
			default :
			break;

	}
}

//**************************************************************************************
// function name: BgCmd
// Description: if command is in background, insert the command to jobs
// Parameters: command string, pointer to jobs
// Returns: 0- BG command -1- if not
//**************************************************************************************
int BgCmd(char* lineSize, void* jobs)
{
	char* Command;
	char* delimiters = " \t\n";
	char *args[MAX_ARG];
	if (lineSize[strlen(lineSize)-2] == '&')
	{
		lineSize[strlen(lineSize)-2] = '\0';
		return 0;
	}
	return -1;
}

