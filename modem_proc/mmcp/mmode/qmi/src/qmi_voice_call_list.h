#ifndef _DS_QMI_VOICE_CALL_LIST_H
#define _DS_QMI_VOICE_CALL_LIST_H
/*===========================================================================

                         Q M I _ V O I C E _ C A L L _ L I S T . H

DESCRIPTION

 The Data Services Qualcomm Voice Call list management header.


EXTERNALIZED FUNCTIONS



Copyright (c) 2009-2014 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
===========================================================================*/
/*===========================================================================

                      EDIT HISTORY FOR FILE

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/mmode/qmi/src/qmi_voice_call_list.h#1 $ $DateTime: 2016/12/13 08:00:05 $ $Author: mplcsds1 $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
08/25/09    sk     Initial revision.
===========================================================================*/

#include "mmcp_variation.h"
#include "IxErrno.h"
#include "cm.h"
#include "qmi_voice_cm_if.h"
#include "cm_msgr_msg.h"


/*===========================================================================

                   EXTERNAL DEFINITIONS AND TYPES

===========================================================================*/

#define QMI_VOICE_CALL_LIST_REPORT_CALL_STATE_CHANGED( call_type ) \
  ( ( call_type == CM_CALL_TYPE_VOICE ) || ( call_type == CM_CALL_TYPE_EMERGENCY ) || ( call_type == CM_CALL_TYPE_CS_DATA ) || \
    ( call_type == CM_CALL_TYPE_TEST ) || ( call_type == CM_CALL_TYPE_OTAPA) || ( call_type == CM_CALL_TYPE_STD_OTASP ) || \
    ( call_type == CM_CALL_TYPE_NON_STD_OTASP ) || ( call_type == CM_CALL_TYPE_SUPS) || ( call_type == CM_CALL_TYPE_VT ) || \
    ( call_type == CM_CALL_TYPE_VS ) )

#define QMI_VOICE_CALL_LIST_CALL_TYPE_IS_VOICE( call_type ) \
  ( ( call_type == CM_CALL_TYPE_VOICE ) || ( call_type == CM_CALL_TYPE_EMERGENCY ) || ( call_type == CM_CALL_TYPE_TEST ) || \
    ( call_type == CM_CALL_TYPE_OTAPA) || ( call_type == CM_CALL_TYPE_STD_OTASP ) || ( call_type == CM_CALL_TYPE_NON_STD_OTASP ) || \
    ( call_type == CM_CALL_TYPE_VT ) )

#define QMI_VOICE_CALL_LIST_HANDLE_CALL_TYPES( call_type ) \
  ( ( call_type == CM_CALL_TYPE_VOICE ) || ( call_type == CM_CALL_TYPE_EMERGENCY ) || ( call_type == CM_CALL_TYPE_NONE ) || \
    ( call_type == CM_CALL_TYPE_TEST ) || ( call_type == CM_CALL_TYPE_OTAPA) || ( call_type == CM_CALL_TYPE_STD_OTASP ) || \
    ( call_type == CM_CALL_TYPE_NON_STD_OTASP ) || ( call_type == CM_CALL_TYPE_SUPS) || ( call_type == CM_CALL_TYPE_VT ) || \
    ( call_type == CM_CALL_TYPE_VS ))

/*! @brief Call State
*/
typedef enum
{
  QMI_VOICE_CALL_LIST_STATE_IDLE          = 0x0000,
  QMI_VOICE_CALL_LIST_STATE_ACTIVE        = 0x0001,
  QMI_VOICE_CALL_LIST_STATE_HOLDING       = 0x0002,
  QMI_VOICE_CALL_LIST_STATE_DIALING       = 0x0004,
  QMI_VOICE_CALL_LIST_STATE_ALERTING      = 0x0008,
  QMI_VOICE_CALL_LIST_STATE_INCOMING      = 0x0010,
  QMI_VOICE_CALL_LIST_STATE_WAITING       = 0x0020,
  QMI_VOICE_CALL_LIST_STATE_DISCONNECTING = 0x0040,
  QMI_VOICE_CALL_LIST_STATE_END           = 0x0080,
  QMI_VOICE_CALL_LIST_STATE_CC_IN_PROG    = 0x0100,
  QMI_VOICE_CALL_LIST_STATE_SETUP         = 0x0200

} qmi_voice_call_list_state_e_type;

typedef enum qmi_voice_call_list_otasp_status_e {


  QMI_VOICE_CALL_LIST_OTASP_STATUS_NONE = -1,

  QMI_VOICE_CALL_LIST_OTASP_STATUS_SPL_UNLOCKED,
    /**< SPL unlocked. Only for user-initiated OTASP. */

  QMI_VOICE_CALL_LIST_OTASP_STATUS_SPC_RETRIES_EXCEEDED,
    /**< SPC retries exceeded. Only for user-initiated OTASP. */

  QMI_VOICE_CALL_LIST_OTASP_STATUS_AKEY_EXCHANGED,
    /**< A-key exchanged. Only for user-initiated OTASP. */

  /* 3 */
  QMI_VOICE_CALL_LIST_OTASP_STATUS_SSD_UPDATED,
    /**< SSD updated. For both user-initiated OTASP and 
         network-initiated OTASP(OTAPA). */

  QMI_VOICE_CALL_LIST_OTASP_STATUS_NAM_DOWNLOADED,
    /**< NAM downloaded. Only for user-initiated OTASP. */

  QMI_VOICE_CALL_LIST_OTASP_STATUS_MDN_DOWNLOADED,
    /**< Mobile Directory Number (MDN) downloaded. Only for 
         user-initiated OTASP. */

  QMI_VOICE_CALL_LIST_OTASP_STATUS_IMSI_DOWNLOADED,
    /**< IMSI downloaded. Only for user-initiated OTASP. */

  /* 7 */
  QMI_VOICE_CALL_LIST_OTASP_STATUS_PRL_DOWNLOADED,
    /**< Preferred Roaming List (PRL) downloaded. Only for 
         user-initiated OTASP. */

  QMI_VOICE_CALL_LIST_OTASP_STATUS_COMMITTED,
    /**< Commit successful. Only for user-initiated OTASP. */

  QMI_VOICE_CALL_LIST_OTASP_STATUS_OTAPA_STARTED,
    /**< OTAPA started. Only for network-initiated OTASP(OTAPA). */

  QMI_VOICE_CALL_LIST_OTASP_STATUS_OTAPA_STOPPED,
    /**< OTAPA stopped. Only for network-initiated OTASP(OTAPA). */

  /* 11 */
  QMI_VOICE_CALL_LIST_OTASP_STATUS_OTAPA_ABORTED,
    /**< OTAPA aborted. Only for network-initiated OTASP(OTAPA). */

  /* 12 */
  QMI_VOICE_CALL_LIST_OTASP_STATUS_OTAPA_COMMITTED,
    /**< OTAPA commited, Only for network-initiated OTASP(OTAPA). Added to meet WM requirement*/
 
  QMI_VOICE_CALL_LIST_OTASP_STATUS_MAX

} qmi_voice_call_list_otasp_status_e_type;


/*! @brief Call Info
*/
typedef struct
{
  uint8                               conn_index;  /* GSM index for use with, e.g. AT+CHLD */
  cm_call_id_type                     call_id;    /* Call ID */
  cm_call_type_e_type                 call_type;  /* Type of call e.g. Voice */
  cm_call_direction_e_type            direction;  /* Incoming or Outgoing */
  qmi_voice_call_list_state_e_type    prev_state; /* Previous state of the call */
  qmi_voice_call_list_state_e_type    state;      /* State of the call */
  sys_sys_mode_e_type                 call_mode;  /* Mode that the incoming or outgoing call started */
  cm_als_line_e_type                  line;       /* ALS line indicator */
  cm_num_s_type                       num;        /* Phone number */
  cm_call_mode_info_e_type            mode_info_type;  /* Call mode info type */
  boolean                             answered; /* Indicates if a call is answered */
  boolean                             is_privacy_valid; 
  uint8                               privacy;/* Voice privacy mode.Filled upon privacy evt from CM */
  boolean                             is_otasp_status_valid;
  qmi_voice_call_list_otasp_status_e_type  otasp_status; /* Applicable for CDMA only. Filled upon otasp status evt from CM */
  boolean                             is_srv_opt_valid;
  uint16                              srv_opt; /* Applicable for CDMA only */
  boolean                             is_alerting_type_valid;
  uint8                               alerting_type;
  boolean                             is_cnap_info_valid; /* CNAP info valid */
  qmi_voice_cm_if_cnap_info_s_type    cnap_info;
  cm_call_event_user_data_s_type      uus_data; /* UUS data. To be filled by checking is_user_data_present of gw_cs_call*/
  boolean                             is_alpha_valid; /* CNAP info valid */
  cm_alpha_s_type                     alpha_info;
  boolean                             is_conn_num_valid;
  qmi_voice_cm_if_num_info_s          conn_num_info;
  boolean                             is_diagnostics_info_valid;
  qmi_voice_cm_if_call_end_diagnostics_s_type diagnostic_info;
  boolean                             is_as_id_valid;   /* Subscription ID for Voice DSDS info valid */
  uint8                                       asubs_id;
  boolean                             is_multi_party; /* Indicates if a call is MPTY call */
  boolean                             is_called_party_num_valid;
  qmi_voice_cm_if_num_info_s          called_party_num_info;
  boolean                             is_redirecting_party_num_valid;
  qmi_voice_cm_if_num_info_s          redirecting_party_num_info;  
  boolean                             is_alert_pattern_valid;
  uint32                               alert_pattern; 
  boolean                             is_audio_attrib_valid;   /* Audio Attributes for VOIP/VT calls */
  uint32                              audio_attrib;
  boolean                             is_video_attrib_valid;   /* Video Attributes for VOIP/VT calls */
  uint32                              video_attrib;
  boolean                             is_attrib_status_valid;  /* Attribute Status for VOIP/VT calls */
  uint32                              attrib_status;
  boolean                             is_vs_call_variant_valid;
  uint32                               vs_call_variant;
  boolean                             is_srvcc_call_available;
  boolean                             is_srvcc_call;
  boolean                             is_parent_call_id_valid;    
  uint8                               parent_call_id;    /* parent Call ID */
  boolean                             is_parent_call_id_cleared;    /* parent Call ID cleared or not*/
  boolean                             is_drvcc_call_available;
  boolean                             is_drvcc_call;
  boolean                             is_drvcc_parent_call_id_valid;    
  uint8                               drvcc_parent_call_id;    /* DRVCC parent Call ID */
  boolean                             is_drvcc_parent_call_id_cleared;    /* DRVCC parent Call ID cleared or not*/


  boolean                             is_local_cap_available;
  qmi_voice_cm_if_call_capabilities   local_cap;
  boolean                             is_peer_cap_available;
  qmi_voice_cm_if_call_capabilities   peer_cap;

  boolean                             is_child_num_available;
  qmi_voice_cm_if_ip_num_s_type       child_num;

  boolean                             is_display_text_available;
  qmi_voice_cm_if_display_text_s_type display_text;

  boolean                             is_adnl_call_info_available;
  uint16                              num_indications;

  boolean                             is_second_alpha_valid;
  cm_alpha_s_type                     second_alpha_info;

  boolean                                 is_end_reason_text_available;
  qmi_voice_cm_if_end_reason_text_s_type  end_reason_text;

  boolean                             is_ip_caller_name_available;
  qmi_voice_cm_if_ip_caller_name_s_type ip_caller_name;

  boolean                             is_srvcc_caller_name_available;
  cm_npi_type                         name_pi;

  boolean                                is_orig_fail_reason_available;
  qmi_voice_cm_if_failure_cause_e_type   orig_fail_reason;

  cm_call_em_e_type                    em_type;

  boolean   is_connected_number_ECT;
  boolean   is_connected_number_ECT_valid;

  boolean is_secure_call_available;
  boolean is_secure_call;

  boolean is_called_party_ringing_status_avail;
  boolean is_called_party_ringing;

} qmi_voice_call_list_public_type;

/*! @brief Call IDs List
*/
typedef struct
{
  uint32 num_of_call_ids;                    /* Number of call IDs in the list */
  cm_call_id_type call_id[ QMI_VOICE_CM_IF_CALL_ID_MAX_PER_SUBS ]; /* Call ID */
  boolean answered[ QMI_VOICE_CM_IF_CALL_ID_MAX_PER_SUBS ];        /* Indicates whether the call has been answered */
} qmi_voice_call_list_call_ids_list_type;

/*! @brief Call Info List
*/
typedef struct
{
  uint32 num_of_calls;                               /* Number of calls in the list */
  cm_call_id_type call_id[ QMI_VOICE_CM_IF_CALL_ID_MAX_PER_SUBS ]; /* Call ids */
} qmi_voice_call_list_call_info_list_type;

typedef struct qmi_voice_call_list_buf_tag
{    
  qmi_voice_call_list_public_type    pub;
  struct qmi_voice_call_list_buf_tag *prev_ptr;
  struct qmi_voice_call_list_buf_tag *next_ptr;
} qmi_voice_call_list_buf_type;

/*===========================================================================

                    EXTERNAL FUNCTION PROTOTYPES

===========================================================================*/

void qmi_voice_call_list_init
(
  void
);

IxErrnoType qmi_voice_call_list_new
( 
  cm_call_id_type          call_id,
  cm_call_type_e_type      call_type,      
  sys_sys_mode_e_type      call_mode,
  cm_call_direction_e_type direction,
  qmi_voice_call_list_state_e_type      call_state,          
  cm_als_line_e_type       line,           
  cm_num_s_type            num,
  cm_alpha_s_type          alpha,
  cm_call_mode_info_e_type mode_info_type,
  boolean                  report_state_change,
  uint8                    as_id,
  cm_ip_call_attrib_info_s_type     call_attrib
);

void qmi_voice_call_list_free_all
( 
  void
);

IxErrnoType qmi_voice_call_list_free
( 
  cm_call_id_type call_id,
  boolean         report_state_changed
);

IxErrnoType qmi_voice_call_list_alert
( 
  cm_call_id_type call_id,
  uint8 call_progress
);

IxErrnoType qmi_voice_call_list_answer
( 
  cm_call_id_type call_id
);

IxErrnoType qmi_voice_call_list_connect
( 
  cm_call_id_type call_id
);

IxErrnoType qmi_voice_call_list_disconnecting
( 
  cm_call_id_type call_id
);

IxErrnoType qmi_voice_call_list_manage
( 
  const active_calls_list_s_type *active_calls_list_ptr,
  uint8                                   as_id
);

IxErrnoType qmi_voice_call_list_active_call
( 
  cm_call_id_type call_id
);

IxErrnoType qmi_voice_call_list_hold_call
( 
  cm_call_id_type call_id
);

IxErrnoType qmi_voice_call_list_query
( 
  cm_call_id_type call_id,
  qmi_voice_call_list_public_type *info_ptr
);

void qmi_voice_call_list_query_voice_call_ids_list_by_state
( 
  uint32 state_mask,
  qmi_voice_call_list_call_ids_list_type *call_ids_list_ptr,
  uint8  as_id
);

void qmi_voice_call_list_query_call_info_list
( 
  qmi_voice_call_list_call_info_list_type *info_list_ptr,
  uint8                                   as_id
);

void qmi_voice_call_list_query_call_info_list_per_subs
( 
  qmi_voice_call_list_call_info_list_type *info_list_ptr,uint8 as_id
);

IxErrnoType qmi_voice_call_list_update_voice_privacy_mode
( 
  cm_call_id_type call_id,
  boolean privacy_mode
);

uint32 qmi_voice_call_list_count_num_of_voice_calls
( 
  uint8  as_id
);

uint32 qmi_voice_call_list_count_num_of_voice_calls_by_state
( 
  uint16 state_mask,
  uint8  as_id
);

IxErrnoType qmi_voice_call_list_update_ip_caller_name
(
  const cm_mm_call_info_s_type * call_info_ptr
);

boolean qmi_voice_call_list_voice_is_emergency
( 
  cm_call_id_type call_id
);

void qmi_voice_call_list_report_all_call_status_changed
(
  uint8 as_id
);

void qmi_voice_call_list_report_voice_call_state_changed
(
  cm_call_type_e_type call_type,
  uint8               as_id
);

IxErrnoType qmi_voice_call_list_update_name_presentation
(
  cm_call_id_type call_id,
  uint8  name_presentation
);

IxErrnoType qmi_voice_call_list_update_number
( 
  cm_call_id_type call_id,
  cm_num_s_type * number
);

IxErrnoType qmi_voice_call_list_update_alpha_info
( 
  cm_call_id_type call_id,
  cm_alpha_s_type alpha
);

IxErrnoType qmi_voice_call_list_query_call_id
(
  cm_call_id_type call_id,
  qmi_voice_call_list_public_type* info_ptr
) ;

IxErrnoType qmi_voice_call_list_query_by_call_id_and_call_state
(
  cm_call_id_type call_id,
  qmi_voice_call_list_state_e_type call_state
) ;

IxErrnoType qmi_voice_call_list_query_sys_mode_from_call_list
(
  sys_sys_mode_e_type     *sys_mode,
  uint8                    as_id
) ;

IxErrnoType qmi_voice_call_list_replace
( 
  cm_call_id_type          call_id,
  uint32                   conn_index,
  cm_call_type_e_type      call_type,      
  sys_sys_mode_e_type      call_mode,
  cm_call_direction_e_type direction,
  cm_call_state_e_type     call_state,          
  cm_als_line_e_type       line,           
  cm_num_s_type            num,
  cm_call_mode_info_e_type mode_info_type,
  boolean                  report_call_state_changed
);

IxErrnoType qmi_voice_call_list_set_uus_data
(
  cm_call_id_type call_id,
  cm_call_event_user_data_s_type  *call_event_user_data
);

IxErrnoType qmi_voice_call_list_update_otasp_status
(
  cm_call_id_type         call_id,
  qmi_voice_call_list_otasp_status_e_type  otasp_status
);

IxErrnoType qmi_voice_call_list_update_cdma_srv_opt
(
  cm_call_id_type    call_id,
  uint16             srv_opt
);

IxErrnoType qmi_voice_call_list_update_call_state
(
  cm_call_id_type                  call_id,
  qmi_voice_call_list_state_e_type state
);

IxErrnoType qmi_voice_call_list_update_call_type
(
  cm_call_id_type                  call_id,
  cm_call_type_e_type              call_type
);

IxErrnoType qmi_voice_call_list_update_num_pi
(
  cm_call_id_type                  call_id,
  cm_npi_type                      npi
);

boolean qmi_voice_call_list_is_remote_alerting
(
  cm_call_id_type call_id
);

IxErrnoType qmi_voice_call_list_update_alerting_type
(
  cm_call_id_type call_id,
  uint8           alert_type
);

IxErrnoType qmi_voice_call_list_update_srvcc_caller_info_type
(
  cm_call_id_type call_id,
  cm_srvcc_caller_name_s_type     name_info
);


IxErrnoType qmi_voice_call_list_update_end_reason_text
(
  cm_call_id_type                call_id,
  cm_ip_call_end_reason_s_type  *ims_end_reason_ptr
);  

IxErrnoType qmi_voice_call_list_query_by_call_id
( 
  cm_call_id_type call_id,
  qmi_voice_call_list_public_type *info_ptr
);

IxErrnoType qmi_voice_call_list_update_coding_scheme
(
  cm_call_id_type call_id,
  uint8  coding_scheme
);

boolean qmi_voice_call_list_is_call_present
( 
  cm_call_id_type call_id
);

IxErrnoType qmi_voice_call_list_query_voice_calls_by_conn_index
( 
  uint8 conn_index,
  qmi_voice_call_list_public_type *info_ptr
);

boolean qmi_voice_call_list_is_mpty_call
(
  cm_call_id_type call_id
);


IxErrnoType qmi_voice_call_list_query_by_conn_index
( 
  uint8 conn_index,
  qmi_voice_call_list_public_type *info_ptr
);

IxErrnoType qmi_voice_call_list_get_conn_idx_from_call_id
( 
  cm_call_id_type call_id,
  uint8           *conn_idx
);

IxErrnoType qmi_voice_call_list_get_as_id
( 
  cm_call_id_type call_id,
  uint8  *as_id
);

boolean qmi_voice_call_list_is_mode_updated
(
void
);

IxErrnoType qmi_voice_call_list_incoming
( 
  cm_call_id_type call_id
);
/*CDMA only*/
IxErrnoType qmi_voice_call_list_update_num_info
(
  cm_call_id_type call_id,
  cm_num_s_type   *num_info
);

/*GW+CDMA*/
IxErrnoType qmi_voice_call_list_update_connected_num_info
(
  cm_call_id_type call_id,
  cm_num_s_type   *num_info,
  boolean is_connected_number_ECT
);

/*GW only*/
IxErrnoType qmi_voice_call_list_update_diagnostic_info
(
  cm_call_id_type call_id,
  ie_cm_cc_cause_s_type   *cc_cause
);

/*GW only*/
void qmi_voice_call_list_update_mpty_bit
(
  uint8 as_id
);

IxErrnoType qmi_voice_call_list_update_cnap_name
( 
  cm_call_id_type call_id,
  const byte *name,
  uint8 len
);

/* set mpty bit for single call */
IxErrnoType qmi_voice_call_list_set_mpty_status
( 
  cm_call_id_type call_id,
  boolean         is_conf_call
);

IxErrnoType qmi_voice_call_list_update_mode_info_type
(
  cm_call_id_type call_id,
  cm_call_mode_info_e_type  mode_type
);

IxErrnoType qmi_voice_call_list_update_called_party_num_info
(
  cm_call_id_type call_id, 
  cm_called_party_bcd_no_s_type *bcd_num
);

IxErrnoType qmi_voice_call_list_update_alerting_pattern
(
  cm_call_id_type         call_id,
  uint8  alerting_pattern
);


IxErrnoType qmi_voice_call_list_update_redirecting_party_num_info
(
  cm_call_id_type call_id,
  cm_num_s_type *num
);

boolean qmi_voice_call_list_update_call_attributes_type
(
  cm_call_id_type call_id,
  cm_ip_call_attrib_info_s_type  call_attrib_info,
  cm_call_type_e_type            call_type
);

IxErrnoType qmi_voice_call_list_update_call_mode
(
  cm_call_id_type call_id,
  sys_sys_mode_e_type  call_mode
);

boolean qmi_voice_call_list_is_pure_incom_call_present
( 
  uint8  as_id
);

IxErrnoType qmi_voice_call_list_set_srvcc_flag
(
  cm_call_id_type call_id,
  boolean         is_srvcc_call
);

IxErrnoType qmi_voice_call_list_reset_srvcc_available_flags
(
  void
);
IxErrnoType qmi_voice_call_list_reset_srvcc_caller_name_flags
(
  void
);
IxErrnoType qmi_voice_call_list_reset_parent_id_valid_flags
(
  void
);

IxErrnoType qmi_voice_call_list_set_drvcc_flag
(
  cm_call_id_type call_id,
  boolean         is_drvcc_call
);

IxErrnoType qmi_voice_call_list_reset_drvcc_available_flags
(
  void
);

IxErrnoType qmi_voice_call_list_reset_drvcc_parent_id_valid_flags
(
  void
);

void qmi_voice_call_list_fill_all_call_info
(
  qmi_voice_cm_if_all_call_info_s *info,
  uint8                            as_id
);


IxErrnoType qmi_voice_call_list_update_parent_id
(
  cm_call_id_type                  call_id,
  cm_call_id_type                  parent_id,
  boolean                          is_cleared,
  boolean                          is_int_ims
);

boolean qmi_voice_call_list_update_srvcc_calls_info
(
  cm_mid_srvcc_ho_comp_list_s_type *cm_data,
  uint8 *as_id
);

boolean qmi_voice_call_list_update_srvcc_calls_info_add
(
  cm_mid_srvcc_ho_comp_list_s_type *cm_data,
  uint8 *as_id
);

boolean qmi_voice_call_list_update_srvcc_calls_info_replace
(
  cm_mid_srvcc_ho_comp_list_s_type *cm_data,
  uint8 *as_id
);

boolean qmi_voice_call_list_update_srvcc_calls_info_delete
(
  cm_mid_srvcc_ho_comp_list_s_type *cm_data,
  uint8 *as_id
);

IxErrnoType qmi_voice_call_list_update_drvcc_parent_id
(
  cm_call_id_type                  call_id,
  cm_call_id_type                  parent_id,
  boolean                          is_cleared,
  boolean                          is_int_ims
);

boolean qmi_voice_call_list_update_drvcc_calls_info
(
  cm_mid_srvcc_ho_comp_list_s_type *cm_data,
  uint8 *as_id
);

boolean qmi_voice_call_list_update_drvcc_calls_info_add
(
  cm_mid_srvcc_ho_comp_list_s_type *cm_data,
  uint8 *as_id
);

boolean qmi_voice_call_list_update_drvcc_calls_info_replace
(
  cm_mid_srvcc_ho_comp_list_s_type *cm_data,
  uint8 *as_id
);

boolean qmi_voice_call_list_update_drvcc_calls_info_delete
(
  cm_mid_srvcc_ho_comp_list_s_type *cm_data,
  uint8 *as_id
);

IxErrnoType qmi_voice_call_list_update_call_capabilities
(
  cm_call_id_type call_id,
  cm_ip_call_cap_attrib_info_s_type  local_attr,
  cm_ip_call_cap_attrib_info_s_type  peer_attr
);

IxErrnoType qmi_voice_call_list_update_child_number
(
  cm_call_id_type call_id,
  cm_num_s_type child_num
);


IxErrnoType qmi_voice_call_list_update_display_text
(
  cm_call_id_type call_id,
  cm_display_text_s_type display_text
);


IxErrnoType qmi_voice_call_list_update_additional_call_info
(
  cm_call_id_type          call_id,
  cm_ip_caller_info_s_type *adnl_call_info_ptr,
  boolean                  *is_adnl_info_flag_set
);

IxErrnoType qmi_voice_call_list_query_call_mode_by_state
(
  sys_sys_mode_e_type     *sys_mode,
  uint8                    as_id,
  qmi_voice_call_list_state_e_type state
);

IxErrnoType qmi_voice_call_list_update_second_alpha_info
(
  cm_call_id_type              call_id,
  cm_orig_client_categ_e_type  client_categ,
  cm_alpha_s_type              alpha
);

IxErrnoType qmi_voice_call_list_get_conf_call_id
( 
  cm_call_id_type *call_id
);

#ifdef FEATURE_DUAL_SIM
IxErrnoType qmi_voice_call_list_update_as_id
(
  cm_call_id_type call_id,
  uint8           asubs_id
);

qmi_voice_call_list_buf_type *qmi_voice_call_list_get_callid_info
( 
  cm_call_id_type call_id
);
#endif /* FEATURE_DUAL_SIM */

IxErrnoType qmi_voice_call_list_update_orig_fail_reason
(
  cm_call_id_type      call_id,
  cm_call_end_e_type   end_status
);

IxErrnoType qmi_voice_call_list_update_called_party_ring_alert_info
(
  cm_call_id_type call_id,
  boolean is_called_party_ringing
);

#endif /* _DS_QMI_VOICE_CALL_LIST_H */

