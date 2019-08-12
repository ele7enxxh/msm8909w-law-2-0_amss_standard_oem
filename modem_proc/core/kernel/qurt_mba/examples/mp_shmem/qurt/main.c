#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <assert.h>
#include <qurt.h>
#include <qurt_timer.h>

#define MAX_THREADS 6 

int main(int argc, char **argv)
{
    volatile int i = 0;
    int ret, status;

    printf("qurt pid is %d\n", qurt_getpid());

    extern void install_shmem_driver(void);

    install_shmem_driver();

    //currently qurt_spawn starts all child processes
    qurt_spawn("app1.pbn");
    qurt_spawn("app2.pbn");
    
    ret = qurt_wait(&status);
    printf("guest OS detected process %d exits with status =%x\n",ret, status);
    
    ret = qurt_wait(&status);
    printf("guest OS detected process %d exits with status =%x\n",ret, status);

    printf( "qurt main exit.\n");
    return 0;
}

