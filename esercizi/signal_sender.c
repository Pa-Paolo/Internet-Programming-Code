#include "signal.h"
#include "stdlib.h"
#include "stdio.h"

int main(int argc, char **argv)
{
    char buf[40];
    printf("Insert pid to signal: ");
    gets(buf);
    int pid = atoi(buf);

    kill(pid,SIGUSR1);
}