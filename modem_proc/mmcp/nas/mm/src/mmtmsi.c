/*===========================================================================
            COPYRIGHT INFORMATION

Copyright (c) 2001 Qualcomm Technologies, Incorporated and its licensors.  All Rights 
Reserved.  QUALCOMM Proprietary.  Export of this technology or software 
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.

            EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/MSM5200/mm/vcs/mmtmsi.c_v   1.7   24 Apr 2002 15:06:50   jault  $   
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/nas/mm/src/mmtmsi.c#1 $    $DateTime: 2016/12/13 08:00:05 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/14/01   jca     Added mm_ prefix to MM/GMM shared variables.
05/15/01   sbk     Added error checking for the valid mobile identity type
06/20/01   sbk     Use a function to write LOCI
11/02/01   sbk     Rearranged the ordering of writing TMSI to SIM and sending reallocation
                   complete msg to network. The order should be maintined for consistency
12/06/01   jca     Now sending UE_DYNAMIC_ID log packet when TMSI value changes.
04/24/02   jca     Removed errant call to mm_state_control().
02/10/03   cd      Fixed Lint errors
11/21/11   gps     Removed refrence for header file "gsdi_exp.h"
===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include <stringl/stringl.h>
#include "mmcp_variation.h"
#include "customer.h"
#include "rex.h"
#include "sys_type.h"
#include "sys_cnst_v.h"
#include "sys_stru_v.h"
#include "timers_v.h"
#include "gs_v.h"
#include "mm_v.h"
#include "err.h"
#include "mm_sim.h"
#include "naslog_v.h"
#include "mm_coord.h"
#include "sys_eplmn_list.h"
#include "sys_eplmn_list_v.h"

#if defined FEATURE_DUAL_SIM || defined FEATURE_SGLTE
 #define sys_eplmn_list_equivalent_plmn(a) sys_eplmn_per_nas_stacks_list_equivalent_plmn(a,(sys_modem_as_id_e_type)mm_sub_id,(sys_modem_as_id_e_type)mm_as_id)
#endif


/*===========================================================================

FUNCTION MM_PERFORM_TMSI_REALLOCATION

DESCRIPTION
  This function reallocates the TMSI on instruction from the network.
  Note: The spec (04.08 4.3.1.4) details what to do in the event of
  RR failure during TMSI reallocation, however this affets the Network
  side only.


DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void mm_perform_tmsi_reallocation(mm_cmd_type* msg_p)
{
   sys_plmn_id_s_type tmsi_realloc_plmn;
   tmsi_reallocation_command_msg_T  *tmsi_reallocation_command_msg
      = (tmsi_reallocation_command_msg_T *)((void *)&msg_p->cmd.hdr);

   if ((tmsi_reallocation_command_msg->mobile_identity_data[1] & (byte)(0x07))
                                                               == (byte)(IMSI) )
   {

      /*
       * check if imsi matches
       */

      if( memcmp( &mm_stored_imsi,
                  tmsi_reallocation_command_msg->mobile_identity_data,
                  mm_stored_imsi.length ) == 0 )
      {
         (void)memset( mm_location_information.tmsi, 0xff, TMSI_SIZE );

         /* --------------------------------------------
         ** Send the log packet which updates TMSI value
         ** -------------------------------------------- */
         mmsend_nas_ue_dynamic_id_log_packet( LOG_TMSI_TYPE );

      }
      else
      {
         MSG_ERROR_DS(MM_SUB, "=MM= Non matching IMSI", 0,0,0);
         return;
      }
   }
   else if ((tmsi_reallocation_command_msg->mobile_identity_data[1] & (byte)(0x07))
                                                               == (byte)(TMSI) )
   {
      /*
       * copy message.tmsi to TMSI_allocation
       */

      (void)memscpy(mm_location_information.tmsi,TMSI_SIZE,
              tmsi_reallocation_command_msg->mobile_identity_data + 2,
              TMSI_SIZE );

      /* --------------------------------------------
      ** Send the log packet which updates TMSI value
      ** -------------------------------------------- */
      mmsend_nas_ue_dynamic_id_log_packet( LOG_TMSI_TYPE );
   }
   else
   {
      MSG_ERROR_DS(MM_SUB, "=MM= Unexpected identity type %d",
          tmsi_reallocation_command_msg->mobile_identity_data[1] & 0x07, 0,0);
      return;
   }
    tmsi_realloc_plmn.identity[0] = tmsi_reallocation_command_msg->location_area_identification_data[0];
    tmsi_realloc_plmn.identity[1] = tmsi_reallocation_command_msg->location_area_identification_data[1];
    tmsi_realloc_plmn.identity[2] = tmsi_reallocation_command_msg->location_area_identification_data[2];

	if((mm_reg_waiting_for_reg_cnf) && 
        ((mmcoord_pending_reg_message.cmd.hdr.message_id == (int)MMR_REG_REQ) &&
		(mmcoord_pending_reg_message.cmd.mmr_reg_req.network_selection_mode == SYS_NETWORK_SELECTION_MODE_AUTOMATIC ||
         mmcoord_pending_reg_message.cmd.mmr_reg_req.network_selection_mode == SYS_NETWORK_SELECTION_MODE_MANUAL)) &&
	   !((sys_plmn_match(mm_serving_plmn.info.plmn , tmsi_realloc_plmn )) ||
	     (sys_eplmn_list_equivalent_plmn(tmsi_realloc_plmn))))
    {
        MSG_HIGH_DS_3(MM_SUB, "=MM= Invalid PLMN ID received during tmsi reallocation MCC/MNC %02x %02x %02x ",
                                 tmsi_realloc_plmn.identity[0],tmsi_realloc_plmn.identity[1],tmsi_realloc_plmn.identity[2]);
    }
	else
	{
      (void)memscpy( mm_location_information.lai,LAI_SIZE,
             tmsi_reallocation_command_msg->location_area_identification_data,
             LAI_SIZE );

      if ((memcmp( mm_serving_lai,
                     mm_location_information.lai,
                     LAI_SIZE ) != 0 ) &&
           mm_is_cs_connected_due_to_srvcc)
      {
        if(mm_pending_lu_request == LTE_IRAT_LU)
        {
          MSG_HIGH_DS_0(MM_SUB, "=MM= since IRAT LU is pended, not making any change in mm_pending_lu_request\n");
        }
        else
        {
          mm_pending_lu_request = LU_PENDED_TMSI_REALLOCATION;
        }
      }
      else
      {
             /*------------------------------------------------------------
              ** This is very rare scenario. UE moves from old LAI to new LAI in connected mode.
              ** New LAI inforamtion will not be avaiable to NAS (CN info is absent in DCH state).
              ** N/W initiates TMSI reallocation in new LAI and N/W is already updated about
              ** the new location of this UE. So UE/NAS should update the stored LAI as new LAI.
              ** Consider UE as updated in new LAI and serving/camped on new LAI.
              ** So Update currently camped PLMN/LA with the PLMN/LA Id received
              ** in TMSI reallocation message.
              **-------------------------------------------------------------*/
   
         mm_serving_plmn.info.plmn.identity[0] = mm_location_information.lai[0];
         mm_serving_plmn.info.plmn.identity[1] = mm_location_information.lai[1];
         mm_serving_plmn.info.plmn.identity[2] = mm_location_information.lai[2];
         mm_serving_plmn.lac.lac[0] = mm_location_information.lai[3];
         mm_serving_plmn.lac.lac[1] = mm_location_information.lai[4];

        (void) memscpy(mm_serving_lai,LAI_SIZE, mm_location_information.lai, LAI_SIZE);
      }
    }

   /*
    * Copy location information to SIM.
    */
   mmsim_write_loci();

  /*
   * send TMSI_REALLOCATION_COMPLETE message
   */
   mm_send_tmsi_reallocation_complete();

#ifdef FEATURE_DUAL_SIM
   if(MM_MODEM_STATISTICS_SUB1)
#endif
   {
     mm_set_signal_for_cm(SYS_NAS_REGISTRATION_INFO_MASK);
   }

}
