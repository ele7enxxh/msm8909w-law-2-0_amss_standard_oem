#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <qube.h>
#include "memload_handler.h"
#include "two_threads.h"

volatile int counter = 0;
#define PAGE_SIZE   (4*1024)

void dl_segment_init (swap_pools_t *);
void init_active_page_pool(int, unsigned int, int);
void dump_mapping_table (void );
void dump_pool (int);

extern void pager(void * name);

int thread_create(void *pc, void *stack, unsigned int stack_size, char *arg, int prio){

    qurt_thread_attr_t attr;
    qurt_thread_t tid;
    int ret;

    qurt_thread_attr_init (&attr);
    qurt_thread_attr_set_stack_size (&attr, stack_size);
    qurt_thread_attr_set_stack_addr (&attr, stack);
    qurt_thread_attr_set_priority (&attr, (unsigned short)prio);

    ret = qurt_thread_create (&tid, &attr, pc, (void *)arg);
    if (ret == -1) {
			printf(" failed to create thread \n");
	}
	return tid;
}

int main(int argc, char **argv)
{

    int i, t1, t[MAX_FAULTING_THREADS];
    void *stack1 = malloc( 4096 ), *stack[MAX_FAULTING_THREADS];

    //set logical_segm_start, map for the size of the swapped segment:
    unsigned int size = (unsigned int)&__swapped_segments_end__ - (unsigned int)&__swapped_segments_start__;
    qurt_mapping_create(logical_start, (unsigned int)&__swapped_segments_start__, size, QMEM_CACHE_WRITEBACK, 0x7);
    printf ("mapped size: %x\n", size);
    printf ("segment info: start .... %x, end ..... %x\n",
            &__swapped_segments_start__, &__swapped_segments_end__);

    //end

    t1 = thread_create(pager, stack1, 4096, (void *)0x0, /*unsigned int prio*/0);
    
    while (counter++ < 500000) 
        __asm__ __volatile__ (
            "r11 = r11 \n"
            :::"memory");

    //for(i = 0; i < MAX_FAULTING_THREADS; i++) {
    i = 0;
    while (i < MAX_FAULTING_THREADS)
    {
        stack[i] = malloc( 4096 );
        t[i] = thread_create(faulting_thread1, stack[i], 4096, (void *)i, /*unsigned int prio*/0);
        i++;
       
 
        //stack[i] = malloc( 4096 );
        //stack[i] = (void *)(((unsigned int)(stack[i] + 4096)) & (-8));
        //t[i] = thread_create(faulting_thread2, stack[i], (void *)i, /*unsigned int prio*/0);
        //i++;
    
    }

    int r;
    int status; 
    for(i = 0; i < MAX_FAULTING_THREADS; i++) {
        r = qurt_thread_join(t[i], &status ); 
        printf( "faulting thread %d exit return %p status %p\n", i, r, status);
    }

    /* This is optional... */
    printf ("dumping mapping_table.... \n");
    dump_mapping_table();
    printf ("dumping pools..... \n");
    dump_pool(0);
    dump_pool(1);
    return 0;
}

