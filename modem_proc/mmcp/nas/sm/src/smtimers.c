/*===========================================================================
            COPYRIGHT INFORMATION

Copyright (c) 2001,2002 Qualcomm Technologies, Incorporated and its licensors.  All Rights
Reserved.  QUALCOMM Proprietary.  Export of this technology or software
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.

            EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/nas/sm/src/smtimers.c#1 $    $DateTime: 2016/12/13 08:00:05 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
07/17/06   ss       Delaying Local deactivation for 2 secs to avoid RAB setup failure.
03/28/05   kvk     Now storing the timer type information in sm_timer_status variable
                   rather then just storing SM_TIMER_ACTIVE.
04/13/04   kvk     fixed the timer related issues by assigning the MS_TIMER to 
                   the correct structure member in sm_cmd_ptr in sm_timer_expiry() 
                   function. The correct message_id member is been taken in                    sm_process_timer_expiry() from sm_cmd_ptr
11/22/03   mks     Initial version to support session management timers.
===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "mmcp_variation.h"
#include "customer.h"
#include "rex.h"
#include "err.h"
#include "msg.h"
#include "sys_type.h"
#include "sys_cnst_v.h"
#include "sys_stru_v.h"
#include "timers_v.h"
#include "gs_v.h"
#include "sm_v.h"
#include "smtimers.h"

/* =========================================================================
** LOCAL Data Declarations
** ======================================================================= */



#ifdef FEATURE_DUAL_DATA
    LOCAL word sm_timer_status_sim[MAX_AS_IDS][MAX_PDP_NUM];    /* RABM Timer Status List */
    #define sm_timer_status sm_timer_status_sim[sm_as_id]
#else
    LOCAL word sm_timer_status[MAX_PDP_NUM];    /* SM Timer Status List */
#endif /*FEATURE_DUAL_DATA*/

/*===========================================================================

 FUNCTION SM_INITIALIZE_TIMERS

 DESCRIPTION
   This function initialized SM timer status list

 DEPENDENCIES
   None

 RETURN VALUE
   None

 SIDE EFFECTS
   None

===========================================================================*/
void sm_initialize_timers ( void )
{
  unsigned int index = 0;

#ifdef FEATURE_DUAL_DATA
  unsigned int as_index = 0;
  for ( as_index = 0; as_index < MAX_AS_IDS; as_index ++ )
  {
     sm_as_id = as_index;
#endif
    for ( index = 0; index < MAX_PDP_NUM; index ++ )
    {
      sm_timer_status[index] = (word)SM_TIMER_STOPPED;
    }
#ifdef FEATURE_DUAL_DATA
  }
#endif
}


/*===========================================================================

FUNCTION SM_START_TIMER

DESCRIPTION
  This function starts the SM timer specified by timer_id

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void sm_start_timer( word timer_id, sm_timer_T sm_timer_type )
{
  gs_status_T    status;
  word           num_millisecs = 0;
  boolean        valid_timer = TRUE;

  switch ( sm_timer_type )
  {
    case TIMER_ACTIVE_PENDING:
      num_millisecs = ACTIVE_PENDING_TIME;
      break;
  
    case TIMER_MODIFY_PENDING:
      num_millisecs = MODIFY_PENDING_TIME;
      break;
  
    case TIMER_INACTIVE_PENDING:
      num_millisecs = INACTIVE_PENDING_TIME;
      break;

    case TIMER_RABM_RESP_PENDING:
      num_millisecs = RABM_PENDING_RESP_TIME;
      break;

    case TIMER_ESTABLISH_PENDING:
      num_millisecs = ESTABLISH_PENDING_TIME;
      break;

    case TIMER_EARLY_RAB_FAILURE:
      num_millisecs = EARLY_RAB_FAILURE_TIME;
      break;

    default:
      valid_timer = FALSE;
      break;
  }
   
  if ( valid_timer )
  {
   #ifdef FEATURE_DUAL_DATA
       status = gs_start_timer( GS_QUEUE_SM, (byte) ( (timer_id&0x0f)| ((0x01 & sm_as_id)<<4)), sm_timer_expiry, num_millisecs, (byte) sm_timer_type );
   #else
    status = gs_start_timer( GS_QUEUE_SM, (byte) (timer_id&0x0f), sm_timer_expiry, num_millisecs, (byte) sm_timer_type );
   #endif
    sm_timer_status[ timer_id ] = (word)sm_timer_type;

    MSG_HIGH_DS_3(SM_SUB, "SM Started %d, timeout %u:%u",
            pdp_list[timer_id]->nsapi.nsapi, (word) (num_millisecs /1000L), (word)(num_millisecs % 1000L) );

    if ( status != GS_SUCCESS )
    {
      MSG_ERROR_DS_1(SM_SUB,"SM timer start failure: Status %d", status);
    }
  }
  else
  {
    MSG_ERROR_DS_1(SM_SUB,"SM timer start failure: Type %d", sm_timer_type);
  }
}


/*===========================================================================

FUNCTION SM_STOP_TIMER

DESCRIPTION
  This function stops the sm timer specified by timer_id


DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void sm_stop_timer( word timer_id )
{
   gs_status_T gs_status;

   if (timer_id >= (word) MAX_PDP_NUM)
   {
      MSG_ERROR_DS_1(SM_SUB,"SM timer stop failure: invalid timer ID = %u", timer_id);
   }
   else if (sm_timer_status[timer_id] == (word) TIMER_EARLY_RAB_FAILURE)
   {
     MSG_HIGH_DS_0(SM_SUB, "Not Stipping TIMER_RABM_RESP_PENDING");
   }
   else if (sm_timer_status[timer_id] != (word)SM_TIMER_STOPPED)
   {
      MSG_HIGH_DS_1(SM_SUB, "SM stopping %u", timer_id);

   #if defined(FEATURE_DUAL_DATA)
       gs_status = gs_cancel_timer( GS_QUEUE_SM, (timer_id_T) ( (timer_id&0x0f)| ((0x01 & sm_as_id)<<4)) );
       /* do no reset sm_max_timer_count, we would use max_est_timer_count*/
       if (sm_timer_status[timer_id] != (word)TIMER_ESTABLISH_PENDING)
       {
         sm_max_timer_count[timer_id] = 0;
       }
   #else
      gs_status = gs_cancel_timer( GS_QUEUE_SM, (timer_id_T) timer_id );
      //Reset the sm_max_timer_count to zero
      sm_max_timer_count[timer_id] = 0;
   #endif
   
      if (gs_status != GS_SUCCESS)
      {
         MSG_ERROR_DS_1(SM_SUB,"SM timer %u stop failure", timer_id);
      }

      sm_timer_status[timer_id] = (word)SM_TIMER_STOPPED;
   }
}


/*===========================================================================

FUNCTION SM_TIMER_EXPIRY

DESCRIPTION

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
#ifdef FEATURE_NON_DEMAND_PAGED_FUNCTION
#error code not present
#endif

void sm_timer_expiry(unsigned long param)
{
   sm_cmd_type   *cmd_ptr;

   /* Get command buffer from reg task */
   cmd_ptr = sm_get_cmd_buf();

   if (cmd_ptr == NULL)
   {
      MSG_FATAL_DS(SM_SUB,  "ERROR: Unable to allocate memory", 0,0,0 );
   }
    /* Construct message */
   cmd_ptr->header.message_set = MS_TIMER;
   cmd_ptr->cmd.timer_expiry.IMH.message_id  = MID_TIMER_EXPIRY;

   PUT_IMH_LEN (sizeof (cmd_ptr->cmd.timer_expiry) - sizeof (cmd_ptr->cmd.timer_expiry.IMH),
                     &(cmd_ptr->cmd.timer_expiry.IMH));

   cmd_ptr->cmd.timer_expiry.data        = (byte)(param & 0x0000FFFF);
   cmd_ptr->cmd.timer_expiry.timer_id    = (byte)(param >> 16);

   /* Put on SM command queue */
   sm_put_cmd(cmd_ptr);
}
#ifdef FEATURE_NON_DEMAND_PAGED_FUNCTION
#error code not present
#endif


/*===========================================================================

FUNCTION SM_PROCESS_TIMER_EXPIRY

DESCRIPTION

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void sm_process_timer_expiry_cmd ( sm_cmd_type *cmd_ptr )
{
  if (cmd_ptr->cmd.timer_expiry.IMH.message_id == MID_TIMER_EXPIRY)
  { 
    
#ifdef FEATURE_DUAL_DATA
     cmd_ptr->cmd.timer_expiry.timer_id = cmd_ptr->cmd.timer_expiry.timer_id & 0x0f;
#endif
    MSG_HIGH_DS_1(SM_SUB,"SM Timer %u expired", cmd_ptr->cmd.timer_expiry.timer_id); 

    /* Clear timer information */
    gs_clear_timer(GS_QUEUE_SM, cmd_ptr->cmd.timer_expiry.timer_id);
    
    if (cmd_ptr->cmd.timer_expiry.timer_id < MAX_PDP_NUM)
    {
      sm_timer_status[cmd_ptr->cmd.timer_expiry.timer_id] = (word)SM_TIMER_STOPPED;

      sm_proc_timer_expired (cmd_ptr->cmd.timer_expiry.timer_id, 
                             (sm_timer_T) cmd_ptr->cmd.timer_expiry.data);
    }
    else
    {
      MSG_ERROR_DS_1(SM_SUB,"Invalid SM timer expired = %u", cmd_ptr->cmd.timer_expiry.timer_id);
    }
  }
  else
  {
    MSG_ERROR_DS_1(SM_SUB,"Unexpected message id %d for timer message", cmd_ptr->header.cmd_id);
  }
}
