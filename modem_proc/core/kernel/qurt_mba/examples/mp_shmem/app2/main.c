#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <assert.h>
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

struct region *app2_rptr;

int main(int argc, char **argv)
{
    int ret, fd;

    printf("app2 pid is %d\n", qurt_getpid());

    //negative testing 1: incorrect device name
    fd = shm_open("/dev/shm/myshm", 0, 0);
    assert (fd == -1);

    ret = shm_close(fd);
    assert (ret == -1);

    //open shared memory
    fd = shm_open("/shm/my_shmem", O_RDWR, S_IRUSR);
    printf("app2: fd = %x\n",fd);

    app2_rptr = mmap(NULL, sizeof(struct region), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    
    printf("app2: mmaped to %x\n",app2_rptr);
    qurt_timer_sleep(20000);

    printf("app2: reading data from shared memory...\n");
    printf("app2: buf[%d] = %d\n",400, app2_rptr->buf[400]);

    shm_close(fd);
    
    printf( "app2 main exit.\n");
    return 0;
}
