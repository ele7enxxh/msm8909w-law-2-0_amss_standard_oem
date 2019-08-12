/*===========================================================================
            COPYRIGHT INFORMATION

Copyright (c) 2001,2002 Qualcomm Technologies, Incorporated and its licensors.  All Rights
Reserved.  QUALCOMM Proprietary.  Export of this technology or software
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.

            EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/nas/rabm/src/rabmtimers.c#1 $    $DateTime: 2016/12/13 08:00:05 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
04/15/04   kvk     fixed the timer related issues by assigning the MS_TIMER to 
                   the correct structure member in rabm_cmd_type in rabmm_timer_expiry() 
                   function. The correct message_id member is been taken in 
                   rabm_process_timer_expiry() from rabm cmd_ptr
08/16/03   mks     Initial version to support RABM timers.
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
#include "rabmtask_v.h"

/* =========================================================================
** LOCAL Data Declarations
** ======================================================================= */

#ifdef FEATURE_DUAL_DATA
    LOCAL byte rabm_timer_status_sim[MAX_AS_IDS][MAX_RAB_NUM];    /* RABM Timer Status List */
    #define rabm_timer_status rabm_timer_status_sim[sm_as_id]
#else
LOCAL byte rabm_timer_status[MAX_RAB_NUM];    /* RABM Timer Status List */
#endif /*FEATURE_DUAL_DATA*/

/*===========================================================================

 FUNCTION RABM_INITIALIZE_TIMERS

 DESCRIPTION
   This function initialized RABM timer status list

 DEPENDENCIES
   None

 RETURN VALUE
   None

 SIDE EFFECTS
   None

===========================================================================*/
void rabm_initialize_timers ( void )
{
  unsigned int index = 0;

#ifdef FEATURE_DUAL_DATA
   unsigned int as_index = 0;
   for ( as_index = 0; as_index < MAX_AS_IDS; as_index ++ )
   {
     sm_as_id = as_index;
#endif
     for ( index = 0; index < MAX_RAB_NUM; index ++ )
     {
       rabm_timer_status[index] = (byte)RABM_TIMER_STOPPED;

     }
#ifdef FEATURE_DUAL_DATA
   }
#endif
}


/*===========================================================================

FUNCTION RABM_START_TIMER

DESCRIPTION
  This function starts the RABM timer specified by timer_id

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void rabm_start_timer( word timer_id )
{
  gs_status_T    status;
  word num_millisecs = (word) TIME_RAB_EST_PENDING;

  if ( timer_id < (word) MAX_RAB_NUM )
  {
     #ifdef FEATURE_DUAL_DATA
       status = gs_start_timer( GS_QUEUE_SM, (byte) ( (timer_id&0x0f)| ((0x01 & sm_as_id)<<4)), 
                             rabm_timer_expiry, (dword)TIME_RAB_EST_PENDING, (byte)0 );
     #else
           status = gs_start_timer( GS_QUEUE_SM, (byte) (timer_id&0x0f), 
                             rabm_timer_expiry, (dword)TIME_RAB_EST_PENDING, (byte)0 );
     #endif
 
    
    rabm_timer_status[ timer_id ] = (byte)RABM_TIMER_ACTIVE;

    MSG_HIGH_DS_3( RABM_SUB,"RABM Started %d, timeout %u:%u",
            timer_id, (word) (num_millisecs /1000L), (word)(num_millisecs % 1000L) );

    if ( status != GS_SUCCESS )
    {
      MSG_ERROR_DS_1(RABM_SUB,"RABM timer start failure: Status %d", status);
    }
  }
  else
  {
    MSG_ERROR_DS_1(RABM_SUB,"RABM timer startup failure, invalid timer ID = %u", timer_id);
  }
}


/*===========================================================================

FUNCTION RABM_STOP_TIMER

DESCRIPTION
  This function stops the sm timer specified by timer_id


DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void rabm_stop_timer( word timer_id )
{
   gs_status_T gs_status;

   if ( timer_id >= (word) MAX_RAB_NUM )
   {
     MSG_ERROR_DS_1(RABM_SUB,"RABM timer stop failure, invalid timer ID = %u", timer_id);
   }
   else if (rabm_timer_status[timer_id] != (byte)RABM_TIMER_STOPPED)
   {
      MSG_HIGH_DS_1( RABM_SUB,"RABM stopping %d", timer_id);

     #ifdef FEATURE_DUAL_DATA
       gs_status = gs_cancel_timer( GS_QUEUE_SM, (timer_id_T) ( (timer_id&0x0f)| ((0x01 & sm_as_id)<<4)) );
     #else
      gs_status = gs_cancel_timer( GS_QUEUE_SM, (timer_id_T) timer_id );
     #endif

      if (gs_status != GS_SUCCESS)
      {
         MSG_ERROR_DS_0(RABM_SUB,"RABM timer stop failure");
      }

      rabm_timer_status[timer_id] = (byte)RABM_TIMER_STOPPED;
   }
}


/*===========================================================================

FUNCTION RABM_TIMER_EXPIRY

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
void rabm_timer_expiry(unsigned long param)
{
   rabm_cmd_type   *cmd_ptr;

   /* Get command buffer from reg task */
   cmd_ptr = rabm_get_cmd_buf();

   if (cmd_ptr == NULL)
   {
      MSG_FATAL_DS( RABM_SUB,"ERROR: Unable to allocate memory", 0,0,0 );
   }
    /* Construct message */
   cmd_ptr->header.message_set = MS_TIMER;
   cmd_ptr->cmd.timer_expiry.IMH.message_id  = MID_TIMER_EXPIRY;

   PUT_IMH_LEN (sizeof (cmd_ptr->cmd.timer_expiry) - sizeof (cmd_ptr->cmd.timer_expiry.IMH),
                     &(cmd_ptr->cmd.timer_expiry.IMH));

   cmd_ptr->cmd.timer_expiry.data        = (byte)(param & 0x0000FFFF);
   cmd_ptr->cmd.timer_expiry.timer_id    = (byte)(param >> 16);

   /* Put on RABM command queue */
   rabm_put_cmd(cmd_ptr);
}
#ifdef FEATURE_NON_DEMAND_PAGED_FUNCTION
#error code not present
#endif


/*===========================================================================

FUNCTION RABM_PROCESS_TIMER_EXPIRY

DESCRIPTION

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void rabm_process_timer_expiry_cmd ( rabm_cmd_type *cmd_ptr )
{
  if (cmd_ptr->cmd.timer_expiry.IMH.message_id == MID_TIMER_EXPIRY)
  {
#if defined(FEATURE_DUAL_SIM)
       cmd_ptr->cmd.timer_expiry.timer_id = cmd_ptr->cmd.timer_expiry.timer_id & 0x0f;
#endif

    /* Clear timer information */
    gs_clear_timer(GS_QUEUE_SM, cmd_ptr->cmd.timer_expiry.timer_id);

    MSG_HIGH_DS_1(RABM_SUB,"RABM Timer %d expired", cmd_ptr->cmd.timer_expiry.timer_id);    
    
    if (cmd_ptr->cmd.timer_expiry.timer_id < MAX_RAB_NUM)
    {
      rabm_timer_status[cmd_ptr->cmd.timer_expiry.timer_id] = (byte)RABM_TIMER_STOPPED;
      rabm_rab_est_timer_expired ( (word) cmd_ptr->cmd.timer_expiry.timer_id );
    }
  }
  else
  {
    MSG_ERROR_DS_1(RABM_SUB,"Unexpected message id %d for timer message", cmd_ptr->header.cmd_id);
  }
}
