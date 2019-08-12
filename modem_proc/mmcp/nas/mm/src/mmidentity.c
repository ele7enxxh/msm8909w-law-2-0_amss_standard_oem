/*===========================================================================
            COPYRIGHT INFORMATION

Copyright (c) 2001 Qualcomm Technologies, Incorporated and its licensors.  All Rights 
Reserved.  QUALCOMM Proprietary.  Export of this technology or software 
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.

            EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/MSM5200/mm/vcs/mmidentity.c_v   1.5   01 Jul 2002 11:36:14   jault  $   
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/nas/mm/src/mmidentity.c#1 $    $DateTime: 2016/12/13 08:00:05 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
08/07/01   ts      Standardized MSG_ERROR and MSG_HIGH usage.
04/24/02   jca     Removed errant call to mm_state_control().
02/10/03   cd      Fixed Lint errors
11/21/11   gps     Removed refrence for header file "gsdi_exp.h"
===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "mmcp_variation.h"
#include "customer.h"
#include "rex.h"
#include "sys_type.h"
#include "sys_cnst_v.h"
#include "sys_stru_v.h"
#include "timers_v.h"
#include "gs_v.h"
#include "ghdi_exp.h"
#include "ghdi_exp_int.h"
#include "libprot.h"
#include "mm_v.h"
#include "mm_umts_v.h"
#include "err.h"
#ifdef FEATURE_NAS_REL11
#include "gmm_msg.h"
#include "mm_v.h"
#include "emm_database.h"
#include "mm_multimode_common.h"
#endif

#if defined(FEATURE_DUAL_SIM) && defined(FEATURE_DUAL_IMEI)
void ghdi_ds_read_imeisv(byte *imeisv,sys_modem_as_id_e_type as_id);
#endif

/*===========================================================================

FUNCTION MM_PACK_NO_IDENTITY

DESCRIPTION
  This function packs the mobile identity of type NO_IDENTITY

DEPENDENCIES
  None

RETURN VALUE
   Number of octets encoded.

SIDE EFFECTS
  None

===========================================================================*/

uint32 mm_pack_no_identity(byte *write_ptr)
{
    /* ------------------------------------------------
      ** Set Identity type field (TS 24.008 Table 10.5.4)
      ** ------------------------------------------------ */
    /* code the length for  NO_IDENTITY */
   *write_ptr = IDENTIFICATION_PROC_NO_IDENTITY_LEN;
   write_ptr++;

   /* code as NO_IDENTITY */
   *write_ptr = 0x08;

   return (IDENTIFICATION_PROC_NO_IDENTITY_LEN + 1);
}

/*===========================================================================

FUNCTION MM_RETURN_IDENTITY

DESCRIPTION
  This function return the identity as requested by the identity request
  message. This can be either the IMSI, TMSI, or IMEI. Function is called
  with a pointer to an IDENTITY_REQUEST messsage
 

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void mm_return_identity( mm_cmd_type* msg_p)
{
   identity_response_msg_T    identity_response_msg;
   byte                       *write_ptr;
   boolean                    send_msg = TRUE;
   byte                       identity_type;
   
#ifdef FEATURE_NAS_REL11
   byte                       i;
 
#ifdef FEATURE_LTE
   emm_ctrl_data_type *emm_ctrl_data_ptr = NULL;
   mm_gw_identity_param_type gw_identity;
   emm_ctrl_data_ptr = emm_db_get_ctrl_data();   
#endif
#endif


   memset(&identity_response_msg, 0x00, sizeof(identity_response_msg_T));
   identity_response_msg.msg_head.skip_pd = PD_MM;
   identity_response_msg.msg_head.type = IDENTITY_RESPONSE;

   write_ptr = identity_response_msg.mobile_identity_data;

   identity_type = ((identity_request_msg_T *)((void*)&msg_p->cmd.hdr))->identity_type & 0x07;
   
   if (identity_type == IMSI )
   {
     /*  Request is for IMSI */
     if(mm_serving_plmn.info.sim_state == SYS_SIM_STATE_NOT_AVAILABLE)
     {
       /* IMSI is not there. So copy NO_IDENTITY and advance the pointer */
       write_ptr += mm_pack_no_identity(write_ptr);     
     }
     else
     {
       /* Copy IMSI and advance the pointer */
       write_ptr += mm_pack_imsi(write_ptr);
     }
   }
   else if (identity_type == TMSI)
   {
      /*  Request is for TMSI */
     if ( !has_valid_tmsi() )
     {
       /* TMSI is not there. So copy NO_IDENTITY and advance the pointer */
       write_ptr += mm_pack_no_identity(write_ptr);          
     }
     else
     {
       /* Copy TMSI and advance the pointer */
       write_ptr += mm_pack_tmsi(write_ptr);          
     }
   }
   else if (identity_type == IMEI )
   {

      mm_read_imei( write_ptr );

      write_ptr += *write_ptr + 1;

   }
   else if (identity_type == IMEISV )
   {

#if defined(FEATURE_DUAL_SIM) && defined(FEATURE_DUAL_IMEI)
      ghdi_ds_read_imeisv(write_ptr,(sys_modem_as_id_e_type)mm_sub_id);    
#else
      read_imeisv( write_ptr );
#endif
      write_ptr += *write_ptr + 1;

   }

   /*Spec 24.008 4.3.3.2 Upon receipt of the IDENTITY REQUEST message with the Identity Type IE indicating that 
        P-TMSI, RAI and P-TMSI signature are being requested:
           
             **If the MS does not support S1 mode, it shall handle IDENTITY RESPONSE as follows::
                    If the MS holds a valid P-TMSI and RAI, the MS shall indicate the P-TMSI in the Mobile identity IE 
                    and shall indicate the RAI in the Routing area identification IE. In addition, the MS shall include the
                    P-TMSI type IE with P-TMSI type set to "native P-TMSI". If the MS holds a valid P-TMSI signature, 
                    it shall include it in the P-TMSI signature IE.  
           
            **If an MS that supports S1 mode shall handle IDENTITY RESPONSE as follows:
                   If the TIN indicates "P-TMSI" or "RAT related TMSI" and the MS holds a valid P-TMSI and RAI, 
                   the MS shall indicate the P-TMSI in the Mobile identity IE and shall indicate the RAI in the Routing area
                   identification IE. In addition, the MS shall include the P-TMSI type IE with P-TMSI type set to "native P-TMSI"
           .       If the MS holds a valid P-TMSI signature, it shall include it in the P-TMSI signature IE.
           -------------------------------------------------------------------------------------------------***/
          

#ifdef FEATURE_NAS_REL11
   else if (identity_type == PTMSI_RAI_PTMSI_SIG)
   {
     /* Request is for PTMSI, RAI and PTMSI_SIGNATURE */
      if(!(mm_checkif_rat_pri_list_has_rat(&mm_rat_pri_list_info, SYS_RAT_LTE_RADIO_ACCESS))
#ifdef FEATURE_LTE                     
            || (emm_ctrl_data_ptr->TIN == NAS_MM_TIN_P_TMSI || 
                 emm_ctrl_data_ptr->TIN == NAS_MM_TIN_RAT_RELATED_TMSI)
#endif         
         )
       {
         if( !(mm_has_valid_ptmsi() && has_valid_rai()))
         {
     /* PTMSI or RAI is not there. So copy NO_IDENTITY and advance the pointer */
           write_ptr += mm_pack_no_identity(write_ptr);
         }
         else
         {
#ifdef FEATURE_SGLTE
           mm_as_id_e_type local_as_id;
           local_as_id = mm_as_id;
           if(MM_IS_IN_SGLTE_MODE && !MM_IS_IN_SGLTE_STATE_4)
           {
             mm_as_id = mm_ps_stack_id;
           }
#endif
           write_ptr += mm_pack_ptmsi(write_ptr);
           if(mm_nv_nas_rel_compliance >= NV_NAS_COMPLIANCE_REL11)
           {
         /*If the MS holds a valid P-TMSI and RAI, the MS shall indicate the P-TMSI in the Mobile identity IE 
           and shall indicate the RAI in the Routing area identification IE. In addition, the MS shall include the
           P-TMSI type IE with P-TMSI type set to "native P-TMSI". If the MS holds a valid P-TMSI signature, 
           it shall include it in the P-TMSI signature IE.  */
          
            *write_ptr++ = 0xE0;
		    *write_ptr++ = GMM_IEI_RAI2;
			*write_ptr++ = GMM_IEI_RAI2_LENGTH;
			*write_ptr++ = gmm_stored_rai.plmn_id.identity[0];
			*write_ptr++ = gmm_stored_rai.plmn_id.identity[1];
			*write_ptr++ = gmm_stored_rai.plmn_id.identity[2];
			*write_ptr++ = gmm_stored_rai.location_area_code.lac[0];
			*write_ptr++ = gmm_stored_rai.location_area_code.lac[1];
			*write_ptr++ = gmm_stored_rai.routing_area_code;
			
            if(mm_has_valid_ptmsi_signature())
            {
              *write_ptr++ = GMM_IEI_PTMSI_SIG2;//IEI
              *write_ptr++ = GMM_IEI_PTMSI_SIG2_LENGTH; 
              *write_ptr++ = gmm_stored_ptmsi_signature.value[0];
              *write_ptr++ = gmm_stored_ptmsi_signature.value[1];
              *write_ptr++ = gmm_stored_ptmsi_signature.value[2];
            }
          }
#ifdef FEATURE_SGLTE
          mm_as_id = local_as_id;
#endif
        }
      }
#ifdef FEATURE_LTE
       else 
       {
         if((emm_ctrl_data_ptr->TIN == NAS_MM_TIN_GUTI &&
                emm_ctrl_data_ptr->emm_guti_valid == TRUE ))
         {
/*If the TIN indicates "GUTI" and the MS holds a valid GUTI, the MS shall map the GUTI into a P-TMSI, P TMSI signature and RAI as specified in
   3GPP TS 23.003 [4]. The MS shall indicate the P-TMSI in the Mobile identity IE. In addition, the MS shall include the mapped RAI in the Routing
   area identification IE and the mapped P-TMSI signature in the P-TMSI signature IE. In addition, the MS shall include the P-TMSI type IE with
   P-TMSI type set to "mapped P-TMSI". */
#ifdef FEATURE_SGLTE
           mm_as_id_e_type local_as_id;
           local_as_id = mm_as_id;
           if(MM_IS_IN_SGLTE_MODE && !MM_IS_IN_SGLTE_STATE_4)
           {
             mm_as_id = mm_ps_stack_id;
           }
#endif
   
           emm_get_mapped_gw_identity(&gw_identity, emm_ctrl_data_ptr);
           identity_response_msg.mobile_identity_data[0] = PTMSI_SIZE +1;/*ptmsi_size + type field*/
           identity_response_msg.mobile_identity_data[1] = 0xF4;/* type*/
           for(i=2; i<(PTMSI_SIZE+2); i++)/* copy mapped ptmsi to identity response mobile id*/
           {
            identity_response_msg.mobile_identity_data[i] = gw_identity.ptmsi.digit[i-2];
           }
           write_ptr += 6;
           if(mm_nv_nas_rel_compliance >= NV_NAS_COMPLIANCE_REL11)
           {
             *write_ptr++ = 0XE1;
             *write_ptr++ = GMM_IEI_RAI2;
             *write_ptr++ = GMM_IEI_RAI2_LENGTH;
             *write_ptr++ = gw_identity.ra_id.plmn_id.identity[0];
             *write_ptr++ = gw_identity.ra_id.plmn_id.identity[1];
             *write_ptr++ = gw_identity.ra_id.plmn_id.identity[2];
             *write_ptr++ = gw_identity.ra_id.location_area_code.lac[0];
             *write_ptr++ = gw_identity.ra_id.location_area_code.lac[1];
             *write_ptr++ = gw_identity.ra_id.routing_area_code;
             *write_ptr++ = GMM_IEI_PTMSI_SIG2;//IEI
             *write_ptr++ = GMM_IEI_PTMSI_SIG2_LENGTH;
             for(i=0; i<PTMSI_SIGNATURE_SIZE; i++)
             {
               *write_ptr++ = gw_identity.ptmsi_signature.value[i];
             }
           } 
#ifdef FEATURE_SGLTE
           mm_as_id = local_as_id;
#endif
         }
         else
         {
            /* PTMSI or RAI is not there. So copy NO_IDENTITY and advance the pointer */
              write_ptr += mm_pack_no_identity(write_ptr);
         }
        }
#endif
     }
#endif   
 

   else
   {

      /*
       * This should not happen as it is trapped by check for protocol errors
       */
      MSG_ERROR_DS( MM_SUB, "=MM= Unexpected identity type %d", identity_type, 0,0);
      send_msg = FALSE;

   }

   if (send_msg)
   {
      dword length;
      length = (dword)((byte *)(write_ptr) - (byte *)(&identity_response_msg))
               - sizeof( IMH_T );

      MSG_HIGH_DS_0(MM_SUB, "=MM= MM sent IDENTITY RESPONSE");
      
      mm_send_ota_message((byte*)&identity_response_msg, length);

   }
}
