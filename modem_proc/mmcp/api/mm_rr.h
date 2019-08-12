#ifndef mm_rr_h
#define mm_rr_h
/*===========================================================================
            COPYRIGHT INFORMATION

Copyright (c) 2002, 2003-2011 Qualcomm Technologies, Incorporated and its licensors.  All Rights
Reserved.  QUALCOMM Proprietary.  Export of this technology or software
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.

            EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/MSM5200/mm/vcs/mm_rr.h_v   1.11   18 Jul 2002 11:41:12   vdrapkin  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/api/mm_rr.h#1 $ $DateTime: 2016/12/13 08:00:05 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
09/02/11   rajesh Added "is_periodic_lau" field in structure "rr_est_req_T".
10/01/11   abhi   Added feild to send registered/mapped RAI to GRR in Activation Response
08/09/11   abhi    Added new cause for deact request 
07/05/11  mrk      Support of Ciphering enabled Indication for CS Voice calls
04/04/01   jca     Fixed LINT errors.
03/03/02   rmc     Modified PLMN_select_req for dual mode cell selection changes
03/07/02   SB      Added Mode Activation/Deactivation messages.
04/29/02   rmc     Changes for dual mode.
06/12/02   jca     Further changes for dual mode.
06/17/02   ks      commented the l3_message_length in the primitives.
07/07/02   vdr     Multiple changes in order to support GMM-GRR interface
07/12/02   ks      Added paged_with_TLLI in rr_page_ind_T primitive
07/18/02   vdr     Renamed FEATURE_GPRS to be FEATURE_GSM_GPRS compilation flag
07/22/02   Rao     Added activation_ind for InterRAT Handover support
                   also added mm_as_status to rr_plmn_list_cnf, rr_abort_cnf.
08/02/02   ATM     Remapped msgIDs for logging tool stability
09/24/02   mks     Added rr_act_req to initiate powerscan at startup.
10/08/02   ks      Added service_domain of type sys_srv_domain_e_type in
                   rr_plmn_select_req_T primitive for GPRS/GSM coordination.
10/28/02   Rao     Added service_domain of type sys_srv_domain_e_type
                   in rr_activation_rsp_T.
11/05/02   Rao     Added new sync_reason MM_RR_WCDMA_GSM_HO to sync_reason_T
11/18/02   ks      RR_GMM_GPRS_MS_RA_CAP_IE_IND,RR_GMM_START_MM_NON_DRX_IND
                   and RR_GMM_STOP_MM_NON_DRX_IND primitives are added for GPRS
12/04/02   mks     Added RR_GMM_GPRS_SUSPENSION primitive to support GPRS
                   suspension procedure. Included the gprs_resumption_ind flag
                   in rr_rel_ind_T, rr_abort_ind_T and rr_abort_cnf_T to notify
                   the GPRS resumption status.
12/06/02   mks     Pooled RR_GMM_GPRS_SUSPENSION primitive with the rest of the
                   MM-RR primitives.
01/30/03   Rao     Added PS Attached Flag to rr_activation_rsp.
02/14/03   Rao     Added rr_mm_gsm_stop_reason_T definition and also the same
                   in rr_stop_gsm_mode_req_T.
02/21/03   ks      Removed the FEATURE_GSM_DEEPSLEEP in rr_stop_gsm_mode_req_T
03/18/03   ks      Added new primitive rr_mm_gsm_deact_reason_T to specify the
                   Deactivation reason to RR in rr_deact_req_T primitive.
03/20/03   ks      Swapped the RR_MM_POWER_OFF and RR_MM_OFF_LINE enum values.
04/22/03   ks      Added RAC and LAC in the rr_activation_rsp_T primitive.
07/16/03   jca     Added RR_GMM_GPRS_RESUMPTION_IND primitive.  Removed
                   gprs_resumption_ind field from RR_ABORT_CNF, RR_ABORT_IND,
                   and RR_REL_IND.
08/28/03   ks      Added field nv_anite_gcf_flag in rr_gmm_gprs_nv_params_ind_t;
08/29/03   cd      Added band preference to rr_plmn_select_req/cnf, rr_service_ind
                   rr_plmn_list_req and rr_act_req
10/17/03   jca     Replaced select_specific_PLMN and automatic fields in
                   RR_PLMN_SELECT_REQ with network_select_mode field.
04/01/04   ks      Replaced the mm_rr_list_search_mode_T with
                   sys_network_selection_mode_e_type in rr_plmn_list_req_T.
04/06/04   cd      LCS support
05/12/04   cd      Fixed Lint error
05/19/04   cd      band_pref in rr_plmn_select_req, rr_plmn_list_req and rr_act_req,
                   is included if either GSM/WCDMA multiband feature is present
06/08/04   cd      LCS support.  Removed rr_lcs_active_ind
08/25/04   npr     Added RR_RESELECTION_IND.
11/04/04   sv      Additions for EDGE: modified the GPRS NV Params Ind structure.
11/22/04   ks      Added gsm_only and plmn_list in rr_plmn_list_req_T to support
                   3G background PLMN search when UE is in 2G.
02/14/05   sv      Added support for Rel-4 changes.
03/31/05   sm      Added auth_failed_bar_cells in RR_ABORT_REQ
09/15/06   sn      Added RR_NW_SEL_MODE_RESET_IND
06/21/05   up      Added RR_EQ_PLMN_LIST_CHANGE_IND and message structure
                   rr_eq_plmn_list_change_ind_T.
08/14/05   hj      Added handling for monitoring CCCH paging channel when detached from
                   GPRS Services.
10/08/05   abe     Added code for FEATURE_INTERRAT_PCCO_GTOW
10/24/05   up      Updated rr_plmn_list_cnf_T to add net sel mode.
11/30/05   jca     Added DTM NV parameters to rr_gmm_gprs_nv_params_ind_t.
12/15/05   abe     Supported GSM DTM under FEATURE_GSM_DTM
01/18/06   up      Added New primitive RR_BPLMN_SEARCH_ABORT_REQ and message.
02/02/06   up      Adding interrat_nacc_support NV item for RR.
07/14/06   nr      Adding changes for SMS optimization CR 93099 
07/27/06   sn      Adding Uniform OOS code changes.
                   FEATURE_UMTS_UNIFORM_OOS_HANDLING  
08/23/06   up      Added mm_rr_plmn_lai_forbidden.
09/21/06   pk      Updated for Feature EDTM
11/23/06   sn      FEATURE_UMTS_BACKGROUND_FULL_SERVICE_SEARCH
03/18/09   abhi   Adding element nv_disable_call_type in rr_gmm_gprs_nv_params_ind_t 
                          to pass disable call type value to GRR.
05/07/09   abhi   Changed type of nv_disable_call_type in rr_gmm_gprs_nv_params_ind_t 
                  to uint16. 
===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "sys_stru.h"
#include "sys_type.h"
#include "sys_cnst.h"
#include "sys.h"
#include "sys_v.h"
#include "sys_plmn.h"
#include "sys_plmn_selection.h"
#include "mm_as_types.h"

#define NV_DISABLE_VOICE_SUPPORT_MASK  1
#define NV_DISABLE_CSDATA_SUPPORT_MASK  2
/*===========================================================================

                        DATA DECLARATIONS

===========================================================================*/

/*****************************************************/
/* The following enumeration defines the message ids */
/* of the messages in transit between MM and RR      */
/* IMPORTANT:Please do not renumber this list!!!     */
/* It is used by logging. If you must change this    */
/* enum, send notification mailto:cfa.team           */
/*****************************************************/
typedef enum
{
  /* MM->RR primitives */
   RR_ABORT_REQ                    = 0x00
  ,RR_DATA_REQ                     = 0x01
  ,RR_DEACT_REQ                    = 0x02
  ,RR_EST_REQ                      = 0x03
  ,RR_PLMN_LIST_REQ                = 0x04
  ,RR_PLMN_SELECT_REQ              = 0x05
  ,RR_FORBIDDEN_LAI_UPDATE_REQ     = 0x06
  ,RR_SIM_INSERTED_REQ             = 0x07
  ,RR_INVALIDATE_SIM_DATA_REQ      = 0x08
  ,RR_STOP_GSM_MODE_REQ            = 0x09
  ,RR_SIM_UPDATE_REQ               = 0x0a
  ,RR_ACT_REQ                      = 0x0b
  ,RR_EQ_PLMN_LIST_CHANGE_IND      = 0x0c
  ,RR_BPLMN_SEARCH_ABORT_REQ       = 0x0d
  ,RR_CHANGE_MODE_IND              = 0x0e
  ,RR_LTE_RESELECTION_ALLOWED_STATUS_IND  = 0x0f
  ,RR_LAI_REJECT_LIST_IND          = 0x10
  ,RR_CSFB_BLIND_REDIR_TO_LTE_REQ  = 0x11
  ,RR_CSFB_CALL_STATUS_IND = 0x12
  ,RR_CSG_WHITE_LIST_UPDATE_IND    = 0x13
  ,RR_MT_CSFB_FAILURE_IND          = 0x14
        

  ,RR_GMM_GPRS_STATE_CHANGE_REQ    = 0x20
  ,RR_GMM_GPRS_TLLI_ASSIGNMENT_REQ = 0x21
  ,RR_GMM_READY_TIMER_STATE_REQ    = 0x22
  ,RR_GMM_GPRS_MS_RA_CAP_IE_IND    = 0x23
  ,RR_GMM_START_MM_NON_DRX_IND     = 0x24
  ,RR_GMM_STOP_MM_NON_DRX_IND      = 0x25
  ,RR_GMM_GPRS_SERVICE_CNF         = 0x26
  ,RR_GMM_GPRS_NV_PARAMS_IND       = 0x27
  ,RR_GMM_GPRS_CCCH_CAMP_IND       = 0x28
  ,RR_GMM_GPRS_PCCCH_CAMP_REQ      = 0x29
  ,RR_GMM_GPRS_AUTH_FAIL_IND       = 0x2A



  ,RR_PSHO_CNF                     = 0x2B




  ,RR_DS_STATUS_CHANGE_IND         = 0x2C
  ,RR_PS_ABORT_REQ                 = 0x2D
  ,RR_GMM_GPRS_SERVICE_REQ         = 0x2E
  ,RR_UE_MODE_IND                  = 0x2F
  ,RR_SERVICE_STATUS_IND           = 0x30
  ,RR_MULTIMODE_SUBS_CHGD_REQ      = 0x31
  ,RR_DATA_PRIORITY_IND            = 0x32
  ,RR_PSEUDO_LTE_LIST_UPDATE_IND   = 0x33
  ,RR_PS_RESUME_IND                = 0x34
                        
  /* MM<-RR primitives */
  ,RR_ABORT_CNF                    = 0x80
  ,RR_ABORT_IND                    = 0x81
  ,RR_DATA_IND                     = 0x82
  ,RR_DEACT_CNF                    = 0x83
  ,RR_EST_CNF                      = 0x84
  ,RR_PAGING_IND                   = 0x85
  ,RR_PLMN_LIST_CNF                = 0x86
  ,RR_PLMN_SELECT_CNF              = 0x87
  ,RR_REL_IND                      = 0x88
  ,RR_SERVICE_IND                  = 0x89
  ,RR_SYNC_IND                     = 0x8a
  ,RR_STOP_GSM_MODE_CNF            = 0x8b
  ,RR_ACTIVATION_IND               = 0x8c
  ,RR_ACTIVATION_RSP               = 0x8d

  ,RR_GMM_GPRS_SUSPENSION          = 0x8e
  ,RR_GMM_GPRS_RESUMPTION_IND      = 0x90
  ,RR_RESELECTION_IND              = 0x91
  ,RR_GPRS_PCCCH_CAMP_CNF          = 0x92


  ,RR_GTOW_CCO_COMPLETE_IND        = 0x93



  ,RR_SERVICE_IND_FROM_OTA_MSG     = 0x94



  ,RR_EST_IND                      = 0x95

  ,RR_NW_SEL_MODE_RESET_IND        = 0x96


  ,RR_PSHO_IND                     = 0x97


  ,RR_PS_ABORT_CNF                 = 0x98
  ,RR_DS_DATA_IND                  = 0x99

  ,RR_GMM_GPRS_MS_RA_CAP_REQ       = 0x9a
  ,RR_GMM_GPRS_MULTISLOT_CLASS_IND           =0x9b
  ,RR_SRVCC_HANDOVER_FAILURE_IND           =0x9c
  ,RR_MULTIMODE_SUBS_CHGD_CNF             = 0x9d
  ,RR_PS_STATUS_CHANGE_IND         = 0x9e
} mm_rr_message_id_enum_T;

#define MAX_FORBIDDEN_LAIS    20

typedef enum
{
  MM_RR_CIPHERING = 0,
  MM_RR_RES_ASS,
  MM_RR_CHANNEL_MODE_MODIFIED,
  MM_RR_HANDOVER,
  MM_RR_WCDMA_GSM_HO
} sync_reason_T;

typedef enum
{
  MM_RR_MO_FULL_SPEECH,
  MM_RR_MO_HALF_SPEECH,
  MM_RR_MO_DATA_9600,
  MM_RR_MO_DATA_4800,
  MM_RR_MO_DATA_2400,
  MM_RR_SHORT_MESSAGE,
  MM_RR_SUPPLEMENTARY_SERVICE,
  MM_RR_LOCATION_UPDATE,
  MM_RR_IMSI_DETACH,
  MM_RR_CALL_REEST,
  MM_RR_EMERGENCY_CALL,
  MM_RR_ANSWER_TO_PAGING,
  MM_RR_NO_CNM_SERVICE = 0xff
} establishment_cause_T;

/*
** GMM<->LLC interface module state machine definition
*/
typedef enum gmm_llc_if_state_tag
{
  GMM_IDLE_STATE,
  GMM_READY_STATE,
  GMM_STANDBY_STATE
} gmm_llc_if_state_type ;


/*
**  MM <-> RR stop gsm mode reasons
*/

typedef enum
{
  RR_MM_RAT_CHANGE,
  RR_MM_DEEP_SLEEP,
  RR_GTOW_CC_ORDER,
  RR_MM_USER_RAT_CHANGE,
  RR_MM_DUAL_SWITCH,
  RR_LTOG_CCO_FAILURE,
  RR_SGLTE_RAT_CHANGE,
  RR_UE_SGLTE_MODE_SWITCH,
  RR_SGLTE_USER_RAT_CHANGE
} rr_mm_gsm_stop_reason_T;


typedef enum
{
  RR_MM_MODE_CHANGE,
  RR_MM_POWER_OFF,
  RR_MM_OFF_LINE,
  RR_MM_LPM_POWER_OFF
} rr_mm_gsm_deact_reason_T;



typedef enum
{
  MM_RR_PLMN_SEARCH_IMMEDIATE,     /* Paging channel will not be monitored during search   */
  MM_RR_PLMN_SEARCH_BACKGROUND     /* Paging channel will still be monitored during search */
} mm_rr_list_search_mode_T;



typedef enum
{
  RR_MM_MO_CS_CALL,
  RR_MM_MO_PS_CALL,
  RR_MM_NORMAL_ACQ,
  RR_MM_ACQ_DB_SCAN
} rr_mm_service_req_cause_e_type;

typedef enum
{
  MM_RR_OTHER_FAILURE,
  MM_RR_NO_RESOURCE_AVAILABLE,
  MM_RR_SGLTE_SRVCC_CS_HANDOVER
} mm_rr_failure_cause_e_type;



/**************************/
/* Messages from MM to RR */
/**************************/

/*************************************************************/
/* RR_BPLMN_SEARCH_ABORT_REQ primitive                       */
/* FROM : MM                                                 */
/* TO   : RR                                                 */
/* DESCRIPTION :                                             */
/* Request to abort the ongoing manual search.               */
/*************************************************************/

typedef struct
{
   IMH_T                   message_header;

   sys_modem_as_id_e_type  as_id;

} rr_bplmn_search_abort_req_T;

/*************************************************************/
/* RR_EQ_PLMN_LIST_CHANGE_IND primitive                                  */
/* FROM : MM                                                 */
/* TO   : RR                                                 */
/* DESCRIPTION :                                             */
/* Indiactes RR when equivalent plmn list is changed         */
/*************************************************************/

typedef struct
{
   IMH_T                   message_header;

   sys_modem_as_id_e_type  as_id;

} rr_eq_plmn_list_change_ind_T;

/*************************************************************/
/* RR_ACT_REQUEST primitive                                  */
/* FROM : MM                                                 */
/* TO   : RR                                                 */
/* DESCRIPTION :                                             */
/* Request RR to start a power scan, before upper layers are */
/* are started. Should only be used on initial power up      */
/*************************************************************/

typedef struct
{
   IMH_T                   message_header;

   sys_band_mask_type      band_pref;


   sys_modem_as_id_e_type  as_id;

} rr_act_req_T;

/*************************************************************/
/* RR_ABORT_REQUEST primitive                                */
/* FROM : MM                                                 */
/* TO   : RR                                                 */
/* DESCRIPTION :                                             */
/* Request to abort the RR connection                        */
/*************************************************************/

typedef struct
{
   IMH_T                   message_header;
   boolean                 auth_failed_bar_cells;
   boolean                 csmt_fail_depriotize_cells;
   sys_modem_as_id_e_type  as_id;

} rr_abort_req_T;

/*************************************************************/
/* RR_MT_CSFB_FAILURE_IND primitive                                */
/* FROM : MM                                                 */
/* TO   : RR                                                 */
/* DESCRIPTION :                                             */
/* Request to abort the RR connection                        */
/*************************************************************/

typedef struct
{
   IMH_T                   message_header;
   boolean                 csmt_fail_depriotize_cells;
   sys_modem_as_id_e_type  as_id;
} rr_mt_csfb_failure_ind_T;

/*************************************************************/
/* RR_GMM_GPRS_FAIL_IND primitive                            */
/* FROM : MM                                                 */
/* TO   : RR                                                 */
/* DESCRIPTION :                                             */
/* Request to block cell for GPRS as GMM Authentication      */
/* failed in this cell                                       */
/*************************************************************/

typedef struct
{
   IMH_T                   message_header;
   dword                   t3302_timer_value; /*Timer value in millisecs*/
   ARFCN_T                 arfcn;
   uint8                   bsic;
   sys_modem_as_id_e_type  as_id;

} rr_gmm_gprs_auth_fail_ind_T;

/*************************************************************/
/* RR_DATA_REQUEST primitive                                 */
/* FROM : MM                                                 */
/* TO   : RR                                                 */
/* DESCRIPTION :                                             */
/* Is used by MM to request the sending of data to its peer  */
/* entity on the network side                                */
/*************************************************************/

typedef void (*rr_data_req_cb_fn_T)(boolean transmit_ok); 


typedef struct
{
   IMH_T                   message_header;
   sys_modem_as_id_e_type    as_id;
   rr_data_req_cb_fn_T   callback_fn;
 
   byte                   l3_message_length;
   byte                   layer3_message[MAX_OCTETS_L3_MESSAGE];
} rr_data_req_T;

/*************************************************************
 * RR_DEACTIVATION_REQUEST primitive
 * FROM : MM
 * TO   : RR
 * DESCRIPTION :
 * Request from MM to deactivate the RR Layer.
 ************************************************************/

typedef struct
{
   IMH_T                     message_header;
   rr_mm_gsm_deact_reason_T  deact_reason;

   sys_modem_as_id_e_type    as_id;

} rr_deact_req_T;

/*************************************************************/
/* RR_ESTABLISH_REQUEST primitive                            */
/* FROM : MM                                                 */
/* TO   : RR                                                 */
/* DESCRIPTION :                                             */
/* Request from the Mobility Management entity of the        */
/* establishment of a Mobile Originated RR connection        */
/*************************************************************/

typedef struct
{
   IMH_T                     message_header;
   byte                      establishment_cause;
   byte                      layer3_message[N201_MAX];
   byte                      l3_message_size;

   sys_modem_as_id_e_type    as_id;

   boolean                   is_periodic_lau;

} rr_est_req_T;


/*************************************************************/
/* RR_PS_RESUME_IND primitive                    */
/* FROM : MM                                                 */
/* TO   : RR                                                 */
/* DESCRIPTION :                                             */
/* Informes RR/GRR that PS can resumed and the PS restriction,*/
/* imposed to manual search on other SUB is removed now      */
/*************************************************************/
typedef struct
{
   IMH_T                   message_header;
   sys_modem_as_id_e_type as_id;
} rr_ps_resume_ind_T;



/*************************************************************/
/* RR_GMM_READY_TIMER_STATE_REQ primitive                    */
/* FROM : MM                                                 */
/* TO   : RR                                                 */
/* DESCRIPTION :                                             */
/* Informes RR/GRR about current state of the READY TIMER,   */
/* If negotiated value of READY TIMER is iqual to 0 the      */
/* primitive reporting TRUE and FALSE otherwise              */
/*************************************************************/

typedef struct
{
  IMH_T              message_header;
  boolean            negotiated_ready_timer_zero ;

  sys_modem_as_id_e_type      as_id;

} rr_gmm_ready_timer_state_req_T;

/*************************************************************/
/* RR_GMM_GPRS_TLLI_ASSIGNMENT_REQ primitive                 */
/* FROM : MM                                                 */
/* TO   : RR                                                 */
/* DESCRIPTION :                                             */
/* Informs the RR/GRR about the new TLLI                     */
/*************************************************************/

typedef struct
{
  IMH_T                   message_header;
  dword                   oldTLLI ;
  dword                   newTLLI ;
  sys_srv_domain_e_type   service_domain;
  sys_modem_as_id_e_type  as_id;

} rr_gmm_gprs_tlli_assignment_req_T;

/*************************************************************/
/* RR_GMM_GPRS_STATE_CHANGE_REQ primitive                    */
/* FROM : MM                                                 */
/* TO   : RR                                                 */
/* DESCRIPTION :                                             */
/* Informs the RR/GRR about the new GMM GPRS state           */
/*************************************************************/

typedef struct
{
  IMH_T                    message_header;
  gmm_llc_if_state_type    new_state;

  sys_modem_as_id_e_type   as_id;

} rr_gmm_gprs_state_change_req_T;

/*************************************************************/
/* RR_GMM_GPRS_MS_RA_CAP_IE_IND primitive                    */
/* FROM : MM                                                 */
/* TO   : RR                                                 */
/* DESCRIPTION :                                             */
/* Passes the MS Radio Access Capabilities IE to GRR         */
/*************************************************************/

typedef struct
{
  IMH_T                       message_header;
  uint16                      ms_ra_cap_ie_len_bits;
  uint8                       * ms_ra_cap_ie;

  sys_modem_as_id_e_type      as_id;

} rr_gmm_gprs_ms_ra_cap_ie_ind_T;

/*************************************************************/
/* RR_GMM_START_MM_NON_DRX_IND primitive                     */
/* FROM : MM                                                 */
/* TO   : RR                                                 */
/* DESCRIPTION :                                             */
/* Indicates that MM has entered the MM non-DRX period       */
/*************************************************************/

typedef struct
{
  IMH_T              message_header;

  sys_modem_as_id_e_type      as_id;

} rr_gmm_start_mm_non_drx_ind_T;

/*************************************************************/
/* RR_GMM_STOP_MM_NON_DRX_IND primitive                      */
/* FROM : MM                                                 */
/* TO   : RR                                                 */
/* DESCRIPTION :                                             */
/* Indicates that MM has left the MM non-DRX period          */
/*************************************************************/

typedef struct
{
  IMH_T             message_header;

  sys_modem_as_id_e_type     as_id;

} rr_gmm_stop_mm_non_drx_ind_T;

/*************************************************************/
/* RR_GMM_GPRS_SERVICE_CNF primitive                         */
/* FROM : MM                                                 */
/* TO   : RR                                                 */
/* DESCRIPTION :                                             */
/* Indicates resumption of GPRS service                      */
/*************************************************************/
typedef struct
{
  IMH_T                message_header;
  sys_modem_as_id_e_type        as_id;
} rr_gmm_gprs_service_cnf_T;


/*************************************************************/

/* RR_GMM_GPRS_MULTISLOT_CLASS_IND  primitive                         */
/* FROM : MM                                                 */
/* TO   : RR                                                 */
/* DESCRIPTION :                                             */
/* Indicates number of multislots used un the next Routing    */
/* Area update*/
/*************************************************************/

typedef struct
{
  IMH_T                message_header;

  byte                 multi_slot_class;
  byte                 edge_multi_slot_class;
  uint8                dtm_multi_slot_subclass;
  sys_modem_as_id_e_type        as_id;
} rr_gmm_gprs_multislot_class_ind_T;



/*************************************************************/
/* RR_FORBIDDEN_LAI_UPDATE_REQ primitive                     */
/* FROM : MM                                                 */
/* TO   : RR                                                 */
/* DESCRIPTION :                                             */
/* Message updating RR with the current list of forbidden    */
/* LAI                                                       */
/*************************************************************/

typedef struct
{
   IMH_T                     message_header;
   inter_task_lai_list_T     forbidden_lai_list;

   sys_modem_as_id_e_type    as_id;

} rr_forbidden_lai_update_req_T;

/*************************************************************
 * RR_PLMN_LIST_REQUEST primitive
 * FROM : MM
 * TO   : RR
 * DESCRIPTION :
 * MM Asks for the list of the PLMNs which are present
 *************************************************************/

typedef struct
{
   IMH_T                             message_header;
   sys_network_selection_mode_e_type network_selection_mode;

   sys_band_mask_type                band_pref;


   boolean                           gsm_only;


   sys_plmn_rat_list_s_type          plmn_list;

   byte                              trans_id;

   sys_service_search_e_type         service_search;


   sys_rat_pri_list_info_s_type      rat_pri_list_info;

  /* following rat pri list to be used as a filter 
  ** in bplmn search by lower layers*/  
   sys_rat_pri_list_info_s_type    mcc_mode_band_filter;   

   sys_manual_search_timer_value_type   search_timer_value;  
   sys_scan_scope_e_type   scan_scope;                     


  sys_modem_as_id_e_type             as_id;

}  rr_plmn_list_req_T;

/*************************************************************
* RR_PLMN_SELECT_REQUEST primitive
* FROM : MM
* TO   : RR
* DESCRIPTION :
* A selection of LAI has been made, either manually or
* automatically. The selection is sent to RR requesting
* it to camp on the appropriate channel which matches the
* request
*************************************************************/
typedef struct
{
   IMH_T                              message_header;

   sys_network_selection_mode_e_type  network_select_mode;

   /* PLMNId of the requested PLMN */
   PLMN_id_T                          PLMN_id;

   /* TRUE if only GSM RAT is active */
   boolean                            gsm_only;

   /* Requested Service domain, CS, PS or CS_PS */
   sys_srv_domain_e_type              service_domain;


   /* User band preference */
   sys_band_mask_type                 band_pref;


  sys_rat_pri_list_info_s_type    rat_pri_list_info;



   rr_mm_service_req_cause_e_type     service_req_cause;
   
   boolean                            is_ehplmn_to_be_camped;

   boolean                            use_sib7;

   sys_modem_as_id_e_type             as_id;

} rr_plmn_select_req_T;

/*************************************************************/
/* RR_SIM_INSERTED_REQ                                       */
/* FROM : MM                                                 */
/* TO   : RR                                                 */
/* DESCRIPTION :                                             */
/*************************************************************/

typedef struct
{
  IMH_T                   message_header;
  mm_sim_efkc_T           ciphering_key;
  mm_sim_efkcsn_T         ciphering_key_seq_no;
  mm_sim_efacc_T          access_control;
  mm_sim_imsi_T           imsi;
  mm_sim_tmsi_T           tmsi;
  mm_sim_efad_T           admin_data;
  mm_sim_ptmsi_T          ptmsi;
  mm_sim_efkcgprs_T       gprs_ciphering_key;
  mm_gsm_kc_128bit_T      gsm_kc_128bit;
  mm_gsm_kc_128bit_T      gsm_gprs_kc_128bit;
  mm_sim_efkcsngprs_T     gprs_ciphering_key_seq_no;
  boolean                 bcch_allocation_valid;
  mm_sim_efbcch_T         bcch_allocation;
  PLMN_id_T               reg_plmn;

  mm_sim_efloci_lai_T     loci_lai;
  mm_sim_update_status_T  loci_update_status;
  mm_sim_eflocigprs_rai_T locigprs_rai;
  mm_sim_update_status_T  locigprs_update_status;
  byte                    imsi_mnc_len;  


  boolean                 dual_mode;
  boolean                 wcdma_allowed;
  sys_modem_as_id_e_type  as_id;

} rr_sim_inserted_req_T;

/*************************************************************/
/* RR_INVALIDATE_SIM_DATA_REQ                                */
/* FROM : MM                                                 */
/* TO   : RR                                                 */
/* DESCRIPTION :                                             */
/*************************************************************/

typedef struct
{
   IMH_T                 message_header;

   sys_modem_as_id_e_type as_id;

} rr_invalidate_sim_data_req_T;

/*************************************************************/
/* RR_SIM_UPDATE_REQ                                         */
/* FROM : MM                                                 */
/* TO   : RR                                                 */
/* DESCRIPTION : Update SIM information in RR                */
/*************************************************************/

typedef struct
{
  IMH_T                   message_header;
  mm_sim_efkc_T           ciphering_key;
  mm_sim_efkcsn_T         ciphering_key_seq_no;
  mm_sim_tmsi_T           tmsi;
  mm_sim_ptmsi_T          ptmsi;
  mm_sim_efkcgprs_T       gprs_ciphering_key;
  mm_sim_efkcsngprs_T     gprs_ciphering_key_seq_no;
  mm_gsm_kc_128bit_T      gsm_kc_128bit;
  mm_gsm_kc_128bit_T      gsm_gprs_kc_128bit;
  routing_area_id_T       registered_rai;

  mm_sim_efloci_lai_T     loci_lai;
  mm_sim_update_status_T  loci_update_status;
  mm_sim_update_status_T  locigprs_update_status;


  boolean                 wcdma_allowed;
  sys_modem_as_id_e_type  as_id;

} rr_sim_update_req_T;

/*************************************************************
 * RR_STOP_GSM_MODE_REQUEST primitive
 * FROM : MM
 * TO   : RR
 * DESCRIPTION :
 * Request from MM to deactivate the GSM mode
 ************************************************************/

typedef struct
{
   IMH_T                    message_header;
   rr_mm_gsm_stop_reason_T  rr_mm_gsm_stop_reason;

   sys_modem_as_id_e_type   as_id;

} rr_stop_gsm_mode_req_T;



typedef enum
{
  MM_RR_DUAL_STANDBY_CS_MO_CALL,
  MM_RR_PS_ABORT_MANUAL_PLMN_SEARCH,
  MM_RR_PS_ABORT_CAUSE_MAX = 0xff
} rr_ps_abort_cause_e_type;

typedef struct

{

  IMH_T                  message_header;

  rr_ps_abort_cause_e_type  abort_cause;

  sys_modem_as_id_e_type as_id;

} rr_ps_abort_req_T;



/**************************/
/* Messages from RR to MM */
/**************************/

/*************************************************************
 * RR_ABORT_CONFIRM primitive
 * FROM : RR
 * TO   : MM
 * DESCRIPTION :
 * The RR connection has been released
 *************************************************************/

typedef struct
{
  IMH_T                   message_header;
  mm_as_status_T          status;

  sys_modem_as_id_e_type  as_id;

} rr_abort_cnf_T;

/*************************************************************/
/* RR_ABORT_INDICATION primitive                             */
/* FROM : RR                                                 */
/* TO   : MM                                                 */
/* DESCRIPTION :                                             */
/* Indication that the RR connection has been aborted by a   */
/* lower layer                                               */
/*************************************************************/

typedef struct
{
   IMH_T                        message_header;
   byte                         RR_abort_cause;

   mm_rr_failure_cause_e_type   failure_cause;
   sys_modem_as_id_e_type       as_id;

} rr_abort_ind_T;

/*************************************************************/
/* RR_PS_STATUS_CHANGE_INDICATION primitive                             */
/* FROM : RR                                                 */
/* TO   : MM                                                 */
/* DESCRIPTION :                                             */
/* Indication that the RR has any uplink data to be sent     */
/* lower layer                                               */
/*************************************************************/
typedef struct
{
  IMH_T                  message_header;
  sys_modem_as_id_e_type as_id;
  boolean                ps_is_active;
} rr_ps_status_change_ind_T;


/*************************************************************/
/* RR_DATA_INDICATION primitive                              */
/* FROM : RR                                                 */
/* TO   : MM                                                 */
/* DESCRIPTION :                                             */
/* Is used by RR to indicate control data, which has been    */
/* received from its peer entity on the network side         */
/*************************************************************/

typedef struct
{
   IMH_T                   message_header;
// byte                    l3_message_length;
   byte                    layer3_message[MAX_OCTETS_L3_MESSAGE];
} rr_data_ind_T;


typedef struct
{
   IMH_T                   message_header;
   sys_modem_as_id_e_type  as_id;
   byte                    l3_message_length;
   byte                    layer3_message[MAX_OCTETS_L3_MESSAGE];
} rr_ds_data_ind_T;


/*************************************************************
 * RR_DEACTIVATION_CONFIRM primitive
 * FROM : RR
 * TO   : MM
 * DESCRIPTION :
 * Confirmation of the RR deactivation
 ************************************************************/

typedef struct
{
   IMH_T                   message_header;

   sys_modem_as_id_e_type  as_id;

} rr_deact_cnf_T;

/*************************************************************
 * RR_ESTABLISHMENT_INDICATION primitive
 * FROM : RR
 * TO   : MM
 * DESCRIPTION :
 *The establishment of an unsolicited CS assignment (EDTM only)
 will cause GRR to send the new primitive RR_EST_IND to MM.
 ************************************************************/


typedef struct
{
   IMH_T                   message_header;

   sys_modem_as_id_e_type  as_id;

} rr_est_ind_T;


/*************************************************************/
/* RR_ESTABLISH_CONFIRM primitive                            */
/* FROM : RR                                                 */
/* TO   : MM                                                 */
/* DESCRIPTION :                                             */
/* Indicates the successful completion of a Mobile           */
/* Originated RR connection establishment. RR connection     */
/* exists and RR is in the dedicated mode                    */
/*************************************************************/

typedef struct
{
   IMH_T                   message_header;

   sys_modem_as_id_e_type  as_id;

} rr_est_cnf_T;



typedef struct
{
   IMH_T                   message_header;

   sys_modem_as_id_e_type  as_id;

} rr_gtow_cco_complete_ind_T;



typedef enum
{
  CS_PAGING,
  PS_PAGING,
  PS_PAGING_ABNORMAL,
  CS_PAGING_WITH_IMSI
} rr_paging_type_T ;

/*************************************************************/
/* RR_PAGING_IND primitive                                   */
/* FROM : MM                                                 */
/* TO   : RR                                                 */
/* DESCRIPTION :                                             */
/* Indicates the receipt of a page by RR                     */
/*************************************************************/

typedef struct
{
   IMH_T                message_header ;
   boolean              paged_with_TLLI;
   rr_paging_type_T     paging_type ;

   sys_modem_as_id_e_type        as_id;

} rr_paging_ind_T;

/*************************************************************
 * RR_PLMN_LIST_CONFIRM primitive
 * FROM : RR
 * TO   : MM
 * DESCRIPTION :
 * This primitive indicates the list of available PLMN present
 *************************************************************/

typedef struct
{
   IMH_T                              message_header;

   sys_network_selection_mode_e_type  net_selection_mode;

   sys_detailed_plmn_list_s_type      available_plmn_list;
   mm_as_status_T                     status;

   byte                               trans_id;

   sys_service_search_e_type          service_search;




   mm_rr_failure_cause_e_type         failure_cause;
   sys_modem_as_id_e_type             as_id;

} rr_plmn_list_cnf_T;

/*************************************************************
 * RR_PLMN_SELECT_CONFIRM primitive
 * FROM : RR
 * TO   : MM
 * DESCRIPTION :
 * RR has attempted to camp on a channel corresponding
 * to the selected PLMN
 *************************************************************/

typedef struct
{
   IMH_T                          message_header;
   mm_as_status_T                 error_code;
   mm_as_service_state_T          service_state;
   inter_task_lai_T               location_area_identification;
   byte                           ATT;
   byte                           T3212_timeout_value;
   byte                           reestablishment_allowed;
   byte                           MSCR;
   mm_sim_efbcch_T                bcch_allocation;
   mm_as_cell_access_status_T     cell_access;
   sys_srv_capability_e_type      service_capability;
   sys_detailed_plmn_list_s_type  available_plmn_list;

  boolean                         gprs_sys_info_present ;
  mm_gprs_sys_info_T              gprs_sys_info ;


   sys_band_mask_e_type           band_pref;

   boolean                        ue_oos;
   boolean                        gmm_suspended;
   sys_band_class_e_type          band_indicator;


   mm_rr_failure_cause_e_type     failure_cause;
   sys_modem_as_id_e_type         as_id;

   boolean                        t3122_running;

   boolean                      gsm_search_complete;

   ARFCN_T                      arfcn;
   uint8                        bsic;
   word  cell_identity;
   boolean cell_identity_valid;

} rr_plmn_select_cnf_T;

/*************************************************************
 * RR_REL_IND primitive
 * FROM : RR
 * TO   : MM
 * DESCRIPTION :
 * RR tells MM that the RR connection has been released
 *************************************************************/

typedef struct
{
   IMH_T                        message_header;
   byte                         RR_cause;


   mm_rr_failure_cause_e_type   failure_cause;
   sys_modem_as_id_e_type       as_id;

} rr_rel_ind_T;

/*************************************************************/
/* RR_SERVICE_IND                                            */
/* FROM : RR                                                 */
/* TO   : MM                                                 */
/* DESCRIPTION :                                             */
/* RR has selected another cell in another location area     */
/*************************************************************/

typedef struct
{
  IMH_T                       message_header;
  mm_as_service_state_T       service_state;
  inter_task_lai_T            location_area_identification;
  byte                        ATT;
  byte                        T3212_timeout_value;
  byte                        reestablishment_allowed;
  byte                        MSCR;
  mm_sim_efbcch_T             bcch_allocation;
  boolean                     cell_changed;
  mm_as_cell_access_status_T  cell_access;
  sys_srv_capability_e_type   service_capability;

  boolean                     gprs_sys_info_present ;
  mm_gprs_sys_info_T          gprs_sys_info ;


  sys_band_mask_e_type        band_pref;

  boolean                     gmm_suspended;

  boolean                     ps_handover_indicator;

  sys_band_class_e_type       band_indicator;


  mm_rr_failure_cause_e_type  failure_cause;
  sys_modem_as_id_e_type      as_id;

  boolean                     t3122_running;

  ARFCN_T                     arfcn;
  uint8                       bsic;
  
  word cell_identity;
  boolean cell_identity_valid;
} rr_service_ind_T;

/*************************************************************
 * RR_STOP_GSM_MODE_CONFIRM primitive
 * FROM : RR
 * TO   : MM
 * DESCRIPTION :
 * Confirmation of the GSM mode deactivation
 ************************************************************/

typedef struct
{
   IMH_T                   message_header;
   mm_as_status_T          status;

   sys_modem_as_id_e_type  as_id;

} rr_stop_gsm_mode_cnf_T;

/*************************************************************/
/* RR_SYNC_INDICATION primitive                              */
/* FROM : RR                                                 */
/* TO   : MM                                                 */
/* DESCRIPTION :                                             */
/* Is used for synchronizing RR and the Mobility Management  */
/* entity.                                                   */
/* This indication is provided to MM in the following cases: */
/* 1. ciphering has been started                             */
/* 2. a traffic channel has been assigned                    */
/* 3. The channel mode has been modified                     */
/* 4. A handover has taken place                             */
/*************************************************************/

typedef struct
{
   IMH_T                   message_header;
   sync_reason_T           sync_reason;
   channel_mode_T          channel_mode;
   channel_type_T          channel_type;
   byte                    subchannel;
   boolean                 ciphering_enabled;

   sys_modem_as_id_e_type  as_id;

} rr_sync_ind_T;

/*************************************************************/
/* RR_ACTIVATION_INDICATION primitive                        */
/* FROM : RR                                                 */
/* TO   : MM                                                 */
/* DESCRIPTION :                                             */
/* Indicates that RR has become active RAT due to InterRAT   */
/* Handover or Cell Reselection.                             */
/*************************************************************/

typedef struct
{
   IMH_T                   message_header;
   mm_as_status_T          status;

   sys_modem_as_id_e_type  as_id;

} rr_activation_ind_T;

/*************************************************************/
/* RR_ACTIVATION_RSP primitive                               */
/* FROM : MM                                                 */
/* TO   : RR                                                 */
/* DESCRIPTION :                                             */
/* This is sent in response to ACTIVATION IND from RR        */
/* Indicates that MM acknowledges GSM as active RAT          */
/*************************************************************/

typedef struct
{
   IMH_T                   message_header;
   boolean                 any_cell_selection_mode;
   PLMN_id_T               PLMN_id;
   sys_srv_domain_e_type   service_domain;
   boolean                 PS_Attached;
   byte                    routing_area_code;
   word                    location_area_code;

   sys_modem_as_id_e_type  as_id;

   routing_area_id_T       registered_rai;
} rr_activation_rsp_T;


/*************************************************************/
/* RR_GMM_GPRS_SUSPENSION primitive                          */
/* FROM : RR                                                 */
/* TO   : GMM                                                */
/* DESCRIPTION :                                             */
/* Indicates that mobile is about to make a CS call and      */
/* therefore GPRS needs to be suspended                      */
/*************************************************************/
typedef struct
{
    IMH_T                  message_header;

    sys_modem_as_id_e_type as_id;

} rr_gmm_gprs_suspension_T;

/*************************************************************/
/* RR_GMM_GPRS_RESUMPTION_IND primitive                      */
/* FROM : RR                                                 */
/* TO   : GMM                                                */
/* DESCRIPTION :                                             */
/* Indicates that GPRS can leave the SUSPENDED substate      */
/*************************************************************/
typedef struct
{
   IMH_T                  message_header;
   boolean                gprs_resumption_ind;

   sys_modem_as_id_e_type as_id;


   boolean                cell_update_required; 

} rr_gmm_gprs_resumption_ind_T;

/*************************************************************/
/* RR_GMM_GPRS_NV_PARAMS_IND primitive                       */
/* FROM : RR                                                 */
/* TO   : GMM                                                */
/* DESCRIPTION :                                             */
/* Indicates the NV parameter values for Multi_slot_class,   */
/* DRX_TIMER, SPLIT_PAGING_CYCLE and ANITE_GCF_FLAG to the   */
/* GPRS lower layers */
/*************************************************************/
typedef struct
{
  IMH_T                    message_header;
  byte                     nv_multislot_class;
  byte                     nv_non_drx_timer;
  byte                     nv_split_paging_cycle;
  byte                     nv_anite_gcf_flag;

  boolean                  nv_edge_feature_support;
  byte                     nv_edge_multislot_class;
  byte                     nv_edge_8psk_power_class;
  byte                     nv_edge_8psk_power_capability;


  boolean                  nv_edge_nw_ass_cell_change;
  byte                     nv_interrat_nacc_support;


  boolean                  nv_dtm_feature_support;
  uint8                    nv_dtm_multislot_subclass;


  boolean                  nv_edtm_feature_support;

  uint8                    nv_gea_supported ;

  uint8                    nv_hmsc_gprs_coded_ms_class;

  uint8                    nv_hmsc_egprs_coded_ms_class;


  uint16                   nv_disable_call_type;

  uint16                   nv_disable_call_type_subs_2;

  byte                     as_id;

} rr_gmm_gprs_nv_params_ind_t;

/*************************************************************
 * RR_RESELECTION_IND primitive
 * FROM : RR
 * TO   : MM
 * DESCRIPTION :
 * RR tells MM that a reselection is in progress
 *************************************************************/

typedef struct
{
   IMH_T                   message_header;

   sys_modem_as_id_e_type  as_id;

} rr_reselection_ind_T;


/*************************************************************
 * RR_GMM_GPRS_CCCH_CAMP_IND primitive
 * FROM : MM
 * TO   : RR
 * DESCRIPTION :
 * MM indicates to RR that it only needs to camp for GSM services
 *************************************************************/

typedef struct
{
   IMH_T                   message_header;

   sys_modem_as_id_e_type  as_id;

} rr_gmm_gprs_ccch_camp_ind_T;


/*************************************************************
 * RR_GMM_GPRS_PCCCH_CAMP_REQ primitive
 * FROM : MM
 * TO   : RR
 * DESCRIPTION :
 * MM request that RR camp for packet services
 *************************************************************/

typedef struct
{
   IMH_T                   message_header;

   sys_modem_as_id_e_type  as_id;

} rr_gmm_gprs_pccch_camp_req_T;


/*************************************************************
 * RR_GPRS_PCCCH_CAMP_CNF primitive
 * FROM : RR
 * TO   : MM
 * DESCRIPTION :
 * RR confirms to MM a previous request to camp for packet services
 *************************************************************/

typedef struct
{
   IMH_T                   message_header;

   sys_modem_as_id_e_type  as_id;

} rr_gprs_pccch_camp_cnf_T;
/*************************************************************
 * RR_GMM_GPRS_SERVICE_REQ primitive
 * FROM : MM
 * TO   : RR
 * DESCRIPTION :
 * MM request that RR camp for packet services
 *************************************************************/

typedef struct
{
   IMH_T                   message_header;

   sys_modem_as_id_e_type  as_id;

} rr_gmm_gprs_service_req_T;






/*************************************************************
 * RR_NW_SEL_MODE_RESET_IND primitive
 * FROM : MM
 * TO   : RR
 * DESCRIPTION :
 * Used by MM to indicate to reset the N/W mode to AUTOMATIC/ 
 * MANUAL when FULL service is acquired in LIMITED_ROUTINE_ACQ 
 * and LIMITED_MANUAL_ACQ.
 *************************************************************/
typedef struct
{
  IMH_T                              message_header;
  sys_network_selection_mode_e_type  network_select_mode;

  sys_modem_as_id_e_type             as_id;

}rr_nw_sel_mode_reset_ind_T;

/*************************************************************
* RR_CHANGE_MODE_IND
* FROM : MM
* To   : RR 
* DESCRIPTION : 
*Used by MM to inform the change in band pref in connected
*mode  to RR
*************************************************************/
typedef struct
{
    IMH_T                        message_header;
    boolean                      gsm_only;

    /* added band_pref for dual mode change handling*/
    sys_band_mask_type              band_pref;


    sys_rat_pri_list_info_s_type    rat_pri_list_info; 
    sys_srv_domain_e_type             service_domain; 


  sys_modem_as_id_e_type             as_id;

} rr_change_mode_ind_T;

typedef struct
{
 
   IMH_T              message_header;
   sys_srv_domain_e_type   service_domain;
   sys_srv_status_e_type   service_state;
   sys_modem_as_id_e_type  as_id;
} rr_service_status_ind_T;

/*************************************************************/
/* RR_LAI_REJECT_LIST_IND primitive                     */
/* FROM : MM                                                 */
/* TO   : RR                                                 */
/* DESCRIPTION :                                             */
/* Message updating RR with the current list of reject*/
/* LAIs                                                       */
/*************************************************************/

typedef struct
{
   IMH_T                     message_header;

   inter_task_lai_list_T     nas_lai_reject_list;

   sys_modem_as_id_e_type    as_id;

} rr_lai_reject_list_ind_T;


/*************************************************************
 * RR_GMM_GPRS_MS_RA_CAP_REQ primitive
 * FROM : RR
 * TO   : MM
 * DESCRIPTION :
 * RR needs MS RA capability now as LTE attach is in progress and NAS hasnt 
 * sent this information till now.
 ************************************************************/


typedef struct
{
   IMH_T                   message_header;

   sys_modem_as_id_e_type  as_id;

} rr_gmm_gprs_ms_ra_cap_req_T;


/*************************************************************
 * RR_LTE_RESELECTION_ALLOWED_STATUS_IND   primitive
 * FROM : MM
 * TO   : RR
 * DESCRIPTION :
 * Used by MM to enable/disable lte_reslection
 *************************************************************/
typedef struct
{
    IMH_T                        message_header;
    boolean                      lte_reselection_allowed;
    sys_modem_as_id_e_type       as_id;
} rr_lte_reselection_allowed_status_ind_T;

typedef struct
{
   IMH_T                 message_header;
   boolean call_aborted;
   sys_modem_as_id_e_type as_id; //This is for future use on DSDA targets.
} rr_csfb_blind_redir_to_lte_req_T;


/*************************************************************
 * RR_CSFB_CALL_STATUS_IND   primitive
 * FROM : MM
 * TO   : RR
 * DESCRIPTION :
 * Used by MM to indicate CSG call status
 *************************************************************/
typedef struct
{
   IMH_T          message_header;
   boolean        csfb_call_status; /*TRUE if call is activated, FALSE if call is cancelled*/
   boolean        csmt;
   sys_modem_as_id_e_type as_id;
} rr_csfb_call_status_ind_T;

/*************************************************************
 * DS_STATUS_CHANGE_IND primitive
 * FROM : MM
 * TO   : RR
 * DESCRIPTION :
 * Used by MM to indicate change in DUAL SIM DUAL STANDBY status to RR
 *************************************************************/
typedef struct
{
  IMH_T                            message_header;
  boolean                          dual_standby_mode;
  sys_modem_dual_standby_pref_e_type multi_standby_mode_pref;
  sys_modem_device_mode_e_type    device_mode;
}rr_ds_status_change_ind_T;

/*************************************************************
 * CM_MULTIMODE_SUBS_CHGD_REQ primitive
 * FROM : MM
 * TO   : RR
 * DESCRIPTION :
 * Used by MM to indicate subs_capability to RR
 *************************************************************/

typedef struct
{
   IMH_T                   message_header;
   subs_capability_e_type                     subs_capability[MAX_AS_IDS];
} rr_multimode_subs_chgd_req_T; 

/*************************************************************
 * CM_MULTIMODE_SUBS_CHGD_REQ primitive
 * FROM : RR
 * TO   : MM
 * DESCRIPTION :
 * Used by MM to indicate multimode_subs_chgd_cnf to MM
 *************************************************************/
typedef struct
{
   IMH_T                   message_header;
} rr_multimode_subs_chgd_cnf_T;
/**************************************************************/
typedef struct

{
  IMH_T                  message_header;
  sys_modem_as_id_e_type as_id;
} rr_ps_abort_cnf_T;

typedef struct
{
  IMH_T                  message_header;
  sys_modem_as_id_e_type as_id;
}rr_srvcc_handover_failure_ind_T;

typedef struct rr_ue_mode_ind_s
{
  IMH_T                          message_header;
  sys_ue_mode_e_type             ue_mode;
  sys_modem_as_id_e_type as_id;
}rr_ue_mode_ind_T;

typedef struct rr_data_priority_ind_s
{
  IMH_T                          message_header;
  sys_data_priority_e_type       priority;
  sys_modem_as_id_e_type         as_id;
} rr_data_priority_ind_T;


typedef struct rr_pseudo_lte_list_update_s
{
  IMH_T                          message_header;
  boolean  can_use_pseudo;
  sys_modem_as_id_e_type as_id;
}rr_pseudo_lte_list_update_ind_T;

typedef struct
{
   IMH_T                   message_header;
   sys_modem_as_id_e_type  as_id;
} rr_csg_white_list_update_ind_T;

typedef union
{
  IMH_T                              header;
  rr_abort_req_T                     rr_abort_req;
  rr_data_req_T                      rr_data_req;
  rr_deact_req_T                     rr_deact_req;
  rr_est_req_T                       rr_est_req;
  rr_plmn_list_req_T                 rr_plmn_list_req;
  rr_plmn_select_req_T               rr_plmn_select_req;
  rr_forbidden_lai_update_req_T      rr_forbidden_lai_update_req;
  rr_sim_inserted_req_T              rr_sim_inserted_req;
  rr_invalidate_sim_data_req_T       rr_invalidate_sim_data_req;
  rr_stop_gsm_mode_req_T             rr_stop_gsm_mode_req;
  rr_sim_update_req_T                rr_sim_update_req;
  rr_act_req_T                       rr_act_req;
  rr_eq_plmn_list_change_ind_T       rr_eq_plmn_list_change_ind;
  rr_bplmn_search_abort_req_T        rr_bplmn_search_abort_req;
  rr_nw_sel_mode_reset_ind_T         rr_nw_sel_mode_reset_ind;
  rr_change_mode_ind_T               rr_change_mode_ind;
  rr_service_status_ind_T            rr_service_status_ind;
  rr_lte_reselection_allowed_status_ind_T  rr_lte_reselection_allowed_status_ind;
  rr_gmm_gprs_state_change_req_T     rr_gmm_gprs_state_change_req;
  rr_gmm_gprs_tlli_assignment_req_T  rr_gmm_gprs_tlli_assignment_req;
  rr_gmm_ready_timer_state_req_T     rr_gmm_ready_timer_state_req;
  rr_gmm_gprs_ms_ra_cap_ie_ind_T     rr_gmm_gprs_ms_ra_cap_ie_ind;
  rr_gmm_start_mm_non_drx_ind_T      rr_gmm_start_mm_non_drx_ind;
  rr_gmm_stop_mm_non_drx_ind_T       rr_gmm_stop_mm_non_drx_ind;
  rr_gmm_gprs_service_cnf_T          rr_gmm_gprs_service_cnf;
  rr_gmm_gprs_nv_params_ind_t        rr_gmm_gprs_nv_params_ind;
  rr_gmm_gprs_ccch_camp_ind_T        rr_gmm_gprs_ccch_camp_ind;
  rr_gmm_gprs_pccch_camp_req_T       rr_gmm_gprs_pccch_camp_req;
  rr_gmm_gprs_service_req_T          rr_gmm_gprs_service_req;  
  rr_gmm_gprs_auth_fail_ind_T        rr_gmm_gprs_auth_fail_ind;
  rr_csfb_blind_redir_to_lte_req_T   rr_csfb_blind_redir_to_lte_req;
  rr_lai_reject_list_ind_T           rr_lai_reject_list_ind;
  rr_ds_status_change_ind_T          rr_ds_status_change_ind;
  rr_ps_abort_req_T                  rr_ps_abort_req;
  rr_csfb_call_status_ind_T          rr_csfb_call_status_ind;
  rr_ue_mode_ind_T                   rr_ue_mode_ind;
  rr_multimode_subs_chgd_req_T       rr_multimode_subs_chgd_req;
  rr_csg_white_list_update_ind_T     rr_csg_white_list_update_ind;
  rr_data_priority_ind_T             rr_data_prio_ind;
  rr_pseudo_lte_list_update_ind_T    rr_pseudo_lte_list_update_ind;  
  rr_ps_resume_ind_T                 rr_ps_resume_ind;
  rr_mt_csfb_failure_ind_T           rr_mt_csfb_failure_ind;
} mm_rr_msgs_u;

typedef union
{
  IMH_T                              header;
  rr_abort_cnf_T                     rr_abort_cnf;
  rr_abort_ind_T                     rr_abort_ind;
  rr_data_ind_T                      rr_data_ind;
  rr_deact_cnf_T                     rr_deact_cnf;
  rr_est_cnf_T                       rr_est_cnf;
  rr_paging_ind_T                    rr_paging_ind;
  rr_plmn_list_cnf_T                 rr_plmn_list_cnf;
  rr_plmn_select_cnf_T               rr_plmn_select_cnf;
  rr_rel_ind_T                       rr_rel_ind;
  rr_service_ind_T                   rr_service_ind;
  rr_sync_ind_T                      rr_sync_ind;
  rr_stop_gsm_mode_cnf_T             rr_stop_gsm_mode_cnf;
  rr_activation_ind_T                rr_activation_ind;
  rr_activation_rsp_T                rr_activation_rsp;

  rr_gmm_gprs_suspension_T           rr_gmm_gprs_suspension;
  rr_gmm_gprs_resumption_ind_T       rr_gmm_gprs_resumption_ind;
  rr_reselection_ind_T               rr_reselection_ind;
  rr_gprs_pccch_camp_cnf_T           rr_gprs_pccch_camp_cnf;
  rr_gmm_gprs_service_cnf_T          rr_gmm_gprs_service_cnf;
  rr_gmm_gprs_ms_ra_cap_req_T        rr_gmm_gprs_ms_ra_cap_req;


  rr_gtow_cco_complete_ind_T         rr_gtow_cco_complete_ind;


  rr_service_ind_T                   rr_service_ind_from_ota_msg;


  rr_est_ind_T                       rr_est_ind;
  rr_ps_abort_cnf_T                  rr_ps_abort_cnf;
  rr_ds_data_ind_T                   rr_ds_data_ind;
  
  rr_gmm_gprs_multislot_class_ind_T  rr_gmm_gprs_multislot_class_ind;
  rr_srvcc_handover_failure_ind_T    rr_srvcc_handover_failure_ind;
  rr_multimode_subs_chgd_cnf_T       rr_multimode_subs_chgd_cnf;
} rr_mm_msgs_u;

typedef union
{
  IMH_T            header;
  mm_rr_msgs_u     mm_rr;
  rr_mm_msgs_u     rr_mm;
} rr_sap_msgs_u;



extern boolean  mm_rr_plmn_lai_forbidden( inter_task_lai_T lai_info );

extern boolean mm_get_last_updated_lai_in_lte (inter_task_lai_T *rr_lai, const sys_modem_as_id_e_type as_id);

extern boolean  mm_rr_plmn_lai_forbidden_per_subs( inter_task_lai_T lai_info ,sys_modem_as_id_e_type as_id );



typedef struct 
{
   byte     data[5];
}nas_container_T;

/*************************************************************
 * RR_PSHO_IND primitive
 * FROM : RR
 * TO   : GMM
 * DESCRIPTION :
 * Used by RR to indicate to that PSHO is in progress 
 * and along with it the NAS parameters that have to be passed 
 * on to DATA Plane.
 *************************************************************/
typedef struct
{
  IMH_T                   message_header;
  boolean                 nas_container_present;
  nas_container_T         nas_container;

  sys_modem_as_id_e_type  as_id;

}rr_psho_ind_T;

/*************************************************************
 * RR_PSHO_CNF primitive
 * FROM : GMM
 * TO   : RR
 * DESCRIPTION :
 * GMM intimates RR that DATA plane has been configured with
 * the NAS parameters received from NW.
 *************************************************************/
typedef struct
{
   IMH_T                   message_header;

   sys_modem_as_id_e_type  as_id;

} rr_psho_cnf_T;





#endif // mm_rr_h
