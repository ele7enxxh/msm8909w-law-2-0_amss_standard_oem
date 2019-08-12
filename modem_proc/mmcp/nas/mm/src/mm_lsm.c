/*===========================================================================
            COPYRIGHT INFORMATION

Copyright (c) 2004 Qualcomm Technologies, Incorporated and its licensors.  All Rights
Reserved.  QUALCOMM Proprietary.  Export of this technology or software
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.

            EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/nas/mm/src/mm_lsm.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
06/08/04   cd      LCS support.
                   Fixed Lint error.
08/12/04   cd      Replaced FEATURE_LCS by FEATURE_MMGPS
08/31/04   cd      Replaced queueing mechanism
03/13/06   abe     Added mm_process_lcs_ue_pos_stored_info() for routing  
                   UE POSITION STORED INFO to LSM
===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "mmcp_variation.h"
#include "mm_v.h"
#include "msg.h"
#include "err.h"
#include <stringl/stringl.h>



#if defined ( FEATURE_MMGPS ) || defined ( FEATURE_CGPS_UMTS_CP )

#ifdef FEATURE_MMGPS
#include "lsmi_cmd.h"
#else

#ifdef FEATURE_CGPS_UMTS_CP
#include "mm_lsm.h"

static mm_cgps_event_cb_type   mm_cgps_event_cb = NULL;
#endif /*end of FEATURE_CGPS_UMTS_CP */

#endif /* end of FEATURE_MMGPS */


/*===========================================================================

FUNCTION MM_ABORT_LCS

DESCRIPTION
  This function requests an abort of a current LCS session to LSM

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void mm_abort_lcs(void)
{
#ifdef FEATURE_MMGPS

   lsm_cmd_type *lsm_cmd;

   if ( ( lsm_cmd = lsm_get_free_cmd() ) != 0 )
   {
     lsm_cmd->cmd_hdr.cmd_type = LSM_CMD_LCS_ABORT_IND;
     lsm_cmd_q_put( lsm_cmd );
     MSG_HIGH_DS_0(MM_SUB, "=MM= MM sent LCS_ABORT_IND");
   }
   else
     MSG_ERROR_DS(MM_SUB, "=MM= Unable to obtain free LSM CMD",0,0,0);



#else

#ifdef FEATURE_CGPS_UMTS_CP

   mm_cgps_event_data_s_type    event_data;

   event_data.e_event_type = MM_CGPS_MSG_EVENT_ABORT;
   event_data.u.z_mm_cgps_msg_data.length = 0;  /* no data involved */
   event_data.u.z_mm_cgps_msg_data.p_data = NULL;
  
   if(mm_cgps_event_cb != NULL)
   {
     MSG_HIGH_DS_0( MM_SUB, "=MM= MM sent MM_CGPS_MSG_EVENT_RESET_UE_POS_INFO to LSM");	    	   
     (void) mm_cgps_event_cb(&event_data);
   }
   else
   {
     MSG_ERROR_DS( MM_SUB, "=MM= mm_cgps_event_cb event not initialized by LCS", 0,0,0 );
   }

#endif  /* end of  FEATURE_CGPS_UMTS_CP */

#endif /* end of  FEATURE_MMGPS */
   mm_lcs_active = FALSE;


}

/*===========================================================================

FUNCTION MM_PROCESS_LCS_SESSION_END

DESCRIPTION
  This function processes the indication that an LCS session has finished.
  Such an indication can be a LCS_ACTIVE_IND with lcs_in_progress = FALSE, or 
  MM sending a LCS_ABORT_IND

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void mm_process_lcs_session_end(void)
{

   mm_lcs_active = FALSE;

   if (mm_state == MM_RR_CONNECTION_RELEASE_NOT_ALLOWED)
   {
      mm_state_control(MM_STD_LCS_FINISHED_RR_CONN_ACTIVE);
   }
   else if ((mm_state == MM_CONNECTION_ACTIVE) ||
            (mm_state == MM_LOCATION_UPDATE_INITIATED) ||
            (mm_state == MM_WAIT_FOR_ADDITIONAL_OUTGOING_MM_CONNECTION) ||
            (mm_state == MM_WAIT_FOR_NETWORK_COMMAND))
   {
      mm_state_control(MM_STD_LCS_FINISHED);
   }
   else
   {
      MSG_HIGH_DS_1(MM_SUB ,"=MM= LCS session ended in unexpected state %d",mm_state);
   }
   mm_stop_timer(TIMER_T3241);

}


/*===========================================================================

FUNCTION MM_PROCESS_LCS_UE_POS_STORED_INFO

DESCRIPTION
  This function routes the UE POSITIONING INFORMATION to LSM

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void mm_process_lcs_ue_pos_stored_info(mm_cmd_type *msg_p)
{

#ifdef FEATURE_MMGPS

   lsm_cmd_type *lsm_cmd;

   if ( ( lsm_cmd = lsm_get_free_cmd() ) != 0 )
   {
     lsm_cmd->cmd_hdr.cmd_type = LSM_CMD_LCS_RESET_UE_POS_STORED_INFO;
     memscpy((void *)lsm_cmd->cmd_msg.cmd_ue_postioning_info.data, UE_POSITION_INFO,(void *)msg_p->cmd.rr_data_ind.layer3_message,UE_POSITION_INFO);
     lsm_cmd_q_put( lsm_cmd );
     MSG_HIGH_DS_0(MM_SUB, "=MM= MM sent LCS_RESET_UE_POS_STORED_INFO to LSM");
   }
   else
     MSG_ERROR_DS(MM_SUB, "=MM= Unable to obtain free LSM CMD",0,0,0);

#else

#ifdef FEATURE_CGPS_UMTS_CP

    mm_cgps_event_data_s_type    event_data;

    event_data.e_event_type = MM_CGPS_MSG_EVENT_RESET_UE_POS_INFO;
    event_data.u.z_mm_cgps_msg_data.length = 0;  /* no data involved */
    event_data.u.z_mm_cgps_msg_data.p_data = NULL;

    if(mm_cgps_event_cb != NULL)
    {
      (void) mm_cgps_event_cb(&event_data);
    }
    else
    {
      MSG_ERROR_DS( MM_SUB, "=MM= mm_cgps_event_cb event not initialized by LCS", 0,0,0 );
    }

#endif  /* end of FEATURE_CGPS_UMTS_CP */

#endif  /* end of FEATURE_MMGPS */

}


#ifdef FEATURE_CGPS_UMTS_CP


/*===========================================================================

FUNCTION  mm_register_cgps_event_cb

DESCRIPTION
  This function is called by CGPS to install a callback which will be used
  by MM to provide events and/or data back to CGPS

DEPENDENCIES None

RETURN VALUE : TRUE/FALSE

SIDE EFFECTS None

===========================================================================*/

boolean mm_register_cgps_event_cb( mm_cgps_event_cb_type event_cb_fp )
{
  if ( event_cb_fp == NULL )
    return FALSE;

  mm_cgps_event_cb = event_cb_fp;
  return TRUE;
}

#endif /*  end of FEATURE_CGPS_UMTS_CP */


#endif /* FEATURE_MMGPS || FEATURE_CGPS_UMTS_CP */

