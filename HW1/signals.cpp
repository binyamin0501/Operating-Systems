// signals.c
// contains signal handler funtions
// contains the function/s that set the signal handlers

/*******************************************/
/* Name: handler_cntlc
   Synopsis: handle the Control-C */
#include "signals.h"

using namespace std;
// import globals
extern job main_job;
extern list<job> Job_List;

void catch_C_int(int sig_num) {
   printf("Ctrl + C\n");
   // check that the job is valid and the SIGINT signal was sent
	if ((main_job.get_pid() == -1) || (sig_num != SIGINT))
		return;
	// check if the job is still alive
	if (!waitpid(main_job.get_pid(), NULL, WNOHANG))
	{
		if (!signal_sending_printing(main_job.get_pid(), SIGINT))
			return;
	}
}

void catch_Z_int(int sig_num) {
   printf("Ctrl + Z\n");
   // check that the job is valid and the SIGSTP signal was sent
	if ((main_job.get_pid() == -1) || (sig_num != SIGTSTP)) return;
	// check if the job still alive
	if (waitpid(main_job.get_pid(), NULL, WNOHANG) == 0)
	{
		// send the SIGTSTP to the job in the fg and print the msg
		if (!signal_sending_printing(main_job.get_pid(), SIGTSTP))
			return;
		// create the new background job and put it in the background list
		job new_job_to_bg(main_job.get_pid(), main_job.get_cmd_name(), true, (int)time(NULL));
		Job_List.push_back(new_job_to_bg);
		main_job.set_pid(-1);
	}
}

//sends a signal and prints a message with the signals info
bool signal_sending_printing(pid_t pid, int signal_number)
{
    if (!kill(pid, signal_number))
    {
		signal_printing(pid, signal_number);
        return SUCCESS;
    }
    perror("smash");
    return FAIL;
}
// gets a signal number and return his name
string SIG_NUM_TO_NAME(int signum) {
	switch (signum) {
	case 1: return "SIGHUP";
		break;
	case 2: return "SIGINT";
		break;
	case 3: return "SIGQUIT";
		break;
	case 4: return "SIGILL";
		break;
	case 5: return "SIGTRACE";
		break;
	case 6: return "SIGABORT";
		break;
	case 7: return "SIGBUS";
		break;
	case 8: return "SIGFLOAT";
		break;
	case 9: return "SIGKILL";
		break;
	case 10: return "SIGUSER1";
		break;
	case 11: return "SIGSEGFAULT";
		break;
	case 12: return "SIGUSER2";
		break;
	case 13: return "SIGBROKENPIPE";
		break;
	case 14: return "SIGALARMCLK";
		break;
	case 15: return "SIGTERM";
		break;
	case 16: return "SIGSTACK";
		break;
	case 17: return "SIGCHILD";
		break;
	case 18: return "SIGCONT";
		break;
	case 19: return "SIGSTOP";
		break;
	case 20: return "SIGSTP";
		break;
	case 21: return "SIGSTOPTTYIN";
		break;
	case 22: return "SIGSTOPTTYOUT";
		break;
	case 23: return "SIGURGENTIO";
		break;
	case 24: return "SIGCPUTIME";
		break;
	case 25: return "SIGFILESIZE";
		break;
	case 26: return "SIGVIRTIMER";
		break;
	case 27: return "SIGPROFTIMER";
		break;
	case 28: return "SIGWINDOW";
		break;
	case 29: return "SIGIOPOSIBLE";
		break;
	case 30: return "SIGPOWER";
		break;
	case 31: return "SIGBADSYSCALL";
		break;
	default: return "";
	}
	return NULL;
}

void signal_printing(pid_t pid, int signal_number)
{
	cout << endl;
	cout << "smash > signal " << SIG_NUM_TO_NAME(signal_number) << " was sent to pid " << pid << endl;
}
