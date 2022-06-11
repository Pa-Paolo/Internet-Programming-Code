#include <stdio.h>
#include <unistd.h>
#include <string.h>

int main(int argc, char **argv)
{
    int pfd[2];
    if(pipe(pfd)<0){
        perror("Error in creating the pipe\n");
        exit(-1);
    }

    int pid;
    if(pid=fork()>0)
    {
        //CHILD
        printf("Child: started\n");
        fflush(stdout);
        close(pfd[0]); //chiudo quello per leggere

        FILE *fd = fdopen(pfd[1],"w");
        if(fd==NULL){
            perror("Child: opening the fd\n");
            exit(-1);
        }

        fprintf(fd,"Starting countdown:\n");
        for(int i=0;i<10;i++)
        {
            fprintf(fd,"%d\n",i);
            sleep(0.1);
        }

        printf("Child: job done\n");
        fflush(stdout);
        fclose(fd);
    }
    else
    {
        printf("Father: continue\n");
        close(pfd[1]);

        char buf[120];
        int nread;

        while ((nread = read(pfd[0], buf, sizeof(buf))) > 0) {
            printf("Child: Read %d from the pipe\n", nread);
            buf[nread] = 0;
            printf("Child: Buffer content: \n%s", buf);
        }
        fflush(stdout);
    }

    close(pfd[0]);
    close(pfd[1]);
    return 0;
}