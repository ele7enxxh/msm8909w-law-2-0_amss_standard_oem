#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <assert.h>
#include <qurt.h>
#include <qurt_timer.h>

#define MAX_THREADS 6 

int ceiling_prio = (255-200);
static qurt_sem_t to_start; 
static int t[MAX_THREADS];
int original_prio[MAX_THREADS];

int test_qurtos_timer()
{
    qurt_timer_t timer;
    qurt_timer_attr_t attr;
    qurt_anysignal_t sigs;
    qurt_timer_duration_t duration = 10000;
    qurt_timer_duration_t remaining;
    unsigned int mask = 0x80;
    int status; 
    unsigned long long ticks = 0;
    int index;
    unsigned long long start_pcycle, end_pcycle;

    qurt_timer_attr_init(&attr);
    qurt_anysignal_init(&sigs);
    qurt_timer_attr_set_duration(&attr, duration);
    qurt_timer_attr_set_group(&attr, 1);

    status = qurt_timer_group_enable(3);
    if (QURT_EOK != status)
    {
        printf( "qurt_timer_group_enable failed. Returned %d\n", status);
        return -1;
    }
    
    ticks = qurt_timer_get_ticks();
    printf( "current ticks is %llu.\n", ticks);

    start_pcycle = qurt_get_core_pcycles();  
    qurt_timer_sleep(20000);
    end_pcycle = qurt_get_core_pcycles();
    printf( "qurt_timer_sleep for 20ms.\n");
    printf("qurt_timer_sleep took %llu pcycles.\n", end_pcycle - start_pcycle);
	
    ticks = qurt_timer_get_ticks();
    printf( "current ticks is %llu.\n", ticks);

    qurt_timer_sleep(30000);
    printf( "qurt_timer_sleep for 30ms.\n");

    ticks = qurt_timer_get_ticks();
    printf( "current ticks is %llu.\n", ticks);

    status = qurt_timer_group_disable(3);
    if (QURT_EOK != status)
    {
        printf("qurt_timer_group_disable failed. Returned %d\n", status);
        return -1;
    }

    qurt_timer_recover_pc();

    for (index = 0; index< 1000; index++)
    {
        status = qurt_timer_create(&timer, &attr, &sigs, mask);
        //printf( "qurt_timer_create returned. Return value is %d\n", status);    
        (void)qurt_timer_delete(timer);
    }

    //create timer
    start_pcycle = qurt_get_core_pcycles();
    status = qurt_timer_create(&timer, &attr, &sigs, mask);
    end_pcycle = qurt_get_core_pcycles();
    
    if (QURT_EOK != status)
    {
        printf("qurt_timer_create failed. Returned %d\n", status);
        return -1;
    }
    else
    {
        printf("qurt_timer_create passed. Returned %d\n", status);  
        printf("qurt_timer_create took %llu pcycles.\n", end_pcycle - start_pcycle);
    }

    //get attr 1
    if (QURT_EOK != qurt_timer_get_attr(timer, &attr))
        return -1;

    qurt_timer_attr_get_remaining(&attr, &remaining);
    printf("timer remaining is %llu, timer duration was %llu.\n", remaining, duration);

    //get attr 2
    if (QURT_EOK != qurt_timer_get_attr(timer, &attr))
        return -1;

    qurt_timer_attr_get_remaining(&attr, &remaining);
    printf("timer remaining is %llu, timer duration was %llu.\n", remaining, duration);

    //get attr 3
    if (QURT_EOK != qurt_timer_get_attr(timer, &attr))
        return -1;

    qurt_timer_attr_get_remaining(&attr, &remaining);
    printf("timer remaining is %llu, timer duration was %llu.\n", remaining, duration);

    //wait for signal from timer
    printf("waiting for timer signal back.\n");
    (void)qurt_anysignal_wait(&sigs, mask);
    end_pcycle = qurt_get_core_pcycles();

    //get attr 4
    if (QURT_EOK != qurt_timer_get_attr(timer, &attr))
        return -1;

    qurt_timer_attr_get_remaining(&attr, &remaining);
    printf("timer remaining is %llu, timer duration was %llu.\n", remaining, duration);

    // delete timer
    start_pcycle = qurt_get_core_pcycles();
    status = qurt_timer_delete(timer);
    end_pcycle = qurt_get_core_pcycles();
    if (QURT_EOK != status)
    {
        printf("qurt_timer_delete failed. Returned %d\n", status);
        return -1;
    }
    else
    {        
        printf("qurt_timer_delete passed. Returned %d\n", status);
        printf("qurt_timer_delete took %llu pcycles.\n", end_pcycle - start_pcycle);
    }

    return 0;	

}

int main(int argc, char **argv)
{
    volatile int i = 0;
    int t1, t2;
    int r;
    int status; 

    printf("app1 pid is %d\n", qurt_getpid());

    if (test_qurtos_timer() == -1)
        return -1;
	
    //while(i++<100000) {i++;i--;};

    printf( "app1 main exit.\n");
	
    return 0;
}

