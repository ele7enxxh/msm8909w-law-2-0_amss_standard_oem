/*===========================================================================
                      DS_ANDSF_MSG.H

DESCRIPTION
 MSG header file for ANDSF.

EXTERNALIZED FUNCTIONS

 Copyright (c) 2014 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/
/*===========================================================================

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  when        who                            what, where, why
--------    -------                ----------------------------------------
06/09/14    Youjunf                          First version of file
===========================================================================*/
#ifndef DS_ANDSF_MSG_H
#define DS_ANDSF_MSG_H

/*===========================================================================
                           INCLUDE FILES
===========================================================================*/
#include "datamodem_variation.h"
#ifdef FEATURE_DATA_ANDSF

#include "ds_andsf_location_update.h"
#include "ds_andsf_wqe_prof_mgr.h"
#include "ps_sys.h"
#include "ps_sys_ioctl.h"
#include "ds_dsd_ext_i.h"
#include "cm.h"
#include "ps_sys_event.h"


#ifdef __cplusplus
extern "C"
{
#endif

#include "ds_dsd_ext_i.h"

#ifdef __cplusplus
}
#endif

/*===========================================================================
                   EXTERNAL DEFINITIONS AND TYPES
===========================================================================*/
typedef enum
{
  DS_ANDSF_MSG_ID_MIN,

  DS_ANDSF_POLICY_PULLED_IND = DS_ANDSF_MSG_ID_MIN,
  DS_ANDSF_POLICY_SMS_PUSH_IND,
  DS_ANDSF_POLICY_AP_QUERY_IND,
  DS_ANDSF_POLICY_REFRESH_TIMER_EXPIRE,
  DS_ANDSF_POLICY_RETRY_TIMER_EXPIRE,

  DS_ANDSF_HTTP_READY_IND,

  DS_ANDSF_FTP_FILE_UPDATED_IND,
  DS_ANDSF_FTP_SERVER_READY,
  DS_ANDSF_FTP_SERVER_CLOSED,
  DS_ANDSF_FTP_COMPLETE_TIMER_EXPIRE,
  DS_ANDSF_FTP_SERVER_IDLE_IND,

  DS_ANDSF_WWAN_CM_SS_CB_EVENT,
  DS_ANDSF_WLAN_LOCATION_UPDATED_IND,

  DS_ANDSF_RAN_MEAS_DSD_EVENT,
  DS_ANDSF_RAN_MEAS_LTE_SAMPLING_TIMER_EXPIRE,
  DS_ANDSF_RAN_MEAS_1X_SAMPLING_TIMER_EXPIRE,
  DS_ANDSF_RAN_MEAS_1X_ACQ_LOST_HYST_TIMER_EXPIRE,

  DS_ANDSF_WQE_PROF_WLAN_CHG_EVENT,
  DS_ANDSF_WQE_PROF_SET_PROF_TYPE_IOCTL,
  DS_ANDSF_WQE_PROF_SET_QUALITY_STATUS_IOCTL,

  DS_ANDSF_WIFI_CONFIGURE_STATUS_IND,
  DS_ANDSF_WIFI_MEAS_REPORT,
  DS_ANDSF_WQE_PROF_WAIT_TIMER_EXPIRE,
  DS_ANDSF_WQE_PROF_POLICY_CHANGE_REQ,
  DS_ANDSF_WQE_PROF_UPDATE_WIFI_THRESHOLD_REQ,

  DS_ANDSF_MSG_ID_MAX

}ds_andsf_msg_id_enum_type;


typedef enum
{
  DS_ANDSF_POLICY_PULL_SUCC,
  DS_ANDSF_POLICY_PULL_FAIL_NET_DOWN,
  DS_ANDSF_POLICY_PULL_FAIL_HTTP,
}ds_andsf_policy_result_enum_type;


typedef struct
{
  ds_andsf_policy_result_enum_type result;
  uint8                            *content_ptr;
  uint32                           content_len;
}ds_andsf_policy_pulled_event_type;

typedef struct
{
  boolean is_ap_query;
}ds_andsf_file_updated_event_type;


typedef struct
{
 cm_ss_event_e_type         evt;
 cm_mm_msim_ss_info_s_type  ss_info;
}ds_andsf_wwan_cm_ss_cb_event_type;


typedef ps_sys_wlan_status_type    ds_andsf_wlan_location_updated_event_type;
typedef ds_dsd_ext_event_info_type ds_andsf_dsd_event_type;
typedef ps_sys_wlan_status_type    ds_andsf_wqe_prof_wlan_chg_event_type;

typedef struct
{
  ds_andsf_msg_id_enum_type andsf_msg_id;
  union
  {
    ds_andsf_policy_pulled_event_type          policy_pulled_event;
    ds_andsf_file_updated_event_type           file_updated_event;
    ds_andsf_wlan_location_updated_event_type  wlan_location_updated_event;
    ds_andsf_wwan_cm_ss_cb_event_type          wwan_cm_ss_cb_event;
    ds_andsf_dsd_event_type                    dsd_event;
    ds_andsf_wqe_prof_wlan_chg_event_type      wlan_status_chg_event;
    ds_andsf_wqe_prof_wqe_prof_type            set_wqe_prof_type_ioctl;
    ds_andsf_wqe_prof_status_type              set_wqe_prof_status_ioctl;
    ps_sys_wlan_wifi_meas_report_config_status_type   wifi_configure_status_event;
    ps_sys_wlan_wifi_meas_report_type                 wifi_meas_report_event;
    ds_andsf_wqe_prof_wifi_thres_icotl_list_type      update_wqe_prof_threshold_ioctl;
    ds_andsf_policy_threshold_chg_type                policy_threshold_chg_event;  
  }msg_content;
}ds_andsf_msg_type;

/*===========================================================================
                    EXTERNAL FUNCTION PROTOTYPES
===========================================================================*/
#ifdef __cplusplus
extern "C"
{
#endif

void  ds_andsf_msg_init(void);

void  ds_andsf_msg_post(ds_andsf_msg_id_enum_type msg_id, void  *msg_content_ptr);

void  ds_andsf_policy_msg_hdlr(ds_andsf_msg_type *msg_ptr);
void  ds_andsf_transporter_msg_hdlr(ds_andsf_msg_type *msg_ptr);
void  ds_andsf_location_update_msg_hdlr(ds_andsf_msg_type *msg_ptr);
void  ds_andsf_ran_meas_msg_hdlr(ds_andsf_msg_type *msg_ptr);
void  ds_andsf_wwan_cm_ss_msg_hdlr(ds_andsf_msg_type *msg_ptr);
void  ds_andsf_wqe_prof_mgr_msg_hdlr(ds_andsf_msg_type *msg_ptr);

#ifdef __cplusplus
}
#endif

#endif /* FEATURE_DATA_ANDSF */
#endif /* DS_ANDSF_MSG_H */
