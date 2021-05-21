#include "Bank-Account.h"

Bank_Account::Bank_Account() 
    :_acc_num(0),_pass(0),_balance(0),_read_count(0) {
        sem_init_check(&_read_lock);
        sem_init_check(&_write_lock)
    }

Bank_Account::Bank_Account(int account, int password, int initial_amount) 
    :_acc_num(account),_pass(password),_balance(initial_amount),_read_count(0) {
        sem_init_check(&_read_lock);
        sem_init_check(&_write_lock);
    }

Bank_Account::~Bank_Account() {
    sem_destroy_check(&_read_lock);
    sem_destroy_check(&_write_lock);
}

int Bank_Account::get_account_num() const {
    return _acc_num;
}

int Bank_Account::get_balance() const {
    return _balance;
}

int Bank_Account::get_pass() const {
    return _pass;
}

int Bank_Account::get_read_count() const {
    return _read_count;
}

int Bank_Account::set_balance(int amount) {
    _balance += amount;
}

void Bank_Account::account_write_lock() {
    sem_wait_check(&_write_lock);

}

void Bank_Account::account_read_lock() {
    // a thread is accessing the balance so we will lock the read
    sem_wait_check(&_read_lock);
    // now it is safe to add a value to the read count
    _read_count++;
    // if i am the first reader thread
    if (_read_count == FIRST_READER) {
        // lock the write option so we wont read while writing
        sem_wait_check(&_write_lock);
    }
    // unlock the read option so other's may also access the information
    sem_post_check(&_read_lock);
}

void Bank_Account::account_write_unlock() {
    sem_post_check(&_write_lock);
}

void Bank_Account::account_read_unlock() {
    // the thread is done and ready to realese the lock
    sem_wait_check(&_read_lock);
    // now it is safe to substruct a value from the read count
    _read_count--;
    // if i am the only reader that is using it
    if (_read_count == LAST_READER) {
        // unlock the write option
        sem_post_check(&_write_lock);
    }
    // unlock the read option so other's may also access the information
    sem_post_check(&_read_lock);
}

bool Bank_Account::operator<(const Bank_Account& lhs, const Bank_Account& rhs)
{
	if (lhs._acc_num < rhs._acc_num)
		return true;
	return false;
}

void sem_init_check(sem_t* s) {
    // initialize the writer lock to 1 (open)
    if (sem_init(s, 0, 1)) { 
		perror("semaphore_init");
		exit(1);
	}
}
void sem_destroy_check(sem_t* s) {
    if (sem_destroy(s)) {
		perror("semaphore_destroy");
		exit(1);
	}
}

void sem_wait_check(sem_t* s) {
    if (sem_wait(s)) {
		perror("semaphore wait");
		exit(1);
	}
}
void sem_post_check(sem_t* s) {
    if (sem_post(s)) {
		perror("semaphore post");
		exit(1);
	}
}
