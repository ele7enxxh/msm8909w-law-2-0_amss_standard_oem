#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <qurt.h>

#if 0
void ExceptionHandler (void *arg)
{
   unsigned int error_thread, ip, sp, badva, err_code;

   //err_code= qurt_exception_wait_ext (&sys_err);
   err_code=qurt_exception_wait (&ip, &sp, &badva, &err_code);
   if (error_thread == -1) {
      printf ("QURT error handler registration unsuccessfull\n");
      exit (-1);
   }
   printf ("!!! Error occured\n");
   printf ("Thread ID 0x%x\n", error_thread);
   printf ("IP        0x%x\n", ip);
   printf ("SP        0x%x\n", sp);
   printf ("BADVA     0x%x\n", badva);
   printf ("Err  code 0x%x\n", err_code);

   qurt_thread_exit(0);
} /* end of mobile_exception_handler */
#endif
void TestMain (void *arg)
{
   volatile int value1, value2;
   char *ptr = (char *)&value1;

   qurt_timer_sleep (10000);
   value2 = *(int *)(ptr + 1);
   //printf ("Reading the unaligned address 0x%x, value:%d\n", &value2, value2);
   qurt_thread_exit (0);
}

/**
* Main function
*/
int main (int argc, char *argv[])
{
#define STACK_SIZE 2048
   int status;
   qurt_thread_attr_t attr;
   qurt_thread_t thr1, thr2, thr3;
   void* stack1, *stack2;
      	
    stack1=malloc(STACK_SIZE);
    assert(NULL!=stack1);
//    stack2=malloc(STACK_SIZE);
//    assert(NULL!=stack2);

   qurt_thread_attr_init(&attr);
   qurt_thread_attr_set_stack_size(&attr, STACK_SIZE);
   qurt_thread_attr_set_priority(&attr, 99);

 
   qurt_thread_attr_set_stack_addr(&attr,stack1 );
   status = qurt_thread_create(&thr1, &attr,TestMain, NULL);
   assert(QURT_EOK==status);	

   //qurt_thread_attr_set_stack_addr(&attr,stack2 );
   //status = qurt_thread_create(&thr3, &attr,TestMain2, NULL);
   //assert(QURT_EOK==status);	


   status = qurt_thread_join (thr1, &status);
//   status = qurt_thread_join (thr2, &status);
   //status = qurt_thread_join (thr3, &status);
   return 0;
}
