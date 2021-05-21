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

void* Atm::ATM_thread_func (void* atm) {
    Atm *curr_atm = (Atm*)atm;
    ifstream input_file(curr_atm->get_atm_input().c_str());
    // check if the file is alredy open
	if (!input_file.is_open())
	{
		cerr << "input file cannot be opened" << endl;
		exit(ERROR);
	}

    char* cmd = NULL; 
	int args[MAX_ARG];
	char* delimiters = (char*)" "; 
	int i = 0;
	string line ;

    while (getline(input_file, line)) {
        if (!line.length()) {
            continue;
        }
        
	    cmd = strtok(, delimiters);

    }

	if (cmd == NULL)
		return 0; 
   	args[0] = cmd; // first arg is the command name
	// the following are the commands arguments
	for (i = 1; i < MAX_ARG; i++)
	{
		args[i] = strtok(NULL, delimiters);
		if (args[i] != NULL)
			num_arg++;
	}

}
void Atm::Open_new_account(int account, int password, int initial_amount);
void Atm::Deposit(int account, int password, int amount);
void Atm::Withdraw(int account, int password, int amount);
void Atm::get_account_balance(int account, int password);
void Atm::Close_account(int account, int password);
void Atm::Transaction(int account, int password, int target_account, int amount);