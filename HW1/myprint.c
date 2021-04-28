#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

int main(int argc, char const *argv[])
{
    int i = 0, delay = 1, count = 10000;

    for (; i < count; i++) {
        printf("%d\n",i);
        sleep(delay);
    }
    return 0;
}
