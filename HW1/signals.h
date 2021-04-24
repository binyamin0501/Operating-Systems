#ifndef _SIGS_H
#define _SIGS_H
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h> 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <signal.h>
#include "Job.h"
#include <iostream>
#include<ostream>

void catch_C_int(int sig_num);
void catch_Z_int(int sig_num);

// functions to manage signals
bool signal_sending_printing(pid_t pid, int signal_number);
void signal_printing(pid_t pid, int signal_number);
string SIG_NUM_TO_NAME(int signum);

#endif

