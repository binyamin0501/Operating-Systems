#ifndef _ATM_H_
#define _ATM_H_

#include <string>
#include <string.h>
#include <fstream>
#include <iostream>
#include <unistd.h>
#include <pthread.h>
#include "Bank.h"

#define ERROR 1
#define INVALID -1
#define MAX_ARG 4
#define ATM_SLEEP 100000
#define ACTION_SLEEP 1

using namespace std;

class Atm {
    int _atm_num;
    string _atm_input;

public:
    Atm();
    ~Atm();

    int get_atm_num() const;
    string get_atm_input() const;

    void set_atm_num(int atm_num);
    void set_atm_input(const char* input);

    void* ATM_thread_func (void* ATM_input_file);
    void Open_new_account(int account, int password, int initial_amount);
    void Deposit(int account, int password, int amount);
    void Withdraw(int account, int password, int amount);
    void get_account_balance(int account, int password);
    void Close_account(int account, int password);
    void Transaction(int account, int password, int target_account, int amount);
};

void Parse_atm_line(char* Operation, int args[MAX_ARG], string line);

//*******************************************************************
// function name: Check_account_exist
// Description: check if an account with account number exist
// Parameters: account - account number
// Returns: true if there is an account with this id, false if there is not
//*******************************************************************
bool Check_account_exist(int account);

//*******************************************************************
// function name: Check_password
// Description: check if the password provided match the account password
// Parameters: account - account number trying to access, password - the password provided 
// Returns: true if the password match, false otherwise
//*******************************************************************
bool Check_password(int account, int password);

//FIXME meybe use singelton ?
extern Bank* bank;
extern Log_File* Bank_Log;
extern bool is_open;

#endif //!_ATM_H_
