#include "rex.h"
#include "rex_port.h"
#include "timer.h"

/*lint -esym(765,rex_timer_group) supress warning 765 */
timer_group_type  rex_timer_group;
extern rex_tcb_type timer_tcb;

typedef enum
{
   REX_TIMER_OP_DEF_TIMER = 0,
   REX_TIMER_OP_TIMED_WAIT,
   
   REX_TIMER_OP_INVALID
} rex_timer_op;

static boolean rex_timer_check_if_task_is_allowed_to_peform_operation
(
   rex_tcb_type *p_tcb, 
   rex_timer_op oper
)
{
   boolean is_allowed = FALSE;
   
   switch(oper)
   {
      case REX_TIMER_OP_DEF_TIMER:
      case REX_TIMER_OP_TIMED_WAIT:
         /* Check if the timer task is trying to do either of the following and if so return FALSE
          * 
          * 1) Init/create a new timer:
          * A situation in which the timer task inadvertently attempts to do this is when 
          * another task creates a timer using rex_def_timer/_ex() with a callback(cb) 
          * func and that cb func when executed, issues a call to this function. If this 
          * were allowed, the timer created will be created in the wrong context since the 
          * context in which the cb func runs is the timer task's.
          *
          * 2) Timed Wait
          * If this condition is not checked, it can result in a watchdog timeout since 
          * the timer task cannot send itself any signal on timer expiration since it 
          * is blocked by the call to rex_wait.
          * A situation in which the timer task inadvertently attempts to do this is when 
          * another task does a rex_def_timer/_ex() with a callback(cb) func and that cb func  
          * does a rex_timed_wait().
          */
         if(p_tcb != &timer_tcb)
         {
            is_allowed = TRUE;
         }

         break;
      default:
         is_allowed = TRUE;
         break;
   }

   return is_allowed;
}

void rex_def_timer
(
   rex_timer_type  *p_timer,     /* pointer to a valid timer structure */
   rex_tcb_type    *p_tcb,       /* tcb to associate with the timer    */
   rex_sigs_type    sigs         /* sigs to set upon timer expiration  */
)
{
   if (NULL == p_timer)
   {
      REX_ERR_FATAL("Error bad timer pointer passed in rex_def_timer");
   }

   if (NULL == p_tcb)
   {
      REX_ERR_FATAL("Error bad TCB pointer passed in rex_def_timer");
   }

   if (!rex_timer_check_if_task_is_allowed_to_peform_operation
      (p_tcb, REX_TIMER_OP_DEF_TIMER))
   {
      REX_ERR_FATAL("This task is not allowed to call rex_def_timer and its variants.");
   }

   timer_def(p_timer, &rex_timer_group, p_tcb, sigs, NULL, 0);
}

rex_timer_cnt_type rex_clr_timer(rex_timer_type *p_timer)
{
   if (NULL == p_timer)
   {
      REX_ERR_FATAL("Error bad timer pointer passed in rex_clr_timer");
   }

   return timer_clr(p_timer, T_MSEC);
}

rex_timer_cnt_type rex_set_timer
(
   rex_timer_type     *p_timer,  /* pointer to timer to set  */
   rex_timer_cnt_type  cnt       /* value to set the timer   */
)
{
   rex_timer_cnt_type prev_value; /* the value before the set */
   
   if (NULL == p_timer)
   {
      REX_ERR_FATAL("Error bad timer pointer passed in rex_set_timer");
   }

   prev_value = timer_get(p_timer, T_MSEC);

   timer_set(p_timer, cnt, 0, T_MSEC);

   return prev_value;
}

rex_timer_cnt_type rex_get_timer
(
  rex_timer_type *p_timer       /* pointer to the timer to get */
)
{
   if (NULL == p_timer)
   {
      REX_ERR_FATAL("Error bad timer pointer passed in rex_get_timer");
   }
   
   return timer_get(p_timer, T_MSEC);
}

void rex_pause_timer
(
   rex_timer_type *p_timer
)
{
   if (NULL == p_timer)
   {
      REX_ERR_FATAL("Error bad timer pointer passed in rex_pause_timer");
   }

   timer_pause(p_timer);
}

void rex_resume_timer
(
   rex_timer_type *p_timer
)
{
   if (NULL == p_timer)
   {
      REX_ERR_FATAL("Error bad timer pointer passed in rex_resume_timer");
   }

   timer_resume(p_timer);
}

rex_sigs_type rex_timed_wait
(
   rex_sigs_type     sigs,          /* sigs to wait on          */
   rex_timer_type    *p_timer,      /* timer to set and wait on */
   rex_timer_cnt_type cnt           /* timer to wait            */
)
{
   rex_tcb_type *rex_self_tcb;
   
   if (NULL == p_timer)
   {
      REX_ERR_FATAL("Error bad timer pointer passed in rex_timed_wait");
   }
   
   rex_self_tcb = rex_self();
   if (NULL == rex_self_tcb)
   {
      REX_ERR_FATAL("Error not a REX task");
   }

   /* Check if this task is allowed to perform a timed wait */   
   if (!rex_timer_check_if_task_is_allowed_to_peform_operation(
      rex_self_tcb, REX_TIMER_OP_TIMED_WAIT))
   {
      REX_ERR_FATAL("Error: Cannot allow this task to perform a REX timed wait");
   }

   /*-------------------------------------------------------
   ** A call to rex_set_timer and rex_wait will perform a
   ** timed wait
   **-----------------------------------------------------*/
   (void) rex_set_timer(p_timer, cnt);
   
   return rex_wait(sigs);
}

void rex_def_timer_ex
(
   rex_timer_type    *p_timer,            /* pointer to a valid timer structure  */
   rex_timer_cb_type  timer_cb_ptr,       /* pointer to timer callback           */
   unsigned long      cb_param            /* argument passed to timer callback   */
)
{
   rex_tcb_type *rex_self_tcb;

   if (NULL == p_timer)
   {
      REX_ERR_FATAL("Error bad timer pointer passed in rex_def_timer_ex");
   }

   rex_self_tcb = rex_self();
   if (NULL == rex_self_tcb)
   {
      REX_ERR_FATAL("Error not a REX task");
   }
   
   if (!rex_timer_check_if_task_is_allowed_to_peform_operation
      (rex_self_tcb, REX_TIMER_OP_DEF_TIMER))
   {
      REX_ERR_FATAL("This task is not allowed to call rex_def_timer and its variants.");
   }

   timer_def(p_timer, &rex_timer_group, rex_self_tcb, 0, timer_cb_ptr, cb_param);   
}

void rex_undef_timer
(
   rex_timer_type *p_timer
)
{
    if(p_timer)
    {
        timer_undef(p_timer);
    }
}

boolean rex_queue_apc_internal
(
   unsigned long apc_handle,
   unsigned long tcb_handle,
   unsigned long param
)
{
   REX_ERR_FATAL("API rex_queue_apc_internal() is not supported");
   return FALSE;
}

rex_timer_type *rex_create_timer_ex
(
   rex_timer_cb_type  timer_cb_ptr,    /* pointer to timer callback           */
   unsigned long      cb_param         /* argument passed to timer callback   */
)
{
   rex_timer_type *p_timer = NULL;
   rex_tcb_type *rex_self_tcb;

   do
   {
      if (NULL == timer_cb_ptr)
      {
         break;
      }

      rex_self_tcb = rex_self();
      if (NULL == rex_self_tcb)
      {
         break;
      }

      if (!rex_timer_check_if_task_is_allowed_to_peform_operation
         (rex_self_tcb, REX_TIMER_OP_DEF_TIMER))
      {
         break;
      }

      p_timer = (rex_timer_type *) calloc(1, sizeof(rex_timer_type));
      if (NULL == p_timer)
      {
         break;
      }

      timer_def(p_timer, &rex_timer_group, rex_self_tcb, 0, timer_cb_ptr, cb_param);

   } while(0);

   return p_timer;
}

void rex_delete_timer_ex(rex_timer_type *p_timer)
{
   (void)timer_undef(p_timer);
   free(p_timer);
}
