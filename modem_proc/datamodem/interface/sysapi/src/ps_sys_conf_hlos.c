/******************************************************************************
  @file    ps_sys_conf.c
  @brief   

  DESCRIPTION
  This file defines functions, enums and structs used to read and update
  technology configurations

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
 
  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/sysapi/src/ps_sys_conf_hlos.c#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $ 

  
when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/31/11   bvd     Created module
===========================================================================*/

/*---------------------------------------------------------------------------
                           INCLUDE FILES
---------------------------------------------------------------------------*/

#include "ps_sys_confi.h"
#include "ps_sys_eventi.h"
#include "ps_system_heap.h"
#include "dserrno.h"
#include "ds_Utils_DebugMsg.h"
#include "qmi_wds_srvc.h"
#include "ps_sys_eventi.h"
#include "ps_system_heap.h"

/* Cache for all technology configuration data */
static ps_sys_confi_tech_all conf_tech_all;

/* Cache for 3gpp2 configuration data */
static ps_sys_confi_tech_3gpp2 conf_tech_3gpp2;

/* Cache for wlan configuration data */
static ps_sys_confi_tech_wlan conf_tech_wlan;

extern int ds_sys_qmi_client_handle;


int16  ps_sys_confi_map_qmi_err_ds
(
  int    qmi_err_code
)
{
  int16 ds_errno = DS_ENOERR;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  switch (qmi_err_code)
  {    
    case QMI_SERVICE_ERR_NONE:
      ds_errno = DS_ENOERR;
      break;

    case QMI_SERVICE_ERR_INTERNAL:
    case QMI_SERVICE_ERR_NO_EFFECT:
    case QMI_SERVICE_ERR_INVALID_ID:
      ds_errno = DS_EINVAL;
      break;

    case QMI_SERVICE_ERR_MISSING_ARG:
    case QMI_SERVICE_ERR_INVALID_ARG:
    case QMI_SERVICE_ERR_MALFORMED_MSG:
      ds_errno = DS_EFAULT;
      break;

    case QMI_SERVICE_ERR_NO_MEMORY:
      ds_errno = DS_ENOMEM;
      break;

    case QMI_SERVICE_ERR_OP_DEVICE_UNSUPPORTED:
      ds_errno = DS_EOPNOTSUPP;
      break;

    case QMI_SERVICE_ERR_OUT_OF_CALL:
      ds_errno = DS_ENOTCONN;
      break;

    case QMI_SERVICE_ERR_INFO_UNAVAILABLE:
      ds_errno = DS_ENETDOWN;
      break;

    default:
      LOG_MSG_ERROR_1("ps_sys_confi_map_qmi_err_ds Unrecognized qmi error "
                      "code: %d", qmi_err_code );
      ds_errno = DS_EINVAL;
      break;
  }

  return ds_errno;

} /* ps_sys_confi_map_qmi_err_ds() */

int16 ps_sys_conf_get
(
  ps_sys_tech_enum_type             tech_type,
  ps_sys_conf_enum_type             conf_name,
  void                            * argval_ptr,
  int16                           * dss_errno
)
{
  int16                       ret_val = 0;
  int                         qmi_err_code = QMI_SERVICE_ERR_NONE;
  qmi_wds_dun_ctrl_info_type  dun_ctrl_info;
  unsigned int                network_info_len = DS_SYS_NETWORK_MAX;
  qmi_wds_data_network_type   pref_network;
  unsigned int                cnt_network;

  qmi_wds_data_sys_status_network_info_type * network_info = NULL;
  ps_sys_system_status_type                 * system_status_type;
  ps_sys_conf_get_dun_ctrl_pref_type        * dun_ctrl_info_type;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  LOG_MSG_FUNCTION_ENTRY_2("ps_sys_conf_get(): conf_name:%d, argval_ptr:0x%p",
                           conf_name, argval_ptr);

  ret_val = ps_sys_confi_verify_tech_conf_name(tech_type, conf_name);
  if (0 > ret_val || argval_ptr == NULL)
  {
    *dss_errno = DS_EFAULT;
    LOG_MSG_ERROR_0("ps_sys_conf_get(): Invalid argument passed");
    return -1;
  }

  switch (conf_name)
  {
    case PS_SYS_CONF_SYSTEM_STATUS:  
    {     
      system_status_type = (ps_sys_system_status_type *) argval_ptr; 
       PS_SYSTEM_HEAP_MEM_ALLOC (network_info, 
        sizeof(qmi_wds_data_sys_status_network_info_type) * network_info_len,
        qmi_wds_data_sys_status_network_info_type*);

      ret_val = qmi_wds_get_curr_data_sys_status(ds_sys_qmi_client_handle,
                                                 &pref_network,
                                                 network_info,                                                 
                                                 &network_info_len,
                                                 &qmi_err_code);
      if (0 > ret_val)
      {
        LOG_MSG_ERROR_1("ps_sys_conf_get(): get conf sys status: err=0x%p",
                        qmi_err_code );
        *dss_errno = ps_sys_confi_map_qmi_err_ds(qmi_err_code);
        return -1;
      }
      
      system_status_type->preferred_network = pref_network;

      for (cnt_network = 0; cnt_network < network_info_len; cnt_network++)
      {
        system_status_type->network_info[cnt_network].rat_mask =
          network_info[cnt_network].rat_mask;
        system_status_type->network_info[cnt_network].so_mask =
          network_info[cnt_network].so_mask;
      }

      LOG_MSG_INFO2_3("ps_sys_conf_get : pref network %d, RAT mask 0x%x"
                      "so mask 0x%x",
                      pref_network,
                      network_info[cnt_network].rat_mask,
                      network_info[cnt_network].so_mask);
                      
      PS_SYSTEM_HEAP_MEM_FREE(network_info);

      break;
    }

    case PS_SYS_CONF_3GPP2_PRE_DORMANCY_SYS_INFO:
    {
      ret_val = qmi_wds_get_pre_dormancy_cdma_settings
                (
                  ds_sys_qmi_client_handle,
                  argval_ptr,
                  &qmi_err_code 
                );
      if (0 > ret_val )
      {
        LOG_MSG_ERROR_1("ps_sys_conf_get(): get pre dorm settings: err=0x%p",
                        qmi_err_code );
        *dss_errno = ps_sys_confi_map_qmi_err_ds(qmi_err_code);
        return -1;
      }

      break;
    }

    case PS_SYS_CONF_3GPP2_CAM_TIMER:
    {
      ret_val = qmi_wds_get_cam_timer(ds_sys_qmi_client_handle,
                                      argval_ptr,
                                      &qmi_err_code);
      if (0 > ret_val )
      {
        LOG_MSG_ERROR_1("ps_sys_conf_get(): get CAM timer: err=0x%p",
                        qmi_err_code );
        *dss_errno = ps_sys_confi_map_qmi_err_ds(qmi_err_code);
        return -1;
      }

      break;
    }

    case PS_SYS_CONF_3GPP2_SCRM:
    {
      ret_val = qmi_wds_get_scrm(ds_sys_qmi_client_handle,
                                 argval_ptr,
                                 &qmi_err_code);
      if (0 > ret_val )
      {
        LOG_MSG_ERROR_1("ps_sys_conf_get(): get SCRM: err=0x%p",
                        qmi_err_code );
        *dss_errno = ps_sys_confi_map_qmi_err_ds(qmi_err_code);
        return -1;
      }

      break;
    }

    case PS_SYS_CONF_3GPP2_RDUD:
    {
      ret_val = qmi_wds_get_rdud(ds_sys_qmi_client_handle,
                                 argval_ptr,
                                 &qmi_err_code);
      if (0 > ret_val )
      {
        LOG_MSG_ERROR_1("ps_sys_conf_get(): get RDUD: err=0x%p",
                        qmi_err_code );
        *dss_errno = ps_sys_confi_map_qmi_err_ds(qmi_err_code);
        return -1;
      }

      break;
    }

    case PS_SYS_CONF_3GPP2_CALL_THROTTLE_INFO:
    {
      ret_val = qmi_wds_get_call_throttle_info(ds_sys_qmi_client_handle,
                                               argval_ptr,
                                               &qmi_err_code);
      if (0 > ret_val )
      {
        LOG_MSG_ERROR_1("ps_sys_conf_get(): get call throttle info: err=0x%p",
                        qmi_err_code );
        *dss_errno = ps_sys_confi_map_qmi_err_ds(qmi_err_code);
        return -1;
      }

      break;
    }

    case PS_SYS_CONF_3GPP_NSAPI:      
    {
      ret_val = qmi_wds_get_nsapi
                (
                  ds_sys_qmi_client_handle,
                  (char *)(((ps_sys_conf_3gpp_nsapi_type *) argval_ptr)->apn),
                  ((ps_sys_conf_3gpp_nsapi_type *) argval_ptr)->nsapi,
                  &(((ps_sys_conf_3gpp_nsapi_type *) argval_ptr)->num_nsapi),
                  &qmi_err_code 
                );
      if (0 > ret_val )
      {
        LOG_MSG_ERROR_1("ps_sys_conf_get(): get NSAPI: err=0x%p",
                        qmi_err_code );
        *dss_errno = ps_sys_confi_map_qmi_err_ds(qmi_err_code);
        return -1;
      }

      break;
    }

    case PS_SYS_CONF_3GPP2_PAGE_MONITOR_PERIOD:
    {
      ret_val = qmi_wds_get_evdo_page_monitor_period
                (
                  ds_sys_qmi_client_handle,
                  (qmi_wds_evdo_page_monitor_period_type *)
                    &(((ds_sys_3gpp2_page_monitor_type *) argval_ptr)-> \
                      page_monitor_period),
                  (qmi_wds_evdo_force_long_sleep_type *)
                    &(((ds_sys_3gpp2_page_monitor_type *) argval_ptr)-> \
                        force_long_sleep),
                  &qmi_err_code 
                );
      if (0 > ret_val )
      {
        LOG_MSG_ERROR_1("ps_sys_conf_get(): get page monitor period: err=0x%p",
                        qmi_err_code );
        *dss_errno = ps_sys_confi_map_qmi_err_ds(qmi_err_code);
        return -1;
      }

      break;
    }

    case PS_SYS_CONF_DUN_CTRL_PREF:
    {
      dun_ctrl_info_type = (ps_sys_conf_get_dun_ctrl_pref_type *) argval_ptr;
      memset(&dun_ctrl_info, 0, sizeof(qmi_wds_dun_ctrl_info_type));

      ret_val = qmi_wds_get_dun_ctrl_info(ds_sys_qmi_client_handle,
                                          &dun_ctrl_info,
                                          &qmi_err_code);

      if (0 > ret_val )
      {
        LOG_MSG_ERROR_2("ps_sys_conf_get(): conf %d, QMI err %d",
                        conf_name, qmi_err_code );
        *dss_errno = ps_sys_confi_map_qmi_err_ds(qmi_err_code);
        return -1;
      }

      dun_ctrl_info_type->is_dun_ctrl_enabled = dun_ctrl_info.dun_ctrl_status;
      dun_ctrl_info_type->allow_dun_call      = dun_ctrl_info.dun_allow_pref;

      if (dun_ctrl_info.event_report_mask & 
            QMI_WDS_DUN_CTRL_SEND_CALL_COMPLETION_NOTIF_MASK)
      {
        dun_ctrl_info_type->is_call_status_notification_enabled = TRUE;
      }

      if (dun_ctrl_info.event_report_mask & 
            QMI_WDS_DUN_CTRL_SEND_ENTITLEMENT_NOTIF_MASK)
      {
        dun_ctrl_info_type->is_entitlement_check_enabled = TRUE;
      }

      if (dun_ctrl_info.event_report_mask & 
            QMI_WDS_DUN_CTRL_SEND_SILENT_REDIAL_NOTIF_MASK)
      {
        dun_ctrl_info_type->is_silent_redial_check_enabled = TRUE;
      }           

      break;
    }

    default:
    {
       LOG_MSG_ERROR_1("ps_sys_conf_get(): Invalid config type: %d",
                       conf_name );
       *dss_errno = ps_sys_confi_map_qmi_err_ds(qmi_err_code);
       return -1;
    }
  }

  return 0;
} /* ps_sys_conf_get */


int16 ps_sys_conf_set
(
  ps_sys_tech_enum_type             tech_type,
  ps_sys_conf_enum_type             conf_name,
  void                            * argval_ptr,
  int16                           * dss_errno
)
{
  int16                                     ret_val = 0;
  int                                       return_val = 0;
  int16                                     ps_errno = 0;
  int                                       qmi_err;
  int                                       qmi_err_code = 
                                              QMI_SERVICE_ERR_NONE;
  qmi_wds_dun_ctrl_pref_type                dun_ctrl_pref;
  ps_sys_conf_set_dun_ctrl_pref_type      * set_dun_ctrl_pref_ptr =  NULL;
  uint32                                    dun_ctrl_event_report_bit_mask = 0;
  qmi_wds_set_dun_ctrl_evt_report_type      wds_dun_ctrl_report_state;
  qmi_wds_dun_accepted_evt_report_mask_type wds_dun_ctrl_accepted_mask = 0;
  ps_sys_conf_3gpp2_set_page_monitor_type * set_page_monitor_type =  NULL;
  ds_sys_3gpp2_page_monitor_period_type     page_monitor_period_type;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  LOG_MSG_FUNCTION_ENTRY_2("ps_sys_conf_set(): conf_name:%d, argval_ptr:0x%p",
                           conf_name, argval_ptr);

  ret_val = ps_sys_confi_verify_tech_conf_name(tech_type, conf_name);
  if (0 > ret_val || argval_ptr == NULL)
  {
    *dss_errno = DS_EFAULT;
    LOG_MSG_ERROR_0("ps_sys_conf_set(): Invalid argument passed");
    return -1;
  }

  switch (conf_name)
  {
    case PS_SYS_CONF_3GPP2_CAM_TIMER:
    {
      ret_val = qmi_wds_set_cam_timer(ds_sys_qmi_client_handle,
                                      *((qmi_wds_cam_timer_type *) argval_ptr),
                                      &qmi_err_code);
      if (0 > ret_val )
      {
        LOG_MSG_ERROR_1("ps_sys_conf_set(): set CAM timer: err=0x%p",
                        qmi_err_code );
        *dss_errno = ps_sys_confi_map_qmi_err_ds(qmi_err_code);
        return -1;
      }

      break;
    }

    case PS_SYS_CONF_3GPP2_SCRM:
    {
      ret_val = qmi_wds_set_scrm(ds_sys_qmi_client_handle,
                                 *((qmi_wds_scrm_type *) argval_ptr),
                                 &qmi_err_code);
      if (0 > ret_val )
      {
        LOG_MSG_ERROR_1("ps_sys_conf_set(): set SCRM: err=0x%p",
                        qmi_err_code );
        *dss_errno = ps_sys_confi_map_qmi_err_ds(qmi_err_code);
        return -1;
      }

      break;
    }

    case PS_SYS_CONF_3GPP2_RDUD:
    {
      ret_val = qmi_wds_set_rdud(ds_sys_qmi_client_handle,
                                 *((qmi_wds_rdud_type *) argval_ptr),
                                 &qmi_err_code);
      if (0 > ret_val )
      {
        LOG_MSG_ERROR_1("ps_sys_conf_set(): set RDUD: err=0x%p",
                        qmi_err_code );
        *dss_errno = ps_sys_confi_map_qmi_err_ds(qmi_err_code);
        return -1;
      }

      break;
    }

    case PS_SYS_CONF_3GPP2_PAGE_MONITOR_PERIOD:
    {
      /* Register for event. We will deregister it when we get the
         event back
       */      
      set_page_monitor_type = 
        (ps_sys_conf_3gpp2_set_page_monitor_type *)argval_ptr;

      if (set_page_monitor_type->event_cback_f_ptr != NULL)
      {
        ret_val =
          ps_sys_event_reg(PS_SYS_TECH_3GPP2, 
                           PS_SYS_EVENT_3GPP2_EVDO_PAGE_MONITOR_PERIOD_STATUS,
                           (ps_sys_event_cback_f_ptr_type)
                             set_page_monitor_type->event_cback_f_ptr, 
                           NULL, 
                           dss_errno);
        if (0 > ret_val )
        {
          LOG_MSG_ERROR_1("ps_sys_conf_set(): Event reg failed : err=%d",
                          dss_errno );
          *dss_errno = ps_sys_confi_map_qmi_err_ds(qmi_err_code);
          return -1;
        }
      }
          
      page_monitor_period_type = set_page_monitor_type->page_monitor_period;

      ret_val = qmi_wds_set_evdo_page_monitor_period
                (
                  ds_sys_qmi_client_handle,
                  (qmi_wds_evdo_page_monitor_period_type)
                    page_monitor_period_type,
                  &qmi_err_code 
                );
      if (0 > ret_val )
      {
        LOG_MSG_ERROR_1("ps_sys_conf_set(): set page monitor period: err=0x%p",
                        qmi_err_code );
        *dss_errno = ps_sys_confi_map_qmi_err_ds(qmi_err_code);       
        return -1;
      }
     
      break;
    }

    case PS_SYS_CONF_3GPP2_FORCE_LONG_SLEEP:
    {
      ret_val = qmi_wds_set_evdo_force_long_sleep
                (
                  ds_sys_qmi_client_handle,
                  *((qmi_wds_evdo_force_long_sleep_type *) argval_ptr),
                  &qmi_err_code 
                );
      if (0 > ret_val )
      {
        LOG_MSG_ERROR_1("ps_sys_conf_set(): set EVDO long sleep: err=0x%p",
                        qmi_err_code );
        *dss_errno = ps_sys_confi_map_qmi_err_ds(qmi_err_code);
        return -1;
      }

      break;
    }

    case PS_SYS_CONF_DUN_CTRL_PREF:
    {
      /*-----------------------------------------------------------------------
        We empty the queue for entitilement check, silent redial and
        call notification events everytime user calls set conf api
        for dun ctrl events. We are blocking explicit registration
        for all the three events in ds_sys_event_reg function
      -----------------------------------------------------------------------*/
      ret_val = ps_sys_eventi_empty_event_queue
                (
                  PS_SYS_EVENT_DUN_CTRL_ENTITLEMENT_CHECK,
                  &ps_errno
                );

      if (ret_val < 0)
      {
        LOG_MSG_ERROR_3("ps_sys_conf_set() conf %d event=%d, err=%d",
                        conf_name,
                        PS_SYS_EVENT_DUN_CTRL_ENTITLEMENT_CHECK,
                        ps_errno);
        return -1;
      }

      ret_val = ps_sys_eventi_empty_event_queue
                (
                  PS_SYS_EVENT_DUN_CTRL_SILENT_REDIAL_CHECK,
                  &ps_errno
                );

      if (ret_val < 0)
      {
        LOG_MSG_ERROR_3("ps_sys_conf_set() conf %d event=%d, err=%d",
                        conf_name,
                        PS_SYS_EVENT_DUN_CTRL_SILENT_REDIAL_CHECK,
                        ps_errno);
        return -1;
      }

      ret_val = ps_sys_eventi_empty_event_queue
                (
                  PS_SYS_EVENT_DUN_CTRL_CALL_STATUS,
                  &ps_errno
                );

      if (ret_val < 0)
      {
        LOG_MSG_ERROR_3("ps_sys_conf_set() conf %d event=%d, err=%d",
                        conf_name,
                        PS_SYS_EVENT_DUN_CTRL_CALL_STATUS,
                        ps_errno);
        return -1;
      }

      set_dun_ctrl_pref_ptr = 
        (ps_sys_conf_set_dun_ctrl_pref_type *) argval_ptr;
      
      /*-----------------------------------------------------------------------
        if mask is not set by application then we set it to default value
        of true for dun allow preference
      -----------------------------------------------------------------------*/
      if (set_dun_ctrl_pref_ptr->optional_pref.param_mask & 
            PS_SYS_CONF_DUN_CTRL_PREF_ALLOW_DUN_CALL_MASK)
      {
        dun_ctrl_pref.dun_allow_pref = 
          set_dun_ctrl_pref_ptr->optional_pref.allow_dun_call;
      }
      else
      {  
        dun_ctrl_pref.dun_allow_pref = TRUE;
        set_dun_ctrl_pref_ptr->optional_pref.param_mask |= 
          PS_SYS_CONF_DUN_CTRL_PREF_ALLOW_DUN_CALL_MASK;
      }

      dun_ctrl_pref.dun_control_pref = set_dun_ctrl_pref_ptr->enable_dun_ctrl;
      dun_ctrl_pref.param_mask       =
        set_dun_ctrl_pref_ptr->optional_pref.param_mask;

      /*-----------------------------------------------------------------------
        Set WDS service to report DUN ctrl events 
      -----------------------------------------------------------------------*/
      memset(&wds_dun_ctrl_report_state,
             0, 
             sizeof(qmi_wds_set_dun_ctrl_evt_report_type));

      if (TRUE == set_dun_ctrl_pref_ptr->enable_dun_ctrl)
      {
         if (dun_ctrl_pref.param_mask &
               PS_SYS_CONF_DUN_CTRL_PREF_ENTITLEMENT_CHECK_MASK)
         {
           dun_ctrl_event_report_bit_mask |=
             QMI_WDS_SET_DUN_CTRL_EVT_RPT_ENABLE_ENTITLEMENT_NOTIF_PARAM_MASK;
           wds_dun_ctrl_report_state.notify_entitlement = 
             DUN_CTRL_ENABLE_ENTITLEMENT_NOTIF;

          if (NULL != set_dun_ctrl_pref_ptr-> \
                        optional_pref.entitlement_check_cback_f_ptr)
                        
           {
            /*-----------------------------------------------------------------            
              Register for event. We will deregister it when we recieve 
              the event.
            -----------------------------------------------------------------*/
            ret_val = 
              ps_sys_event_reg
              ( 
                PS_SYS_TECH_ALL, 
                PS_SYS_EVENT_DUN_CTRL_ENTITLEMENT_CHECK,
                (ps_sys_event_cback_f_ptr_type)
                  set_dun_ctrl_pref_ptr->optional_pref.entitlement_check_cback_f_ptr, 
                set_dun_ctrl_pref_ptr-> \
                  optional_pref.entitlement_check_cback_user_data_ptr, 
                dss_errno
              );

            if (0 > ret_val )
            {
              LOG_MSG_ERROR_3("ps_sys_conf_set() conf %d event=%d, err=%d",
                              conf_name,
                              PS_SYS_EVENT_DUN_CTRL_ENTITLEMENT_CHECK,
                              dss_errno);
              goto bail_dun_ctrl_pref;                           
            }
          }
        }
        
        if (dun_ctrl_pref.param_mask & 
              PS_SYS_CONF_DUN_CTRL_PREF_SILENT_REDIAL_CHECK_MASK)       
        { 
          dun_ctrl_event_report_bit_mask = dun_ctrl_event_report_bit_mask | 
            QMI_WDS_SET_DUN_CTRL_EVT_RPT_ENABLE_SILENT_REDIAL_NOTIF_PARAM_MASK;
          wds_dun_ctrl_report_state.notify_silent_redial = 
            DUN_CTRL_ENABLE_SILENT_REDIAL_NOTIF;
                
          if (NULL != set_dun_ctrl_pref_ptr-> \
                        optional_pref.silent_redial_check_cback_f_ptr)
          {          
            /*-----------------------------------------------------------------            
              Register for event. We will deregister it when we recieve 
              the event.
            -----------------------------------------------------------------*/
            ret_val = 
              ps_sys_event_reg
              ( 
                PS_SYS_TECH_ALL, 
                PS_SYS_EVENT_DUN_CTRL_SILENT_REDIAL_CHECK,
                (ps_sys_event_cback_f_ptr_type) set_dun_ctrl_pref_ptr-> \
                  optional_pref.silent_redial_check_cback_f_ptr, 
                set_dun_ctrl_pref_ptr-> \
                  optional_pref.silent_redial_check_cback_user_data_ptr, 
                dss_errno
              );

            if (0 > ret_val )
            {
              LOG_MSG_ERROR_3("ps_sys_conf_set() conf %d event=%d, err=%d",
                              conf_name,
                              PS_SYS_EVENT_DUN_CTRL_SILENT_REDIAL_CHECK,
                              dss_errno);
              goto bail_dun_ctrl_pref;
            }
          }
        }
        
        if (PS_SYS_CONF_DUN_CTRL_PREF_CALL_STATUS_NOTIFICATION_MASK &
            dun_ctrl_pref.param_mask)
        {          
          wds_dun_ctrl_report_state.notify_dun_call = 
            DUN_CTRL_ENABLE_DUN_CALL_NOTIF;

          if (NULL != set_dun_ctrl_pref_ptr-> \
                        optional_pref.call_status_notification_cback_f_ptr)
          {
            /*-----------------------------------------------------------------            
              Register for event. We will deregister it when we recieve 
              the event.
            -----------------------------------------------------------------*/
            ret_val = 
              ps_sys_event_reg
              ( 
                PS_SYS_TECH_ALL, 
                PS_SYS_EVENT_DUN_CTRL_CALL_STATUS,
                (ps_sys_event_cback_f_ptr_type) set_dun_ctrl_pref_ptr-> \
                  optional_pref.call_status_notification_cback_f_ptr, 
                set_dun_ctrl_pref_ptr-> \
                  optional_pref.call_status_notification_cback_user_data_ptr, 
                dss_errno
              );

            if (0 > ret_val )
            {
              LOG_MSG_ERROR_3("ps_sys_conf_set() conf %d event=%d, err=%d",
                              conf_name,
                              PS_SYS_EVENT_DUN_CTRL_CALL_STATUS,
                              dss_errno);
              goto bail_dun_ctrl_pref;
            }
          }         
        }
        else
        {
          /*-------------------------------------------------------------------
            if mask is not set then we set it to disable dun call notif
          -------------------------------------------------------------------*/
          wds_dun_ctrl_report_state.notify_dun_call = 
            DUN_CTRL_DISABLE_DUN_CALL_NOTIF;
        }
      }
      else
      {
        LOG_MSG_INFO2_0("Dun control preference not set");
      }

      ret_val = qmi_wds_set_dun_ctrl_pref(ds_sys_qmi_client_handle,
                                          &dun_ctrl_pref,
                                          &qmi_err_code);
      if (0 > ret_val )
      {
        LOG_MSG_ERROR_2("ps_sys_conf_set(): conf %d, QMI err %d",
                        conf_name, qmi_err_code);
        *dss_errno = ps_sys_confi_map_qmi_err_ds(qmi_err_code);
        goto bail_dun_ctrl_pref;
      }

      if (TRUE == set_dun_ctrl_pref_ptr->enable_dun_ctrl)
      {
        wds_dun_ctrl_report_state.param_mask = dun_ctrl_event_report_bit_mask;

        return_val = qmi_wds_set_dun_ctrl_event_report
                     (
                       ds_sys_qmi_client_handle,
                       &wds_dun_ctrl_report_state,
                       &wds_dun_ctrl_accepted_mask,
                       &qmi_err 
                     );
        if (return_val < 0)
        {
          LOG_MSG_ERROR_3("ps_sys_conf_set(): conf %d QMI err=%d accept_mask=%d",
                          conf_name, qmi_err, wds_dun_ctrl_accepted_mask);
          goto bail_dun_ctrl_pref;
        }
      }

      return 0;

      bail_dun_ctrl_pref :        
        
        /*---------------------------------------------------------------------
          Deregister all events callbacks in error case
        ---------------------------------------------------------------------*/
        (void)ps_sys_event_dereg
        (
          PS_SYS_TECH_ALL,
          PS_SYS_EVENT_DUN_CTRL_ENTITLEMENT_CHECK, 
          (ps_sys_event_cback_f_ptr_type)
            set_dun_ctrl_pref_ptr->optional_pref.entitlement_check_cback_f_ptr,  
          &ps_errno
        );

        (void)ps_sys_event_dereg
        (
          PS_SYS_TECH_ALL,
          PS_SYS_EVENT_DUN_CTRL_SILENT_REDIAL_CHECK, 
          (ps_sys_event_cback_f_ptr_type) 
            set_dun_ctrl_pref_ptr-> \
              optional_pref.silent_redial_check_cback_f_ptr,  
          &ps_errno
        );

        (void)ps_sys_event_dereg
        (
          PS_SYS_TECH_ALL,
          PS_SYS_EVENT_DUN_CTRL_CALL_STATUS, 
          (ps_sys_event_cback_f_ptr_type) set_dun_ctrl_pref_ptr-> \
            optional_pref.call_status_notification_cback_f_ptr,  
          &ps_errno
        );       

      return -1;

      break;
    }

    default:
    {
      LOG_MSG_ERROR_1("ps_sys_conf_set(): Invalid config type: %d", conf_name);
      *dss_errno = ps_sys_confi_map_qmi_err_ds(qmi_err_code);
      return -1;
    }
  }

  return 0;

} /* ps_sys_conf_set */


void ps_sys_confi_alloc_temp_data
(
  void
)
{

  LOG_MSG_FUNCTION_ENTRY_0("ps_sys_confi_alloc_temp_data() ");

  /* System Status Struct*/
   PS_SYSTEM_HEAP_MEM_ALLOC(conf_tech_all.system_status_type,
                           sizeof(ps_sys_system_status_type),
                           ps_sys_system_status_type*);
    
  if (NULL == conf_tech_all.system_status_type)
  {
    goto bail;
  }

  memset(conf_tech_all.system_status_type,
         0,
         sizeof(ps_sys_system_status_type));

  /* s101 status info type structure*/
   PS_SYSTEM_HEAP_MEM_ALLOC( conf_tech_3gpp2.s101_status_change_type, 
                             sizeof(ps_sys_3gpp2_s101_status_change_type),
                             s101_status_change_type*);

  if (NULL == conf_tech_3gpp2.s101_status_change_type)
  {
    goto bail;
  }

  memset(conf_tech_3gpp2.s101_status_change_type,
         0,
         sizeof(ps_sys_3gpp2_s101_status_change_type));

  return;

bail:
  DATA_ERR_FATAL("ps_sys_confi_alloc_temp_data() : Mem alloc failed");
  return;
} /* ps_sys_confi_alloc_temp_data() */

int ps_sys_confi_verify_tech_conf_name
(
  ps_sys_tech_enum_type          tech_type,
  ps_sys_conf_enum_type          conf_name
)
{
  LOG_MSG_FUNCTION_ENTRY_2("ps_sys_confi_verify_tech_conf_name() : tech %d "
                           "conf %d", tech_type, conf_name);

  if (conf_name >= PS_SYS_CONF_3GPP_CONF_MIN && 
      conf_name < PS_SYS_CONF_3GPP_INTERNAL_MAX)
  {
    if (tech_type != PS_SYS_TECH_3GPP)
    {
      LOG_MSG_INVALID_INPUT_1("ps_sys_confi_verify_tech_conf_name(): "
                              "3gpp conf %d ", conf_name);
      return -1;
    }
    return 0;
  }

  if (conf_name >= PS_SYS_CONF_3GPP2_CONF_MIN && 
      conf_name < PS_SYS_CONF_3GPP2_INTERNAL_MAX)
  {
    if (tech_type != PS_SYS_TECH_3GPP2)
    {
      LOG_MSG_INVALID_INPUT_1("ps_sys_confi_verify_tech_conf_name(): "
                              "3gpp2 conf %d", conf_name);
      return -1;
    }
    return 0;
  }

  if (conf_name >= PS_SYS_CONF_COMMON_CONF_MIN && 
      conf_name < PS_SYS_CONF_COMMON_INTERNAL_MAX)
  {
    if (conf_name == PS_SYS_CONF_SYSTEM_STATUS)
    {
      if (tech_type != PS_SYS_TECH_ALL)
      {
        LOG_MSG_INVALID_INPUT_1("ps_sys_confi_verify_tech_conf_name(): "
                                "common conf %d ", conf_name);
        return -1;
      }      
    }   
    return 0;
  }

  LOG_MSG_INVALID_INPUT_1("ps_sys_confi_verify_tech_conf_name(): "
                          "conf %d invalid", conf_name);

  return -1;
} /* ps_sys_confi_verify_tech_conf_name() */
