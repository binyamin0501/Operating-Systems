#include "Job.h"
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h> 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <list>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include<ostream>

extern list <job> Job_List;

//removes finished jobs from job list
void Remove_finished_jobs()
{
	// go over all the job list in the background
	list <job> ::iterator it = Job_List.begin();
	for (; it != Job_List.end(); it++)
	{
		// if the job is still alive
		if (waitpid(it->get_pid(), NULL, WNOHANG) != 0)
		{
			Job_List.erase(it);
			it = Job_List.begin();
		}
	}
}