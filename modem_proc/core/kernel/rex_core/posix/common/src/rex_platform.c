#include "assert.h"
#include "pthread.h"
#include "sched.h"
#include "rex.h"
#include "rex_port.h"
#include "rex_tcb.h"

#define REX_CORE_OS_SLEEP_SIG_MASK     0x80000000
int rex_any_task_allowed_to_block = 1;

//REXTODO use sysconf to find this @ runtime
unsigned int rex_os_get_task_min_stack_size(void)
{
   int size;
   #if defined(PTHREAD_STACK_MIN)
      size = PTHREAD_STACK_MIN;
   #else
      size = 0;   
   #endif

   return size;
}

unsigned int rex_os_get_task_min_priority(void)
{
   return sched_get_priority_min(SCHED_OTHER);
}

unsigned int rex_os_get_task_max_priority(void)
{
   //REXTODO do this temporarily until this POSIX API is fixed for Unicore builds
   return REX_OS_TASK_MAX_PRIORITY;
   //return sched_get_priority_max(SCHED_OTHER);
}

boolean rex_os_is_task_start_in_initially_suspended_state_supported(void)
{
   return FALSE;
}

rex_priority_type rex_set_task_pri(rex_tcb_type *p_tcb, rex_priority_type priority)
{
   int err_code, policy;
   struct sched_param sch_param_set, sch_param_get;
   pthread_t thread_id;
   rex_os_tcb_type *p_os_tcb;   

   if (NULL == p_tcb)
   {
      REX_ERR_FATAL("REX TCB ptr is invalid");
   }

   REX_GET_OS_TCB_REF(p_tcb, &p_os_tcb);
   thread_id = rex_os_get_thread_id(p_os_tcb);

   err_code = pthread_getschedparam(thread_id, &policy, &sch_param_get);
   if (0 != err_code)
   {
      REX_ERR_FATAL("REX get priority failed");
   }

   /* Set prio for the calling process */
   SCHED_PARAM_GET_PRIORITY(sch_param_set) = priority;
   err_code = pthread_setschedparam(thread_id, policy, &sch_param_set);
   if (0 != err_code)
   {
      REX_ERR_FATAL("REX set priority failed");
   }

   return SCHED_PARAM_GET_PRIORITY(sch_param_get);
}

void rex_yield(void)
{
   /* It is not allowed to yield when TASKLOCKed */
   if (rex_check_if_tasks_are_locked())
   {
      return;
   }

   if (sched_yield())
   {
       REX_ERR_FATAL("sched yield failed");
   }
}

void rex_sleep(unsigned long ms)
{
   boolean        sleeping = TRUE;
   boolean        sig_set  = FALSE;
   unsigned long  tmo      = ms;
   rex_timer_type sleep_timer;

   rex_tcb_type *p_self_tcb = rex_self();

   rex_def_timer(&sleep_timer, p_self_tcb, REX_CORE_OS_SLEEP_SIG_MASK);

   while (sleeping)
   {
      (void) rex_timed_wait(REX_CORE_OS_SLEEP_SIG_MASK, &sleep_timer, tmo);
      if (0 == (tmo = rex_get_timer(&sleep_timer)))
      {
         sleeping = FALSE;
      }
      else
      {
         sig_set = TRUE;
      }
   }
   
   if (!sig_set)
   {
      (void) rex_clr_sigs(p_self_tcb, REX_CORE_OS_SLEEP_SIG_MASK);
   }
   
   rex_undef_timer(&sleep_timer);
   
   return;
}

void rex_set_task_as_ist_task(rex_tcb_type *p_tcb)
{
   rex_os_tcb_type *p_os_tcb;
   REX_GET_OS_TCB_REF(p_tcb, &p_os_tcb);
   rex_os_set_ist_mode(p_os_tcb, TRUE);
}

rex_priority_type rex_get_pri(void)
{
   int policy;
   struct sched_param sch_param_get;

   if(0 == pthread_getschedparam(pthread_self(), &policy, &sch_param_get))
   {
   return SCHED_PARAM_GET_PRIORITY(sch_param_get);
   }
   else
   {
      return rex_os_get_pri();
   }
}

void rex_suspend_task(rex_tcb_type *p_tcb)
{
   REX_ERR_FATAL("API is not supported. Please use REX signals to suspend a task indefinately.");
}

void rex_resume_task(rex_tcb_type *p_tcb)
{
   REX_ERR_FATAL("API is not supported. Please use REX signals to resume a suspended task.");
}

boolean rex_check_if_task_allowed_to_block(rex_os_tcb_type *p_os_tcb)
{
   //REXTODO remove when finalized
   if (rex_any_task_allowed_to_block)
   {
      return TRUE;
   }

   /* Check if IST is allowed to call this function */
   if (rex_os_get_ist_mode(p_os_tcb))
   {
      return FALSE;
   }

   /* TASKLOCKed TASK is not allowed to block */
   if (rex_check_if_tasks_are_locked())
   {
      return FALSE;
   }

   return TRUE;
}
