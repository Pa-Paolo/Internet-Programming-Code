#include <signal.h>
#include "stdio.h"
#include <unistd.h>

void signal_handler()
{
    printf("Prova\n");
}

int main(int argc, char **argv) {
    for(int i=0;i<argc;i++)
    {
        printf("%s\n",argv[i]);
    }

    printf("Process id: %d",getpid());
    signal(SIGUSR1,signal_handler);
    fflush(stdout);

    for(int i=0;i<120;i++){
        sleep(1);
    }

    return 0;
}


