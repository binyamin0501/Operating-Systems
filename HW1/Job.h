#ifndef JOB_H_
#define JOB_H_

#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h> 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <list>
#include <signal.h>

#define DEFULT_PID 0
#define DEFULT_START_TIME -1
using namespace std;

typedef enum { FAIL, SUCCESS } Result;

class job {
public:
	// defult c'tor - no paramters
	job():
		pid_(DEFULT_PID), cmd_name_(""), stop_flag_(true), time_started_(DEFULT_START_TIME) {}

	// Parmetrize c'tor
	job(pid_t pid, string cmd_name, bool stop_flag, int time_started) :
		pid_(pid), cmd_name_(cmd_name), stop_flag_(stop_flag), time_started_(time_started) {}

	// copy c'tor
	job(const job& rhs)
	{
		pid_ = rhs.pid_;
		cmd_name_ = rhs.cmd_name_;
		stop_flag_ = rhs.stop_flag_;
		time_started_ = rhs.time_started_;
	}

	job& operator=(const job& rhs)
	{
		if (this != &rhs)
		{
			pid_ = rhs.pid_;
			cmd_name_ = rhs.cmd_name_;
			stop_flag_ = rhs.stop_flag_;
			time_started_ = rhs.time_started_;
		}
		return (*this);
	}
    
	// get functions
	pid_t get_pid() const { return pid_; }
	string get_cmd_name() const { return cmd_name_; }
	bool get_stop_flag() const { return stop_flag_; }
	int get_time_started_()const { return time_started_; }

	// set functions
	void set_pid(pid_t new_pid) { pid_ = new_pid; }
	void set_cmd_name(string new_cmd) { cmd_name_ = new_cmd; }
	void set_stop_flag(bool new_stop_flag) { stop_flag_ = new_stop_flag; }
	void set_time_started(int new_time_started) { time_started_ = new_time_started; }

private:
	pid_t pid_;
	string cmd_name_;
	bool stop_flag_;
	int time_started_;
};


// helper function
void Remove_finished_jobs();

#endif // !JOB_H_

