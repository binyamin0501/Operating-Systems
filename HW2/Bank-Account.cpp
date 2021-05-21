#include "Bank-Account.h"

Bank_Account::Bank_Account() 
:_acc_num(0),_pass(0),_balance(0),_read_count(0){}

Bank_Account::Bank_Account(int account, int password, int initial_amount) {
    
}

Bank_Account::~Bank_Account();
int Bank_Account::get_account_num() const;
int Bank_Account::get_balance() const;
int Bank_Account::get_pass() const;
int Bank_Account::get_read_count() const;
int Bank_Account::set_balance();

void Bank_Account::account_write_lock();
void Bank_Account::accoun_read_lock();
void Bank_Account::account_write_unlock();
void Bank_Account::accoun_read_unlock();