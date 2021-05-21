#ifndef _LOG_FILE_
#define _LOG_FILE_

#include <iostream>
#include <fstream>
#include <pthread.h>

using namespace std;

class Log_File
{
    pthread_mutex_t _log_mutex;
public:
    ofstream _log;

    Log_File() {
        _log.open("log.txt");
        if (pthread_mutex_init(&_log_mutex, NULL)) {
			perror("pthread_mutex_init");
			exit(1);
        }
    }

    ~Log_File() {
        _log.close();
        if (pthread_mutex_destroy(&_log_mutex)) {
			perror("pthread_mutex_destroy");
			exit(1);
        }
    }

    void lock_log_file() {
        if (pthread_mutex_lock(&_log_mutex)) {
			perror("pthread_mutex_lock");
			exit(1);
        }
    }

    void unlock_log_gile() {
        if (pthread_mutex_unlock(&_log_mutex)) {
			perror("pthread_mutex_unlock");
			exit(1);
        }
    }
};


#endif  //!_LOG_FILE_
