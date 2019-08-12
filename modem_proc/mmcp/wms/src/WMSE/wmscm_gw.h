#ifndef WMSCM_GW_H
#define WMSCM_GW_H
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
           W I R E L E S S    M E S S A G I N G   S E R V I C E S
           wmscm_gw.c -- GW Interfaces with CM and NAS Definitions

  The WMS module which implements the User API for SMS. This source file
  defines the transport interfaces with Call Manager and NAS layers.

  -------------

    Copyright (c) 2008,2010-2012 QUALCOMM Technologies Incorporated.
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

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/wms/src/WMSE/wmscm_gw.h#1 $
  $DateTime: 2016/12/13 08:00:05 $
  $Author: mplcsds1 $
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

#include "mmcp_variation.h"
#if defined(FEATURE_GWSMS)

#include "wms.h"
#include "nv_items.h"
#include "wmsi.h"
#include "wmsmsgi.h"
#include "CMCNas_MN.h"
//==============================================================================
//==============================================================================
wms_cmd_err_e_type wms_cm_gw_send_rl_data(wms_msg_gw_mo_record_type *mo_rec_ptr,
                                          wms_address_s_type *sc_address_ptr,
                                          uint8  message_number,
                                          wms_raw_ts_data_s_type  *raw_ts_ptr);

//==============================================================================
//==============================================================================
wms_cmd_err_e_type wms_cm_gw_send_rl_report(sys_modem_as_id_e_type  as_id,
                                            wms_cm_conn_id_type connection_id,
                                            boolean is_success,      /* */
                                            uint8 rp_cause_value,  /* */
                                            uint8* tpdu_ptr,
                                            uint8 tpdu_len);

//==============================================================================
//==============================================================================
wms_cmd_err_e_type wms_cm_gw_send_rl_memory_available(
                              wms_msg_gw_mo_record_type *mo_rec_ptr,
                              boolean is_abort,
                              wms_msg_transport_type_e_type transport_type);

#ifdef FEATURE_GWSMS_BROADCAST
//==============================================================================
//==============================================================================
wms_cmd_err_e_type wms_cm_gw_cb_disable(
  sys_modem_as_id_e_type as_id
);

//==============================================================================
//==============================================================================
wms_cmd_err_e_type wms_cm_gw_cb_search_list(
  sys_modem_as_id_e_type as_id,
  wms_gw_cb_srv_id_type* list1_ptr,
  uint16 len1, /* number of nv_sms_gw_cb_service_table_type */
  nv_sms_gw_cb_service_table_type* list2_ptr,
  uint16 len2,  /* number of nv_sms_gw_cb_service_table_type */
  boolean reading_advised,
  boolean reading_optional,
  uint32* num_selected_ptr /* OUT */
);

//==============================================================================
//==============================================================================
wms_cmd_err_e_type wms_cm_gw_cb_delete_message(
   sys_modem_as_id_e_type as_id,
   uint16 message_id, /* message id */
   uint16 message_code, /* message code */
   uint8 page_number /* page number */);

//==============================================================================
//==============================================================================
wms_cmd_err_e_type wms_cm_gw_cb_delete_all_messages(
  sys_modem_as_id_e_type as_id
);

void wms_cm_gw_cb_send_standby_status(
   sys_modem_dual_standby_pref_e_type standby_pref,
   sys_modem_as_id_e_type             active_subs);

#endif /* FEATURE_GWSMS_BROADCAST */

//==============================================================================
//==============================================================================
wms_cmd_err_e_type wms_cm_gw_bearer_selection(
   sys_modem_as_id_e_type    as_id,
   wms_gw_domain_pref_e_type gw_domain_pref);

//==============================================================================
//==============================================================================
wms_cmd_err_e_type wms_cm_gw_rl_link_control(
   sys_modem_as_id_e_type     as_id,
   wms_cfg_link_control_mode_e_type link_control_mode,
   uint8 link_control_timer);

wms_cmd_err_e_type wms_cm_gw_send_sms_status_to_mm(
   wms_mm_sms_call_info_T     mm_sms_status);

wms_cmd_err_e_type wms_cm_gw_rl_update_timer_f(
  sys_modem_as_id_e_type,
  wms_msg_transport_f_timer_type);


//==============================================================================
//==============================================================================
boolean wms_cm_gw_is_idle(void);

//==============================================================================
// service one of the queued messages, return TRUE if more pending
// or FALSE if no more
//==============================================================================
boolean wms_cm_gw_dispatch_one(void);

//==============================================================================
//==============================================================================
wms_cmd_err_e_type wms_cm_gw_init(void);

gs_status_T wmscmi_send_routine(CMCNas_Rpt* msg_p);

#ifdef TEST_FRAMEWORK
#error code not present
#endif

#endif /* FEATURE_GWSMS */

#endif /* WMSCM_GW_H */
