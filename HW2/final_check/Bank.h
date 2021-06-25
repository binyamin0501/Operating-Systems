#ifndef _BANK_H_
#define _BANK_H_

#include <map>
#include <unistd.h>
#include <iostream>
#include <math.h>
#include "Bank-Account.h"
#include "Log-File.h"

#define PRINT_SLEEP 500000
#define BANK_COMMISION_SLEEP 3
#define MIN_COMMISSION 2
#define MAX_COMMISSION 4

using namespace std;

class Bank {
    int _bank_balance;
    int _read_count;
    sem_t _read_lock;
    sem_t _write_lock;
    map<int, Bank_Account*> _account_list;

public:
    Bank();
    ~Bank();

    int get_bank_amount() const;
    void set_bank_balance(int amount);

    void bank_write_lock();
    void bank_read_lock();
    void bank_write_unlock();
    void bank_read_unlock();

    void add_new_account(int account, Bank_Account *new_acc);

    map<int, Bank_Account*>::iterator get_begin();
    map<int, Bank_Account*>::iterator get_end();

    Bank_Account& get_account(int account);
    void delete_account(int account);
    
};

void* Bank_Printing_func(void* is_open);
void* Charge_Commission_func(void* is_open);

//FIXME meybe use singelton ?
extern Bank* bank;
extern Log_File* Bank_Log;
extern bool is_open;

#endif //!_BANK_H_