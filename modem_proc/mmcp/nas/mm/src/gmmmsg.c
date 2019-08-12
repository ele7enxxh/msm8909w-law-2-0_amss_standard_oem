/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                     GMM Message Parsing (GMMMSG.C)

GENERAL DESCRIPTION

EXTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright (c) 2011 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/MSM5200/mm/vcs/gmmmsg.c_v   1.22   06 Jun 2002 14:24:54   kishores  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/nas/mm/src/gmmmsg.c#1 $ $DateTime: 2016/12/13 08:00:05 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
09/30/13   rajesh  Made code changes to exclude PTMSI type IE field in attach and RAU
                   request if NV item "exclude_identity_type_ie" is set.
01/31/11   rajesh  voice domain preference and UE usage setting related changes
04/13/10   MNK     Minor name changes 
01/26/01   jca     Created module
05/23/01   jca     Added support for GMM Release 1 messages.
06/18/01   jca     Now setting correct message type for SERVICE REQUEST message.
08/03/01   jca     Now setting correct message length for ATTACH REQUEST message.
09/12/01   jca     Now setting correct message length for RAU REQUEST message.
09/18/01   sbk     Implemented functions to fill MS Network capability and
                   MS Radio access capabilities IEs
09/21/01   sbk     CSN_BIT_SET macro is reworked
02/20/02   jca     Now setting correct bit to indicate power off in DETACH REQUEST.
02/20/02   sbk     CSN encoding code is rewritten to use generic encoding routines
02/26/02   jca     Now setting msg_index correctly for contents of RAND IEI.
02/26/02   vdr     Added handling of repeated EIs for Authentication and Ciphering
                   Request message
04/08/02   ps      Workaround for PTMSI.UE always sends IMSI in GPRS attach requests
04/15/02   vdr     Now setting correct parsing of bit fields for Authentication &
                   Ciphering Request message
04/24/02   ks      Setting the correct CKSN from the SIM during Service_Request
                   procedure.
04/30/02   ks      Modified input parameter in gmm_send_identity_response_message
                   to gmm_ie_LV_resp_mobile_id_T from gmm_ie_LV_mobile_id_T.
05/02/02   vdr     Now CKSN is set correctly for GMM Service Request message
05/15/02   ks      Removed FEATURE_PTMSI_WORK_AROUND as GMM Identity procedure
                   is supported.
06/04/02   ks      Fixed the NAS message length in identity response message.
07/20/02   vdr     The functions get_ms_network_capability() and
                   get_ms_radio_access_capability() have been modified in order to
                   support FEATURE_GSM and FEATURE_GSM_MULTIBAND compilation flags
08/26/02   JC      Renamed MULTIBAND_SUPPORT to FEATURE_GSM_MULTIBAND.
09/04/02   vdr     Now CKSN field of the GMM Service Request message is set correctly
                   according to the current security context
09/06/02   vdr     Set up MS RAT and Network Capabilities fields differently
                   for different revision levels according to the Revision Level
                   indicator (R97, R99)
10/07/02   jca     Modified processing of MS Identity field in Attach Accept
                   and Routing Area Update Accept message functions.
10/09/02   vdr     Changed the interfaces of the following functions:
                   o gmm_send_attach_request_message()
                   o gmm_send_routing_area_update_request_message()
10/10/02   vdr     Changed the following messages in order to be backward compatible
                   to R'97:
                   o GPRS ATTACH REQUEST
                   o GPRS DETACH REQUEST
                   o AUTHENTICATION AND CIPHERING REQUEST
                   o ROUTING AREA UPDATE REQUEST
10/15/02   vdr     Fixed some LINT errors
10/21/02   mks     Merged the fixes to get_ms_radio_access_capability() from
                   the branch
11/14/02   ks      Added support for SGSNR for GPRS revision info. Fixed the
                   MS Network capability and Radio Access capability IEs for
                   R97 and R99 networks.
12/02/02   mks     Merged the fixes to get_ms_radio_access_capability() for
                   building MS radio access capability IE for use with a GMM
                   OTA message or with GPRS RLC/MAC packet resource request
                   message.
12/13/02   vdr     Fixed Authentication and Cipering Response message
12/13/02   ks      Fixed encoding of Multi-class capability IE, setting the
                   SMS_VALUE to 8/4 TS & SM_VALUE to 2/4 TS
01/09/02   ks      If Multiband capability is present set the Further MS Radio
                   Access Capability IE. Also changed GPRS multiclass to 4.
01/10/02   ks      Fixed the rf_power_class in Radio Access Capability IE when
                   Multiband is supported. Changed GPRS multiclass to 8.
02/10/03   cd      Fixed Lint errors
02/14/03   ks      Removed the check for MSCR in get_revision_level_indicator().
03/06/03   ks      Setting the ECSD, EGPRS Multi-slot and M-slot IEs as absent
                   as they are not supported.
03/13/03   ks      Fixed the bit operand in setting the TMSI_STATUS IE in
                   gmm_send_attach_request_message() function.
04/01/03   ks      Fixed the bit operand in setting the TMSI_STATUS IE in
                   gmm_send_routing_area_update_request_message()
04/03/03   cd      Added support for equivalent PLMN list in the GMM_ATTACH_ACCEPT
                   and GMM_RAU_ACCEPT msg
04/10/03   cd      Corrected msg header length for a GMM_ATTACH_REQ msg
04/16/03   vdr     Corrected construction of the A&C Failure msg
05/09/03   vdr     Now TMSI_STATUS field is set correctly
05/13/03   npr     GPRS supported multislot class changed from 8 to 10.
05/29/03   jca     Added support for follow-on request bit in Attach Request message.
06/19/03   ks      Using the multi_slot_class, split_paging_cycle & non_drx_timer
                   from the stored NV.
07/07/03   vdr     Remove dependencies on Revision Level when Authentication And
                   Ciphering Response message is created
07/10/03   jca     Fixed GMM_IEI_MS_IDENTITY parsing error in RAU Accept Message.
07/16/03   ks      setting the gprs_msclass_already_included = TRUE only for R99
                   revision in function set_access_tech_info()
07/23/03   ks      Added debug statements for Split Paging Cycle
07/28/03   ks      Fixed the message_length if receive_npdu_number_list_present
                   is true in the RAU complete message.
08/28/03   ks      Setting the Ready timer value to 10sec in Attach Request and
                   RAU request. Optimum value proposed from GPRS field testing.
08/28/03   ks      Setting the revision_level properly based on the user
                   selected NV parameters.
09/16/03   ks      Added the correct enum check for NV parameter that sets the UE
                   revision number.
12/03/03   vdr     Now saving gmm_ptmsi_sig_tobe_deleted indicator in NV RAM when
                   P-TMSI is to be deleted
02/13/04   ks      Removed the check for ANITE_GCF_FLAG NV item when setting
                   the UE revision level.
02/20/04   vdr     Added processing required by READY timer negotiation procedure
04/08/04   vdr     Added follow-on request processing when GMM is sending RA update
                   request message during G2W inter-RAT change and user data is
                   available
04/19/04   vdr     Fixed compilation warnings
05/12/04   jca     Set rrc_establishment_cause to RRC_EST_CALL_RE_ESTABLISH when performing
                   initial RA Update procedure after receipt of a RRC_REL_IND with rel_cause
                   set to RRC_DIRECTED_SIGNALLING_REESTABLISHMENT.
05/14/04   jca     Now interpret undefined values of detach_type field in DETACH REQUEST
                   message as "re-attach not required" as per 24.008 Table 10.5.138.
06/09/04   vdr     Added support for Radio Access Capabilities describing GSM 850
                   frequency band
08/11/04   ks      Calling dsumtsps_is_ps_data_available() before the RAU procedure
                   to decide if FOR needs to be enabled.
08/25/04   jca     Modified function get_revision_level_indicator() to ensure
                   revision_level_ind always set to R99 when operating in UMTS mode.
08/27/04   ab      CR39294.  Added processing of received NPDU list.
09/21/04   vdr     Fixed Authentication And Ciphering Failure message format
11/05/04   ks      Setting FOR in Attach request message if a PDP context is being
                   activated by calling sm_is_pdp_activation_pending().
19/11/04   sm      Fixed for sending TMSI status in gmm_send_attach_request_message() and
                   gmm_send_routing_area_update_request_message() for combined procedures
                   and follow-on-request set.
11/22/04   sv      EDGE changes for MS RA Capability information.
11/30/04   kvk     PDP context status IE supported added in RAU request & GMM Service request
                   messages.
01/20/05   kvk    Setting the FOR bit in case any PDP connection is pending
02/14/05   sv     Added support for Rel-4 changes.
03/07/05   sv     N/W assisted cell change set based on NV item.
05/04/05   abe    If the data_is_available flag is already set during RAT change, do not
                  query PS again
06/20/05   hj      Added support for new events EVENT_SMGMM_REQUEST_SENT,
                   EVENT_SMGMM_REJECT_RECEIVED.
09/23/05   abe    If DCS-1800 and PCS-1900 bands are supported by our phone then
                  use only one in RAC based on the band_pref_mask.
09/16/05   ajt    Added code for parsing EMERGENCY Number List IE in function
                  gmm_valid_routing_area_update_accept_message and
                  gmm_valid_attach_accept_message.
10/3/05    up     Updated for Rel5 changes. RAC should not be set to 0x7 in "Attach Request".
10/06/05   ajt    Decode LCS-MOLR support in Attach Accept, RAU Accept Message
10/30/05   abe    Modified the get_revision_level_indicator function to take care of
                  UE revision level REL5.
11/23/05   ajt    Added processing for PDP Context Status IE in RAU Accept and
                  Service Accept messages.
11/30/05   jca    Merged DTM-related changes from Release 3.1 branch.
12/15/05   abe    Supported GSM DTM under FEATURE_GSM_DTM
02/24/06   abe     Adding support for FEATURE_REL_6_SUPPORT
07/25/06   pk    Modifying  Multiple TBF capability,SAIC/DARP and EDTM capability bits in set_access_tech_info() for REL-6.
08/18/11   abhi  Indicating PTMSI type in Attach/RAU request
08/18/11   abhi  Indicating PTMSI type in Attach/RAU request
08/19/11   jbk   Fix buffer overreads while processing ANC
09/04/11   gps   Removing unnecessary checks and checking with actual message length
                  rather than the maximum message size in gmm_valid_routing_area_update_accept_message() 
                  and gmm_valid_attach_accept_message()

===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "mmcp_variation.h"
#include "customer.h"
#include "comdef.h"
#include "err.h"
#include "sys_cnst_v.h"
#include "gmm_v.h"
#include "gmm_int_v.h"
#include "sm_v.h"
#include "gmm_msg.h"
#include "mm_v.h"
#include "mm_umts_v.h"
#include "cause.h"
#include "ghdi_exp.h"
#include "ghdi_exp_int.h"
#include "aconfig.h"
#include "gmmsecurity.h"
#include "libprot.h"
#include <stringl/stringl.h>
#if defined(FEATURE_GSM) && defined(FEATURE_GSM_MULTIBAND)
#include "rr_multiband.h"
#endif /* #if defined(FEATURE_GSM) && defined(FEATURE_GSM_MULTIBAND) */
#include "dsumtsps.h"
#include "sm_gmm.h"
#include "gmmllcif.h"
#ifdef FEATURE_LTE
#include "mm_multimode_common.h"
#include "emm.h"
#endif
#if defined(FEATURE_LTE)
#include "bit.h"
#include "emm_utility.h"
#include "lte_nas_emm_message_description.h"
#include "lte_rrc_ext_api.h"
#endif

#if defined(FEATURE_LTE)  && defined(FEATURE_LTE_REL9)
#include "mm_coord.h"
#endif

/* -----------------------------------------------------------------------
**  FUNCTION     : SM_PACK_PDP_CONTEXT_STATUS

**  DESCRIPTION  : This function packs the PDP context status for 16
**                 NSAPIs(0-15) in 16 bits
** ----------------------------------------------------------------------- */
extern void sm_pack_pdp_context_status(byte *ext_buf, sys_modem_as_id_e_type as_id);

#if defined(FEATURE_DUAL_SIM) && defined(FEATURE_DUAL_IMEI)
void ghdi_ds_read_imeisv(byte *imeisv,sys_modem_as_id_e_type as_id);
#endif
#ifdef FEATURE_MBMS
#error code not present
#endif
#ifdef FEATURE_LTE
#if defined FEATURE_DUAL_SIM || defined FEATURE_SGLTE
sys_drx_cn_coefficient_s1_e_type gmm_drx_cn_coeff_s1_sim[MAX_AS_IDS];
#define gmm_drx_cn_coeff_s1 gmm_drx_cn_coeff_s1_sim[mm_sub_id]
#else
extern  sys_drx_cn_coefficient_s1_e_type gmm_drx_cn_coeff_s1;
#endif
#endif/*FEATURE_LTE*/
#if defined FEATURE_DUAL_SIM || defined FEATURE_SGLTE
boolean  data_is_available_sim[MAX_NAS_STACKS] ;
#define  data_is_available data_is_available_sim[mm_as_id]
  extern ue_rev_level_indicator_type get_revision_level_indicator_per_nas_stacks
  (
    sys_modem_as_id_e_type as_id 
  );
#else
boolean  data_is_available ;
#endif

#ifdef FEATURE_DUAL_SIM
#define mm_ghdi_nvmem_write(a,b) ghdi_nvmem_write_per_subs(a,b,mm_nv_context_id)
#else
#define mm_ghdi_nvmem_write(a,b) ghdi_nvmem_write(a,b)
#endif

#if defined(FEATURE_LTE)
extern void encode_ue_nw_cap_ie
(
  lte_nas_emm_ue_capability_type *ue_capability,
  byte *msg_index,
  byte *ota_buf
);
#endif
extern byte gsm_wcdma_amr_codec_preference;
#ifdef FEATURE_LTE
#ifdef TEST_FRAMEWORK
#error code not present
#endif
#endif

/*===========================================================================

                DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains definitions for constants, macros, types, variables
and other items needed by this module.

===========================================================================*/


/*===========================================================================

FUNCTION GET_REVISION_LEVEL_INDICATOR

DESCRIPTION
  This function calculates Revision Level Indicator according to MS
  capabilities

DEPENDENCIES
  None

RETURN VALUE
  Revision Level Indicator.Based on the UE NV setting and the N/W
  revision it returns of the following:
  UE_REV_PRE_REL99, UE_REV_REL99 or UE_REV_REL5

SIDE EFFECTS
  None

===========================================================================*/
#ifdef FEATURE_NON_DEMAND_PAGED_FUNCTION
#error code not present
#endif
ue_rev_level_indicator_type get_revision_level_indicator( void )
{
#if defined FEATURE_DUAL_SIM || defined FEATURE_SGLTE
  MSG_ERROR_DS_0( 0, "=MM= DUAL SIM: Wrong API called get_revision_level_indicator()" );
  return get_revision_level_indicator_per_nas_stacks((sys_modem_as_id_e_type)mm_as_id);

#else

  ue_rev_level_indicator_type revision_level_ind = UE_REV_REL99;

  /*
  ** Set the revision level value based on the configured NV item NV_FORCE_UE_SGSNR_R99_I.
  */

  /* If the NV is value is to DYNAMIC, set the revision level based on NW sent SGSNR*/
  if (mm_nv_force_sgsnr_r99 == NV_FORCE_UE_DYNAMIC)
  {
    if ((mm_system_information.sgsnr || mm_serving_plmn.egprs_supported ) || is_umts_mode()
#ifdef FEATURE_LTE
        || is_lte_mode()
#endif
       )
    {
      /* it is a R99 or Later Network */
      revision_level_ind = UE_REV_REL99;
    }
    else /* network MSC revision level is R98 or earlier */
    {
      /* must be set to PHASE_2 for R98 or earlier networks */
      revision_level_ind = UE_REV_PRE_REL99 ;
    }
  }

#ifdef FEATURE_GSM_GPRS
  if (mm_nv_force_sgsnr_r99 == NV_FORCE_UE_REL99)
  {
    /* The NV Item forces the UE to always behave as R99 otherwise the workaround
    ** to change the UE behavior dynamically based on Network rev. # is applied */
    revision_level_ind = UE_REV_REL99;
  }

  /* Force the UE to R97 if the NV item is set to force R97 only */
  if (mm_nv_force_sgsnr_r99 == NV_FORCE_UE_REL97)
  {
    if (is_umts_mode()
#ifdef FEATURE_LTE
        || is_lte_mode()
#endif
       )
    {
      /* it is a R99 Network */
      revision_level_ind = UE_REV_REL99;
    }
    else
    {
      /* The NV Item forces the UE to always behave as R97 otherwise the workaround
      ** to change the UE behavior dynamically based on Network rev. # is applied */
      revision_level_ind = UE_REV_PRE_REL99;
    }
  }
#endif

  return revision_level_ind ;

#endif

} /* end get_revision_level_indicator() */
#ifdef FEATURE_NON_DEMAND_PAGED_FUNCTION
#error code not present
#endif

#if defined FEATURE_DUAL_SIM || defined FEATURE_SGLTE
#ifdef FEATURE_NON_DEMAND_PAGED_FUNCTION
#error code not present
#endif
ue_rev_level_indicator_type get_revision_level_indicator_per_nas_stacks
(
  sys_modem_as_id_e_type stack_id_nas 
)
{

  ue_rev_level_indicator_type revision_level_ind = UE_REV_REL99;

  sys_modem_as_id_e_type sub_id; 
  if (IS_NOT_VALID_STACK_ID(stack_id_nas))
  {
    MSG_ERROR_DS_1(MM_SUB,"get_revision_level_indicator_per_nas_stacks - Invalid STACK ID: %d", stack_id_nas+1);
    return revision_level_ind;
  }

  sub_id = (sys_modem_as_id_e_type)mm_sub_id_stack[stack_id_nas];

  if(IS_NOT_VALID_SUB_ID(sub_id))
  {
    MSG_ERROR_DS_1(MM_SUB,"get_revision_level_indicator_per_nas_stacks - Invalid SUB ID: %d", sub_id+1);
    return revision_level_ind;
  }
  /*
  ** Set the revision level value based on the configured NV item NV_FORCE_UE_SGSNR_R99_I.
  */

  /* If the NV is value is to DYNAMIC, set the revision level based on NW sent SGSNR*/
  if (mm_nv_force_sgsnr_r99_sim[sub_id] == NV_FORCE_UE_DYNAMIC)
  {
    if ((mm_system_information_sim[stack_id_nas].sgsnr || mm_serving_plmn_sim[stack_id_nas].egprs_supported ) ||
        (is_umts_mode_per_nas_stacks(stack_id_nas)) 
#ifdef FEATURE_LTE
        || is_lte_mode_per_nas_stacks(stack_id_nas)
#endif
        )
    {
      /* it is a R99 or Later Network */
      revision_level_ind = UE_REV_REL99;
    }
    else /* network MSC revision level is R98 or earlier */
    {
      /* must be set to PHASE_2 for R98 or earlier networks */
      revision_level_ind = UE_REV_PRE_REL99 ;
    }
  }

#ifdef FEATURE_GSM_GPRS
  if (mm_nv_force_sgsnr_r99_sim[sub_id] == NV_FORCE_UE_REL99)
  {
    /* The NV Item forces the UE to always behave as R99 otherwise the workaround
    ** to change the UE behavior dynamically based on Network rev. # is applied */
    revision_level_ind = UE_REV_REL99;
  }

  /* Force the UE to R97 if the NV item is set to force R97 only */
  if (mm_nv_force_sgsnr_r99_sim[sub_id] == NV_FORCE_UE_REL97)
  {
    if (is_umts_mode_per_nas_stacks(stack_id_nas)
#ifdef FEATURE_LTE
        || is_lte_mode_per_nas_stacks(stack_id_nas)
#endif
       )
    {
      /* it is a R99 Network */
      revision_level_ind = UE_REV_REL99;
    }
    else
    {
      /* The NV Item forces the UE to always behave as R97 otherwise the workaround
      ** to change the UE behavior dynamically based on Network rev. # is applied */
      revision_level_ind = UE_REV_PRE_REL99;
    }
  }
#endif

  return revision_level_ind ;

}
#ifdef FEATURE_NON_DEMAND_PAGED_FUNCTION
#error code not present
#endif
#endif


/*===========================================================================

FUNCTION GET_PDP_CONTEXT_STATUS

DESCRIPTION
  This function creates PDP Context Status indicating which NSAPIs are in use
  by SM.

DEPENDENCIES
  None

RETURN VALUE
   PDP Context Status indicating which NSAPIs are currently in use.

SIDE EFFECTS
  None

===========================================================================*/
gmm_ie_pdp_context_status_T get_pdp_context_status(sys_modem_as_id_e_type as_id) 
{
  gmm_ie_pdp_context_status_T pdp_cxt_status;
  //Set the PDP CONTEXT STATUS IE lenght
  pdp_cxt_status.length = GMM_PDP_CONTEXT_STATUS_LENGTH;
  //Now pack the status of each PDP
  sm_pack_pdp_context_status( pdp_cxt_status.value, as_id );


  return pdp_cxt_status;
}

/*===========================================================================

FUNCTION GET_UPLINK_DATA_STATUS

DESCRIPTION
  This function creates Uplink Data Status indicating which NSAPIs are in use
  by SM.

DEPENDENCIES
  None

RETURN VALUE
  Uplink Data Status indicating which NSAPIs are currently in use.

SIDE EFFECTS
  None

===========================================================================*/

gmm_ie_pdp_context_status_T get_uplink_data_status(sys_modem_as_id_e_type as_id)

{
  gmm_ie_pdp_context_status_T uplink_data_status;
  //Set the PDP CONTEXT STATUS IE lenght as it is same for Uplink_data_status.
  uplink_data_status.length = GMM_PDP_CONTEXT_STATUS_LENGTH;
  //Now pack the status of each
  rabm_pack_uplink_data_status( uplink_data_status.value, as_id ); 

  return uplink_data_status;
}

#ifdef FEATURE_MBMS
#error code not present
#endif
/*===========================================================================
 
FUNCTION GMM_SET_MULTISLOT_CLASS
 
DESCRIPTION
  This function returns the multislot class to be used in the Radio Access
  Capabilities IE.  For MS classes 1-12 there is no conversion.  For MS classes
  30-34 a conversion takes place See Table 10.5.146 of 24.008.
  Note.  This function is multi-entry and will be called by GMM, GMAC and GRR.
 
DEPENDENCIES
  None
 
RETURN VALUE
  uint8 - converted multislot class.
 
SIDE EFFECTS
  None
 
===========================================================================*/
#ifdef FEATURE_NON_DEMAND_PAGED_FUNCTION
#error code not present
#endif
uint8 gmm_set_multislot_class(uint8 ms_class)
{
  uint8 converted_ms_class = GPRS_MULTISLOT_CLASS;
 
#if defined (FEATURE_GSM_GPRS_MSC34) && defined (FEATURE_GSM_GPRS_MSC34_SUPPORTED)
  if (ms_class == 30)
  {
    converted_ms_class = 8;
  }
  else if (ms_class == 31)
  {
    converted_ms_class = 10;
  }
  else if (ms_class == 32)
  {
    converted_ms_class = 11;
  }
  else if (ms_class == 33)
  {
    converted_ms_class = 12;
  }
  else if (ms_class == 34)
  {
    converted_ms_class = 9;
  }
  else
#endif /*FEATURE_GSM_GPRS_MSC34 && FEATURE_GSM_GPRS_MSC34_SUPPORTED*/
  if (ms_class <= 12)
  {
    converted_ms_class = ms_class;
  }
  else
  {
    MSG_ERROR_DS_1(MM_SUB, "=MM= GMM MS Class Out of Range: %d",ms_class);
  }
 
  return converted_ms_class;
}

#ifdef FEATURE_NON_DEMAND_PAGED_FUNCTION
#error code not present
#endif

/*===========================================================================

FUNCTION GMM_ENCODE_RAC_LENGTH

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
void gmm_encode_rac_length(csn_encoder_T  *encoder,
                           word            access_caps_length_bit_pos)
{
  word length = 0;

  /* Now figure out how big is the access caps structure including length */
  length = csn_bit_pos(encoder) - access_caps_length_bit_pos;

  /* Length is in units of BITS and won't include its own size (7 bits) */
  length -= 7;

  /* Write this computed access caps length at the length element */
  csn_encode_bits_at_pos(encoder, 7, (byte) length, access_caps_length_bit_pos);
}
#ifdef FEATURE_NON_DEMAND_PAGED_FUNCTION
#error code not present
#endif


/*===========================================================================

FUNCTION GMM_TRUNCATE_MAC_RAC

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
boolean gmm_truncate_mac_rac(word max_len, 
                             word curr_len, 
                             word req_len)
{
  boolean truncate = FALSE;

  if (max_len != 0)
  {
    if ( (curr_len + req_len) > max_len)
    {      
      truncate = TRUE;
    }
  }
  return truncate;
}
#ifdef FEATURE_NON_DEMAND_PAGED_FUNCTION
#error code not present
#endif


#define TRUNCATE_RAC(max_len, req_len, encoder, access_caps_length_bit_pos) \
   if (gmm_truncate_mac_rac(max_len, (word)(csn_bit_pos(encoder) - (access_caps_length_bit_pos - 4)), req_len) == TRUE) \
   { \
     gmm_encode_rac_length(encoder, access_caps_length_bit_pos); \
     return TRUE; \
   }


/*===========================================================================

FUNCTION SET_ACCESS_TECH_INFO

DESCRIPTION
  This function constructs Access Capability Structure of the MS RAC IE using
  the classmark and RR information

DEPENDENCIES
  None

RETURN VALUE
  TRUE -- RAC truncated
  FALSE -- RAC not truncated
SIDE EFFECTS
  None

===========================================================================*/
#ifdef FEATURE_NON_DEMAND_PAGED_FUNCTION
#error code not present
#endif
boolean set_access_tech_info
(
  csn_encoder_T  *encoder ,
  byte           access_technology ,
  byte           revision_level_ind,
  byte           rf_power_class,
  hw_classmark_T *hw_classmark,
  boolean        *A5_already_included,
  boolean        *gprs_msclass_already_included,
  word           max_rac_length /*0 means no length specified*/
)
{
#if defined FEATURE_DUAL_SIM 
  MSG_FATAL_DS( MM_SUB, "=MM= DUAL SIM: Wrong API called set_access_tech_info()", 0,0,0 );
#elif defined FEATURE_SGLTE
 return set_access_tech_info_per_nas_stacks(encoder,access_technology,revision_level_ind,rf_power_class,
               hw_classmark,A5_already_included,gprs_msclass_already_included,max_rac_length,SYS_MODEM_AS_ID_1,mm_ps_stack_id);

#else
  word                        length;
  word                        access_caps_length_bit_pos;
  byte                        rf_power_capability = 0;
  ue_rev_level_indicator_type ms_revision_level;
#if defined (FEATURE_GSM_GPRS_MSC34) && defined (FEATURE_GSM_GPRS_MSC34_SUPPORTED)
  boolean hmsc_already_included = FALSE;
#endif
#ifdef FEATURE_LTE
  boolean eutra_FDD_TDD_support = FALSE;
#endif
  csn_encode_bits(encoder, 4, (byte)access_technology);

  (void) revision_level_ind;
  
  /* Get the Revision Level indicator */
  ms_revision_level = get_revision_level_indicator();

  /* Save the starting bit pos of access capability length element */
  access_caps_length_bit_pos = csn_bit_pos(encoder);

  /*
  * For now, pack length place holder
  * Once the real length is known, put that length in this field
  */
  csn_encode_bits(encoder, 7, (byte) 0 ) ;

  /*
  ** RF power class capability shall be set with respect
  ** to the supported BAND, does not matter what is the current RAT.
  if (is_umts_mode())
  {
    rf_power_capability = 0x7 ;
  }
  else
  */
  {
    if (mm_gsm_supported)
    {
#ifdef FEATURE_GSM_MULTIBAND
      switch (rf_power_class)
      {
        case CLASS_1:
          rf_power_capability = 1;
          break;
        case CLASS_2:
          rf_power_capability = 2;
          break;
        case CLASS_3:
          rf_power_capability = 3;
          break;
        case CLASS_4:
          rf_power_capability = 4;
          break;
        case CLASS_5:
          rf_power_capability = 5;
          break;
        default:
          MSG_ERROR_DS_1(MM_SUB, "=MM= Invalid power class=%d",(int)rf_power_class);
          break;
      } /* switch */
#else
      #error code not present
#endif /* #ifdef FEATURE_GSM_MULTIBAND */
    }
    else
    {
      /*
      ** Table 10.5.146, 24.008 MS RAC Information element:
      ** A MS which does not support any GSM access technology type shall set this field to '000'.
      */
      rf_power_capability = 0x0;
    }
  } /* if (is_umts_mode()) */

  /* RF Power caps (3bit) */
  csn_encode_bits(encoder, 3, (byte) rf_power_capability );

  if (*A5_already_included == TRUE)
  {
    /*
    ** Zero means that the same values apply for parameters as in the
    ** immediately preceeding Access capabilities field within this IE
    */
    csn_encode_bit(encoder, (byte) 0 );
  }
  else
  {
    /* Mark A5 algorithm info as already included */
    *A5_already_included = TRUE ;

    /* Indicate that A5 bits are following */
    csn_encode_bit(encoder, (byte) 1 );

    csn_encode_bit(encoder, (byte) hw_classmark->A5_1_supported);
    csn_encode_bit(encoder, (byte) hw_classmark->A5_2_supported);
    csn_encode_bit(encoder, (byte) hw_classmark->A5_3_supported);
    csn_encode_bit(encoder, (byte) hw_classmark->A5_4_supported);
    csn_encode_bit(encoder, (byte) hw_classmark->A5_5_supported);
    csn_encode_bit(encoder, (byte) hw_classmark->A5_6_supported);
    csn_encode_bit(encoder, (byte) hw_classmark->A5_7_supported);
  }

  csn_encode_bit(encoder, (byte) ES_IND);
  csn_encode_bit(encoder, (byte) ps_capability);
  csn_encode_bit(encoder, (byte) VGCS_supported);
  csn_encode_bit(encoder, (byte) VBS_supported);

  if ( *gprs_msclass_already_included == TRUE )
  {
    /*
    ** Zero means that the same values apply for parameters as in the
    ** immediately preceeding Access capabilities field within this IE
    */

    csn_encode_bit(encoder, (byte) 0 ); /* Field is not present */
  }
  else
  {
    /* If it is R99, Mark GPRS Multislot capability field as already included. */
    if (ms_revision_level == UE_REV_REL99)
    {
      *gprs_msclass_already_included = TRUE;
    }

    csn_encode_bit(encoder, (byte) 1 ); /* Field is present */
    csn_encode_bit(encoder, (byte) 0 ); /* HSCSD Sub-field is not present. */
    csn_encode_bit(encoder, (byte) 1 ); /* Sub-field is present. */
    csn_encode_bits(encoder, 5, (byte) gmm_set_multislot_class (gmm_multislot_class) );

#ifdef FEATURE_GPRS_EDA
    if ( gmm_nv_eda_feature_support )
      csn_encode_bit(encoder, (byte) 1 ); /* GPRS Ext Dyn Alloc Cap */
    else
      csn_encode_bit(encoder, (byte) 0 ); /* GPRS Ext Dyn Alloc Cap */
      
    MSG_HIGH_DS_3(MM_SUB, "=MM= Acc Tech %d: GPRS Class = %d with EDA = %d",
      access_technology, gmm_multislot_class, gmm_nv_eda_feature_support);
#else
    csn_encode_bit(encoder, (byte) 0 ); /* GPRS Ext Dyn Alloc Cap */
    
    MSG_HIGH_DS_2(MM_SUB, "=MM= Acc Tech %d: GPRS Class = %d",
      access_technology, gmm_multislot_class);
      
#endif /* FEATURE_GPRS_EDA */

    /*  sms_sm value is not supported by NAS*/
    csn_encode_bit(encoder, (byte) 0 ); /* Sub-field is not included. */


    if (ms_revision_level == UE_REV_REL99)
    {
      csn_encode_bit(encoder, (byte) 0 ); /* ECSD Field is Absent */
#ifdef FEATURE_GSM_EGPRS
      /* If EDGE is enabled in the phone, set the values accordingly */
      if (gmm_edge_feature_support)
      {
          csn_encode_bit(encoder, (byte) 1 ); /* EGPRS M-slot Field is Present */
          csn_encode_bits(encoder, 5, (byte) gmm_set_multislot_class (gmm_edge_multislot_class) );

#ifdef FEATURE_GPRS_EDA
          if ( gmm_nv_eda_feature_support )
            csn_encode_bit(encoder, (byte) 1 ); /* GPRS Ext Dyn Alloc Cap */
          else
            csn_encode_bit(encoder, (byte) 0 ); /* GPRS Ext Dyn Alloc Cap */

          MSG_HIGH_DS_2(MM_SUB, "=MM= EGPRS Class = %d with EDA = %d",
            gmm_edge_multislot_class, gmm_nv_eda_feature_support);            
#else
          csn_encode_bit(encoder, (byte) 0 ); /* GPRS Ext Dyn Alloc Cap */
          
          MSG_HIGH_DS_1(MM_LOCAL_SUB, "=MM= EGPRS Class = %d", gmm_edge_multislot_class);

#endif /* FEATURE_GPRS_EDA */
      }
      else
      {
         csn_encode_bit(encoder, (byte) 0 ); /* EGPRS M-slot Field is Absent */
         MSG_HIGH_DS_0(MM_LOCAL_SUB, "=MM= EGPRS Disabled");
      }
#else
      csn_encode_bit(encoder, (byte) 0 ); /* EGPRS M-slot Field is Absent */
#endif
#ifdef FEATURE_GSM_DTM
      if ( gmm_nv_dtm_feature_support )
      {      
        csn_encode_bit(encoder, (byte) 1 ); /* DTM M-slot Field is Present */
        csn_encode_bits(encoder, 2, (byte) gmm_nv_dtm_multislot_subclass); 
        csn_encode_bit(encoder, (byte) 0 ); /* single slot dtm not supported */
#ifdef FEATURE_GSM_EGPRS
        if (gmm_edge_feature_support)
        {
          csn_encode_bit(encoder, (byte) 1 ); /* EGPRS DTM supported */
          csn_encode_bits(encoder, 2, (byte) gmm_nv_dtm_multislot_subclass);
        }
        else
#endif /* FEATURE_GSM_EGPRS */
        {
          csn_encode_bit(encoder, (byte) 0 ); /* EGPRS DTM not supported */
        }
        MSG_HIGH_DS_2(MM_LOCAL_SUB, "=MM= Acc Tech %d: DTM Multislot Sub Class = %d", access_technology,
          gmm_nv_dtm_multislot_subclass);
      }
      else
      {
        csn_encode_bit(encoder, (byte) 0 ); /* DTM M-slot Field is Absent */
        MSG_HIGH_DS_0(MM_LOCAL_SUB, "=MM= DTM Disabled");
      }
#else
      csn_encode_bit(encoder, (byte) 0 ); /* DTM M-slot Field is Absent */
#endif /* FEATURE_GSM_DTM */
    }
  }
  /* Revision level is set to 1 for Rel 99 and onwards */
  if (ms_revision_level == UE_REV_REL99)
  {
#ifdef FEATURE_GSM_EGPRS
      /*
      * 8PSK power capability
      */
      /* Check the NV item to see if EDGE is enabled. If enabled, fill the power
      ** capability values based on the support for 8PSK modulation in uplink
      */
      if (gmm_edge_feature_support)
      {
         /* 8PSK Power Class indicates whether or not 8PSK mod in uplink is
         ** supported by the phone. Based on that, the power class shall be set.
         */
         if (gmm_edge_8psk_power_class != 0)
         {
             /* 8PSK modulation in uplink supported */
             MSG_HIGH_DS_2(MM_LOCAL_SUB, "=MM= EGPRS UL 8PSK class %d, capability %d", gmm_edge_8psk_power_class,
                       gmm_edge_8psk_power_capability);
             csn_encode_bit(encoder, (byte) 1 );
             csn_encode_bits(encoder, 2, gmm_edge_8psk_power_capability); /* Power Class supported by the phone */
         }
         else
         {
             MSG_HIGH_DS_0(MM_LOCAL_SUB, "=MM= EGPRS UL 8PSK Disabled");
             csn_encode_bit(encoder, (byte) 0 );
         }
      }
      else
      {
          /* EDGE not supported, hence ignore support for 8PSK */
          csn_encode_bit(encoder, (byte) 0 );
      }
#else
      /* No EGPRS in the stack. Disable 8PSK by default */
      csn_encode_bit(encoder, (byte) 0 );
#endif

    /* COMPACT interference measurement caps */
    csn_encode_bit(encoder, (byte) compact_interference_caps);

    csn_encode_bit(encoder, (byte) ms_revision_level);

    /*
    ** UE preferred mode is GSM Only, set UMTS FDD as FALSE.
    */
#ifndef FEATURE_RAT_PRIORITY_LIST
    if ((mm_ue_pref_rat == SYS_RAT_GSM_RADIO_ACCESS) && 
        (mm_dual_mode == FALSE))
#else
    /* Report UMTS capability accordingly
    ** UMTS FDD Radio Access Technology Capability field per 3GPP 24.008 sect 10.5.1.7
    */
    if ((!mm_checkif_rat_pri_list_has_rat(&mm_rat_pri_list_info,SYS_RAT_UMTS_RADIO_ACCESS))
#ifdef FEATURE_SGLTE
        || MM_IS_IN_SGLTE_STATE_4
#endif
       )
#endif /* FEATURE_RAT_PRIORITY_LIST */
    {
      csn_encode_bit(encoder, (byte) 0);
    }
    else
    {
      /* Regardless if TDSCDMA is present in the rat prio list, report WCDMA if present */
      csn_encode_bit(encoder, (byte) UMTS_FDD_RAT_supported);
    }
    /* This field is for 3.84 Mcps TD , no support*/
    csn_encode_bit(encoder, (byte) 0);
    csn_encode_bit(encoder, (byte) CDMA2000_RAT_supported);

    /* Rel - 4 IE's start here */

#ifdef FEATURE_REL_4_SUPPORT
    TRUNCATE_RAC (max_rac_length, 1, encoder, access_caps_length_bit_pos)
    /* UMTS 1.28 Mcps TDD Radio Access Technology Capability field */
#ifdef FEATURE_RAT_PRIORITY_LIST
    if((!mm_checkif_rat_pri_list_has_rat(&mm_rat_pri_list_info,SYS_RAT_TDS_RADIO_ACCESS))
#ifdef FEATURE_SGLTE
        || MM_IS_IN_SGLTE_STATE_4
#endif

      )
    {
    csn_encode_bit(encoder, (byte) 0 );
    }
    else if (!mm_checkif_rat_pri_list_has_rat(&mm_rat_pri_list_info,SYS_RAT_UMTS_RADIO_ACCESS))
    {  /* If TDSCDMA is present and WCDMA is not, report TDSCMA */
       csn_encode_bit(encoder, (byte) 1);
    }   
    else
#endif
    {
    csn_encode_bit(encoder, (byte) 0 );
    }
    /* Support GERAN release pack 1 */
    TRUNCATE_RAC (max_rac_length, 1, encoder, access_caps_length_bit_pos)
#ifdef FEATURE_GSM_GERAN_FEATURE_PACK_1
    csn_encode_bit(encoder, (byte) gmm_edge_nw_ass_cell_change);
#else
    csn_encode_bit(encoder, (byte) 0 );
#endif /* FEATURE_GSM_GERAN_FEATURE_PACK_1 */

    /* Extended DTM GPRS/EGPRS multislot class - NOT supported */
    TRUNCATE_RAC (max_rac_length, 1, encoder, access_caps_length_bit_pos)
    csn_encode_bit(encoder, (byte) 0 );

    /* Modulation based multislot class support (1 bit field) - NOT Supported */
    TRUNCATE_RAC (max_rac_length, 1, encoder, access_caps_length_bit_pos)
    csn_encode_bit(encoder, (byte) 0 );

#endif /* FEATURE_REL_4_SUPPORT */

#if defined(FEATURE_REL5) || \
    (!defined(FEATURE_WCDMA) && defined(FEATURE_REL_6_SUPPORT))
   #ifndef FEATURE_REL_4_SUPPORT
     #error FEATURE_REL5 requires FEATURE_REL_4_SUPPORT
   #endif

    /* High Multislot Capability */
#if defined (FEATURE_GSM_GPRS_MSC34) && defined (FEATURE_GSM_GPRS_MSC34_SUPPORTED)
    if ( hmsc_already_included == TRUE )   
    {
      TRUNCATE_RAC (max_rac_length, 1, encoder, access_caps_length_bit_pos)
      csn_encode_bit(encoder, (byte) 0 );
    }
    else
    { 
#ifdef FEATURE_GSM_EGPRS
      /* If EDGE is enabled in the phone, set the values accordingly */
      if (gmm_edge_feature_support)
      {
        if ((gmm_edge_multislot_class == (byte)30) || (gmm_edge_multislot_class == (byte)32) || (gmm_edge_multislot_class == (byte)33))
        {
          TRUNCATE_RAC (max_rac_length, 3, encoder, access_caps_length_bit_pos)
          csn_encode_bit(encoder, (byte) 1 );
          csn_encode_bits(encoder, 2, (byte) 0 );
        }
        else if ((gmm_edge_multislot_class == (byte)31) || (gmm_edge_multislot_class == (byte)34))
        { 
          TRUNCATE_RAC (max_rac_length, 3, encoder, access_caps_length_bit_pos)
          csn_encode_bit(encoder, (byte) 1 );
          csn_encode_bits(encoder, 2, (byte) 3 );          
        }
        else
        {
          TRUNCATE_RAC (max_rac_length, 1, encoder, access_caps_length_bit_pos)
          csn_encode_bit(encoder, (byte) 0 );       
        }
      }
      else
#endif /* FEATURE_GSM_EGPRS */
      {
        if ((gmm_multislot_class == (byte)30) || (gmm_multislot_class == (byte)32) || (gmm_multislot_class == (byte)33))
        {
          TRUNCATE_RAC (max_rac_length, 3, encoder, access_caps_length_bit_pos)
          csn_encode_bit(encoder, (byte) 1 );
          csn_encode_bits(encoder, 2, (byte) 0 );
        }
        else if ((gmm_multislot_class == (byte)31) ||(gmm_multislot_class == (byte)34))
        { 
          TRUNCATE_RAC (max_rac_length, 3, encoder, access_caps_length_bit_pos)
          csn_encode_bit(encoder, (byte) 1 );
          csn_encode_bits(encoder, 2, (byte) 3 );          
        }
        else
        {
          TRUNCATE_RAC (max_rac_length, 1, encoder, access_caps_length_bit_pos)
          csn_encode_bit(encoder, (byte) 0 );       
        }
      }
    }
#else
    TRUNCATE_RAC (max_rac_length, 1, encoder, access_caps_length_bit_pos)
    csn_encode_bit(encoder, (byte) 0 );
#endif /* FEATURE_GSM_GPRS_MSC34 */

    /* GERAN Iu Mode Capability  - Not Supported ? */
    TRUNCATE_RAC (max_rac_length, 1, encoder, access_caps_length_bit_pos)
#ifdef FEATURE_GSM_GERAN_FEATURE_PACK_2
    csn_encode_bit(encoder, (byte) 1 );
#else
    csn_encode_bit(encoder, (byte) 0 );
#endif

    /* GMSK Multislot Power Profile - Need support from RR */
    TRUNCATE_RAC (max_rac_length, 2, encoder, access_caps_length_bit_pos)
    csn_encode_bits(encoder,2,(byte) gmm_nv_multislot_gmsk_power_profile);

    /* 8-PSK Multislot Power Profile - Need support from RR */
    TRUNCATE_RAC (max_rac_length, 2, encoder, access_caps_length_bit_pos)
    csn_encode_bits(encoder,2,(byte) gmm_nv_multislot_8psk_power_profile);

#endif /* FEATURE_REL5 */

#ifdef FEATURE_REL_6_SUPPORT  
    #if !defined (FEATURE_REL_4_SUPPORT) || \
        (defined(FEATURE_WCDMA) && !defined(FEATURE_REL5))
      #error FEATURE_REL_6_SUPPORT requires FEATURE_REL_4_SUPPORT and FEATURE_REL5
    #endif

    /* Multiple TBF capability - Not supported */
    TRUNCATE_RAC (max_rac_length, 1, encoder, access_caps_length_bit_pos)
    csn_encode_bit(encoder, (byte) 0 );

    /* SAIC/DARP Capability */
    TRUNCATE_RAC (max_rac_length, 2, encoder, access_caps_length_bit_pos)
#if (defined(FEATURE_GSM_EGPRS_SAIC) && defined(FEATURE_GSM_MDSP_AEQ))
    csn_encode_bits(encoder,2,gmm_nv_saic_feature_support);
#else
    /* not available as feature in this build */
    csn_encode_bits(encoder,2,0); 
#endif /* FEATURE_GSM_EGPRS_SAIC */

    TRUNCATE_RAC (max_rac_length, 2, encoder, access_caps_length_bit_pos)
#if defined(FEATURE_GSM_DTM) && defined(FEATURE_GSM_EDTM)
    /* Enhanced DTM (EDTM)and Extended RLC/MAC segmentation */
    csn_encode_bit(encoder, (byte) gmm_nv_edtm_feature_support );
    csn_encode_bit(encoder, (byte) gmm_nv_edtm_feature_support );
#else
        csn_encode_bits(encoder, 2, 0);
#endif

    /* DTM GPRS High Multi Slot Class - Not supported */
    TRUNCATE_RAC (max_rac_length, 1, encoder, access_caps_length_bit_pos)
    csn_encode_bit(encoder, (byte) 0 );

    TRUNCATE_RAC (max_rac_length, 1, encoder, access_caps_length_bit_pos)
#ifdef FEATURE_GPRS_PS_HANDOVER
    if (gmm_nv_psho_feature_support == TRUE)
    {
      /* PS Handover capability -supported */
      csn_encode_bit(encoder, (byte) 1 );
    }
    else
    {
      csn_encode_bit(encoder, (byte) 0 );
    }
#else
    /* PS Handover capability - Not supported */
    csn_encode_bit(encoder, (byte) 0 );
#endif
    
#endif /* FEATURE_REL_6_SUPPORT */

#if defined(FEATURE_LTE) 
   /**** REL 7 Additions - Not Supported ***/

    /*DTM Handover Capability - Not supported*/
    TRUNCATE_RAC (max_rac_length, 1, encoder, access_caps_length_bit_pos)
    csn_encode_bit(encoder, (byte) 0 ); 

    /*Multislot Capability Reduction for Downlink Dual Carrier - Not Supported*/
    TRUNCATE_RAC (max_rac_length, 1, encoder, access_caps_length_bit_pos)
    csn_encode_bit(encoder, (byte) 0 ); 

    /*Flexible Timeslot Assignment - Not Supported*/   
    TRUNCATE_RAC (max_rac_length, 1, encoder, access_caps_length_bit_pos)
    csn_encode_bit(encoder, (byte) 0 ); 

    /*GAN PS Handover Capability - Not Supported*/
    TRUNCATE_RAC (max_rac_length, 1, encoder, access_caps_length_bit_pos)
    csn_encode_bit(encoder, (byte) 0 );   

    /*RLC Non-persistent Mode - Not Supported*/
    TRUNCATE_RAC (max_rac_length, 1, encoder, access_caps_length_bit_pos)
    csn_encode_bit(encoder, (byte) 0 );

    /*Reduced Latency Capability - Not Supported*/
    TRUNCATE_RAC (max_rac_length, 1, encoder, access_caps_length_bit_pos)
    csn_encode_bit(encoder, (byte) 0 );

    /*Uplink EGPRS2 - Not Supported*/
    TRUNCATE_RAC (max_rac_length, 2, encoder, access_caps_length_bit_pos)
    csn_encode_bits(encoder, 2, 0);

    /*Downlink EGPRS2  - Not Supported*/
    TRUNCATE_RAC (max_rac_length, 2, encoder, access_caps_length_bit_pos)
    csn_encode_bits(encoder, 2, 0);

    /*****     REL8 Features      **/
    TRUNCATE_RAC (max_rac_length, 1, encoder, access_caps_length_bit_pos)
    /*E-UTRA FDD support is true only if Rat priority list contains the lte rat */
    if (mm_checkif_rat_pri_list_has_rat(&mm_rat_pri_list_info, SYS_RAT_LTE_RADIO_ACCESS) &&
        lte_rrc_lte_fdd_is_supported(mm_get_lte_band_capacity(&mm_rat_pri_list_info))
#ifdef FEATURE_SGLTE
         && !MM_IS_IN_SGLTE_STATE_4
#endif
        )
    {
      /* E-UTRA FDD support - Supported*/
      csn_encode_bit(encoder, (byte) 1 );
      eutra_FDD_TDD_support = TRUE ;
    }
    else
    {
      /* E-UTRA FDD support - Not Supported*/
      csn_encode_bit(encoder, (byte) 0 );
    }

    /*E-UTRA TDD support is true only if Rat priority list contains the lte rat */
    TRUNCATE_RAC (max_rac_length, 1, encoder, access_caps_length_bit_pos)
    if (mm_checkif_rat_pri_list_has_rat(&mm_rat_pri_list_info, SYS_RAT_LTE_RADIO_ACCESS)&&
        lte_rrc_lte_tdd_is_supported(mm_get_lte_band_capacity(&mm_rat_pri_list_info)) 
#ifdef FEATURE_SGLTE
         && !MM_IS_IN_SGLTE_STATE_4
#endif
        )
    {
      /* E-UTRA TDD support - Supported*/
      csn_encode_bit(encoder, (byte) 1 );
      eutra_FDD_TDD_support = TRUE ;
    }
    else
    {
      /* E-UTRA TDD support - Not Supported*/
      csn_encode_bit(encoder, (byte) 0 );  
    }
      
    /*GERAN to E-UTRA support in GERAN packet transfer mode --    
      0 0 - Not supported    */
    TRUNCATE_RAC (max_rac_length, 2, encoder, access_caps_length_bit_pos)
    if(eutra_FDD_TDD_support == TRUE)
    {
      /*Setting two bits as per efs value read*/
      csn_encode_bits(encoder, 2, gmm_nv_geran_to_eutra_measurement_support);
    }
    else 
    {
   /* If both "E-UTRA FDD support" and "E-UTRA TDD support" bits are set to '0', 
                                             the bit field shall be set to '0 0'*/
      csn_encode_bits(encoder, 2, 0);
    }


    /* Priority-based reselection - Supported*/
    TRUNCATE_RAC (max_rac_length, 1, encoder, access_caps_length_bit_pos)
    csn_encode_bit(encoder, (byte) 1 );

#endif

    TRUNCATE_RAC (max_rac_length, 1, encoder, access_caps_length_bit_pos)
    /* Fill enhanced flexible timeslot assignment */
    csn_encode_bit(encoder, 0);

    TRUNCATE_RAC (max_rac_length, 1, encoder, access_caps_length_bit_pos)
    /* Indication of Upper Layer PDU Start Capability for RLC UM  */
    csn_encode_bit(encoder, 0);

    TRUNCATE_RAC (max_rac_length, 1, encoder, access_caps_length_bit_pos)    
    /* EMST Capability  */
    csn_encode_bit(encoder, 0);

    TRUNCATE_RAC (max_rac_length, 1, encoder, access_caps_length_bit_pos)    
    /* MTTI capability */
    csn_encode_bit(encoder, 0);

    TRUNCATE_RAC (max_rac_length, 1, encoder, access_caps_length_bit_pos)
    /* UTRA CSG Cells Reporting  */
    csn_encode_bit(encoder, 0);

    TRUNCATE_RAC (max_rac_length, 1, encoder, access_caps_length_bit_pos)
    /* E-UTRA CSG Cells Reporting  */
    csn_encode_bit(encoder, 0);

    TRUNCATE_RAC (max_rac_length, 1, encoder, access_caps_length_bit_pos)
    /* DTR Capability  */
    csn_encode_bit(encoder, 0);

    TRUNCATE_RAC (max_rac_length, 1, encoder, access_caps_length_bit_pos)
    /* EMSR Capability  */
    csn_encode_bit(encoder, 0);

    TRUNCATE_RAC (max_rac_length, 1, encoder, access_caps_length_bit_pos)
    /* Fast Downlink Frequency Switching Capability */
    csn_encode_bit(encoder, 0);

    TRUNCATE_RAC (max_rac_length, 2, encoder, access_caps_length_bit_pos)    
    /* Tighter cap */
    csn_encode_bits(encoder, 2, mm_nv_tighter_capability);

  }

  /* Now figure out how big is the access caps structure including length */
  length = csn_bit_pos(encoder) - access_caps_length_bit_pos;

  /* Length is in units of BITS and won't include its own size (7 bits) */
  length -= 7;

  /* Write this computed access caps length at the length element */
  csn_encode_bits_at_pos(encoder, 7, (byte) length, access_caps_length_bit_pos);

#endif
  return FALSE;
} /* end set_access_tech_info() */
#ifdef FEATURE_NON_DEMAND_PAGED_FUNCTION
#error code not present
#endif

#if defined FEATURE_DUAL_SIM || defined FEATURE_SGLTE
#ifdef FEATURE_NON_DEMAND_PAGED_FUNCTION
#error code not present
#endif

/*===========================================================================

FUNCTION set_access_tech_info_per_nas_stacks

DESCRIPTION
  This function constructs Access Capability Structure of the MS RAC IE using
  the classmark and RR information.
  This function is called within NAS. This function takes subscription ID and nas stack ID
  as parameters.

DEPENDENCIES
  None

RETURN VALUE
  TRUE -- RAC truncated
  FALSE -- RAC not truncated
SIDE EFFECTS
  None

===========================================================================*/
boolean set_access_tech_info_per_nas_stacks
(
  csn_encoder_T  *encoder ,
  byte           access_technology ,
  byte           revision_level_ind,
  byte           rf_power_class,
  hw_classmark_T *hw_classmark,
  boolean        *A5_already_included,
  boolean        *gprs_msclass_already_included,
  word           max_rac_length, /*0 means no length specified*/
  sys_modem_as_id_e_type sub_id,
  sys_modem_as_id_e_type as_id
)
{ 
  word                        length;
  word                        access_caps_length_bit_pos;
  byte                        rf_power_capability = 0;
  ue_rev_level_indicator_type ms_revision_level;
#if defined (FEATURE_GSM_GPRS_MSC34) && defined (FEATURE_GSM_GPRS_MSC34_SUPPORTED)
  boolean hmsc_already_included = FALSE;
#endif
#ifdef FEATURE_LTE
  boolean eutra_FDD_TDD_support = FALSE;
#endif
  if (IS_NOT_VALID_SUB_ID(sub_id))
  {
    MSG_ERROR_DS_1(MM_SUB,"set_access_tech_info_per_nas_stacks - Invalid AS ID: %d", sub_id+1);
    return FALSE;
  }

  if (IS_NOT_VALID_STACK_ID(as_id))
  {
    MSG_ERROR_DS_1(MM_SUB,"set_access_tech_info_per_nas_stacks - Invalid STACK ID: %d", as_id+1);
    return FALSE;
  }

  csn_encode_bits(encoder, 4, (byte)access_technology);

  (void) revision_level_ind;
  
  /* Get the Revision Level indicator */
  ms_revision_level = get_revision_level_indicator_per_nas_stacks(as_id);

  /* Save the starting bit pos of access capability length element */
  access_caps_length_bit_pos = csn_bit_pos(encoder);

  /*
  * For now, pack length place holder
  * Once the real length is known, put that length in this field
  */
  csn_encode_bits(encoder, 7, (byte) 0 ) ;
  /*
  ** RF power class capability shall be set with respect
  ** to the supported BAND, does not matter what is the current RAT.
  if (is_umts_mode())
  {
    rf_power_capability = 0x7 ;
  }
  else
  */
  {
    if (mm_gsm_supported)
    {
#ifdef FEATURE_GSM_MULTIBAND
      switch (rf_power_class)
      {
        case CLASS_1:
          rf_power_capability = 1;
          break;
        case CLASS_2:
          rf_power_capability = 2;
          break;
        case CLASS_3:
          rf_power_capability = 3;
          break;
        case CLASS_4:
          rf_power_capability = 4;
          break;
        case CLASS_5:
          rf_power_capability = 5;
          break;
        default:
          MSG_ERROR_DS_1(MM_LOCAL_SUB, "=MM= Invalid power class=%d",(int)rf_power_class);
          break;
      } /* switch */
#else
      #error code not present
#endif /* #ifdef FEATURE_GSM_MULTIBAND */
    }
    else
    {
      /*
      ** Table 10.5.146, 24.008 MS RAC Information element:
      ** A MS which does not support any GSM access technology type shall set this field to '000'.
      */
      rf_power_capability = 0x0;
    }
  } /* if (is_umts_mode()) */

  /* RF Power caps (3bit) */
  csn_encode_bits(encoder, 3, (byte) rf_power_capability );

  if (*A5_already_included == TRUE)
  {
    /*
    ** Zero means that the same values apply for parameters as in the
    ** immediately preceeding Access capabilities field within this IE
    */
    csn_encode_bit(encoder, (byte) 0 );
  }
  else
  {
    /* Mark A5 algorithm info as already included */
    *A5_already_included = TRUE ;

    /* Indicate that A5 bits are following */
    csn_encode_bit(encoder, (byte) 1 );

    csn_encode_bit(encoder, (byte) hw_classmark->A5_1_supported);
    csn_encode_bit(encoder, (byte) hw_classmark->A5_2_supported);
    csn_encode_bit(encoder, (byte) hw_classmark->A5_3_supported);
    csn_encode_bit(encoder, (byte) hw_classmark->A5_4_supported);
    csn_encode_bit(encoder, (byte) hw_classmark->A5_5_supported);
    csn_encode_bit(encoder, (byte) hw_classmark->A5_6_supported);
    csn_encode_bit(encoder, (byte) hw_classmark->A5_7_supported);
  }

  csn_encode_bit(encoder, (byte) ES_IND);
  csn_encode_bit(encoder, (byte) ps_capability);
  csn_encode_bit(encoder, (byte) VGCS_supported);
  csn_encode_bit(encoder, (byte) VBS_supported);

  if ( *gprs_msclass_already_included == TRUE )
  {
    /*
    ** Zero means that the same values apply for parameters as in the
    ** immediately preceeding Access capabilities field within this IE
    */

    csn_encode_bit(encoder, (byte) 0 ); /* Field is not present */
  }
  else
  {
    /* If it is R99, Mark GPRS Multislot capability field as already included. */
    if (ms_revision_level == UE_REV_REL99)
    {
      *gprs_msclass_already_included = TRUE;
    }

    csn_encode_bit(encoder, (byte) 1 ); /* Field is present */
    csn_encode_bit(encoder, (byte) 0 ); /* HSCSD Sub-field is not present. */
    csn_encode_bit(encoder, (byte) 1 ); /* Sub-field is present. */
    csn_encode_bits(encoder, 5, (byte) gmm_set_multislot_class (gmm_multislot_class_sim[sub_id]) );

#ifdef FEATURE_GPRS_EDA
    if ( gmm_nv_eda_feature_support )
      csn_encode_bit(encoder, (byte) 1 ); /* GPRS Ext Dyn Alloc Cap */
    else
      csn_encode_bit(encoder, (byte) 0 ); /* GPRS Ext Dyn Alloc Cap */
      
    MSG_HIGH_DS_3(MM_LOCAL_SUB, "=MM= Acc Tech %d: GPRS Class = %d with EDA = %d",
      access_technology, gmm_multislot_class_sim[sub_id], gmm_nv_eda_feature_support);
#else
    csn_encode_bit(encoder, (byte) 0 ); /* GPRS Ext Dyn Alloc Cap */
    
    MSG_HIGH_DS_2(MM_LOCAL_SUB, "=MM= Acc Tech %d: GPRS Class = %d",
      access_technology, gmm_multislot_class_sim[sub_id]);
      
#endif /* FEATURE_GPRS_EDA */

    /*  sms_sm value is not supported by NAS*/
    csn_encode_bit(encoder, (byte) 0 ); /* Sub-field is not included. */


    if (ms_revision_level == UE_REV_REL99)
    {
      csn_encode_bit(encoder, (byte) 0 ); /* ECSD Field is Absent */
#ifdef FEATURE_GSM_EGPRS
      /* If EDGE is enabled in the phone, set the values accordingly */
      if (gmm_edge_feature_support)
      {
          csn_encode_bit(encoder, (byte) 1 ); /* EGPRS M-slot Field is Present */
          csn_encode_bits(encoder, 5, (byte) gmm_set_multislot_class (gmm_edge_multislot_class_sim[sub_id]) );

#ifdef FEATURE_GPRS_EDA
          if ( gmm_nv_eda_feature_support )
            csn_encode_bit(encoder, (byte) 1 ); /* GPRS Ext Dyn Alloc Cap */
          else
            csn_encode_bit(encoder, (byte) 0 ); /* GPRS Ext Dyn Alloc Cap */

          MSG_HIGH_DS_2(MM_LOCAL_SUB, "=MM= EGPRS Class = %d with EDA = %d",
            gmm_edge_multislot_class_sim[sub_id], gmm_nv_eda_feature_support);            
#else
          csn_encode_bit(encoder, (byte) 0 ); /* GPRS Ext Dyn Alloc Cap */
          
          MSG_HIGH_DS_1(MM_LOCAL_SUB, "=MM= EGPRS Class = %d", gmm_edge_multislot_class_sim[sub_id]);

#endif /* FEATURE_GPRS_EDA */
      }
      else
      {
         csn_encode_bit(encoder, (byte) 0 ); /* EGPRS M-slot Field is Absent */
         MSG_HIGH_DS_0(MM_LOCAL_SUB, "=MM= EGPRS Disabled");
      }
#else
      csn_encode_bit(encoder, (byte) 0 ); /* EGPRS M-slot Field is Absent */
#endif
#ifdef FEATURE_GSM_DTM
      if ( gmm_nv_dtm_feature_support_sim[sub_id] )
      {      
        csn_encode_bit(encoder, (byte) 1 ); /* DTM M-slot Field is Present */
        csn_encode_bits(encoder, 2, (byte) gmm_nv_dtm_multislot_subclass_sim[sub_id]); 
        csn_encode_bit(encoder, (byte) 0 ); /* single slot dtm not supported */
#ifdef FEATURE_GSM_EGPRS
        if (gmm_edge_feature_support_sim[sub_id])
        {
          csn_encode_bit(encoder, (byte) 1 ); /* EGPRS DTM supported */
          csn_encode_bits(encoder, 2, (byte) gmm_nv_dtm_multislot_subclass_sim[sub_id]);
        }
        else
#endif /* FEATURE_GSM_EGPRS */
        {
          csn_encode_bit(encoder, (byte) 0 ); /* EGPRS DTM not supported */
        }
        MSG_HIGH_DS_2(MM_LOCAL_SUB, "=MM= Acc Tech %d: DTM Multislot Sub Class = %d", access_technology,
          gmm_nv_dtm_multislot_subclass_sim[sub_id]);
      }
      else
      {
        csn_encode_bit(encoder, (byte) 0 ); /* DTM M-slot Field is Absent */
        MSG_HIGH_DS_0(MM_LOCAL_SUB, "=MM= DTM Disabled");
      }
#else
      csn_encode_bit(encoder, (byte) 0 ); /* DTM M-slot Field is Absent */
#endif /* FEATURE_GSM_DTM */
    }
  }
  /* Revision level is set to 1 for Rel 99 and onwards */
  if (ms_revision_level == UE_REV_REL99)
  {
#ifdef FEATURE_GSM_EGPRS
      /*
      * 8PSK power capability
      */
      /* Check the NV item to see if EDGE is enabled. If enabled, fill the power
      ** capability values based on the support for 8PSK modulation in uplink
      */
      if (gmm_edge_feature_support_sim[sub_id])
      {
         /* 8PSK Power Class indicates whether or not 8PSK mod in uplink is
         ** supported by the phone. Based on that, the power class shall be set.
         */
         if (gmm_edge_8psk_power_class_sim[sub_id] != 0)
         {
             /* 8PSK modulation in uplink supported */
             csn_encode_bit(encoder, (byte) 1 );
             csn_encode_bits(encoder, 2, gmm_edge_8psk_power_capability_sim[sub_id]); /* Power Class supported by the phone */
         }
         else
         {
             MSG_HIGH_DS_0(MM_LOCAL_SUB, "=MM= EGPRS UL 8PSK Disabled");
             csn_encode_bit(encoder, (byte) 0 );
         }
      }
      else
      {
          /* EDGE not supported, hence ignore support for 8PSK */
          csn_encode_bit(encoder, (byte) 0 );
      }
#else
      /* No EGPRS in the stack. Disable 8PSK by default */
      csn_encode_bit(encoder, (byte) 0 );
#endif

    /* COMPACT interference measurement caps */
    csn_encode_bit(encoder, (byte) compact_interference_caps);

    csn_encode_bit(encoder, (byte) ms_revision_level);

NAS_ENTER_CRIT_SECT(mm_rat_prioritylist_crit_sec);    
    /*
    ** UE preferred mode is GSM Only, set UMTS FDD as FALSE.
    */
#ifndef FEATURE_RAT_PRIORITY_LIST
    if ((mm_ue_pref_rat_sim[as_id] == SYS_RAT_GSM_RADIO_ACCESS) && 
        (mm_dual_mode_sim[as_id] == FALSE))
#else
    /* Report UMTS capability accordingly
    ** UMTS FDD Radio Access Technology Capability field per 3GPP 24.008 sect 10.5.1.7
    */
    if ((!mm_checkif_rat_pri_list_has_rat(&mm_rat_pri_list_info_sim[as_id],SYS_RAT_UMTS_RADIO_ACCESS))
#ifdef FEATURE_SGLTE
        || MM_IS_IN_SGLTE_STATE_4
#endif
       )
#endif /* FEATURE_RAT_PRIORITY_LIST */
    {
      csn_encode_bit(encoder, (byte) 0);
    }
    else
    {
      /* Regardless if TDSCDMA is present in the rat prio list, report WCDMA if present */
      csn_encode_bit(encoder, (byte) UMTS_FDD_RAT_supported);
    }
NAS_EXIT_CRIT_SECT(mm_rat_prioritylist_crit_sec);

    csn_encode_bit(encoder, (byte) UMTS_TDD_RAT_supported);
    csn_encode_bit(encoder, (byte) CDMA2000_RAT_supported);

    /* Rel - 4 IE's start here */

#ifdef FEATURE_REL_4_SUPPORT

    NAS_ENTER_CRIT_SECT(mm_rat_prioritylist_crit_sec);   
 
    /* UMTS 1.28 Mcps TDD Radio Access Technology Capability field */
#ifdef FEATURE_RAT_PRIORITY_LIST
    if((!mm_checkif_rat_pri_list_has_rat(&mm_rat_pri_list_info_sim[as_id],SYS_RAT_TDS_RADIO_ACCESS))
#ifdef FEATURE_SGLTE
        || MM_IS_IN_SGLTE_STATE_4
#endif
       )
    {
       csn_encode_bit(encoder, (byte) 0 );
    }
    else if (!mm_checkif_rat_pri_list_has_rat(&mm_rat_pri_list_info_sim[as_id],SYS_RAT_UMTS_RADIO_ACCESS))
    {  /* If TDSCDMA is present and WCDMA is not, report TDSCMA */
      csn_encode_bit(encoder, (byte) 1);
    }   
    else
#endif
    {
    csn_encode_bit(encoder, (byte) 0 );
    }

    NAS_EXIT_CRIT_SECT(mm_rat_prioritylist_crit_sec);

    /* Support GERAN release pack 1 */
#ifdef FEATURE_GSM_GERAN_FEATURE_PACK_1
    csn_encode_bit(encoder, (byte) gmm_edge_nw_ass_cell_change_sim[sub_id]);
#else
    csn_encode_bit(encoder, (byte) 0 );
#endif /* FEATURE_GSM_GERAN_FEATURE_PACK_1 */

    /* Extended DTM GPRS/EGPRS multislot class - NOT supported */
    csn_encode_bit(encoder, (byte) 0 );

    /* Modulation based multislot class support (1 bit field) - NOT Supported */
    csn_encode_bit(encoder, (byte) 0 );

#endif /* FEATURE_REL_4_SUPPORT */

#if defined(FEATURE_REL5) || \
    (!defined(FEATURE_WCDMA) && defined(FEATURE_REL_6_SUPPORT))
   #ifndef FEATURE_REL_4_SUPPORT
     #error FEATURE_REL5 requires FEATURE_REL_4_SUPPORT
   #endif

    /* High Multislot Capability */
#if defined (FEATURE_GSM_GPRS_MSC34) && defined (FEATURE_GSM_GPRS_MSC34_SUPPORTED)
    if ( hmsc_already_included == TRUE )   
    {
      csn_encode_bit(encoder, (byte) 0 );
    }
    else
    { 
#ifdef FEATURE_GSM_EGPRS
      /* If EDGE is enabled in the phone, set the values accordingly */
      if (gmm_edge_feature_support_sim[sub_id])
      {
        if ((gmm_edge_multislot_class_sim[sub_id] == (byte)30) || (gmm_edge_multislot_class_sim[sub_id] == (byte)32) || (gmm_edge_multislot_class_sim[sub_id] == (byte)33))
        {
          csn_encode_bit(encoder, (byte) 1 );
          csn_encode_bits(encoder, 2, (byte) 0 );
        }
        else if ((gmm_edge_multislot_class_sim[sub_id] == (byte)31) || (gmm_edge_multislot_class_sim[sub_id] == (byte)34))
        { 
          csn_encode_bit(encoder, (byte) 1 );
          csn_encode_bits(encoder, 2, (byte) 3 );          
        }
        else
        {
          csn_encode_bit(encoder, (byte) 0 );       
        }
      }
      else
#endif /* FEATURE_GSM_EGPRS */
      {
        if ((gmm_multislot_class_sim[sub_id] == (byte)30) || (gmm_multislot_class_sim[sub_id] == (byte)32) || (gmm_multislot_class_sim[sub_id] == (byte)33))
        {
          csn_encode_bit(encoder, (byte) 1 );
          csn_encode_bits(encoder, 2, (byte) 0 );
        }
        else if ((gmm_multislot_class_sim[sub_id] == (byte)31) ||(gmm_multislot_class_sim[sub_id] == (byte)34))
        { 
          csn_encode_bit(encoder, (byte) 1 );
          csn_encode_bits(encoder, 2, (byte) 3 );          
        }
        else
        {
          csn_encode_bit(encoder, (byte) 0 );       
        }
      }
    }
#else
    csn_encode_bit(encoder, (byte) 0 );
#endif /* FEATURE_GSM_GPRS_MSC34 */

    /* GERAN Iu Mode Capability  - Not Supported ? */
#ifdef FEATURE_GSM_GERAN_FEATURE_PACK_2
    csn_encode_bit(encoder, (byte) 1 );
#else
    csn_encode_bit(encoder, (byte) 0 );
#endif

    /* GMSK Multislot Power Profile - Need support from RR */
    csn_encode_bits(encoder,2,(byte) gmm_nv_multislot_gmsk_power_profile_sim[sub_id]);

    /* 8-PSK Multislot Power Profile - Need support from RR */
    csn_encode_bits(encoder,2,(byte) gmm_nv_multislot_8psk_power_profile_sim[sub_id]);

#endif /* FEATURE_REL5 */

#ifdef FEATURE_REL_6_SUPPORT  
    #if !defined (FEATURE_REL_4_SUPPORT) || \
        (defined(FEATURE_WCDMA) && !defined(FEATURE_REL5))
      #error FEATURE_REL_6_SUPPORT requires FEATURE_REL_4_SUPPORT and FEATURE_REL5
    #endif

    /* Multiple TBF capability - Not supported */
    csn_encode_bit(encoder, (byte) 0 );

    /* SAIC/DARP Capability */
#if (defined(FEATURE_GSM_EGPRS_SAIC) && defined(FEATURE_GSM_MDSP_AEQ))
    csn_encode_bits(encoder,2,gmm_nv_saic_feature_support);
#else
    /* not available as feature in this build */
    csn_encode_bits(encoder,2,0); 
#endif /* FEATURE_GSM_EGPRS_SAIC */

#if defined(FEATURE_GSM_DTM) && defined(FEATURE_GSM_EDTM)
    /* Enhanced DTM (EDTM)and Extended RLC/MAC segmentation */
    csn_encode_bit(encoder, (byte) gmm_nv_edtm_feature_support_sim[sub_id] );
    csn_encode_bit(encoder, (byte) gmm_nv_edtm_feature_support_sim[sub_id] );
#else
        csn_encode_bits(encoder, 2, 0);
#endif

    /* DTM GPRS High Multi Slot Class - Not supported */
    csn_encode_bit(encoder, (byte) 0 );

#ifdef FEATURE_GPRS_PS_HANDOVER
    if (gmm_nv_psho_feature_support == TRUE)
    {
      /* PS Handover capability -supported */
      csn_encode_bit(encoder, (byte) 1 );
    }
    else
    {
      csn_encode_bit(encoder, (byte) 0 );
    }
#else
    /* PS Handover capability - Not supported */
    csn_encode_bit(encoder, (byte) 0 );
#endif
    
#endif /* FEATURE_REL_6_SUPPORT */
#ifdef FEATURE_LTE
   /**** REL 7 Additions - Not Supported ***/

    /*DTM Handover Capability - Not supported*/
    TRUNCATE_RAC (max_rac_length, 1, encoder, access_caps_length_bit_pos)
    csn_encode_bit(encoder, (byte) 0 ); 

    /*Multislot Capability Reduction for Downlink Dual Carrier - Not Supported*/
    TRUNCATE_RAC (max_rac_length, 1, encoder, access_caps_length_bit_pos)
    csn_encode_bit(encoder, (byte) 0 ); 

    /*Flexible Timeslot Assignment - Not Supported*/   
    TRUNCATE_RAC (max_rac_length, 1, encoder, access_caps_length_bit_pos)
    csn_encode_bit(encoder, (byte) 0 ); 

    /*GAN PS Handover Capability - Not Supported*/
    TRUNCATE_RAC (max_rac_length, 1, encoder, access_caps_length_bit_pos)
    csn_encode_bit(encoder, (byte) 0 );   

    /*RLC Non-persistent Mode - Not Supported*/
    TRUNCATE_RAC (max_rac_length, 1, encoder, access_caps_length_bit_pos)
    csn_encode_bit(encoder, (byte) 0 );

    /*Reduced Latency Capability - Not Supported*/
    TRUNCATE_RAC (max_rac_length, 1, encoder, access_caps_length_bit_pos)
    csn_encode_bit(encoder, (byte) 0 );

    /*Uplink EGPRS2 - Not Supported*/
    TRUNCATE_RAC (max_rac_length, 2, encoder, access_caps_length_bit_pos)
    csn_encode_bits(encoder, 2, 0);

    /*Downlink EGPRS2  - Not Supported*/
    TRUNCATE_RAC (max_rac_length, 2, encoder, access_caps_length_bit_pos)
    csn_encode_bits(encoder, 2, 0);

    /*****     REL8 Features      **/
    TRUNCATE_RAC (max_rac_length, 1, encoder, access_caps_length_bit_pos)
    /*E-UTRA FDD support is true only if Rat priority list contains the lte rat */
    if (mm_checkif_rat_pri_list_has_rat(&mm_rat_pri_list_info_sim[as_id], SYS_RAT_LTE_RADIO_ACCESS) &&
         lte_rrc_lte_fdd_is_supported(mm_get_lte_band_capacity(&mm_rat_pri_list_info_sim[as_id])) 
#ifdef FEATURE_SGLTE
         && !MM_IS_IN_SGLTE_STATE_4
#endif
        )
    {
      /* E-UTRA FDD support - Supported*/
      csn_encode_bit(encoder, (byte) 1 );
      eutra_FDD_TDD_support = TRUE ;
    }
    else
    {
      /* E-UTRA FDD support - Not Supported*/
      csn_encode_bit(encoder, (byte) 0 );
    }

    /*E-UTRA TDD support is true only if Rat priority list contains the lte rat */
    TRUNCATE_RAC (max_rac_length, 1, encoder, access_caps_length_bit_pos)
    if (mm_checkif_rat_pri_list_has_rat(&mm_rat_pri_list_info_sim[as_id], SYS_RAT_LTE_RADIO_ACCESS)&&
        lte_rrc_lte_tdd_is_supported(mm_get_lte_band_capacity(&mm_rat_pri_list_info_sim[as_id])) 
#ifdef FEATURE_SGLTE
         && !MM_IS_IN_SGLTE_STATE_4
#endif
        )
    {
      /* E-UTRA TDD support - Supported*/
      csn_encode_bit(encoder, (byte) 1 );
      eutra_FDD_TDD_support = TRUE ;
    }
    else
    {
      /* E-UTRA TDD support - Not Supported*/
      csn_encode_bit(encoder, (byte) 0 );  
    }
      
    /*GERAN to E-UTRA support in GERAN packet transfer mode --    
      0 0 - Not supported    */
    TRUNCATE_RAC (max_rac_length, 2, encoder, access_caps_length_bit_pos)
    if(eutra_FDD_TDD_support == TRUE)
    {
      /*Setting two bits as per efs value read*/
      csn_encode_bits(encoder, 2, gmm_nv_geran_to_eutra_measurement_support_sim[sub_id]);
    }
    else 
    {
   /* If both "E-UTRA FDD support" and "E-UTRA TDD support" bits are set to '0', 
                                             the bit field shall be set to '0 0'*/
      csn_encode_bits(encoder, 2, 0);
    }


    /* Priority-based reselection - Supported*/
    TRUNCATE_RAC (max_rac_length, 1, encoder, access_caps_length_bit_pos)
     if (
#ifdef FEATURE_SGLTE
         (!MM_IS_IN_SGLTE_STATE_4) &&
#endif
         (mm_checkif_rat_pri_list_has_rat(&mm_rat_pri_list_info,SYS_RAT_LTE_RADIO_ACCESS) ||
          mm_checkif_rat_pri_list_has_rat(&mm_rat_pri_list_info,SYS_RAT_UMTS_RADIO_ACCESS) || 
          mm_checkif_rat_pri_list_has_rat(&mm_rat_pri_list_info,SYS_RAT_TDS_RADIO_ACCESS)) &&
         (mm_checkif_rat_pri_list_has_rat(&mm_rat_pri_list_info,SYS_RAT_GSM_RADIO_ACCESS)))
	 {
           csn_encode_bit(encoder, (byte) 1 );
	 }
	else
        {       
          csn_encode_bit(encoder, (byte) 0 );
        }

#endif


    TRUNCATE_RAC (max_rac_length, 1, encoder, access_caps_length_bit_pos)
    /* Fill enhanced flexible timeslot assignment */
    csn_encode_bit(encoder, 0);

    TRUNCATE_RAC (max_rac_length, 1, encoder, access_caps_length_bit_pos)
    /* Indication of Upper Layer PDU Start Capability for RLC UM  */
    csn_encode_bit(encoder, 0);

    TRUNCATE_RAC (max_rac_length, 1, encoder, access_caps_length_bit_pos)    
    /* EMST Capability  */
    csn_encode_bit(encoder, 0);

    TRUNCATE_RAC (max_rac_length, 1, encoder, access_caps_length_bit_pos)    
    /* MTTI capability */
    csn_encode_bit(encoder, 0);

    TRUNCATE_RAC (max_rac_length, 1, encoder, access_caps_length_bit_pos)
    /* UTRA CSG Cells Reporting  */
    csn_encode_bit(encoder, 0);

    TRUNCATE_RAC (max_rac_length, 1, encoder, access_caps_length_bit_pos)
    /* E-UTRA CSG Cells Reporting  */
    csn_encode_bit(encoder, 0);

    TRUNCATE_RAC (max_rac_length, 1, encoder, access_caps_length_bit_pos)
    /* DTR Capability  */
    csn_encode_bit(encoder, 0);

    TRUNCATE_RAC (max_rac_length, 1, encoder, access_caps_length_bit_pos)
    /* EMSR Capability  */
    csn_encode_bit(encoder, 0);

    TRUNCATE_RAC (max_rac_length, 1, encoder, access_caps_length_bit_pos)
    /* Fast Downlink Frequency Switching Capability */
    csn_encode_bit(encoder, 0);

    TRUNCATE_RAC (max_rac_length, 2, encoder, access_caps_length_bit_pos)    
    /* Tighter cap */
    csn_encode_bits(encoder, 2, mm_nv_tighter_capability);

  }

  /* Now figure out how big is the access caps structure including length */
  length = csn_bit_pos(encoder) - access_caps_length_bit_pos;

  /* Length is in units of BITS and won't include its own size (7 bits) */
  length -= 7;

  /* Write this computed access caps length at the length element */
  csn_encode_bits_at_pos(encoder, 7, (byte) length, access_caps_length_bit_pos);

  return FALSE;
}
#ifdef FEATURE_NON_DEMAND_PAGED_FUNCTION
#error code not present
#endif

#ifdef FEATURE_NON_DEMAND_PAGED_FUNCTION
#error code not present
#endif
/*===========================================================================

FUNCTION set_access_tech_info_per_subs

DESCRIPTION
  This function derives the stack_id_nas from the sub_id passed to NAS and
  calls set_access_tech_info_per_nas_stacks to get Access Capability Structure of the MS RAC IE

DEPENDENCIES
  None

RETURN VALUE
  TRUE -- RAC truncated
  FALSE -- RAC not truncated
SIDE EFFECTS
  None

===========================================================================*/
boolean set_access_tech_info_per_subs
(
  csn_encoder_T  *encoder ,
  byte           access_technology ,
  byte           revision_level_ind,
  byte           rf_power_class,
  hw_classmark_T *hw_classmark,
  boolean        *A5_already_included,
  boolean        *gprs_msclass_already_included,
  word           max_rac_length, /*0 means no length specified*/
  sys_modem_as_id_e_type sub_id
)
{
  sys_modem_as_id_e_type stack_id_nas = sub_id;
  if (IS_NOT_VALID_SUB_ID(sub_id))
  {
    MSG_ERROR_DS_1(MM_SUB,"set_access_tech_info_per_nas_stacks - Invalid AS ID: %d", sub_id+1);
    return FALSE;
  }

  /*Map  as_id to stack_id*/
#if defined(FEATURE_SGLTE)
  if(MM_SUB_IS_SGLTE_SUB(sub_id))
  {
    stack_id_nas = (sys_modem_as_id_e_type)mm_ps_stack_id;
  }
#endif

  return set_access_tech_info_per_nas_stacks(encoder,access_technology,revision_level_ind,rf_power_class,
            hw_classmark,A5_already_included,gprs_msclass_already_included,max_rac_length,sub_id,stack_id_nas);
}
#ifdef FEATURE_NON_DEMAND_PAGED_FUNCTION
#error code not present
#endif

#endif



/*===========================================================================

FUNCTION GET_MS_RADIO_ACCESS_CAPABILITY

DESCRIPTION
  This function constructs MS Radio Access capability IE using the classmark
  information

  The MS Radio Access Capability IE can be built either for use in a GMM
  OTA message, or for use in a GPRS RLC/MAC Packet Resource Request message
  (as indicated by a gprs_pkt_res_req_ms_ra_cap parameter of TRUE).

DEPENDENCIES
  None

RETURN VALUE
  MS RAT IE structure of type gmm_ie_LV_ms_radio_access_caps_T.

  Note that if the gprs_pkt_res_req_ms_ra_cap parameter is TRUE then the
  length field of the returned gmm_ie_LV_ms_radio_access_caps_T structure
  type is given in units of BITS, otherwise in units of BYTES.

SIDE EFFECTS
  None

===========================================================================*/

gmm_ie_LV_ms_radio_access_caps_T get_ms_radio_access_capability
(
  boolean gprs_pkt_res_req_ms_ra_cap
)
{
  gmm_ie_LV_ms_radio_access_caps_T rac;
  hw_classmark_T                   hw_classmark;
  ghdi_status_T                    status;
  csn_encoder_T                    encoder;
  ue_rev_level_indicator_type      revision_level_ind;
  boolean                          A5_already_included           = FALSE ;
  boolean                          gprs_msclass_already_included = FALSE ;

#ifdef FEATURE_GSM
  boolean                          DCS1800_supported;
  boolean                          PCS1900_supported;
  boolean                          PGSM_supported;
  boolean                          EGSM_supported;
  boolean                          GSM_850_supported;
  byte                             rf_power_class_pgsm = 0;
  byte                             rf_power_class_egsm = 0;
  byte                             rf_power_class_dcs = 0;
  byte                             rf_power_class_pcs = 0;
  byte                             rf_power_class_gsm_850 = 0;
#endif /* #ifdef FEATURE_GSM */
#ifdef FEATURE_RAT_PRIORITY_LIST
  sys_band_mask_e_type  band_cap = SYS_BAND_MASK_EMPTY;
#endif
#if defined(FEATURE_GSM) && defined(FEATURE_GSM_MULTIBAND)
  mb_initialise();
#endif /* #if defined(FEATURE_GSM) && defined(FEATURE_GSM_MULTIBAND) */

  memset((void*)rac.value, 0, sizeof(rac.value));

  status = ghdi_read_hw_classmark( &hw_classmark );
  if (status != GHDI_SUCCESS)
  {
    MSG_ERROR_DS_1(MM_SUB, "=MM= ERROR: Unable to read classmark: status=%d",status);
  }

  /* Init CSN encoder */
  csn_encoder_init(&encoder, rac.value, sizeof(rac.value));

  /* Get current Revision Level Indicator */
#if defined FEATURE_DUAL_SIM || defined FEATURE_SGLTE
  revision_level_ind = get_revision_level_indicator_per_nas_stacks((sys_modem_as_id_e_type)mm_as_id) ;
#else
  revision_level_ind = get_revision_level_indicator() ;
#endif

#ifdef FEATURE_GSM

#ifdef FEATURE_GSM_MULTIBAND
  if (mm_gsm_supported)
  {
#ifdef FEATURE_DUAL_SIM
    PGSM_supported    = rr_ds_is_supported_band(BAND_PGSM,(sys_modem_as_id_e_type)mm_sub_id) ;
    EGSM_supported    = rr_ds_is_supported_band(BAND_EGSM,(sys_modem_as_id_e_type)mm_sub_id) ;
    DCS1800_supported = rr_ds_is_supported_band(BAND_DCS,(sys_modem_as_id_e_type)mm_sub_id)  ;
    PCS1900_supported = rr_ds_is_supported_band(BAND_PCS,(sys_modem_as_id_e_type)mm_sub_id)  ;
    GSM_850_supported = rr_ds_is_supported_band(BAND_GSM_850,(sys_modem_as_id_e_type)mm_sub_id)  ;
#else
    PGSM_supported    = rr_is_supported_band(BAND_PGSM) ;
    EGSM_supported    = rr_is_supported_band(BAND_EGSM) ;
    DCS1800_supported = rr_is_supported_band(BAND_DCS)  ;
    PCS1900_supported = rr_is_supported_band(BAND_PCS)  ;
    GSM_850_supported = rr_is_supported_band(BAND_GSM_850)  ;
#endif
    
#ifdef FEATURE_DUAL_SIM
    /* get the power class values */
    if (PCS1900_supported)
    {
      rf_power_class_pcs = rr_ds_get_band_rf_power_class(BAND_PCS,(sys_modem_as_id_e_type)mm_sub_id);
    }

    if (DCS1800_supported)
    {
      rf_power_class_dcs = rr_ds_get_band_rf_power_class(BAND_DCS,(sys_modem_as_id_e_type)mm_sub_id);
    }

    if (EGSM_supported)
    {
      rf_power_class_egsm = rr_ds_get_band_rf_power_class(BAND_EGSM,(sys_modem_as_id_e_type)mm_sub_id);
    }
    else
    {
      rf_power_class_pgsm = rr_ds_get_band_rf_power_class(BAND_PGSM,(sys_modem_as_id_e_type)mm_sub_id);

    }

    if (GSM_850_supported)
    {
      rf_power_class_gsm_850 = rr_ds_get_band_rf_power_class(BAND_GSM_850,(sys_modem_as_id_e_type)mm_sub_id);
    }
#else
    /* get the power class values */
    if (PCS1900_supported)
    {
      rf_power_class_pcs = rr_get_band_rf_power_class(BAND_PCS);
    }

    if (DCS1800_supported)
    {
      rf_power_class_dcs = rr_get_band_rf_power_class(BAND_DCS);
    }

    if (EGSM_supported)
    {
      rf_power_class_egsm = rr_get_band_rf_power_class(BAND_EGSM);
    }
    else
    {
      rf_power_class_pgsm = rr_get_band_rf_power_class(BAND_PGSM);

    }

    if (GSM_850_supported)
    {
      rf_power_class_gsm_850 = rr_get_band_rf_power_class(BAND_GSM_850);
    }
#endif /*FEATURE_DUAL_SIM || FEATURE_SGLTE*/
  }
  else /* GSM is not supported */
  {
    /* code all as false */
    PCS1900_supported = FALSE;
    DCS1800_supported = FALSE;
    PGSM_supported    = FALSE;
    EGSM_supported    = FALSE;
    GSM_850_supported = FALSE;
  }
#else /* multiband not supported */
  #error code not present
#endif /* FEATURE_GSM_MULTIBAND */

  if (!(PCS1900_supported || DCS1800_supported
                     || PGSM_supported || EGSM_supported || GSM_850_supported))
  {
    MSG_ERROR_DS_0(MM_SUB, "=MM= There is no supported access technology") ;
  }

  /*
  ** According to the TS 24.008 Table 10.5.146, if both EGSM and
  ** PGSM are supported, the EGSM definitions cover PGSM and PGSM
  ** info can be excluded from the MS RAC IE
  */
  if (EGSM_supported)
  {
    PGSM_supported = FALSE ;
  }

  /*
     Spec 24.008 sec 10.5.5.12a
     Due to shared radio frequency channel numbers between GSM 1800
     and GSM 1900, the mobile station should provide the relevant
     radio access capability for either GSM 1800 band OR GSM 1900 band, not both.
  */

  if (DCS1800_supported && PCS1900_supported)
  {
    /* Both 1800 & 1900 bands should not be set in mm_band_pref.
       Incase of none of them set give preferance to 1800 in RAC
    */
    if ((is_gsm_mode()) && (mm_band_indicator) )
    {
#ifndef FEATURE_RAT_PRIORITY_LIST
      if ((mm_band_pref & SYS_BM(SYS_SBAND_GSM_PCS_1900)) && 
#else
      band_cap = mm_get_band_capacity(&mm_rat_pri_list_info,SYS_SYS_MODE_GSM); 
      if ((band_cap & SYS_BM(SYS_SBAND_GSM_PCS_1900)) && 
#endif /* FEATURE_RAT_PRIORITY_LIST */
          (mm_band_indicator == SYS_BAND_GSM_PCS_1900))
      {
        DCS1800_supported = FALSE;
      }
      else
      {
        PCS1900_supported = FALSE;
      }
    }
    else if (is_umts_mode() || (!mm_band_indicator) 
#ifdef FEATURE_LTE
                || (is_lte_mode())
#endif
               )
    {  
#ifdef FEATURE_GSM_BAND_PREF
      if (mm_active_band & SYS_BM(SYS_SBAND_GSM_PCS_1900))        
      {        
        DCS1800_supported = FALSE;   
      }
      else if (mm_active_band & SYS_BM(SYS_SBAND_GSM_DCS_1800))
      {
        PCS1900_supported = FALSE;
      }
      else      
#endif    
#ifndef FEATURE_RAT_PRIORITY_LIST
      if (mm_band_pref & SYS_BM(SYS_SBAND_GSM_PCS_1900))
#else
      band_cap= mm_get_band_capacity(&mm_rat_pri_list_info,SYS_SYS_MODE_GSM); 
      if (band_cap & SYS_BM(SYS_SBAND_GSM_PCS_1900))
#endif /* FEATURE_RAT_PRIORITY_LIST */ 
      {
        DCS1800_supported = FALSE;
      }
      else
      {
        PCS1900_supported = FALSE;
      }
    }
  }

  /* Set PGSM description */
  if (PGSM_supported)
  {
#if defined FEATURE_DUAL_SIM || defined FEATURE_SGLTE
    (void) set_access_tech_info_per_nas_stacks(&encoder,
       (byte)ACCESS_TECH_GSM_P,(byte)revision_level_ind,rf_power_class_pgsm,
       &hw_classmark, &A5_already_included, &gprs_msclass_already_included
       ,0
       ,(sys_modem_as_id_e_type)mm_sub_id
       ,(sys_modem_as_id_e_type)mm_as_id
                        ) ;
#else
    set_access_tech_info(&encoder,
       (byte)ACCESS_TECH_GSM_P,(byte)revision_level_ind,rf_power_class_pgsm,
       &hw_classmark, &A5_already_included, &gprs_msclass_already_included
       ,0
                        ) ;
#endif

    /* -----------------------------------------------------------------------
    ** Check for Multiband capability, if present set the Further MS Radio
    ** Access Capability Elements.
    ** ---------------------------------------------------------------------*/
    if (GSM_850_supported || DCS1800_supported || PCS1900_supported)
    {
      csn_encode_bit(&encoder, (byte) 1 );
    }
  }

  /* Set EGSM description */
  if (EGSM_supported)
  {
#if defined FEATURE_DUAL_SIM || defined FEATURE_SGLTE
    (void) set_access_tech_info_per_nas_stacks(&encoder,
       (byte)ACCESS_TECH_GSM_E,(byte)revision_level_ind,rf_power_class_egsm,
       &hw_classmark, &A5_already_included, &gprs_msclass_already_included
       ,0
       ,(sys_modem_as_id_e_type)mm_sub_id
       ,(sys_modem_as_id_e_type)mm_as_id
                        ) ;       
#else
    set_access_tech_info(&encoder,
       (byte)ACCESS_TECH_GSM_E,(byte)revision_level_ind,rf_power_class_egsm,
       &hw_classmark, &A5_already_included, &gprs_msclass_already_included
       ,0
                        ) ;       
#endif

    /* -----------------------------------------------------------------------
    ** Check for Multiband capability, if present set the Further MS Radio
    ** Access Capability Elements.
    ** ---------------------------------------------------------------------*/
    if (GSM_850_supported || DCS1800_supported || PCS1900_supported)
    {
      csn_encode_bit(&encoder, (byte) 1 );
    }
  }

  if (GSM_850_supported)
  {
#if defined FEATURE_DUAL_SIM || defined FEATURE_SGLTE
    (void) set_access_tech_info_per_nas_stacks(&encoder,
       (byte)ACCESS_TECH_GSM_850,(byte)revision_level_ind,rf_power_class_gsm_850,
       &hw_classmark, &A5_already_included, &gprs_msclass_already_included
       ,0
       ,(sys_modem_as_id_e_type)mm_sub_id
       ,(sys_modem_as_id_e_type)mm_as_id
                        ) ;       

#else

    set_access_tech_info(&encoder,
       (byte)ACCESS_TECH_GSM_850,(byte)revision_level_ind,rf_power_class_gsm_850,
       &hw_classmark, &A5_already_included, &gprs_msclass_already_included
       ,0
                        ) ;       
#endif

    /* -----------------------------------------------------------------------
    ** Check for Multiband capability, if present set the Further MS Radio
    ** Access Capability Elements.
    ** ---------------------------------------------------------------------*/
    if (DCS1800_supported || PCS1900_supported)
    {
      csn_encode_bit(&encoder, (byte) 1 );
    }
  }

  /* Set DCS (GSM 1800) description */
  if (DCS1800_supported)
  {
#if defined FEATURE_DUAL_SIM || defined FEATURE_SGLTE
    (void) set_access_tech_info_per_nas_stacks(&encoder,
       (byte)ACCESS_TECH_GSM_1800,(byte)revision_level_ind,rf_power_class_dcs,
       &hw_classmark, &A5_already_included, &gprs_msclass_already_included
       ,0
       ,(sys_modem_as_id_e_type)mm_sub_id
       ,(sys_modem_as_id_e_type)mm_as_id
                        ) ;       
#else
    set_access_tech_info(&encoder,
       (byte)ACCESS_TECH_GSM_1800,(byte)revision_level_ind,rf_power_class_dcs,
       &hw_classmark, &A5_already_included, &gprs_msclass_already_included
       ,0
                        ) ;       
#endif

    /* -----------------------------------------------------------------------
    ** Check for Multiband capability, if present set the Further MS Radio
    ** Access Capability Elements.
    ** ---------------------------------------------------------------------*/
    if (PCS1900_supported)
    {
      csn_encode_bit(&encoder, (byte) 1 );
    }
  }

  /* Set PCS (GSM 1900) description */
  if (PCS1900_supported)
  {
#if defined FEATURE_DUAL_SIM || defined FEATURE_SGLTE
    (void) set_access_tech_info_per_nas_stacks(&encoder,
       (byte)ACCESS_TECH_GSM_1900,(byte)revision_level_ind,rf_power_class_pcs,
       &hw_classmark, &A5_already_included, &gprs_msclass_already_included
       ,0
       ,(sys_modem_as_id_e_type)mm_sub_id
       ,(sys_modem_as_id_e_type)mm_as_id
                        ) ;       
#else
    set_access_tech_info(&encoder,
       (byte)ACCESS_TECH_GSM_1900,(byte)revision_level_ind,rf_power_class_pcs,
       &hw_classmark, &A5_already_included, &gprs_msclass_already_included
       ,0
                        ) ;       
#endif
  }

#else /* #ifdef FEATURE_GSM */

#if defined FEATURE_DUAL_SIM || defined FEATURE_SGLTE
    (void) set_access_tech_info_per_nas_stacks(&encoder,
       (byte)ACCESS_TECH_GSM_E,(byte)revision_level_ind,0,
       &hw_classmark, &A5_already_included, &gprs_msclass_already_included
       ,0
       ,(sys_modem_as_id_e_type)mm_sub_id
       ,(sys_modem_as_id_e_type)mm_as_id
                        ) ;       

#else

  set_access_tech_info(&encoder,
       (byte)ACCESS_TECH_GSM_E,(byte)revision_level_ind,0,
       &hw_classmark, &A5_already_included, &gprs_msclass_already_included
       ,0
                        ) ;       
#endif

#endif /* #ifdef FEATURE_GSM */

  /* Further MS Radio Access Capability elements not present. */
  csn_encode_bit(&encoder, (byte) 0 );

  if ( gprs_pkt_res_req_ms_ra_cap == TRUE )
  {
    /* The IE is being constructed for use in a GPRS RLC/MAC Packet
       Resource Request message. In this case the bit_pos is bit
       aligned and length is set in units of BITS. No spare bits
       must be present. */

    rac.length = csn_bit_pos(&encoder);
  }
  else
  {
    /* The IE is being constructed for use in GMM OTA messaging.
       In this case the bit_pos must be byte aligned and spare
       bits filled with 0. Length is set in units of BYTES */

    csn_fill_spare_bits(&encoder);
    rac.length = (csn_bit_pos(&encoder) / 8);
  }

  return rac;
} /* end get_ms_network_capability( boolean ) */


/*===========================================================================

FUNCTION GET_DRX_PARAMETER

DESCRIPTION
  This function constructs a DRX Parameter IE.

DEPENDENCIES
  None

RETURN VALUE
  DRX Parameter IE

SIDE EFFECTS
  None

===========================================================================*/

gmm_ie_TV_drx_parameter_T get_drx_parameter( void )
{
  gmm_ie_TV_drx_parameter_T dummy;

  dummy.IEI      = GMM_IEI_DRX_PARAMETER;
  dummy.value[0] = gmm_split_paging_cycle;

#ifndef FEATURE_LTE
  dummy.value[1] = gmm_non_drx_timer & 0x0F;
#else
  dummy.value[1] = (gmm_non_drx_timer & 0x0F) | (gmm_drx_cn_coeff_s1 << 4);
#endif /*FEATURE_LTE*/

  MSG_HIGH_DS_2(MM_SUB, "=MM= Split Pg Cycle Code = 0x%x, Non DRX Tmr Code/CN_COEFF_S1 = 0x%x",
            dummy.value[0], dummy.value[1]);

  return dummy;

} /* end get_drx_parameter ( void ) */


/*===========================================================================

FUNCTION SET_REQUESTED_READY_TIMER

DESCRIPTION
  This function constructs Requested Ready Timer IE at the location given
  by the message pointer, and advances the given message index accordingly.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void set_requested_ready_timer( byte *msg_p, byte *msg_index )
{
  *msg_p++ = GMM_IEI_READY_TIMER_VALUE;
  *msg_p   = GMM_REQ_READY_TIMER_VALUE_SECS / 2;
  *msg_index += 2;

} /* end set_requested_ready_timer() */


/*===========================================================================

FUNCTION GMM_GET_SUPPORTED_CODEC_LIST

DESCRIPTION
  This function constructs MS Radio Access capability IE using the classmark
  information

  The MS Radio Access Capability IE can be built either for use in a GMM
  OTA message, or for use in a GPRS RLC/MAC Packet Resource Request message
  (as indicated by a gprs_pkt_res_req_ms_ra_cap parameter of TRUE).

DEPENDENCIES
  None

RETURN VALUE
  MS RAT IE structure of type gmm_ie_LV_ms_radio_access_caps_T.

  Note that if the gprs_pkt_res_req_ms_ra_cap parameter is TRUE then the
  length field of the returned gmm_ie_LV_ms_radio_access_caps_T structure
  type is given in units of BITS, otherwise in units of BYTES.

SIDE EFFECTS
  None

===========================================================================*/

cm_supported_codecs_list_T gmm_get_supported_codec_list( void )
{

  cm_supported_codecs_list_T supported_codec_list;

  memset(&supported_codec_list, 0, sizeof(cm_supported_codecs_list_T));

  supported_codec_list.present = TRUE;
  supported_codec_list.length = 2;

  /* SysID = UMTS */ 
  supported_codec_list.codec_list[0].sysID = 
                                                                    SYSID_UMTS;
  supported_codec_list.codec_list[0].bitmap_length = 
                                                                    0x02; /* bitmap length */
  supported_codec_list.codec_list[0].bitmap_1_8 =
                                                                    0x60; /* bitmap 8...1 */
  supported_codec_list.codec_list[0].bitmap_9_16 = 
                                                                    0x00; /* bitmap 16...9 */
#ifdef FEATURE_VOC_AMR_WB
  if( (gsm_wcdma_amr_codec_preference & 0x08) )
  {
    supported_codec_list.codec_list[0].bitmap_9_16 = 
                                                                    0x04; /* bitmap 16...9 */
  }
#endif  /*end of FEATURE_VOC_AMR_WB */


  /* SysID = GSM */
  supported_codec_list.codec_list[1].sysID = 
                                                                    SYSID_GSM;
  supported_codec_list.codec_list[1].bitmap_length = 
                                                                    0x02; /* bitmap length */

/* GSM AMR codecs should be included conditionally depending on the 
NV Item - NV_GSM_AMR_CALL_CONFIG_I */          
  
  supported_codec_list.codec_list[1].bitmap_1_8 = 
                                                                    0x07; /* bitmap 8...1 */
  if( (gsm_wcdma_amr_codec_preference & 0x01) )
  {
    /*GSM AMR NB is supported*/
    supported_codec_list.codec_list[1].bitmap_1_8 |= 
                                                                    0x08; /* bitmap 8...1 */
  }
  if( (gsm_wcdma_amr_codec_preference & 0x04) )
  {
    /*GSM AMR NB is supported*/
    supported_codec_list.codec_list[1].bitmap_1_8 |=
                                                                     0x10; /* bitmap 8...1 */
  }

/*GSM AMR WB is NOT supported*/
supported_codec_list.codec_list[1].bitmap_9_16 = 
                                                                     0x00; /* bitmap 16...9 */ 

#ifdef FEATURE_GSM_AMR_WB
  if( (gsm_wcdma_amr_codec_preference & 0x02) )
  {
    /*GSM AMR WB is supported*/
    supported_codec_list.codec_list[1].bitmap_9_16 = 
                                                                     0x02; /* bitmap 16...9 */
  }
#endif

  return supported_codec_list;

}



/*===========================================================================

FUNCTION GMM_SEND_ATTACH_REQUEST_MESSAGE

DESCRIPTION

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void gmm_send_attach_request_message
(
  gmm_attach_type_type attach_type,
  auth_security_context_id cksn_context
)
{
  gmm_attach_request_msg_T         attach_request;
  byte                             msg_index;
  gmm_ie_LV_ms_network_caps_T      ms_network_capability;
  gmm_ie_LV_ms_radio_access_caps_T ms_radio_access_capability;
  gmm_ie_TV_drx_parameter_T        drx_parameter;
  boolean                          ptmsi_present;
  boolean                          ptmsi_signature_present;
  boolean                          tmsi_present;
  byte                             i;
  byte                             attach_type_to_log;
  byte                             length;


#if defined(FEATURE_LTE)
  boolean is_lte_supported = FALSE;
  lte_nas_emm_ue_capability_type ue_capability;
  emm_ctrl_data_type *emm_ctrl_data_ptr = NULL;
  mm_gw_identity_param_type gw_identity;
  byte            ptmsi_type_iei_value = 0;
#endif
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  byte   max_data_size =   sizeof(gmm_ie_LV_ms_network_caps_T)
                + 1     /* Attach type & GMM CKSN */
                + 2     /* DRX param */
                + sizeof(gmm_ie_LV_mobile_id_T)
                + 6     /* Old Routing area identification */
                + sizeof(gmm_ie_LV_ms_radio_access_caps_T)
                + 4     /* Old PTMSI signature */
                + 2     /* GMM Timer */
                + 1/*TMSI status */
                +15 /* UE network capability*/
                + 7  /*Additional Mobile Identity*/
                + 8  /*Additional Old RAI */
                + 3 /*Voice domain preference and UE's usage setting */
                + 1 /*PTMSI Type*/
                + 7 /*Old LAI*/ 
#if defined FEATURE_NAS_REL11
                + 1  /*Network support feature */
                + 4; /*Tmsi based NRI container */
#else                
                + 1; /*Network support feature */  
#endif

  memset(&attach_request, 0x00, sizeof(gmm_attach_request_msg_T));

  attach_request.msg_head.skip_pd = PD_GMM;
  attach_request.msg_head.type    = GMM_ATTACH_REQUEST;

  msg_index = 0;

  attach_type_to_log = attach_type;
  ptmsi_present = FALSE;
  ptmsi_signature_present = FALSE;
#if defined(FEATURE_LTE)
  is_lte_supported = (mm_checkif_rat_pri_list_has_rat(&mm_rat_pri_list_info,
                                                                   SYS_RAT_LTE_RADIO_ACCESS)
#ifdef FEATURE_SGLTE
                        && !MM_IS_IN_SGLTE_STATE_4
#endif
                     );
  emm_ctrl_data_ptr = emm_db_get_ctrl_data();
/* If the MS supports S1 mode and
 * 1. If the TIN indicates "GUTI" and the MS holds a valid GUTI, the MS shall use the mapped
 *     identities from GUTI.
 * 2. If the TIN indicates "P-TMSI" or "RAT-related TMSI" and the MS holds a valid P-TMSI 
 *     and a RAI, the MS shall use the native identities.
 * 3. If the TIN is deleted and 
 *      a) the MS holds a valid P-TMSI and a RAI, the MS shall use the native identities. 
 *      b) otherwise, if the MS holds a valid GUTI, the MS shall use the mapped identities from GUTI
 * 4. Otherwise the MS shall include the IMSI in the Old GUTI or IMSI IE.
 */
  if(is_lte_supported)
  {
    MSG_HIGH_DS_3(MM_SUB, "=MM= TIN value %d, GUTI valid %d, PTMSI valid %d", emm_ctrl_data_ptr->TIN,
                   emm_ctrl_data_ptr->emm_guti_valid, mm_has_valid_ptmsi());
    if((emm_ctrl_data_ptr->TIN == NAS_MM_TIN_GUTI &&
        emm_ctrl_data_ptr->emm_guti_valid == TRUE ))
    {
      emm_get_mapped_gw_identity(&gw_identity, emm_ctrl_data_ptr);
      ptmsi_present = TRUE;
      ptmsi_signature_present = TRUE;
      ptmsi_type_iei_value = 1; 
    }
    else if(((emm_ctrl_data_ptr->TIN == NAS_MM_TIN_P_TMSI ||
               emm_ctrl_data_ptr->TIN == NAS_MM_TIN_RAT_RELATED_TMSI) &&
              mm_has_valid_ptmsi() && has_valid_rai()))
    {
      mm_get_native_gw_identity(&gw_identity,cksn_context);
      ptmsi_present = TRUE;
      ptmsi_signature_present = mm_has_valid_ptmsi_signature();
      ptmsi_type_iei_value = 0;
    }
    else if(((emm_ctrl_data_ptr->TIN == NAS_MM_TIN_NONE)
            && mm_has_valid_ptmsi() && has_valid_rai()))
    {
      mm_get_native_gw_identity(&gw_identity,cksn_context);
      ptmsi_present = TRUE;
      ptmsi_signature_present = mm_has_valid_ptmsi_signature();
      ptmsi_type_iei_value = 0;
    }
    else if((emm_ctrl_data_ptr->TIN == NAS_MM_TIN_NONE
              && emm_ctrl_data_ptr->emm_guti_valid == TRUE ))
    {
      emm_get_mapped_gw_identity(&gw_identity, emm_ctrl_data_ptr);
      ptmsi_present = TRUE;
      ptmsi_signature_present = TRUE;
      ptmsi_type_iei_value = 1;
    }
    else
    {
      mm_get_native_gw_identity(&gw_identity,cksn_context);
      ptmsi_signature_present = mm_has_valid_ptmsi_signature();
    }
  }
  else
  {
    mm_get_native_gw_identity(&gw_identity,cksn_context);
    ptmsi_present = mm_has_valid_ptmsi();
    ptmsi_signature_present = mm_has_valid_ptmsi_signature();
    ptmsi_type_iei_value = 0;
  }
#else
  ptmsi_present = mm_has_valid_ptmsi();
  ptmsi_signature_present = mm_has_valid_ptmsi_signature();
#endif

  /* -------------------------------
  ** Set MS network capability field
  ** ------------------------------- */
  ms_network_capability = get_ms_network_capability();
  if(ms_network_capability.length > sizeof(ms_network_capability.value))
  {
    ms_network_capability.length = 8;
    MSG_HIGH_DS_0(MM_SUB, "=MM= truncating ms_network_capability.length");
  }
  attach_request.data[msg_index] = ms_network_capability.length;
  msg_index = msg_index + 1;
  if(msg_index+ms_network_capability.length <= max_data_size )
  {
    for (i=0; i < ms_network_capability.length; i++)
    {
      attach_request.data[msg_index+i] = ms_network_capability.value[i];
    }
  }
  msg_index = msg_index + ms_network_capability.length;

  /* ---------------------
  ** Set Attach type field
  ** --------------------- */
#ifdef FEATURE_REL6_PS_FOP
  gmm_ps_fop_req = FALSE;
#endif

  if ((is_umts_mode()) && ((gmm_follow_on_request) || 
#ifdef FEATURE_DUAL_SIM
  (!gmm_attach_request_on_network_reject && sm_is_pdp_activation_pending((sys_modem_as_id_e_type)mm_sub_id))
#else
  (!gmm_attach_request_on_network_reject && sm_is_pdp_activation_pending())
#endif
  ))
  {
    /* -------------------------------------------------
    ** Follow-on request (bit 4) applicable only in UMTS
    ** ------------------------------------------------- */
    attach_type = attach_type | 0x08;
#ifdef FEATURE_REL6_PS_FOP
    gmm_ps_fop_req = TRUE;
#endif
  }
  gmm_attach_request_on_network_reject = FALSE;
  
  if(msg_index < max_data_size)
  {
    attach_request.data[msg_index] = attach_type;
  }

  /* --------------------------------------------
  ** Set GPRS ciphering key sequence number field
  ** -------------------------------------------- */
#if defined(FEATURE_LTE)
  /* during the GPRS Attach  there is no method of generating
  * a UMTS security context from an EPS security context, so hardcoding this value
  * to 0x07 i.e. no key available.
  */
  if(msg_index < max_data_size)
  {
    attach_request.data[msg_index] = (byte)(((gw_identity.eKSI) << 4) + attach_request.data[msg_index]);
  } 
#else
  /* For now hard code field to 0x07 which indicates no key is available
     (see TS 24.008 10.5.1.2) */
  if(msg_index < max_data_size)
  {
    attach_request.data[msg_index] =
      (byte)(auth_get_ciphering_key_sequence_number(cksn_context) << 4) +
                                                 attach_request.data[msg_index] ;
  }
#endif
  msg_index = msg_index + 1;

  /* -------------
  ** DRX Parameter
  ** ------------- */
  drx_parameter = get_drx_parameter();
  if(msg_index+1 < max_data_size)
  {
    attach_request.data[msg_index]   = drx_parameter.value[0];
    attach_request.data[msg_index+1] = drx_parameter.value[1];
  }
  msg_index = msg_index + 2;

  /* -------------------------------------------------------------------------
  ** Set P-TMSI or IMSI field - first determine if we are using P-TMSI or IMSI
  ** as the mobile identity
  ** ------------------------------------------------------------------------- */

  if (ptmsi_present)
  {
    /* -----------------------------------------------------------------------------
    ** Use PTMSI as mobile_identity - first set length of mobility identity contents
    ** ----------------------------------------------------------------------------- */
    if(msg_index+1 < max_data_size)
    {
      attach_request.data[msg_index] = TMSI_SIZE + 1;

      /* ------------------------------------------------
      ** Set Identity type field (TS 24.008 Table 10.5.4)
      ** ------------------------------------------------ */
      attach_request.data[msg_index+1] = 0xF4;

    }
    msg_index = msg_index + 2;

    for (i=0; i < PTMSI_SIZE; i++)
    {
      if(msg_index+i < max_data_size)
      {
#if defined(FEATURE_LTE)
        attach_request.data[msg_index+i] = gw_identity.ptmsi.digit[i] ;
#else
        attach_request.data[msg_index+i] = gmm_stored_ptmsi.digit[i];
#endif
      }
    }
    msg_index = msg_index + PTMSI_SIZE;
  }
  else
  {
    /* ---------------------------
    ** Use IMSI as mobile_identity
    ** --------------------------- */
    length = MIN (mm_stored_imsi.length, IMSI_LEN_IN_BYTES);
    if(msg_index < max_data_size)
    {
      attach_request.data[msg_index] = length;
    }
    msg_index = msg_index + 1;

    for (i=0; i < length; i++)
    {
      if(msg_index+i < max_data_size)
      {
        attach_request.data[msg_index+i] = mm_stored_imsi.data[i];
      }
    }
    msg_index = msg_index + length;
  }

  /* -----------------------------------------
  ** Set Old routing area identification field
  ** ----------------------------------------- */
  for (i=0; i < PLMN_SIZE; i++)
  {
    if(msg_index+i < max_data_size)
    {
#if defined(FEATURE_LTE)
      attach_request.data[msg_index+i] = gw_identity.ra_id.plmn_id.identity[i];
#else
      attach_request.data[msg_index+i] = gmm_stored_rai.plmn_id.identity[i];
#endif
    }
  }
  msg_index = msg_index + PLMN_SIZE;

  for (i=0; i < LAC_SIZE; i++)
  {
    if(msg_index+i < max_data_size)
    {
#if defined(FEATURE_LTE)
      attach_request.data[msg_index+i] = gw_identity.ra_id.location_area_code.lac[i];
#else
      attach_request.data[msg_index+i] = gmm_stored_rai.location_area_code.lac[i];
#endif
    }
  }
  msg_index = msg_index + LAC_SIZE;
  if(msg_index < max_data_size)
  {
#if defined(FEATURE_LTE)
    attach_request.data[msg_index] = gw_identity.ra_id.routing_area_code;
#else
    attach_request.data[msg_index] = gmm_stored_rai.routing_area_code;
#endif

  }
  msg_index = msg_index + 1;

  /* ------------------------------------
  ** Set MS radio access capability field
  ** ------------------------------------ */
  ms_radio_access_capability = get_ms_radio_access_capability( FALSE );
  ms_radio_access_capability.length = MIN(ms_radio_access_capability.length, sizeof(ms_radio_access_capability.value));
  if(msg_index < max_data_size)
  {
    attach_request.data[msg_index] = (byte) ms_radio_access_capability.length;
  }

  msg_index = msg_index + 1;

  for (i=0; i < ms_radio_access_capability.length; i++)
  {
    if(msg_index+i < max_data_size)
    {
      attach_request.data[msg_index+i] = ms_radio_access_capability.value[i];
    }
  }
  msg_index = msg_index + (byte) ms_radio_access_capability.length;

  if (ptmsi_signature_present)
  {
    if(msg_index < max_data_size)
    {
      attach_request.data[msg_index] = GMM_IEI_PTMSI_SIGNATURE;
    }
    msg_index = msg_index + 1;

    for (i=0; i < PTMSI_SIGNATURE_SIZE; i++)
    {
      if(msg_index+i < max_data_size)
      {
#if defined(FEATURE_LTE)
        attach_request.data[msg_index+i] = gw_identity.ptmsi_signature.value[i];
#else
        attach_request.data[msg_index+i] = gmm_stored_ptmsi_signature.value[i];
#endif
      }
    } /* end for (i=0; i < PTMSI_SIGNATURE_SIZE; i++) */

    msg_index = msg_index + PTMSI_SIGNATURE_SIZE;
  } /* end if (ptmsi_signature_present) */
  /* ------------------------------------------------------
  ** Requested READY timer value field
  ** ------------------------------------------------------ */
  if((msg_index +1) < max_data_size)
  {
    set_requested_ready_timer( &attach_request.data[msg_index], &msg_index );
  }


  /* ---------------------
  ** Set TMSI status field
  ** --------------------- */
  if ((attach_type & 0x07) == GMM_COMBINED_GPRS_IMSI_ATTACH)
  {
    tmsi_present = FALSE;

    for (i=0; i < TMSI_SIZE; i++)
    {
      if (mm_location_information.tmsi[i] != 0xFF)
      {
        tmsi_present = TRUE;
        break;
      }
    } /* end for (i=0; i < PTMSI_SIZE; i++) */

    if (!tmsi_present)
    {
      if(msg_index < max_data_size)
      {
        attach_request.data[msg_index] = (GMM_IEI_TMSI_STATUS << 4) | tmsi_present;
      }
      msg_index += 1;
    }
  }
#if defined(FEATURE_LTE)
    /* -----------------------
    ** UE NETWORK CAPABILITY
    ** ---------------------  */
  if ((mm_checkif_rat_pri_list_has_rat(&mm_rat_pri_list_info, SYS_RAT_LTE_RADIO_ACCESS) 

#ifdef FEATURE_SGLTE
                            &&     MM_IS_SGLTE_ON_PS_STACK_ID && !MM_IS_IN_SGLTE_STATE_4)
#else
                                    )
#endif

  )
  {
    memset(&ue_capability, 0x0, sizeof(ue_capability));
    
    memscpy( (void *)&ue_capability.ue_sec_capability, sizeof(ue_capability.ue_sec_capability),
                   (void *) &emm_ctrl_data_ptr->ue_security_capability,
                   sizeof(lte_nas_emm_ue_security_type));
    
    emm_update_gw_sec_capabilities(emm_ctrl_data_ptr,
                                                              &ue_capability);
    if(msg_index + 1 < max_data_size)
    {
      attach_request.data[msg_index] = GMM_IEI_UE_NW_CAPABILITY;
    }
    msg_index += 1;
    if(msg_index + MAX_SIZE_OF_UE_NE_CAPABILITY_IE < max_data_size)
    {
      encode_ue_nw_cap_ie(&ue_capability,&msg_index,attach_request.data);
    }
  }

  if((emm_ctrl_data_ptr->TIN == NAS_MM_TIN_GUTI &&
      emm_ctrl_data_ptr->emm_guti_valid == TRUE &&
      is_lte_supported == TRUE ))
  {
    //Fill the additional identities:
    if(mm_has_valid_ptmsi() && has_valid_rai())
    {
      /* ----------------
      ** Set Additional P-TMSI field
      ** ---------------- */
      if(msg_index+1 < max_data_size)
      {
        attach_request.data[msg_index]   = GMM_ADDITIONAL_MOBILE_IDENTITY;
        attach_request.data[msg_index+1] = TMSI_SIZE + 1;
      }
  
      /* ------------------------------------------------
      ** Set Identity type field (TS 24.008 Table 10.5.4)
      ** ------------------------------------------------ */
      if(msg_index+2 < max_data_size)
      {
        attach_request.data[msg_index+2] = 0xF4;
      }
  
      msg_index = msg_index + 3;
  
      for (i=0; i < PTMSI_SIZE; i++)
      {
        if((msg_index+i) < max_data_size)
        {
          attach_request.data[msg_index+i] = gmm_stored_ptmsi.digit[i];
        }
      }  
      msg_index = msg_index + PTMSI_SIZE;          
    
      /*Set Additional OLD RAI data */
      if(msg_index < max_data_size)
      {
        attach_request.data[msg_index] = GMM_ADDITIONAL_OLD_RAI;
      }
      msg_index = msg_index + 1;
      
      
      if(msg_index < max_data_size)
      {
        attach_request.data[msg_index] = PLMN_SIZE + LAC_SIZE + sizeof(routing_area_code_T);
      }
      msg_index = msg_index + 1;
        
      for (i=0; i < PLMN_SIZE; i++)
      {
        if((msg_index+i) < max_data_size)
        {
          attach_request.data[msg_index+i] = gmm_stored_rai.plmn_id.identity[i];
        }  
      }
      msg_index = msg_index + PLMN_SIZE; 
  
      for (i=0; i < LAC_SIZE; i++)
      {
        if((msg_index+i) < max_data_size)
        {
          attach_request.data[msg_index+i] = gmm_stored_rai.location_area_code.lac[i];
        }  
      }
      msg_index = msg_index + LAC_SIZE; 
      
      if(msg_index < max_data_size)
      {
        attach_request.data[msg_index] = gmm_stored_rai.routing_area_code;
      }        
      msg_index = msg_index + 1; 
    }
  }
  
#endif
#if defined(FEATURE_LTE)  && defined(FEATURE_LTE_REL9)
  if((emm_ctrl_data_ptr->emm_3gpp_spec_ver >= LTE_3GPP_REL9 ))
  {
    if ((is_lte_supported == TRUE ) &&
        (mmcoord_pending_reg_message.cmd.mmr_reg_req.lte_cs_domain_param.is_voice_domain_ie_valid == TRUE))
    {
      if(msg_index+2 < max_data_size)
      {
        attach_request.data[msg_index]   = GMM_VOICE_DOMAIN_PREF_IEI;
        attach_request.data[msg_index+1] = 1; //length of the VOICE DOMAIN PREF IE
        attach_request.data[msg_index+2] = 
         (((byte)(mmcoord_pending_reg_message.cmd.mmr_reg_req.lte_cs_domain_param.voice_domain_pref) & 0x03) |
         ((((byte)mmcoord_pending_reg_message.cmd.mmr_reg_req.lte_cs_domain_param.voice_or_data_centric) & 0x01) << 0x02));
               
        msg_index = msg_index + 3; 
      }
    }
  }
#endif
#ifdef FEATURE_LTE
  /* ------------------------------------------------
  ** Set PTMSI type field 
  ** ------------------------------------------------ */ 
  if((emm_ctrl_data_ptr->exclude_identity_type_ie != TRUE ) )
  {
    if(ptmsi_present)
    {
      if(msg_index < max_data_size)
      {
        attach_request.data[msg_index] = (byte)GMM_IEI_PTMSI_TYPE | ptmsi_type_iei_value;
      }
      msg_index = msg_index + 1;
    }
  }
#endif

#ifdef FEATURE_NAS_REL10
    /* ------------------------------------------------
    ** Set Network_support_feature IE 
    ** ------------------------------------------------ */ 
    if(mm_nv_nas_rel_compliance >= NV_NAS_COMPLIANCE_REL10)
    { 
      if(msg_index < max_data_size)
      {
        attach_request.data[msg_index] = (byte)(GMM_IEI_UL_NW_FTR_SUPPORT) | 0x01;
      }
      msg_index = msg_index + 1;
    }
#endif

#ifdef FEATURE_LTE
  /*--------------------------------------------
  **Set Old location area identification
  **-------------------------------------------*/
  if(((mm_nv_exclude_old_lai_type_ie != TRUE) &&
     (is_lte_supported == TRUE)  && 
     (((attach_type & 0x07) == GMM_COMBINED_GPRS_IMSI_ATTACH) || 
     ((attach_type & 0x07) == GMM_GPRS_ATTACH_WHILE_IMSI_ATTACHED)) && 
     mm_has_valid_lai() ))
  {
    if(msg_index < max_data_size)
    {
      attach_request.data[msg_index] = GMM_IEI_OLD_LAI;
    }
    msg_index = msg_index + 1;    
    
    if(msg_index < max_data_size)
    {
      attach_request.data[msg_index] = LAI_SIZE;
    }
    msg_index = msg_index + 1;
    
    for (i=0; i < LAI_SIZE; i++)
    {
      if((msg_index+i) < max_data_size)
      {
        attach_request.data[msg_index+i] = mm_location_information.lai[i];
      }  
    }
    msg_index = msg_index + LAI_SIZE;     
  }
#endif
/** Spec 24.008 4.7.3.2.1:
       If the MS has stored a valid TMSI, the MS shall include the TMSI based NRI container IE in the COMBINED ATTACH REQUEST message
  **==============================================================================**/
#ifdef FEATURE_NAS_REL11
  if(mm_nv_nas_rel_compliance >= NV_NAS_COMPLIANCE_REL11)
  {
    if(has_valid_tmsi() && 
       ((gmm_attach_type == GMM_COMBINED_GPRS_IMSI_ATTACH) ||
        (gmm_attach_type == GMM_GPRS_ATTACH_WHILE_IMSI_ATTACHED)))
    {
      if(msg_index < max_data_size)
      {
        attach_request.data[msg_index] = GMM_IEI_TMSI_BASED_NRI_CONTAINER;// IEI on first byte
      }
      msg_index = msg_index + 1;
      if(msg_index < max_data_size) 
      {
        attach_request.data[msg_index] = GMM_IEI_TMSI_BASED_NRI_CONTAINER_CONTENT_LENGTH;//NRI_CONTENT_LENGTH 
      }
      msg_index = msg_index + 1;
      if(msg_index < max_data_size)
      {
        attach_request.data[msg_index] = mm_location_information.tmsi[1];
      // bit 23 to 16 of tmsi
      }
      msg_index = msg_index + 1;
      if(msg_index < max_data_size)
      {
        attach_request.data[msg_index] = (mm_location_information.tmsi[2] & 0xC0);
        // bit 15 , 14 of tmsi and rest are spare (0)
      }
      msg_index = msg_index + 1;
    }
  }
#endif

  if(msg_index > max_data_size)
  {
    MSG_ERROR_DS_0(MM_SUB,"=MM= Index out of bounds and buffer overflow");
    return;
  }
  /* ---------------------------------------
  ** Set Attach Request Message length field
  ** --------------------------------------- */
  PUT_IMH_LEN(msg_index + 2, &attach_request.msg_head.IMH);

  MSG_HIGH_DS_0( MM_SUB, "=MM= Sending ATTACH REQUEST message");

  mmsend_request_reject_event( TRUE, SMGMM_EVENT_ATTACH_REQUEST,
                               attach_type_to_log);

  mmrrcconn_send_L3_message_to_rrc(
    &attach_request, RRC_EST_REGISTRATION, RRC_PS_DOMAIN_CN_ID );

} /* end gmm_send_attach_request_message() */


/*===========================================================================

FUNCTION GMM_SEND_ATTACH_COMPLETE_MESSAGE

DESCRIPTION

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void gmm_send_attach_complete_message( void )
{
  gmm_attach_complete_msg_T gmm_attach_complete_msg;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  memset(&gmm_attach_complete_msg, 0x00, sizeof(gmm_attach_complete_msg_T));

  gmm_attach_complete_msg.msg_head.skip_pd = PD_GMM;
  gmm_attach_complete_msg.msg_head.type    = GMM_ATTACH_COMPLETE;

  gmm_attach_complete_msg.msg_head.IMH.message_len_lsb = 2;
  gmm_attach_complete_msg.msg_head.IMH.message_len_msb = 0;

  MSG_HIGH_DS_0( MM_SUB, "=MM= Sending ATTACH COMPLETE message" );

  mmrrcconn_send_L3_message_to_rrc(
    &gmm_attach_complete_msg, RRC_EST_LOW_PRI_SIGNALLING, RRC_PS_DOMAIN_CN_ID );

} /* end gmm_send_attach_complete_message() */



/*===========================================================================

FUNCTION GMM_SEND_MO_DETACH_REQUEST_MESSAGE

DESCRIPTION

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void gmm_send_mo_detach_request_message
(
  gmm_detach_type_type detach_type,
  boolean              power_down
)
{
  gmm_mo_detach_request_msg_T      detach_request;
  byte                             msg_length;
  boolean                          ptmsi_signature_present;
  boolean                          ptmsi_present;
  byte                             i;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  memset(&detach_request, 0x00, sizeof(gmm_mo_detach_request_msg_T));

  detach_request.msg_head.skip_pd = PD_GMM;
  detach_request.msg_head.type    = GMM_DETACH_REQUEST;

  /* ---------------------
  ** Set Detach type field
  ** --------------------- */
  detach_request.detach_type_spare = detach_type;

  if (power_down)
  {
    /* ---------------------------
    ** Set power off bit indicator
    ** --------------------------- */
    detach_request.detach_type_spare = detach_request.detach_type_spare | 0x08;
  }

  /* Minimal mandatory message length (in octets) */
  msg_length = 3 ;

  /*
  ** MS initiated DETACH REQUEST message must not contain P-TMSI and
  ** P-TMSI signature if the network indicates R'97
  */
#if defined FEATURE_DUAL_SIM || defined FEATURE_SGLTE
  if (get_revision_level_indicator_per_nas_stacks((sys_modem_as_id_e_type)mm_as_id) != UE_REV_PRE_REL99)
#else
  if (get_revision_level_indicator() != UE_REV_PRE_REL99)
#endif
  {
    /* ----------------------------------------------------------------------
    ** Set P-TMSI field (if a valid P-TMSI is stored)
    ** ---------------------------------------------------------------------- */
    ptmsi_present = FALSE;

    for (i=0; i < PTMSI_SIZE; i++)
    {
      if (gmm_stored_ptmsi.digit[i] != 0xFF)
      {
        ptmsi_present = TRUE;
        break;
      }
    } /* end for (i=0; i < PTMSI_SIGNATURE_SIZE; i++) */

    if (ptmsi_present)
    {
      /* ----------------
      ** Set P-TMSI field
      ** ---------------- */
      detach_request.ptmsi[0] = GMM_IEI_PTMSI;
      detach_request.ptmsi[1] = PTMSI_SIZE + 1;

      /* ------------------------------------------------
      ** Set Identity type field (TS 24.008 Table 10.5.4)
      ** ------------------------------------------------ */
      detach_request.ptmsi[2] = 0xF4;

      for (i=0; i < PTMSI_SIZE; i++)
      {
        detach_request.ptmsi[3+i] = gmm_stored_ptmsi.digit[i];
      }

      msg_length = 10;

      /* ----------------------------------------------------------------------
      ** Set Old P-TMSI signature field (if a valid P-TMSI signature is stored)
      ** ---------------------------------------------------------------------- */
      ptmsi_signature_present = FALSE;

      for (i=0; i < PTMSI_SIGNATURE_SIZE; i++)
      {
        if (gmm_stored_ptmsi_signature.value[i] != 0xFF)
        {
          ptmsi_signature_present = TRUE;
          break;
        }
      } /* end for (i=0; i < PTMSI_SIGNATURE_SIZE; i++) */

      if (ptmsi_signature_present)
      {

        detach_request.ptmsi_signature[0] = GMM_IEI_PTMSI_SIGNATURE;
        detach_request.ptmsi_signature[1] = PTMSI_SIGNATURE_SIZE;

        for (i=0; i < PTMSI_SIGNATURE_SIZE; i++)
        {
          detach_request.ptmsi_signature[2+i] =
                                          gmm_stored_ptmsi_signature.value[i] ;
        } /* end for (i=0; i < PTMSI_SIGNATURE_SIZE; i++) */

        msg_length = msg_length + 5;

        gmm_ptmsi_sig_tobe_deleted = TRUE ;

      } /* end if (ptmsi_signature_present) */
    }
  } /* if (get_revision_level_indicator()) - R'99 or R5 */

  /* ---------------------------------------
  ** Set Attach Request Message length field
  ** --------------------------------------- */
  PUT_IMH_LEN(msg_length,&detach_request.msg_head.IMH);

  MSG_HIGH_DS_0( MM_SUB, "=MM= Sending DETACH REQUEST message");

  mmsend_request_reject_event(TRUE, SMGMM_EVENT_DETACH_REQUEST,
                              detach_type);

  mmrrcconn_send_L3_message_to_rrc( &detach_request, RRC_EST_DETACH, RRC_PS_DOMAIN_CN_ID );

} /* end gmm_send_mo_detach_request_message() */


/*===========================================================================

FUNCTION GMM_SEND_MT_DETACH_ACCEPT_MESSAGE

DESCRIPTION

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void gmm_send_mt_detach_accept_message( void )
{
  gmm_mt_detach_accept_msg_T detach_accept;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  memset(&detach_accept, 0x00, sizeof(gmm_mt_detach_accept_msg_T));

  detach_accept.msg_head.skip_pd = PD_GMM;
  detach_accept.msg_head.type    = GMM_DETACH_ACCEPT;

  detach_accept.msg_head.IMH.message_len_lsb = 2;
  detach_accept.msg_head.IMH.message_len_msb = 0;

  MSG_HIGH_DS_0( MM_SUB, "=MM= Sending DETACH ACCEPT message");

  mmrrcconn_send_L3_message_to_rrc(
    &detach_accept, RRC_EST_LOW_PRI_SIGNALLING, RRC_PS_DOMAIN_CN_ID );

} /* end gmm_send_mo_detach_request_message() */


/*===========================================================================

FUNCTION GMM_SEND_PTMSI_REALLOCATION_COMPLETE_MESSAGE

DESCRIPTION

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void gmm_send_ptmsi_reallocation_complete_message( void )
{
  gmm_ptmsi_realloc_complete_msg_T ptmsi_reallocation_complete;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  memset(&ptmsi_reallocation_complete, 0x00, sizeof(gmm_ptmsi_realloc_complete_msg_T));

  ptmsi_reallocation_complete.msg_head.skip_pd = PD_GMM;
  ptmsi_reallocation_complete.msg_head.type    = GMM_PTMSI_REALLOCATION_COMPLETE;

  ptmsi_reallocation_complete.msg_head.IMH.message_len_lsb = 2;
  ptmsi_reallocation_complete.msg_head.IMH.message_len_msb = 0;

  MSG_HIGH_DS_0( MM_SUB, "=MM= Sending PTMSI REALLOCATION COMPLETE message");

  mmrrcconn_send_L3_message_to_rrc(
    &ptmsi_reallocation_complete, RRC_EST_LOW_PRI_SIGNALLING, RRC_PS_DOMAIN_CN_ID );

} /* end gmm_send_ptmsi_reallocation_complete_message() */


/*===========================================================================

FUNCTION GMM_SEND_IDENTITY_RESPONSE_MESSAGE

DESCRIPTION

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void gmm_send_identity_response_message
(
  gmm_ie_LV_resp_mobile_id_T mobile_identity
)
{
  gmm_identity_response_msg_T identity_response_msg;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  memset(&identity_response_msg, 0x00, sizeof(gmm_identity_response_msg_T));

  identity_response_msg.msg_head.skip_pd = PD_GMM;
  identity_response_msg.msg_head.type    = GMM_IDENTITY_RESPONSE;

  identity_response_msg.mobile_identity.length = mobile_identity.length;
  memscpy(identity_response_msg.mobile_identity.value, sizeof(identity_response_msg.mobile_identity.value), mobile_identity.value,
         sizeof(mobile_identity.value));
  PUT_IMH_LEN((3 + mobile_identity.length),&identity_response_msg.msg_head.IMH);

  MSG_HIGH_DS_0( MM_SUB, "=MM= Sending IDENTITY RESPONSE message");

  mmrrcconn_send_L3_message_to_rrc(
    &identity_response_msg, RRC_EST_LOW_PRI_SIGNALLING, RRC_PS_DOMAIN_CN_ID );

} /* end gmm_send_identity_response_message() */


/*===========================================================================

FUNCTION GMM_SEND_ROUTING_AREA_UPDATE_REQUEST_MESSAGE

DESCRIPTION

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void gmm_send_routing_area_update_request_message
(
  gmm_ra_update_type_type ra_update_type,
  auth_security_context_id cksn_context
)
{
  gmm_ra_update_request_msg_T      ra_update_request;
  byte                             msg_index;
  gmm_ie_LV_ms_radio_access_caps_T ms_radio_access_capability;
  boolean                          ptmsi_signature_present;
  gmm_ie_TV_drx_parameter_T        drx_parameter;
  boolean                          tmsi_present;
  gmm_ie_LV_ms_network_caps_T      ms_network_capability;
  gmm_ie_pdp_context_status_T      pdp_context_status;
  byte                             i;
  byte                             update_type_to_log;

#ifdef FEATURE_MBMS  
  #error code not present
#endif

#if defined(FEATURE_LTE)
   emm_ctrl_data_type *emm_ctrl_data_ptr = NULL;
   lte_nas_emm_ue_capability_type ue_capability;
   boolean is_lte_supported = FALSE;
   byte ptmsi_type_iei_value = 0;
   mm_gw_identity_param_type mm_identity;
#endif
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  byte max_index_size             = sizeof(gmm_ie_LV_ms_radio_access_caps_T)
                                          + 4  /* Old P-TMSI Signature*/
                                          + 2  /* Requested Ready timer value */
                                          + 3  /* DRX parameter */
                                          + 1  /* TMSI status*/
                                          + 7  /* P_TMSI*/
                                          + 10 /* MS Network Capability  */
                                          + 4  /* PDP Context Status  */
                                          + 3  /* PS LCS Capability  */
                                          + 18 /* MBMS Context Status */
                                          + 15 /* UE network capability*/
                                          + 7  /*Additional Mobile Identity*/
                                          + 8  /*Additional Old RAI */
                                          + 3  /*Voice domain preference and UE's usage setting */
                                          + 1  /*PTMSI type IEI */
                                          + 7  /*Old LAI*/
#ifdef FEATURE_NAS_REL11
                                          + 1 /*Network support feature */
                                          + 4; /*TMSI based NRI container*/
#else
                                          + 1;/*Network support feature */
#endif


#if defined(FEATURE_LTE)
  is_lte_supported = (mm_checkif_rat_pri_list_has_rat(&mm_rat_pri_list_info,
                                                                   SYS_RAT_LTE_RADIO_ACCESS)
#ifdef FEATURE_SGLTE
                        && !MM_IS_IN_SGLTE_STATE_4
#endif
                     );
#endif

  memset(&ra_update_request, 0x00, sizeof(gmm_ra_update_request_msg_T));
  ra_update_request.msg_head.skip_pd = PD_GMM;
  ra_update_request.msg_head.type    = GMM_ROUTING_AREA_UPDATE_REQUEST;

  update_type_to_log                 = ra_update_type;
  /* ---------------------
  ** Set Update Type field
  ** --------------------- */

  /* If the data_is_available flag is already set during RAT change, do not
  ** query PS again */

  if (!data_is_available)
  {
#if defined(FEATURE_WCDMA) || defined(FEATURE_GSM_GPRS) 
    data_is_available = dsumtsps_is_ps_data_available();
#endif
  }

  if (data_is_available)
  {
    MSG_HIGH_DS_0(MM_SUB, "=MM= GMM: User data IS available") ;
  }
  else
  {
    MSG_HIGH_DS_0(MM_SUB, "=MM= GMM: User data IS NOT available") ;
  }

#ifdef FEATURE_REL6_PS_FOP
  gmm_ps_fop_req = FALSE;
#endif
#ifdef FEATURE_LTE
  emm_ctrl_data_ptr = emm_db_get_ctrl_data();

  ASSERT(emm_ctrl_data_ptr != NULL);


    if(((emm_ctrl_data_ptr->TIN == NAS_MM_TIN_GUTI) &&
       (emm_ctrl_data_ptr->emm_guti_valid == TRUE) &&
       (ps_session_status == MMRRC_IDLE) &&
       (gmm_state != GMM_NULL) && (gmm_state != GMM_DEREGISTERED)))
  {
      MSG_HIGH_DS_0(MM_SUB,"=MM= Use mapped context ");
      emm_get_mapped_gw_identity(&mm_identity, emm_ctrl_data_ptr);
      
  }
#endif
  if ((is_umts_mode()) && ( 
#ifdef FEATURE_DUAL_SIM
                          sm_is_pdp_activation_pending((sys_modem_as_id_e_type)mm_sub_id) ||
#else
                          sm_is_pdp_activation_pending() ||
#endif
                          data_is_available || gmm_follow_on_request ||
                          gmm_extend_connection_due_to_mt_csfb ||
                          (gmm_service_request_initiator == MS_CC_MM)))
  {
    /* -------------------------------------------------
    ** Follow-on request (bit 4) applicable only in UMTS
    ** ------------------------------------------------- */
    ra_update_type = ra_update_type | 0x08;
#ifdef FEATURE_REL6_PS_FOP    
    if ( 
#ifdef FEATURE_DUAL_SIM
         sm_is_pdp_activation_pending((sys_modem_as_id_e_type)mm_sub_id) || 
#else
         sm_is_pdp_activation_pending() || 
#endif
         (gmm_service_request_initiator == MS_CC_MM) )
    {
     gmm_ps_fop_req = TRUE;
    }
#endif
    
  }

  data_is_available = FALSE;
  ra_update_request.update_type_cksn = ra_update_type;

  /* --------------------------------------------
  ** Set GPRS ciphering key sequence number field
  ** -------------------------------------------- */
  ra_update_request.update_type_cksn =
    (byte)(auth_get_ciphering_key_sequence_number(cksn_context) << 4) +
                                         ra_update_request.update_type_cksn ;

  /* -----------------------------------------
  ** Set Old routing area identification field
  ** ----------------------------------------- */
#if defined(FEATURE_LTE)
  if((emm_ctrl_data_ptr->TIN == NAS_MM_TIN_GUTI &&
        emm_ctrl_data_ptr->emm_guti_valid == TRUE ))
  {
    for (i=0; i < PLMN_SIZE; i++)
    {
      ra_update_request.rai[i] = mm_mapped_gw_identity.ra_id.plmn_id.identity[i];
    }  
    for (i=0; i < LAC_SIZE; i++)
    {
      ra_update_request.rai[PLMN_SIZE+i] = mm_mapped_gw_identity.ra_id.location_area_code.lac[i];
    }  
    ra_update_request.rai[PLMN_SIZE+LAC_SIZE] = mm_mapped_gw_identity.ra_id.routing_area_code;
    
    ptmsi_type_iei_value = 1; //Using Mapped PTMSI type;
  }
  else  
#endif
  {
    for (i=0; i < PLMN_SIZE; i++)
    {
      ra_update_request.rai[i] = gmm_stored_rai.plmn_id.identity[i];
    }
  
    for (i=0; i < LAC_SIZE; i++)
    {
      ra_update_request.rai[PLMN_SIZE+i] = gmm_stored_rai.location_area_code.lac[i];
    }
  
    ra_update_request.rai[PLMN_SIZE+LAC_SIZE] = gmm_stored_rai.routing_area_code;
  } 
  msg_index = 0;

  /* ------------------------------------
  ** Set MS radio access capability field
  ** ------------------------------------ */
  ms_radio_access_capability = get_ms_radio_access_capability( FALSE );
  ms_radio_access_capability.length = MIN(ms_radio_access_capability.length, sizeof(ms_radio_access_capability.value));

  ra_update_request.data[msg_index] = (byte) ms_radio_access_capability.length;
  msg_index = msg_index + 1;
  if(msg_index+ms_radio_access_capability.length <= max_index_size)
  {
    for (i=0; i < ms_radio_access_capability.length; i++)
    {
      ra_update_request.data[msg_index+i] = ms_radio_access_capability.value[i];
    }
  }
  msg_index = msg_index + (byte) ms_radio_access_capability.length;

  /*
  ** P-TMSI signature is used only if the network indicates
  ** that the R'99 is supported
  */
  // if (get_revision_level_indicator()) 
    /* ----------------------------------------------------------------------
    ** Set Old P-TMSI signature field (if a valid P-TMSI signature is stored)
    ** ---------------------------------------------------------------------- */
#if defined(FEATURE_LTE)
    if((emm_ctrl_data_ptr->TIN == NAS_MM_TIN_GUTI &&
          emm_ctrl_data_ptr->emm_guti_valid == TRUE ))
    {
      if(msg_index < max_index_size)
      {
        ra_update_request.data[msg_index] = GMM_IEI_PTMSI_SIGNATURE;
      }
      msg_index = msg_index + 1;
      for (i=0; i < PTMSI_SIGNATURE_SIZE; i++)
      {
        if((msg_index+i) < max_index_size)
        {
          ra_update_request.data[msg_index+i] = mm_mapped_gw_identity.ptmsi_signature.value[i];
        }
      } /* end for (i=0; i < PTMSI_SIGNATURE_SIZE; i++) */      
      msg_index = msg_index + PTMSI_SIGNATURE_SIZE;
    }
    else
#endif
    {
      ptmsi_signature_present = FALSE;
  
      for (i=0; i < PTMSI_SIGNATURE_SIZE; i++)
      {
        if (gmm_stored_ptmsi_signature.value[i] != 0xFF)
        {
          ptmsi_signature_present = TRUE;
          break;
        }
      } /* end for (i=0; i < PTMSI_SIGNATURE_SIZE; i++) */
  
      if (ptmsi_signature_present)
      {
        if(msg_index < max_index_size)
        {
          ra_update_request.data[msg_index] = GMM_IEI_PTMSI_SIGNATURE;
        }
        msg_index = msg_index + 1;
        for (i=0; i < PTMSI_SIGNATURE_SIZE; i++)
        {
          if((msg_index+i) < max_index_size)
          {
            ra_update_request.data[msg_index+i] = gmm_stored_ptmsi_signature.value[i];
          }
        } /* end for (i=0; i < PTMSI_SIGNATURE_SIZE; i++) */
  
        msg_index = msg_index + PTMSI_SIGNATURE_SIZE;
      } /* end if (ptmsi_signature_present) */
    }   

  

  /* ------------------------------------------------------
  ** Requested READY timer value field
  ** ------------------------------------------------------ */
  if((msg_index+1) < max_index_size)
  {
    set_requested_ready_timer( &ra_update_request.data[msg_index], &msg_index );
  }

  /* -----------------------
  ** Set DRX parameter field
  ** ----------------------- */
#if defined(FEATURE_LTE)
  if(((emm_ctrl_data_ptr->emm_rat_drx_param_to_be_sent == NAS_SEND_DRX_ON_GW_RAT) ||
      (emm_ctrl_data_ptr->emm_rat_drx_param_to_be_sent == NAS_SEND_DRX_ON_ALL_RATS)))
#endif
  {
    drx_parameter = get_drx_parameter();
    if(msg_index+2 < max_index_size)
    {
      ra_update_request.data[msg_index]   = GMM_IEI_DRX_PARAMETER;
      ra_update_request.data[msg_index+1] = drx_parameter.value[0];
      ra_update_request.data[msg_index+2] = drx_parameter.value[1];
    }
    msg_index = msg_index + 3;
  }

  /* ---------------------
  ** Set TMSI status field
  ** --------------------- */
  if (((ra_update_type & 0x07) == GMM_COMBINED_RA_LA_UPDATING) ||
      ((ra_update_type & 0x07) == GMM_COMBINED_RA_LA_UPDATING_WITH_IMSI_ATTACH))
  {
    tmsi_present = FALSE;

    for (i=0; i < TMSI_SIZE; i++)
    {
      if (mm_location_information.tmsi[i] != 0xFF)
      {
        tmsi_present = TRUE;
        break;
      }
    } /* end for (i=0; i < PTMSI_SIZE; i++) */

    /* Refer to the 9.4.14.5, Ts 24.008 */
    if (!tmsi_present)
    {
      if(msg_index < max_index_size)
      {
        ra_update_request.data[msg_index] = (GMM_IEI_TMSI_STATUS << 4) | tmsi_present;
      }
      msg_index = msg_index + 1;
    }
  }

  /* Set the P-TMSI and Network Capabilities IEs only for R99 system */
#if defined FEATURE_DUAL_SIM || defined FEATURE_SGLTE
  if (get_revision_level_indicator_per_nas_stacks((sys_modem_as_id_e_type)mm_as_id) != UE_REV_PRE_REL99)
#else
  if (get_revision_level_indicator() != UE_REV_PRE_REL99)
#endif
  {

    if (is_umts_mode())
    {
      /* ----------------
      ** Set P-TMSI field
      ** ---------------- */
      if(msg_index+1 < max_index_size)
      {
        ra_update_request.data[msg_index]   = GMM_IEI_PTMSI;
        ra_update_request.data[msg_index+1] = TMSI_SIZE + 1;
      }

      /* ------------------------------------------------
      ** Set Identity type field (TS 24.008 Table 10.5.4)
      ** ------------------------------------------------ */
      if(msg_index+2 < max_index_size)
      {
        ra_update_request.data[msg_index+2] = 0xF4;
      }

      msg_index = msg_index + 3;
#if defined(FEATURE_LTE)
      if((emm_ctrl_data_ptr->TIN == NAS_MM_TIN_GUTI &&
            emm_ctrl_data_ptr->emm_guti_valid == TRUE ))
      {
        for (i=0; i < PTMSI_SIZE; i++)
        {
          if((msg_index+i) < max_index_size)
          {
            ra_update_request.data[msg_index+i] = mm_mapped_gw_identity.ptmsi.digit[i];
          }
        }
      }     
      else
#endif
      {
        for (i=0; i < PTMSI_SIZE; i++)
        {
          if((msg_index+i) < max_index_size)
          {
            ra_update_request.data[msg_index+i] = gmm_stored_ptmsi.digit[i];
          }
        }
      }
      msg_index = msg_index + PTMSI_SIZE;
    }

    /* -------------------------------
    ** Set MS network capability field
    ** ------------------------------- */
    ms_network_capability = get_ms_network_capability();
    ms_network_capability.length = MIN(ms_network_capability.length, sizeof(ms_network_capability.value));
    if(msg_index+1 < max_index_size)
    {
      ra_update_request.data[msg_index]   = GMM_IEI_MS_NETWORK_CAPABILITY;
      ra_update_request.data[msg_index+1] = ms_network_capability.length;
    }
    msg_index = msg_index + 2;


    for (i=0; i < ms_network_capability.length; i++)
    {
      if((msg_index+i) < max_index_size)
      {
        ra_update_request.data[msg_index+i] = ms_network_capability.value[i];
      }
    }
    msg_index = msg_index + ms_network_capability.length;

    //Pack the PDP context status IE
    pdp_context_status = get_pdp_context_status((sys_modem_as_id_e_type)mm_sub_id);
    pdp_context_status.length = MIN(pdp_context_status.length, GMM_PDP_CONTEXT_STATUS_LENGTH);
    if(msg_index+1 < max_index_size)
    {
      ra_update_request.data[msg_index]   = GMM_IEI_PDP_CONTEXT_STATUS;
      ra_update_request.data[msg_index+1] = pdp_context_status.length;
    }
    msg_index = msg_index + 2;


    for (i=0; i < pdp_context_status.length; i++)
    {
      if((msg_index+i) < max_index_size)
      {
        ra_update_request.data[msg_index+i] = pdp_context_status.value[i];
      }
    }
    msg_index = msg_index + pdp_context_status.length;
#ifdef FEATURE_MBMS    
    #error code not present
#endif
   
#if defined(FEATURE_LTE)
    /* -----------------------
    ** UE NETWORK CAPABILITY
    ** ---------------------  */
    if (((
           mm_checkif_rat_pri_list_has_rat(&mm_rat_pri_list_info, SYS_RAT_LTE_RADIO_ACCESS)
#ifdef FEATURE_SGLTE
           && !MM_IS_IN_SGLTE_STATE_4
#endif
          ) &&
        ((ra_update_type & 0x07) != GMM_PERIODIC_UPDATING) ) )
    {
      memset(&ue_capability, 0x0, sizeof(ue_capability));
      
      memscpy( (void *)&ue_capability.ue_sec_capability, sizeof(ue_capability.ue_sec_capability),
                     (void *) &emm_ctrl_data_ptr->ue_security_capability,
                     sizeof(lte_nas_emm_ue_security_type));
      
      emm_update_gw_sec_capabilities(emm_ctrl_data_ptr,
                                                                &ue_capability);
      if(msg_index + 1 < max_index_size)
      {
        ra_update_request.data[msg_index] = GMM_IEI_UE_NW_CAPABILITY;
      }
      msg_index += 1;
      if(msg_index + MAX_SIZE_OF_UE_NE_CAPABILITY_IE < max_index_size)
      {
        encode_ue_nw_cap_ie(&ue_capability,&msg_index,ra_update_request.data);
      }
    }

    if((emm_ctrl_data_ptr->TIN == NAS_MM_TIN_GUTI &&
         emm_ctrl_data_ptr->emm_guti_valid == TRUE))
    {
      //Fill the additional identities:
      if(mm_has_valid_ptmsi() && has_valid_rai())
      {
        /* ----------------
        ** Set Additional P-TMSI field
        ** ---------------- */
        if(msg_index+1 < max_index_size)
        {
          ra_update_request.data[msg_index]   = GMM_ADDITIONAL_MOBILE_IDENTITY;
          ra_update_request.data[msg_index+1] = TMSI_SIZE + 1;
        }
  
        /* ------------------------------------------------
        ** Set Identity type field (TS 24.008 Table 10.5.4)
        ** ------------------------------------------------ */
        if(msg_index+2 < max_index_size)
        {
          ra_update_request.data[msg_index+2] = 0xF4;
        }
  
        msg_index = msg_index + 3;
  
        for (i=0; i < PTMSI_SIZE; i++)
        {
          if((msg_index+i) < max_index_size)
          {
            ra_update_request.data[msg_index+i] = gmm_stored_ptmsi.digit[i];
          }
        }  
        msg_index = msg_index + PTMSI_SIZE;          
      
        /*Set Additional OLD RAI data */
        if(msg_index < max_index_size)
        {
          ra_update_request.data[msg_index] = GMM_ADDITIONAL_OLD_RAI;
        }
        msg_index = msg_index + 1;
        
        
        if(msg_index < max_index_size)
        {
          ra_update_request.data[msg_index] = PLMN_SIZE + LAC_SIZE + sizeof(routing_area_code_T);
        }
        msg_index = msg_index + 1;
          
        for (i=0; i < PLMN_SIZE; i++)
        {
          if((msg_index+i) < max_index_size)
          {
            ra_update_request.data[msg_index+i] = gmm_stored_rai.plmn_id.identity[i];
          }  
        }
        msg_index = msg_index + PLMN_SIZE; 
  
        for (i=0; i < LAC_SIZE; i++)
        {
          if((msg_index+i) < max_index_size)
          {
            ra_update_request.data[msg_index+i] = gmm_stored_rai.location_area_code.lac[i];
          }  
        }
        msg_index = msg_index + LAC_SIZE; 
        
        if(msg_index < max_index_size)
        {
          ra_update_request.data[msg_index] = gmm_stored_rai.routing_area_code;
        }        
        msg_index = msg_index + 1; 
      }
    }
#endif /*defined(FEATURE_LTE)*/
#if defined(FEATURE_LTE)  && defined(FEATURE_LTE_REL9)
    if((emm_ctrl_data_ptr->emm_3gpp_spec_ver >= LTE_3GPP_REL9 ) )
    {
      if ((is_lte_supported == TRUE ) &&
          (mmcoord_pending_reg_message.cmd.mmr_reg_req.lte_cs_domain_param.is_voice_domain_ie_valid == TRUE))
      {
        if(msg_index+2 < max_index_size)
        {
          ra_update_request.data[msg_index]   = GMM_VOICE_DOMAIN_PREF_IEI;
          ra_update_request.data[msg_index+1] = 1; //length of the VOICE DOMAIN PREF IE
          ra_update_request.data[msg_index+2] = 
           ((((byte)mmcoord_pending_reg_message.cmd.mmr_reg_req.lte_cs_domain_param.voice_domain_pref) & 0x03) |
           ((((byte)mmcoord_pending_reg_message.cmd.mmr_reg_req.lte_cs_domain_param.voice_or_data_centric)  & 0x01) << 0x02));
          msg_index = msg_index + 3; 
        }
      }
    }
#endif
#ifdef FEATURE_LTE
    /*---------------------------------
    ** Set PTMSI Type IEI 
    *-----------------------------*/
    if((emm_ctrl_data_ptr->exclude_identity_type_ie != TRUE ) )
    {
      if(msg_index < max_index_size)
      {
        ra_update_request.data[msg_index] = (byte) GMM_IEI_PTMSI_TYPE | ptmsi_type_iei_value;
      }        
      msg_index = msg_index + 1; 
    }
#endif

#ifdef FEATURE_NAS_REL10
    /* ------------------------------------------------
    ** Set Network_support_feature IE
    ** ------------------------------------------------ */
    if(mm_nv_nas_rel_compliance >= NV_NAS_COMPLIANCE_REL10)
    {
      if(msg_index < max_index_size)
      {
        ra_update_request.data[msg_index] = (byte)(GMM_IEI_UL_NW_FTR_SUPPORT) | 0x01;
      }
      msg_index = msg_index + 1;
    }
#endif
#ifdef FEATURE_LTE
    /*--------------------------------------------
    **Set Old location area identification
    **-------------------------------------------*/
    if((mm_nv_exclude_old_lai_type_ie != TRUE) &&
       (((ra_update_type & 0x07) == GMM_COMBINED_RA_LA_UPDATING) || 
       ((ra_update_type & 0x07) == GMM_COMBINED_RA_LA_UPDATING_WITH_IMSI_ATTACH)) && 
         mm_has_valid_lai())
    {
      if(msg_index < max_index_size)
      {
         ra_update_request.data[msg_index] = GMM_IEI_OLD_LAI;
      }
      msg_index = msg_index + 1;    
      
      if(msg_index < max_index_size)
      {
         ra_update_request.data[msg_index] = LAI_SIZE;
      }
      msg_index = msg_index + 1;
      
      for (i=0; i < LAI_SIZE; i++)
      {
        if((msg_index+i) < max_index_size)
        {
           ra_update_request.data[msg_index+i] = mm_location_information.lai[i];
        }  
      }
      msg_index = msg_index + LAI_SIZE;     
    }
#endif /*FEATURE_LTE*/

 /***Spec 24.008 4.7.5.2.1:
     ** If the MS has stored a valid TMSI, the MS shall include the TMSI based NRI container IE in the COMBINED RAU REQUEST message
     **====================================================================================**/
#ifdef FEATURE_NAS_REL11
      if(mm_nv_nas_rel_compliance >= NV_NAS_COMPLIANCE_REL11)
      {
        if(has_valid_tmsi() &&
          ((gmm_ra_update_type == GMM_COMBINED_RA_LA_UPDATING) ||
           (gmm_ra_update_type == GMM_COMBINED_RA_LA_UPDATING_WITH_IMSI_ATTACH) ||
            ((gmm_ra_update_type == GMM_PERIODIC_UPDATING) && mm_gs_association_established))) 
        {
          if(msg_index < max_index_size)
          {
            ra_update_request.data[msg_index] = GMM_IEI_TMSI_BASED_NRI_CONTAINER;// IEI on first byte
          }
          msg_index = msg_index + 1;
          if(msg_index < max_index_size) 
          {
            ra_update_request.data[msg_index] = GMM_IEI_TMSI_BASED_NRI_CONTAINER_CONTENT_LENGTH;//NRI_CONTENT_LENGTH
          }
          msg_index = msg_index + 1;
          if(msg_index < max_index_size)
          {
            ra_update_request.data[msg_index] = mm_location_information.tmsi[1];
          // bit 23 to 16 of tmsi
          }
          msg_index = msg_index + 1;
          if(msg_index < max_index_size)
          {
            ra_update_request.data[msg_index] = (mm_location_information.tmsi[2] & 0xC0);
            // bit 15 , 14 of tmsi and rest are spare (0)
          }
          msg_index = msg_index + 1;
        }
      }
#endif
  }
  if(msg_index > max_index_size)
  {
    MSG_ERROR_DS_0(MM_SUB,"=MM= Index out of bounds and buffer overflow");
    return;
  }
  /* ----------------------------------------------------
  ** Set Routing Area Update Request message length field
  ** ---------------------------------------------------- */
  PUT_IMH_LEN((msg_index + 9),&ra_update_request.msg_head.IMH);

  MSG_HIGH_DS_0( MM_SUB, "=MM= Sending RAU REQUEST message");

  mmsend_request_reject_event(TRUE, SMGMM_EVENT_ROUTING_AREA_UPDATE_REQUEST,
                              update_type_to_log);
  if (gmm_directed_signalling_reestablishment ||
      gmm_cause_directed_signalling_reest)
  {
    mmrrcconn_send_L3_message_to_rrc(
      &ra_update_request, RRC_EST_CALL_RE_ESTABLISH, RRC_PS_DOMAIN_CN_ID );
  }
  else
  {
    mmrrcconn_send_L3_message_to_rrc(
      &ra_update_request, RRC_EST_REGISTRATION, RRC_PS_DOMAIN_CN_ID );
  }
} /* end gmm_send_routing_area_update_request_message() */


/*===========================================================================

FUNCTION GMM_SEND_ROUTING_AREA_UPDATE_COMPLETE_MESSAGE

DESCRIPTION

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void gmm_send_routing_area_update_complete_message
(
  boolean                   receive_npdu_number_list_present,
  gmm_npdu_number_list_type *receive_npdu_number_list
)
{
  gmm_ra_update_complete_msg_T gmm_ra_update_complete_msg;
  byte                         msg_length, nsapi_index;
  csn_encoder_T                encoder;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  memset(&gmm_ra_update_complete_msg, 0x00, sizeof(gmm_ra_update_complete_msg_T));

  gmm_ra_update_complete_msg.msg_head.skip_pd = PD_GMM;
  gmm_ra_update_complete_msg.msg_head.type    = GMM_ROUTING_AREA_UPDATE_COMPLETE;

  msg_length = 2;

  if (receive_npdu_number_list_present)
  {
    gmm_ra_update_complete_msg.npdu_num_list[0] = GMM_IEI_RECEIVE_NPDU_NUMBERS_LIST;

    /* Clear the message construction area, and initialise CSN encoder to point
       just after the NPDU Number List length element. */
    memset
    (
      &gmm_ra_update_complete_msg.npdu_num_list[2], 0,
      GMM_RECEIVE_NPDU_NUMBER_LIST_MAX_LENGTH
    );

    csn_encoder_init
    (
      &encoder, &gmm_ra_update_complete_msg.npdu_num_list[2],
      GMM_RECEIVE_NPDU_NUMBER_LIST_MAX_LENGTH
    );

    /* Code the Receive NPDU Number for each NSAPI Address. */

    for ( nsapi_index = 0; nsapi_index < GMM_RECEIVE_NPDU_NUMBER_LIST_MAX_NSAPIS; nsapi_index++ )
    {
      if ( receive_npdu_number_list->nsapi_valid_list[nsapi_index] )
      {
        csn_encode_bits(
          &encoder, 4, (byte)(nsapi_index + GMM_RECEIVE_NPDU_NUMBER_LIST_MIN_NSAPI) );

        csn_encode_bits(
          &encoder, 8, (byte)(receive_npdu_number_list->npdu_number_list[nsapi_index]) );
      }
    }

    /* Encode padding bits. */
    csn_fill_spare_bits(&encoder);

    /* Encode NPDU Number IE length */
    if( csn_bit_pos(&encoder) % 8 == 0)
    {
      gmm_ra_update_complete_msg.npdu_num_list[1] = (byte)csn_bit_pos(&encoder) / 8;
    }
    else
    {
      MSG_ERROR_DS_0(MM_SUB, "=MM= Error encoding NPDU Number List");
      gmm_ra_update_complete_msg.npdu_num_list[1] = 0;
    }

    msg_length += 2 + gmm_ra_update_complete_msg.npdu_num_list[1];
  }

  PUT_IMH_LEN(msg_length,&gmm_ra_update_complete_msg.msg_head.IMH);

  MSG_HIGH_DS_0( MM_SUB, "=MM= Sending RAU COMPLETE message" );

  mmrrcconn_send_L3_message_to_rrc(
    &gmm_ra_update_complete_msg, RRC_EST_LOW_PRI_SIGNALLING, RRC_PS_DOMAIN_CN_ID );

} /* end gmm_send_routing_area_update_complete_message() */


/*===========================================================================

FUNCTION GMM_SEND_SERVICE_REQUEST_MESSAGE

DESCRIPTION

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void gmm_send_service_request_message
(
  rrc_establish_cause_e_type establish_cause,
  auth_security_context_id security_context,
  gmm_ie_pdp_context_status_T* uplink_data_status
)
{
  gmm_service_request_msg_T     service_request_msg;
  gmm_ie_pdp_context_status_T   pdp_context_status;
  byte                          i;
  byte                          cksn;
  byte                          msg_index;
  byte                         max_data_size = 4  /*PDP context status*/
                     + 18  /*MBMS context status*/
                     +4    /*Uplink data status*/;
#ifdef FEATURE_MBMS  
  #error code not present
#endif
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  memset(&service_request_msg, 0x00, sizeof(gmm_service_request_msg_T));
  service_request_msg.msg_head.skip_pd = PD_GMM;
  service_request_msg.msg_head.type    = GMM_SERVICE_REQUEST;

  /* --------------------------------------------
  ** Set GPRS ciphering key sequence number field
  ** -------------------------------------------- */

  cksn = auth_get_ciphering_key_sequence_number(security_context) ;

  service_request_msg.cksn_service_type = cksn;

  /* ----------------------
  ** Set Service type field
  ** ---------------------- */
  if ((gmm_service_request_type != GMM_SIGNALLING) &&
      (gmm_service_request_type != GMM_DATA) &&
      (gmm_service_request_type != GMM_PAGING_RESPONSE)
      #ifdef FEATURE_MBMS
      #error code not present
#endif
      )
  {
    MSG_FATAL_DS( MM_SUB, "=MM= Illegal service request type %d", gmm_service_request_type, 0,0 );
  }

  service_request_msg.cksn_service_type =
   (byte) ((gmm_service_request_type << 4) + service_request_msg.cksn_service_type);

  /* -----------------------
  ** Set P-TMSI length field
  ** ----------------------- */
  service_request_msg.ptmsi[0] = PTMSI_SIZE + 1;

  /* ------------------------------------------------
  ** Set Identity type field (TS 24.008 Table 10.5.4)
  ** ------------------------------------------------ */
  service_request_msg.ptmsi[1] = 0xF4;

  for (i=0; i < PTMSI_SIZE; i++)
  {
    service_request_msg.ptmsi[2+i] = gmm_stored_ptmsi.digit[i];
  }

  msg_index = 0;
  //Pack the PDP context status IE
  pdp_context_status = get_pdp_context_status((sys_modem_as_id_e_type)mm_sub_id);

  service_request_msg.data[msg_index++] = GMM_IEI_PDP_CONTEXT_STATUS;
  service_request_msg.data[msg_index++] = pdp_context_status.length;  

  for (i=0; i < pdp_context_status.length; i++)
  {
    if(msg_index < max_data_size )
    {
      service_request_msg.data[msg_index++] = pdp_context_status.value[i];
    }
    else
    {
      MSG_ERROR_DS_0(MM_SUB,"=MM= Index out of bound and buffer overflow");
      break;
    }
  }

#ifdef FEATURE_MBMS  
  #error code not present
#endif   

  if ((uplink_data_status != NULL) && (gmm_service_request_type == GMM_DATA))
  {
    if(msg_index+1 < max_data_size)
    {
      service_request_msg.data[msg_index++] = GMM_IEI_UPLINK_DATA_STATUS;
      service_request_msg.data[msg_index++] = uplink_data_status->length;
    }
    else
    {
      MSG_ERROR_DS_0(MM_SUB,"=MM= Index out of bound and buffer overflow");
    }
    for(i = 0; i < uplink_data_status->length; i++)
    {
      if(msg_index < max_data_size)
      {
        service_request_msg.data[msg_index++] = uplink_data_status->value[i];        
      }
      else
      {
        MSG_ERROR_DS_0(MM_SUB,"=MM= Index out of bound and buffer overflow");
      }
    }
  }
  else if(gmm_service_request_type == GMM_PAGING_RESPONSE)
  {
    gmm_controls_resel_dec_mt_ps_call = TRUE;
  }
  PUT_IMH_LEN((9 + msg_index),&service_request_msg.msg_head.IMH);

  MSG_HIGH_DS_0( MM_SUB, "=MM= Sending SERVICE REQUEST message");

  mmrrcconn_send_L3_message_to_rrc(
   &service_request_msg, establish_cause, RRC_PS_DOMAIN_CN_ID );

} /* end gmm_send_service_request_message() */


/*===========================================================================

FUNCTION GMM_SEND_GMM_STATUS_MESSAGE

DESCRIPTION

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void gmm_send_gmm_status_message( byte gmm_cause )
{
  gmm_mo_gmm_status_msg_T gmm_status_msg;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  memset(&gmm_status_msg, 0x00, sizeof(gmm_mo_gmm_status_msg_T));

  gmm_status_msg.msg_head.skip_pd = PD_GMM;
  gmm_status_msg.msg_head.type    = GMM_STATUS;
  gmm_status_msg.gmm_cause        = gmm_cause;

  gmm_status_msg.msg_head.IMH.message_len_lsb = 3;
  gmm_status_msg.msg_head.IMH.message_len_msb = 0;

  MSG_HIGH_DS_0( MM_SUB, "=MM= Sending GMM STATUS message");

  mmrrcconn_send_L3_message_to_rrc(
    &gmm_status_msg, RRC_EST_LOW_PRI_SIGNALLING, RRC_PS_DOMAIN_CN_ID );

} /* end gmm_send_gmm_status_message() */


/*===========================================================================

FUNCTION GMM_VALID_ATTACH_ACCEPT_MESSAGE

DESCRIPTION

DEPENDENCIES
  None

RETURN VALUE
  TRUE
  FALSE - Ignore

SIDE EFFECTS
  None

===========================================================================*/

#define GMM_ATTACH_ACCEPT_MSG_MIN_LENGTH 11 /* Includes all mandatory IEs */

boolean gmm_valid_attach_accept_message
(
  mm_cmd_type             *message,
  gmm_attach_accept_msg_T *attach_accept_msg
)
{
  boolean valid_msg;
  uint16  msg_index;
  byte    iei_length;
  byte    optional_ie_index;
  byte    i;
  byte    emergency_list_length;
  boolean tmsi_avail = FALSE;
  uint16 gmm_max_octet_len;
  uint16    msg_length;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  valid_msg  = TRUE;
  gmm_max_octet_len= gmm_max_L3_msg_length(MM_PS_DOMAIN);
  GET_IMH_LEN(msg_length,&message->cmd.mm_data_ind.message_header);

  if (msg_length < GMM_ATTACH_ACCEPT_MSG_MIN_LENGTH)
  {
    valid_msg = FALSE;
  }

  if (valid_msg)
  {
    attach_accept_msg->attach_result =
      message->cmd.mm_data_ind.L3_data[2] & 0x07;

#ifdef FEATURE_REL6_PS_FOP
    {
      attach_accept_msg->follow_on_proceed_response = 
              (message->cmd.mm_data_ind.L3_data[2] & 0x08) >> 3;
    }        
#endif/* FEATURE_REL6_PS_FOP */

    if ((attach_accept_msg->attach_result != GMM_GPRS_ONLY_ATTACHED) &&
        (attach_accept_msg->attach_result != GMM_COMBINED_GPRS_IMSI_ATTACHED))
    {
      valid_msg = FALSE;
    }

    attach_accept_msg->force_to_standby =
      (message->cmd.mm_data_ind.L3_data[2] & 0x70) >> 4;

    if (attach_accept_msg->force_to_standby != 0x01)
    {
      /* -----------------------------------------------------------------------
      ** All other values are interpreted as "force to standby not indicated" by
      ** this version of the protocol (TS 24.008 Table 10.5.140)
      ** ----------------------------------------------------------------------- */
      attach_accept_msg->force_to_standby = 0;
    }
  }

  if (valid_msg)
  {
    attach_accept_msg->periodic_ra_update_timer =
      message->cmd.mm_data_ind.L3_data[3];

    attach_accept_msg->sms_radio_priority_nw =
      message->cmd.mm_data_ind.L3_data[4] & 0x07;

    for (i=0; i < RAI_SIZE; i++)
    {
      attach_accept_msg->routing_area_identification[i] =
        message->cmd.mm_data_ind.L3_data[5+i];
    }
  }

  if (!valid_msg)
  {
    gmm_send_gmm_status_message( INVALID_MANDATORY_INFORMATION );
  }

  /* -----------------------------------------
  ** Now parse the non-imperative message part
  ** ----------------------------------------- */
  attach_accept_msg->ptmsi_signature_present   = FALSE;
  attach_accept_msg->ready_timer_value_present = FALSE;
  attach_accept_msg->allocated_ptmsi_present   = FALSE;
  attach_accept_msg->ms_identity_present       = FALSE;
  attach_accept_msg->gmm_cause_present         = FALSE;
  attach_accept_msg->t3302_value_present       = FALSE;
  attach_accept_msg->cell_notification_present = FALSE;
  attach_accept_msg->equiv_plmn_list_present   = FALSE;
  attach_accept_msg->emergency_number_list_present = FALSE;
  memset(&(attach_accept_msg->emergency_number_list), 0,
     sizeof(mmr_emergency_num_list_s_type));
  attach_accept_msg->nw_ftr_support_present    =  FALSE;
  attach_accept_msg->nw_ftr_support            = 0;
#if defined(FEATURE_LTE)
  attach_accept_msg->t3323_value_present = FALSE;
#endif

  attach_accept_msg->t3319_value_present       = FALSE;

#ifdef FEATURE_NAS_REL10
  attach_accept_msg->ext_t3312_value_present =  FALSE;
#endif


  /* ----------------------------------------------------------
  ** Used to ensure optional IE is not received out of sequence
  ** ---------------------------------------------------------- */
  optional_ie_index = 0;

  msg_index = GMM_ATTACH_ACCEPT_MSG_MIN_LENGTH;

  if(msg_length > gmm_max_octet_len)

  {
    valid_msg = FALSE;
    MSG_ERROR_DS_0(MM_SUB,"=MM= Index out of bound and buffer overflow");
  }
  while ((msg_index < msg_length) && (valid_msg))
  {
    switch (message->cmd.mm_data_ind.L3_data[msg_index])
    {
      /* ---------------------------------------------------------------
      ** Optional IEs listed in order specified in TS 24.008 Table 9.4.2
      ** --------------------------------------------------------------- */
      case GMM_IEI_PTMSI_SIGNATURE:
        if ((optional_ie_index == 0) &&
            ((msg_index + sizeof( gmm_ie_TV_ptmsi_signature_T )) <= msg_length))
        {
          for (i=0; i < PTMSI_SIGNATURE_SIZE; i++)
          {
            attach_accept_msg->ptmsi_signature[i] =
              message->cmd.mm_data_ind.L3_data[msg_index+1+i];
          }

          attach_accept_msg->ptmsi_signature_present = TRUE;
          optional_ie_index = 1;
        }
        msg_index = msg_index + sizeof( gmm_ie_TV_ptmsi_signature_T );
        break;

      case GMM_IEI_READY_TIMER_VALUE:
        if ((optional_ie_index <= 1) &&
            ((msg_index + sizeof( gmm_ie_TV_timer_value_T )) <= msg_length))
        {
          attach_accept_msg->ready_timer_value =
            message->cmd.mm_data_ind.L3_data[msg_index+1];
          attach_accept_msg->ready_timer_value_present = TRUE;
          optional_ie_index = 2;
        }
        msg_index = msg_index + sizeof( gmm_ie_TV_timer_value_T );
        break;

      case GMM_IEI_PTMSI:
        if(msg_index+1 < msg_length)
        {
          iei_length = message->cmd.mm_data_ind.L3_data[msg_index+1];
        }
        else
        {
          MSG_ERROR_DS_0(MM_SUB,"=MM= Index out of bound and buffer overflow");
          valid_msg = FALSE;
          break;
        }

        if(msg_index+2 >= msg_length)
        {
          MSG_ERROR_DS_0(MM_SUB,"=MM= Index out of bound and buffer overflow");
          valid_msg = FALSE;
          break;
        }
        else if ((optional_ie_index <= 2) &&
            ((msg_index + 2+ iei_length) <= msg_length))
        {
          if ((message->cmd.mm_data_ind.L3_data[msg_index+2] != 0xF4) ||
              (iei_length != (PTMSI_SIZE+1)))
          {
            /* -----------------------------------------
            ** IE should only be used to assign a P-TMSI
            ** ----------------------------------------- */
            gmm_send_gmm_status_message( SEMANTICALLY_INCORRECT_MESSAGE );
            valid_msg = FALSE;
          }
          else
          {
            for (i=0; i < PTMSI_SIZE; i++)
            {
              attach_accept_msg->allocated_ptmsi[i] =
                message->cmd.mm_data_ind.L3_data[msg_index+3+i];
            }
            attach_accept_msg->allocated_ptmsi_present = TRUE;
            optional_ie_index = 3;
          }
        }
        msg_index = msg_index + 2 + iei_length;
        break;

      case GMM_IEI_MS_IDENTITY:
        if(msg_index+1 < msg_length)
        {
          iei_length = message->cmd.mm_data_ind.L3_data[msg_index+1];
        }
        else
        {
          MSG_ERROR_DS_0(MM_SUB,"=MM= Index out of bound and buffer overflow");
          valid_msg = FALSE;
          break;
        }

        if ((optional_ie_index <= 3) &&
            ((msg_index + 2 + iei_length) <= msg_length))
        {
          attach_accept_msg->ms_identity.id_type =
            message->cmd.mm_data_ind.L3_data[msg_index+2];
          
          if ((attach_accept_msg->ms_identity.id_type & 0x07) == GMM_TMSI)
          {
            if ((message->cmd.mm_data_ind.L3_data[msg_index+2] != 0xF4) ||
                (iei_length != (TMSI_SIZE+1)))
            {
              gmm_send_gmm_status_message( SEMANTICALLY_INCORRECT_MESSAGE );
              valid_msg = FALSE;
            }
            else
            {
              for (i=0; i < (iei_length-1); i++)
              {
                attach_accept_msg->ms_identity.value[i] =
                  message->cmd.mm_data_ind.L3_data[msg_index+3+i];
              }
              attach_accept_msg->ms_identity_present = TRUE;
              tmsi_avail = TRUE;
              optional_ie_index = 4;
            }
          }
          else
          {
            attach_accept_msg->ms_identity_present = TRUE;
            optional_ie_index = 4;
          }
        }
        msg_index = msg_index + 2 + iei_length;
        break;

      case GMM_IEI_CAUSE:
        if ((optional_ie_index <= 4) &&
            ((msg_index + sizeof( gmm_ie_TV_cause_value_T )) <= msg_length))
        {
          attach_accept_msg->gmm_cause =
            message->cmd.mm_data_ind.L3_data[msg_index+1];
          attach_accept_msg->gmm_cause_present = TRUE;
          optional_ie_index = 5;
        }
        msg_index = msg_index + sizeof( gmm_ie_TV_cause_value_T );
        break;

      case GMM_IEI_T3302_VALUE:
        if ((optional_ie_index <= 5) &&
            ((msg_index + sizeof( gmm_ie_TLV_timer_value_T )) <= msg_length))
        {
          attach_accept_msg->t3302_value =
            message->cmd.mm_data_ind.L3_data[msg_index+2];
          attach_accept_msg->t3302_value_present = TRUE;
          optional_ie_index = 6;
        }
        msg_index = msg_index + sizeof( gmm_ie_TLV_timer_value_T );
        break;

      case GMM_IEI_CELL_NOTIFICATION:
        if ((optional_ie_index <= 6) && ((msg_index + 1) <= msg_length))
        {
          attach_accept_msg->cell_notification_present = TRUE;
          optional_ie_index = 7;
        }
        msg_index = msg_index + 1;
        break;

      case IEI_EQUIV_PLMN:
        if(msg_index+1 < msg_length)
        {
          attach_accept_msg->equiv_plmn_list.length = message->cmd.mm_data_ind.L3_data[msg_index+1];
        }
        else
        {
          MSG_ERROR_DS_0(MM_SUB,"=MM= Index out of bound and buffer overflow");
          valid_msg = FALSE;
          break;
        }

        if ((optional_ie_index <= 7) &&
            (attach_accept_msg->equiv_plmn_list.length >0) &&
            ((msg_index + 2 + attach_accept_msg->equiv_plmn_list.length) <= msg_length))
        {
          /* (24.008, 4.4.1) The maximum number of entries in the stored equivalent
          ** PLMN list is six (5 equiv PLMN + PLMN of the network that downloaded
          ** the list.
          */

          /* (24.008, 4.4.1) The maximum number of entries in the stored equivalent
          ** PLMN list is sixteen in REL 6 (15 equiv PLMN + PLMN of the network that downloaded
          ** the list.
          */
          if (attach_accept_msg->equiv_plmn_list.length > (EQUIVALENT_PLMN_LIST_MAX_LENGTH * 3))
          {
            attach_accept_msg->equiv_plmn_list.length = EQUIVALENT_PLMN_LIST_MAX_LENGTH * 3;
          }

          /*
          ** Copy the equivalent PLMN list to the msg structure
          */
          memscpy(attach_accept_msg->equiv_plmn_list.plmn, sizeof(sys_plmn_id_s_type)*EQUIVALENT_PLMN_LIST_MAX_LENGTH,
                      &(message->cmd.mm_data_ind.L3_data[msg_index+2]),
                      attach_accept_msg->equiv_plmn_list.length);

          attach_accept_msg->equiv_plmn_list_present = TRUE;
          optional_ie_index = 8;
        }
        msg_index = msg_index + 2 + attach_accept_msg->equiv_plmn_list.length;

        MSG_MED_DS_0(MM_SUB, "=MM= Rec'd Equiv PLMN list");
        break;

      case IEI_EMERGENCY_NUM_LIST:
        if(msg_index+1 < msg_length)
        {
          emergency_list_length = message->cmd.mm_data_ind.L3_data[msg_index+1];
        }
        else
        {
          MSG_ERROR_DS_0(MM_SUB,"=MM= Index out of bound and buffer overflow");
          valid_msg = FALSE;
          break;
        }
        if ((optional_ie_index <= 9) && (emergency_list_length > 0)&&
            ((msg_index + emergency_list_length) <= msg_length))
        {
            mm_parse_emergency_number(&(attach_accept_msg->emergency_number_list),
                      &(message->cmd.mm_data_ind.L3_data[msg_index+1]) );
            optional_ie_index = 10;
      		attach_accept_msg->emergency_number_list_present = TRUE;
        }/*Optional Index valid*/

        msg_index = msg_index + 2 + emergency_list_length;
        break;
        /* Spec_ref: 9.4.2       
              Optional IE in ATTACH_Accept*/
      case GMM_IEI_T3319_VALUE:
        if ((optional_ie_index <= 11) &&
            ((msg_index + sizeof( gmm_ie_TLV_timer_value_T )) <= msg_length))
        {
          if (msg_index+2 < gmm_max_octet_len)
          {
            attach_accept_msg->t3319_value =
               message->cmd.mm_data_ind.L3_data[msg_index+2];
          }
          else
          {
            MSG_ERROR_DS_0(MM_SUB,"Index out of bound and buffer overflow");
            valid_msg = FALSE;
            break;
          }
          attach_accept_msg->t3319_value_present = TRUE;
          optional_ie_index = 12;
        }
        msg_index = msg_index + sizeof( gmm_ie_TLV_timer_value_T );
        break;

#if defined(FEATURE_LTE)
      case GMM_IEI_T3323_VALUE:

        if ((optional_ie_index <= 12) &&
            ((msg_index + sizeof( gmm_ie_TLV_timer_value_T )) <= msg_length))
        {
          attach_accept_msg->t3323_value =
            message->cmd.mm_data_ind.L3_data[msg_index+2];
          attach_accept_msg->t3323_value_present = TRUE;
          optional_ie_index = 13;
        }
        msg_index = msg_index + sizeof( gmm_ie_TLV_timer_value_T );
        break;
#endif

#ifdef FEATURE_NAS_REL10
            case GMM_IEI_EX_T3312_VALUE:
      
              if ((optional_ie_index <= 13) &&
                  ((msg_index + sizeof( gmm_ie_TLV_timer_value_T )) <= msg_length))
              {
                attach_accept_msg->ext_t3312_value =
                  message->cmd.mm_data_ind.L3_data[msg_index+2];
                attach_accept_msg->ext_t3312_value_present= TRUE;
                optional_ie_index = 14;
              }
              msg_index = msg_index + sizeof( gmm_ie_TLV_timer_value_T );
              break;
#endif

      default:

        if ((message->cmd.mm_data_ind.L3_data[msg_index] & 0xF0) == 
                  GMM_IEI_NW_FTR_SUPPORT)
        {
          if ((optional_ie_index <= 8) && ((msg_index + 1 )<= msg_length))
          {
           attach_accept_msg->nw_ftr_support_present  = TRUE;
           gmm_decode_nw_ftr_support
           (
                &(message->cmd.mm_data_ind.L3_data[msg_index]),
                &(attach_accept_msg->nw_ftr_support)
           );
             optional_ie_index = 9;
          }
          msg_index = msg_index + 1;
        }
        else if((message->cmd.mm_data_ind.L3_data[msg_index] & 0xF0) == 
                  GMM_IEI_REQUESTED_MS_INFORMATION)
        {
          /*---------------------------------*/
          
          if ((optional_ie_index <= 10) && ((msg_index + 1 )<= msg_length))
          {
            attach_accept_msg->req_ms_info = ( message->cmd.mm_data_ind.L3_data[msg_index]) & 0x0F;
            optional_ie_index = 11; 
          }
           msg_index = msg_index + 1;
        }
        /* ----------
        ** Unknown IE
        ** ---------- */

        else if ((message->cmd.mm_data_ind.L3_data[msg_index] & 0xF0) == 0x00)
        {
          /* --------------------------------------
          ** Comprehension required (TS 24.008 8.5)
          ** -------------------------------------- */
          gmm_send_gmm_status_message( INVALID_MANDATORY_INFORMATION );
          valid_msg = FALSE;
        }
        else
        {
          /* ------------------------------------------------------------
          ** Ignore unknown IE - increment msg_index per TS 24.007 11.2.4
          ** ------------------------------------------------------------ */
          if ((message->cmd.mm_data_ind.L3_data[msg_index] & 0x80) == 0x80)
          {
            /* --------------------------------------------------------------
            ** A 1 valued bit 8 indicates that the whole IE is one octet long
            ** -------------------------------------------------------------- */
            msg_index = msg_index + 1;
          }
          else
          {
            /* ---------------------------------------------------------------------
            ** A 0 valued bit 8 indicates that the following octet is a length octet
            ** --------------------------------------------------------------------- */
            if(msg_index+1 < msg_length)
            {
              msg_index =
                msg_index + 2 + message->cmd.mm_data_ind.L3_data[msg_index+1];
            }
            else
            {
              MSG_ERROR_DS_0(MM_SUB,"=MM= Index out of bound and buffer overflow");
              valid_msg = FALSE;
              break;
            }
            
          }
        }
        break;
    }
  } /* end while ((msg_index < msg_length) && (valid_msg)) */

  if (valid_msg)
  {
    if( (gmm_state == GMM_REGISTERED) &&(is_gsm_mode()))
    {
       if ((mm_timer_status[TIMER_T3350] == TIMER_STOPPED) ||
           (gmm_is_ptmsi_present_in_prev_gmm_msg != attach_accept_msg->allocated_ptmsi_present)||
           ((gmm_is_ptmsi_present_in_prev_gmm_msg) &&
            (memcmp(gmm_stored_ptmsi.digit,attach_accept_msg->allocated_ptmsi,PTMSI_SIZE)!= 0))||
           (gmm_is_tmsi_present_in_prev_gmm_msg !=tmsi_avail))
       {
         valid_msg = FALSE;
         gmm_send_gmm_status_message(
                    MESSAGE_NOT_COMPATIBLE_WITH_PROTOCOL_STATE );
       }
    } 
  }

#ifdef FEATURE_LTE
  /* Since LTE cannot continue without P-TMSI allocated in GW, GMM Attach Accept or RAU Accept without P-TMSI 
  ** allocated shall be blocked by dropping incoming message but no Status OTA cause matches with this error */
  if (!mm_has_valid_ptmsi() &&
      !attach_accept_msg->allocated_ptmsi_present)
  {
    valid_msg = FALSE;
  }
#endif

  return valid_msg;

} /* end gmm_valid_attach_accept_message() */


/*===========================================================================

FUNCTION GMM_VALID_ATTACH_REJECT_MESSAGE

DESCRIPTION

DEPENDENCIES
  None

RETURN VALUE
  TRUE
  FALSE - Ignore

SIDE EFFECTS
  None

===========================================================================*/

#define GMM_ATTACH_REJECT_MSG_MIN_LENGTH 3 /* Includes all mandatory IEs */

boolean gmm_valid_attach_reject_message
(
  mm_cmd_type             *message,
  gmm_attach_reject_msg_T *attach_reject_msg
)
{
  boolean valid_msg;
  uint16  msg_index;
  byte    optional_ie_index;
  uint16 gmm_max_octet_length;
  uint16    msg_length;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  valid_msg  = TRUE;

  GET_IMH_LEN(msg_length,&message->cmd.mm_data_ind.message_header);
  gmm_max_octet_length = gmm_max_L3_msg_length(MM_PS_DOMAIN);

  if (msg_length < GMM_ATTACH_REJECT_MSG_MIN_LENGTH)
  {
    gmm_send_gmm_status_message( INVALID_MANDATORY_INFORMATION );
    valid_msg = FALSE;
  }
  else
  {
    attach_reject_msg->gmm_cause = message->cmd.mm_data_ind.L3_data[2];
  }

  /* -----------------------------------------
  ** Now parse the non-imperative message part
  ** ----------------------------------------- */
  attach_reject_msg->t3302_value_present = FALSE;
#ifdef FEATURE_NAS_REL10
  attach_reject_msg->t3346_value_present = FALSE;
#endif

  /* ----------------------------------------------------------
  ** Used to ensure optional IE is not received out of sequence
  ** ---------------------------------------------------------- */
  optional_ie_index = 0;

  msg_index = GMM_ATTACH_REJECT_MSG_MIN_LENGTH;
  if(msg_length > gmm_max_octet_length)
  {
    valid_msg = FALSE;
    MSG_ERROR_DS_0(MM_SUB,"=MM= Index out of bounds and buffer overflow");
  }
  while ((msg_index < msg_length) && (valid_msg))
  {
    switch (message->cmd.mm_data_ind.L3_data[msg_index])
    {
      /* ---------------------------------------------------------------
      ** Optional IEs listed in order specified in TS 24.008 Table 9.4.2
      ** --------------------------------------------------------------- */
      case GMM_IEI_T3302_VALUE:
        if ((optional_ie_index == 0) &&
            ((msg_index + sizeof( gmm_ie_TLV_timer_value_T )) <= msg_length))
        {
      
        if(msg_index+2 < gmm_max_octet_length)
          {
            attach_reject_msg->t3302_value =
            message->cmd.mm_data_ind.L3_data[msg_index+2];
          }
          else
          {
            MSG_ERROR_DS_0(MM_SUB,"=MM= Index out of bound and buffer overflow");
            valid_msg = FALSE;
            break;
          }
          attach_reject_msg->t3302_value_present = TRUE;
          optional_ie_index = 1;
        }
        msg_index = msg_index + sizeof( gmm_ie_TLV_timer_value_T );
        break;
#ifdef FEATURE_NAS_REL10
      case GMM_IEI_T3346_VALUE:
        if ((optional_ie_index <= 1) &&
            ((msg_index + sizeof( gmm_ie_TLV_timer_value_T )) <= msg_length))
        {
          if(msg_index+2 < gmm_max_octet_length)
          {
            attach_reject_msg->t3346_value =
            message->cmd.mm_data_ind.L3_data[msg_index+2];
          }
          else
          {
            MSG_ERROR_DS_0(MM_SUB,"=MM= Index out of bound and buffer overflow");
            valid_msg = FALSE;
            break;
          }
          attach_reject_msg->t3346_value_present = TRUE;
          optional_ie_index = 2;
        }
        msg_index = msg_index + sizeof( gmm_ie_TLV_timer_value_T );
        break;
#endif
      default:
        /* ----------
        ** Unknown IE
        ** ---------- */
        if ((message->cmd.mm_data_ind.L3_data[msg_index] & 0xF0) == 0x00)
        {
          /* --------------------------------------
          ** Comprehension required (TS 24.008 8.5)
          ** -------------------------------------- */
          gmm_send_gmm_status_message( INVALID_MANDATORY_INFORMATION );
          valid_msg = FALSE;
        }
        else
        {
          /* ------------------------------------------------------------
          ** Ignore unknown IE - increment msg_index per TS 24.007 11.2.4
          ** ------------------------------------------------------------ */
          if ((message->cmd.mm_data_ind.L3_data[msg_index] & 0x80) == 0x80)
          {
            /* --------------------------------------------------------------
            ** A 1 valued bit 8 indicates that the whole IE is one octet long
            ** -------------------------------------------------------------- */
            msg_index = msg_index + 1;
          }
          else
          {
            /* ---------------------------------------------------------------------
            ** A 0 valued bit 8 indicates that the following octet is a length octet
            ** --------------------------------------------------------------------- */
            if( msg_index+1 < gmm_max_octet_length)
            {
              msg_index =
                msg_index + 2 + message->cmd.mm_data_ind.L3_data[msg_index+1];
            }
            else
            {
              MSG_ERROR_DS_0(MM_SUB,"=MM= Index out of bounds and buffer overflow");
              valid_msg = FALSE;
            }
          }
        }
        break;
    } /* end switch (message->cmd.mm_data_ind.L3_data[msg_index]) */
  } /* end while ((msg_index < msg_length) && (valid_msg)) */

  return valid_msg;

} /* end gmm_valid_attach_reject_message() */


/*===========================================================================

FUNCTION GMM_VALID_MT_DETACH_REQUEST_MESSAGE

DESCRIPTION

DEPENDENCIES
  None

RETURN VALUE
  TRUE
  FALSE - Ignore

SIDE EFFECTS
  None

===========================================================================*/

#define GMM_MT_DETACH_REQUEST_MSG_MIN_LENGTH 3 /* Includes all mandatory IEs */

boolean gmm_valid_mt_detach_request_message
(
  mm_cmd_type                 *message,
  gmm_mt_detach_request_msg_T *detach_request_msg
)
{
  boolean valid_msg;
  uint16  msg_length;
  uint16  msg_index;
  byte    optional_ie_index;
  uint16 gmm_max_octet_length;


/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  valid_msg  = TRUE;

  GET_IMH_LEN(msg_length,&message->cmd.mm_data_ind.message_header);
  gmm_max_octet_length = gmm_max_L3_msg_length(MM_PS_DOMAIN);

  if (msg_length < GMM_MT_DETACH_REQUEST_MSG_MIN_LENGTH)
  {
    valid_msg = FALSE;
  }

  if (valid_msg)
  {
    detach_request_msg->detach_type =
      message->cmd.mm_data_ind.L3_data[2] & 0x07;

    if ((detach_request_msg->detach_type != GMM_REATTACH_REQUIRED) &&
        (detach_request_msg->detach_type != GMM_REATTACH_NOT_REQUIRED) &&
        (detach_request_msg->detach_type != GMM_MT_IMSI_DETACH))
    {
      /* ----------------------------------------------------------
      ** All other values are interpreted as re-attach not required
      ** by this version of the protocol (24.008 Table 10.5.138)
      ** ---------------------------------------------------------- */
      detach_request_msg->detach_type = GMM_REATTACH_NOT_REQUIRED;
    }

    detach_request_msg->force_to_standby =
      (message->cmd.mm_data_ind.L3_data[2] & 0x70) >> 4;

    if (detach_request_msg->force_to_standby != 0x01)
    {
      /* -----------------------------------------------------------------------
      ** All other values are interpreted as "force to standby not indicated" by
      ** this version of the protocol (TS 24.008 Table 10.5.140)
      ** ----------------------------------------------------------------------- */
      detach_request_msg->force_to_standby = 0;
    }
  }

  if (!valid_msg)
  {
    gmm_send_gmm_status_message( INVALID_MANDATORY_INFORMATION );
  }

  /* -----------------------------------------
  ** Now parse the non-imperative message part
  ** ----------------------------------------- */
  detach_request_msg->gmm_cause_present = FALSE;

  /* ----------------------------------------------------------
  ** Used to ensure optional IE is not received out of sequence
  ** ---------------------------------------------------------- */
  optional_ie_index = 0;

  msg_index = GMM_MT_DETACH_REQUEST_MSG_MIN_LENGTH;
  if(msg_length > gmm_max_octet_length)
  {
    valid_msg = FALSE;
    MSG_ERROR_DS_0(MM_SUB,"=MM= Index out of bounds and buffer overflow");
  }
  while ((msg_index < msg_length) && (valid_msg))
  {
    switch (message->cmd.mm_data_ind.L3_data[msg_index])
    {
      /* ---------------------------------------------------------------
      ** Optional IEs listed in order specified in TS 24.008 Table 9.4.2
      ** --------------------------------------------------------------- */
      case GMM_IEI_CAUSE:
        if ((optional_ie_index == 0) &&
            ((msg_index + sizeof( gmm_ie_TV_cause_value_T )) <= msg_length))
        {
          if(msg_index+1 < gmm_max_octet_length)
          {
            detach_request_msg->gmm_cause =
              message->cmd.mm_data_ind.L3_data[msg_index+1];
          }
          else
          {
            MSG_ERROR_DS_0(MM_SUB,"=MM= Index out of bound and buffer overflow");
            valid_msg = FALSE;
            break;
          }

          detach_request_msg->gmm_cause_present = TRUE;
          optional_ie_index = 1;
        }
        msg_index = msg_index + sizeof( gmm_ie_TV_cause_value_T );
        break;

      default:
        /* ----------
        ** Unknown IE
        ** ---------- */

        if ((message->cmd.mm_data_ind.L3_data[msg_index] & 0xF0) == 0x00)
        {
          /* --------------------------------------
          ** Comprehension required (TS 24.008 8.5)
          ** -------------------------------------- */
          gmm_send_gmm_status_message( INVALID_MANDATORY_INFORMATION );
          valid_msg = FALSE;
        }
        else
        {
          /* ------------------------------------------------------------
          ** Ignore unknown IE - increment msg_index per TS 24.007 11.2.4
          ** ------------------------------------------------------------ */
          if ((message->cmd.mm_data_ind.L3_data[msg_index] & 0x80) == 0x80)
          {
            /* --------------------------------------------------------------
            ** A 1 valued bit 8 indicates that the whole IE is one octet long
            ** -------------------------------------------------------------- */
            msg_index = msg_index + 1;
          }
          else
          {
            /* ---------------------------------------------------------------------
            ** A 0 valued bit 8 indicates that the following octet is a length octet
            ** --------------------------------------------------------------------- */
            if( msg_index+1 < gmm_max_octet_length)
            {
              msg_index =
                msg_index + 2 + message->cmd.mm_data_ind.L3_data[msg_index+1];
            }
            else
            {
                MSG_ERROR_DS_0(MM_SUB,"=MM= Index out of bounds and buffer overflow");
                valid_msg = FALSE;
            }

          }
        }
        break;
    }
  } /* end while ((msg_index < msg_length) && (valid_msg)) */


  if ((detach_request_msg->detach_type == GMM_REATTACH_NOT_REQUIRED) &&
      (!detach_request_msg->gmm_cause_present))
  {
    detach_request_msg->gmm_cause_present = TRUE;
    detach_request_msg->gmm_cause = 0;
  }

  return valid_msg;

} /* end gmm_valid_mt_detach_request_message() */


/*===========================================================================

FUNCTION GMM_VALID_MO_DETACH_ACCEPT_MESSAGE

DESCRIPTION

DEPENDENCIES
  None

RETURN VALUE
  TRUE
  FALSE - Ignore

SIDE EFFECTS
  None

===========================================================================*/

#define GMM_MO_DETACH_ACCEPT_MSG_MIN_LENGTH 3 /* Includes all mandatory IEs */

boolean gmm_valid_mo_detach_accept_message
(
  mm_cmd_type                *message,
  gmm_mo_detach_accept_msg_T *detach_accept_msg
)
{
  boolean valid_msg;
#ifdef FEATURE  
  uint16 msg_length;
#else
  byte msg_length;
#endif
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  valid_msg  = TRUE;

    GET_IMH_LEN(msg_length,&message->cmd.mm_data_ind.message_header);
  
  if (msg_length < GMM_MO_DETACH_ACCEPT_MSG_MIN_LENGTH)
  {
    valid_msg = FALSE;
  }

  if (valid_msg)
  {
    detach_accept_msg->force_to_standby =
      message->cmd.mm_data_ind.L3_data[2] & 0x07;

    if (detach_accept_msg->force_to_standby != 0x01)
    {
      /* -----------------------------------------------------------------------
      ** All other values are interpreted as "force to standby not indicated" by
      ** this version of the protocol (TS 24.008 Table 10.5.140)
      ** ----------------------------------------------------------------------- */
      detach_accept_msg->force_to_standby = 0;
    }
  }
  else /* (!valid_msg) */
  {
    gmm_send_gmm_status_message( INVALID_MANDATORY_INFORMATION );
  }

  return valid_msg;

} /* end gmm_valid_mo_detach_accept_message() */


/*===========================================================================

FUNCTION GMM_VALID_PTMSI_REALLOCATION_COMMAND_MESSAGE

DESCRIPTION

DEPENDENCIES
  None

RETURN VALUE
  TRUE
  FALSE - Ignore

SIDE EFFECTS
  None

===========================================================================*/

#define GMM_PTMSI_REALLOC_COMMAND_MSG_MIN_LENGTH 15 /* Includes all mandatory IEs */

boolean gmm_valid_ptmsi_reallocation_command_message
(
  mm_cmd_type                     *message,
  gmm_ptmsi_realloc_command_msg_T *ptmsi_reallocation_command_msg
)
{
  boolean valid_msg;
  uint16  msg_index;
  byte    iei_length;
  byte    optional_ie_index;
  byte    i;
  uint16  max_L3_length;
  uint16  msg_length;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  valid_msg  = TRUE;

  GET_IMH_LEN(msg_length,&message->cmd.mm_data_ind.message_header);
  max_L3_length = gmm_max_L3_msg_length(MM_PS_DOMAIN);
  if (msg_length < GMM_PTMSI_REALLOC_COMMAND_MSG_MIN_LENGTH)
  {
    gmm_send_gmm_status_message( INVALID_MANDATORY_INFORMATION );
    valid_msg = FALSE;
  }

  if (valid_msg)
  {
    iei_length = message->cmd.mm_data_ind.L3_data[2];

    if ((message->cmd.mm_data_ind.L3_data[3] != 0xF4) ||
        (iei_length != (PTMSI_SIZE+1)))
    {
      /* -----------------------------------------
      ** IE should only be used to assign a P-TMSI
      ** ----------------------------------------- */
      gmm_send_gmm_status_message( SEMANTICALLY_INCORRECT_MESSAGE );
      valid_msg = FALSE;
    }
    else
    {
      for (i=0; i < PTMSI_SIZE; i++)
      {
        ptmsi_reallocation_command_msg->allocated_ptmsi[i] =
          message->cmd.mm_data_ind.L3_data[4+i];
      }
    }
  }

  if (valid_msg)
  {
    for (i=0; i < RAI_SIZE; i++)
    {
      ptmsi_reallocation_command_msg->routing_area_identification[i] =
        message->cmd.mm_data_ind.L3_data[8+i];
    }

    ptmsi_reallocation_command_msg->force_to_standby =
      message->cmd.mm_data_ind.L3_data[14] & 0x07;

    if (ptmsi_reallocation_command_msg->force_to_standby != 0x01)
    {
      /* -----------------------------------------------------------------------
      ** All other values are interpreted as "force to standby not indicated" by
      ** this version of the protocol (TS 24.008 Table 10.5.140)
      ** ----------------------------------------------------------------------- */
      ptmsi_reallocation_command_msg->force_to_standby = 0;
    }
  }

  /* -----------------------------------------
  ** Now parse the non-imperative message part
  ** ----------------------------------------- */
  ptmsi_reallocation_command_msg->ptmsi_signature_present   = FALSE;

  /* ----------------------------------------------------------
  ** Used to ensure optional IE is not received out of sequence
  ** ---------------------------------------------------------- */
  optional_ie_index = 0;

  msg_index = GMM_PTMSI_REALLOC_COMMAND_MSG_MIN_LENGTH;
  if(msg_length > max_L3_length )
  {
    valid_msg = FALSE;
    MSG_ERROR_DS_0(MM_SUB,"=MM= Index out of bounds and buffer overflow");
  }
  while ((msg_index < msg_length) && (valid_msg))
  {
    switch (message->cmd.mm_data_ind.L3_data[msg_index])
    {
      /* ---------------------------------------------------------------
      ** Optional IEs listed in order specified in TS 24.008 Table 9.4.2
      ** --------------------------------------------------------------- */
      case GMM_IEI_PTMSI_SIGNATURE:
        if ((optional_ie_index == 0) &&
            ((msg_index + sizeof( gmm_ie_TV_ptmsi_signature_T )) <= msg_length))
        {
          for (i=0; i < PTMSI_SIGNATURE_SIZE; i++)
          {
            if(msg_index+3+i < max_L3_length )
            {
              ptmsi_reallocation_command_msg->ptmsi_signature[i] =
                message->cmd.mm_data_ind.L3_data[msg_index+1+i];
            }
            else
            {
              MSG_ERROR_DS_0(MM_SUB,"=MM= Index out of bound and buffer overflow");
              valid_msg = FALSE;
              break;
            }
          }

          ptmsi_reallocation_command_msg->ptmsi_signature_present = TRUE;
          optional_ie_index = 1;
        }
        msg_index = msg_index + sizeof( gmm_ie_TV_ptmsi_signature_T );
        break;

      default:
        /* ----------
        ** Unknown IE
        ** ---------- */

        if ((message->cmd.mm_data_ind.L3_data[msg_index] & 0xF0) == 0x00)
        {
          /* --------------------------------------
          ** Comprehension required (TS 24.008 8.5)
          ** -------------------------------------- */
          gmm_send_gmm_status_message( INVALID_MANDATORY_INFORMATION );
          valid_msg = FALSE;
        }
        else
        {
          /* ------------------------------------------------------------
          ** Ignore unknown IE - increment msg_index per TS 24.007 11.2.4
          ** ------------------------------------------------------------ */
          if ((message->cmd.mm_data_ind.L3_data[msg_index] & 0x80) == 0x80)
          {
            /* --------------------------------------------------------------
            ** A 1 valued bit 8 indicates that the whole IE is one octet long
            ** -------------------------------------------------------------- */
            msg_index = msg_index + 1;
          }
          else
          {
            /* ---------------------------------------------------------------------
            ** A 0 valued bit 8 indicates that the following octet is a length octet
            ** --------------------------------------------------------------------- */
            if( msg_index+1 < max_L3_length )
            {
              msg_index =
                msg_index + 2 + message->cmd.mm_data_ind.L3_data[msg_index+1];
            }
            else
            {
              MSG_ERROR_DS_0(MM_SUB,"=MM= Index out of bounds and buffer overflow");
              valid_msg = FALSE;
            }

          }
        }
        break;
    }
  } /* end while ((msg_index < msg_length) && (valid_msg)) */

  return valid_msg;

} /* end gmm_valid_ptmsi_reallocation_command_message() */


/*===========================================================================

FUNCTION GMM_VALID_IDENTITY_REQUEST_MESSAGE

DESCRIPTION

DEPENDENCIES
  None

RETURN VALUE
  TRUE
  FALSE - Ignore

SIDE EFFECTS
  None

===========================================================================*/

#define GMM_IDENTITY_REQUEST_MSG_MIN_LENGTH 3 /* Includes all mandatory IEs */

boolean gmm_valid_identity_request_message
(
  mm_cmd_type                *message,
  gmm_identity_request_msg_T *identity_request_msg
)
{
  boolean valid_msg;
  uint16    msg_length;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  valid_msg  = TRUE;


  GET_IMH_LEN(msg_length,&message->cmd.mm_data_ind.message_header);

  if (msg_length < GMM_IDENTITY_REQUEST_MSG_MIN_LENGTH)
  {
    gmm_send_gmm_status_message( INVALID_MANDATORY_INFORMATION );
    valid_msg = FALSE;
  }

  if (valid_msg)
  {
    identity_request_msg->identity_type =
      message->cmd.mm_data_ind.L3_data[2] & 0x07;

    if ((identity_request_msg->identity_type != GMM_IMEI) &&
        (identity_request_msg->identity_type != GMM_IMEISV) &&
        (identity_request_msg->identity_type != GMM_TMSI))
    {
      /* --------------------------------------------------------
      ** All other values are interpreted as IMSI by this version
      ** of the protocol (TS 24.008 Table 10.5.142
      ** -------------------------------------------------------- */
      identity_request_msg->identity_type = GMM_IMSI;
    }

    identity_request_msg->force_to_standby =
      (message->cmd.mm_data_ind.L3_data[2] & 0x70) >> 4;

    if (identity_request_msg->force_to_standby != 0x01)
    {
      /* -----------------------------------------------------------------------
      ** All other values are interpreted as "force to standby not indicated" by
      ** this version of the protocol (TS 24.008 Table 10.5.140)
      ** ----------------------------------------------------------------------- */
      identity_request_msg->force_to_standby = 0;
    }
  }

  return valid_msg;

} /* end gmm_valid_routing_area_update_accept_message() */


/*===========================================================================

FUNCTION GMM_VALID_ROUTING_AREA_UPDATE_ACCEPT_MESSAGE

DESCRIPTION

DEPENDENCIES
  None

RETURN VALUE
  TRUE
  FALSE - Ignore

SIDE EFFECTS
  None

===========================================================================*/

#define GMM_RAU_ACCEPT_MSG_MIN_LENGTH 10 /* Includes all mandatory IEs */

boolean gmm_valid_routing_area_update_accept_message
(
  mm_cmd_type                *message,
  gmm_ra_update_accept_msg_T *ra_update_accept_msg
)
{
  boolean        valid_msg;
  uint16         msg_index;
  byte           iei_length;
  word           iei_length_bits;
  byte           optional_ie_index;
  byte           i, nsapi;
  csn_encoder_T  decoder;
  byte           emergency_list_length;
  boolean        tmsi_avail = FALSE; 
  uint16         max_L3_length;
  uint16         msg_length;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  valid_msg  = TRUE;

  GET_IMH_LEN(msg_length,&message->cmd.mm_data_ind.message_header);
  max_L3_length = gmm_max_L3_msg_length(MM_PS_DOMAIN);

  if (msg_length < GMM_RAU_ACCEPT_MSG_MIN_LENGTH)
  {
    valid_msg = FALSE;
  }

  if (valid_msg)
  {
    ra_update_accept_msg->force_to_standby =
      message->cmd.mm_data_ind.L3_data[2] & 0x07;

    if (ra_update_accept_msg->force_to_standby != 0x01)
    {
      /* -----------------------------------------------------------------------
      ** All other values are interpreted as "force to standby not indicated" by
      ** this version of the protocol (TS 24.008 Table 10.5.140)
      ** ----------------------------------------------------------------------- */
      ra_update_accept_msg->force_to_standby = 0;
    }

    ra_update_accept_msg->update_result =
      (message->cmd.mm_data_ind.L3_data[2] & 0x70) >> 4;

#ifdef FEATURE_REL6_PS_FOP
    {
      ra_update_accept_msg->follow_on_proceed_response = 
              (message->cmd.mm_data_ind.L3_data[2] & 0x80) >> 7;
    }        
#endif/* FEATURE_REL6_PS_FOP */

    if ((ra_update_accept_msg->update_result != GMM_RA_UPDATED) &&
        (ra_update_accept_msg->update_result != GMM_COMBINED_RA_LA_UPDATED)
#if defined(FEATURE_LTE)
        && (ra_update_accept_msg->update_result != GMM_COMBINED_RA_LA_UPDATED_AND_ISR_ACTIVATED)
         && (ra_update_accept_msg->update_result != GMM_RA_UPDATED_AND_ISR_ACTIVATED)
#endif
        )
    {
      valid_msg = FALSE;
    }
  }

  if (valid_msg)
  {
    ra_update_accept_msg->periodic_ra_update_timer =
      message->cmd.mm_data_ind.L3_data[3];

    for (i=0; i < RAI_SIZE; i++)
    {
      ra_update_accept_msg->routing_area_identification[i] =
        message->cmd.mm_data_ind.L3_data[4+i];
    }
  }

  if (!valid_msg)
  {
    gmm_send_gmm_status_message( INVALID_MANDATORY_INFORMATION );
  }

  /* -----------------------------------------
  ** Now parse the non-imperative message part
  ** ----------------------------------------- */
  ra_update_accept_msg->ptmsi_signature_present              = FALSE;
  ra_update_accept_msg->allocated_ptmsi_present              = FALSE;
  ra_update_accept_msg->ms_identity_present                  = FALSE;
  ra_update_accept_msg->list_of_receive_npdu_numbers_present = FALSE;
  ra_update_accept_msg->ready_timer_value_present            = FALSE;
  ra_update_accept_msg->gmm_cause_present                    = FALSE;
  ra_update_accept_msg->t3302_value_present                  = FALSE;
  ra_update_accept_msg->cell_notification_present            = FALSE;
  ra_update_accept_msg->equiv_plmn_list_present              = FALSE;
  ra_update_accept_msg->emergency_number_list_present        = FALSE;
  memset(&(ra_update_accept_msg->emergency_number_list), 0,
     sizeof(mmr_emergency_num_list_s_type));
  ra_update_accept_msg->pdp_context_status_present           = FALSE;
  memset(&(ra_update_accept_msg->pdp_context_status), 0,
     sizeof(gmm_ie_pdp_context_status_T));
  ra_update_accept_msg->nw_ftr_support_present               = FALSE; 
  ra_update_accept_msg->nw_ftr_support                       = 0;     
  ra_update_accept_msg->mbms_context_status_present          = FALSE;
  memset(&(ra_update_accept_msg->mbms_context_status), 0,
     sizeof(gmm_ie_mbms_context_status_T));   
#if defined(FEATURE_LTE)
  ra_update_accept_msg->t3323_value_present = FALSE;
#endif
  ra_update_accept_msg->t3319_value_present                   = FALSE;
#ifdef FEATURE_NAS_REL10
  ra_update_accept_msg->ext_t3312_value_present               = FALSE;
#endif



  /* ----------------------------------------------------------
  ** Used to ensure optional IE is not received out of sequence
  ** ---------------------------------------------------------- */
  optional_ie_index = 0;

  msg_index = GMM_RAU_ACCEPT_MSG_MIN_LENGTH;
  if(msg_length > max_L3_length)
  {
    valid_msg = FALSE;
    MSG_ERROR_DS_0(MM_SUB,"=MM= Index out of bounds and buffer overflow");
  }
  while ((msg_index < msg_length) && (valid_msg))
  {
    switch (message->cmd.mm_data_ind.L3_data[msg_index])
    {
      /* ---------------------------------------------------------------
      ** Optional IEs listed in order specified in TS 24.008 Table 9.4.2
      ** --------------------------------------------------------------- */
      case GMM_IEI_PTMSI_SIGNATURE:
        if ((optional_ie_index == 0) &&
            ((msg_index + sizeof( gmm_ie_TV_ptmsi_signature_T )) <= msg_length))
        {
          for (i=0; i < PTMSI_SIGNATURE_SIZE; i++)
          {
            ra_update_accept_msg->ptmsi_signature[i] =
              message->cmd.mm_data_ind.L3_data[msg_index+1+i];
          }

          ra_update_accept_msg->ptmsi_signature_present = TRUE;
          optional_ie_index = 1;
        }
        msg_index = msg_index + sizeof( gmm_ie_TV_ptmsi_signature_T );
        break;

      case GMM_IEI_PTMSI:
        if(msg_index+1 < msg_length)
        {
          iei_length = message->cmd.mm_data_ind.L3_data[msg_index+1];
        }
        else
        {
          MSG_ERROR_DS_0(MM_SUB,"=MM= Index out of bound and buffer overflow");
          valid_msg = FALSE;
          break;
        }
        if ((optional_ie_index <= 1) &&
            ((msg_index + 2+ iei_length) <= msg_length))
        {
          if ((message->cmd.mm_data_ind.L3_data[msg_index+2] != 0xF4) ||
              (iei_length != (PTMSI_SIZE+1)))
          {
            /* -----------------------------------------
            ** IE should only be used to assign a P-TMSI
            ** ----------------------------------------- */
            gmm_send_gmm_status_message( SEMANTICALLY_INCORRECT_MESSAGE );
            valid_msg = FALSE;
          }
          else
          {
            for (i=0; i < PTMSI_SIZE; i++)
            {
              ra_update_accept_msg->allocated_ptmsi[i] =
                message->cmd.mm_data_ind.L3_data[msg_index+3+i];
            }

            ra_update_accept_msg->allocated_ptmsi_present = TRUE;
            optional_ie_index = 2;
          }
        }
        msg_index = msg_index + 2 + iei_length;
        break;

      case GMM_IEI_MS_IDENTITY:
        if(msg_index+1 < msg_length)
        {
          iei_length = message->cmd.mm_data_ind.L3_data[msg_index+1];
        }
        else
        {
          MSG_ERROR_DS_0(MM_SUB,"=MM= Index out of bound and buffer overflow");
          valid_msg = FALSE;
          break;
        }

        if ((optional_ie_index <= 2) &&
            ((msg_index + 2 + iei_length) <= msg_length))
        {
          ra_update_accept_msg->ms_identity.id_type =
            message->cmd.mm_data_ind.L3_data[msg_index+2];
          
          if ((ra_update_accept_msg->ms_identity.id_type & 0x07) == GMM_TMSI)
          {
            if ((message->cmd.mm_data_ind.L3_data[msg_index+2] != 0xF4) ||
               (iei_length != (TMSI_SIZE+1)))
            {
               gmm_send_gmm_status_message( SEMANTICALLY_INCORRECT_MESSAGE );
               valid_msg = FALSE;
            }
            else
            {
              for (i=0; i < (iei_length-1); i++)
              {
                 ra_update_accept_msg->ms_identity.value[i] =
                   message->cmd.mm_data_ind.L3_data[msg_index+3+i];
              }
               ra_update_accept_msg->ms_identity_present = TRUE;
               tmsi_avail = TRUE;
               optional_ie_index = 3;
            }
          }
          else
          {
            ra_update_accept_msg->ms_identity_present = TRUE;
            optional_ie_index = 3;
          }
        }
        msg_index = msg_index + 2 + iei_length;
        break;

      case GMM_IEI_RECEIVE_NPDU_NUMBERS_LIST:
        if(msg_index+1 < msg_length)
        {
          iei_length = message->cmd.mm_data_ind.L3_data[msg_index+1];
        }
        else
        {
          MSG_ERROR_DS_0(MM_SUB,"=MM= Index out of bound and buffer overflow");
          valid_msg = FALSE;
          break;
        }

        if ((optional_ie_index <= 3) &&
            (iei_length <= GMM_RECEIVE_NPDU_NUMBER_LIST_MAX_LENGTH) &&
            ((byte)(msg_index + 2 + iei_length ) <= msg_length))
        {
          /* Clear out the NPDU elements of the accept message structure. */
          memset ( ra_update_accept_msg -> nsapi_valid_list, FALSE, GMM_RECEIVE_NPDU_NUMBER_LIST_MAX_NSAPIS);
          memset ( ra_update_accept_msg -> npdu_number_list, 0, GMM_RECEIVE_NPDU_NUMBER_LIST_MAX_NSAPIS);

          /* Initialise the CSN Decoder to the point just after the IEI Length */

          
          csn_encoder_init ( &decoder, &message->cmd.mm_data_ind.L3_data[msg_index+2], iei_length);
          
          ra_update_accept_msg->list_of_receive_npdu_numbers_present = TRUE;

          iei_length_bits = iei_length * 8;

          /* Ensure there are enough bits to decode the another NPDU number
             element comprising NSAPI (4 bits) and NPDU Number (8 bits) */

          while ( (iei_length_bits - csn_bit_pos(&decoder)) >= 12 )
          {
            nsapi = csn_decode_bits( &decoder, 4 );

            /* Ensure that NSAPI address is valid and that there are sufficient
               CSN bits left to justify a decode for this NSAPI element */

            if ( (nsapi >= GMM_RECEIVE_NPDU_NUMBER_LIST_MIN_NSAPI) &&
                 (nsapi <= GMM_RECEIVE_NPDU_NUMBER_LIST_MAX_NSAPI)    )
            {
               ra_update_accept_msg ->
                 nsapi_valid_list[nsapi-GMM_RECEIVE_NPDU_NUMBER_LIST_MIN_NSAPI] = TRUE;
               ra_update_accept_msg ->
                 npdu_number_list[nsapi-GMM_RECEIVE_NPDU_NUMBER_LIST_MIN_NSAPI] =
                   csn_decode_bits( &decoder, 8 );
            }
            else
            {
              /* Something has gone wrong with decoding, flag error and
                 pretend the IE was not present. */
              ra_update_accept_msg->list_of_receive_npdu_numbers_present = FALSE;

              MSG_ERROR_DS_0(MM_SUB, "=MM= Error decoding NPDU Number List");

              break;
            }

          } /* while (iei_lenght_bits) */

          optional_ie_index = 4;
        }

        msg_index = msg_index + 2 + iei_length;
        break;

      case GMM_IEI_READY_TIMER_VALUE:
        if ((optional_ie_index <= 4) &&
            ((msg_index + sizeof( gmm_ie_TV_timer_value_T )) <= msg_length))
        {
          ra_update_accept_msg->ready_timer_value =
            message->cmd.mm_data_ind.L3_data[msg_index+1];
          
          ra_update_accept_msg->ready_timer_value_present = TRUE;
          optional_ie_index = 5;
        }
        msg_index = msg_index + sizeof( gmm_ie_TV_timer_value_T );
        break;

      case GMM_IEI_CAUSE:
        if ((optional_ie_index <= 5) &&
            ((msg_index + sizeof( gmm_ie_TV_cause_value_T )) <= msg_length))
        {
          ra_update_accept_msg->gmm_cause =
            message->cmd.mm_data_ind.L3_data[msg_index+1];
          ra_update_accept_msg->gmm_cause_present = TRUE;
          optional_ie_index = 6;
        }
        msg_index = msg_index + sizeof( gmm_ie_TV_cause_value_T );
        break;

      case GMM_IEI_T3302_VALUE:
        if ((optional_ie_index <= 6) &&
            ((msg_index + sizeof( gmm_ie_TLV_timer_value_T )) <= msg_length))
        {
          ra_update_accept_msg->t3302_value =
            message->cmd.mm_data_ind.L3_data[msg_index+2];
        
          ra_update_accept_msg->t3302_value_present = TRUE;
          optional_ie_index = 7;
        }
        msg_index = msg_index + sizeof( gmm_ie_TLV_timer_value_T );
        break;

      case GMM_IEI_CELL_NOTIFICATION:
        if ((optional_ie_index <= 7) && ((msg_index + 1) <= msg_length))
        {
          ra_update_accept_msg->cell_notification_present = TRUE;
          optional_ie_index = 8;
        }
        msg_index = msg_index + 1;
        break;

      case IEI_EQUIV_PLMN:
        if((msg_index+1) < msg_length)
        {
          ra_update_accept_msg->equiv_plmn_list.length = message->cmd.mm_data_ind.L3_data[msg_index+1];
        }
        else
        {
          MSG_ERROR_DS_0(MM_SUB,"=MM= Index out of bound and buffer overflow");
          valid_msg = FALSE;
          break;
        }
        if ((optional_ie_index <= 8) &&
            (ra_update_accept_msg->equiv_plmn_list.length >0) &&
              ((msg_index + 2 + ra_update_accept_msg->equiv_plmn_list.length) <= msg_length))
        {
          /* (24.008, 4.4.1) The maximum number of entries in the stored equivalent
          ** PLMN list is six (5 equiv PLMN + PLMN of the network that downloaded
          ** the list.
          */
          /* (24.008, 4.4.1) The maximum number of entries in the stored equivalent
          ** PLMN list is sixteen in REL 6 (15 equiv PLMN + PLMN of the network that downloaded
          ** the list.
          */
          
          if (ra_update_accept_msg->equiv_plmn_list.length > (EQUIVALENT_PLMN_LIST_MAX_LENGTH * 3))
          {
            ra_update_accept_msg->equiv_plmn_list.length = EQUIVALENT_PLMN_LIST_MAX_LENGTH * 3;
          }

          /*
          ** Copy the equivalent PLMN list to the msg structure
          */
          memscpy(ra_update_accept_msg->equiv_plmn_list.plmn, sizeof(sys_plmn_id_s_type)*EQUIVALENT_PLMN_LIST_MAX_LENGTH,
                      &(message->cmd.mm_data_ind.L3_data[msg_index+2]),
                      ra_update_accept_msg->equiv_plmn_list.length);
          ra_update_accept_msg->equiv_plmn_list_present  = TRUE;
          optional_ie_index = 9;
        }
        msg_index = msg_index + 2 + ra_update_accept_msg->equiv_plmn_list.length;

        MSG_MED_DS_0(MM_SUB, "=MM= Rec'd Equiv PLMN list");
        break;

      case GMM_IEI_PDP_CONTEXT_STATUS:
        if ((msg_index+1) < msg_length)
        {
          if (( optional_ie_index <= 9 ) &&
              ((msg_index + 1 + MIN(message->cmd.mm_data_ind.L3_data[msg_index+1], 
                          GMM_PDP_CONTEXT_STATUS_LENGTH)) < msg_length))
          {
            ra_update_accept_msg->pdp_context_status_present = TRUE;
            gmm_parse_pdp_context_status_ie
            (
              &(ra_update_accept_msg->pdp_context_status),
              &(message->cmd.mm_data_ind.L3_data[msg_index+1])
            );  
            optional_ie_index = 10;
          }
          msg_index = msg_index + 2 + message->cmd.mm_data_ind.L3_data[msg_index+1];
        }
        else
        {
          MSG_ERROR_DS_0(MM_SUB,"=MM= PDP_CONTEXT_STATUS IE is out of message boundary");
          valid_msg = FALSE;
        }
        break;


      case IEI_EMERGENCY_NUM_LIST:

        if(msg_index+1 < msg_length)
        {
          emergency_list_length = message->cmd.mm_data_ind.L3_data[msg_index+1];
        }
        else
        {
          MSG_ERROR_DS_0(MM_SUB,"=MM= Index out of bound and buffer overflow");
          valid_msg = FALSE;
          break;
        }

        if ((optional_ie_index <= 11) && (emergency_list_length > 0)&&
            ((msg_index + emergency_list_length) <= msg_length))
        {
            mm_parse_emergency_number(&(ra_update_accept_msg->emergency_number_list),
                        &(message->cmd.mm_data_ind.L3_data[msg_index+1]) );
            optional_ie_index = 12;
     		ra_update_accept_msg->emergency_number_list_present = TRUE;
        }/*Optional Index valid*/

        msg_index = msg_index + 2 + emergency_list_length;
		break;

      case GMM_IEI_MBMS_CONTEXT_STATUS:

        if ((msg_index+1) < msg_length)
        {
          if (( optional_ie_index <= 12 ) &&
               ((msg_index + 1 + MIN(message->cmd.mm_data_ind.L3_data[msg_index+1],
                          GMM_MBMS_CONTEXT_STATUS_LENGTH) )) < msg_length)
          {
            ra_update_accept_msg->mbms_context_status_present = TRUE;
            gmm_parse_mbms_context_status_ie
            (
              &(ra_update_accept_msg->mbms_context_status),
              &(message->cmd.mm_data_ind.L3_data[msg_index+1])
            );
            optional_ie_index = 13;
          }
          msg_index = msg_index + 2 + message->cmd.mm_data_ind.L3_data[msg_index+1];
        }
        else
        {
          MSG_ERROR_DS_0(MM_SUB,"=MM= MBMS_CONTEXT_STATUS IE is out of message boundary");
          valid_msg = FALSE;
        }
        break;
        /* Spec_ref: 9.4.15       
              Optional IE in RAU_Accept*/
      case GMM_IEI_T3319_VALUE:
        if ((optional_ie_index <= 14) &&
            ((msg_index + sizeof( gmm_ie_TLV_timer_value_T )) <= msg_length))
        {
          if (msg_index+2 < max_L3_length)
          {
            ra_update_accept_msg->t3319_value =
              message->cmd.mm_data_ind.L3_data[msg_index+2];
          }
          else
          {
            MSG_ERROR_DS_0(MM_SUB,"Index out of bound and buffer overflow");
          valid_msg = FALSE;
            break;
        }
          ra_update_accept_msg->t3319_value_present = TRUE;
          optional_ie_index = 15;
        }
        msg_index = msg_index + sizeof( gmm_ie_TLV_timer_value_T );
        break;

#if defined(FEATURE_LTE)
      case GMM_IEI_T3323_VALUE:

        if ((optional_ie_index <= 15) &&
            ((msg_index + sizeof( gmm_ie_TLV_timer_value_T )) <= msg_length))
        {
          ra_update_accept_msg->t3323_value =
            message->cmd.mm_data_ind.L3_data[msg_index+2];
          
          ra_update_accept_msg->t3323_value_present = TRUE;
          optional_ie_index = 16;
        }
        msg_index = msg_index + sizeof( gmm_ie_TLV_timer_value_T );
        break;
#endif

#ifdef FEATURE_NAS_REL10
      case GMM_IEI_EX_T3312_VALUE:
      
       if ((optional_ie_index <= 16) &&
           ((msg_index + sizeof( gmm_ie_TLV_timer_value_T )) <= msg_length))
       {
         ra_update_accept_msg->ext_t3312_value=
           message->cmd.mm_data_ind.L3_data[msg_index+2];
                
         ra_update_accept_msg->ext_t3312_value_present= TRUE;
         optional_ie_index = 17;
       }
       msg_index = msg_index + sizeof( gmm_ie_TLV_timer_value_T );
       break;
#endif


      default:

       if ((message->cmd.mm_data_ind.L3_data[msg_index] & 0xF0) ==
                GMM_IEI_NW_FTR_SUPPORT)
        {
          if ((optional_ie_index <= 10) &&  ((msg_index +1) <= msg_length))
          {
            ra_update_accept_msg->nw_ftr_support_present  = TRUE;
            
          gmm_decode_nw_ftr_support
          (
            &(message->cmd.mm_data_ind.L3_data[msg_index]),
            &(ra_update_accept_msg->nw_ftr_support)
          );
            optional_ie_index = 11;
          }
           msg_index = msg_index + 1;
        }
        else if((message->cmd.mm_data_ind.L3_data[msg_index] & 0xF0) ==
                GMM_IEI_REQUESTED_MS_INFORMATION)
        {
          if ((optional_ie_index <= 13) &&  ((msg_index +1) <= msg_length))
          {
           ra_update_accept_msg->req_ms_info = (message->cmd.mm_data_ind.L3_data[msg_index]) & 0x0F;
           optional_ie_index = 14;  
          }
          msg_index = msg_index + 1;
        }

        /* ----------
        ** Unknown IE
        ** ---------- */
        else if ((message->cmd.mm_data_ind.L3_data[msg_index] & 0xF0) == 0x00)
        {
          /* --------------------------------------
          ** Comprehension required (TS 24.008 8.5)
          ** -------------------------------------- */
          gmm_send_gmm_status_message( INVALID_MANDATORY_INFORMATION );
          valid_msg = FALSE;
        }
        else
        {
          /* ------------------------------------------------------------
          ** Ignore unknown IE - increment msg_index per TS 24.007 11.2.4
          ** ------------------------------------------------------------ */
          if ((message->cmd.mm_data_ind.L3_data[msg_index] & 0x80) == 0x80)
          {
            /* --------------------------------------------------------------
            ** A 1 valued bit 8 indicates that the whole IE is one octet long
            ** -------------------------------------------------------------- */
            msg_index = msg_index + 1;
          }
          else
          {
            /* ---------------------------------------------------------------------
            ** A 0 valued bit 8 indicates that the following octet is a length octet
            ** --------------------------------------------------------------------- */
            
            if( msg_index+1 < msg_length)
            {
              msg_index =
                msg_index + 2 + message->cmd.mm_data_ind.L3_data[msg_index+1];
            }
            else
            {
              MSG_ERROR_DS_0(MM_SUB,"=MM= Index out of bounds and buffer overflow");
              valid_msg = FALSE;
            }
          }
        }
        break;
    }
  } /* end while ((msg_index < msg_length) && (valid_msg)) */

  if (valid_msg)
  {
    if( (gmm_state == GMM_REGISTERED) &&(is_gsm_mode()))
    {
       if ((mm_timer_status[TIMER_T3350] == TIMER_STOPPED) ||
           (gmm_is_ptmsi_present_in_prev_gmm_msg != ra_update_accept_msg->allocated_ptmsi_present)||
           ((gmm_is_ptmsi_present_in_prev_gmm_msg) &&
            (memcmp(gmm_stored_ptmsi.digit,ra_update_accept_msg->allocated_ptmsi,PTMSI_SIZE)!= 0))||
           (gmm_is_tmsi_present_in_prev_gmm_msg != tmsi_avail))
       {
         valid_msg = FALSE;
         gmm_send_gmm_status_message(
                    MESSAGE_NOT_COMPATIBLE_WITH_PROTOCOL_STATE );
       }
    } 
  }

#ifdef FEATURE_LTE
  /* Since LTE cannot continue without P-TMSI allocated in GW, GMM Attach Accept or RAU Accept without P-TMSI 
  ** allocated shall be blocked by dropping incoming message but no Status OTA cause matches with this error */
  if (!mm_has_valid_ptmsi() &&
      !ra_update_accept_msg->allocated_ptmsi_present)
  {
    valid_msg = FALSE;
  }
#endif

  return valid_msg;

} /* end gmm_valid_routing_area_update_accept_message() */


/*===========================================================================

FUNCTION GMM_VALID_ROUTING_AREA_UPDATE_REJECT_MESSAGE

DESCRIPTION

DEPENDENCIES
  None

RETURN VALUE
  TRUE
  FALSE - Ignore

SIDE EFFECTS
  None

===========================================================================*/

#define GMM_RAU_REJECT_MSG_MIN_LENGTH 4 /* Includes all mandatory IEs */

boolean gmm_valid_routing_area_update_reject_message
(
  mm_cmd_type                *message,
  gmm_ra_update_reject_msg_T *ra_update_reject_msg
)
{
  boolean valid_msg;
  uint16  msg_index;
  byte    optional_ie_index;
  uint16 max_L3_length;
  uint16    msg_length;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  valid_msg  = TRUE;
  GET_IMH_LEN(msg_length,&message->cmd.mm_data_ind.message_header);
  max_L3_length = gmm_max_L3_msg_length(MM_PS_DOMAIN);

  if (msg_length < GMM_RAU_REJECT_MSG_MIN_LENGTH)
  {
    valid_msg = FALSE;
  }

  if (valid_msg)
  {
    ra_update_reject_msg->gmm_cause = message->cmd.mm_data_ind.L3_data[2];

    ra_update_reject_msg->force_to_standby =
      message->cmd.mm_data_ind.L3_data[3] & 0x07;

    if (ra_update_reject_msg->force_to_standby != 0x01)
    {
      /* -----------------------------------------------------------------------
      ** All other values are interpreted as "force to standby not indicated" by
      ** this version of the protocol (TS 24.008 Table 10.5.140)
      ** ----------------------------------------------------------------------- */
      ra_update_reject_msg->force_to_standby = 0;
    }
  }

  if (!valid_msg)
  {
    gmm_send_gmm_status_message( INVALID_MANDATORY_INFORMATION );
  }

  /* -----------------------------------------
  ** Now parse the non-imperative message part
  ** ----------------------------------------- */
  ra_update_reject_msg->t3302_value_present = FALSE;
#ifdef FEATURE_NAS_REL10
  ra_update_reject_msg->t3346_value_present = FALSE;
#endif

  /* ----------------------------------------------------------
  ** Used to ensure optional IE is not received out of sequence
  ** ---------------------------------------------------------- */
  optional_ie_index = 0;

  msg_index = GMM_RAU_REJECT_MSG_MIN_LENGTH;
  if(msg_length > max_L3_length)
  {
    valid_msg = FALSE;
    MSG_ERROR_DS_0(MM_SUB,"=MM= Index out of bounds and buffer overflow");
  }
  while ((msg_index < msg_length) && (valid_msg))
  {
    switch (message->cmd.mm_data_ind.L3_data[msg_index])
    {
      /* ---------------------------------------------------------------
      ** Optional IEs listed in order specified in TS 24.008 Table 9.4.2
      ** --------------------------------------------------------------- */
      case GMM_IEI_T3302_VALUE:
        if ((optional_ie_index == 0) &&
            ((msg_index + sizeof( gmm_ie_TLV_timer_value_T )) <= msg_length))
        {
          if(msg_index+2 < max_L3_length)
          {
            ra_update_reject_msg->t3302_value =
              message->cmd.mm_data_ind.L3_data[msg_index+2];
          }
          else
          {
            MSG_ERROR_DS_0(MM_SUB,"=MM= Index out of bound and buffer overflow");
            valid_msg = FALSE;
            break;
          }
          ra_update_reject_msg->t3302_value_present = TRUE;
          optional_ie_index = 1;
        }
        msg_index = msg_index + sizeof( gmm_ie_TLV_timer_value_T );
        break;
#ifdef FEATURE_NAS_REL10
      case GMM_IEI_T3346_VALUE:
        if ((optional_ie_index <= 1) &&
            ((msg_index + sizeof( gmm_ie_TLV_timer_value_T )) <= msg_length))
        {
          if(msg_index+2 < max_L3_length)
          {
            ra_update_reject_msg->t3346_value =
              message->cmd.mm_data_ind.L3_data[msg_index+2];
          }
          else
          {
            MSG_ERROR_DS_0(MM_SUB,"=MM= Index out of bound and buffer overflow");
            valid_msg = FALSE;
            break;
          }
          ra_update_reject_msg->t3346_value_present = TRUE;
          optional_ie_index = 2;
        }
        msg_index = msg_index + sizeof( gmm_ie_TLV_timer_value_T );
        break;
#endif

      default:
        /* ----------
        ** Unknown IE
        ** ---------- */
        if ((message->cmd.mm_data_ind.L3_data[msg_index] & 0xF0) == 0x00)
        {
          /* --------------------------------------
          ** Comprehension required (TS 24.008 8.5)
          ** -------------------------------------- */
          gmm_send_gmm_status_message( INVALID_MANDATORY_INFORMATION );
          valid_msg = FALSE;
        }
        else
        {
          /* ------------------------------------------------------------
          ** Ignore unknown IE - increment msg_index per TS 24.007 11.2.4
          ** ------------------------------------------------------------ */
          if ((message->cmd.mm_data_ind.L3_data[msg_index] & 0x80) == 0x80)
          {
            /* --------------------------------------------------------------
            ** A 1 valued bit 8 indicates that the whole IE is one octet long
            ** -------------------------------------------------------------- */
            msg_index = msg_index + 1;
          }
          else
          {
            /* ---------------------------------------------------------------------
            ** A 0 valued bit 8 indicates that the following octet is a length octet
            ** --------------------------------------------------------------------- */
            
            if( msg_index+1 < max_L3_length)
            {
              msg_index =
                msg_index + 2 + message->cmd.mm_data_ind.L3_data[msg_index+1];
            }
            else
            {
              MSG_ERROR_DS_0(MM_SUB,"=MM= Index out of bounds and buffer overflow");
              valid_msg = FALSE;
            }
          }
        }
        break;
    }
  } /* end while ((msg_index < msg_length) && (valid_msg)) */

  return valid_msg;

} /* end gmm_valid_routing_area_update_reject_message() */


/*===========================================================================

FUNCTION GMM_VALID_GMM_STATUS_MESSAGE

DESCRIPTION

DEPENDENCIES
  None

RETURN VALUE
  TRUE
  FALSE - Ignore

SIDE EFFECTS
  None

===========================================================================*/

#define GMM_STATUS_MSG_MIN_LENGTH 3 /* Includes all mandatory IEs */

boolean gmm_valid_gmm_status_message
(
  mm_cmd_type             *message,
  gmm_mt_gmm_status_msg_T *gmm_status_msg
)
{
  boolean valid_msg;
  uint16    msg_length;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  valid_msg  = TRUE;

  GET_IMH_LEN(msg_length,&message->cmd.mm_data_ind.message_header);

  if (msg_length < GMM_STATUS_MSG_MIN_LENGTH)
  {
    gmm_send_gmm_status_message( INVALID_MANDATORY_INFORMATION );
    valid_msg = FALSE;
  }

  if (valid_msg)
  {
    gmm_status_msg->gmm_cause = message->cmd.mm_data_ind.L3_data[2];
  }

  return valid_msg;

} /* end gmm_valid_gmm_status_message() */


/*===========================================================================

FUNCTION GMM_VALID_SERVICE_ACCEPT_MESSAGE

DESCRIPTION

DEPENDENCIES
  None

RETURN VALUE
  TRUE
  FALSE - Ignore

SIDE EFFECTS
  None

===========================================================================*/

#define GMM_SERVICE_ACCEPT_MSG_MIN_LENGTH 2 /* Includes all mandatory IEs */

boolean gmm_valid_service_accept_message( mm_cmd_type              *message,
                                          gmm_service_accept_msg_T *service_accept_msg)
{
  boolean valid_msg;
  uint16  msg_index;
  byte    optional_ie_index = 0;
  uint16 msg_length;
  uint16 max_L3_message;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  valid_msg  = TRUE;

  max_L3_message = gmm_max_L3_msg_length(MM_PS_DOMAIN);
  
  GET_IMH_LEN(msg_length,&message->cmd.mm_data_ind.message_header);

  if (msg_length < GMM_SERVICE_ACCEPT_MSG_MIN_LENGTH)
  {
    gmm_send_gmm_status_message( INVALID_MANDATORY_INFORMATION );
    valid_msg = FALSE;
  }

  msg_index = GMM_SERVICE_ACCEPT_MSG_MIN_LENGTH;
  
  service_accept_msg->pdp_context_status_present           = FALSE;
  memset(&(service_accept_msg->pdp_context_status), 0,
     sizeof(gmm_ie_pdp_context_status_T));     
  service_accept_msg->mbms_context_status_present          = FALSE;
  memset(&(service_accept_msg->mbms_context_status), 0,
     sizeof(gmm_ie_mbms_context_status_T));
  if(msg_length > max_L3_message)
  {
    valid_msg = FALSE;
    MSG_ERROR_DS_0(MM_SUB,"=MM= Index out of bounds and buffer overflow");
  }
  while ((msg_index < msg_length) && (valid_msg))
  {
    switch(message->cmd.mm_data_ind.L3_data[msg_index])
    {
      case GMM_IEI_PDP_CONTEXT_STATUS:
        if (msg_index+1 < MIN(msg_length,max_L3_message))
        {
          if ((optional_ie_index == 0) &&
              ((msg_index + 1 + MIN(message->cmd.mm_data_ind.L3_data[msg_index+1],
                          GMM_PDP_CONTEXT_STATUS_LENGTH)) < msg_length))
          {
            service_accept_msg->pdp_context_status_present = TRUE;
            gmm_parse_pdp_context_status_ie(&(service_accept_msg->pdp_context_status),
                                   &(message->cmd.mm_data_ind.L3_data[msg_index+1]));
            optional_ie_index = 1;
          }
          msg_index = msg_index + 2 + message->cmd.mm_data_ind.L3_data[msg_index+1];
        }
        else
        {
          MSG_ERROR_DS_0(MM_SUB,"=MM= PDP_CONTEXT_STATUS IE is out of message boundary");
          valid_msg = FALSE;
        }
        break;

      case GMM_IEI_MBMS_CONTEXT_STATUS:
        if (msg_index+1 < MIN(msg_length,max_L3_message))
        {
          if (( optional_ie_index <= 1 ) &&
              ((msg_index + 1 + MIN(message->cmd.mm_data_ind.L3_data[msg_index+1],
                          GMM_MBMS_CONTEXT_STATUS_LENGTH)) < msg_length))
          {
            service_accept_msg->mbms_context_status_present = TRUE;
            gmm_parse_mbms_context_status_ie
            (
              &(service_accept_msg->mbms_context_status),
              &(message->cmd.mm_data_ind.L3_data[msg_index+1])
            );
            optional_ie_index = 2;
          }
          msg_index = msg_index + 2 + message->cmd.mm_data_ind.L3_data[msg_index+1];
        }
        else
        {
          MSG_ERROR_DS_0(MM_SUB,"=MM= MBMS_CONTEXT_STATUS IE is out of message boundary");
          valid_msg = FALSE;
        }
        break;
        
      default:
        if ((message->cmd.mm_data_ind.L3_data[msg_index] & 0xF0) == 0x00)
        {
          /* --------------------------------------
          ** Comprehension required (TS 24.008 8.5)
          ** -------------------------------------- */
          gmm_send_gmm_status_message( INVALID_MANDATORY_INFORMATION );
          valid_msg = FALSE;
        }
        else
        {
          /* ------------------------------------------------------------
          ** Ignore unknown IE - increment msg_index per TS 24.007 11.2.4
          ** ------------------------------------------------------------ */
          if ((message->cmd.mm_data_ind.L3_data[msg_index] & 0x80) == 0x80)
          {
            /* --------------------------------------------------------------
            ** A 1 valued bit 8 indicates that the whole IE is one octet long
            ** -------------------------------------------------------------- */
            msg_index = msg_index + 1;
          }
          else
          {
            /* ---------------------------------------------------------------------
            ** A 0 valued bit 8 indicates that the following octet is a length octet
            ** --------------------------------------------------------------------- */
            if( msg_index+1 < max_L3_message)
            {
              msg_index = msg_index + 2 + message->cmd.mm_data_ind.L3_data[msg_index+1];
            }
            else
            {
              MSG_ERROR_DS_0(MM_SUB,"=MM= Index out of bounds and buffer overflow");
              valid_msg = FALSE;
            }
          }
        }
        break;
    }
  }

  return valid_msg;

} /* end gmm_valid_service_accept_message() */


/*===========================================================================

FUNCTION GMM_VALID_SERVICE_REJECT_MESSAGE

DESCRIPTION

DEPENDENCIES
  None

RETURN VALUE
  TRUE
  FALSE - Ignore

SIDE EFFECTS
  None

===========================================================================*/

#define GMM_SERVICE_REJECT_MSG_MIN_LENGTH 3 /* Includes all mandatory IEs */

boolean gmm_valid_service_reject_message
(
  mm_cmd_type              *message,
  gmm_service_reject_msg_T *service_reject_msg
)
{
  boolean valid_msg;
  uint16    msg_length;

#ifdef FEATURE_NAS_REL10
  byte    msg_index  = 0; 
#endif
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
#ifdef FEATURE_NAS_REL10
  service_reject_msg->t3346_value_present = FALSE;
#endif

  valid_msg  = TRUE;

  GET_IMH_LEN(msg_length,&message->cmd.mm_data_ind.message_header);

  if (msg_length < GMM_SERVICE_REJECT_MSG_MIN_LENGTH)
  {
    gmm_send_gmm_status_message( INVALID_MANDATORY_INFORMATION );
    valid_msg = FALSE;
  }

  if (valid_msg)
  {
    service_reject_msg->gmm_cause = message->cmd.mm_data_ind.L3_data[2];

#ifdef FEATURE_NAS_REL10
    msg_index = 3;
    if(message->cmd.mm_data_ind.L3_data[msg_index] == GMM_IEI_T3346_VALUE)
    {
      service_reject_msg->t3346_value = message->cmd.mm_data_ind.L3_data[msg_index+2];
      service_reject_msg->t3346_value_present = TRUE;
      msg_index = msg_index + message->cmd.mm_data_ind.L3_data[msg_index+1] + 2;
    }
#endif
  }

  return valid_msg;

} /* end gmm_valid_service_reject_message() */

/*===========================================================================

FUNCTION gmm_check_if_ciphering_algo_supported

DESCRIPTION
  This function checks if in input ciphering algorithm is supported by UE or not.

DEPENDENCIES
  None

RETURN VALUE
  TRUE if input ciphering algorithm is supported, FALSE otherwise.

SIDE EFFECTS
  None

===========================================================================*/

boolean gmm_check_if_ciphering_algo_supported(byte ciphering_algorithm)
{
  gprs_hw_classmark_T classmark;

#ifdef FEATURE_DUAL_SIM
  nas_read_gprs_hw_classmark_per_subs(&classmark,(sys_modem_as_id_e_type)mm_sub_id);
#else
  nas_read_gprs_hw_classmark(&classmark);
#endif

#ifdef FEATURE_GSM_GPRS  
  if ((ciphering_algorithm == (int) GMMLLC_GEA_1 && classmark.GEA_1_supported) ||
      (ciphering_algorithm == (int) GMMLLC_GEA_2 && classmark.GEA_2_supported) ||
      (ciphering_algorithm == (int) GMMLLC_GEA_3 && classmark.GEA_3_supported) ||
      (ciphering_algorithm == (int) GMMLLC_GEA_4 && classmark.GEA_4_supported) ||
      (ciphering_algorithm == (int) GMMLLC_GEA_5 && classmark.GEA_5_supported) ||
      (ciphering_algorithm == (int) GMMLLC_GEA_6 && classmark.GEA_6_supported) ||
      (ciphering_algorithm == (int) GMMLLC_GEA_7 && classmark.GEA_7_supported) ||
      (ciphering_algorithm == 0))
  {
    return TRUE;
  }
  else 
#endif /*FEATURE_GSM_GPRS*/
  {
    return FALSE;
  }
}

/*===========================================================================

FUNCTION GMM_TRANSLATE_ANC_REQ

DESCRIPTION
  This function translates the Authentication And Ciphering Request message
  from the OTA format into internal representation.

DEPENDENCIES
  None

RETURN VALUE
  This function returns TRUE in case if translation has been completed successfully
  and FALSE otherwise.

SIDE EFFECTS
  None

===========================================================================*/

#define GMM_AUTH_N_CIPH_REQ_MSG_MIN_LENGTH 4 /* Includes all mandatory IEs */

boolean gmm_get_valid_anc_request_message
(
  /* Pointer to the incoming AUTHENTICATION AND CIPHERING REQUEST message */
  mm_cmd_type     *message,

  /* Pointer to valid AUTHENTICATION AND CIPERING REQUEST message parameters */
  gmm_anc_request_message_T *gmm_anc_request_message
)
{

  uint16 total_msg_length ; /* Total message length value */

  byte force_to_standby ;
  uint16 msg_index ;
  boolean rand_present = FALSE ;
  boolean autn_present = FALSE ;
  boolean is_req_algo_supported = FALSE;
  
  /* Read total length of the receivde message */
  GET_IMH_LEN(total_msg_length,&message->cmd.mm_data_ind.message_header);


  if (total_msg_length < GMM_AUTH_N_CIPH_REQ_MSG_MIN_LENGTH)
  {
    /* received message too short */

    gmm_send_gmm_status_message( INVALID_MANDATORY_INFORMATION );

    MSG_HIGH_DS_0(MM_SUB, "=MM= THE A&C REQUEST MESSAGE IS TOO SHORT.");

    return FALSE ;
  }

  total_msg_length = MIN(total_msg_length, gmm_max_L3_msg_length(MM_PS_DOMAIN));

  /* Read required Ciphering algorithm */
  gmm_anc_request_message->ciphering_algorithm =
                  (byte)(message->cmd.mm_data_ind.L3_data[2] & 0x07) ;
                  
  is_req_algo_supported = gmm_check_if_ciphering_algo_supported(gmm_anc_request_message->ciphering_algorithm);
  
  if (!is_req_algo_supported)
  {
    gmm_send_gmm_status_message( INVALID_MANDATORY_INFORMATION );
    MSG_HIGH_DS_1(MM_SUB, "=MM= THE A&C REQUEST has unsupported ciphering algorithm (%d).",
                        gmm_anc_request_message->ciphering_algorithm);
    return FALSE ;  
  }                

  /* Read IMEISV Required indicator */
  gmm_anc_request_message->imeisv_required =
                            (message->cmd.mm_data_ind.L3_data[2] & 0x70) >> 4;

  /* Check validity of the IMEISV Required indicator */
  if (gmm_anc_request_message->imeisv_required > 1)
  {
    /*
    ** If this field is equal to one of the reserved values, interpret this
    ** value as IMEISV is not requested by this version of the protocol
    ** according to TS 24.008 10.5.5.10
    */
    gmm_anc_request_message->imeisv_required = 0 ;
  }

  /* Read Force To Standby indicator */
  force_to_standby = (message->cmd.mm_data_ind.L3_data[3] & 0x07) ;

  /* Check validity of the Force To Standby indicator */
  if (force_to_standby > 1)
  {
    /*
    ** If this field is equal to one of the reserved values, interpret this
    ** value as FORCE TO STANDBY is not requested by this version of the
    ** protocol according to TS 24.008 10.5.5.7
    */
    force_to_standby = 0 ;
  }

  /* Save force_to_standby indicator */
  gmm_anc_request_message->force_to_standby = force_to_standby ;

  /* Save Authentication And Ciphering message reference number */
  gmm_anc_request_message->anc_reference_number =
                       (message->cmd.mm_data_ind.L3_data[3] & 0xF0)>>4 ;


  /* Start to work on an optional part of the message from the fixed offset 4 */
  msg_index = 4 ;

  while (msg_index < total_msg_length)
  {
    /* Get current IEI */
    byte current_iei = message->cmd.mm_data_ind.L3_data[msg_index] ;

    switch (current_iei)
    {
      case GMM_IEI_AUTH_PARAMETER_RAND:
        /* IEI itself is one octet length */
        msg_index = msg_index + 1 ;
        /*
        ** If GMM_IEI_AUTH_PARAMETER_RAND is repeated only the contents
        ** of the first appearing shall be handled
        */
        if (rand_present == FALSE)
        {
          /* Read RAND value, RAND value is always 16 octets length */
          if(msg_index+MAX_AUTH_DATA_LENGTH < total_msg_length)
          {
            gmm_anc_request_message->rand.value_length = MAX_AUTH_DATA_LENGTH ;
            memscpy(gmm_anc_request_message->rand.value_data, MAX_AUTH_DATA_LENGTH,
                            &message->cmd.mm_data_ind.L3_data[msg_index],MAX_AUTH_DATA_LENGTH);
          }
          else
          {
            MSG_ERROR_DS_0(MM_SUB,"=MM= Index out of bounds and buffer overflow");
            /* dont break here, need to send status message */
          }
        }

        /* The RAND value is 16 octets length */
        msg_index += MAX_AUTH_DATA_LENGTH;

        if (total_msg_length > msg_index)
        {
          /* Read GPRS Ciphering Key Sequence number IEI element */
          if (((message->cmd.mm_data_ind.L3_data[msg_index] & 0xF0)>>4) == GMM_IEI_AUTH_GPRS_CKSN)
          {
            if (rand_present == FALSE)
            {
              /* Read GPRS Ciphering Key Sequence number value */
              gmm_anc_request_message->gprs_ck_seqnumber=
                     message->cmd.mm_data_ind.L3_data[msg_index] & 0x0F;
            }
          }
          else
          {
            /* The MS can not complete authentication if a new 'CKSN' parameter
            ** has not been transmitted, The MS shall send Status message to
            ** the network in order to notify it that this parameter missing
            */

            gmm_send_gmm_status_message( CONDITIONAL_IE_ERROR );

            MSG_HIGH_DS_0(MM_SUB, "=MM= THE FIELD 'CKSN' DOES NOT EXIST");

            return FALSE ;
          }

          /* The CKSN IEI is 1 octets length */
          msg_index += 1 ;

          rand_present = TRUE ;
        }
        else
        {
          /* The MS can not complete authentication if a new 'CKSN' parameter
          ** has not been transmitted, The MS shall send Status message to the
          ** network in order to notify it that this parameter missing
          */

          gmm_send_gmm_status_message( CONDITIONAL_IE_ERROR );

          MSG_HIGH_DS_0(MM_SUB, "=MM= THE MESSAGE IS TOO SHORT TO CONTAIN FIELD 'CKSN'" );

          return FALSE ;
        }
        break ;

      case GMM_IEI_AUTH_PARAMETER_AUTN:
        {
          byte autn_length ;

          /* IEI itself is one octet length */
          msg_index = msg_index + 1 ;

          /* Read AUTH length */
          if((msg_index < total_msg_length) &&
             ((msg_index + MIN(message->cmd.mm_data_ind.L3_data[msg_index],MAX_AUTH_DATA_LENGTH))
                        < total_msg_length))
          {
            autn_length = message->cmd.mm_data_ind.L3_data[msg_index] ;
          }
          else
          {
            MSG_ERROR_DS_0(MM_SUB,"=MM= Index out of bounds and buffer overflow");
            msg_index = total_msg_length;
            break;
          }

          /*
          ** If GMM_IEI_AUTH_PARAMETER_AUTN is repeated only the contents
          ** of the first appearing shall be handled
          */
          if (autn_present == FALSE)
          {

            /* Check current AUTH length */
            if (autn_length > MAX_AUTH_DATA_LENGTH)
            {
              /* The AUTH length can not exceed 16 octets */
              MSG_HIGH_DS_1(MM_SUB, "=MM= THE FIELD 'AUTN' HAS UNACCEPTABLE LENGTH (%d)",
                                                          autn_length );
              /****************************************************************************
              *From section 10.5.3.1.1 of 24.008,MAX length of AUTN is 18 bytes.So its
              *value part is max of 16 bytes.From section 11.4.2 of 24.007 network can send
              *more than 16 bytes of AUTN value.If Network sends the AUTN value more than
              *16 bytes,take only 16 bytes form the network given value since AUTN value part
              *can have MAX of 16 bytes only as per 24.008            
              ****************************************************************************/
              gmm_anc_request_message->autn.value_length = MAX_AUTH_DATA_LENGTH ;
            }
            else
            {
              gmm_anc_request_message->autn.value_length = autn_length ;
            }

            /* Read AUTH value */
            memscpy(gmm_anc_request_message->autn.value_data, MAX_AUTH_DATA_LENGTH,
                    &message->cmd.mm_data_ind.L3_data[msg_index+1],
                                  gmm_anc_request_message->autn.value_length) ;

            autn_present = TRUE ;

          }

          /* increase decoded message length */
          msg_index += (autn_length + 1) ;
        }
        break ;
      default:
        /* ----------
        ** Unknown IE
        ** ---------- */
        if ((current_iei & 0xF0) == 0x00)
        {
          /* --------------------------------------
          ** Comprehension required (TS 24.008 8.5)
          ** -------------------------------------- */
          gmm_send_gmm_status_message( INVALID_MANDATORY_INFORMATION );
          return FALSE ;
        }
        else
        {
          /* ------------------------------------------------------------
          ** Ignore unknown IE - increment msg_index per TS 24.007 11.2.4
          ** ------------------------------------------------------------ */
          if ((current_iei & 0x80) == 0x80)
          {
            /* --------------------------------------------------------------
            ** A 1 valued bit 8 indicates that the whole IE is one octet long
            ** -------------------------------------------------------------- */
            msg_index = msg_index + 1;
          }
          else
          {
            /* ---------------------------------------------------------------------
            ** A 0 valued bit 8 indicates that the following octet is a length octet
            ** --------------------------------------------------------------------- */
            if(msg_index+1 < total_msg_length)
            {
              msg_index =
                msg_index + 2 + message->cmd.mm_data_ind.L3_data[msg_index+1];
            }
            else
            {
              MSG_ERROR_DS_0(MM_SUB,"=MM= Index out of bounds and buffer overflow");
              msg_index++;
              break;
            }
          }
        }
        break;
    } /* switch */
  } /* while */

  /* Authentication And Ciphering Request message has been successfully translated */
  return TRUE ;
} /* end gmm_get_valid_anc_request_message() */

/*===========================================================================

FUNCTION GMM_BUILD_ANC_FAILURE

DESCRIPTION
  This function builds AUTHENTICATION AND CIPHERING FAILURE message

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void gmm_build_anc_failure
(
  /* Authentication error code */
  auth_error_type gmm_auth_error,

  /* AUTS value */
  auth_auts_type *gmm_auth_auts,

  /* Generated AUTHENTICATION AND CIPHERING FAILURE message length */
  byte *gmm_anc_failure_message_length,

  /* Generated AUTHENTICATION AND CIPHERING FAILURE message body */
  gmm_authentication_message_buffer_type *message_buffer
)
{
  byte *gmm_anc_failure_buf = message_buffer->message_body ;

  /* Protocol Discriminator and Skip Indicator */
  message_buffer->gmm_msg_head.skip_pd = PD_GMM;

  /* Message type */
  message_buffer->gmm_msg_head.type = GMM_AUTHENTICATION_AND_CIPHERING_FAILURE ;

  /* Message body length */
  *gmm_anc_failure_message_length = 3;

  /* GMM Couse of failure */
  gmm_anc_failure_buf[0] = (byte)gmm_auth_error ;

  if (gmm_auth_auts->value_length > MAX_AUTH_DATA_LENGTH)
  {
    MSG_ERROR_DS_2(MM_SUB, "=MM= Authentication data length %d is greater than %d", 
                  gmm_auth_auts->value_length, MAX_AUTH_DATA_LENGTH) ;   
  }
  if (gmm_auth_auts->value_length > 0)
  {
    gmm_anc_failure_buf[1] = GMM_IEI_AUTH_FAILURE_PARAMETER ;

    gmm_anc_failure_buf[2] = gmm_auth_auts->value_length ;

    memscpy(&gmm_anc_failure_buf[3], sizeof(message_buffer->message_body)-3, gmm_auth_auts->value_data,
                        MIN(gmm_auth_auts->value_length, MAX_AUTH_DATA_LENGTH)) ;

    (*gmm_anc_failure_message_length) += (2 + gmm_auth_auts->value_length) ;
  }
} /* end gmm_build_anc_failure() */

/*===========================================================================

FUNCTION GMM_BUILD_ANC_RESPONSE

DESCRIPTION
  This function builds the Authentication And Ciphering Response message
  according to TS 24.008 9.4.9

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void gmm_build_anc_response
(
  /*pointer to Authentication Response parameter */
  auth_res_type *gmm_auth_res_ptr,

  /* built message length */
  byte *gmm_anc_response_message_length,

  /* built message buffer */
  gmm_authentication_message_buffer_type *message_buffer
)
{
  unsigned char auth_param_response_length ;
  byte index = 1;

  byte *gmm_anc_response_buf_ptr = message_buffer->message_body ;

  /* Protocol Discriminator and Skip Indicator */
  message_buffer->gmm_msg_head.skip_pd = PD_GMM;

  /* Message type */
  message_buffer->gmm_msg_head.type = GMM_AUTHENTICATION_AND_CIPHERING_RESPONSE;

  /* Initialize message buffer pointer */
  // gmm_anc_response_buf_ptr = message_buffer->message_body ;

   /* A&C Request Message Reference Number */
  gmm_anc_response_buf_ptr[0] = gmm_anc_reference_number ;

  /* Current length of the decoded message buffer */
  (*gmm_anc_response_message_length) = 3 ;

  if (gmm_auth_res_ptr != NULL)
  {
    /* Authentication Parameter Response IEI */
    gmm_anc_response_buf_ptr[index] = GMM_IEI_AUTH_PARAM_RESPONSE ;

    /* Response value length */
    auth_param_response_length =
      (gmm_auth_res_ptr->value_length>4) ? 4 : gmm_auth_res_ptr->value_length ;

    /* Set SRES value or 4 most significant octets of UMTS RES */
    memscpy(&gmm_anc_response_buf_ptr[2], sizeof(message_buffer->message_body)-2,
                     gmm_auth_res_ptr->value_data,auth_param_response_length) ;

    /* Current length of the decoded message buffer */
    (*gmm_anc_response_message_length) += 5 ;

    index += 5 ;
  }

    /* If the IMEISV value required by the network,
    ** convert it into OTA format and put into message
    */
  if (gmm_imeisv_required == 1)
  {
      byte imeisv[MAX_IMEI_LEN+2] ;
#if defined(FEATURE_DUAL_SIM) && defined(FEATURE_DUAL_IMEI)
      ghdi_ds_read_imeisv(imeisv,(sys_modem_as_id_e_type)mm_sub_id);
#else
      read_imeisv(imeisv) ;
#endif

    /* Initialize IMEISV type */
    gmm_anc_response_buf_ptr[index] = GMM_IEI_MS_IDENTITY ;

    /* Initialize IMEISV content length */
    gmm_anc_response_buf_ptr[index+1] = imeisv[0] ;

    /* According to 24.008 10.5.1.4, IMEISV field is coded using BCD  coding.
    ** If the number of identity digits is  even then  bits  5 to 8 of the
    ** last octet shall  be  filled with an end mark coded as "1111"
    */
    memscpy((void *)&gmm_anc_response_buf_ptr[index+2],sizeof(message_buffer->message_body)-(index+2),&imeisv[1],9) ;

    (*gmm_anc_response_message_length) += 11 ;

    index += 11 ;
  }

  if (gmm_auth_res_ptr != NULL)
  {
    if (gmm_auth_res_ptr->value_length > MAX_AUTH_DATA_LENGTH)
    {
      MSG_ERROR_DS_2(MM_SUB, "=MM= Authentication data length %d is greater than %d", 
                  gmm_auth_res_ptr->value_length, MAX_AUTH_DATA_LENGTH) ;   
    }
    /* Set Authentication response extension value */
    if (gmm_auth_res_ptr->value_length > 4)
    {
      gmm_anc_response_buf_ptr[(*gmm_anc_response_message_length) - 2] =
                                              GMM_IEI_AUTH_PARAM_RESPONSE_EXT ;

      gmm_anc_response_buf_ptr[(*gmm_anc_response_message_length) - 1] =
                                           gmm_auth_res_ptr->value_length - 4 ;

      memscpy(&gmm_anc_response_buf_ptr[(*gmm_anc_response_message_length)], sizeof(message_buffer->message_body)-(*gmm_anc_response_message_length),
                        (unsigned char *)(&gmm_auth_res_ptr->value_data[4]),
                        MIN(gmm_auth_res_ptr->value_length , MAX_AUTH_DATA_LENGTH)-4) ;

      (*gmm_anc_response_message_length) +=
                                         (gmm_auth_res_ptr->value_length - 2) ;
    }
  }
} /* end gmm_build_anc_response */

/*==========================================================================
 CSN_MASK is used to set a bit at the given position easily. The mask can
 aslo be generated dynamically if space at a premium.
 ==========================================================================*/

static const byte CSN_MASK[8] = { 0x80, 0x40, 0x20, 0x10, 0x08, 0x04, 0x02, 0x01 };

/*===========================================================================

FUNCTION CSN_ENCODER_INIT

DESCRIPTION
  This function initializes the CSN structure that holds information about the
  encoder buffer.

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
void csn_encoder_init(csn_encoder_T *csn, byte *buf, uint16 size)
{
   csn->buf = buf;
   csn->buf_size = size;
   csn->byte_pos = csn->bit_pos = 0;
}

#ifdef FEATURE_NON_DEMAND_PAGED_FUNCTION
#error code not present
#endif

/*===========================================================================

FUNCTION CSN_BIT_POS

DESCRIPTION
  This function returns the absolute index of bit position in the buffer.
  The retrieved bit pos can be later used to encode bits at that position
  by constructing a new CSN structure.

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
uint16 csn_bit_pos(csn_encoder_T *csn)
{
   return ( csn->byte_pos*8 + csn->bit_pos );
}

#ifdef FEATURE_NON_DEMAND_PAGED_FUNCTION
#error code not present
#endif



/*===========================================================================

FUNCTION CSN_ENCODE_BIT

DESCRIPTION
  This function encodes one bit at the next logical position if val is a
  non-zero value

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
void csn_encode_bit(csn_encoder_T *csn, byte val)
{
   if( csn->bit_pos > 7)
   {
      MSG_ERROR_DS_1(MM_SUB, "=MM= Invalid csn bit pos %d", csn->bit_pos);
      return;
   }

   if( csn->byte_pos >= csn->buf_size)
   {
      MSG_ERROR_DS_0(MM_SUB, "=MM= csn buffer is not big enough!");
      return;
   }

   if(val)
   {
      /* Set bit to 1 at bit_pos */
      csn->buf[csn->byte_pos] |= CSN_MASK[csn->bit_pos];
   }
   else
   {
      /* Set bit to 0 at bit_pos */
      csn->buf[csn->byte_pos] &=  ~(CSN_MASK[csn->bit_pos]);
   }


   csn->bit_pos++;

   if(csn->bit_pos > 7)
   {
      /* move to next byte */
      csn->bit_pos = 0;
      csn->byte_pos++;
   }
}

#ifdef FEATURE_NON_DEMAND_PAGED_FUNCTION
#error code not present
#endif


/*===========================================================================

FUNCTION:  CSN_DECODE_BIT

DESCRIPTION
  This function decodes one bit at the next logical position

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
byte csn_decode_bit( csn_encoder_T *csn )
{
   byte val;

   if((csn->bit_pos <= 7) && (csn->byte_pos < csn->buf_size))
   {
       /* byte value of specified bit position */
       val = csn->buf[csn->byte_pos] & CSN_MASK[csn->bit_pos];

       /* advance bit_pos, byte_pos */
       csn->bit_pos = ((csn->bit_pos == 7) ? 0 : csn->bit_pos+1);

       csn->byte_pos = ((csn->bit_pos == 0) ? csn->byte_pos+1 : csn->byte_pos);
   }
   else
   {
       MSG_ERROR_DS_2(MM_SUB, "=MM= Error: csn->bit_pos=%d, csn->byte_pos=%d", csn->bit_pos, csn->byte_pos);

       val = 0;
   }

   return (val ? 1 : 0);
}


/*===========================================================================

FUNCTION:   csn_encode_bits()

DESCRIPTION
  This function encodes bits a the next logical position
  Can not fill more than 8 bits at a time

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

void csn_encode_bits(csn_encoder_T *csn, byte num_bits, byte val)
{
   uint32 index;
   if(num_bits > 8 || num_bits < 1)
   {
      MSG_ERROR_DS_1(MM_SUB, "=MM= Invalid num of bits to be set %d", num_bits);
      return;
   }

   /* Transform data to left side of the byte ti use CSN MASK macro */
   val= (byte)(val<< (8 - num_bits));

   for(index = 0; index < num_bits; index++)
   {
      csn_encode_bit(csn, (byte)(val & CSN_MASK[index]));
   }
}

#ifdef FEATURE_NON_DEMAND_PAGED_FUNCTION
#error code not present
#endif

/*===========================================================================

FUNCTION: csn_decode_bits()

DESCRIPTION
  This function decodes bits a the next logical position
  Can not decode more than 8 bits at a time

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
byte csn_decode_bits(csn_encoder_T *csn, byte num_bits )
{
   byte val;

   if((num_bits > 0) && (num_bits <= 8))
   {
       for (val=0; (num_bits > 0) ; num_bits--)
       {
           val <<=1;
           val |= csn_decode_bit( csn );
       }
   }
   else
   {
       val = 0;
   }

   return( val );
}



/*===========================================================================

FUNCTION CSN_FILL_SPARE_BITS

DESCRIPTION
  This function fills the last byte with the spare bits of 0's if not filled
  already

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void csn_fill_spare_bits(csn_encoder_T *csn)
{
   if( csn->bit_pos > 0)
   {
      csn_encode_bits(csn, (byte)(8-csn->bit_pos), 0);
   }
}


/*===========================================================================

FUNCTION CSN_ENCODE_BITS_AT_POS

DESCRIPTION
  This function encodes bits in a buffer starting at the given bit position
  Can not encode more than 8 bits at a time

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

void csn_encode_bits_at_pos(csn_encoder_T *csn, byte num_bits, byte val, uint16 pos)
{
   csn_encoder_T csn_temp;
   csn_temp.buf = csn->buf;
   csn_temp.buf_size = csn->buf_size;
   csn_temp.byte_pos = (byte)pos / 8;
   csn_temp.bit_pos  = pos % 8;

   csn_encode_bits(&csn_temp, num_bits, val);
}

#ifdef FEATURE_NON_DEMAND_PAGED_FUNCTION
#error code not present
#endif
/*===========================================================================

FUNCTION GMM_DECODE_NW_FTR_SUPPORT

DESCRIPTION
  This function decodes Network Feature Support IE.

DEPENDENCIES
  None

RETURN VALUE
  
SIDE EFFECTS
  None

===========================================================================*/
void gmm_decode_nw_ftr_support
(
   byte  *ie_ptr,
   byte  *nw_ftr_support   
)
{
   *nw_ftr_support = ((*ie_ptr) & 0x0F);                 
   if (((*ie_ptr) & GMM_PS_LCS_SUPPORTED) == GMM_PS_LCS_SUPPORTED)
   {
         mm_lcs_domain = MM_PS;
      }
}
/*===========================================================================

FUNCTION GMM_IS_PERIODIC_RAU_ACCEPT_MSG_VALID

DESCRIPTION
  This function will check whether the ptmsi received in periodic rau accept 
  message is valid or not

DEPENDENCIES
  None

RETURN VALUE
  
SIDE EFFECTS
  None

===========================================================================*/
boolean gmm_is_periodic_rau_accept_msg_valid( mm_cmd_type  *message)
{
   boolean valid_msg = TRUE;
   byte    msg_index = 10; //includes all mandatory elements of RAU accept message

   if( message->cmd.mm_data_ind.L3_data[msg_index] == GMM_IEI_PTMSI_SIGNATURE)
      msg_index = msg_index + sizeof( gmm_ie_TV_ptmsi_signature_T );

   if (message->cmd.mm_data_ind.L3_data[msg_index] == GMM_IEI_PTMSI)
   {
     if( (message->cmd.mm_data_ind.L3_data[msg_index+1] != (PTMSI_SIZE+1)) ||
         (message->cmd.mm_data_ind.L3_data[msg_index+2] != 0xF4)
       )
     {
        gmm_send_gmm_status_message( SEMANTICALLY_INCORRECT_MESSAGE );
        valid_msg = FALSE;
     }
   }

   return valid_msg;
}
