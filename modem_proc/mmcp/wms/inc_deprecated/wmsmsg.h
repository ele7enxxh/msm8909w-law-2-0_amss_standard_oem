#ifndef WMSMSG_H
#define WMSMSG_H
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
           W I R E L E S S    M E S S A G I N G   S E R V I C E S
           wmsmsg.h -- Message Handling Group definitions

  This module defines the data types and functions for the Short Message
  Services (SMS) which are used by the client software and are part of the
  UAPI (User Application Programming Interface).
  -------------

    Copyright (c) 2001-2008,2010-2013 QUALCOMM Technologies Incorporated.
    All Rights Reserved.
    Qualcomm Confidential and Proprietary

  Export of this technology or software is regulated by the U.S. Government.
  Diversion contrary to U.S. law prohibited.

  All ideas, data and information contained in or disclosed by
  this document are confidential and proprietary information of
  Qualcomm Technologies Incorporated and all rights therein are expressly reserved.
  By accepting this material the recipient agrees that this material
  and the information contained therein are held in confidence and in
  trust and will not be used, copied, reproduced in whole or in part,
  nor its contents revealed in any manner to others without the express
  written permission of Qualcomm Technologies Incorporated.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/wms/inc_deprecated/wmsmsg.h#1 $
  $DateTime: 2016/12/13 08:00:05 $
  $Author: mplcsds1 $
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

#include "mmcp_variation.h"

#if defined(FEATURE_CDSMS) || defined(FEATURE_GWSMS)

#include "wms.h"
#include "wmsi.h"
#include "wmsll.h"
#include "wmscl.h"

#ifdef FEATURE_GSTK
#ifdef FEATURE_GWSMS_MO_CONTROL
#include "gstk_exp.h"
#endif /* FEATURE_GWSMS_MO_CONTROL */
#endif /* FEATURE_GSTK */

#ifdef FEATURE_GWSMS_BROADCAST
#include "log.h"
#endif /*FEATURE_GWSMS_BROADCAST*/

/* forward declaration
*/
typedef struct wms_cfg_struct  cfg_s_type;

#ifdef FEATURE_GWSMS
#include "nas_exp.h"
typedef connection_id_T wms_cm_conn_id_type;
#endif /* FEATURE_GWSMS */

#ifdef FEATURE_CDSMS

/* Parameter lengths
*/
enum{ WMS_OTA_ADDRESS_MAX      = ( WMS_ADDRESS_MAX + 5 ) };
enum{ WMS_OTA_SUBADDRESS_MAX   = ( WMS_SUBADDRESS_MAX + 4 ) };
enum{ WMS_OTA_CAUSE_CODES_MAX  = 4 };

enum{ WMS_OTA_TL_ACK_MAX       = ( 1 + WMS_OTA_ADDRESS_MAX +
                                         WMS_OTA_SUBADDRESS_MAX +
                                         WMS_OTA_CAUSE_CODES_MAX ) };

enum{ WMS_OTA_PARM_MAX        =  WMS_MAX_LEN };

enum{ WMS_EP_MSG_LEN_BITS     =  5 };
enum{ WMS_EP_MST_BITS         =  8 };
enum{ WMS_EP_CB_DIGITS        =  36 };

enum{ WMS_A_CLI_LEN           = 40 };
enum{ WMS_A_VOICE_MAIL_LEN    = 30 };
enum{ WMS_A_SHORT_MSG_LEN     = 30 };

enum{ WMS_MWI_LEN             = 1 };

/* SMS Transport Layer sequence number is 6 bits (0-63).
*/
enum{ WMS_TL_SEQ_NUM_MAX        = 64 };


/* Analog AWI SMS flag
*/
typedef enum
{
  WMS_AWI_INTERMEDIATE = 0,
  WMS_AWI_FINAL        = 1,
  WMS_AWI_FIRST        = 2,
  WMS_AWI_UNSEGMENTED  = 3

} wms_awi_flag_e_type;



/* Struct for MC task to send sms
*/
typedef struct
{
  wms_OTA_message_type     ota;      /* the raw SMS data to be sent */
  wms_address_s_type       address;  /* SMS destination addr for auth. */

} wms_cdma_sms_type;

#endif /* FEATURE_CDSMS */

/* Message record state
*/
/* NOTE:
   For MO msg: IDLE->QUEUED->LAERY2_IN_PROGRESS->WAIT_FOR_ACK->IDLE
   For MT msg: IDLE->WAIT_FOR_ACK->QUEUED->LAYER2_IN_PROGRESS->IDLE
*/
typedef enum
{
  WMS_MSG_STATE_IDLE   = 0,
  WMS_MSG_STATE_QUEUED,
  WMS_MSG_STATE_LAYER2_IN_PROGRESS,
  WMS_MSG_STATE_WAIT_FOR_ACK,
  WMS_MSG_STATE_ACK_IN_PROGRESS
} wms_msg_state_e_type;

#ifdef FEATURE_CDSMS

#define WMS_RUIM_SMS_MAX   255

typedef PACKED struct
{
  uint8                     status;
  wms_OTA_message_type      ota;
} wms_msg_ruim_sms_s_type;

typedef struct {
  wms_msg_state_e_type           state;
  wms_client_ch_pref_e_type      ch_pref;
  sint31                         retry_time_left;
  sint31                         retry_wait_time;
} cdma_mo_next_retry_rec_s_type;

#endif /* FEATURE_CDSMS */

#ifdef FEATURE_CDSMS
/* Duplicate Detection info
*/
typedef struct
{
  wms_message_index_type          index;
  wms_memory_store_e_type         mem_store;
  /* Mask for tl parameters */
  uint16                          tl_mask;
  /* Mask for client bd optional parameters */
  uint32                          cl_mask;
  /* Parameters to compare for uniqueness  IS_637A: 4.3.1.6 */
  wms_message_id_s_type           msg_id;
  wms_address_s_type              address;
  wms_subaddress_s_type           sub_address;
  wms_timestamp_s_type            mc_time;
} wms_msg_dup_detect_info_s_type;
#endif /* FEATURE_CDSMS */

#ifdef FEATURE_GWSMS_BROADCAST
typedef struct
{
  log_hdr_type header;

  uint8 cb_data[88];

} wms_msg_gw_cb_receive_log_type;
#endif /* FEATURE_GWSMS_BROADCAST */

/* Struct stored transport layer info
*/
typedef struct
{
  boolean                                  is_registered;    // is transport registered to wms
  boolean                                  is_mn_registered; // is transport registered to mn
  wms_msg_transport_cap_type               cap;              // transport layer's capability
  wms_msg_transport_mo_sms_cb_type         mo_sms_cb;        // CB provided by transport layer to send MO SMS
  wms_msg_transport_nw_reg_status_e_type   nw_reg_status;    // transport layer network reg status
  wms_msg_transport_id_type                transport_id;     // transport id maintained by WMS
  wms_msg_transport_type_e_type            transport_type;   // Transport type registered
  sys_modem_as_id_e_type                   as_id;            // AS_ID on which IMS is registered
} wms_msg_transport_s_type;


void wms_msg_offline(
  void
);

void wms_msg_init(
  void
);

void wms_msg_process_cmd(
  wms_cmd_type                       *cmd_ptr
);

void wms_msg_cleanup(
  void
);

/*===========================================================================
FUNCTION wms_msg_send_smsc_address_change_ind

DESCRIPTION
  Send request to QMI_WMS about SMSC address change

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void wms_msg_send_smsc_address_change_ind(
  wms_prov_app_info_s_type   *appInfo_ptr
);

#ifdef FEATURE_GWSMS

wms_message_number_type  wms_msg_map_status_rpt(
  wms_message_number_type             message_ref,
  wms_memory_store_e_type             mem_store
);

void wms_msg_update_sts_mapping_table(
  wms_memory_store_e_type             mem_store,
  wms_message_number_type             index,
  wms_message_number_type             message_ref
);

/*===========================================================================
FUNCTION wms_msg_notify_SMMA

DESCRIPTION
  Send request to lower layers in order to notify the network that memory
  is available to receive messages.

DEPENDENCIES
  none

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void wms_msg_notify_SMMA(
  wms_prov_app_info_s_type           *appInfo_ptr,
  boolean                             retry
);


/*===========================================================================
FUNCTION wms_msg_total_memory_full

DESCRIPTION
  Function to check if all the Memory Stores are full.

DEPENDENCIES
  none

RETURN VALUE
  TRUE / FALSE

SIDE EFFECTS
  None.
===========================================================================*/
boolean wms_msg_total_memory_full(
  wms_prov_app_info_s_type           *appInfo_ptr
);

#ifdef FEATURE_GSTK
#ifdef FEATURE_GWSMS_MO_CONTROL
/*===========================================================================
FUNCTION wms_msg_send_gstk_evt

DESCRIPTION
  Used to post GSTK Event to WMS Command Queue for Processing.

DEPENDENCIES
  none

RETURN VALUE
  wms_status_e_type

SIDE EFFECTS
  None.
===========================================================================*/
wms_status_e_type wms_msg_send_gstk_evt(
  gstk_cmd_from_card_type            *gstk_ptr
);

#endif /* FEATURE_GWSMS_MO_CONTROL */
#endif /* FEATURE_GSTK */

boolean wms_msg_gw_message_id_is_valid(
  wms_message_number_type             message_id
);

#endif /* FEATURE_GWSMS */

#ifdef FEATURE_CDSMS
/*=========================================================================
FUNCTION wms_msg_cdma_mt_msg

DESCRIPTION
  This function receives an SMS message and sends a command to the SMS task
  for processing.
  For Analog AWI SMS messages, segments are assembled into a complete
  message before being sent to the SMS task and an ack order is returned
  for intermediate segments.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Add a command to the command queue. AWI SMS info is updated for AWI SMS
  segments.

=========================================================================*/
extern void  wms_msg_cdma_mt_msg(
  wms_format_e_type                   format,
  uint16                              sms_len, /* length of sms message in bytes */
  uint8                              *sms_ptr, /* Pointer to received (mobile-terminated) SMS message */
  wms_bearer_e_type                   bearer,
  boolean                             ack_pending,
  sys_modem_as_id_e_type              as_id
);

/*=========================================================================
FUNCTION wms_msg_cdma_mt_ack_timer_proc

DESCRIPTION
  This function processed the MT SMS Ack Timer.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Sends a Negative Ack to the Network

=========================================================================*/
void wms_msg_cdma_mt_ack_timer_proc(
  void
);

/*=========================================================================
FUNCTION wms_msg_awi_ack_timer_proc

DESCRIPTION
  This function processes the AWI Ack Timer.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Sends a Negative Ack to the Network

=========================================================================*/
void wms_msg_awi_ack_timer_proc(
  void
);

/*=========================================================================
FUNCTION wms_msg_mc_mo_status_proc

DESCRIPTION
  This function processed the MO SMS Status WMS Command.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

=========================================================================*/
void wms_msg_mc_mo_status_proc(
  wms_cmd_type                       *cmd_ptr
);

/*=========================================================================
FUNCTION wms_msg_mc_mo_status

DESCRIPTION
  This function processed the MO SMS Status recieved from MC.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Posts a command to WMS Command Queue

=========================================================================*/
void wms_msg_mc_mo_status(
  wms_error_class_e_type              error_class,
  wms_status_e_type                   status,
  wms_bearer_e_type                   bearer
);


void wms_msg_send_scpt_result(
  wms_cmd_type                       *cmd_ptr,
  wms_tl_message_type                *tl_ptr
);

#endif /* FEATURE_CDSMS */

/*=========================================================================
FUNCTION
  wms_msg_evt_mo_sms_status

DESCRIPTION
  Event Notification function for MO SMS Status to DIAG.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

=========================================================================*/
void wms_msg_evt_mo_sms_status(
  wms_msg_event_info_s_type          *msg_evt_ptr,
  wms_client_msg_hdr_s_type          *msg_hdr_ptr
);

/*=========================================================================
FUNCTION
  wms_msg_evt_mo_sms_retry_attempt

DESCRIPTION
  Event Notification function for MO SMS Retry Attempt to DIAG.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

=========================================================================*/
void wms_msg_evt_mo_sms_retry_attempt(
  uint32                              transaction_id,
  uint32                              record_id,
  uint8                               prev_status_report,
  uint8                               prev_cause_value,
  uint32                              retry_time_left
);

wms_transaction_id_type wms_msg_get_next_tid(
  void
);

boolean wms_msg_check_in_use_records(
  sys_modem_as_id_e_type              as_id
);

#ifdef FEATURE_CDSMS
boolean wms_msg_cdma_check_dups(
  const wms_tl_message_type          *new_tl_msg
);


/*=========================================================================
FUNCTION wms_msg_cdma_mt_busy

DESCRIPTION
  Check whether CDMA MT records are in use or not.

DEPENDENCIES
  None

RETURN VALUE
  TRUE: busy
  FALSE: not busy

SIDE EFFECTS
  None

=========================================================================*/
boolean wms_msg_cdma_mt_busy(
  void
);

/*=========================================================================
FUNCTION wms_msg_cdma_mo_busy

DESCRIPTION
  Check whether CDMA MO records are in use or not.

DEPENDENCIES
  None

RETURN VALUE
  TRUE: busy
  FALSE: not busy

SIDE EFFECTS
  None

=========================================================================*/
boolean wms_msg_cdma_mo_busy
(
  cdma_mo_next_retry_rec_s_type *next_retry_rec_ptr
);


/*=========================================================================
FUNCTION wms_msg_auto_dc_send

DESCRIPTION
  Automatically send large messages when DC is connected.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

=========================================================================*/
void wms_msg_auto_dc_send(
  void
);

/*=========================================================================
FUNCTION wms_msg_auto_send_ach

DESCRIPTION
  Automatically send the first ACH messages in the list when DC disconnected.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

=========================================================================*/
void wms_msg_auto_send_ach(
  void
);

/*=========================================================================
FUNCTION wms_msg_stop_dc_resend

DESCRIPTION
  Stop resending large messages over DC when it cannot be established.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

=========================================================================*/
void wms_msg_stop_dc_resend(
  void
);

boolean wms_msg_cdma_message_id_is_valid(
  wms_message_number_type             message_id
);

#endif /* FEATURE_CDSMS */


#ifdef FEATURE_GWSMS_BROADCAST

void wms_msg_gw_cb_data_ind_proc(
  sys_modem_as_id_e_type              as_id,
  uint8                              *puPage,
  int                                 nPageLen,
  boolean                             message_from_lte
);

void wms_msg_gw_cb_duplicate_ind_proc(
  cb_duplication_detection_ind_type  *gw_cb_duplicate_ptr
);

#if defined(FEATURE_UMTS_BMC) && defined(FEATURE_WCDMA_ETWS)
void wms_msg_etws_prim_notification_ind_proc(
  cb_etws_prim_notification_ind_type *cb_etws_prim_ind_ptr
);

void wms_msg_etws_sec_notification_ind_proc(
  cb_etws_sec_notification_ind_type  *cb_etws_sec_ind_ptr
);
#endif /* defined(FEATURE_UMTS_BMC) && defined(FEATURE_WCDMA_ETWS) */
#endif /* FEATURE_GWSMS_BROADCAST */

/*=========================================================================
FUNCTION wms_set_retry_timer

DESCRIPTION
  This function Sets the Global Retry Timer to tick every second.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

=========================================================================*/
void wms_set_retry_timer(
  void
);

/*=========================================================================
FUNCTION wms_msg_process_retry_sig

DESCRIPTION
  This function processed the Retry Signal

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

=========================================================================*/
void wms_msg_process_retry_sig(
  void
);

wms_msg_transport_s_type * wms_msg_choose_registered_transport(
  sys_modem_as_id_e_type as_id
);

uint32 wms_msg_get_pending_gw_mo_sms_count(
 sys_modem_as_id_e_type               as_id
);

uint32 wms_msg_get_pending_1x_mo_sms_count(
  void
);

#ifdef FEATURE_SMS_PWS_OVER_LTE

void wms_msg_lte_etws_prim_proc(
  lte_rrc_etws_prim_ind_s            *lte_etws_prim_ptr
);

void wms_msg_lte_etws_sec_proc(
  lte_rrc_etws_sec_ind_s             *lte_etws_sec_ptr
);

void wms_msg_lte_cmas_proc(
  lte_rrc_cmas_ind_s                 *lte_cmas_ptr
);

void wms_msg_lte_srv_ind_proc(
  lte_rrc_service_ind_s              *lte_srv_ind_ptr
);

#endif /* FEATURE_SMS_PWS_OVER_LTE */

wms_msg_transport_s_type* wms_msg_transport_ptr
(
  wms_msg_transport_id_type transport_id
);

wms_status_e_type  wms_msg_transport_ms_reg_mo_sms_cb
(
  wms_msg_transport_id_type         transport_id,
  sys_modem_as_id_e_type            as_id,
  wms_msg_transport_mo_sms_cb_type  mo_sms_cb
);

wms_status_e_type  wms_msg_transport_ms_cap_update
(
  wms_msg_transport_id_type         transport_id,
  sys_modem_as_id_e_type            as_id,
  wms_msg_transport_cap_type        transport_cap
);

wms_status_e_type  wms_msg_transport_ms_nw_reg_status_update
(
  wms_msg_transport_id_type               transport_id,
  sys_modem_as_id_e_type                  as_id,
  wms_msg_transport_nw_reg_status_e_type  transport_nw_reg_status
);

wms_status_e_type  wms_msg_transport_ms_rpt_ind
(
  wms_msg_transport_rpt_ind_s_type   *rpt_ind_ptr,
  sys_modem_as_id_e_type              as_id
);

wms_status_e_type  wms_msg_transport_ms_rpt_mwi_ind
(
  wms_msg_transport_rpt_mwi_ind_s_type   *rpt_mwi_ind_ptr,
  sys_modem_as_id_e_type                  as_id
);

#endif /* FEATURE_CDSMS || FEATURE_GWSMS */

#endif /* WMSMSG_H */
