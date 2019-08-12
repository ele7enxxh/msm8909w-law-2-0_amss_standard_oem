#include <stdlib.h>

#include "pthread.h"
#include "rex.h"
#include "rex_port.h"
#include "rex_os_port.h"
#include "posix_np.h"
#include "rex_tcb.h"

/* Externs and Forward Declarations */

extern void * rex_os_thread_entry_func(void *param);
extern int pthread_attr_setexternalcontext_np(pthread_attr_t *attr, int context);

static int rex_create_and_init_os_tcb(rex_tcb_type *p_tcb, rex_task_attributes_type *p_attr);

INLINE void stack_align_helper_rev
(
   rex_stack_pointer_type  orig_stack_start_ptr, 
   rex_stack_size_type     orig_stack_size,
   unsigned int            align_to,   
   rex_stack_pointer_type  *pp_adj_stack_start_ptr,
   rex_stack_size_type     *p_adj_stack_size,
   rex_stack_pointer_type  *pp_adj_stack_end_ptr
)
{
   /* In this case, stacks are passed as arrays of bytes with the size
   ** indicating the number of bytes. The adjustment of the stack pointer
   ** is to assure alignment. 
   */
   rex_stack_pointer_type temp_ptr, adj_stack_ptr;
   rex_stack_size_type adj_stksize;

   temp_ptr = (rex_stack_pointer_type)((uint32)orig_stack_start_ptr + orig_stack_size);
   /* Adjust stack alignment to input byte boundary */
   adj_stack_ptr = (rex_stack_pointer_type)((uint32)temp_ptr - ((uint32)temp_ptr % align_to));

   /* Adjusted stack size after alignment */
   adj_stksize = (rex_stack_size_type)((uint32)adj_stack_ptr - (uint32)(orig_stack_start_ptr));

   /* Ensure that the entire stack (start to end) is aligned so drop off the last odd bytes */
   adj_stksize &= ~(align_to - 1);

   *pp_adj_stack_start_ptr  = (adj_stack_ptr - adj_stksize);
   *p_adj_stack_size        = adj_stksize;
   *pp_adj_stack_end_ptr    = adj_stack_ptr;
}

extern void rex_set_common_task_attributes
(
   rex_task_attributes_type   *p_rex_attr,
   rex_stack_pointer_type     p_stack,
   rex_stack_size_type        stack_size,
   rex_priority_type          priority,
   rex_task_func_type         p_task,
   rex_task_func_param_type   param,
   char                       *task_name,
   boolean                    suspended
);

void rex_def_task_ext3
(
   unsigned int        hw_bitmask,
   rex_tcb_type *      p_tcb,       /*!< valid tcb for new task */
   void *              p_stack,        /*!< stack for new task */
   rex_stack_size_type stack_size,     /*!< stack size in units of rex_stack_word_type */
   rex_priority_type   priority,       /*!< priority for new task */
   void                (*p_task)( dword ), /*!< task startup function	*/
   dword               param           /*!< parameter for new task */
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
                                    "REX_DEFLT", 
                                    FALSE);

   err_code = rex_task_attr_set_cpu_affinity(&rex_attr, hw_bitmask);
   if (REX_SUCCESS != err_code)
   {
      REX_ERR_FATAL("Invalid CPU Affinity");
   }
   
   err_code = rex_common_def_task(p_tcb, &rex_attr);
   if (REX_SUCCESS != err_code)
   {
      REX_ERR_FATAL("Failed to create REX Task");
   }
   
   return;                                    
}

void rex_def_task_ext4
(
   unsigned int        hw_bitmask,
   rex_tcb_type *      p_tcb,          /*!< valid tcb for new task */
   unsigned char *     p_stack,           /*!< stack for new task */
   rex_stack_size_type stack_size,        /*!< stack size in bytes */
   rex_priority_type   priority,          /*!< priority for new task */
   rex_task_func_type  p_task,            /*!< task startup function */
   dword               param,             /*!< parameter for new task */
   char *              task_name,         /*!< A/N string for task name */
   boolean             suspended          /*!< is task initially suspended? */
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

   err_code = rex_task_attr_set_cpu_affinity(&rex_attr, hw_bitmask);
   if (REX_SUCCESS != err_code)
   {
      REX_ERR_FATAL("Invalid CPU Affinity");
   }

   err_code = rex_common_def_task(p_tcb, &rex_attr);
   if(REX_SUCCESS != err_code)
   {
      REX_ERR_FATAL("Failed to create REX Task");
   }
}

void rex_def_task_ext5
(
   unsigned int        hw_bitmask,
   rex_tcb_type *      p_tcb,             /*!< valid tcb for new task */
   unsigned char *     p_stack,           /*!< stack for new task */
   rex_stack_size_type stack_size,        /*!< stack size in bytes */
   rex_priority_type   priority,          /*!< priority for new task */
   rex_task_func_type  p_task,            /*!< task startup function */
   dword               param,             /*!< parameter for new task */
   char *              task_name,         /*!< A/N string for task name */
   boolean             suspended,         /*!< is task initially suspended? */
   int                 ignored            /*!< ignored   */
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

   err_code = rex_task_attr_set_cpu_affinity(&rex_attr, hw_bitmask);
   if (REX_SUCCESS != err_code)
   {
      REX_ERR_FATAL("Invalid CPU Affinity");
   }

   err_code = rex_common_def_task(p_tcb, &rex_attr);
   if(REX_SUCCESS != err_code)
   {
      REX_ERR_FATAL("Failed to create REX Task");
   }
}

int rex_common_def_task(rex_tcb_type *p_tcb, rex_task_attributes_type *p_attr)
{
   int err_code;
   int attr_validation_check = 0;
   pthread_attr_t attr;
   struct sched_param schd;

   if (NULL == p_tcb)
   {
      return REX_ERROR_BAD_PARAM;
   }
   
   if (NULL == p_attr)
   {
      return REX_ERROR_BAD_PARAM;
   }

   /* IST threads are not allowed to call this function */
   if (rex_check_if_task_is_an_ist_task(rex_self()))
   {
      REX_ERR_FATAL("IST tried to create a REX Task");
   }

   /* This extra validation check is required in case user did not init the attributes type */
   err_code = rex_task_attr_validate(p_attr, &attr_validation_check);
   if ((REX_SUCCESS != err_code) || (REX_ATTR_VALIDATION_OK != attr_validation_check))
   {
      return REX_ERROR_BAD_PARAM;
   }

   /* Adjust the stack start, stack end and size after alignment */
   {
      rex_stack_pointer_type p_adj_stack_start, p_adj_stack_end;
      rex_stack_size_type adj_stack_size;
      stack_align_helper_rev(p_attr->p_orig_stack_limit,
                           p_attr->orig_stack_size,
                           8,
                           &p_adj_stack_start,
                           &adj_stack_size,
                           &p_adj_stack_end);
      //REXTODO
      #ifdef FEATURE_QUARTZ_3X
         p_attr->p_orig_stack_limit = p_adj_stack_end;
      #else
         p_attr->p_orig_stack_limit = p_adj_stack_start;
      #endif
      p_attr->orig_stack_size = adj_stack_size;
   }

   /* Save off the p_tcb in the optional attr pointer */
   p_attr->p_opt = (void*)p_tcb;

   /* Now create the POSIX thread */
   pthread_attr_init(&attr);
   pthread_attr_setstacksize(&attr, p_attr->orig_stack_size);
   pthread_attr_setstackaddr(&attr, p_attr->p_orig_stack_limit);
   SCHED_PARAM_GET_PRIORITY(schd) = p_attr->priority;
   pthread_attr_setschedparam(&attr, &schd);
   pthread_attr_setthreadname(&attr, p_attr->task_name);
   // indicates that no fake tcb should be created.
   pthread_attr_setexternalcontext_np(&attr, -1);
   
   // Create & initialize tcb and associate with p_tcb->p.
   if ( (err_code = rex_create_and_init_os_tcb(p_tcb, p_attr)) != REX_SUCCESS )
   {
      pthread_attr_destroy(&attr);
      return err_code;
   }
   
   /* Pass in default entry func and p_attr as arg */
   pthread_create(&(p_tcb->p->thread_id), &attr, rex_os_thread_entry_func, (void *)p_attr->p_opt);
   /* Set the thread ID member of the user tcb */
   p_tcb->thread_id = p_tcb->p->thread_id;
   
   //REXTODO remove this once 3.x POSIX supports names
   #ifdef FEATURE_QUARTZ_3X
   SYSCALL_thread_setname((sys_thread_t)p_tcb->thread_id, p_os_tcb->task_name);
   #endif
   
   pthread_attr_destroy(&attr);

   return REX_SUCCESS;
}

int rex_create_and_init_os_tcb
(
   rex_tcb_type *p_tcb, 
   rex_task_attributes_type *p_attr
)
{
   int err_code = REX_SUCCESS;
   rex_os_tcb_type *p_os_tcb;

   do
   {
    
	  /* Create the OS TCB */
	  p_os_tcb = calloc(1, sizeof(rex_os_tcb_type));
	  if (NULL == p_os_tcb)
	  {
	    err_code = REX_ERROR_NO_MEM;
		break;
	  }

      /* Init the ROP TCB */
      if( (err_code = rex_tcb_init(p_os_tcb)) != REX_SUCCESS )
      {
         break;
      }
	  
	  /* Init the OS TCB with REX attributes */
	  rex_tcb_init_attr(p_os_tcb, p_attr);

      /* Set the user passed in TCB ref in the ROP TCB */
      REX_INIT_OS_AND_USER_TCB_REFS(p_os_tcb, p_tcb);      

      /* Add this task to the list to list of REX tasks */
      err_code = rex_task_list_mgr_add_task(p_os_tcb);
   } while(0);
   
   if (err_code != REX_SUCCESS && NULL != p_os_tcb)
   {
      rex_tcb_delete(p_os_tcb);
      free(p_os_tcb);
   }

   return err_code;
}

