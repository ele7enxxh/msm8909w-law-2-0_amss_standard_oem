#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <qube.h>
#define test_range 64*1024
#include "memload_handler.h"

extern int swapped_data[test_range];

void faulting_thread2(void *arg){
    int i, start, end;
    start = (int)arg * (test_range/MAX_FAULTING_THREADS);
    end = start + (test_range/MAX_FAULTING_THREADS);
    printf("*******thread %d access range from %x -- %x\n\n\n",(int)arg, start, end);

    for (i = start; i < end; i++)
        if (swapped_data[i] == 0xdeadbeef)
            printf("deadbeef found by thread 2\n");


    printf("\n>>>>>>>successfully tested resuming faulting thread %x after demand loading\n\n\n", (int)arg);

    qurt_thread_exit(0);
}

