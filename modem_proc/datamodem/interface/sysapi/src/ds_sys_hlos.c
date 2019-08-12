/******************************************************************************
  @file    ds_sys.c
  @brief   

  DESCRIPTION
  This file defines function, variables and data structures common to all
  DS System API module

  INITIALIZATION AND SEQUENCING REQUIREMENTS
  N/A

  ---------------------------------------------------------------------------
  Copyright (C) 2011-14 Qualcomm Technologies Incorporated.
  All Rights Reserved. QUALCOMM Proprietary and Confidential.
  ---------------------------------------------------------------------------
******************************************************************************/
/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/sysapi/src/ds_sys_hlos.c#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $
 
when       who     what, where, why
--------   ---     ----------------------------------------------------------
02/06/12    bvd     Created module
===========================================================================*/

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/
#include "ds_sysi.h"
#include "qmi_wds_srvc.h"
#include "ps_sys_confi.h"
#include "ps_sys_eventi.h"
#include "ps_sys_ioctl.h"
#include "ps_svc.h"
#include "ps_system_heap.h"
#include <stringl/stringl.h>

/*===========================================================================

                           LOCAL DATA DECLARATIONS

===========================================================================*/

/*
  Variable representing qmi handle provided by qmimsglib 
*/
static int qmi_handle = -1;

/*
  Variable representing client handle provided by qmimsglib during
  initialization of qmi client
*/
int ds_sys_qmi_client_handle = -1;

/*
  Variable represnting whether modem restart is in progress or not
*/
static boolean  modem_reset_in_progress = FALSE;

extern int16 ps_sys_confi_map_qmi_err_ds ( int );

/*===========================================================================

                           EXTERNAL FUNCTIONS

===========================================================================*/
void ds_sysi_set_modem_reset_in_progress
(
  boolean is_modem_reset_in_progress
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  modem_reset_in_progress = is_modem_reset_in_progress;
} /*  ds_sysi_set_modem_reset_in_progress() */

boolean ds_sysi_is_modem_reset_in_progress
(
  void
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return modem_reset_in_progress;
} /* ds_sysi_is_modem_reset_in_progress() */

/**
  @brief  This method is a callback registered for handling qmi sys level 
          events.

  @param[in] event_id        QMI event id
  @param[in] event_info      Event info related to event
  @param[in] user_data       User data associated with event

*/
void ds_sys_qmi_sys_event_handler
(
  qmi_sys_event_type                event_id,
  const qmi_sys_event_info_type   * event_info,
  void                            * user_data
)
{
  ps_sys_eventi_cmd_type                * cmd_data_info;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  LOG_MSG_FUNCTION_ENTRY_1("ds_sys_qmi_sys_event_handler(): Event id:%d",
                           event_id);

  if (QMI_SYS_EVENT_MODEM_OUT_OF_SERVICE_IND == event_id)
  {
    ds_sysi_set_modem_reset_in_progress(TRUE);

    PS_SYSTEM_HEAP_MEM_ALLOC_ASSERT(cmd_data_info, sizeof(ps_sys_eventi_cmd_type),
                                     ps_sys_eventi_cmd_type*);

    if (NULL == cmd_data_info)
    {     
      return;
    }

    cmd_data_info->event      = PS_SYS_EVENT_MODEM_OUT_OF_SERVICE;
    cmd_data_info->tech_type  = PS_SYS_TECH_ALL;
    cmd_data_info->event_info = NULL;

    ps_send_cmd(PS_SYS_EVENT_CMD_HANDLER,(void *)cmd_data_info);
  }
  else if (QMI_SYS_EVENT_MODEM_IN_SERVICE_IND == event_id)
  {
    PS_SYSTEM_HEAP_MEM_ALLOC_ASSERT(cmd_data_info, sizeof(ps_sys_eventi_cmd_type),
                                     ps_sys_eventi_cmd_type*);

    if (NULL == cmd_data_info)
    {
      return;
    }

    cmd_data_info->event      = PS_SYS_EVENT_MODEM_IN_SERVICE;
    cmd_data_info->tech_type  = PS_SYS_TECH_ALL;
    cmd_data_info->event_info = NULL;

    ps_send_cmd(PS_SYS_EVENT_CMD_HANDLER, (void *)cmd_data_info);
  }

  return;
} /* ds_sys_qmi_sys_event_handler() */

int16 ds_sys_3gpp_ioctl_cb
(
  ps_sys_ioctl_enum_type          ioctl_name,
  void                          * arg_val_ptr,
  int16                         * ps_errno
)
{
  int                                   qmi_err_code = QMI_SERVICE_ERR_NONE;
  unsigned int                          cnt_apn;
  int16                                 ret_val;
  qmi_wds_pdn_throttle_info_type      * wds_pdn_throttle_info;
  unsigned int                          throttle_info_len = PS_SYS_MAX_APNS;
  ps_sys_ioctl_pdn_throttle_info_type * sys_pdn_throttle_info;
  ps_sys_ioctl_dun_ctrl_response_type * sys_dun_call_pref_info;
  qmi_wds_dun_call_action_type          dun_call_action_type;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  LOG_MSG_FUNCTION_ENTRY_1("ds_sys_3gpp_ioctl_cb(): ioctl %d", ioctl_name);

  switch (ioctl_name)
  { 
    case DS_SYS_IOCTL_PDN_THROTTLE_INFO :
    {
      sys_pdn_throttle_info = 
        (ps_sys_ioctl_pdn_throttle_info_type *)arg_val_ptr;

      PS_SYSTEM_HEAP_MEM_ALLOC(wds_pdn_throttle_info, 
                               sizeof(qmi_wds_pdn_throttle_info_type) * PS_SYS_MAX_APNS,
                               qmi_wds_pdn_throttle_info_type*);

      if (wds_pdn_throttle_info == NULL)
      {
        *ps_errno =  DS_ENOMEM;
        return -1;
      }          

      ret_val = qmi_wds_get_pdn_throttle_info(ds_sys_qmi_client_handle,
                                              PS_SYS_TECH_3GPP,
                                              wds_pdn_throttle_info,
                                              &throttle_info_len,
                                              &qmi_err_code);
      if ( 0 > ret_val )
      {
        LOG_MSG_ERROR_1("ds_sys_3gpp_ioctl_cb(): PDN Throttle info : "
                        "err=0x%p", qmi_err_code );
        *ps_errno = ps_sys_confi_map_qmi_err_ds(qmi_err_code);
        PS_SYSTEM_HEAP_MEM_FREE(wds_pdn_throttle_info);
        return -1;
      }

      sys_pdn_throttle_info->num_apn = throttle_info_len;

      for (cnt_apn = 0; cnt_apn < throttle_info_len; cnt_apn++)
      {
        sys_pdn_throttle_info->pdn_throttle_info[cnt_apn].is_ipv4_throttled =
          wds_pdn_throttle_info[cnt_apn].is_ipv4_throttled;
        sys_pdn_throttle_info->pdn_throttle_info[cnt_apn].is_ipv6_throttled =
          wds_pdn_throttle_info[cnt_apn].is_ipv6_throttled;
        sys_pdn_throttle_info-> \
          pdn_throttle_info[cnt_apn].remaining_ipv4_throttled_time =
            wds_pdn_throttle_info[cnt_apn].ipv4_throttled_remaining;
        sys_pdn_throttle_info-> \
          pdn_throttle_info[cnt_apn].remaining_ipv6_throttled_time =
            wds_pdn_throttle_info[cnt_apn].ipv6_throttled_remaining;

        sys_pdn_throttle_info->pdn_throttle_info[cnt_apn].apn_string_len =
          strlen(wds_pdn_throttle_info[cnt_apn].apn_name) + 1;

        /* Revisit to check*/
        memscpy
        (
          sys_pdn_throttle_info->pdn_throttle_info[cnt_apn].apn_string,
          strlen(wds_pdn_throttle_info[cnt_apn].apn_name) + 1,
          wds_pdn_throttle_info[cnt_apn].apn_name,
          strlen(wds_pdn_throttle_info[cnt_apn].apn_name) + 1
        );
      }

      PS_SYSTEM_HEAP_MEM_FREE(wds_pdn_throttle_info);
      break;
    }

    case DS_SYS_IOCTL_DUN_CTRL_RESPONSE :
    {
      sys_dun_call_pref_info = 
        (ps_sys_ioctl_dun_ctrl_response_type *)arg_val_ptr;

      if (sys_dun_call_pref_info->allow_call == TRUE)
      {
        dun_call_action_type = DUN_CALL_ACTION_ALLOW_DUN_CALL;
      }
      else
      {
        dun_call_action_type = DUN_CALL_ACTION_DENY_DUN_CALL;
      }

      ret_val = qmi_wds_control_pending_dun_call
                (
                  ds_sys_qmi_client_handle,
                  dun_call_action_type,
                  sys_dun_call_pref_info->cb_data,
                  &qmi_err_code
                );

      if ( 0 > ret_val )
      {
        LOG_MSG_ERROR_2("ds_sys_3gpp_ioctl_cb(): ioctl %d : err=%d",
                        DS_SYS_IOCTL_DUN_CTRL_RESPONSE, qmi_err_code );
        *ps_errno = ps_sys_confi_map_qmi_err_ds(qmi_err_code);
        return -1;
      }
      break;
    }

    default:
    {
      LOG_MSG_ERROR_1("ds_sys_3gpp_ioctl_cb(): Unknown ioctl %d :"
                      " err=%d", ioctl_name );
      *ps_errno = DS_EINVAL;
      return -1;
    }
  }
  
  return ret_val;
} /* ds_sys_3gpp_ioctl_cb() */

int16 ds_sys_3gpp2_ioctl_cb
(
  ps_sys_ioctl_enum_type          ioctl_name,
  void                          * arg_val_ptr,
  int16                         * ps_errno
)
{
  int                                   qmi_err_code = QMI_SERVICE_ERR_NONE;
  unsigned int                          cnt_apn;
  int16 ret_val;
  qmi_wds_pdn_throttle_info_type      * wds_pdn_throttle_info;
  unsigned int                          throttle_info_len = PS_SYS_MAX_APNS;
  ps_sys_ioctl_pdn_throttle_info_type * sys_pdn_throttle_info;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  LOG_MSG_FUNCTION_ENTRY_1("ds_sys_3gpp2_ioctl_cb(): Ioctl %d", ioctl_name);

  switch (ioctl_name)
  { 
    case DS_SYS_IOCTL_PDN_THROTTLE_INFO :
    {
      sys_pdn_throttle_info = 
        (ps_sys_ioctl_pdn_throttle_info_type *) arg_val_ptr;

       PS_SYSTEM_HEAP_MEM_ALLOC(wds_pdn_throttle_info, 
                                sizeof(qmi_wds_pdn_throttle_info_type) * PS_SYS_MAX_APNS,
                                qmi_wds_pdn_throttle_info_type*);

      if (wds_pdn_throttle_info == NULL)
      {
        *ps_errno =  DS_ENOMEM;   
        return -1;
      }           

      ret_val = qmi_wds_get_pdn_throttle_info(ds_sys_qmi_client_handle,
                                              PS_SYS_TECH_3GPP2,
                                              wds_pdn_throttle_info,
                                              &throttle_info_len,
                                              &qmi_err_code);
      if ( 0 > ret_val )
      {
        LOG_MSG_ERROR_1("ds_sys_3gpp2_ioctl_cb():  PDN Throttle info : "
                        "err=0x%p", qmi_err_code );
        *ps_errno = ps_sys_confi_map_qmi_err_ds(qmi_err_code);
        PS_SYSTEM_HEAP_MEM_FREE(wds_pdn_throttle_info);
        return -1;
      }

      sys_pdn_throttle_info->num_apn = throttle_info_len;

      for (cnt_apn = 0; cnt_apn < throttle_info_len; cnt_apn++)
      {
        sys_pdn_throttle_info->pdn_throttle_info[cnt_apn].is_ipv4_throttled =
          wds_pdn_throttle_info[cnt_apn].is_ipv4_throttled;
        sys_pdn_throttle_info->pdn_throttle_info[cnt_apn].is_ipv6_throttled =
          wds_pdn_throttle_info[cnt_apn].is_ipv6_throttled;
        sys_pdn_throttle_info-> \
          pdn_throttle_info[cnt_apn].remaining_ipv4_throttled_time =
            wds_pdn_throttle_info[cnt_apn].ipv4_throttled_remaining;
        sys_pdn_throttle_info-> \
          pdn_throttle_info[cnt_apn].remaining_ipv6_throttled_time =
            wds_pdn_throttle_info[cnt_apn].ipv6_throttled_remaining;

        sys_pdn_throttle_info->pdn_throttle_info[cnt_apn].apn_string_len =
          strlen(wds_pdn_throttle_info[cnt_apn].apn_name) + 1;

        /* Revisit to check*/
        memscpy
        (
          sys_pdn_throttle_info->pdn_throttle_info[cnt_apn].apn_string,
          strlen(wds_pdn_throttle_info[cnt_apn].apn_name) + 1,
          wds_pdn_throttle_info[cnt_apn].apn_name,
          strlen(wds_pdn_throttle_info[cnt_apn].apn_name) + 1
        );
      }

      PS_SYSTEM_HEAP_MEM_FREE(wds_pdn_throttle_info);
      break;
    }

    default:
    {
      LOG_MSG_ERROR_1("ds_sys_3gpp2_ioctl_cb():  Unknown ioctl %d ",
                      ioctl_name );
      *ps_errno = DS_EINVAL;
      return -1;
    }
  }
  
  return ret_val;
} /* ds_sys_3gpp_ioctl_cb() */

void  ds_sys_ind_cb
(
  int                             user_handle,
  qmi_service_id_type             service_id,
  void                          * user_data,
  qmi_wds_indication_id_type      ind_id,
  qmi_wds_indication_data_type  * ind_data
)
{
  int16                                result = 0;
  int16                                dss_errno  = 0;
  unsigned int                         cnt_network;
  ps_iface_net_down_reason_type        ps_reason_code;
  ps_sys_event_data_call_status_change_type call_status_change_type;
  qmi_wds_data_call_type_type          data_call_type;
  qmi_wds_tethered_call_type_type      tethered_call_type;
  qmi_wds_data_call_status_type        data_call_status;

  ps_sys_system_status_type                    system_status_type;
  qmi_wds_data_sys_status_network_info_type  * network_info;
  unsigned int                                 network_info_len;
  qmi_wds_data_network_type                    pref_network;

  unsigned long                                event_param_mask = 0;
  ps_sys_event_dun_ctrl_entitlement_check_info_type 
    event_entitlement_check_type;
  ps_sys_event_dun_ctrl_silent_redial_check_info_type 
    event_silent_redial_check_type;
  ps_sys_event_dun_ctrl_call_status_info_type event_call_status_type;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  LOG_MSG_FUNCTION_ENTRY_1("ds_sys_ind_cb(): qmimsglib event:%d", ind_id);

  switch (ind_id)
  {
    case QMI_WDS_SRVC_EVDO_PAGE_MONITOR_RESULT_IND_MSG:
    {
      result = ps_sys_event_ind 
              (
                PS_SYS_TECH_3GPP2,
                PS_SYS_EVENT_3GPP2_EVDO_PAGE_MONITOR_PERIOD_STATUS,
                ind_data,
                &dss_errno
              );
      if (0 > result)
      {
        LOG_MSG_ERROR_2("ds_sys_ind_cb(): event:%d, dss_errno:%d "
                        "QMI_WDS_SRVC_EVDO_PAGE_MONITOR_RESULT_IND_MSG",
                        PS_SYS_EVENT_3GPP2_EVDO_PAGE_MONITOR_PERIOD_STATUS,
                        dss_errno );
      }
      break;
    }

    case QMI_WDS_SRVC_EVENT_REPORT_IND_MSG:
    {
      if (QMI_WDS_EVENT_PAGE_MONITOR_PERIOD_CHANGE_IND & 
          ind_data->event_report.event_mask)
      {
        LOG_MSG_INFO2_0("ds_sys_ind_cb() Event mask set for "
                        "QMI_WDS_EVENT_PAGE_MONITOR_PERIOD_CHANGE_IND" );

        result = ps_sys_event_ind 
                (
                  PS_SYS_TECH_3GPP2,
                  PS_SYS_EVENT_3GPP2_EVDO_PAGE_MONITOR_PERIOD_CHANGE,
                  &(ind_data->event_report.evdo_page_monitor_report),
                  &dss_errno
                );
        if (0 > result )
        {
          LOG_MSG_ERROR_2("ds_sys_ind_cb() event:%d, dss_errno:%d",
                          PS_SYS_EVENT_3GPP2_EVDO_PAGE_MONITOR_PERIOD_CHANGE,
                          dss_errno );
        }
      }
   
      if (QMI_WDS_EVENT_DATA_SYS_STATUS_IND  & 
          ind_data->event_report.event_mask)
      {
        LOG_MSG_INFO2_0("ds_sys_ind_cb() Event mask set for "
                        "QMI_WDS_EVENT_DATA_SYS_STATUS_IND" );

        network_info = ind_data->event_report.data_sys_status.network_info;
        network_info_len = 
          ind_data->event_report.data_sys_status.network_info_len;
        pref_network = ind_data->event_report.data_sys_status.pref_network;

        system_status_type.preferred_network = pref_network;

        for (cnt_network = 0; cnt_network < network_info_len; cnt_network++)
        {
          system_status_type.network_info[cnt_network].rat_mask = 
            network_info[cnt_network].rat_mask;
          system_status_type.network_info[cnt_network].so_mask = 
            network_info[cnt_network].so_mask;
        }

        result = ps_sys_event_ind 
                (
                  PS_SYS_TECH_ALL,
                  PS_SYS_EVENT_SYSTEM_STATUS_CHANGE,
                  &system_status_type,
                  &dss_errno
                );

        if (0 > result )
        {
          LOG_MSG_ERROR_2("ds_sys_ind_cb() event:%d, dss_errno:%d",
                          PS_SYS_EVENT_SYSTEM_STATUS_CHANGE, dss_errno );
        }
      }

      if ((QMI_WDS_EVENT_DATA_CALL_STATUS_CHG_IND & 
           ind_data->event_report.event_mask) && 
           (QMI_WDS_EVENT_DATA_CALL_TYPE_IND  & 
           ind_data->event_report.event_mask) )
      {
        LOG_MSG_INFO2_0("ds_sys_ind_cb() Event mask set for "
                        "QMI_WDS_EVENT_DATA_CALL_STATUS_CHG_IND and"
                        "QMI_WDS_EVENT_DATA_CALL_TYPE_IND " );
                
        data_call_type     = 
          ind_data->event_report.data_call_type.data_call_type;
        tethered_call_type = 
          ind_data->event_report.data_call_type.tethered_call_type;         
        data_call_status   =
          ind_data->event_report.data_call_status;
             
        switch (data_call_type)
        {
          case QMI_WDS_DATA_CALL_TYPE_MODEM_EMBEDDED :
          {
            call_status_change_type.call_type = 
              PS_SYS_EVENT_EMBEDDED_DATA_CALL; 
            break;
          }

          case QMI_WDS_DATA_CALL_TYPE_EMBEDDED :
          {
            call_status_change_type.call_type = 
              PS_SYS_EVENT_EMBEDDED_RMNET_DATA_CALL; 
            break;
          }

          case QMI_WDS_DATA_CALL_TYPE_TETHERED :
          {
            if (tethered_call_type == QMI_WDS_TETHERED_CALL_TYPE_DUN)
            {
              call_status_change_type.call_type = PS_SYS_EVENT_DUN_DATA_CALL;
            }
            else if (tethered_call_type == QMI_WDS_TETHERED_CALL_TYPE_RMNET)          
            {
              call_status_change_type.call_type = 
                PS_SYS_EVENT_TETHERED_RMNET_DATA_CALL;
            }
            break;
          }

          default:
          {
            LOG_MSG_ERROR_1("ds_sys_ind_cb() Invalid call type:%d",
                            data_call_type);
            break;
          }           
        }  
        
        switch (data_call_status)
        {
          case QMI_WDS_DATA_CALL_ACTIVATED :
          {
            call_status_change_type.call_status = 
              PS_SYS_EVENT_DATA_CALL_ESTABLISHED; 
            break;
          }

          case QMI_WDS_DATA_CALL_TERMINATED :
          {
            call_status_change_type.call_status = 
              PS_SYS_EVENT_DATA_CALL_TERMINATED; 
            break;
          }

          default:
          {
            LOG_MSG_ERROR_1("ds_sys_ind_cb() Invalid call status:%d",
                            data_call_status);
            break;
          }
        }              
                                    
        result = ps_sys_event_ind (PS_SYS_TECH_ALL,
                                   PS_SYS_EVENT_DATA_CALL_STATUS_CHANGE,
                                   &call_status_change_type,
                                   &dss_errno);
        if (0 > result )
        {
          LOG_MSG_ERROR_2("ds_sys_ind_cb() event:%d, dss_errno:%d",
                          PS_SYS_EVENT_DATA_CALL_STATUS_CHANGE, dss_errno );
          break;
        }
      }
      break;
    }

    case QMI_WDS_SRVC_DUN_CTRL_EVENT_REPORT_IND_MSG:
    {
      if (DUN_CTRL_EVENT_CALL_NOTIFICATION == 
          ind_data->dun_ctrl_event_report.dun_control_event)
      {
        event_param_mask = ind_data->dun_ctrl_event_report.param_mask;
        if (event_param_mask & QMI_WDS_DUN_CTRL_EVENT_CALL_NOTIF_PARAM_MASK)
        {
          LOG_MSG_INFO2_0("ds_sys_ind_cb() Call notification mask set for "
                          "call notif event" );
          event_call_status_type.is_call_established = 
            ind_data->dun_ctrl_event_report.dun_call_notif;
        }

        if (event_param_mask & QMI_WDS_DUN_CTRL_EVENT_CALL_ID_PARAM_MASK)
        {
          LOG_MSG_INFO2_0("ds_sys_ind_cb() Call Id mask set for call"
                          " notif event" );
          event_call_status_type.cb_data =
            ind_data->dun_ctrl_event_report.dun_call_id;
        }

        if (event_param_mask & 
            QMI_WDS_DUN_CTRL_EVENT_CALL_END_REASON_PARAM_MASK)
        {
          LOG_MSG_INFO2_0("ds_sys_ind_cb() Call end reason mask set for "
                          "call notif event" );
          ps_reason_code = 
            TYPE_REASON_VAL
              (ind_data-> \
                 dun_ctrl_event_report.call_end_reason.verbose_reason_type,
              ind_data->dun_ctrl_event_report.call_end_reason.verbose_reason);

          event_call_status_type.net_down_reason = ps_reason_code;
        } 

        /* Defaulting to 3GPP since dun cntrl notif for 3gpp2 is not
           supported*/  
        event_call_status_type.tech_type = PS_SYS_TECH_3GPP;

        result = ps_sys_event_ind 
                 (
                   PS_SYS_TECH_ALL,
                   PS_SYS_EVENT_DUN_CTRL_CALL_STATUS,
                   &event_call_status_type,
                   &dss_errno
                 );

        if (0 > result )
        {
          LOG_MSG_ERROR_2("ds_sys_ind_cb() event:%d, dss_errno:%d",
                          PS_SYS_EVENT_DUN_CTRL_CALL_STATUS, dss_errno );
          break;
        }
      }

      else if (DUN_CTRL_EVENT_ENTITILEMENT_NOTIFICATION == 
               ind_data->dun_ctrl_event_report.dun_control_event)
      {
        event_param_mask = ind_data->dun_ctrl_event_report.param_mask;
        if (event_param_mask & QMI_WDS_DUN_CTRL_EVENT_CALL_ID_PARAM_MASK)
        {
          LOG_MSG_INFO2_0("ds_sys_ind_cb() Call Id mask set for entitlement"
                          " notif event " );
          event_entitlement_check_type.cb_data =
            ind_data->dun_ctrl_event_report.dun_call_id;
        }
        
        /* Defaulting to 3GPP since dun cntrl notif for 3gpp2 is not
           supported*/  
        event_entitlement_check_type.tech_type = PS_SYS_TECH_3GPP;

        result = ps_sys_event_ind
                (
                  PS_SYS_TECH_ALL,
                  PS_SYS_EVENT_DUN_CTRL_ENTITLEMENT_CHECK,
                  &event_entitlement_check_type,
                  &dss_errno
                );
        if (0 > result )
        {
          LOG_MSG_ERROR_2("ds_sys_ind_cb() event:%d, dss_errno:%d",
                          PS_SYS_EVENT_DUN_CTRL_ENTITLEMENT_CHECK, dss_errno );
          break;
        }
      }

      else if (DUN_CTRL_EVENT_SILENT_REDIAL_NOTIFICATION == 
               ind_data->dun_ctrl_event_report.dun_control_event)
      {
        event_param_mask = ind_data->dun_ctrl_event_report.param_mask;

        if (event_param_mask & QMI_WDS_DUN_CTRL_EVENT_CALL_ID_PARAM_MASK)
        {
          LOG_MSG_INFO2_0("ds_sys_ind_cb() Call Id mask set for silent "
                          "redial notif" );
          event_silent_redial_check_type.cb_data =
          ind_data->dun_ctrl_event_report.dun_call_id;
        }

        if (event_param_mask & 
            QMI_WDS_DUN_CTRL_EVENT_CALL_END_REASON_PARAM_MASK)
        {
          LOG_MSG_INFO2_0("ds_sys_ind_cb() Call end reason mask set for "
                          "silent redial notif" );
          ps_reason_code = 
            TYPE_REASON_VAL
            (ind_data-> \
               dun_ctrl_event_report.call_end_reason.verbose_reason_type,
             ind_data->dun_ctrl_event_report.call_end_reason.verbose_reason);
          event_silent_redial_check_type.net_down_reason = ps_reason_code;
        } 

        /* Defaulting to 3GPP since dun cntrl notif for 3gpp2 is not
           supported*/  
        event_silent_redial_check_type.tech_type = PS_SYS_TECH_3GPP;

        result = ps_sys_event_ind 
                 (
                   PS_SYS_TECH_ALL,
                   PS_SYS_EVENT_DUN_CTRL_SILENT_REDIAL_CHECK,
                   &event_silent_redial_check_type,
                   &dss_errno
                 );

        if (0 > result )
        {
          LOG_MSG_ERROR_2("ds_sys_ind_cb() event:%d, dss_errno:%d",
                          PS_SYS_EVENT_DUN_CTRL_SILENT_REDIAL_CHECK,
                          dss_errno );
          break;
        }
      }
      break;
    }
    default:
    {
      LOG_MSG_ERROR_0("ds_sys_ind_cb() DS SYS unsupported event");
      break;
    }
  }

} /* ds_sys_ind_cb */

void ds_sys_qmi_client_release
(
  void
)
{
  int qmi_err;

  /* Release QMI WDS Service client */
  if (ds_sys_qmi_client_handle >= 0)
  {
    LOG_MSG_ERROR_1("ds_sys_qmi_client_release(): Releasing WDS qmi client handle 0x%08x",
                    ds_sys_qmi_client_handle);
    if ( qmi_wds_srvc_release_client( ds_sys_qmi_client_handle, &qmi_err ) < 0 )
    {
      if (!ds_sysi_is_modem_reset_in_progress())
      {
        LOG_MSG_ERROR_1("ds_sys_qmi_client_release(): Failed to release qmi client handle 0x%08x",
                        ds_sys_qmi_client_handle);
      }
    }

    ds_sys_qmi_client_handle = -1;
  }
} /* ds_sys_qmi_client_release */

void ds_sys_qmi_cleanup
(
  void
)
{
  /* Make sure qmi handle has been acquired */
  if (qmi_handle < 0)
  {
    LOG_MSG_ERROR_0("ds_sys_qmi_cleanup(): QMI message library was never initialized");
    return;
  }

  ds_sys_qmi_client_release();

  /* Release QMI library connection */
  (void)qmi_release(qmi_handle);
  qmi_handle = -1;

} /* ds_sys_qmi_cleanup */

int16 ds_sys_qmi_client_init
(
  void
)
{
  qmi_wds_event_report_params_type          wds_report_state;
  qmi_wds_set_dun_ctrl_evt_report_type      wds_dun_ctrl_report_state;
  qmi_wds_dun_accepted_evt_report_mask_type wds_dun_ctrl_accepted_mask = 0;

  int                                       return_val = 0;
  int                                       qmi_err;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  LOG_MSG_FUNCTION_ENTRY_0("ds_sys_qmi_client_init()");
  /*-------------------------------------------------------------------------
    Initialize qmi connection
  -------------------------------------------------------------------------*/
  return_val = qmi_connection_init(QMI_PORT_RMNET_1, &qmi_err);
  if (return_val < 0)
  {
    LOG_MSG_ERROR_1("ds_sys_qmi_client_init(): QMI connection init failed: err=%d",
                    qmi_err);
    return -1;
  }

  /*-------------------------------------------------------------------------
    Initialize WDS client
  -------------------------------------------------------------------------*/
  ds_sys_qmi_client_handle = qmi_wds_srvc_init_client
                             (
                               QMI_PORT_RMNET_1,
                               ds_sys_ind_cb,
                               NULL,
                               &qmi_err
                             );

  if (ds_sys_qmi_client_handle < 0)
  {
    LOG_MSG_ERROR_1("ds_sys_qmi_client_init(): qmi_wds_srvc_init_client failed: err=%d",
                    qmi_err);
    return -1;
  }

  /*-------------------------------------------------------------------------
    Set WDS service to report global events.
  -------------------------------------------------------------------------*/
  #define QMI_WDS_STATE_REPORT_MASK                              \
          (QMI_WDS_EVENT_PAGE_MONITOR_PERIOD_CHANGE_IND |       \
            QMI_WDS_EVENT_DATA_CALL_STATUS_CHG_IND |             \
             QMI_WDS_EVENT_DATA_CALL_TYPE_IND      |             \
               QMI_WDS_EVENT_DATA_SYS_STATUS_IND)

  wds_report_state.param_mask = QMI_WDS_STATE_REPORT_MASK;
  wds_report_state.report_evdo_page_monitor_event_chg = TRUE;
  wds_report_state.report_data_call_status_chg = TRUE;
  wds_report_state.report_data_sys_status      = TRUE;

  return_val = qmi_wds_set_event_report(ds_sys_qmi_client_handle,
                                        &wds_report_state,
                                        &qmi_err);
  if (return_val < 0)
  {
    LOG_MSG_ERROR_1("ds_sys_qmi_client_init(): Set event report failed: err=%d",
                    qmi_err);
    return -1;
  }

  /*-------------------------------------------------------------------------
    Set WDS service to report DUN ctrl events.
  -------------------------------------------------------------------------*/
  #define QMI_WDS_DUN_CTRL_REPORT_MASK                              \
          (0)

  memset(&wds_dun_ctrl_report_state, 0,
         sizeof(qmi_wds_set_dun_ctrl_evt_report_type));

  wds_dun_ctrl_report_state.param_mask = QMI_WDS_DUN_CTRL_REPORT_MASK;
  wds_dun_ctrl_report_state.notify_dun_call = DUN_CTRL_ENABLE_DUN_CALL_NOTIF;

  return_val = qmi_wds_set_dun_ctrl_event_report(ds_sys_qmi_client_handle,
                                                 &wds_dun_ctrl_report_state,
                                                 &wds_dun_ctrl_accepted_mask,
                                                 &qmi_err );
  if (return_val < 0)
  {
    LOG_MSG_ERROR_2("ds_sys_qmi_client_init():  DSSYS wds set dun ctrl event "
                    "report failed  err=%d, accept_mask=%d",
                    qmi_err, wds_dun_ctrl_accepted_mask);
    return -1;
  }

  return 0;
} /* ds_sys_qmi_client_init */

int16 ds_sys_init_lib
(
  void
)
{
  int                                       return_val = 0;
  int16                                     ps_errno;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
 
  LOG_MSG_FUNCTION_ENTRY_0("ds_sys_init_lib()");

  qmi_handle = qmi_init(ds_sys_qmi_sys_event_handler, NULL);

  atexit(ds_sys_qmi_cleanup);

  return_val = ds_sys_qmi_client_init();
  if (return_val < 0) 
  {
    return -1;
  }

  /*-------------------------------------------------------------------------
    Register for 3GPP/3GPP2 ioctl callbacks with PS sys layer
  -------------------------------------------------------------------------*/
  return_val = ps_sys_ioctl_register_tech_cback(PS_SYS_TECH_3GPP,
                                                ds_sys_3gpp_ioctl_cb,
                                                &ps_errno);

  if (return_val < 0)
  {
    LOG_MSG_ERROR_1("ds_sys_init_lib(): Ioctl CB registration for "
                    "3gpp failed: err=%d", ps_errno);
    return -1;
  }

  return_val = ps_sys_ioctl_register_tech_cback(PS_SYS_TECH_3GPP2,
                                                ds_sys_3gpp2_ioctl_cb,
                                                &ps_errno);

  if (return_val < 0)
  {
    LOG_MSG_ERROR_1("ds_sys_init_lib(): Ioctl CB registration for "
                    "3gpp2 failed: err=%d", ps_errno);
    return -1;
  }

  return 0;
} /* ds_sys_init_lib() */
