#ifndef _ATM_H_
#define _ATM_H_

#include <string>
#include <string.h>

using namespace std;

class Atm {
    int _atm_num;
    string ATM_input;

public:
    Atm();
    ~Atm();

    int get_atm_num() const;
    string get_atm_input() const;

    void set_atm_num() const;
    void set_atm_input() const;

    void* ATM_thread_func (void* ATM_input_file);

};

#endif //!_ATM_H_
