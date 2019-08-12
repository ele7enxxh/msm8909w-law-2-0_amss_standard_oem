#include <string.h>
#include <stringl/stringl.h>

#include "rex.h"
#include "rex_port.h"

/* Forward Declarations */
static boolean rex_cpu_set_validate(unsigned int cpu_bitmask);

int rex_task_attr_init(rex_task_attributes_type *p_task_attr)
{
   if(NULL == p_task_attr)
   {
      return REX_ERROR_BAD_PARAM;
   }
   p_task_attr->p_orig_stack_limit     = NULL;
   p_task_attr->orig_stack_size        = 0;
   p_task_attr->priority               = 0;
   p_task_attr->p_task_entry_func      = NULL;
   p_task_attr->task_entry_func_param  = 0;
   p_task_attr->is_initially_suspended = FALSE;
   p_task_attr->cpu_affinity           = REX_ANY_CPU_AFFINITY_MASK;
   strlcpy(p_task_attr->task_name, "REX_DEFLT" , (REX_TASK_NAME_LEN + 1));
   p_task_attr->p_opt                  = NULL;

   return REX_SUCCESS;
}

int rex_task_attr_set_stack_addr
(
   rex_task_attributes_type *p_task_attr, 
   rex_stack_pointer_type p_stack
)
{   
   if (NULL == p_task_attr)
   {
      return REX_ERROR_BAD_PARAM;
   }

   if (NULL == p_stack)
   {
      if (!rex_os_is_null_user_stack_supported())
      {
         return REX_ERROR_INVALID;
      }
   }

   p_task_attr->p_orig_stack_limit = p_stack;

   return REX_SUCCESS;
}

int rex_task_attr_set_stack_size
(
   rex_task_attributes_type *p_task_attr, 
   rex_stack_size_type stack_size
)
{
   unsigned int min_stack_size = rex_os_get_task_min_stack_size();

   if (NULL == p_task_attr)
   {
      return REX_ERROR_BAD_PARAM;
   }

   if (stack_size < min_stack_size)
   {
      return REX_ERROR_INVALID;
   }

   p_task_attr->orig_stack_size = stack_size;

   return REX_SUCCESS;
}

int rex_task_attr_get_stack_attr
(
   rex_task_attributes_type *p_task_attr, 
   rex_stack_pointer_type *pp_stack,
   rex_stack_size_type *p_stack_size
)
{
   if ((NULL == p_task_attr) || (NULL == pp_stack) || (NULL == p_stack_size))
   {
      return REX_ERROR_BAD_PARAM;
   }
   
   *pp_stack = p_task_attr->p_orig_stack_limit;
   *p_stack_size = p_task_attr->orig_stack_size;

   return REX_SUCCESS;
}

int rex_task_attr_set_prio(rex_task_attributes_type *p_task_attr, rex_priority_type priority)
{
   unsigned int min_prio = rex_os_get_task_min_priority();
   unsigned int max_prio = rex_os_get_task_max_priority();

   if(NULL == p_task_attr)
   {
      return REX_ERROR_BAD_PARAM;
   }

   if ((min_prio > priority) || (max_prio < priority))
   {
      return REX_ERROR_INVALID;
   }
   
   p_task_attr->priority = priority;

   return REX_SUCCESS;
}

int rex_task_attr_get_prio(rex_task_attributes_type *p_task_attr, rex_priority_type *p_priority)
{
   if ((NULL == p_task_attr) || (NULL == p_priority))
   {
      return REX_ERROR_BAD_PARAM;
   }

   *p_priority = p_task_attr->priority;
   return REX_SUCCESS;
}

int rex_task_attr_set_entry_func
(
   rex_task_attributes_type *p_task_attr, 
   rex_task_func_type p_task_entry_func,
   rex_task_func_param_type param
)
{
   if (NULL == p_task_attr)
   {
      return REX_ERROR_BAD_PARAM;
   }

   if (NULL == p_task_entry_func)
   {
      return REX_ERROR_INVALID;
   }
   
   p_task_attr->p_task_entry_func = p_task_entry_func;
   p_task_attr->task_entry_func_param = param;

   return REX_SUCCESS;
}

int rex_task_attr_set_task_name(rex_task_attributes_type *p_task_attr, char *p_task_name)
{
   if (NULL == p_task_attr)
   {
      return REX_ERROR_BAD_PARAM;
   }

   if (NULL == p_task_name)
   {
      return REX_ERROR_INVALID;
   }

   strlcpy(p_task_attr->task_name, p_task_name, (REX_TASK_NAME_LEN + 1));

   return REX_SUCCESS;
}

int rex_task_attr_get_task_name
(
   rex_task_attributes_type *p_task_attr, 
   char *dest,
   size_t size,
   size_t *p_len
)
{
   int len;

   if ((NULL == p_task_attr) || (NULL == dest))
   {
      return REX_ERROR_BAD_PARAM;
   }
   
   size = ((size < (REX_TASK_NAME_LEN+1))?size:(REX_TASK_NAME_LEN+1));
   len = strlcpy(dest, p_task_attr->task_name, size);

   if (p_len)
   {
      *p_len = len;
   }
   
   return REX_SUCCESS;
}

int rex_task_attr_set_initially_suspended(rex_task_attributes_type *p_task_attr, boolean suspended)
{
   if (NULL == p_task_attr)
   {
      return REX_ERROR_BAD_PARAM;
   }

   if (TRUE == suspended)
   {
      if (!rex_os_is_task_start_in_initially_suspended_state_supported())
      {
         return REX_ERROR_INVALID;
      }
   }

   p_task_attr->is_initially_suspended = suspended;

   return REX_SUCCESS;
}

int rex_task_attr_set_cpu_affinity
(
   rex_task_attributes_type   *p_task_attr, 
   unsigned int               cpu_bitmask
)
{
   if (NULL == p_task_attr)
   {
      return REX_ERROR_BAD_PARAM;
   }

   if (!rex_cpu_set_validate(cpu_bitmask))
   {
      return REX_ERROR_INVALID;
   }

   p_task_attr->cpu_affinity = cpu_bitmask;   

   return REX_SUCCESS;
}

int rex_task_attr_get_cpu_affinity
(
   rex_task_attributes_type   *p_task_attr,
   unsigned int               *p_cpu_bitmask
)
{
   if ((NULL == p_task_attr) || (NULL == p_cpu_bitmask))
   {
      return REX_ERROR_BAD_PARAM;
   }
   *p_cpu_bitmask = p_task_attr->cpu_affinity;

   return REX_SUCCESS;
}

static boolean rex_cpu_set_validate(unsigned int cpu_bitmask)
{
   unsigned int bitmask;

   if (REX_SINGLE_CPU_AFFINITY_MASK != cpu_bitmask)
   {
      bitmask = rex_os_get_task_valid_cpu_affinity_bitmask();
      if (0 == (bitmask & cpu_bitmask))
      {
         return FALSE;
      }
   }
   return TRUE;
}

int rex_task_attr_validate(rex_task_attributes_type *p_task_attr, int *p_reason)
{
   int err_code = REX_ERROR_BAD_PARAM;
   unsigned int min_stack_size;
   unsigned int min_prio;
   unsigned int max_prio;

   do
   {
      if(NULL == p_task_attr)
      {
         break;
      }

      if(NULL == p_reason)
      {
         break;
      }

      /* Re-init err_code */
      err_code = REX_ERROR_INVALID;
      
      min_stack_size = rex_os_get_task_min_stack_size();   
      if (p_task_attr->orig_stack_size < min_stack_size)
      {
         *p_reason = REX_ATTR_VALIDATION_BADSTACK;
         break;
      }

      if (NULL == p_task_attr->p_orig_stack_limit)
      {
         if (!rex_os_is_null_user_stack_supported())
         {
            *p_reason = REX_ATTR_VALIDATION_BADSTACK;      
            break;
         }
      }

      min_prio = rex_os_get_task_min_priority();
      max_prio = rex_os_get_task_max_priority();
      if ((p_task_attr->priority < min_prio) || 
            (max_prio < p_task_attr->priority))
      {
         *p_reason = REX_ATTR_VALIDATION_BADPRIORITY;
         break;
      }

      if (NULL == p_task_attr->p_task_entry_func)
      {
         *p_reason = REX_ATTR_VALIDATION_BADENTRYFUNC;
         break;
      }

      if (0 == strlen(p_task_attr->task_name))
      {
         *p_reason = REX_ATTR_VALIDATION_BADTASKNAME;
         break;
      }

      if (TRUE == p_task_attr->is_initially_suspended)
      {
         if (!rex_os_is_task_start_in_initially_suspended_state_supported())
         {
            *p_reason = REX_ATTR_VALIDATION_BADINITSUSPENDSTATE;
            break;         
         }
      }

      if (FALSE == rex_cpu_set_validate(p_task_attr->cpu_affinity))
      {
         *p_reason = REX_ATTR_VALIDATION_BADCPUAFFINITY;
         break;
      }

      *p_reason = REX_ATTR_VALIDATION_OK;
      err_code = REX_SUCCESS;
   } while(0);   

   return err_code;
}
