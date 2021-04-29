//		commands.c
//********************************************
#include "commands.h"
#include <string>
#include <iostream>
#include <cerrno>
#include <list>
#include "Job.h"
#include "signals.h"

using namespace std;

#define FAIL 1
#define SUCCESS 0
#define SYS_ERR_VAL -1
#define ZERO_ARG 0
#define ONE_ARG 1
#define TWO_ARG 2
#define READ_BUFFER 1024

char* Prev_Path_name = NULL;

// import globals
extern job main_job;
extern list <job> Job_List;
extern list <string> Cmd_History;
//********************************************
// function name: ExeCmd
// Description: interperts and executes built-in commands
// Parameters: pointer to jobs, command string
// Returns: 0 - success,1 - failure
//**************************************************************************************
int ExeCmd(char* lineSize, char* cmdString)
{
	char* cmd; 
	char* args[MAX_ARG];
	char* delimiters = (char*)" \t\n";  
	char* Path_name = NULL;
	int i = 0, num_arg = 0;
	Bool illegal_cmd = FALSE; // illegal command
	string error_msg = "";

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
		// check the num of arguments
		if (num_arg != ZERO_ARG)
		{
			illegal_cmd = TRUE;
			error_msg = "\"" + (string)cmdString + "\"";
		}

		else
		{
			// get the absolute path, the system call will take care of memory allocation
			Path_name = get_current_dir_name(); 
			if (Path_name == NULL) //syscall failed
			{
				perror("smash error");
				return FAIL;
			}
			cout << Path_name << endl;
			free(Path_name);
			return SUCCESS;
		}
	} 
	/*************************************************/
	else if (!strcmp(cmd, "cd")) 
	{
		char *Path_name = NULL;

		if (num_arg != ONE_ARG) {
			illegal_cmd = TRUE;
			error_msg = "\"" + (string)cmdString + "\"";
		} 

		else
		{
			// in case of '-' passed as an argument
			if (!strcmp(args[1], "-"))
			{
				// get the current path name
				Path_name = get_current_dir_name();
				if (Path_name == NULL) //syscall failed
				{
					perror("smash error");
					return FAIL;
				}
				// change the directory to the old path
				if (chdir(Prev_Path_name) == SYS_ERR_VAL) //syscall failed
				{
					int err_check = errno;
					if (err_check == ENOENT)
						cout << "smash error: > " <<"\""<< Prev_Path_name <<"\""<< " - No such file or directory" << endl;
					else
						perror("smash error");
					return FAIL;
				}

				// print the old path
				cout << Prev_Path_name << endl;
				//update the prev path to be the directory we were in
				strcpy(Prev_Path_name,Path_name);
				free(Path_name);
				return SUCCESS;
			}
			else
			{
				// get the current path and put it in prev
				Prev_Path_name = get_current_dir_name();
				if (Prev_Path_name == NULL) //syscall failed
				{
					perror("smash error");
					return FAIL;
				}
				//change the directory to the new path
				if (chdir(args[1]) == SYS_ERR_VAL) //syscall failed
				{
					int err_check = errno;
					if (err_check == ENOENT)
						cout << "smash error: > " << args[1] << " - No such file or directory" << endl;
					else
						perror("smash error");
					return FAIL;
				}
				return SUCCESS;
			}	
		}

	}
	/*************************************************/
	else if (!strcmp(cmd, "history"))
	{
 		// check the num of arguments
		if (num_arg != ZERO_ARG)
		{
			illegal_cmd = TRUE;
			error_msg = "\"" + (string)cmdString + "\"";
		}
		
		else
		{
			// in case the list is empty
			if (!Cmd_History.size())
				return SUCCESS;
			// declare the iterator and go over all the history list
			list <string>::iterator it = Cmd_History.begin();
			for (; it != --Cmd_History.end(); it++)
			{
				cout << *it << endl;
			}
			return SUCCESS;
		}
	}
	/*************************************************/
	else if (!strcmp(cmd, "jobs")) 
	{
 		// check the num of arguments
		if (num_arg != ZERO_ARG)
		{
			illegal_cmd = TRUE;
			error_msg = "\"" + (string)cmdString + "\"";
		}

		else
		{
			// we first remove all the jobs that were done "zombie" state
			Remove_finished_jobs();
			int counter = 0,current_time = 0;
			list<job> ::iterator it = Job_List.begin();
			// go over all the background jobs
			for (counter = 1; it != Job_List.end(); it++, counter++)
			{
				// get the current time
				current_time = time(NULL);
				if (current_time == SYS_ERR_VAL) //syscall failed
				{
					perror("smash error");
					return FAIL;
				}
				// print each job and the details needed
				cout << "[" << counter << "] " << it->get_cmd_name() << " : " << it->get_pid();
				cout << " " << current_time - it->get_time_started_() << " secs";
				// print for a job which got the SIGTSTP signal
				if (it->get_stop_flag())
					cout << " (Stopped)";
				cout << endl;
			}
			return SUCCESS;
		}
	}
	/*************************************************/
	else if (!strcmp(cmd, "kill")) 
	{
		// check the num of arguments
		if (num_arg != TWO_ARG)
		{
			illegal_cmd = TRUE;
			error_msg = "\"" + (string)cmdString + "\"";
		}

		else
		{
			// we first remove all the jobs that were done "zombie" state
			Remove_finished_jobs();
			int job_num = atoi(args[2]) - 1, job_indx = 0, signum = 0;
			string signame;
			list <job> ::iterator it = Job_List.begin();

			//check if job exists
			if (job_num < 0 || job_num >= (int)Job_List.size())
			{
				illegal_cmd = TRUE;
				error_msg = "kill " + (string)args[2] + " - job does not exist";
			}
			// check if the Arguments are illegal
			else if (args[1][0] != '-')
			{
				illegal_cmd = TRUE;
				error_msg = "\"" + (string)cmdString + "\"";
			}
			else
			{
				//find the job in the job list
				if (atoi(args[2]) == 0)
					job_indx = 0;
				else
					job_indx = job_num;
				advance(it, job_indx);

				//manage signal
				signum = -(atoi(args[1]));
				signame = SIG_NUM_TO_NAME(signum);
				if (signame == "" || (signum == 0 && strcmp(args[1], "-0")) || !signal_sending_printing(it->get_pid(), signum))
				{
					illegal_cmd = TRUE;
					error_msg = "kill " + (string)args[2] + " - cannot send signal";
				}

				else
				{
					if (signame == "SIGCONT")
						it->set_stop_flag(FALSE);
					if ((signame == "SIGTSTP") || (signame == "SIGSTOP") || (signame == "SIGSTP")){
						it->set_stop_flag(TRUE);
					}
					if ((signame == "SIGTERM") || (signame == "SIGKILL"))
						Job_List.erase(it);
				}
			}
		}	
	}
	/*************************************************/
	else if (!strcmp(cmd, "showpid")) 
	{
		// check the num of arguments
		if (num_arg != ZERO_ARG)
		{
			illegal_cmd = TRUE;
			error_msg = "\"" + (string)cmdString + "\"";
		}

		else
		{
			// print the pid of the smash job 
			// no need to check the system call it is successful all the time :)
			cout << "smash pid is " << getpid() << endl;
			return SUCCESS;
		}

	} 
	/*************************************************/
	else if (!strcmp(cmd, "fg")) 
	{
		// check the num of arguments
		if (num_arg > ONE_ARG)
		{
			illegal_cmd = TRUE;
			error_msg = "\"" + (string)cmdString + "\"";
		}

		else
		{
			// we first remove all the jobs that were done "zombie" state
			Remove_finished_jobs();

			// check if there are jobs runing in the background
			if (!Job_List.size())
			{
				cout << "smash error: > \"fg\" : Currently there is no jobs in background" << endl;
				return FAIL;
			}

			// check if the job number is legal
			if (num_arg != 0)
			{
				int check = atoi(args[1]);
				// if job index is 0 or bigger then the size of the job list
				if ((check <= 0) || (check > (int)Job_List.size()))
				{
					error_msg = "\"" + (string)cmdString + "\"";
					cout << "smash error: > "<< error_msg << endl;
					return FAIL;
				}
			}

			list <job> ::iterator it = Job_List.begin();
			if (!num_arg)
			{
				main_job = Job_List.back();
				Job_List.pop_back();
			}

			else
			{
				int job_index = (atoi(args[1]) - 1);
				if ((job_index < 0) || (job_index >= (int)Job_List.size()))
				{
					error_msg = "\"" + (string)cmdString + "\"";
					cout << "smash error: > " << error_msg << endl;
					return FAIL;
				}
				advance(it, job_index);
				main_job = *it;
				Job_List.erase(it);
			}

		}	

		cout << main_job.get_cmd_name() << endl;

		// if the job is stopped 
		if (main_job.get_stop_flag())
		{
			if (!signal_sending_printing(main_job.get_pid(), SIGCONT))
				return FAIL;
			main_job.set_stop_flag(FALSE);
		}

		waitpid(main_job.get_pid(), NULL, WUNTRACED);
		main_job.set_pid(-1);
	}
	/*************************************************/
	else if (!strcmp(cmd, "bg"))
	{
		// chcek if there is more then 1 argument
		if (num_arg > ONE_ARG)
		{
			illegal_cmd = TRUE;
			error_msg = "\"" + (string)cmdString + "\"";
		}

		else
		{
			// we first remove all the jobs that were done "zombie" state
			Remove_finished_jobs();

			// check if there are jobs runing in the background
			if (!Job_List.size())
			{
				cout << "smash error: > \"bg\" : Currently there is no jobs in background" << endl;
				return FAIL;
			}

			// check if the job number is legal
			if (num_arg != 0)
			{
				int check = atoi(args[1]);
				// if job index is 0 or bigger then the size of the job list
				if ((check <= 0) || (check > (int)Job_List.size()))
				{
					error_msg = "\"" + (string)cmdString + "\"";
					cout << "smash error: > " << error_msg << endl;
					return FAIL;
				}
			}

			list <job> ::iterator it;
			// we are looking for the last stopped job
			if (!num_arg)
			{
				// we will go over the job list from the end
				for (it = --Job_List.end(); it != Job_List.begin(); it--)
				{
					// if we found a job that was stopped
					if (it->get_stop_flag())
					{
						cout << it->get_cmd_name() << endl;
						if (!signal_sending_printing(it->get_pid(), SIGCONT))
							return FAIL;
						it->set_stop_flag(FALSE);
						break;
					}
				}

				// check if the first job was stopped
				if ((it == Job_List.begin()) && (it->get_stop_flag()))
				{
					cout << it->get_cmd_name() << endl;
					if (!signal_sending_printing(it->get_pid(), SIGCONT))
						return FAIL;
					it->set_stop_flag(FALSE);
				}

				// in case we went over all the list and no jobs were stopped
				else if ((it == Job_List.begin()) && (!it->get_stop_flag()))
					cout << "There were no proccess a sleep" << endl;
			}

			// the argument holds the job num we want to send SIGCONT
			else
			{
				// get the iterator to the right job
				int job_index =  (atoi(args[1]) - 1);
				if ((job_index < 0) || (job_index >= (int)Job_List.size()))
				{
					error_msg = "\"" + (string)cmdString + "\"";
					cout << "smash error: > " << error_msg << endl;
					return FAIL;
				}

				it = Job_List.begin();
				advance(it, job_index);
				// check if the job is a sleep
				if (it->get_stop_flag())
				{
					cout << it->get_cmd_name() << endl;
					if (!signal_sending_printing(it->get_pid(), SIGCONT))
						return FAIL;
					it->set_stop_flag(false);
			
				}

				else
					cout << "job number " << (job_index + 1) << " is not a sleep" << endl;
			}
		}
	}
	/*************************************************/
	else if (!strcmp(cmd, "quit"))
	{
   		// in case only quit was entered
		if (num_arg == ZERO_ARG)
			exit(0);
		// check the num of arguments
		else if (strcmp(args[1], "kill"))
		{
			illegal_cmd = TRUE;
			error_msg = "\"" + (string)cmdString + "\"";
		}
		else if (num_arg > ONE_ARG)
		{
			illegal_cmd = TRUE;
			error_msg = "\"" + (string)cmdString + "\"";
		}
		//quit kill
		else 
		{
			// we first remove all the jobs that were done "zombie" state
			Remove_finished_jobs();
			int job_num = 1;
			list <job> ::iterator it = Job_List.begin();
			// go all over the background list and print the data
			while (it != Job_List.end())
			{
				// the job it is dead
				cout << "[" << job_num << "] " << it->get_cmd_name() << " - Sending SIGTERM... ";
				fflush(stdout);
				if (waitpid(it->get_pid(), NULL, WNOHANG) != 0)
				{
					it++;
					job_num++;
					cout << " Done." << endl;
					continue;
				}
				//job is alive
				//cout << "[" << job_num << "] " << it->get_cmd_name() << " - Sending SIGTERM... ";

				kill(it->get_pid(), SIGTERM);
				sleep(5);

				//job still alive
				if (waitpid(it->get_pid(), NULL, WNOHANG) == 0)
				{
					cout << " (5 sec passed) Sending SIGKILL... ";
					fflush(stdout);
					kill(it->get_pid(), SIGKILL);
				}
				cout << "Done." << endl;
				it++;
				job_num++;
			}
			exit(0);
		}
	}
	/*************************************************/
	else if (!strcmp(cmd, "cp"))
	{
   		// check the num of arguments
		if (num_arg != TWO_ARG)
		{
			illegal_cmd = TRUE;
			error_msg = "\"" + (string)cmdString + "\"";
		}

		else
		{
			// open old file with read only
			int src_fd = open(args[1], O_RDONLY);
			if (src_fd == SYS_ERR_VAL)
			{
				perror("smash error");
				return FAIL;
			}

			// here we create the new file if it is not exist
			int new_fd = open(args[2], O_CREAT | O_WRONLY | O_EXCL);
			int err_check = errno;
			// if the file exist 
			if (err_check == EEXIST)
			{
				// delete the file and check system call
				if(unlink(args[2]) == SYS_ERR_VAL)
				{
					perror("smash error");
					return FAIL;
				}
				// now we know the file is deleted and we can create it
				new_fd = open(args[2], O_CREAT | O_WRONLY);
			}
			
			if (new_fd == SYS_ERR_VAL)
			{
				perror("smash error");
				return FAIL;
			}

			char buffer[READ_BUFFER];
			ssize_t read_size = 1, write_size = 1;
			// while we still read from the old file
			while (read_size != 0)
			{
				read_size = read(src_fd, buffer, sizeof(char) * READ_BUFFER);
				if (read_size == SYS_ERR_VAL)
				{
					perror("smash error");
					return FAIL;
				}

				write_size = write(new_fd, buffer, sizeof(char)* read_size);
				if (write_size == SYS_ERR_VAL)
				{
					perror("smash error");
					return FAIL;
				}
			}

			// here we close the files
			if ((close(src_fd) == SYS_ERR_VAL) || (close(new_fd) == SYS_ERR_VAL))
			{
				perror("smash error");
				return FAIL;
			}

			cout << args[1] << " has been copied to " << args[2] << endl;
			return SUCCESS;
		}
	} 
	/*************************************************/
	else if (!strcmp(cmd, "diff"))
	{
   		// check the num of arguments
		if (num_arg != TWO_ARG)
		{
			illegal_cmd = TRUE;
			error_msg = "\"" + (string)cmdString + "\"";
		}

		else
		{
			// open the first file with read only
			int file1_fd = open(args[1], O_RDONLY);
			if (file1_fd == SYS_ERR_VAL)
			{
				perror("smash error");
				return FAIL;
			}

			// open the second file with read only
			int file2_fd = open(args[2], O_RDONLY);
			if (file2_fd == SYS_ERR_VAL)
			{
				perror("smash error");
				return FAIL;
			}

			ssize_t file1_size, file2_size;
			char buffer1[READ_BUFFER + 1],buffer2[READ_BUFFER + 1];
			bool is_diff;
			// we read from both files in a while loop
			do
			{
				file1_size = read(file1_fd, buffer1, sizeof(char) * READ_BUFFER);
				if (file1_size == SYS_ERR_VAL)
				{
					perror("smash error");
					return FAIL;
				}
				buffer1[file1_size] = '\0';

				file2_size = read(file2_fd, buffer2, sizeof(char) * READ_BUFFER);
				if (file2_size == SYS_ERR_VAL)
				{
					perror("smash error");
					return FAIL;
				}
				buffer2[file2_size] = '\0';
				// in case we read diffrent number of bytes
				if (file1_size != file2_size)
				{
					is_diff = true;
					break;
				}

				// check if the data we read is the same
				is_diff = strcmp(buffer1, buffer2);
				// if not the same break
				if (is_diff)
					break;
			} 
			while ((file1_size != 0) && (file2_size != 0));

			// here we close the files
			if ((close(file1_fd) == SYS_ERR_VAL) || (close(file2_fd) == SYS_ERR_VAL))
			{
				perror("smash error");
				return FAIL;
			}
			cout << is_diff << endl;
			return SUCCESS;
		}
	} 
	/*************************************************/
	else // external command
	{
 		ExeExternal(args, cmdString);
	 	return 0;
	}

	if (illegal_cmd == TRUE)
	{
		cout <<"smash error: > " << error_msg << endl;
		return FAIL;
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
	int pID = 0;
    switch(pID = fork()) 
	{
    	case SYS_ERR_VAL:
			perror("smash error");
			exit(1);
		break;
	
    	case 0 :
    	    // Child Process
    	   	setpgrp();
			execvp(args[0], args);
			perror("smash error");
			exit(1);
		break;

		default :
			string ext_cmd(cmdString);
			main_job.set_pid(pID);
			main_job.set_cmd_name(ext_cmd);
			main_job.set_stop_flag(false);

			waitpid(pID, NULL, WUNTRACED);
			main_job.set_pid(-1);
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
	char* delimiters = (char*)" \t\n";
	char *args[MAX_ARG];
	int i = 0, num_arg = 0;
	if (lineSize[strlen(lineSize) - 2] == '&')
	{
		lineSize[strlen(lineSize) - 2] = '\0';
		int pID;
		Command = strtok(lineSize, delimiters);
		if (Command == NULL)
			return 0;
		args[0] = Command; // first arg is the command name
		// the followings are the command arguments
		for (i = 1; i < MAX_ARG; i++)
		{
			args[i] = strtok(NULL, delimiters);
			if (args[i] != NULL)
				num_arg++;
		}

		switch (pID = fork())
		{
		// in case fork failed and no child was created
		case SYS_ERR_VAL:
			perror("smash error");
			exit(1);
			break;
		// this case is the child
		case 0:
			setpgrp();
			execvp(args[0], args);
			perror("smash error");
			exit(0);
			break;
		// parent
		default:
			string bg_cmd(args[0]);
			job bg_job(pID, bg_cmd, false, (int)time(NULL));

			Job_List.push_back(bg_job);
			Remove_finished_jobs();

			return 0;
			break;
		}
	}
	return -1;
}

