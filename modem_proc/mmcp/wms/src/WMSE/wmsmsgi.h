#ifndef WMSMSGI_H
#define WMSMSGI_H
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
           W I R E L E S S    M E S S A G I N G   S E R V I C E S
           wmsmsgi.h -- Message Handling Group Definitions

  The WMS module which implements the User API for SMS. This source file
  defines the internal message group definitions.

  -------------

    Copyright (c) 2007-2013 QUALCOMM Technologies Incorporated.
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

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/wms/src/WMSE/wmsmsgi.h#1 $
  $DateTime: 2016/12/13 08:00:05 $
  $Author: mplcsds1 $
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================
========================  INCLUDE FILES =====================================
===========================================================================*/
#include "mmcp_variation.h"

#if defined(FEATURE_CDSMS) || defined(FEATURE_GWSMS)
#include "wmsmsg.h"
#include "mn_uasms.h"
#include "wmscl.h"

#define WMS_MAX_MO_MESSAGES     20
#define WMS_MAX_MT_MESSAGES     20

#ifdef FEATURE_CDSMS
/* Message record
*/
struct wms_msg_cdma_message_record_s_type
{
  wms_client_type_e_type         client_id;
  wms_msg_state_e_type           state;

  uint8                          seq_num;

  boolean                        send_status_to_clients;

  void                          *user_data;
  void                          *mt_rec_user_data;

  wms_address_s_type             address;
  wms_subaddress_s_type          subaddress;

  wms_send_mode_e_type           send_mode;
  wms_client_msg_hdr_s_type      msg_hdr;

  /* Retry Time Left is the Time left for the Current SMS Delivery */
  sint31                         retry_time_left;

  /* Retry Wait Time is the Time to Wait before SMS retransmission */
  sint31                         retry_wait_time;

  /* Use prev_report if the retry period expires when processing the
  ** the submit report event.
  */
  wms_submit_report_info_s_type  prev_report;

  /* Fields for Pending MO SMS Message */
  wms_OTA_message_type          *ota;

  /* Fields for Pending MO SMS Ack*/
  wms_error_class_e_type         error_class;
  wms_cdma_tl_status_e_type      tl_status;

  /* each message has its own timer to wake up the SMS task
  */
  rex_timer_type                 timer;

  uint8                          retry_count;

  wms_prov_app_info_s_type      *appInfo_ptr;

  /* State required for wms_msg_ms_resend() implementation: resend_msg, client_msg_id */
  boolean                        resend_msg;
  wms_message_number_type        client_msg_id;
  /* flag indicates whether the received MT SMS has ESN parameter or not */
  boolean                        esn_present;
  wms_msg_transport_type_e_type  transport_type;

  /* CDMA MO SMS channel preference */
  wms_client_ch_pref_e_type      ch_pref;

  /* CDMA DC service option */
  wms_dc_so_e_type               dc_so;
  boolean                        follow_on_dc; 

  /* CDMA MO SMS records linked list */
  /* Record link which points to the previous record. */
  struct wms_msg_cdma_message_record_s_type *prev;
  /* Record link which points to the next record. */
  struct wms_msg_cdma_message_record_s_type *next;
};

typedef struct wms_msg_cdma_message_record_s_type   wms_msg_cdma_message_record_type;

/* linked list use for traversing the record link during SMS retries */
typedef struct {
  wms_msg_cdma_message_record_type     *head_ptr;
  wms_msg_cdma_message_record_type     *tail_ptr;
  uint8                                 count;
} cdma_mo_rec_llist_type;
#endif /* FEATURE_CDSMS */

#ifdef FEATURE_GWSMS
typedef struct {
   wms_client_type_e_type              client_id;
   void                               *user_data;
   wms_send_mode_e_type                send_mode;
   uint16                              smi;
   wms_client_msg_hdr_s_type           msg_hdr;
#ifdef FEATURE_GWSMS_MO_CONTROL
   boolean                             mo_control_pending;
   wms_client_message_s_type           mo_control_cl_msg;
#endif /* FEATURE_GWSMS_MO_CONTROL */
   /* Retry Time Left is the Time left for the Current SMS Delivery */
   sint31                              retry_time_left;

   /* Retry Wait Time is the Time to Wait before SMS retransmission */
   sint31                              retry_wait_time;

   wms_gw_message_s_type              *sent_gw_msg;
   wms_msg_state_e_type                state;

   /* Use prev_report if the retry period expires when processing the
   ** the submit report event.
   */
   wms_submit_report_info_s_type       prev_report;
   uint8                               retry_count;

   sys_modem_as_id_e_type              as_id;
   wms_prov_app_info_s_type           *appInfo_ptr;

   /* State required for wms_msg_ms_resend() implementation: resend_msg, client_msg_id */
   boolean                             resend_msg;
   wms_message_number_type             client_msg_id;
   wms_msg_transport_type_e_type       transport_type;

} wms_msg_gw_mo_record_type;

typedef struct {
   wms_transaction_id_type             tid;
   wms_cm_conn_id_type                 smi;
   wms_client_id_type                  client_id;
   wms_client_msg_hdr_s_type           msg_hdr;
   wms_client_type_e_type              ack_client_id;
   void                               *user_data;
   sys_modem_as_id_e_type              as_id;
   wms_msg_state_e_type                state;
   wms_prov_app_info_s_type           *appInfo_ptr;
   wms_msg_transport_type_e_type       transport_type;
} wms_msg_gw_mt_record_type;
#endif /* FEATURE_GWSMS */


typedef struct
{

#ifdef FEATURE_CDSMS
  /* Information of incoming and outgoing CDMA mode SMS messages that need
     transport layer ack.
  */
  wms_msg_cdma_message_record_type     *cdma_mt_rec[ WMS_MAX_MT_MESSAGES ];

  cdma_mo_rec_llist_type           cdma_mo_rec; /* linked list for mo record links */

  uint8                            tl_seq_num;
  uint8                            non_tl_seq_num;
  wms_transaction_id_type          tid;

  struct
  {
    boolean                            is_valid;
    boolean                            is_ack;   /* whether the msg is a TL Ack */
    wms_msg_cdma_message_record_type  *rec_ptr;  /* the msg record ptr */
    wms_transaction_id_type            tid;
    wms_error_class_e_type             error_class;
    wms_cdma_tl_status_e_type          tl_status;
    wms_bearer_e_type                  bearer;
  } mc_msg;

#ifdef FEATURE_ACP
  /* AWI SMS info data
  */
  struct wms_awi_info_struct
  {
    uint8                  seq_no;
      /* Current sequence number */
    uint8                  cnt;
      /* Current byte count in awi_data */
    uint8                  data[ WMS_MAX_LEN ];
      /* Contiguous AWI INFO_DATA field */
    wms_awi_flag_e_type    bf;
      /* Begin/Final values */

    boolean                is_valid;         /* whether an AWI SMS is active */
    void                  *user_data;        /* transaction Id */
    rex_timer_type         timer;            /* MT ack timer */

  } awi_info;
#endif /* FEATURE_ACP */
#endif /* FEATURE_CDSMS */


#ifdef FEATURE_GWSMS
/*TP-MTI*/
#define WMS_MESSAGE_TYPE_DELIVER        0
#define WMS_MESSAGE_TYPE_DELIVER_REPORT 0
#define WMS_MESSAGE_TYPE_SUBMIT         1
#define WMS_MESSAGE_TYPE_SUBMIT_REPORT  1
#define WMS_MESSAGE_TYPE_STATUS_REPORT  2
#define WMS_MESSAGE_TYPE_COMMAND        2
#define WMS_MESSAGE_TYPE_RESERVED       3


  /* pending MO message information */
  wms_msg_gw_mo_record_type *gw_mo_rec[WMS_MAX_MO_MESSAGES];

  wms_msg_gw_mt_record_type *gw_mt_rec[WMS_MAX_MT_MESSAGES];
#endif /* FEATURE_GWSMS */

  uint32                                mo_retry_period;
  uint32                                mo_retry_interval;
  uint32                                timerf_value;

} wms_msg_s_type;

wms_msg_s_type * wms_msg_s_ptr(
  void
);

#ifdef FEATURE_GWSMS

/* Each time only one SMMA request is pending
*/
#define WMS_MSG_SMMA_REQUEST_ID   0xFFFF
#define WMS_MSG_DUMMY_SMI_ID      0xFFFE

/* 0x10xx indicates error reports for MT messages
*/
#define WMS_MSG_MT_ERROR_ID_MASK    0x1000
/* 0x20xx indicates Ack Reports
*/
#define WMS_MSG_ACK_REPORT_ID_MASK  0x2000

/* Define the list of SMS Domains
*/
typedef enum
{
  WMS_DOMAIN_CS,
  WMS_DOMAIN_PS,
  WMS_DOMAIN_CS_PS,  /* Both CS and PS Domain */
  WMS_DOMAIN_MAX,
  WMS_DOMAIN_MAX32 = 0x1FFFFFFF
} wms_domain_e_type;

void wms_msg_sm_data_ind_proc(
  sys_modem_as_id_e_type    as_id,
  wms_cm_conn_id_type       ubConnectionId,
  wms_transaction_id_type   transaction_id,
  cm_called_party_bcd_no_T *pSCA, /* originating service center address */
  uint8                    *pubTPDU,
  int                       nTPDULen,
  nas_msg_transport_type_e_type nas_transport_type
);

void wms_msg_sm_report_ind_proc(
  sys_modem_as_id_e_type    as_id,
  uint16                    smi,
  wms_report_status_e_type  error_status,
  uint8                     cause_value,
  uint8                    *puData,
  uint32                    uLen,
  boolean                   diagnostics_present,
  uint8                     diagnostics,
  boolean                   is_permanent_error,
  boolean                   retry_header_present,
  uint32                    retry_header_value,
  wms_domain_e_type         domain,
  uint16                    ims_cause_code
);

void wms_msg_mt_error_ind_proc(
  sys_modem_as_id_e_type    as_id,
  wms_cm_conn_id_type       smi_mt,
  wms_report_status_e_type  error_status,
  uint8                     cause_value
);

void wms_msg_ack_report_ind_proc(
  sys_modem_as_id_e_type    as_id,
  wms_cm_conn_id_type       smi_mt,
  wms_report_status_e_type  error_status
);

void wms_msg_SMMA_CONF_ind_proc(
  sys_modem_as_id_e_type    as_id,
  wms_report_status_e_type  error_status,
  uint8                     cause_value
);

wms_cmd_err_e_type wms_msg_mo_sms_fdn_control_func(
  wms_prov_app_info_s_type *appInfo_ptr,
  wms_message_mode_e_type   msg_mode,
  wms_address_s_type       *address
);

boolean wms_msg_rp_error_permanent(
  wms_report_status_e_type  status,
  uint8                     cause
);

boolean wms_msg_wait_for_ack_report_ind(
  sys_modem_as_id_e_type    as_id
);

boolean wms_msg_check_pending_mo_messages(
  sys_modem_as_id_e_type    as_id
);

#endif /* FEATURE_GWSMS */

#ifdef FEATURE_CDSMS
void wms_msg_mt_CDMA_tl_ack_proc(
  wms_prov_app_info_s_type         *appInfo_ptr,
  wms_msg_cdma_message_record_type *rec_ptr,
  wms_cause_code_type              *cause_code_ptr
);

void wms_msg_mt_CDMA_proc(
  wms_prov_app_info_s_type         *appInfo_ptr,
  wms_cmd_mc_mt_msg_type           *mt_msg_cmd_ptr,
  wms_routing_s_type               *routing_ptr
);
#endif /* FEATURE_CDSMS */

/*
   STRUCTURE: wms_gstk_user_data_s_type
   Holds information from a pp download deliver message which is
   required to ack the message once the envelope has been sent
   and processed by gstk.
   Sent as user_data via gstk_slot_send_envelope_sms_pp_dl_command().
   Returned via the resulting callback function provided in the gstk request.
*/
typedef  struct
{
  wms_message_mode_e_type       msg_mode;
  sys_modem_as_id_e_type        as_id;
#ifdef FEATURE_GWSMS_PP_DOWNLOAD
  wms_transaction_id_type       pp_download_tid;
  wms_pid_e_type                pp_download_pid;
  wms_gw_dcs_s_type             pp_download_dcs;
#endif /* FEATURE_GWSMS_PP_DOWNLOAD */
#if defined (FEATURE_CDSMS) && ((defined(FEATURE_UIM_TOOLKIT_UTK)||defined(FEATURE_CCAT)) || defined(FEATURE_GSTK))
  wms_address_s_type            pp_download_address;
  wms_subaddress_s_type         pp_download_subaddress;
  wms_teleservice_e_type        pp_download_teleservice;
  wms_message_number_type       pp_download_msg_id;
#endif /* defined(FEATURE_CDSMS) && ((defined(FEATURE_UIM_TOOLKIT_UTK)||defined(FEATURE_CCAT)) || defined(FEATURE_GSTK)) */
} wms_gstk_user_data_s_type;

#ifdef FEATURE_GWSMS_DEPERSO
/*
  STRUCTURE: wms_simlock_ota_user_data_s_type;
  Holds information from a received MT deperso message which is
  required to ack the message once the deperso request has been
  processed by mmgsdi.
  Sent as user_data via simlock_ota_unlock().
  Returned via the resulting callback function provided in the
  simlock unlock request.
*/
typedef struct
{
  wms_transaction_id_type  tid;
  wms_pid_e_type           pid;
  wms_gw_dcs_s_type        dcs;
  mmgsdi_session_id_type   session_id;
} wms_simlock_ota_user_data_s_type;
#endif /* FEATURE_GWSMS_DEPERSO */

#endif /* FEATURE_CDSMS || FEATURE_GWSMS */

boolean wms_is_cdma_admin_msg(
  const wms_OTA_message_type   *OTA_ptr
);

boolean wms_is_gw_admin_msg(
  wms_raw_ts_data_s_type       *raw_ts_ptr
);

void wms_msg_send_transport_reg_event(
  wms_msg_transport_s_type  *transport_ptr,
  wms_client_s_type         *client_ptr
);

#endif /* WMSMSGI_H */


