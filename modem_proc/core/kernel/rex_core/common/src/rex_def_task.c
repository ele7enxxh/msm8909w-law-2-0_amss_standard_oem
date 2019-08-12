#include "rex.h"
#include "rex_port.h"
#include <stringl/stringl.h>

void rex_set_common_task_attributes
(
   rex_task_attributes_type   *p_rex_attr,
   rex_stack_pointer_type     p_stack,
   rex_stack_size_type        stack_size,
   rex_priority_type          priority,
   rex_task_func_type         p_task,
   rex_task_func_param_type   param,
   char                       *task_name,
   boolean                    suspended
)
{
   int err_code;

   rex_task_attr_init(p_rex_attr);

   err_code = rex_task_attr_set_stack_addr(p_rex_attr, p_stack);
   if(REX_SUCCESS != err_code)
   {
      REX_ERR_FATAL("Invalid Stack Address specified when creating REX Task");
   }

   err_code = rex_task_attr_set_stack_size(p_rex_attr, stack_size);
   if(REX_SUCCESS != err_code)
   {
      REX_ERR_FATAL("Invalid Stack size specified when creating REX Task");
   }
   
   err_code = rex_task_attr_set_prio(p_rex_attr, priority);
   if(REX_SUCCESS != err_code)
   {
      REX_ERR_FATAL("Invalid priority specified when creating REX Task");
   }

   err_code = rex_task_attr_set_entry_func(p_rex_attr, p_task, param);
   if(REX_SUCCESS != err_code)
   {
      REX_ERR_FATAL("Invalid Entry Function specified when creating REX Task");
   }

   rex_task_attr_set_task_name(p_rex_attr, task_name);

   err_code = rex_task_attr_set_initially_suspended(p_rex_attr, suspended);
   if(REX_SUCCESS != err_code)
   {
      REX_ERR_FATAL("Starting a task in initially suspended state is unsupported.");
   }

   rex_task_attr_set_cpu_affinity(p_rex_attr, REX_SINGLE_CPU_AFFINITY_MASK);
}

void rex_def_task
(
   rex_tcb_type               *p_tcb,              /* valid tcb for new task       */
   void                       *p_stack,            /* stack for new task           */
   rex_stack_size_type        stack_size,          /* stack size in units of
                                                      rex_stack_word_type          */
   rex_priority_type          priority,            /* priority for new task        */
   rex_task_func_type         p_task,              /* task startup function        */
   rex_task_func_param_type   param                /* parameter for new task       */
)
{
   int err_code;
   rex_task_attributes_type rex_attr;
   // char task_name[REX_TASK_NAME_LEN+1] = "REX_DEFLT";
   
   // if (NULL != p_tcb->task_name)
   // {
      // strlcpy(task_name, p_tcb->task_name, sizeof(task_name));
   // }

   rex_set_common_task_attributes(&rex_attr, 
                                    p_stack, 
                                    stack_size, 
                                    priority, 
                                    p_task, 
                                    param, 
                                    "REX_DEFLT", 
                                    FALSE);
   
   err_code = rex_common_def_task(p_tcb, &rex_attr);
   if(REX_SUCCESS != err_code)
   {
      REX_ERR_FATAL("Failed to create REX Task");
   }

   return;
}

void rex_def_task_ext
(
   rex_tcb_type               *p_tcb,              /* valid tcb for new task       */
   rex_stack_pointer_type     p_stack,             /* stack for new task           */
   rex_stack_size_type        stack_size,          /* stack size in bytes          */
   rex_priority_type          priority,            /* priority for new task        */
   rex_task_func_type         p_task,              /* task startup function        */
   rex_task_func_param_type   param,               /* parameter for new task       */
   char                       *task_name,          /* A/N string for task name     */
   boolean                    suspended            /* is task initially suspended? */
)
{
   int err_code;
   rex_task_attributes_type rex_attr;
   
   rex_set_common_task_attributes(&rex_attr, 
                                    p_stack, 
                                    stack_size, 
                                    priority, 
                                    p_task, 
                                    param, 
                                    task_name, 
                                    suspended);
   
   err_code = rex_common_def_task(p_tcb, &rex_attr);
   if(REX_SUCCESS != err_code)
   {
      REX_ERR_FATAL("Failed to create REX Task");
   }

   return;
}

void rex_def_task_ext2
(
   rex_tcb_type               *p_tcb,              /* valid tcb for new task              */
   rex_stack_pointer_type     p_stack,             /* stack for new task                  */
   rex_stack_size_type        stack_size,          /* stack size in bytes                 */
   rex_priority_type          priority,            /* priority for new task               */
   rex_task_func_type         p_task,              /* task startup function               */
   rex_task_func_param_type   param,               /* parameter for new task              */
   char                       *task_name,          /* A/N string for task name            */
   boolean                    suspended,           /* is task initially suspended?        */
   int                        ignored              /* Ignored                             */
)
{
   int err_code;
   rex_task_attributes_type rex_attr;
   
   rex_set_common_task_attributes(&rex_attr, 
                                    p_stack, 
                                    stack_size, 
                                    priority, 
                                    p_task, 
                                    param, 
                                    task_name, 
                                    suspended);
   
   err_code = rex_common_def_task(p_tcb, &rex_attr);
   if(REX_SUCCESS != err_code)
   {
      REX_ERR_FATAL("Failed to create REX Task");
   }
   
   return;
}
