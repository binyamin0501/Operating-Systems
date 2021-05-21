#include "Bank.h"

// this function clear the screen and start writing from the left top corner
static void print_left_top_corner()
{
	printf("\033[2J");
	printf("\033[1;1H");
}

Bank::Bank() 
 :_bank_balance(0),_read_count(0) {
        sem_init_check(&_read_lock);
        sem_init_check(&_write_lock);
}

Bank::~Bank() {
    sem_destroy_check(&_read_lock);
    sem_destroy_check(&_write_lock);

    map<int, Bank_Account*>::iterator it = _account_list.begin();
    for (; it != _account_list.end(); it++) {
        delete it->second;
    }

}

void Bank::bank_write_lock() {
    sem_wait_check(&_write_lock);
}

void Bank::bank_read_lock() {
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
void Bank::bank_write_unlock() {
    sem_post_check(&_write_lock);
}

void Bank::bank_read_unlock() {
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

int Bank::get_bank_amount() const {
    return _bank_balance;
}

void Bank::set_bank_balance(int amount) {
    _bank_balance += amount;
}

map<int, Bank_Account*>::iterator Bank::get_begin() {
    return _account_list.begin();
}

map<int, Bank_Account*>::iterator Bank::get_end() {
    return _account_list.end();
}

void* Bank_Printing_func(void* is_open) {
    // do while the bank is open
	while (*((bool*)is_open)) {
        usleep(PRINT_SLEEP);
        print_left_top_corner();

        cout << "Current Bank Status" << endl;
        bank->bank_read_lock();

        map<int, Bank_Account*>::iterator it = bank->get_begin();
        for (; it != bank->get_end(); it++) {
            it->second->account_read_lock();
            cout << "Account " << it->second->get_account_num() << ": Balance - ";
	    	cout << it->second->get_balance() << " $ , Account Password - " << it->second->get_pass() << endl;
            it->second->account_read_unlock();
        }

        // print the bank amount
		cout << "The Bank has " << bank->get_bank_amount() << " $" << endl;
        bank->bank_read_unlock();
    }
    pthread_exit(NULL);
}

void* Charge_Commission_func(void* is_open) {
    double commission_percentage = 0.0, charge_amount = 0.0;

    // do while the bank is open
	while (*((bool*)is_open)) {
        sleep(BANK_COMMISION_SLEEP);
        // randomize a number between 2-4
		commission_percentage = (rand() % (MIN_COMMISSION + 1)) + (double)(MAX_COMMISSION - MIN_COMMISSION);

        bank->bank_read_lock();

        map<int, Bank_Account*>::iterator it = bank->get_begin();
        for (; it != bank->get_end(); it++) {
            it->second->account_write_lock();
            charge_amount = round(commission_percentage * 0.01 * it->second->get_balance());
            it->second->set_balance(-charge_amount);
            // meybe write unlock and read lock?
            Bank_Log->lock_log_file();
            Bank_Log->_log << "Bank: commissions of " << commission_percentage
			<< " % were charged, the bank gained " << charge_amount << " $ from account " << it->second->get_account_num() << endl;
            Bank_Log->unlock_log_gile();
            it->second->account_write_unlock();
            bank->set_bank_balance(charge_amount);
        }
        bank->bank_read_unlock();
    }
    pthread_exit(NULL);
}
