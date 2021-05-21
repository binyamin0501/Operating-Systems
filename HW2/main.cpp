#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <ostream>
#include <unistd.h>
#include <pthread.h>

using namespace std;

#define FAIL 1

int main(int argc, char const *argv[])
{
    if ((argc < 1) || (argc - 2 != atoi(argv[1]))) {
        cout << "illegal arguments" << endl;
        return FAIL;
    }
    return 0;
}
