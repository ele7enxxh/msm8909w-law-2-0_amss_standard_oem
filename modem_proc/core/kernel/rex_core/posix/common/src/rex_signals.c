#include <string.h>
#include "pthread.h"
#include "signal.h"

#include "rex.h"
#include "rex_port.h"
#include "rex_tcb.h"

#define SIGREX SIGRTMIN
static uint32 REX_ON_POSIX_SIG = SIGRTMIN;
#define REX_SIGS_ON_SEM
static sigset_t rex_sig_set;

/*=================================================================================================
      FORWARD DECLARATIONS
=================================================================================================*/
INLINE 
int rex_os_clr_sigs
(
   rex_tcb_type* p_tcb, 
   const rex_sigs_type *p_sigs, 
   rex_sigs_type *prev_sigs, 
   int sigs_len
);

INLINE
int rex_os_set_sigs
(
   rex_tcb_type *p_tcb, 
   const rex_sigs_type *p_sigs, 
   rex_sigs_type *prev_sigs, 
   int sigs_len
);

INLINE
int rex_os_get_sigs(rex_tcb_type *p_tcb, rex_sigs_type *p_sigs, int sigs_len);

INLINE
int rex_os_wait_on_sigs
(
   const rex_sigs_type *p_wait_sigs, 
   rex_sigs_type *p_rcvd_sigs, 
   int sigs_len
);

rex_sigs_type rex_clr_sigs
(
   rex_tcb_type   *p_tcb,        /* tcb for which the signals will be cleared */
   rex_sigs_type   clr_sigs      /* which signals to clear                    */
)
{
   rex_sigs_type prev_sigs;
   int err_code;

   if (NULL == p_tcb)
   {
      REX_ERR_FATAL("REX TCB ptr is invalid");
   }

   /*-------------------------------------------------------
   ** Clear signals
   **-----------------------------------------------------*/
   err_code = rex_os_clr_sigs(p_tcb, &clr_sigs, &prev_sigs, 1);
   if (REX_SUCCESS != err_code)
   {
      REX_ERR_FATAL("REX Clear Sigs failed");
   }
   
   return prev_sigs;
}

boolean rex_clr_sigs_ext
(
   rex_tcb_type         *p_tcb,         /* tcb for which the signals will be cleared */
   const rex_sigs_type  *p_clr_sigs     /* which signals to clear */
)
{
   rex_sigs_type prev_sigs;
   int err_code;

   if (NULL == p_tcb)
   {
      return FALSE;
   }

   if (NULL == p_clr_sigs)
   {
      return FALSE;
   }

   /*-------------------------------------------------------
   ** Clear signals
   **-----------------------------------------------------*/
   err_code = rex_os_clr_sigs(p_tcb, p_clr_sigs, &prev_sigs, REX_SIGS_ARRAY_LEN);
   if (REX_SUCCESS != err_code)
   {
      REX_ERR_FATAL("REX Clear Sigs failed");
   }   

   return TRUE;
}

rex_sigs_type rex_wait
(
   rex_sigs_type wait_sigs          /* sigs to wait on */
)
{
   int err_code;
   rex_sigs_type recvd_sigs;

   err_code = rex_os_wait_on_sigs(&wait_sigs, &recvd_sigs, 1);
   if(REX_SUCCESS != err_code)
   {
      REX_ERR_FATAL("Error in REX wait");
   }
   
   return recvd_sigs;
}

boolean rex_wait_ext
(
   const rex_sigs_type* p_wait_sigs,      /* sigs to wait on */
   rex_sigs_type* p_rcvd_sigs             /* sigs received */
)
{
   int err_code;
   
   if (NULL == p_wait_sigs)
   {
      return FALSE;
   }

   if (NULL == p_rcvd_sigs)
   {
      return FALSE;
   }
   
   err_code = rex_os_wait_on_sigs(p_wait_sigs, p_rcvd_sigs, REX_SIGS_ARRAY_LEN);
   if(REX_SUCCESS != err_code)
   {
      REX_ERR_FATAL("Error in REX wait ext");
   }

   return TRUE;
}

rex_sigs_type rex_set_sigs
(
   rex_tcb_type   *p_tcb,           /* tcb for which the sigs will be set */
   rex_sigs_type   set_sigs         /* the sigs to set                    */
)
{
   int err_code;
   rex_sigs_type prev_sigs;

   if (NULL == p_tcb)
   {
      REX_ERR_FATAL("REX TCB ptr is invalid");
   }

   err_code = rex_os_set_sigs(p_tcb, &set_sigs, &prev_sigs, 1);
   if (REX_SUCCESS != err_code)
   {
      REX_ERR_FATAL("REX Set Sigs Failed");
   }

   return prev_sigs;
}

boolean rex_set_sigs_ext
(
   rex_tcb_type         *p_tcb,           /* tcb for which the sigs will be set */
   const rex_sigs_type  *p_set_sigs       /* the pointer of the extension array to set */
)
{
   int err_code;
   rex_sigs_type prev_sigs;
   
   if (NULL == p_tcb)
   {
      return FALSE;
   }

   if (NULL == p_set_sigs)
   {
      return FALSE;
   }

   err_code = rex_os_set_sigs(p_tcb, p_set_sigs, &prev_sigs, REX_SIGS_ARRAY_LEN);
   if (REX_SUCCESS != err_code)
   {
      REX_ERR_FATAL("REX Set Sigs Ext Failed");
   }

   return TRUE;
}

/*lint -e{818} p_tcb could be declared const */
rex_sigs_type rex_get_sigs
(
   rex_tcb_type *p_tcb /* tcb for which sigs will be returned */
)
{
   rex_sigs_type ret_sigs;
   int err_code;

   if (NULL == p_tcb)
   {
      return 0;
   }

   err_code = rex_os_get_sigs(p_tcb, &ret_sigs, 1);
   if (REX_SUCCESS != err_code)
   {
      REX_ERR_FATAL("REX Get Sigs Failed");
   }
   
   return ret_sigs;
}

boolean rex_get_sigs_ext
(
   rex_tcb_type   *p_tcb,             /* tcb for which sigs will be returned */
   rex_sigs_type  *p_get_sigs         /* the pointer of the extension array */
)
{  
   int err_code;

   if (NULL == p_tcb)
   {
      return FALSE;
   }

   if (NULL == p_get_sigs)
   {
      return FALSE;      
   }
   
   err_code = rex_os_get_sigs(p_tcb, p_get_sigs, REX_SIGS_ARRAY_LEN);
   if (REX_SUCCESS != err_code)
   {
      REX_ERR_FATAL("REX Get Sigs Ext Failed");
   }
   
   return TRUE;
}

INLINE int rex_os_clr_sigs
(
   rex_tcb_type *p_tcb, 
   const rex_sigs_type *p_sigs, 
   rex_sigs_type *p_ret_sigs,
   int sigs_array_limit
)
{
   int index = 0;
   rex_sigs_type *p_tcb_curr_sigs;
   rex_os_tcb_type *p_os_tcb;

   REX_GET_OS_TCB_REF(p_tcb, &p_os_tcb);

   /* Get the pointer to the currently active sigs */
   p_tcb_curr_sigs = rex_os_get_current_sigs_ref(p_os_tcb);
   
   REX_LOCK_TCB(p_os_tcb);
   {
      *p_ret_sigs = *p_tcb_curr_sigs;
      do
      {
         p_tcb_curr_sigs[index] &= ~(p_sigs[index]);
      } while(++index < sigs_array_limit);
   }
   REX_UNLOCK_TCB(p_os_tcb);

   return REX_SUCCESS;
}

INLINE int rex_os_set_sigs
(
   rex_tcb_type *p_tcb, 
   const rex_sigs_type *p_sigs, 
   rex_sigs_type *p_ret_sigs, 
   int sigs_array_limit
)
{
   int index, err_code;
   boolean found_matched_sigs = FALSE;
   rex_os_tcb_type *p_os_tcb;
   rex_sigs_type *p_tcb_curr_sigs, *p_tcb_wait_sigs;
   
   REX_GET_OS_TCB_REF(p_tcb, &p_os_tcb);

   /* Get the pointer to the currently active sigs */
   p_tcb_curr_sigs = rex_os_get_current_sigs_ref(p_os_tcb);

   /* Get the pointer to the TCB's wait sigs */
   p_tcb_wait_sigs = rex_os_get_wait_sigs_ref(p_os_tcb);
   
   REX_LOCK_TCB(p_os_tcb);
   {
      *p_ret_sigs = *p_tcb_curr_sigs;
      index = 0;
      do
      {
         /* set sigs */
         p_tcb_curr_sigs[index] |= p_sigs[index];

         /*--------------------------------------------------------
         ** Check and see if the p_os_tcb task is waiting for the sigs.
         **--------------------------------------------------------*/         
         found_matched_sigs = found_matched_sigs || (0 != (p_tcb_wait_sigs[index] & p_sigs[index]));
      } while(++index < sigs_array_limit);
      
      /* if the p_os_tcb task is waiting for some sigs that being set, send a 
       * signal out to notify the receiver */
      if (found_matched_sigs)
      {
         /* clear out the wait sigs array */
         index = 0;
         do
         {
            p_tcb_wait_sigs[index] = 0;
         } while(++index < sigs_array_limit);

         #if defined(REX_SIGS_ON_SEM)
         {
            err_code = REX_TCB_SEM_POST(p_os_tcb);
            if (0 != err_code)
            {
               REX_ERR_FATAL("Error when OS sent async signal");
            }            
         }
         #else
         {
            err_code = pthread_kill(rex_os_get_thread_id(p_os_tcb), REX_ON_POSIX_SIG);
            if (REX_SUCCESS != err_code)
            {
               REX_ERR_FATAL("Error when OS sent async signal");
            }
         }
         #endif
      }
   }
   REX_UNLOCK_TCB(p_os_tcb);

   return REX_SUCCESS;
}

INLINE int rex_os_get_sigs(rex_tcb_type *p_tcb, rex_sigs_type *p_sigs, int sigs_array_limit)
{
   rex_os_tcb_type *p_os_tcb;
   rex_sigs_type *p_tcb_curr_sigs;
   
   REX_GET_OS_TCB_REF(p_tcb, &p_os_tcb);
   
   p_tcb_curr_sigs = rex_os_get_current_sigs_ref(p_os_tcb);

   if (!sigs_array_limit)
   {
      *p_sigs = *p_tcb_curr_sigs;
   }
   else
   {
      int index = 0;
      REX_LOCK_TCB(p_os_tcb);
      do
      {
         p_sigs[index] = p_tcb_curr_sigs[index];
      } while(++index < sigs_array_limit);
      REX_UNLOCK_TCB(p_os_tcb);
   }
   
   return REX_SUCCESS;
}

INLINE int rex_os_wait_on_sigs
(
   const rex_sigs_type *p_wait_sigs, 
   rex_sigs_type *p_rcvd_sigs, 
   int sigs_array_limit
)
{
   int err_code, index;
   boolean is_signal_match = FALSE;
   rex_os_tcb_type *p_os_tcb;
   rex_tcb_type *p_tcb;
   rex_sigs_type *p_tcb_curr_sigs, *p_tcb_wait_sigs;

   p_tcb = rex_self();
   REX_GET_OS_TCB_REF(p_tcb, &p_os_tcb);

   if(!rex_check_if_task_allowed_to_block(p_os_tcb))
   {
      REX_ERR_FATAL("Error: Cannot block task");
   }

   REX_LOCK_TCB(p_os_tcb);
   {
      /* Get the pointer to the currently active sigs */
      p_tcb_curr_sigs = rex_os_get_current_sigs_ref(p_os_tcb);

      /*-------------------------------------------------------
      ** if no signals are set, wait, and reschedule
      **-----------------------------------------------------*/
      index = 0;
      do
      {
         if (0 != (p_tcb_curr_sigs[index] & p_wait_sigs[index]))
         {
            /* some wait signal has been set, need to return immediately */
            is_signal_match = TRUE;
            break;
         }      
      } while(++index < sigs_array_limit);


      if (FALSE == is_signal_match)
      {
         /* Get the pointer to the TCB's wait sigs */
         p_tcb_wait_sigs = rex_os_get_wait_sigs_ref(p_os_tcb);
         /* set wait[] in tcb */
         index = 0;
         do
         {
            p_tcb_wait_sigs[index] = p_wait_sigs[index];
         } while(++index < sigs_array_limit);
      }
   }
   REX_UNLOCK_TCB(p_os_tcb);

   if (FALSE == is_signal_match)
   {
      /* Tell Dog to stop monitoring this task. */
      REX_PAUSE_DOG_MONITOR(p_os_tcb);

      #if defined(REX_SIGS_ON_SEM)
      {
         err_code = REX_TCB_SEM_WAIT(p_os_tcb);
         if (0 != err_code)
         {
            REX_ERR_FATAL("Got the wrong sig in rex_wait");
         }         
      }
      #else
      {
         int psx_sig = 0;
         err_code = sigwait(&rex_sig_set, &psx_sig);
         if (0 != err_code)
         {
            REX_ERR_FATAL("Got the wrong sig in rex_wait");
         }

         if (REX_ON_POSIX_SIG != psx_sig)
         {
            REX_ERR_FATAL("Woke up due to incorrect sig");
         }
      }
      #endif

      /* Tell Dog to resume monitoring me */
      REX_RESUME_DOG_MONITOR(p_os_tcb);
   }

   /*-------------------------------------------------------
   ** return with the signal mask of the current task
   **-----------------------------------------------------*/
   for (index=0; index < sigs_array_limit; index++)
   {
      p_rcvd_sigs[index] = p_tcb_curr_sigs[index];
   }

   return REX_SUCCESS;
}

void rex_sigs_init(void)
{
   sigemptyset(&rex_sig_set);
   sigaddset(&rex_sig_set, REX_ON_POSIX_SIG);
}
