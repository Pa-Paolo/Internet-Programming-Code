#include "unistd.h"
#include "stdio.h"
#include "strings.h"

int main()
{
    int pid;
    int pfd[2];

    if(pipe(pfd)<0)
    {
        perror("Error in pipe creation");
        exit(-1);
    }

    if(pid=fork()==0)
    {
        //CHILD
        close(pfd[0]);

        printf("Child: started\n");
        fflush(stdout);
        char buf[120];
        snprintf(buf, 120, "Hello, this is child!\n");
        write(pfd[1],buf,sizeof(buf));
        for (int i = 0; i < 10; ++i) {
            sleep(2);
            snprintf(buf, 120, "Test n%d\n", i);
            write(pfd[1],buf, sizeof(buf));
        }

        printf("Child: Finish\n");
        close(pfd[1]);

    } else //-------------------------------------------
    {
        //PARENT
        close(pfd[1]);

        printf("Father: started");
        fflush(stdout);
        int n;
        char red[120];
        while(n=read(pfd[0],red,sizeof(red))>0)
        {
            printf("Father: read %d bytes from child\n",n);
            //red[n]=0;
            printf("Parent: read -> \n%s",red);
        }

        close(pfd[0]);
    }

    return 0;
}