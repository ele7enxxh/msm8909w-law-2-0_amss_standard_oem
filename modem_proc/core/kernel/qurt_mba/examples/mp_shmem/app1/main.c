#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <assert.h>
#include <qurt_types.h>
#include <qurt.h>
#include <qurt_timer.h>

#define MAX_LEN 10000 

//FOLLOWING MACROs can be removed if tools 4.0 works.
/*
 * Protections are chosen from these bits, or-ed together
  */
#define PROT_NONE   0x00    /* no permissions */
#define PROT_READ   0x01    /* pages can be read */
#define PROT_WRITE  0x02    /* pages can be written */
#define PROT_EXEC   0x04    /* pages can be executed */

 /*
  * Flags contain sharing type and options.
  * Sharing types; choose one.
  */
#define MAP_SHARED  0x0001  /* share changes */
#define MAP_PRIVATE 0x0002  /* changes are private */


                 
struct region {
    int len;
    int buf[MAX_LEN];
};

struct region *app1_rptr;

int main(int argc, char **argv)
{
    int i = MAX_LEN, fd; 
    
    printf("app1 pid is %d\n", qurt_getpid());

    fd = shm_open("/shm/my_shmem", O_CREAT | O_RDWR, S_IRUSR | S_IWUSR);

    assert (fd != -1);
    printf("app1: fd = %x\n",fd);
    app1_rptr = mmap(NULL, sizeof(struct region), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);

    if (app1_rptr < 0) {
        printf("mmap failed, app1 aborts...\n");
        assert(0);
    }

    printf("app1: writing data to shared memory mapped at %x...\n", app1_rptr);

    while (i--) {
        app1_rptr->buf[i] = i;
        if (!(i%200))
            printf("write %d to buf[%d]\n",i, i);
    }


    qurt_timer_sleep(10000);
    shm_close(fd);
    printf( "app1 main exit.\n");
	
    return 0;
}

