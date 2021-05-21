#ifndef _ATM_H_
#define _ATM_H_

#include <string>
#include <string.h>
#include <fstream>
#include <iostream>

#define ERROR 1
#define INVALID -1
#define MAX_ARG 4

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

#endif //!_ATM_H_
