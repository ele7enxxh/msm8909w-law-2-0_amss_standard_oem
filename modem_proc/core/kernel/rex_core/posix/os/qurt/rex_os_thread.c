#include <stdlib.h>

#include "pthread.h"
#include "rex.h"
#include "rex_port.h"
#include "rex_os_port.h"
#include "rex_tcb.h"

extern void rex_context_set_self_ref(rex_tcb_type *p_tcb);

void * rex_os_thread_entry_func(void *param)
{
   rex_tcb_type *p_tcb = (rex_tcb_type *)param;
   rex_os_tcb_type *p_os_tcb = NULL;

   if (NULL == p_tcb)
   {
      REX_ERR_FATAL("Bad Task Entry Pointer Passed in thread entry routine.");
   }

   REX_GET_OS_TCB_REF(p_tcb, &p_os_tcb);

   /* Perform any OS specific thread initialization */
   rex_os_thread_init(p_os_tcb);

   /* Set the REX TCB such that rex_self() returns p_tcb */
   rex_context_set_self_ref(p_tcb);

   /* Invoke callback functions on thread creation */
   rex_invoke_cbs_on_thread_create(p_tcb);

   /*
   ** Check if the task is expected to suspend initially and
   ** if so do not invoke its entry function and just suspend
   */
   if (rex_os_get_task_initially_suspended_flag(p_os_tcb))
   {
      /* Suspend task initially */
      rex_suspend_task(p_tcb);
   }

   /* Execute task entry function */
   rex_os_execute_task_entry_func(p_os_tcb);

   /*
   ** Making sure the last function called on thread
   ** rex_kill_task
   */
   rex_kill_task(p_tcb);

   return NULL;
}

void rex_os_thread_init(rex_os_tcb_type *p_os_tcb)
{
   rex_os_set_rtos_thread_id(p_os_tcb, REX_OS_GET_RTOS_TID());
}

void rex_os_thread_deinit(rex_os_tcb_type *p_os_tcb)
{

}