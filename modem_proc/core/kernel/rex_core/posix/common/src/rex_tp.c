#include <string.h>

#include "rex.h"
#include "rex_port.h"
#include "rex_os_port.h"
#include "log.h"
#include "rexlog.h"
#include "rex_tcb.h"

/*=================================================================================================
                                       DATA DECLARATIONS
=================================================================================================*/
#define LOG_SYSPROFILE_C_VER                1

/*=================================================================================================
                                       FORWARD DECLARATIONS
=================================================================================================*/
extern void dog_kick(void);

static void rex_os_sys_tp(rex_os_tcb_type *p_os_tcb, unsigned int num_tasks)
{
   LOG_BLAST_SYSPROFILE_C_type   *sp_ptr;

   /* Allocate a log record */
   sp_ptr = (LOG_BLAST_SYSPROFILE_C_type *)log_alloc(LOG_BLAST_SYSPROFILE_C,
                                          sizeof(LOG_BLAST_SYSPROFILE_C_type));

   /* Fill in the log if allocation worked */
   if (NULL != sp_ptr)
   {
      sp_ptr->ver              = LOG_SYSPROFILE_C_VER;
      sp_ptr->total_num_tasks  = num_tasks;
      sp_ptr->task_id          = rex_os_get_rtos_thread_id(p_os_tcb);
      sp_ptr->stack_size       = rex_os_get_stack_size(p_os_tcb);
      sp_ptr->stack_limit      = (uint32)rex_os_get_stack_addr(p_os_tcb);
      sp_ptr->max_intlock_time = 0xdeadbeef;  /* not supported */
      sp_ptr->max_intlock_lr   = 0xdeadbeef;  /* not supported */

      memset((void*)sp_ptr->task_name, 0x00, LOG_REX_SYSPROFILE_TASK_NAME_LEN);
      rex_os_get_task_name(p_os_tcb, sp_ptr->task_name, sizeof(sp_ptr->task_name));
      log_commit(sp_ptr);
   }
}

void rex_os_log_sys_profiling_info(void)
{
   unsigned int num_tasks;

   /*
   ** Make sure we have plenty of time to iterate through the list
   ** of tasks and report profile information.
   */
   dog_kick();

   /* Get the number of tasks in the system */
   num_tasks = (unsigned int)rex_get_num_tasks();

   /* Iterate through the list of tasks and collect sys 
   ** profiling information. 
   */
   rex_task_list_mgr_for_each(rex_os_sys_tp, num_tasks);
}
