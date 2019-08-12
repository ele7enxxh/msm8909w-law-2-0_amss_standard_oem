#ifndef cc_mm_h
#define cc_mm_h
/*===========================================================================
                     COPYRIGHT INFORMATION

Copyright (c) 2001, 2002 Qualcomm Technologies, Incorporated and its licensors.  All Rights 
Reserved.  QUALCOMM Proprietary.  Export of this technology or software 
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.

                     EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/MSM5200/cnm/vcs/ms_cc_mm.h_v   1.5   22 Apr 2002 09:45:14   cdealy  $   
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/api/ms_cc_mm.h#1 $    $DateTime: 2016/12/13 08:00:05 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
08/14/01   AB      Renamed connection manager references to CNM from CM.

04/19/02   CD      Replaced cause by rel_type in MMCNM_REL_REQ
                   Deleted CNM_service_type from MMCNM_EST_CNF, MMCNM_REL_IND
                   Deleted reestablishment_success flag from mmcc_reest_cnf
                   Deleted MMCNM_ABORT_IND, MMCNM_ERR_IND, MMCNM_EST_IND

                   Added MMCNM_EST_REJ, MMCNM_REL_CNF, MMCNM_REEST_REJ

                   Updated Copyright

09/19/02   CD      Added 'rej_cause' element into MM established reject 
                   message structure, mmcnm_est_rej_T, to include the lower
                   layer specific cause value, which can be passsed to 
                   application layer.
                    
                   Added 'reason' element in mmcnm_sync_ind_T, to determine 
                   handoff direction between WCDM and GSM.

05/11/05   HS      Added support for FEATURE_CCBS

06/15/06   NR      Adding cause type and value fields in mmcc_reest_rej_tag

07/14/06   NR      Adding changes for SMS optimization CR 93099 

03/05/06   NR      Adding changes for WB_AMR

04/09/07   RD      SMS optmization changes are incorporated 

05/21/07   NR      Adding new field to mmcnm_est_req for passing call_type to MM
===========================================================================*/

#include "sys_type.h"
#include "sys_cnst.h"



/*-------------------------------------------------------------------
                     ENUM: call_type_for_rrc_est_req_T
This enumerates call types which has to be passed in for RRC connection establishment request by MM.
--------------------------------------------------------------------*/
typedef enum
{
  SPEECH_CALL,
  VIDEO_CALL,
  OTHER_CALL,
  NONE_CALL
} call_type_for_rrc_est_req_T;

/**********************************************************************
 **********************************************************************
 *******                                                        *******
 *******           MESSAGES FROM CC TO MM                       *******
 *******                                                        *******
 **********************************************************************
 ********************************************************************** 
 */


/**********************************************************************
 * MMCNM_DATA_REQ
 *
 * This is identical to the MsgHeader defined in cc_cc.h
 */

typedef struct mmcnm_data_req_tag
{ 
   IMH_T             header;

   byte              data[MAX_OCTETS_L3_MESSAGE+1];

} mmcnm_data_req_T;


/************************************************************************
 * MMCNM_EST_REQ
 * Added call type in establishment request in order to pass the type of call 
 * in RRC_EST_REQ by MM
*/
typedef struct mmcnm_est_req_tag
{
   IMH_T              header;
   CNMServiceType_T   CNM_service_type;    
   call_type_for_rrc_est_req_T     mm_rrc_call_type;

   sys_modem_as_id_e_type                   as_id;

} mmcnm_est_req_T;



/************************************************************************
 * MMCC_REEST_REQ
 *
 */

typedef struct mmcc_reest_req_tag
{
   IMH_T          header;
   CNMServiceType_T   CNM_service_type;
   sys_modem_as_id_e_type  as_id;

} mmcc_reest_req_T;


/************************************************************************
 * MMCNM_REL_REQ
 *
 */

typedef struct mmcnm_rel_req_tag
{
   IMH_T                header;
   byte                 rel_type;

   sys_modem_as_id_e_type        as_id;

} mmcnm_rel_req_T;


/************************************************************************
 * MMCNM_PROMPT_RES
 *
 */

typedef struct mmcnm_prompt_res_tag {
   IMH_T          header;
   boolean        supported; /* 1: NIMO supported, 0: not supported */
   boolean        accept;  /* 1: accept, 0: reject */

   sys_modem_as_id_e_type  as_id;

} mmcnm_prompt_res_T;


/************************************************************************
 ************************************************************************
 *******                                                          *******
 *******            MESSAGES FROM MM TO CC                        *******
 *******                                                          *******
 ************************************************************************
 ************************************************************************
 */

/************************************************************************
 * MMCNM_DATA_IND
 * This is identical to the MsgHeader defined in cc_cc.h
 */
typedef struct mmcnm_data_ind_tag
{
   msg_head_T     msg_head;

   byte           data[MAX_OCTETS_L3_MESSAGE+1];              /* max size of message */

}
mmcnm_data_ind_T;

/************************************************************************
 *
 * MMCNM_EST_REJ
 */

typedef struct mmcnm_est_rej_tag
{
   IMH_T            header;
   reject_type_enum_T    cause_type;
   reject_cause_T        cause_value;

   mmcc_domain_name_T domain;



   sys_modem_as_id_e_type      as_id;

} mmcnm_est_rej_T;

/************************************************************************
 *
 * MMCNM_EST_CNF
 */

typedef struct mmcnm_est_cnf_tag
{
   IMH_T            header;

   sys_modem_as_id_e_type    as_id;

} mmcnm_est_cnf_T;


/************************************************************************
 *
 * MMCNM_REL_IND
 */

typedef struct mmcnm_rel_ind_tag
{
   IMH_T                   header;
   boolean                 reest_possible;
   lower_layer_cause_T     rel_cause;

   sys_modem_as_id_e_type           as_id;

} mmcnm_rel_ind_T;

/************************************************************************
 *
 * MMCNM_REL_CNF
 */

typedef struct mmcnm_rel_cnf_tag
{
   IMH_T                   header;
   boolean                 mm_conn_active_nw_data_recd;

   sys_modem_as_id_e_type           as_id;

} mmcnm_rel_cnf_T;

/***********************************************************************
 *
 * MMCC_REEST_CNF
 */

typedef struct mmcc_reest_cnf_tag
{
   IMH_T                      header;

   sys_modem_as_id_e_type              as_id;

} mmcc_reest_cnf_T;

/***********************************************************************
 *
 * MMCC_REEST_REJ
 */

typedef struct mmcc_reest_rej_tag
{
   IMH_T                 header;
   reject_type_enum_T    cause_type;
   reject_cause_T        cause_value;

   sys_modem_as_id_e_type         as_id;

} mmcc_reest_rej_T;


/***********************************************************************
 *
 * MMCC_SYNC_IND
 */

typedef struct mmcc_sync_ind_tag
{
   IMH_T                   header;
   TchRate_T               tch_rate;
   Subchannel_T            subchannel;
   channel_mode_T          channel_mode;
   uint8                   radio_access_bearer_id;
   byte                    reason;  /* int */

  umts_codecs_list_T umts_codec;


  umts_wcdma_cs_voice_bearer_enum_T cs_voice_type;


  sys_modem_as_id_e_type      as_id;

} mmcc_sync_ind_T;


/***********************************************************************
 *
 * MMCNM_SRVCC_HANDOVER_COMPLETE_IND
 */
typedef struct mmcnm_srvcc_handover_complete_ind_tag
{
   IMH_T                          header;
   sys_radio_access_tech_e_type   active_rat; 

/* MM sends Trigger=1 when MM receives RRC_ACTIVATION_IND and Trigger=0 when MM receives LTE_DEACTIVATE_CNF 
 * Based on the Trigger,MN will do vocoder acquisition even before LTE_DEACT_CNF  
 * or MN will do transient procedures after LTE_DEACT_CNF */
   boolean                        first_trigger;  
   sys_modem_as_id_e_type         as_id;
} mmcnm_srvcc_handover_complete_ind_T;

void      CNM_sms_cp_data_l2_ack(boolean transmit_ok);
void      CNM_sms_cp_data_l2_ack_subs_2(boolean transmit_ok);

#endif


