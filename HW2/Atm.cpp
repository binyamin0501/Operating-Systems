#include "Atm.h"

Atm::Atm() 
    :_atm_num(INVALID) {}
    
Atm::~Atm() {}

int Atm::get_atm_num() const {
    return _atm_num;
}

string Atm::get_atm_input() const {
    return _atm_input;
}

void Atm::set_atm_num(int atm_num) {
    _atm_num = atm_num;
}
void Atm::set_atm_input(const char* input) {
    _atm_input = string(input);
}

void* ATM_thread_func (void* atm) {
    Atm *curr_atm = (Atm*)atm;
    ifstream input_file(curr_atm->get_atm_input().c_str());
    // check if the file is alredy open
	if (!input_file.is_open())
	{
		cerr << "input file cannot be opened" << endl;
		exit(ERROR);
	}
    
    char *Operation = NULL;
	int args[MAX_ARG];
	string line ;

    while (getline(input_file, line)) {
        if (!line.length()) {
            continue;
        }
        Parse_atm_line(Operation, args, line);

        switch (*Operation)
        {
        case 'O':
            curr_atm->Open_new_account(args[0], args[1], args[2]);
            break;
        case 'D':
            curr_atm->Deposit(args[0], args[1], args[2]);
            break;
        case 'W':
            curr_atm->Withdraw(args[0], args[1], args[2]);
            break;
        case 'B':
            curr_atm->get_account_balance(args[0], args[1]);
            break;
        case 'C':
            curr_atm->Close_account(args[0], args[1]);
            break;
        case 'T':
            curr_atm->Transaction(args[0], args[1], args[2], args[3]);
            break;
        default:
            cerr << "illegal operation" << endl;
            exit(ERROR);
            break;
        }

        usleep(ATM_SLEEP);
    }
    pthread_exit(NULL);
}

void Atm::Open_new_account(int account, int password, int initial_amount) {
    bank->bank_write_lock();
    sleep(ACTION_SLEEP);
    // check if the account exist
    if(Check_account_exist(account)) {
        Bank_Log->lock_log_file();
        Bank_Log->_log << "Error " << this->_atm_num << ": Your transaction failed - account with the same id exists " << endl;
        Bank_Log->unlock_log_gile();
    }

    // account does not exist
    else {
        Bank_Account *new_acc = new Bank_Account(account, password, initial_amount);
        bank->add_new_account(account, new_acc);
        Bank_Log->lock_log_file();
        Bank_Log->_log << this->_atm_num << ": New account id is " << account << "  with password  "
        << password << " and initial balance " << initial_amount << endl;
        Bank_Log->unlock_log_gile();
    }
    bank->bank_write_unlock();
}

void Atm::Deposit(int account, int password, int amount) {
    bank->bank_read_lock();
    sleep(ACTION_SLEEP);
    // check if the account does not exist
    if(!Check_account_exist(account)) {
        Bank_Log->lock_log_file();
		Bank_Log->_log << "Error " << this->_atm_num << ": Your transaction failed - account id "<< account <<" does not exists" << endl;
		Bank_Log->unlock_log_gile();
    }
    //account exist
    //check if the password provided is correct
    else {
        if (Check_password(account, password)) {
            //password match
            Bank_Account &curr = bank->get_account(account);
            curr.account_write_lock();
            curr.set_balance(amount);
            Bank_Log->lock_log_file();
		    Bank_Log->_log << this->_atm_num << ": Account " << account << " new balance is "
            << curr.get_balance() << " after " << amount <<" $ was deposited " << endl;
		    Bank_Log->unlock_log_gile();
            curr.account_write_unlock();
        }

        else {
            //password does not match
            Bank_Log->lock_log_file();
		    Bank_Log->_log << "Error " << this->_atm_num << ": Your transaction failed - password for account id "
            << account <<" is incorrect" << endl;
		    Bank_Log->unlock_log_gile();
        }
    }
    bank->bank_read_unlock();
}

void Atm::Withdraw(int account, int password, int amount) {
    bank->bank_read_lock();
    sleep(ACTION_SLEEP);
    // check if the account does not exist
    if(!Check_account_exist(account)) {
        Bank_Log->lock_log_file();
		Bank_Log->_log << "Error " << this->_atm_num << ": Your transaction failed - account id "<< account <<" does not exists" << endl;
		Bank_Log->unlock_log_gile();
    }

    //account exist
    //check if the password provided is correct
    else {
        if (Check_password(account, password)) {
            //password match
            Bank_Account &curr = bank->get_account(account);
            curr.account_write_lock();
            //check if the account balance is grater then the amount asked to withdraw
            if (curr.get_balance() >= amount) {
                // the balance is sufficient
                curr.set_balance(-amount);
                Bank_Log->lock_log_file();
		        Bank_Log->_log << this->_atm_num << ": Account " << account << " new balance is "
                << curr.get_balance() << " after " << amount <<" $ was withdrew " << endl;
		        Bank_Log->unlock_log_gile();
            }

            else {
                // amount requested to withdraw is grater then account balance
                Bank_Log->lock_log_file();
		        Bank_Log->_log << "Error " << this->_atm_num << ": Your transaction failed - account id "
                << account <<" balance is lower than " << amount << endl;
		        Bank_Log->unlock_log_gile();
            }

            curr.account_write_unlock();
        }

        else {
            //password does not match
            Bank_Log->lock_log_file();
		    Bank_Log->_log << "Error " << this->_atm_num << ": Your transaction failed - password for account id "
            << account <<" is incorrect" << endl;
		    Bank_Log->unlock_log_gile();
        }
    }
    bank->bank_read_unlock();

}

void Atm::get_account_balance(int account, int password) {
    bank->bank_read_lock();
    sleep(ACTION_SLEEP);
    // check if the account does not exist
    if(!Check_account_exist(account)) {
        Bank_Log->lock_log_file();
		Bank_Log->_log << "Error " << this->_atm_num << ": Your transaction failed - account id "<< account <<" does not exists" << endl;
		Bank_Log->unlock_log_gile();
    }
    //account exist
    //check if the password provided is correct
    else {
        if (Check_password(account, password)) {
            //password match
            Bank_Account &curr = bank->get_account(account);
            curr.account_write_lock();
            Bank_Log->lock_log_file();
		    Bank_Log->_log << this->_atm_num << ": Account " << account << "balance is " << curr.get_balance() << endl;
		    Bank_Log->unlock_log_gile();
            curr.account_write_unlock();
        }

        else {
            //password does not match
            Bank_Log->lock_log_file();
		    Bank_Log->_log << "Error " << this->_atm_num << ": Your transaction failed - password for account id "
            << account <<" is incorrect" << endl;
		    Bank_Log->unlock_log_gile();
        }
    }
    bank->bank_read_unlock();
}

void Atm::Close_account(int account, int password) {
    bank->bank_write_lock();
    sleep(ACTION_SLEEP);
    // check if the account does not exist
    if(!Check_account_exist(account)) {
        Bank_Log->lock_log_file();
		Bank_Log->_log << "Error " << this->_atm_num << ": Your transaction failed - account id "<< account <<" does not exists" << endl;
		Bank_Log->unlock_log_gile();
    }
    //account exist
    //check if the password provided is correct
    else {
        if (Check_password(account, password)) {
            //password match
            Bank_Account &curr = bank->get_account(account);
            //FIXME what happens when i delete this account sempahore ?
            // meybe hold the write semaphore ?
            curr.account_read_lock();
            Bank_Log->lock_log_file();
		    Bank_Log->_log << this->_atm_num << ": Account " << account << "is now closed. Balance was " << curr.get_balance() << endl;
		    Bank_Log->unlock_log_gile();
            bank->delete_account(account);
        }

        else {
            //password does not match
            Bank_Log->lock_log_file();
		    Bank_Log->_log << "Error " << this->_atm_num << ": Your transaction failed - password for account id "
            << account <<" is incorrect" << endl;
		    Bank_Log->unlock_log_gile();
        }
    }
    bank->bank_write_unlock();
}

void Atm::Transaction(int account, int password, int target_account, int amount) {
    bank->bank_read_lock();
    sleep(ACTION_SLEEP);
    // check if the account does not exist
    if(!Check_account_exist(account) || !Check_account_exist(target_account)) {
        Bank_Log->lock_log_file();
		Bank_Log->_log << "Error " << this->_atm_num << ": Your transaction failed - account id "<< account <<" does not exists" << endl;
		Bank_Log->unlock_log_gile();
    }

    //account exist
    //check if the password provided is correct
    else {
        //password match

        if (Check_password(account, password)) {
            Bank_Account &from = bank->get_account(account);
            Bank_Account &to = bank->get_account(target_account);

            //we will lock by order to prevent deadlock
            if (account == target_account) {
                bank->bank_read_unlock();
                return;
            }

            else if (account < target_account) {
                from.account_write_lock();
                to.account_write_lock();
            }

            else {
                to.account_write_lock();
                from.account_write_lock();
            }

            //check if the account balance is grater then the amount asked to transfer
            if (from.get_balance() >= amount) {
                // the balance is sufficient
                from.set_balance(-amount);
                to.set_balance(amount);
                Bank_Log->lock_log_file();
		        Bank_Log->_log << this->_atm_num << ": Transfer " << amount << " from account "<< account << " to account "  
                << target_account << " new account balance is "<< from.get_balance() << " new target account balance is " << to.get_balance()<< endl;
		        Bank_Log->unlock_log_gile();

            }

            else {
                // amount requested to withdraw is grater then account balance
                Bank_Log->lock_log_file();
		        Bank_Log->_log << "Error " << this->_atm_num << ": Your transaction failed - account id "
                << account <<" balance is lower than " << amount << endl;
		        Bank_Log->unlock_log_gile();
            }

            // unlock the accounts - the order does'nt matter
            from.account_write_unlock();
            to.account_write_unlock();
        }

        else {
            //password does not match
            Bank_Log->lock_log_file();
		    Bank_Log->_log << "Error " << this->_atm_num << ": Your transaction failed - password for account id "
            << account <<" is incorrect" << endl;
		    Bank_Log->unlock_log_gile();
        }
    }
    bank->bank_read_unlock();

}

void Parse_atm_line(char* Operation, int args[MAX_ARG], string line) {
    char* cmd = NULL, *token = NULL; 
	char* delimiters = (char*)" "; 
	int i = 0;
    cmd = new char[strlen(line.c_str()) + 1];
    strcpy(cmd,line.c_str());
    Operation = strtok(cmd, delimiters);
	token = strtok(NULL, delimiters);
    while (token) {
        args[i] = atoi(token);
        token = strtok(NULL, delimiters);
        i++;
    }
    delete[] cmd;
}

bool Check_account_exist(int account) {
    map<int, Bank_Account*>::iterator it = bank->get_begin();
    for(; it != bank->get_end(); it++) {
        if (it->second->get_account_num() == account) {
            return true;
        }
    }
    return false;
}

bool Check_password(int account, int password) {
    Bank_Account &acc = bank->get_account(account);
    if (acc.get_pass() == password) {
        return true;
    }
    return false;
}