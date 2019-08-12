#include "pthread.h"
#include "assert.h"
#include "rex_tcb.h"
#include <stringl/stringl.h>

boolean rex_check_if_task_is_an_ist_task(rex_tcb_type *p_tcb)
{
   rex_os_tcb_type *p_os_tcb;

   if (NULL == p_tcb)
   {
      return FALSE;
   }
   REX_GET_OS_TCB_REF(p_tcb, &p_os_tcb);

   return rex_os_get_ist_mode(p_os_tcb);
}

int rex_get_task_name(rex_tcb_type *p_tcb, char *dest, size_t size, size_t *p_len)
{
   size_t adj_size;
   int ret_val;
   rex_os_tcb_type *p_os_tcb;
   if ((!p_tcb) || (!dest))
   {
      return REX_ERROR_BAD_PARAM;
   }
   do
   {
      REX_GET_OS_TCB_REF(p_tcb, &p_os_tcb);
      adj_size = (size <= (REX_TASK_NAME_LEN+1))?size:(REX_TASK_NAME_LEN+1);
      rex_os_get_task_name(p_os_tcb, dest,  adj_size);
      ret_val = strlcpy(dest, p_os_tcb->task_name, adj_size);
      if (p_len)
      {
         *p_len = ret_val;
      }
   } while(0);

   return REX_SUCCESS;
}

int rex_autodog_enable(int dog_report_val)
{
   //dog_report_val is ignored
   return rex_autodog_enable_self();
}


int rex_get_dog_report_val_self(void)
{
   rex_os_tcb_type *p_os_tcb;
   rex_tcb_type *p_tcb = rex_self();

   ASSERT(NULL != p_tcb);
   
   REX_GET_OS_TCB_REF(p_tcb, &p_os_tcb);
   return rex_os_get_dog_registration_handle(p_os_tcb);

}

void rex_set_dog_report_val_self(int dog_reg_handle)
{
   rex_os_tcb_type *p_os_tcb;
   rex_tcb_type *p_tcb = rex_self();

   ASSERT(NULL != p_tcb);

   REX_GET_OS_TCB_REF(p_tcb, &p_os_tcb);
   rex_os_set_dog_registration_handle(p_os_tcb, dog_reg_handle);
}

int rex_autodog_enable_self(void)
{
   int prev_state;
   rex_os_tcb_type *p_os_tcb;
   rex_tcb_type *p_tcb = rex_self();

   ASSERT(NULL != p_tcb);
   
   REX_GET_OS_TCB_REF(p_tcb, &p_os_tcb);
   prev_state = ((TRUE == rex_os_get_auto_dog_state(p_os_tcb))?1:0);
   rex_os_set_auto_dog_state(p_os_tcb, TRUE);
   return prev_state;
}

void rex_set_dog_report_val(rex_tcb_type *p_tcb, int dog_report_val)
{
   rex_os_tcb_type *p_os_tcb;
   ASSERT(p_tcb != NULL);
   REX_GET_OS_TCB_REF(p_tcb, &p_os_tcb);
   rex_os_set_dog_registration_handle(p_os_tcb, dog_report_val);
}

int rex_get_dog_report_val(rex_tcb_type *p_tcb)
{
   rex_os_tcb_type *p_os_tcb;
   ASSERT(p_tcb != NULL);
   REX_GET_OS_TCB_REF(p_tcb, &p_os_tcb);   
   return rex_os_get_dog_registration_handle(p_os_tcb);
}

int rex_autodog_disable(void)
{
   int prev_state;
   rex_os_tcb_type *p_os_tcb;
   rex_tcb_type *p_tcb = rex_self();

   ASSERT(NULL != p_tcb);

   REX_GET_OS_TCB_REF(p_tcb, &p_os_tcb);
   prev_state = ((TRUE == rex_os_get_auto_dog_state(p_os_tcb))?1:0);
   rex_os_set_auto_dog_state(p_os_tcb, FALSE);
   return prev_state;
}

int rex_get_ipc_info_ref(rex_tcb_type *p_tcb, rex_ipc_info_type **pp_node)
{
   rex_os_tcb_type *p_os_tcb;

   if ((NULL == p_tcb) || (NULL == pp_node))
   {
      return REX_ERROR_BAD_PARAM;
   }
   REX_GET_OS_TCB_REF(p_tcb, &p_os_tcb);
   *pp_node = rex_os_get_ipc_info(p_os_tcb);
   return REX_SUCCESS;
}

int rex_task_get_attr(rex_tcb_type *p_tcb, rex_task_attributes_type *p_task_attr)
{
   pthread_attr_t attr;
   pthread_t thread_id;
   rex_os_tcb_type *p_os_tcb;
   int ret_val, policy;
   struct sched_param sch_param_get;
   size_t stack_size;
   cpu_set_t cpu_set;
   if ((NULL == p_tcb) || (NULL == p_task_attr))
   {
      return REX_ERROR_BAD_PARAM;
   }

   REX_GET_OS_TCB_REF(p_tcb, &p_os_tcb);
   thread_id = rex_os_get_thread_id(p_os_tcb);
   
   pthread_attr_init(&attr);
   do
   {
      ret_val = pthread_getattr_np(thread_id, &attr);
      if( 0 != ret_val)
      {
         break;
      }
      pthread_getschedparam(thread_id, &policy, &sch_param_get);
      pthread_attr_getthreadname(&attr, p_task_attr->task_name, (REX_TASK_NAME_LEN+1));
      pthread_attr_getstacksize(&attr, &stack_size);
      p_task_attr->orig_stack_size = stack_size;
      pthread_attr_getstackaddr(&attr, (void**)(&(p_task_attr->p_orig_stack_limit)));
      p_task_attr->priority = SCHED_PARAM_GET_PRIORITY(sch_param_get);
      pthread_attr_getaffinity_np(&attr, sizeof(cpu_set_t), &cpu_set);
      p_task_attr->cpu_affinity = cpu_set;
   } while(0);
   pthread_attr_destroy(&attr);
   
   return ((0 == ret_val)?REX_SUCCESS:REX_ERROR_INVALID);
}
