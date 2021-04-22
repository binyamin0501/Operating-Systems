// signals.c
// contains signal handler funtions
// contains the function/s that set the signal handlers

/*******************************************/
/* Name: handler_cntlc
   Synopsis: handle the Control-C */
#include "signals.h"

void catch_C_int(int sig_num) {
   printf("Ctrl + C\n");
}
void catch_Z_int(int sig_num) {
   printf("Ctrl + Z\n");
}
