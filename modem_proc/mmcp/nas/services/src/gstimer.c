/*===========================================================================
                        COPYRIGHT INFORMATION

Copyright (c) 2001 Qualcomm Technologies, Incorporated and its licensors.  All Rights 
Reserved.  QUALCOMM Proprietary.  Export of this technology or software 
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/MSM5200/nas/vcs/gstimer.c_v   1.2   01 Jul 2002 09:30:12   jault  $   
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/nas/services/src/gstimer.c#1 $    $DateTime: 2016/12/13 08:00:05 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
04/19/01   TS      Now sends an error when an invalid timer command is received.
07/01/02   jca     Modified function gs_enquire_timer(). 
09/11/03   kwa     Added INTLOCK and INTFREE to all functions to eliminate
                   reentrancy.  Reentrant calls to gs_clear_timer and
                   gs_cancel_timer created a condition where an attempt was
                   made to delete a NULL timer pointer.
02/11/04   ks      Added ERR_FATAL when a NULL pointer is passed in gs_modify_timer
                   Added new function gs_timer_exists() to validate a timer.
02/22/04   ks      Added a NULL check before processing the timer in 
                   gs_modify_timer() and removed the stringent ERR_FATAL.
===========================================================================*/

/* Include files */

#include "customer.h"
#include "rex.h"
#include "sys_type.h"
#include "sys_cnst_v.h"
#include "sys_stru_v.h"
#include "timers_v.h"
#include "gs_v.h"
#include "ms_timer_v.h"
#include "msg.h"
#include "err.h"
#include "mm_v.h"
#include "timer.h"

timer_info_T task_timer_info[GS_MAX_TIMER_CLIENT+1][MAXNO_TIMERS];

timer_group_type nas_timer_group_non_deferrable;

/*******************************************************************************
 *
 *  Function name: gs_start_timer
 *  -------------------------------
 *  Description:
 *  ------------
 *
 *
 *  Parameters:
 *  -----------
 *  IN         queue_id
 *  IN         timer_id
 *  IN         num_milli_secs
 *  IN         data
 *
 *  Returns:
 *  --------
 *  status
 *
 ******************************************************************************/

gs_status_T   gs_start_timer(gs_queue_id_T     queue_id,
                             timer_id_T        timer_id,
                             rex_timer_cb_type func_ptr,        
                             milli_secs_T      milli_secs,
                             byte              data)
{
   unsigned long param;
   gs_status_T   status = GS_FAILURE;
   
   if((queue_id <= GS_MAX_TIMER_CLIENT) && (timer_id < MAXNO_TIMERS))
   {
       rex_enter_crit_sect(&timer_crit_sect);

       /* Check to see if timer already exists */
       if(task_timer_info[queue_id][timer_id].timer != NULL)
       {
          /* Stop timer */
          (void) rex_clr_timer(task_timer_info[queue_id][timer_id].timer);

          /* Deallocate timer */
          rex_delete_timer_ex(task_timer_info[queue_id][timer_id].timer);
    
          /* Make sure to mark this timer as inactive */
          task_timer_info[queue_id][timer_id].timer = NULL;
       }
    
       /* Copy data to param */
       param     = data;
    
       /* Copy timer id to param */
       param |= timer_id << 16;
    
       /* Allocate timer */
       task_timer_info[queue_id][timer_id].timer = rex_create_timer_ex(func_ptr, param);
    
       /* If memory was available */  
       if(task_timer_info[queue_id][timer_id].timer != NULL)
       {  
          /* Start Timer */
          (void) rex_set_timer(task_timer_info[queue_id][timer_id].timer, milli_secs);
    
          /* Return Success */
          status = GS_SUCCESS;
       }
    
       rex_leave_crit_sect(&timer_crit_sect);
    }
    else
    {
       /* Raise error condition */
       ERR("ERROR: Invalid timer, queue=%d, timer=%d",queue_id, timer_id,0);

       status = GS_FAILURE;
    }

    return status;
}

/*******************************************************************************
 *
 *  Function name: gs_start_timer_ext
 *  -------------------------------
 *  Description:
 *  ------------
 *  Generic timer start function. Takes extra parameter to specify if 
 *  the timer is non-deferrable
 *
 *  Parameters:
 *  -----------
 *  IN         queue_id
 *  IN         timer_id
 *  IN         num_milli_secs
 *  IN         data
 *  IN         non_deferrable
 *
 *  Returns:
 *  --------
 *  status
 *
 ******************************************************************************/

gs_status_T   gs_start_timer_ext( gs_queue_id_T           queue_id,
                                  timer_id_T              timer_id,
                                  gs_non_def_timer_cb_ptr func_ptr,        
                                  milli_secs_T            milli_secs,
                                  byte                    data,
                                  boolean                 non_deferrable )
{
   unsigned long param;
   gs_status_T   status = GS_FAILURE;
   timer_type *timer_p = NULL;
   
   if((queue_id <= GS_MAX_TIMER_CLIENT) && (timer_id < MAXNO_TIMERS))
   {
       rex_enter_crit_sect(&timer_crit_sect);

       /* Check to see if timer already exists */
       if(task_timer_info[queue_id][timer_id].timer != NULL)
       {
          /* Stop timer and Deallocate timer memory */
          if( non_deferrable == TRUE )
          {
            (void)timer_undef((timer_type *)task_timer_info[queue_id][timer_id].timer);        
            modem_mem_free(task_timer_info[queue_id][timer_id].timer, MODEM_MEM_CLIENT_NAS);
          }
          else
          {
            (void) rex_clr_timer(task_timer_info[queue_id][timer_id].timer);

            rex_delete_timer_ex(task_timer_info[queue_id][timer_id].timer);
          }
    
          /* Make sure to mark this timer as inactive */
          task_timer_info[queue_id][timer_id].timer = NULL;
       }
    
       /* Copy data to param */
       param     = data;
    
       /* Copy timer id to param */
       param |= timer_id << 16;
    
       /* Allocate timer */
       if( non_deferrable == TRUE )
       {
         timer_p = (timer_type *) modem_mem_calloc( 1, sizeof( timer_type ), MODEM_MEM_CLIENT_NAS );
         if(NULL != timer_p)
         {
           timer_group_set_deferrable(&nas_timer_group_non_deferrable,FALSE);
           timer_def(timer_p, 
                     &nas_timer_group_non_deferrable,
                     rex_self(),
                     0,
                     func_ptr,
                     param);
           task_timer_info[queue_id][timer_id].timer = timer_p;
         }
         else
         {
           ERR("ERR: Memory not available to allocate for timer.",0,0,0);
           status = GS_FAILURE;
         }
       }
       else
       {
         task_timer_info[queue_id][timer_id].timer = rex_create_timer_ex(func_ptr, param);
       }
		   
       /* If memory was available */  
       if(task_timer_info[queue_id][timer_id].timer != NULL)
       {  
          /* Start Timer */
          if( non_deferrable == TRUE )
          {
          (void) timer_set(task_timer_info[queue_id][timer_id].timer,
                                            milli_secs,0,T_MSEC);
          }
          else
          {
             (void) rex_set_timer(task_timer_info[queue_id][timer_id].timer, milli_secs);
          }
    
          /* Return Success */
          status = GS_SUCCESS;
       }

       rex_leave_crit_sect(&timer_crit_sect);
    }
    else
    {
       /* Raise error condition */
       ERR("ERROR: Invalid timer, queue=%d, timer=%d",queue_id, timer_id,0);

       status = GS_FAILURE;
    }

    return status;
}

/*******************************************************************************
 *
 *  Function name: gs_clear_timer
 *  -------------------------------
 *  Description:
 *  ------------
 *
 *
 *  Parameters:
 *  -----------
 *  IN         queue_id
 *  IN         timer_id
 *
 *  Returns:
 *  --------
 *  None
 *
 ******************************************************************************/

void gs_clear_timer(gs_queue_id_T     queue_id,
                    timer_id_T        timer_id)
{
    if((queue_id <= GS_MAX_TIMER_CLIENT) && (timer_id < MAXNO_TIMERS))
    {
        rex_enter_crit_sect(&timer_crit_sect);

        if(task_timer_info[queue_id][timer_id].timer != NULL)
        {
            /* Deallocate timer */
            rex_delete_timer_ex(task_timer_info[queue_id][timer_id].timer);

            /* Make sure to mark this timer as inactive */
            task_timer_info[queue_id][timer_id].timer = NULL;
         }
         else
         {
            /* Raise error condition */
            ERR("ERROR: Attempting to stop invalid timer, queue=%d, timer=%d",queue_id, timer_id,0);
         }

         rex_leave_crit_sect(&timer_crit_sect);
    }
    else
    {
         /* Raise error condition */
         ERR("ERROR: Invalid timer, queue=%d, timer=%d",queue_id, timer_id,0);
    }
}

/*******************************************************************************
 *
 *  Function name: gs_clear_timer_ext
 *  -------------------------------
 *  Description:
 *  ------------
 *  Generic timer clear function. Takes extra parameter to specify if 
 *  the timer is non-deferrable
 *
 *  Parameters:
 *  -----------
 *  IN         queue_id
 *  IN         timer_id
 *  IN         non_deferrable
 *
 *  Returns:
 *  --------
 *  None
 *
 ******************************************************************************/

void gs_clear_timer_ext( gs_queue_id_T  queue_id,
                         timer_id_T     timer_id,
                         boolean        non_deferrable )
{
    if((queue_id <= GS_MAX_TIMER_CLIENT) && (timer_id < MAXNO_TIMERS))
    {
        rex_enter_crit_sect(&timer_crit_sect);

        if(task_timer_info[queue_id][timer_id].timer != NULL)
        {
            /* Deallocate timer */
           if( non_deferrable == TRUE )
           {
            (void)timer_undef((timer_type *)task_timer_info[queue_id][timer_id].timer); 
			 modem_mem_free(task_timer_info[queue_id][timer_id].timer, MODEM_MEM_CLIENT_NAS);
           }
           else
           {
             rex_delete_timer_ex(task_timer_info[queue_id][timer_id].timer);
           }

            /* Make sure to mark this timer as inactive */
            task_timer_info[queue_id][timer_id].timer = NULL;
         }
         else
         {
            /* Raise error condition */
            ERR("ERROR: Attempting to stop invalid timer, queue=%d, timer=%d",queue_id, timer_id,0);
         }

         rex_leave_crit_sect(&timer_crit_sect);
    }
    else
    {
         /* Raise error condition */
         ERR("ERROR: Invalid timer, queue=%d, timer=%d",queue_id, timer_id,0);
    }
}

/*******************************************************************************
 *
 *  Function name: gs_cancel_timer
 *  ------------------------------
 *  Description:
 *  ------------
 *  Cancels a timer. This function is left in for backward compatibility
 *  with existing code. It simply calls gs_modify_timer with the 
 *  appropriate parameter
 *
 *  Parameters:
 *  -----------
 *  IN         queue_id
 *  IN         timer_id
 *
 *  Returns:
 *  --------
 *  status
 *
 ******************************************************************************/

gs_status_T gs_cancel_timer(gs_queue_id_T   queue_id,
                            timer_id_T      timer_id)
{
    gs_status_T status = GS_FAILURE;

    if((queue_id <= GS_MAX_TIMER_CLIENT) && (timer_id < MAXNO_TIMERS))
    {
        rex_enter_crit_sect(&timer_crit_sect);

        /* Check to see if timer exists */
        if(task_timer_info[queue_id][timer_id].timer != NULL)
        {
           /* Stop timer */
           (void) rex_clr_timer(task_timer_info[queue_id][timer_id].timer);

           /* Deallocate timer */
           rex_delete_timer_ex(task_timer_info[queue_id][timer_id].timer);
        
           /* Make sure to mark this timer as inactive */
           task_timer_info[queue_id][timer_id].timer = NULL;
        }

        rex_leave_crit_sect(&timer_crit_sect);

       status = GS_SUCCESS;
    }
    else
    {
       /* Raise error condition */
       ERR("ERROR: Invalid timer, queue=%d, timer=%d",queue_id, timer_id,0);

       status = GS_FAILURE;
    }

    /* Return Success */
    return status;
}

/*******************************************************************************
 *
 *  Function name: gs_cancel_timer_ext
 *  ------------------------------
 *  Description:
 *  ------------
 *  Generic timer cancel function. Takes extra parameter to specify if 
 *  the timer is non-deferrable
 *
 *  Parameters:
 *  -----------
 *  IN         queue_id
 *  IN         timer_id
 *  IN         non_deferrable
 *
 *  Returns:
 *  --------
 *  status
 *
 ******************************************************************************/

gs_status_T gs_cancel_timer_ext( gs_queue_id_T   queue_id,
                                 timer_id_T      timer_id,
                                 boolean         non_deferrable )
{
    gs_status_T status = GS_FAILURE;

    if((queue_id <= GS_MAX_TIMER_CLIENT) && (timer_id < MAXNO_TIMERS))
    {
        rex_enter_crit_sect(&timer_crit_sect);

        /* Check to see if timer exists */
        if(task_timer_info[queue_id][timer_id].timer != NULL)
        {
          /* Stop timer and Deallocate timer memory */
          if( non_deferrable == TRUE )
          {
            (void)timer_undef((timer_type *)task_timer_info[queue_id][timer_id].timer); 
            modem_mem_free(task_timer_info[queue_id][timer_id].timer, MODEM_MEM_CLIENT_NAS);
          }
          else
          {
            (void) rex_clr_timer(task_timer_info[queue_id][timer_id].timer);

            rex_delete_timer_ex(task_timer_info[queue_id][timer_id].timer);
          }
        
          /* Make sure to mark this timer as inactive */
          task_timer_info[queue_id][timer_id].timer = NULL;

          status = GS_SUCCESS;
        }

        rex_leave_crit_sect(&timer_crit_sect);
    }
    else
    {
       /* Raise error condition */
       ERR("ERROR: Invalid timer, queue=%d, timer=%d",queue_id, timer_id,0);

       status = GS_FAILURE;
    }

    /* Return Success */
    return status;
}

/*******************************************************************************
 *
 *  Function name: gs_modify_timer
 *  ------------------------------
 *  Description:
 *  ------------
 *  Modifies a timer, by sending a ModifyTimer message to the timer task,
 *  with an appropriate parameter.
 *
 *  Parameters:
 *  -----------
 *  IN         queue_id
 *  IN         timer_id
 *  IN         action
 *
 *  Returns:
 *  --------
 *  status
 *
 ******************************************************************************/

gs_status_T gs_modify_timer( gs_queue_id_T         queue_id,
                             timer_id_T            timer_id,
                             timer_modify_action_T action)
{
    gs_status_T   status = GS_FAILURE;

    if((queue_id <= GS_MAX_TIMER_CLIENT) && (timer_id < MAXNO_TIMERS))
    {
        rex_enter_crit_sect(&timer_crit_sect);

        /* Check to see if timer exists */
        if(task_timer_info[queue_id][timer_id].timer != NULL)
        {
          switch(action)
          {
            case TIMER_MODIFY_PAUSE:
              rex_pause_timer(task_timer_info[queue_id][timer_id].timer);
              break;
                     
            case TIMER_MODIFY_RESUME:
              rex_resume_timer(task_timer_info[queue_id][timer_id].timer);
              break;
                     
            case TIMER_MODIFY_CANCEL:
              (void) gs_cancel_timer(queue_id, timer_id);
              break;

            case TIMER_MODIFY_ENQUIRE:
              ERR("ERR: Invalid timer action request %d",action,0,0);
              break;
                     
            default:
              ERR("ERR: Invalid timer action request %d",action,0,0);
              break;
          }
        }

       rex_leave_crit_sect(&timer_crit_sect);

       status = GS_SUCCESS;
    }     
    else
    {
       /* Raise error condition */
       ERR("ERROR: Invalid timer, queue=%d, timer=%d",queue_id, timer_id,0);

       status = GS_FAILURE;
    }

    return status;
}

/*******************************************************************************
 *
 *  Function name: gs_modify_timer_ext
 *  ------------------------------
 *  Description:
 *  ------------
 *  Generic timer modify function. Takes extra parameter to specify if 
 *  the timer is non-deferrable
 *
 *  Parameters:
 *  -----------
 *  IN         queue_id
 *  IN         timer_id
 *  IN         action
 *  IN         non_deferrable
 *
 *  Returns:
 *  --------
 *  status
 *
 ******************************************************************************/

gs_status_T gs_modify_timer_ext( gs_queue_id_T         queue_id,
                                 timer_id_T            timer_id,
                                 timer_modify_action_T action,
                                 boolean               non_deferrable )
{
    gs_status_T   status = GS_FAILURE;

    if((queue_id <= GS_MAX_TIMER_CLIENT) && (timer_id < MAXNO_TIMERS))
    {
        rex_enter_crit_sect(&timer_crit_sect);

        /* Check to see if timer exists */
        if(task_timer_info[queue_id][timer_id].timer != NULL)
        {
          switch(action)
          {
            case TIMER_MODIFY_PAUSE:
              if( non_deferrable == TRUE )
              {
              timer_pause(task_timer_info[queue_id][timer_id].timer);
              }
              else
              {
                rex_pause_timer(task_timer_info[queue_id][timer_id].timer);
              }
              break;
                     
            case TIMER_MODIFY_RESUME:
              if( non_deferrable == TRUE )
              {
              timer_resume(task_timer_info[queue_id][timer_id].timer);
              }
              else
              {
                rex_resume_timer(task_timer_info[queue_id][timer_id].timer);
              }
              break;
                     
            case TIMER_MODIFY_CANCEL:
              (void) gs_cancel_timer_ext(queue_id, timer_id, non_deferrable);
              break;

            case TIMER_MODIFY_ENQUIRE:
              ERR("ERR: Invalid timer action request %d",action,0,0);
              break;
                     
            default:
              ERR("ERR: Invalid timer action request %d",action,0,0);
              break;
          }
          status = GS_SUCCESS;
        }

        rex_leave_crit_sect(&timer_crit_sect);
    }     
    else
    {
       /* Raise error condition */
       ERR("ERROR: Invalid timer, queue=%d, timer=%d",queue_id, timer_id,0);

       status = GS_FAILURE;
    }

    return status;
}

/*******************************************************************************
 *
 *  Function name: gs_timer_exists
 *  -------------------------------
 *  Description:
 *  ------------
 *  This function checks if a timer is valid (Not NULL) based on the passed
 *  queue_id and timer_id. 
 *
 *  Parameters:
 *  -----------
 *  IN         queue_id
 *  IN         timer_id
 *
 *  Returns:
 *  --------
 *  TRUE = IF the timer is valid, hence FALSE
 *
 ******************************************************************************/
boolean gs_timer_exists
(
   gs_queue_id_T queue_id,
   timer_id_T    timer_id
)
{
    boolean timer_exists = FALSE;
      
    if((queue_id <= GS_MAX_TIMER_CLIENT) && (timer_id < MAXNO_TIMERS))
    {
        rex_enter_crit_sect(&timer_crit_sect);

        /* Check to see if timer exists */
        if (task_timer_info[queue_id][timer_id].timer != NULL)
        {
           timer_exists = TRUE;
        }   
  
        rex_leave_crit_sect(&timer_crit_sect);
    }
    else
    {
        timer_exists = FALSE;
    }

    return timer_exists;
}

/*******************************************************************************
 *
 *  Function name: gs_enquire_timer
 *  -------------------------------
 *  Description:
 *  ------------
 *  Requests details of the current state of a timer. It is recommended
 *  that the timer is paused before this call is made.
 *
 *  Parameters:
 *  -----------
 *  IN         queue_id
 *  IN         timer_id
 *
 *  Returns:
 *  --------
 *  status
 *
 ******************************************************************************/

milli_secs_T gs_enquire_timer
(
   gs_queue_id_T queue_id,
   timer_id_T    timer_id
)
{
    milli_secs_T time_remaining;

    if((queue_id <= GS_MAX_TIMER_CLIENT) && (timer_id < MAXNO_TIMERS))
    {
      
        rex_enter_crit_sect(&timer_crit_sect);

        /* Check to see if timer exists */
        if (task_timer_info[queue_id][timer_id].timer != NULL)
        {
           time_remaining = (milli_secs_T) rex_get_timer( task_timer_info[queue_id][timer_id].timer );
        }
        else
        {
           time_remaining = 0;
        }

        rex_leave_crit_sect(&timer_crit_sect);
    }
    else
    {
        time_remaining = 0;
    }  

    return time_remaining;
}

/*******************************************************************************
 *
 *  Function name: gs_enquire_timer_ext
 *  -------------------------------
 *  Description:
 *  ------------
 *  Generic timer enquire function. Takes extra parameter to specify if 
 *  the timer is non-deferrable
 *
 *  Parameters:
 *  -----------
 *  IN         queue_id
 *  IN         timer_id
 *  IN         non_deferrable
 *
 *  Returns:
 *  --------
 *  status
 *
 ******************************************************************************/

milli_secs_T gs_enquire_timer_ext
(
   gs_queue_id_T queue_id,
   timer_id_T    timer_id,
   boolean       non_deferrable
)
{
    milli_secs_T time_remaining;

    if((queue_id <= GS_MAX_TIMER_CLIENT) && (timer_id < MAXNO_TIMERS))
    {
        rex_enter_crit_sect(&timer_crit_sect);

        /* Check to see if timer exists */
        if (task_timer_info[queue_id][timer_id].timer != NULL)
        {
           if( non_deferrable == TRUE )
           {
             time_remaining = 
               (milli_secs_T) timer_get(task_timer_info[queue_id][timer_id].timer, T_MSEC);
           }
           else
           {
             time_remaining = 
               (milli_secs_T) rex_get_timer( task_timer_info[queue_id][timer_id].timer );
           }
        }
        else
        {
           time_remaining = 0;
        }

        rex_leave_crit_sect(&timer_crit_sect);
    }
    else
    {
        time_remaining = 0;
    }  

    return time_remaining;
}
