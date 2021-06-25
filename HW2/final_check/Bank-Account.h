#ifndef BANK_ACCOUNT_H_
#define BANK_ACCOUNT_H_

#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>

#define FIRST_READER 1
#define LAST_READER 0

class Bank_Account {
    int _acc_num;
    int _pass;
    int _balance;
    int _read_count;
    sem_t _read_lock;
    sem_t _write_lock;

public:

	//*******************************************************************
	// function name: Bank_Account (constructor)
	// Description: Defult C'tor
	// Parameters: none
	// Returns: none
	//*******************************************************************
    Bank_Account();

    //*******************************************************************
	// function name: Bank_Account (constructor)
	// Description: Parmetrize C'tor
	// Parameters: account - account number, password - account password, initial_amount - the starting amount for this account
	// Returns: none
	//*******************************************************************
    Bank_Account(int account, int password, int initial_amount);
    ~Bank_Account();

    int get_account_num() const;
    int get_balance() const;
    int get_pass() const;
    int get_read_count() const;

    void set_balance(int amount);

    void account_write_lock();
    void account_read_lock();
    void account_write_unlock();
    void account_read_unlock();

    bool operator<(const Bank_Account& rhs);
};

void sem_init_check(sem_t* s);
void sem_destroy_check(sem_t* s);
void sem_wait_check(sem_t* s);
void sem_post_check(sem_t* s);

#endif // !BANK_ACCOUNT_H_ 