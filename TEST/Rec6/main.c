/******************************************************************************

                            Online C Compiler.
                Code, Compile, Run and Debug C program online.
Write your code in this editor and press "Run" button to compile and execute it.

*******************************************************************************/

#include <stdio.h>
#include <pthread.h>

#define COUNT 10
#define LIM 12

int count = 0;

pthread_mutex_t count_mutex;
pthread_cond_t count_cv;

void *inc_val(void *arg) {
    printf("inc_val\n");
    for (int i=0; i<COUNT;i++) {
        pthread_mutex_lock(&count_mutex);
        count++;
        if (count == LIM) {
            pthread_cond_signal(&count_cv);
        }
        pthread_mutex_unlock(&count_mutex);
    }
    pthread_exit(NULL);
}

void *watch(void *arg) {
    printf("watch\n");
    pthread_mutex_lock(&count_mutex);
    while (count < LIM) {
        pthread_cond_wait(&count_cv, &count_mutex);
        printf("watch count==LIM\n");
    }
    pthread_mutex_unlock(&count_mutex);
    pthread_exit(NULL);
}

int main()
{
    pthread_t threads[3];
    
    pthread_mutex_init(&count_mutex, NULL);
    pthread_cond_init(&count_cv, NULL);
    
    pthread_create(&threads[0], NULL, inc_val, NULL);
    pthread_create(&threads[1], NULL, inc_val, NULL);
    pthread_create(&threads[2], NULL, watch, NULL);
    
    for ( int i = 0; i < 3; i++) {
        pthread_join(threads[i], NULL);
    }
    
    pthread_mutex_destroy(&count_mutex);
    pthread_cond_destroy(&count_cv);
    pthread_exit(NULL);
    

    return 0;
}

