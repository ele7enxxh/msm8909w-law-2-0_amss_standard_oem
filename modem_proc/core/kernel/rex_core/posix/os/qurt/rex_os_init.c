#include "rex.h"
#include "rex_port.h"
#include "rex_os_port.h"

extern REX_OS_LOCK_TYPE rexl4_int_lock;

static rex_tcb_type ex_handler_tcb;
static int ex_handler_tcb_context_done = 0;
extern void rex_start_test(void);

extern int _rex_create_context_for_external_thread
(
   rex_tcb_type *p_tcb, 
   rex_task_attributes_type *p_attr
);

void rex_init
(
  unsigned int           hw_bitmask,            /* hardware bitmask     */
  void *                 p_istack,              /* interrupt stack      */
  rex_stack_size_type    p_istksiz,             /* interrupt stack size */
  rex_tcb_type           *p_tcb_main_task,      /* task control block   */
  void *                 p_stack,               /* stack                */
  rex_stack_size_type    stack_size,            /* stack size           */
  rex_priority_type      priority,              /* task priority        */
  rex_task_func_type     p_task,                /* task function        */
  dword                  param                  /* task parameter       */
)
{
   int err_code;
   rex_task_attributes_type rex_main_task_attr;
   pthread_attr_t attr;
   pthread_t fake_pthread;

   /* Make the current thread a POSIX thread first */
   //REXTODO move this stuff out of REX and into a process_init type function
   pthread_attr_init(&attr);
   pthread_attr_setthreadname(&attr, "AMSS0");
   pthread_attr_setstacksize(&attr, 0x4000);
   pthread_attr_setstackaddr(&attr, (void*)0xDEADBEEF);
   pthread_fake(&fake_pthread, &attr);
   pthread_attr_destroy(&attr);

   REX_OS_MUTEX_INIT(&rexl4_int_lock);

   /* Perform and REX lib initialization */
   rex_posix_init();

   /* The common REX code requires us to call this function */
   rex_core_common_init();
   
   if (!ex_handler_tcb_context_done)
   {
      rex_task_attributes_type rattr;
      ex_handler_tcb_context_done = 1;
      rex_task_attr_init(&rattr);      
      rex_task_attr_set_task_name(&rattr, "AMSS0");
      err_code = _rex_create_context_for_external_thread(&ex_handler_tcb, &rattr);
      if (REX_SUCCESS != err_code)
      {
         REX_ERR_FATAL("Error when trying to Create REX TCB for a non REX thread");
      }
      /* Sanity check if we are the AMSS 0 startup task */
      ASSERT(&ex_handler_tcb == rex_self());
   }

   /* Attributes for the system Main Task */   
   rex_task_attr_init(&rex_main_task_attr);
   rex_task_attr_set_stack_addr(&rex_main_task_attr, p_stack);
   rex_task_attr_set_stack_size(&rex_main_task_attr, stack_size * sizeof(rex_stack_size_type));
   rex_task_attr_set_prio(&rex_main_task_attr, priority);
   rex_task_attr_set_entry_func(&rex_main_task_attr, p_task, param);
   rex_task_attr_set_task_name(&rex_main_task_attr, "Main Task");
   rex_task_attr_set_initially_suspended(&rex_main_task_attr, FALSE);
   rex_task_attr_set_cpu_affinity(&rex_main_task_attr, hw_bitmask);

   #ifdef REX_CORE_TEST
      rex_start_test();
   #endif
   
   /* Create the system Main Task */
   err_code = rex_common_def_task(p_tcb_main_task, &rex_main_task_attr);   
   if(REX_SUCCESS != err_code)
   {
      REX_ERR_FATAL("Failed to create Main Task");
   }
}
