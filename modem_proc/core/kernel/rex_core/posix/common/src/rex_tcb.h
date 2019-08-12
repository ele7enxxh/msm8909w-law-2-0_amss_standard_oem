#ifndef __REX_TCB_H__
#define __REX_TCB_H__

#include <pthread.h>
#include <semaphore.h>
#include <stringl/stringl.h>
#include "assert.h"
#include "rex.h"
#include "rex_os_tcb_data.h"
#include "posix_np.h"

typedef struct rex_os_tcb
{
   rex_tcb_type               *p_user_tcb;
   pthread_t                  thread_id;
   rex_sigs_type              sigs[REX_SIGS_ARRAY_LEN]; 
   rex_sigs_type              wait[REX_SIGS_ARRAY_LEN];
   boolean                    initial_suspended;
   rex_task_func_type         p_task_entry_func;
   rex_task_func_param_type   param;
   boolean                    is_ist_mode;   //change this to is_ist_task when the TCB is made private
   rex_crit_sect_type         rexl4_lock;    //change this to tcb_lock when the TCB is made private
   rex_ipc_info_type          ipc_info;
   uint32                     rex_nest_depth; //replace with task_lock_ref_ctr when the TCB is made private
   char                       task_name[REX_TASK_NAME_LEN+1];
   rex_stack_pointer_type     p_orig_stack_limit;
   rex_stack_size_type        orig_stack_size;
   rex_priority_type          init_prio;
   rex_cpu_set_type           init_cpu_affinity;
   sem_t                      sig_sem;
   /**
      @note: Watchdog services need to be also managed at the POSIX
      layer. This is being done temporarily until that transition
      occurs. We can use PTHREAD TLS instead for the longer term.
   */
   boolean                    autodog_enabled;
   int                        dog_report_val;
   //REXTODO remove this
   int                        err_num;
   /**
      @note Add any OS specific members here
   */
   REX_OS_SPECIFIC_DATA_MEMBERS;
} rex_os_tcb_type;

#include "rex_os_tcb.h"

INLINE void REX_GET_OS_TCB_REF(rex_tcb_type *p_user_tcb, rex_os_tcb_type **pp_os_tcb)
{
       ASSERT(NULL != p_user_tcb->p);
       *pp_os_tcb = (rex_os_tcb_type*)p_user_tcb->p;
}

INLINE void REX_GET_THREAD_ID(rex_tcb_type *p_user_tcb, pthread_t *thread_id)
{
       *thread_id = p_user_tcb->thread_id;
}

INLINE void REX_GET_USER_TCB_REF(rex_os_tcb_type *p_os_tcb, rex_tcb_type **pp_user_tcb)
{
       *pp_user_tcb = p_os_tcb->p_user_tcb;
}

INLINE void REX_INIT_OS_AND_USER_TCB_REFS(rex_os_tcb_type *p_os_tcb, rex_tcb_type *p_user_tcb)
{
      p_os_tcb->p_user_tcb = p_user_tcb;
      p_user_tcb->p = (void*)p_os_tcb;
}

INLINE void REX_DEINIT_OS_AND_USER_TCB_REFS(rex_os_tcb_type *p_os_tcb, rex_tcb_type *p_user_tcb)
{
      p_user_tcb->p = NULL;
   p_os_tcb->p_user_tcb = NULL;
}

INLINE
void rex_os_set_user_tcb_ref(rex_os_tcb_type *p_os_tcb, rex_tcb_type *p_tcb)
{
      p_os_tcb->p_user_tcb = p_tcb;
}

#define REX_TCB_LOCK_INIT(p_os_tcb)    rex_init_crit_sect(&((p_os_tcb)->rexl4_lock))
#define REX_TCB_LOCK_DELETE(p_os_tcb)  rex_del_crit_sect(&((p_os_tcb)->rexl4_lock))
#define REX_LOCK_TCB(p_os_tcb)         REX_ISR_LOCK(&((p_os_tcb)->rexl4_lock))
#define REX_UNLOCK_TCB(p_os_tcb)       REX_ISR_UNLOCK(&((p_os_tcb)->rexl4_lock))
#define REX_TCB_SEM_POST(p_os_tcb)     sem_post(&(p_os_tcb->sig_sem))
#define REX_TCB_SEM_WAIT(p_os_tcb)     sem_wait(&(p_os_tcb->sig_sem))

INLINE
void rex_os_set_thread_id(rex_os_tcb_type *p_os_tcb, pthread_t thread_id)
{
   p_os_tcb->thread_id = thread_id;
}

INLINE
pthread_t rex_os_get_thread_id(rex_os_tcb_type *p_os_tcb)
{
   return p_os_tcb->thread_id;
}

INLINE
boolean rex_os_get_task_initially_suspended_flag(rex_os_tcb_type *p_os_tcb)
{
   return p_os_tcb->initial_suspended;
}

INLINE
void rex_os_set_task_initially_suspended_flag(rex_os_tcb_type *p_os_tcb, boolean flag)
{
   p_os_tcb->initial_suspended = flag;
}

INLINE
void rex_os_set_task_entry_func
(
   rex_os_tcb_type *p_os_tcb,
   rex_task_func_type p_task_entry_func, 
   rex_task_func_param_type param
)
{
   p_os_tcb->p_task_entry_func = p_task_entry_func;
   p_os_tcb->param = param;
}

INLINE
void rex_os_execute_task_entry_func(rex_os_tcb_type *p_os_tcb)
{
   p_os_tcb->p_task_entry_func(p_os_tcb->param);
}

INLINE
rex_sigs_type *rex_os_get_current_sigs_ref(rex_os_tcb_type *p_os_tcb)
{
   return p_os_tcb->sigs;
}

INLINE
rex_sigs_type *rex_os_get_wait_sigs_ref(rex_os_tcb_type *p_os_tcb)
{
   return p_os_tcb->wait;
}

INLINE
boolean rex_os_get_ist_mode(rex_os_tcb_type *p_os_tcb)
{
   return p_os_tcb->is_ist_mode;
}

INLINE
void rex_os_set_ist_mode(rex_os_tcb_type *p_os_tcb, boolean mode)
{
   p_os_tcb->is_ist_mode = mode;
}

INLINE
rex_ipc_info_type* rex_os_get_ipc_info(rex_os_tcb_type *p_os_tcb)
{
   return &(p_os_tcb->ipc_info);
}

INLINE
uint32 rex_os_get_task_lock_ref_ctr(rex_os_tcb_type *p_os_tcb)
{
   return p_os_tcb->rex_nest_depth;
}

INLINE
uint32 rex_os_incr_task_lock_ref_ctr(rex_os_tcb_type *p_os_tcb)
{
   return p_os_tcb->rex_nest_depth++;
}

INLINE
void rex_os_decr_task_lock_ref_ctr(rex_os_tcb_type *p_os_tcb)
{
   if (0 != p_os_tcb->rex_nest_depth)
   {
      p_os_tcb->rex_nest_depth--;
   }
}

boolean rex_check_if_task_allowed_to_block(rex_os_tcb_type *p_os_tcb);

INLINE
boolean rex_os_get_auto_dog_state(rex_os_tcb_type *p_os_tcb)
{
   return p_os_tcb->autodog_enabled;
}

INLINE
void rex_os_set_auto_dog_state(rex_os_tcb_type *p_os_tcb, boolean state)
{
   p_os_tcb->autodog_enabled = state;
}

INLINE
int rex_os_get_dog_registration_handle(rex_os_tcb_type *p_os_tcb)
{
   return p_os_tcb->dog_report_val;
}

INLINE
void rex_os_set_dog_registration_handle(rex_os_tcb_type *p_os_tcb, int dog_reg_handle)
{
   p_os_tcb->dog_report_val = dog_reg_handle;
}

INLINE
void rex_os_set_stack_addr(rex_os_tcb_type *p_os_tcb, rex_stack_pointer_type p_stack)
{
   p_os_tcb->p_orig_stack_limit = p_stack;
}

INLINE
rex_stack_pointer_type rex_os_get_stack_addr(rex_os_tcb_type *p_os_tcb)
{
   return p_os_tcb->p_orig_stack_limit;
}

INLINE
void rex_os_set_stack_size(rex_os_tcb_type *p_os_tcb, rex_stack_size_type stack_size)
{
   p_os_tcb->orig_stack_size = stack_size;
}

INLINE
rex_stack_size_type rex_os_get_stack_size(rex_os_tcb_type *p_os_tcb)
{
   return p_os_tcb->orig_stack_size;
}

INLINE
void rex_os_set_task_name(rex_os_tcb_type *p_os_tcb, char *task_name)
{
   strlcpy(p_os_tcb->task_name, task_name, sizeof(p_os_tcb->task_name));
}

INLINE
int rex_os_get_task_name(rex_os_tcb_type *p_os_tcb, char *task_name, size_t task_name_size)
{
   return strlcpy(task_name, p_os_tcb->task_name, task_name_size);
}

INLINE
void rex_os_set_init_priority(rex_os_tcb_type *p_os_tcb, rex_priority_type prio)
{
   p_os_tcb->init_prio = prio;
}

INLINE
rex_priority_type rex_os_get_init_priority(rex_os_tcb_type *p_os_tcb)
{
   return p_os_tcb->init_prio;
}

INLINE
void rex_os_set_cpu_affinity(rex_os_tcb_type *p_os_tcb, rex_cpu_set_type cpu_affinity)
{
   p_os_tcb->init_cpu_affinity = cpu_affinity;
}

INLINE
int rex_tcb_init(rex_os_tcb_type *p_os_tcb)
{
   int i;
   if (0 != sem_init(&(p_os_tcb->sig_sem), 0, 0))
   {
      return REX_ERROR;
   }
   rex_os_set_stack_addr(p_os_tcb, NULL);
   rex_os_set_stack_size(p_os_tcb, 0);
   rex_os_set_init_priority(p_os_tcb, 0);
   rex_os_set_task_entry_func(p_os_tcb, NULL, 0);
   rex_os_set_cpu_affinity(p_os_tcb, REX_ANY_CPU_AFFINITY_MASK);
   rex_os_set_task_initially_suspended_flag(p_os_tcb, FALSE);
   rex_os_set_task_name(p_os_tcb, "REX_DEFLT");
   rex_os_set_ist_mode(p_os_tcb, FALSE);
   for (i=0; i<REX_SIGS_ARRAY_LEN; i++)
   {
      p_os_tcb->sigs[i] = 0;
      p_os_tcb->wait[i] = 0;
   }
   rex_os_set_user_tcb_ref(p_os_tcb, NULL);   
   REX_TCB_LOCK_INIT(p_os_tcb);
   rex_tcb_os_specific_init(p_os_tcb);
   return REX_SUCCESS;
}

INLINE
void rex_tcb_init_attr(rex_os_tcb_type *p_os_tcb, rex_task_attributes_type *p_attr)
{
   rex_os_set_stack_addr(p_os_tcb, p_attr->p_orig_stack_limit);
   rex_os_set_stack_size(p_os_tcb, p_attr->orig_stack_size);
   rex_os_set_init_priority(p_os_tcb, p_attr->priority);
   rex_os_set_task_entry_func(p_os_tcb, p_attr->p_task_entry_func, p_attr->task_entry_func_param);
   rex_os_set_cpu_affinity(p_os_tcb, p_attr->cpu_affinity);
   rex_os_set_task_initially_suspended_flag(p_os_tcb, p_attr->is_initially_suspended);
   rex_os_set_task_name(p_os_tcb, p_attr->task_name);
   rex_tcb_os_specific_init_attr(p_os_tcb, p_attr);   
}

INLINE
void rex_tcb_delete(rex_os_tcb_type *p_os_tcb)
{
   rex_tcb_os_specific_delete(p_os_tcb);
   rex_os_set_user_tcb_ref(p_os_tcb, NULL);
   sem_destroy(&(p_os_tcb->sig_sem));
   REX_TCB_LOCK_DELETE(p_os_tcb);
}

void rex_os_pause_dog_monitor(rex_os_tcb_type *p_os_tcb);
void rex_os_resume_dog_monitor(rex_os_tcb_type *p_os_tcb);

INLINE
void REX_PAUSE_DOG_MONITOR(rex_os_tcb_type *p_os_tcb)
{
   if (rex_os_get_auto_dog_state(p_os_tcb))
   {
      rex_os_pause_dog_monitor(p_os_tcb);
   }
}

INLINE 
void REX_RESUME_DOG_MONITOR(rex_os_tcb_type *p_os_tcb)
{
   if (rex_os_get_auto_dog_state(p_os_tcb))
   {
      rex_os_resume_dog_monitor(p_os_tcb);
   }
}

int rex_os_task_lock(rex_os_tcb_type *p_os_tcb);
int rex_os_task_free(rex_os_tcb_type *p_os_tcb);

int rex_os_get_pri(void);

#endif //__REX_TCB_H__
