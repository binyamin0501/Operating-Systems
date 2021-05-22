#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <ostream>
#include <unistd.h>
#include <pthread.h>

#include "Bank-Account.h"
#include "Bank.h"
#include "Log-File.h"
#include "Atm.h"

using namespace std;

Bank* bank;
Log_File* Bank_Log;
bool is_open;

#define FAIL 1

int main(int argc, char const *argv[])
{
    if ((argc <= 1) || (argc - 2 != atoi(argv[1]))) {
        cout << "illegal arguments" << endl;
        return FAIL;
    }

    int Num_of_atm = 0, i = 0;
    pthread_t* thread_id_arr;
    Atm *atm_arr;

    bank = new Bank();
    Bank_Log = new Log_File();
    is_open = true;

    Num_of_atm = atoi(argv[1]);
	
	// alocate the thread id arr and the thread atm_arr
	thread_id_arr = new pthread_t[Num_of_atm + 2];
	atm_arr = new Atm[Num_of_atm];

    for (; i < Num_of_atm; i++) {
        atm_arr[i].set_atm_input(argv[i+2]);
        atm_arr[i].set_atm_num(i);
        if (pthread_create(&thread_id_arr[i], NULL, &ATM_thread_func, (void*)&atm_arr[i])) {
            perror("pthread_create");
            if (thread_id_arr) delete[]thread_id_arr;
			if (atm_arr) delete[]atm_arr;
			exit(ERROR);
        }
    }

    // create the commision thread
	if (pthread_create(&thread_id_arr[Num_of_atm], NULL, &Charge_Commission_func, (void*)(&is_open)) != 0)
	{
        perror("pthread_create");
        if (thread_id_arr) delete[]thread_id_arr;
		if (atm_arr) delete[]atm_arr;
		exit(ERROR);
	}

	// create the printing thread
	if (pthread_create(&thread_id_arr[Num_of_atm + 1], NULL, &Bank_Printing_func, (void*)(&is_open)) != 0)
	{
        perror("pthread_create");
        if (thread_id_arr) delete[]thread_id_arr;
		if (atm_arr) delete[]atm_arr;
		exit(ERROR);
	}

    // wait for all the ATM thread's to call pthread_exit(NULL);
	for (int i = 0; i < Num_of_atm; i++)
	{
		if (pthread_join(thread_id_arr[i], NULL)) {
            perror("pthread_join");
            if (thread_id_arr) delete[]thread_id_arr;
		    if (atm_arr) delete[]atm_arr;
		    exit(ERROR);
        }
	}

    // closing the Bank - closing time, time for you to go home..... (love the song :))
	// set the is_open to false in order to finish the job of Bank_Commission and Print_thread
	is_open = false;
	// wait for Bank_Commission thread to call pthread_exit(NULL);
	if(pthread_join(thread_id_arr[Num_of_atm], NULL)) {
            perror("pthread_join");
            if (thread_id_arr) delete[]thread_id_arr;
		    if (atm_arr) delete[]atm_arr;
		    exit(ERROR);
    }

	// wait for Print_thread thread to call pthread_exit(NULL);
	if(pthread_join(thread_id_arr[Num_of_atm + 1], NULL)) {
            perror("pthread_join");
            if (thread_id_arr) delete[]thread_id_arr;
		    if (atm_arr) delete[]atm_arr;
		    exit(ERROR);
    }

    delete[] thread_id_arr;
    delete[] atm_arr;
    delete Bank_Log;
    delete bank;

    return 0;
}
