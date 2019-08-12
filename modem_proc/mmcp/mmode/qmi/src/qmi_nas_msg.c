/*=========================================================================== 
 
                         QMI_NAS_MSG.C 
 
DESCRIPTION 
 
 The Qualcomm Network Access Services Message MSM Interface source file. 
 
EXTERNALIZED FUNCTIONS 
 
  qmi_nas_0002_req_read 
    Reads QMI_NAS_SET_EVENT_REPORT_REQ 
      
  qmi_nas_0002_req_check 
    Checks QMI_NAS_SET_EVENT_REPORT_REQ for errors 
   
Copyright (c) 2010-2014 QUALCOMM Technologies Incorporated.
All Rights Reserved. 
Qualcomm Confidential and Proprietary. 
===========================================================================*/ 
/*=========================================================================== 
 
                      EDIT HISTORY FOR FILE 
 
  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/mmode/qmi/src/qmi_nas_msg.c#1 $ $Change: 11985193 $ $DateTime: 2016/12/13 08:00:05 $ $Author: mplcsds1 $
 
when        who    what, where, why 
--------    ---    ---------------------------------------------------------- 
05/15/12    aa     Centralized EONS support status message
05/21/10    hs     Initial version
===========================================================================*/ 
 
#include "mmcp_variation.h"
#include "comdef.h"
#include "customer.h"

#include "qmi_nas_msg.h" 
#include "dsm_pool.h" 
#include "msg_diag_service.h" 
#include "dsm_item.h" 
#include "ps_byte.h"
#include "qmi_nas_utils.h"
#include "qmi_svc_utils.h"
#include "network_access_service_v01.h"
#include "qm_util.h" 

/*=========================================================================== 
  FUNCTION QMI_NAS_0001_REQ_CHECK() 
 
  DESCRIPTION 
    Checks QMI_NAS_ABORT_REQ in struct nas_abort_req_msg_v01 for errors.
     
  RETURN VALUE 
    QMI_ERR_NONE_V01 for no error or appropriate error code.
     
  DEPENDENCIES 
    None 
 
  SIDE EFFECTS 
    None 
===========================================================================*/ 
qmi_error_type_v01 qmi_nas_0001_req_check (
  nas_abort_req_msg_v01 *p_msg
)
{
  if (p_msg == NULL)
  {
      return QMI_ERR_INTERNAL_V01;
  }

  if (p_msg->tx_id == 0)
  {
      return QMI_ERR_INVALID_TX_ID_V01;
  }

  return QMI_ERR_NONE_V01;
} /* qmi_nas_0001_req_check */
 
 
/*=========================================================================== 
  FUNCTION QMI_NAS_0002_REQ_CHECK() 
 
  DESCRIPTION 
    Checks QMI_NAS_SET_EVENT_REPORT_REQ in struct nas_set_event_report_req_msg_v01 for errors 
     
  RETURN VALUE 
    QMI_ERR_NONE for no error or appropriate error code. 
     
  DEPENDENCIES 
    None 
 
  SIDE EFFECTS 
    None 
===========================================================================*/ 
qmi_error_e_type qmi_nas_0002_req_check (nas_set_event_report_req_msg_v01 *p_msg) 
{ 
  // If ALL optional TLVs are absent return error 
  if ( !( p_msg->signal_strength_valid || p_msg->report_rf_band_info_valid || p_msg->report_reg_reject_valid || 
          p_msg->rssi_indicator_valid || p_msg->ecio_indicator_valid || p_msg->io_indicator_valid || 
          p_msg->sinr_indicator_valid || p_msg->report_error_rate_valid || p_msg->rsrq_indicator_valid ||
          p_msg->ecio_threshold_indicator_valid || p_msg->sinr_threshold_indicator_valid || p_msg->lte_snr_delta_indicator_valid ||
          p_msg->lte_rsrp_delta_indicator_valid ) ) 
  { 
    return QMI_ERR_MISSING_ARG; 
  } 
 
  /* Validate length of input tlv to ensure it is consistent with  
     num thresholds field specified in the tlv (2bytes + (n*1byte)) */ 
  if ( p_msg->signal_strength_valid ) 
  { 
    if (p_msg->signal_strength.report_signal_strength  != NAS_0002_REQ_DO_NOT_REPORT && 
        p_msg->signal_strength.report_signal_strength != NAS_0002_REQ_REPORT           ) 
      return QMI_ERR_INVALID_ARG; 
 
    if (p_msg->signal_strength.report_signal_strength_threshold_list_len == 0) 
      return QMI_ERR_NO_THRESHOLDS; 
 
    if (p_msg->signal_strength.report_signal_strength_threshold_list_len 
         > NAS_SIG_STRENGTH_THRESHOLD_LIST_MAX_V01) 
      return QMI_ERR_ARG_TOO_LONG; 
  } 
 
  if ( p_msg->report_rf_band_info_valid ) 
  { 
    if (p_msg->report_rf_band_info != NAS_0002_REQ_DO_NOT_REPORT && 
        p_msg->report_rf_band_info != NAS_0002_REQ_REPORT           ) 
      return QMI_ERR_INVALID_ARG; 
  } 
 
  if ( p_msg->report_reg_reject_valid ) 
  { 
    if (p_msg->report_reg_reject != NAS_0002_REQ_DO_NOT_REPORT && 
        p_msg->report_reg_reject != NAS_0002_REQ_REPORT           ) 
      return QMI_ERR_INVALID_ARG; 
  } 
 
  if ( p_msg->rssi_indicator_valid ) 
  { 
    if (p_msg->rssi_indicator.report_rssi != NAS_0002_REQ_DO_NOT_REPORT && 
        p_msg->rssi_indicator.report_rssi != NAS_0002_REQ_REPORT           ) 
      return QMI_ERR_INVALID_ARG; 
  } 
 
  if ( p_msg->ecio_indicator_valid ) 
  { 
    if (p_msg->ecio_indicator.report_ecio != NAS_0002_REQ_DO_NOT_REPORT && 
        p_msg->ecio_indicator.report_ecio != NAS_0002_REQ_REPORT           ) 
      return QMI_ERR_INVALID_ARG; 
  } 
 
  if ( p_msg->io_indicator_valid ) 
  { 
    if (p_msg->io_indicator.report_io != NAS_0002_REQ_DO_NOT_REPORT && 
        p_msg->io_indicator.report_io != NAS_0002_REQ_REPORT           ) 
      return QMI_ERR_INVALID_ARG; 
  } 
 
  if ( p_msg->sinr_indicator_valid ) 
  { 
    if (p_msg->sinr_indicator.report_sinr != NAS_0002_REQ_DO_NOT_REPORT && 
        p_msg->sinr_indicator.report_sinr != NAS_0002_REQ_REPORT           ) 
      return QMI_ERR_INVALID_ARG; 
  } 
 
  if ( p_msg->report_error_rate_valid ) 
  { 
    if (p_msg->report_error_rate != NAS_0002_REQ_DO_NOT_REPORT && 
        p_msg->report_error_rate != NAS_0002_REQ_REPORT           ) 
      return QMI_ERR_INVALID_ARG; 
  } 
 
  if ( p_msg->rsrq_indicator_valid ) 
  { 
    if (p_msg->rsrq_indicator.report_rsrq != NAS_0002_REQ_DO_NOT_REPORT && 
        p_msg->rsrq_indicator.report_rsrq != NAS_0002_REQ_REPORT           ) 
      return QMI_ERR_INVALID_ARG; 
  } 
 
  if ( p_msg->ecio_threshold_indicator_valid )
  {
    if ( p_msg->ecio_threshold_indicator.report_ecio != NAS_0002_REQ_DO_NOT_REPORT &&
         p_msg->ecio_threshold_indicator.report_ecio != NAS_0002_REQ_REPORT )
    {
      return QMI_ERR_INVALID_ARG;
    }

    if ( p_msg->ecio_threshold_indicator.threshold_list_len == 0 )
    {
      return QMI_ERR_NO_THRESHOLDS;
    }

    if ( p_msg->ecio_threshold_indicator.threshold_list_len > NAS_ECIO_THRESHOLD_LIST_MAX_V01 )
    {
      return QMI_ERR_ARG_TOO_LONG;
    }

  }
 
  if ( p_msg->sinr_threshold_indicator_valid )
  {
    if ( p_msg->sinr_threshold_indicator.report_sinr != NAS_0002_REQ_DO_NOT_REPORT &&
         p_msg->sinr_threshold_indicator.report_sinr != NAS_0002_REQ_REPORT )
    {
      return QMI_ERR_INVALID_ARG;
    }

    if ( p_msg->sinr_threshold_indicator.threshold_list_len == 0 )
    {
      return QMI_ERR_NO_THRESHOLDS;
    }

    if ( p_msg->sinr_threshold_indicator.threshold_list_len  > NAS_SINR_THRESHOLD_LIST_MAX_V01 )
    {
      return QMI_ERR_ARG_TOO_LONG;
    }

  }
  
  if ( p_msg->lte_snr_delta_indicator_valid ) 
  { 
    if (p_msg->lte_snr_delta_indicator.report_lte_snr != NAS_0002_REQ_DO_NOT_REPORT && 
        p_msg->lte_snr_delta_indicator.report_lte_snr != NAS_0002_REQ_REPORT           ) 
    {
      return QMI_ERR_INVALID_ARG; 
    }
  } 

  if ( p_msg->lte_rsrp_delta_indicator_valid ) 
  { 
    if (p_msg->lte_rsrp_delta_indicator.report_lte_rsrp != NAS_0002_REQ_DO_NOT_REPORT && 
        p_msg->lte_rsrp_delta_indicator.report_lte_rsrp != NAS_0002_REQ_REPORT           ) 
    {
      return QMI_ERR_INVALID_ARG; 
    }
  }   
 
  return QMI_ERR_NONE; 
} /* qmi_nas_0002_req_check */

/*=========================================================================== 
  FUNCTION QMI_NAS_0003_REQ_CHECK() 
 
  DESCRIPTION 
    Checks QMI_NAS_INDICATION_REGISTER in struct nas_indication_register_req_msg_v01 for errors.
     
  RETURN VALUE 
    QMI_ERR_NONE_V01 for no error or appropriate error code.
     
  DEPENDENCIES 
    None 
 
  SIDE EFFECTS 
    None 
===========================================================================*/ 
qmi_error_type_v01 qmi_nas_0003_req_check (
  nas_indication_register_req_msg_v01 *p_msg
)
{
  if (p_msg == NULL)
  {
    return QMI_ERR_INTERNAL_V01;
  }

  /*-------------------------------------------------------------------------
    If ALL optional TLVs are absent return error
  -------------------------------------------------------------------------*/
  if (!(p_msg->reg_sys_sel_pref_valid      || p_msg->reg_ddtm_events_valid        || p_msg->req_serving_system_valid  ||
        p_msg->dual_standby_pref_valid     || p_msg->subscription_info_valid      || p_msg->reg_network_time_valid    ||
        p_msg->sys_info_valid              || p_msg->sig_info_valid               || p_msg->err_rate_valid            ||
        p_msg->reg_hdr_uati_valid          || p_msg->reg_hdr_session_close_valid  || p_msg->reg_managed_roaming_valid ||
        p_msg->reg_current_plmn_name_valid || p_msg->reg_embms_status_valid       || p_msg->reg_rf_band_info_valid    ||
        p_msg->network_reject_valid        || p_msg->reg_operator_name_data_valid || p_msg->reg_csp_plmn_mode_bit_valid ||
        p_msg->reg_rtre_cfg_valid          || p_msg->reg_ims_pref_status_valid    || p_msg->reg_e911_state_ready_status_valid ||
        p_msg->reg_lte_sib16_network_time_valid ||
        p_msg->reg_lte_cphy_ca_valid       || p_msg->reg_subscription_change_valid ||
        p_msg->reg_ssac_info_valid         || p_msg->reg_emm_t3402_change_valid   || p_msg->reg_acb_info_change_valid ||
        p_msg->reg_t3346_timer_status_change_valid  || p_msg->reg_call_mode_change_valid || p_msg->reg_gcell_info_ind_valid)
      )
  {
    return QMI_ERR_MISSING_ARG_V01;
  }

  return QMI_ERR_NONE_V01;
} /* qmi_nas_0003_req_check */

/*=========================================================================== 
  FUNCTION QMI_NAS_0020_REQ_CHECK() 

  DESCRIPTION 
  Checks QMI_NAS_GET_SIGNAL_STRENGTH in struct nas_0020_req_s for errors 

  RETURN VALUE 
  QMI_ERR_NONE for no error or appropriate error code. 

  DEPENDENCIES 
  None 

  SIDE EFFECTS 
  None 
  ===========================================================================*/ 
qmi_error_e_type qmi_nas_0020_req_check (
    nas_get_signal_strength_req_msg_v01 *p_msg
)
{

  if (p_msg == NULL)
  {
    return QMI_ERR_INTERNAL;
  }

  /* This function does nothing, but it remains as a placeholder should
   * something be added in the future or a check need be implemented
   * in a future version of QMI                                         */

  return QMI_ERR_NONE;
} /* qmi_nas_0020_req_check */

/*=========================================================================== 
  FUNCTION qmi_nas_0021_req_check() 
 
  DESCRIPTION 
    Checks QMI_NAS_PERFORM_NETWORK_SCAN in nas_perform_network_scan_req_msg_v01 for errors 
     
  RETURN VALUE 
    QMI_ERR_NONE for no error or appropriate error code. 
     
  DEPENDENCIES 
    None 
 
  SIDE EFFECTS 
    None 
===========================================================================*/
qmi_error_type_v01 qmi_nas_0021_req_check( nas_perform_network_scan_req_msg_v01 *p_msg)
{
  if ( !p_msg )
  {
    return QMI_ERR_INTERNAL_V01;
  }

  if ( p_msg->network_type_valid )
  {
    if ( p_msg->network_type != 0 &&
         !(p_msg->network_type & NAS_NETWORK_TYPE_GSM_ONLY_V01) &&
         !(p_msg->network_type & NAS_NETWORK_TYPE_WCDMA_ONLY_V01) &&
         !(p_msg->network_type & NAS_NETWORK_TYPE_LTE_ONLY_V01) &&
         !(p_msg->network_type & NAS_NETWORK_TYPE_TDSCDMA_ONLY_V01) )
    {
      return QMI_ERR_INVALID_ARG_V01;
    }
  }

  if ( p_msg->scan_type_valid )
  {
    if ( p_msg->scan_type != NAS_SCAN_TYPE_PLMN_V01 &&
         p_msg->scan_type != NAS_SCAN_TYPE_CSG_V01  &&
         p_msg->scan_type != NAS_SCAN_TYPE_PCI_V01 &&
         p_msg->scan_type != NAS_SCAN_TYPE_MODE_PREF_V01
    )
    {
      return QMI_ERR_INVALID_ARG_V01;
    }
  }

  return QMI_ERR_NONE_V01;
} /* qmi_nas_0021_req_check */


/*=========================================================================== 
  FUNCTION qmi_nas_0022_req_read() 
 
  DESCRIPTION 
    Reads QMI_NAS_INITIATE_NETWORK_REGISTER_REQ into struct nas_0023_req_s 
     
  RETURN VALUE 
    QMI_ERR_NONE for successful read. For failure, returns appropriate error 
    value. 
     
  DEPENDENCIES 
    None 
 
  SIDE EFFECTS 
    None 
===========================================================================*/
qmi_error_e_type qmi_nas_0022_req_read( struct nas_0022_req_s *p_msg, dsm_item_type **p_sdu_in)
{
  uint8                   type;
  uint16                  len;
  uint16                  expected_len;
  void                    *value;

  struct nas_0022_req_p_s nas_0022_req_p;

  memset( &nas_0022_req_p, 0x00, sizeof(nas_0022_req_p));

  if ( !p_msg || !p_sdu_in )
  {
    return QMI_ERR_INTERNAL;
  }

  while (*p_sdu_in)
  {
    if ( !qmi_svc_get_tl( p_sdu_in, &type, &len ) )
    {
      continue;
    }

    expected_len = 0;
    value = NULL;

    switch (type)
    {
      case NAS_0022_REQ_T01:
        p_msg->t01_valid = TRUE;
        value = &nas_0022_req_p.t01;
        expected_len = sizeof(nas_0022_req_p.t01);
        break;

      case NAS_0022_REQ_T10:
        p_msg->t10_valid = TRUE;
        value = &nas_0022_req_p.t10;
        expected_len = sizeof(nas_0022_req_p.t10);
        break;

      case NAS_0022_REQ_T11:
        p_msg->t11_valid = TRUE;
        value = &nas_0022_req_p.t11;
        expected_len = sizeof(nas_0022_req_p.t11);
        break;

      case NAS_0022_REQ_T12:
        p_msg->t12_valid = TRUE;
        value = &nas_0022_req_p.t12;
        expected_len = sizeof(nas_0022_req_p.t12);
        break;

      default:
        QM_MSG_ERROR_1("Unrecognized TLV type (%d)", type);
        break;
    }

    if ((expected_len != 0) && (expected_len != len))
    {
      QM_MSG_ERROR_2("Invalid TLV len (%d) for type (%d)", len, type);
      return QMI_ERR_MALFORMED_MSG;
    }

    if (len != dsm_pullup( p_sdu_in, value, len ))
    {
      QM_MSG_ERROR("Invalid length in TLV");
      return QMI_ERR_MALFORMED_MSG;
    }
  }

  if (p_msg->t01_valid)
  {
    memscpy(&p_msg->t01.register_action, sizeof(nas_0022_req_p.t01.register_action),
            &nas_0022_req_p.t01.register_action, sizeof(nas_0022_req_p.t01.register_action));
  }
  else
  {
    return QMI_ERR_MISSING_ARG;
  }

  if (p_msg->t10_valid)
  {
    memscpy((void*)&p_msg->t10.mobile_country_code, sizeof(nas_0022_req_p.t10.mobile_country_code), 
            (void*)&nas_0022_req_p.t10.mobile_country_code, sizeof(nas_0022_req_p.t10.mobile_country_code));
    memscpy((void*)&p_msg->t10.mobile_network_code, sizeof(nas_0022_req_p.t10.mobile_network_code),
            (void*)&nas_0022_req_p.t10.mobile_network_code, sizeof(nas_0022_req_p.t10.mobile_network_code));
    memscpy(&p_msg->t10.radio_access_technology, sizeof(nas_0022_req_p.t10.radio_access_technology),
            &nas_0022_req_p.t10.radio_access_technology, sizeof(nas_0022_req_p.t10.radio_access_technology));
  }

  if (p_msg->t11_valid)
  {
    memscpy(&p_msg->t11.change_duration, sizeof(nas_0022_req_p.t11.change_duration), 
            &nas_0022_req_p.t11.change_duration, sizeof(nas_0022_req_p.t11.change_duration));
  }
  
  if (p_msg->t12_valid)
  {
    memscpy(&p_msg->t12.mnc_includes_pcs_digit, sizeof(nas_0022_req_p.t12.mnc_includes_pcs_digit),
            &nas_0022_req_p.t12.mnc_includes_pcs_digit, sizeof(nas_0022_req_p.t12.mnc_includes_pcs_digit));
  }  

  return QMI_ERR_NONE;
}

/*=========================================================================== 
  FUNCTION qmi_nas_0022_req_check() 
 
  DESCRIPTION 
    Checks QMI_NAS_INITIATE_NETWORK_REGISTER_REQ in struct nas_0022_req_s for errors 
     
  RETURN VALUE 
    QMI_ERR_NONE for no error or appropriate error code. 
     
  DEPENDENCIES 
    None 
 
  SIDE EFFECTS 
    None 
===========================================================================*/
qmi_error_e_type qmi_nas_0022_req_check( struct nas_0022_req_s *p_msg)
{
  if ( !p_msg )
  {
    return QMI_ERR_INTERNAL;
  }

  if ( p_msg->t01_valid )
  {
    if ( p_msg->t01.register_action == QMI_NAS_REGISTER_ACTION_MANUAL )
    {
      if ( p_msg->t10_valid )
      {
        if ( p_msg->t10.radio_access_technology != QMI_NAS_RADIO_IF_GSM &&
             p_msg->t10.radio_access_technology != QMI_NAS_RADIO_IF_UMTS &&
             p_msg->t10.radio_access_technology != QMI_NAS_RADIO_IF_LTE &&
             p_msg->t10.radio_access_technology != QMI_NAS_RADIO_IF_NO_CHANGE )
        {
          return QMI_ERR_OP_DEVICE_UNSUPPORTED;
        }
      }
      else
      {
        return QMI_ERR_MISSING_ARG;
      }
    }
    else if ( p_msg->t01.register_action == QMI_NAS_REGISTER_ACTION_AUTOMATIC )
    {
      // do nothing. just ignore t10
    }
    else
    {
      return QMI_ERR_INVALID_REGISTER_ACTION;
    }

    if ( p_msg->t11_valid )
    {
      if ( p_msg->t11.change_duration != QMI_NAS_CHANGE_DURATION_POWER_CYCLE &&
           p_msg->t11.change_duration != QMI_NAS_CHANGE_DURATION_PERMANENT )
      {
        return QMI_ERR_INVALID_ARG;
      }
    }
    if ( p_msg->t12_valid )
    {
      if (p_msg->t12.mnc_includes_pcs_digit != FALSE &&
          p_msg->t12.mnc_includes_pcs_digit != TRUE )
      {
        return QMI_ERR_INVALID_ARG;
      }
    }
  }
  else
  {
    return QMI_ERR_MISSING_ARG;
  }

  return QMI_ERR_NONE;
}

/*=========================================================================== 
  FUNCTION QMI_NAS_0022_RSP_WRITE() 

  DESCRIPTION 
  Writes QMI_NAS_INITIATE_NETWORK_REGISTER_RESP response in struct nas_0022_rsp_s

  RETURN VALUE 
  QMI_ERR_NONE for no error or appropriate error code. 

  DEPENDENCIES 
  None 

  SIDE EFFECTS 
  None 
  ===========================================================================*/
qmi_error_e_type qmi_nas_0022_rsp_write( struct nas_0022_rsp_s *p_msg, dsm_item_type **p_response)
{
  struct nas_0022_rsp_p_s nas_0022_rsp_p;

  if ( !p_msg || !p_response )
  {
    return QMI_ERR_INTERNAL;
  }

  memset(&nas_0022_rsp_p, 0, sizeof(nas_0022_rsp_p));

  if (p_msg->t02_valid)
  {
    memscpy((void*)&nas_0022_rsp_p.t02.result_code, sizeof(p_msg->t02.result_code),
            (void*)&p_msg->t02.result_code, sizeof(p_msg->t02.result_code));
    memscpy((void*)&nas_0022_rsp_p.t02.error_code, sizeof(p_msg->t02.error_code),
            (void*)&p_msg->t02.error_code, sizeof(p_msg->t02.error_code));

    if ( !qmi_svc_put_param_tlv( p_response, NAS_0022_RSP_T02, sizeof(nas_0022_rsp_p.t02), &nas_0022_rsp_p.t02 ) )
    {
      qmi_err_write(NAS_0022_RSP_T02, &nas_0022_rsp_p.t02);
      dsm_free_packet(p_response);
      return QMI_ERR_NO_MEMORY;
    }
  }

  return QMI_ERR_NONE;
}

/*=========================================================================== 
  FUNCTION QMI_NAS_0023_REQ_READ() 
 
  DESCRIPTION 
    Reads QMI_NAS_INITIATE_ATTACH into struct nas_0023_req_s 
     
  RETURN VALUE 
    QMI_ERR_NONE for successful read. For failure, returns appropriate error 
    value. 
     
  DEPENDENCIES 
    None 
 
  SIDE EFFECTS 
    None 
===========================================================================*/ 
qmi_error_e_type qmi_nas_0023_req_read (
  struct nas_0023_req_s *p_msg,
  dsm_item_type         **sdu_in
)
{
  uint8                   type;
  uint16                  len;
  uint16                  expected_len;
  void                    *value;

  struct nas_0023_req_p_s nas_0023_req_p;

  if (p_msg == NULL || sdu_in == NULL)
  {
    return QMI_ERR_INTERNAL;
  }

  memset(&nas_0023_req_p, 0, sizeof(nas_0023_req_p));

  while (*sdu_in)
  {
    if ( !qmi_svc_get_tl( sdu_in, &type, &len ) )
    {
      continue;
    }

    expected_len = 0;
    value = NULL;
    switch (type)
    {
      case NAS_0023_REQ_T10:
        p_msg->t10_valid = TRUE;
        value = &nas_0023_req_p.t10;
        expected_len = sizeof(nas_0023_req_p.t10);
        break;

      default:
        QM_MSG_ERROR_1("Unrecognized TLV type (%d)", type);
        break;
    }

    if ((expected_len != 0) && (expected_len != len))
    {
      QM_MSG_ERROR_2("Invalid TLV len (%d) for type (%d)", len, type);
      return QMI_ERR_MALFORMED_MSG;
    }

    if (len != dsm_pullup( sdu_in, value, len ))
    {
      QM_MSG_ERROR("Invalid length in TLV");
      return QMI_ERR_MALFORMED_MSG;
    }
  }

  if (p_msg->t10_valid)
  {
    memscpy(&p_msg->t10.ps_attach_action, sizeof(nas_0023_req_p.t10.ps_attach_action),
            &nas_0023_req_p.t10.ps_attach_action, sizeof(nas_0023_req_p.t10.ps_attach_action));
  }
  else
  {
    return QMI_ERR_MISSING_ARG;
  }

  return QMI_ERR_NONE;
} /* qmi_nas_0023_req_read */


/*=========================================================================== 
  FUNCTION QMI_NAS_0023_REQ_CHECK() 
 
  DESCRIPTION 
    Checks QMI_NAS_INITIATE_ATTACH in struct nas_0023_req_s for errors 
     
  RETURN VALUE 
    QMI_ERR_NONE for no error or appropriate error code. 
     
  DEPENDENCIES 
    None 
 
  SIDE EFFECTS 
    None 
===========================================================================*/ 
qmi_error_e_type qmi_nas_0023_req_check (
  struct nas_0023_req_s *p_msg
)
{
  if (p_msg == NULL)
  {
    return QMI_ERR_INTERNAL;
  }

  if (p_msg->t10.ps_attach_action != NASI_PS_ATTACHED &&
      p_msg->t10.ps_attach_action != NASI_PS_DETACHED)
  {
    return QMI_ERR_INVALID_PS_ATTACH_ACTION;
  }
      

  return QMI_ERR_NONE;
} /* qmi_nas_0023_req_check */


/*=========================================================================== 
  FUNCTION QMI_NAS_002A_REQ_READ() 

  DESCRIPTION 
  Reads QMI_NAS_SET_TECHNOLOGY_PREFERENCE into struct nas_002A_req_s

  RETURN VALUE 
  QMI_ERR_NONE for successful read. For failure, returns appropriate error 
  value. 

  DEPENDENCIES 
  None 

  SIDE EFFECTS 
  None 
  ===========================================================================*/
qmi_error_e_type qmi_nas_002A_req_read  ( struct nas_002A_req_s *p_msg, dsm_item_type **sdu_in)
{
  uint8                   type;
  uint16                  len;
  uint16                  expected_len;
  void                   *value;

  struct nas_002A_req_p_s nas_002A_req_p;

  if ( !p_msg || !sdu_in )
  {
    return QMI_ERR_INTERNAL;
  }

  memset(&nas_002A_req_p, 0, sizeof(nas_002A_req_p));

  while (*sdu_in)
  {
    if ( !qmi_svc_get_tl( sdu_in, &type, &len ) )
    {
      continue;
    }

    expected_len = 0;
    value = NULL;
    switch (type)
    {
      case NAS_002A_REQ_T01:
        p_msg->t01_valid = TRUE;
        value = &nas_002A_req_p.t01;
        expected_len = sizeof(nas_002A_req_p.t01);
        break;

      default:
        QM_MSG_ERROR_1("Unrecognized TLV type (%d)", type);
        break;
    }

    if ((expected_len != 0) && (expected_len != len))
    {
      QM_MSG_ERROR_2("Invalid TLV len (%d) for type (%d)", len, type);
      return QMI_ERR_MALFORMED_MSG;
    }

    if (len != dsm_pullup( sdu_in, value, len ))
    {
      qmi_nas_invalid_len_tlv();
      return QMI_ERR_MALFORMED_MSG;
    }
  }

  if ( p_msg->t01_valid )
  {
    memscpy((void*)&p_msg->t01.technology_pref, sizeof(nas_002A_req_p.t01.technology_pref),
            (void*)&nas_002A_req_p.t01.technology_pref, sizeof(nas_002A_req_p.t01.technology_pref));
    memscpy(&p_msg->t01.duration, sizeof(nas_002A_req_p.t01.duration),
            &nas_002A_req_p.t01.duration, sizeof(nas_002A_req_p.t01.duration));
  }
  else
  {
    return QMI_ERR_MISSING_ARG;
  }

  return QMI_ERR_NONE;
}

/*=========================================================================== 
  FUNCTION QMI_NAS_002A_REQ_CHECK() 

  DESCRIPTION 
  Checks QMI_NAS_SET_TECHNOLOGY_PREFERENCE in struct nas_002A_req_s for errors 

  RETURN VALUE 
  QMI_ERR_NONE for no error or appropriate error code. 

  DEPENDENCIES 
  None 

  SIDE EFFECTS 
  None 
  ===========================================================================*/
qmi_error_e_type qmi_nas_002A_req_check ( struct nas_002A_req_s *p_msg)
{
  uint16 tech_pref;
  
  if ( !p_msg )
  {
    return QMI_ERR_INTERNAL;
  }

  tech_pref = p_msg->t01.technology_pref & 0x003F; // All unlisted bits are reserved for future use and will be ignored

  if ( (tech_pref & 0x0003) == 0x0003 ) // Bits 0 and 1 are exclusive; only one may be set at a time
  {
    return QMI_ERR_INVALID_ARG;
  }
  else if ( (tech_pref & 0x0003) == 0x0000 && tech_pref & 0x003C ) // tech pref set to auto but have other bits set
  {
    return QMI_ERR_OP_DEVICE_UNSUPPORTED;
  }

  if ( p_msg->t01.duration != QMI_NAS_TECH_PREF_DURATION_PERMANENT &&
       p_msg->t01.duration != QMI_NAS_TECH_PREF_DURATION_POWER_CYCLE )
  {
    return QMI_ERR_INVALID_ARG;
  }

  return QMI_ERR_NONE;
}

/*=========================================================================== 
  FUNCTION QMI_NAS_002A_RSP_WRITE() 

  DESCRIPTION 
  Writes QMI_NAS_SET_TECHNOLOGY_PREFERENCE_RESP response in struct nas_002A_rsp_s

  RETURN VALUE 
  QMI_ERR_NONE for no error or appropriate error code. 

  DEPENDENCIES 
  None 

  SIDE EFFECTS 
  None 
  ===========================================================================*/
qmi_error_e_type qmi_nas_002A_rsp_write( struct nas_002A_rsp_s *p_msg, dsm_item_type **p_response)
{
  struct nas_002A_rsp_p_s nas_002A_rsp_p;

  if ( !p_msg || !p_response )
  {
    return QMI_ERR_INTERNAL;
  }

  memset(&nas_002A_rsp_p, 0, sizeof(nas_002A_rsp_p));

  if (p_msg->t02_valid)
  {
    memscpy((void*)&nas_002A_rsp_p.t02.result_code, sizeof(p_msg->t02.result_code),
            (void*)&p_msg->t02.result_code, sizeof(p_msg->t02.result_code));
    memscpy((void*)&nas_002A_rsp_p.t02.error_code, sizeof(p_msg->t02.error_code),
            (void*)&p_msg->t02.error_code, sizeof(p_msg->t02.error_code));

    if ( !qmi_svc_put_param_tlv( p_response, NAS_002A_RSP_T02, sizeof(nas_002A_rsp_p.t02), &nas_002A_rsp_p.t02 ) )
    {
      qmi_err_write(NAS_002A_RSP_T02, &nas_002A_rsp_p.t02);
      dsm_free_packet(p_response);
      return QMI_ERR_NO_MEMORY;
    }
  }

  return QMI_ERR_NONE;
}

/*=========================================================================== 
  FUNCTION QMI_NAS_002B_RSP_WRITE() 

  DESCRIPTION 
  Writes QMI_NAS_SET_TECHNOLOGY_PREFERENCE_RESP response in struct nas_002B_rsp_s

  RETURN VALUE 
  QMI_ERR_NONE for no error or appropriate error code. 

  DEPENDENCIES 
  None 

  SIDE EFFECTS 
  None 
  ===========================================================================*/
qmi_error_e_type qmi_nas_002B_rsp_write( struct nas_002B_rsp_s *p_msg, dsm_item_type **p_response)
{
  struct nas_002B_rsp_p_s nas_002B_rsp_p;

  if ( !p_msg || !p_response )
  {
    return QMI_ERR_INTERNAL;
  }

  memset(&nas_002B_rsp_p, 0, sizeof(nas_002B_rsp_p));

  if (p_msg->t10_valid)
  {
    memscpy((void*)&nas_002B_rsp_p.t10.persistent_technology_pref, sizeof(p_msg->t10.persistent_technology_pref),
            (void*)&p_msg->t10.persistent_technology_pref, sizeof(p_msg->t10.persistent_technology_pref));

    if ( !qmi_svc_put_param_tlv( p_response, NAS_002B_RSP_T10, sizeof(nas_002B_rsp_p.t10), &nas_002B_rsp_p.t10 ) )
    {
      qmi_err_write(NAS_002B_RSP_T10, &nas_002B_rsp_p.t10);
      dsm_free_packet(p_response);
      return QMI_ERR_NO_MEMORY;
    }
  }

  if (p_msg->t02_valid)
  {
    memscpy((void*)&nas_002B_rsp_p.t02.result_code, sizeof(p_msg->t02.result_code),
            (void*)&p_msg->t02.result_code, sizeof(p_msg->t02.result_code));
    memscpy((void*)&nas_002B_rsp_p.t02.error_code, sizeof(p_msg->t02.error_code),
            (void*)&p_msg->t02.error_code, sizeof(p_msg->t02.error_code));

    if ( !qmi_svc_put_param_tlv( p_response, NAS_002B_RSP_T02, sizeof(nas_002B_rsp_p.t02), &nas_002B_rsp_p.t02 ) )
    {
      qmi_err_write(NAS_002B_RSP_T02, &nas_002B_rsp_p.t02);
      dsm_free_packet(p_response);
      return QMI_ERR_NO_MEMORY;
    }
  }

  if (p_msg->t01_valid)
  {
    memscpy((void*)&nas_002B_rsp_p.t01.active_technology_pref, sizeof(p_msg->t01.active_technology_pref),
            (void*)&p_msg->t01.active_technology_pref, sizeof(p_msg->t01.active_technology_pref));
    memscpy((void*)&nas_002B_rsp_p.t01.duration, sizeof(p_msg->t01.duration),
            (void*)&p_msg->t01.duration, sizeof(p_msg->t01.duration));

    if ( !qmi_svc_put_param_tlv( p_response, NAS_002B_RSP_T01, sizeof(nas_002B_rsp_p.t01), &nas_002B_rsp_p.t01 ) )
    {
      qmi_err_write(NAS_002B_RSP_T01, &nas_002B_rsp_p.t01);
      dsm_free_packet(p_response);
      return QMI_ERR_NO_MEMORY;
    }
  }

  return QMI_ERR_NONE;
}

/*=========================================================================== 
  FUNCTION QMI_NAS_002C_RSP_WRITE() 

  DESCRIPTION 
  Writes QMI_NAS_GET_ACCOLC response in struct nas_002C_rsp_s

  RETURN VALUE 
  QMI_ERR_NONE for no error or appropriate error code. 

  DEPENDENCIES 
  None 

  SIDE EFFECTS 
  None 
  ===========================================================================*/ 
qmi_error_e_type qmi_nas_002C_rsp_write (
    struct nas_002C_rsp_s *p_msg,
    dsm_item_type         **p_response
)
{
  struct nas_002C_rsp_p_s nas_002C_rsp_p;

  if (p_msg == NULL || p_response == NULL)
  {
    return QMI_ERR_INTERNAL;
  }

  memset(&nas_002C_rsp_p, 0, sizeof(nas_002C_rsp_p));

  if (p_msg->t01_valid)
  {
    memscpy((void*) &nas_002C_rsp_p.t01.accolc, sizeof(nas_002C_rsp_p.t01.accolc), 
            (void*) &p_msg->t01.accolc, sizeof(nas_002C_rsp_p.t01.accolc));

    if (FALSE == qmi_svc_put_param_tlv(p_response,
                                       NAS_002C_RSP_ACCOLC,
                                       sizeof(nas_002C_rsp_p.t01),
                                       &nas_002C_rsp_p.t01))
    {
      qmi_err_write(NAS_002C_RSP_ACCOLC, &nas_002C_rsp_p.t01);
      dsm_free_packet(p_response);
      return QMI_ERR_NO_MEMORY;
    }
  }

  return QMI_ERR_NONE;
} /* qmi_nas_002C_rsp_write */

/*=========================================================================== 
  FUNCTION QMI_NAS_002D_REQ_READ() 

  DESCRIPTION 
  Reads QMI_NAS_SET_ACCOLC into struct nas_002D_req_read

  RETURN VALUE 
  QMI_ERR_NONE for successful read. For failure, returns appropriate error 
  value. 

  DEPENDENCIES 
  None 

  SIDE EFFECTS 
  None 
  ===========================================================================*/ 
qmi_error_e_type qmi_nas_002D_req_read (
    struct nas_002D_req_s *p_msg,
    dsm_item_type         **p_sdu_in
)
{

  struct nas_002D_req_p_s   nas_002D_req_p;

  uint8                     prm_type;
  uint16                    len;
  uint16                    expected_len;
  void *                    value;

  if ( p_msg == NULL || p_sdu_in == NULL)
  {
    return QMI_ERR_INTERNAL;
  }

  memset(&nas_002D_req_p, 0, sizeof(nas_002D_req_p));

  while (*p_sdu_in)
  {
    if (!qmi_svc_get_tl(p_sdu_in, &prm_type, &len))
    {
      continue;
    }

    // special value 0 = variable length or don't care (unrecognzied TLV)
    expected_len = 0;
    value = NULL;
    switch (prm_type)
    {
      case NAS_002D_REQ_ACCOLC:
        expected_len = sizeof(nas_002D_req_p.t01);
        value = &nas_002D_req_p.t01;
        p_msg->t01_valid = TRUE;
        break;
        
      default:
        QM_MSG_ERROR_1("Unrecognized TLV type (%d)", prm_type);
        break;
    }

    if ((expected_len != 0) && (expected_len != len))
    {
      QM_MSG_ERROR_2("Invalid TLV len (%d) for type (%d)", len, prm_type);
      return QMI_ERR_MALFORMED_MSG;
    }

    if (len != dsm_pullup( p_sdu_in, value, len ))
    {
        qmi_nas_invalid_len_tlv();
      return QMI_ERR_MALFORMED_MSG;
    }
  }

  if (p_msg->t01_valid)
  {
    memscpy((void*) p_msg->t01.spc, sizeof(nas_002D_req_p.t01.spc),
            (void*) nas_002D_req_p.t01.spc, sizeof(nas_002D_req_p.t01.spc));
    memscpy((void*) &p_msg->t01.accolc, sizeof(nas_002D_req_p.t01.accolc),
            (void*) &nas_002D_req_p.t01.accolc, sizeof(nas_002D_req_p.t01.accolc));
  }
  else
  {
    return QMI_ERR_MISSING_ARG;
  }

  return QMI_ERR_NONE;
} /* qmi_nas_002D_req_read */

/*=========================================================================== 
  FUNCTION QMI_NAS_002D_REQ_CHECK() 

  DESCRIPTION 
  Checks QMI_NAS_SET_ACCOLC in struct nas_002D_req_s for errors 

  RETURN VALUE 
  QMI_ERR_NONE for no error or appropriate error code. 

  DEPENDENCIES 
  None 

  SIDE EFFECTS 
  None 
  ===========================================================================*/ 
qmi_error_e_type qmi_nas_002D_req_check (
    struct nas_002D_req_s *p_msg
)
{
  if (p_msg == NULL)
  {
    return QMI_ERR_INTERNAL;
  }

  if (p_msg->t01.accolc > NAS_002D_REQ_ACCOLC_MAX)
  {
    QM_MSG_ERROR("Provided ACCOLC value is above max allowed");
    return QMI_ERR_INVALID_ARG;
  }

  return QMI_ERR_NONE;
} /* qmi_nas_002D_req_check */
 
/*=========================================================================== 
  FUNCTION QMI_NAS_002E_RSP_WRITE() 

  DESCRIPTION 
  Writes QMI_NAS_GET_NETWORK_SYSTEM_PREFERENCE_RESP response in struct nas_002E_rsp_s

  RETURN VALUE 
  QMI_ERR_NONE for no error or appropriate error code. 

  DEPENDENCIES 
  None 

  SIDE EFFECTS 
  None 
  ===========================================================================*/
qmi_error_e_type qmi_nas_002E_rsp_write( struct nas_002E_rsp_s *p_msg, dsm_item_type **p_response)
{
  struct nas_002E_rsp_p_s nas_002E_rsp_p;

  if ( !p_msg || !p_response )
  {
    return QMI_ERR_INTERNAL;
  }

  memset(&nas_002E_rsp_p, 0, sizeof(nas_002E_rsp_p));

  if (p_msg->t02_valid)
  {
    memscpy((void*)&nas_002E_rsp_p.t02.result_code, sizeof(p_msg->t02.result_code),
            (void*)&p_msg->t02.result_code, sizeof(p_msg->t02.result_code));
    memscpy((void*)&nas_002E_rsp_p.t02.error_code, sizeof(p_msg->t02.error_code),
            (void*)&p_msg->t02.error_code, sizeof(p_msg->t02.error_code));

    if ( !qmi_svc_put_param_tlv( p_response, NAS_002E_RSP_T02, sizeof(nas_002E_rsp_p.t02), &nas_002E_rsp_p.t02 ) )
    {
      qmi_err_write(NAS_002E_RSP_T02, &nas_002E_rsp_p.t02);
      dsm_free_packet(p_response);
      return QMI_ERR_NO_MEMORY;
    }
  }

  if (p_msg->t01_valid)
  {
    memscpy((void*)&nas_002E_rsp_p.t01.sys_pref, sizeof(p_msg->t01.sys_pref),
            (void*)&p_msg->t01.sys_pref, sizeof(p_msg->t01.sys_pref));

    if ( !qmi_svc_put_param_tlv( p_response, NAS_002E_RSP_T01, sizeof(nas_002E_rsp_p.t01), &nas_002E_rsp_p.t01 ) )
    {
      qmi_err_write(NAS_002E_RSP_T01, &nas_002E_rsp_p.t01);
      dsm_free_packet(p_response);
      return QMI_ERR_NO_MEMORY;
    }
  }

  return QMI_ERR_NONE;
}

/*=========================================================================== 
  FUNCTION QMI_NAS_0030_REQ_CHECK() 
 
  DESCRIPTION 
    Checks QMI_NAS_SET_DEVICE_CONFIG in struct nas_0030_req_s for errors 
     
  RETURN VALUE 
    QMI_ERR_NONE for no error or appropriate error code. 
     
  DEPENDENCIES 
    None 
 
  SIDE EFFECTS 
    None 
===========================================================================*/ 
qmi_error_e_type qmi_nas_0030_req_check (nas_set_device_config_req_msg_v01 *p_msg)
{
  int i;

  if (p_msg == NULL)
  {
    return QMI_ERR_INTERNAL;
  }

  if( !p_msg->spc_valid && !p_msg->force_hdr_rev0_valid &&
      !p_msg->hdr_scp_config_valid && !p_msg->roam_pref_valid)
  {
    return QMI_ERR_MISSING_ARG;
  }

  if (p_msg->spc_valid)
  {
    for (i = 0; i < 6; i++)
    {
      if (p_msg->spc[i] < '0' || p_msg->spc[i] > '9')
      {
        return QMI_ERR_INVALID_ARG;
      }
    }
  }

  if (p_msg->roam_pref_valid)
  {
    if (p_msg->roam_pref != NAS_ROAM_CONFIG_PREF_AUTO_V01 &&
       p_msg->roam_pref != NAS_ROAM_CONFIG_PREF_HOME_ONLY_V01 &&
       p_msg->roam_pref != NAS_ROAM_CONFIG_PREF_ROAM_ONLY_V01 &&
       p_msg->roam_pref != NAS_ROAM_CONFIG_PREF_HOME_AND_AFFILIATE_V01)
    {
      return QMI_ERR_INVALID_ARG;
    }
  }

  return QMI_ERR_NONE;
} /* qmi_nas_0030_req_check */

/*=========================================================================== 
  FUNCTION QMI_NAS_0031_RSP_WRITE() 
 
  DESCRIPTION 
    Writes QMI_NAS_GET_RF_BAND_INFO response in struct nas_0031_rsp_s
     
  RETURN VALUE 
    QMI_ERR_NONE for no error or appropriate error code. 
     
  DEPENDENCIES 
    None 
 
  SIDE EFFECTS 
    None 
===========================================================================*/ 
qmi_error_e_type qmi_nas_0031_rsp_write (struct nas_0031_rsp_s *p_msg, dsm_item_type **response)
{
  struct nas_0031_rsp_p_s   nas_0031_rsp_p;
  int                       count = 0;

  if (p_msg == NULL || response == NULL)
  {
    return QMI_ERR_INTERNAL;
  }

  memset(&nas_0031_rsp_p, 0, sizeof(nas_0031_rsp_p));

  if (p_msg->t11_valid == TRUE)
  {
    memscpy(&nas_0031_rsp_p.t11.num_instances, sizeof(nas_0031_rsp_p.t11.num_instances),
            &p_msg->t11.num_instances, sizeof(nas_0031_rsp_p.t11.num_instances));

    for(count = 0; count < p_msg->t11.num_instances; count++)
    {
      memscpy((void*) &nas_0031_rsp_p.t11.instances[count].radio_if, sizeof(nas_0031_rsp_p.t11.instances[count].radio_if),
              (void*) &p_msg->t11.instances[count].radio_if, sizeof(nas_0031_rsp_p.t11.instances[count].radio_if));
      memscpy((void*) &nas_0031_rsp_p.t11.instances[count].active_band, sizeof(nas_0031_rsp_p.t11.instances[count].active_band),
              (void*) &p_msg->t11.instances[count].active_band, sizeof(nas_0031_rsp_p.t11.instances[count].active_band));
      memscpy((void*) &nas_0031_rsp_p.t11.instances[count].active_channel, sizeof(nas_0031_rsp_p.t11.instances[count].active_channel),
              (void*) &p_msg->t11.instances[count].active_channel, sizeof(nas_0031_rsp_p.t11.instances[count].active_channel));
    }

    if (FALSE == qmi_svc_put_param_tlv(response, 
                                       NAS_0031_RSP_T11,
                                       sizeof(nas_0031_rsp_p.t11.num_instances)
                                       + nas_0031_rsp_p.t11.num_instances * (sizeof(uint8) + sizeof(uint16) + sizeof(uint32)),
                                       &nas_0031_rsp_p.t11))
    {
      qmi_err_write(NAS_0031_RSP_T11, &nas_0031_rsp_p.t11);
      dsm_free_packet(response);
      return QMI_ERR_NO_MEMORY;
    }
  }

  if (p_msg->t01_valid == TRUE)
  {
    memscpy(&nas_0031_rsp_p.t01.num_instances, sizeof(nas_0031_rsp_p.t01.num_instances),
            &p_msg->t01.num_instances, sizeof(nas_0031_rsp_p.t01.num_instances));

    for(count = 0; count < p_msg->t01.num_instances; count++)
    {
      memscpy((void*) &nas_0031_rsp_p.t01.instances[count].radio_if, sizeof(nas_0031_rsp_p.t01.instances[count].radio_if),
              (void*) &p_msg->t01.instances[count].radio_if, sizeof(nas_0031_rsp_p.t01.instances[count].radio_if));
      memscpy((void*) &nas_0031_rsp_p.t01.instances[count].active_band, sizeof(nas_0031_rsp_p.t01.instances[count].active_band),
              (void*) &p_msg->t01.instances[count].active_band, sizeof(nas_0031_rsp_p.t01.instances[count].active_band));
      memscpy((void*) &nas_0031_rsp_p.t01.instances[count].active_channel, sizeof(nas_0031_rsp_p.t01.instances[count].active_channel),
              (void*) &p_msg->t01.instances[count].active_channel, sizeof(nas_0031_rsp_p.t01.instances[count].active_channel));
    }

    if (FALSE == qmi_svc_put_param_tlv(response, 
                                       NAS_0031_RSP_T01,
                                       sizeof(nas_0031_rsp_p.t01.num_instances)
                                       + nas_0031_rsp_p.t01.num_instances * (sizeof(uint8) + sizeof(uint16) + sizeof(uint16)),
                                       &nas_0031_rsp_p.t01))
    {
      qmi_err_write(NAS_0031_RSP_T01, &nas_0031_rsp_p.t01);
      dsm_free_packet(response);
      return QMI_ERR_NO_MEMORY;
    }
  }

  return QMI_ERR_NONE;
} /* qmi_nas_0031_rsp_write */

/*=========================================================================== 
  FUNCTION QMI_NAS_0032_RSP_WRITE() 
 
  DESCRIPTION 
    Writes QMI_NAS_GET_AN_AAA_STATUS response in struct nas_0032_rsp_s
     
  RETURN VALUE 
    QMI_ERR_NONE for no error or appropriate error code. 
     
  DEPENDENCIES 
    None 
 
  SIDE EFFECTS 
    None 
===========================================================================*/ 
qmi_error_e_type qmi_nas_0032_rsp_write (struct nas_0032_rsp_s *p_msg, dsm_item_type **response)
{
  struct nas_0032_rsp_p_s  nas_0032_rsp_p;

  if (p_msg == NULL || response == NULL)
  {
    return QMI_ERR_INTERNAL;
  }

  memset(&nas_0032_rsp_p, 0, sizeof(nas_0032_rsp_p));

  if (p_msg->t01_valid == TRUE)
  { //should always be true, required parameter
    memscpy(&nas_0032_rsp_p.t01.an_aaa_status, sizeof(nas_0032_rsp_p.t01.an_aaa_status),
            &p_msg->t01.an_aaa_status, sizeof(nas_0032_rsp_p.t01.an_aaa_status));

    if (FALSE == qmi_svc_put_param_tlv(response, 
                                       NAS_0032_RSP_T01,
                                       sizeof(nas_0032_rsp_p.t01),
                                       &nas_0032_rsp_p.t01))
    {
      qmi_err_write(NAS_0032_RSP_T01, &nas_0032_rsp_p.t01);
      dsm_free_packet(response);
      return QMI_ERR_NO_MEMORY;
    }
  }
  else
  {
    return QMI_ERR_INTERNAL;
  }

  return QMI_ERR_NONE;
} /* qmi_nas_0032_rsp_write */

/*=========================================================================== 
  FUNCTION QMI_NAS_0033_REQ_CHECK()

  DESCRIPTION 
    Checks QMI_NAS_SET_SYSTEM_SELECTION_PREFERENCE in nas_set_system_selection_preference_req_msg_v01 for errors

  RETURN VALUE 
    QMI_ERR_NONE_V01 for no error or appropriate error code.

  DEPENDENCIES 
  None 

  SIDE EFFECTS 
  None 
  ===========================================================================*/ 
qmi_error_type_v01 qmi_nas_0033_req_check(
  nas_set_system_selection_preference_req_msg_v01 * p_msg
)
{
  // check TLV 0x10 emergency mode
  if ( p_msg->emergency_mode_valid )
  {
    if ( p_msg->emergency_mode != NAS_0033_EMERGENCY_MODE_ON  &&
         p_msg->emergency_mode != NAS_0033_EMERGENCY_MODE_OFF )
    {
      return QMI_ERR_INVALID_ARG_V01;
    }
  }

  // ignore other TLVs when 'emergency mode' TLV is present and is 'ON'
  if ( p_msg->emergency_mode_valid                         &&
       p_msg->emergency_mode == NAS_0033_EMERGENCY_MODE_ON )
  {
    p_msg->mode_pref_valid       = FALSE;
    p_msg->band_pref_valid       = FALSE;
    p_msg->prl_pref_valid        = FALSE;
    p_msg->roam_pref_valid       = FALSE;
    p_msg->lte_band_pref_valid   = FALSE;
    p_msg->lte_band_pref_ext_valid   = FALSE;
    p_msg->net_sel_pref_valid    = FALSE;
    p_msg->change_duration_valid = FALSE;
  }

  if ( p_msg->mode_pref_valid)
  {
    // check TLV 0x11 mode preference
    if ( p_msg->mode_pref & (mode_pref_mask_type_v01) ~QMI_NAS_RAT_MODE_PREF_ALL )  // check for bits outside range
    {
      p_msg->mode_pref &= QMI_NAS_RAT_MODE_PREF_ALL;
    }
  }

  // check t14.roam_pref TLV
  if ( p_msg->roam_pref_valid )
  {
    if ( p_msg->roam_pref != NAS_ROAMING_PREF_OFF_V01         &&
         p_msg->roam_pref != NAS_ROAMING_PREF_NOT_OFF_V01     &&
         p_msg->roam_pref != NAS_ROAMING_PREF_NOT_FLASING_V01 &&
         p_msg->roam_pref != NAS_ROAMING_PREF_ANY_V01         )
    {
      return QMI_ERR_INVALID_ARG_V01;
    }
  }

  // check t13.prl_pref TLV
  if ( p_msg->prl_pref_valid )
  {
    if ( p_msg->prl_pref != NAS_PRL_PREF_A_SIDE_ONLY_V01 &&
         p_msg->prl_pref != NAS_PRL_PREF_B_SIDE_ONLY_V01 &&
         p_msg->prl_pref != NAS_PRL_PREF_ANY_V01         )
    {
      return QMI_ERR_INVALID_ARG_V01;
    }
  }

  // check t16 TLV
  if ( p_msg->net_sel_pref_valid )
  {
    if ( p_msg->net_sel_pref.net_sel_pref != NAS_NET_SEL_PREF_AUTOMATIC_V01 &&
         p_msg->net_sel_pref.net_sel_pref != NAS_NET_SEL_PREF_MANUAL_V01    )
    {
      return QMI_ERR_INVALID_ARG_V01;
    }
  }

  // check t17 TLV
  if ( p_msg->change_duration_valid )
  {
    if ( ! p_msg->emergency_mode_valid    && ! p_msg->mode_pref_valid       && ! p_msg->band_pref_valid           &&
         ! p_msg->prl_pref_valid          && ! p_msg->roam_pref_valid       && ! p_msg->lte_band_pref_valid       &&
         ! p_msg->net_sel_pref_valid      && ! p_msg->srv_domain_pref_valid && ! p_msg->gw_acq_order_pref_valid   &&
         ! p_msg->tdscdma_band_pref_valid && ! p_msg->acq_order_valid       && ! p_msg->srv_reg_restriction_valid && 
         ! p_msg->usage_setting_valid     && ! p_msg->voice_domain_pref_valid && ! p_msg->lte_band_pref_ext_valid)
    {
      return QMI_ERR_MISSING_ARG_V01;
    }

    if ( p_msg->change_duration != NAS_POWER_CYCLE_V01 &&
         p_msg->change_duration != NAS_PERMANENT_V01   )
    {
      return QMI_ERR_INVALID_ARG_V01;
    }
  }

  if ( p_msg->srv_domain_pref_valid )
  {
    if ( p_msg->srv_domain_pref !=  QMI_SRV_DOMAIN_PREF_CS_ONLY_V01                  &&
         p_msg->srv_domain_pref !=  QMI_SRV_DOMAIN_PREF_PS_ONLY_V01                  &&
         p_msg->srv_domain_pref !=  QMI_SRV_DOMAIN_PREF_CS_PS_V01                    &&
         p_msg->srv_domain_pref !=  QMI_SRV_DOMAIN_PREF_PS_ATTACH_V01                &&
         p_msg->srv_domain_pref !=  QMI_SRV_DOMAIN_PREF_PS_DETACH_V01                &&
         p_msg->srv_domain_pref !=  QMI_SRV_DOMAIN_PREF_PS_DETACH_NO_PREF_CHANGE_V01 &&
         p_msg->srv_domain_pref !=  QMI_SRV_DOMAIN_PREF_ON_DEMAND_PS_ATTACH_V01      &&
         p_msg->srv_domain_pref !=  QMI_SRV_DOMAIN_PREF_FORCE_PS_DETACH_V01)
    {
      return QMI_ERR_INVALID_ARG_V01;
    }
  }

  if ( p_msg->gw_acq_order_pref_valid )
  {
    if ( p_msg->gw_acq_order_pref != NAS_GW_ACQ_ORDER_PREF_AUTOMATIC_V01 &&
         p_msg->gw_acq_order_pref != NAS_GW_ACQ_ORDER_PREF_GSM_WCDMA_V01 &&
         p_msg->gw_acq_order_pref != NAS_GW_ACQ_ORDER_PREF_WCDMA_GSM_V01 )
    {
      return QMI_ERR_INVALID_ARG_V01;
    }
  }

  if ( p_msg->mnc_includes_pcs_digit_valid && ! p_msg->net_sel_pref_valid )
  {
    return QMI_ERR_INVALID_ARG_V01;
  }

  if ( p_msg->acq_order_valid )
  {
    if ( p_msg->acq_order_len > NAS_ACQ_ORDER_LIST_MAX_V01 )
    {
      QM_MSG_HIGH_2("acquisition order length of %d, max expected %d", p_msg->acq_order_len, NAS_ACQ_ORDER_LIST_MAX_V01);
      return QMI_ERR_INVALID_ARG_V01;
    }
  }

  if ( p_msg->srv_reg_restriction_valid )
  {
    if ( p_msg->srv_reg_restriction != NAS_SRV_REG_RESTRICTION_UNRESTRICTED_V01 &&
         p_msg->srv_reg_restriction != NAS_SRV_REG_RESTRICTION_LIMITED_V01      &&
         p_msg->srv_reg_restriction != NAS_SRV_REG_RESTRICTION_CAMPED_ONLY_V01  )
    {
      return QMI_ERR_INVALID_ARG_V01;
    }
  }

  if( p_msg->csg_info_valid && p_msg->net_sel_pref_valid )
  {
    return QMI_ERR_INVALID_ARG_V01;
  }

  if( p_msg->csg_info_valid )
  {
    if( p_msg->csg_info.rat != NAS_RADIO_IF_GSM_V01 &&
        p_msg->csg_info.rat != NAS_RADIO_IF_UMTS_V01 &&
        p_msg->csg_info.rat != NAS_RADIO_IF_LTE_V01 &&
        p_msg->csg_info.rat != NAS_RADIO_IF_TDSCDMA_V01 )
    {
      return QMI_ERR_INVALID_ARG_V01;
    }
  }

  if( p_msg->usage_setting_valid )
  {
    if( p_msg->usage_setting != NAS_USAGE_VOICE_CENTRIC_V01 &&
        p_msg->usage_setting != NAS_USAGE_DATA_CENTRIC_V01 )
    {
      return QMI_ERR_INVALID_ARG_V01;
    }
  }

  if( p_msg->rat_valid )
  {
    if( p_msg->rat != NAS_RADIO_IF_GSM_V01 &&
        p_msg->rat != NAS_RADIO_IF_UMTS_V01 &&
        p_msg->rat != NAS_RADIO_IF_LTE_V01 &&
        p_msg->rat != NAS_RADIO_IF_TDSCDMA_V01 )
    {
      return QMI_ERR_INVALID_ARG_V01;
    }
  }

  if( p_msg->voice_domain_pref_valid)
  {
    if( p_msg->voice_domain_pref  < NAS_VOICE_DOMAIN_PREF_CS_ONLY_V01 ||
        p_msg->voice_domain_pref > NAS_VOICE_DOMAIN_PREF_PS_PREF_V01
      )
    {
      return QMI_ERR_INVALID_ARG_V01;
    }
  }
  
  if( p_msg->lte_band_pref_valid && p_msg->lte_band_pref_ext_valid)//only one may be used
  {
    return QMI_ERR_INVALID_ARG_V01;
  }
  
  return QMI_ERR_NONE_V01;
}


/*=========================================================================== 
  FUNCTION QMI_NAS_003D_REQ_READ() 
 
  DESCRIPTION 
    Reads QMI_NAS_UPDATE_AKEY into struct nas_003D_req_s 
     
  RETURN VALUE 
    QMI_ERR_NONE for successful read. For failure, returns appropriate error 
    value. 
     
  DEPENDENCIES 
    None 
 
  SIDE EFFECTS 
    None 
===========================================================================*/ 
qmi_error_e_type qmi_nas_003D_req_read ( 
  struct nas_003D_req_s  *p_msg, 
  dsm_item_type         **sdu_in 
) 
{
#if defined (FEATURE_CDMA_800) || defined (FEATURE_CDMA_1900)
  uint16                     len;
  uint16                     expected_len;
  uint8                      prm_type;
  struct nas_003D_req_p_s    nas_003D_req_p;
  void                       *value;

  memset(&nas_003D_req_p, 0, sizeof(nas_003D_req_p));

  while (*sdu_in) 
  {
    if ( !qmi_svc_get_tl(sdu_in, &prm_type, &len) )
    {
      continue;
    }

    expected_len = 0;
    value = NULL;

    // special value 0 = variable length or don't care (unrecognized TLV)
    switch (prm_type)
    {
      case NAS_003D_REQ_AKEY:
        expected_len = sizeof(nas_003D_req_p.t01);
        value = &nas_003D_req_p.t01;
        p_msg->t01_valid = TRUE;
        break;

      default:
        QM_MSG_ERROR_1("Unrecognized TLV type (%d)", prm_type);
        break;
    }

    if ((expected_len != 0) && (expected_len != len))
    {
      QM_MSG_ERROR_2("Invalid TLV len (%d) for type (%d)", len, prm_type);
      return QMI_ERR_MALFORMED_MSG;
    }

    if (len != dsm_pullup( sdu_in, value, len ))
    {
        qmi_nas_invalid_len_tlv();
      return QMI_ERR_MALFORMED_MSG;
    }
  }

  if (!p_msg->t01_valid)
  {
    QM_MSG_ERROR("AKEY not provided");
    return QMI_ERR_INVALID_ARG;
  }
  
  /*-----------------------------------------------------------------------
    Transfer to unpacked struct
  -----------------------------------------------------------------------*/
  memscpy(p_msg->t01.akey, sizeof(nas_003D_req_p.t01.akey), nas_003D_req_p.t01.akey, sizeof(nas_003D_req_p.t01.akey));
#endif //(FEATURE_CDMA_800) || defined (FEATURE_CDMA_1900)
  return QMI_ERR_NONE;
} /* qmi_nas_003D_req_read */

/*=========================================================================== 
  FUNCTION QMI_NAS_003D_REQ_CHECK() 
 
  DESCRIPTION 
    Checks QMI_NAS_UPDATE_AKEY in struct nas_003D_req_s for errors 
     
  RETURN VALUE 
    QMI_ERR_NONE for no error or appropriate error code. 
     
  DEPENDENCIES 
    None 
 
  SIDE EFFECTS 
    None 
===========================================================================*/ 
qmi_error_e_type qmi_nas_003D_req_check (struct nas_003D_req_s *p_msg)
{

  /*
   * This is a placeholder function.
   */


  return QMI_ERR_NONE;
} /* qmi_nas_003D_req_check */

/*=========================================================================== 
  FUNCTION QMI_NAS_003E_REQ_CHECK() 
 
  DESCRIPTION 
    Checks QMI_NAS_GET_3GPP2_SUBSCRIPTION_INFO in struct nas_get_3gpp2_subscription_info_req_msg_v01 for errors 
     
  RETURN VALUE 
    QMI_ERR_NONE for no error or appropriate error code. 
     
  DEPENDENCIES 
    None 
 
  SIDE EFFECTS 
    None 
===========================================================================*/ 
qmi_error_e_type qmi_nas_003E_req_check (nas_get_3gpp2_subscription_info_req_msg_v01 *p_msg) 
{
  if (p_msg->nam_id> NV_MAX_NAMS && p_msg->nam_id != NAS_003E_REQ_CURR_NAM_MASK)
  {
    QM_MSG_ERROR("Invalid NAM ID"); 
    return QMI_ERR_INVALID_ARG;
  }

  return QMI_ERR_NONE;
} /* qmi_nas_003E_req_check */


/*=========================================================================
  FUNCTION:  nas_003F_req_cm_jpn_band_is_supported

===========================================================================*/
/*!
    @brief
    Indicates whether JCDMA is supported.

    @return
     True if JPN band is supported,
     False otherwise.
*/
/*=========================================================================*/
boolean nas_003F_req_cm_jpn_band_is_supported(void)
{
   if ( ( qmi_nasi_global.cm_ph_info.band_capability & NAS_003F_REQ_CM_BAND_PREF_JPN ) == NAS_003F_REQ_CM_BAND_PREF_JPN )
   {
     return TRUE;
   }

   return FALSE;

} /* nas_003F_req_cm_jpn_band_is_supported */

/*=========================================================================

  FUNCTION:  nas_003F_req_cellular_sys_is_a_channel

===========================================================================*/
/*!
    @brief
     Check if the CDMA channel is with Cellular Band System A.

    @return
     Boolean
*/
/*=========================================================================*/
static boolean nas_003F_req_cellular_sys_is_a_channel
(
  uint16 cdma_channel       /* The channel to examine */
)
{
   boolean response = FALSE;
   if ( nas_003F_req_cm_jpn_band_is_supported() )
   {
      /* According to T53 Section 6.1.1.1, the CDMA channel must be even, and
      * be in the range of 51-749, 851-989, or 1091-1149.
      */
     if (((cdma_channel & 0x0001) == 0) &&
        (((cdma_channel > 51)   && (cdma_channel <  749))  ||
        ((cdma_channel > 851)  && (cdma_channel <  989))  ||
        ((cdma_channel > 1091) && (cdma_channel < 1149))))
     {
       response = TRUE;
     }
     else
     {
       response =  FALSE;
     }
   }
   else
   {
     response = NAS_003F_REQ_PRL_IS_IN_CHAN_CELLULAR_SYS_A(cdma_channel); /* Sys A */
   }

   return response;
} /* end of nas_003F_req_cellular_sys_is_a_channel */

/*=========================================================================

  FUNCTION:  nas_003F_req_cellular_sys_is_b_channel

===========================================================================*/
/*!
    @brief
      Check if the CDMA channel is with Cellular Band System B

    @return
      Boolean
*/
/*=========================================================================*/
static boolean nas_003F_req_cellular_sys_is_b_channel
(
  uint16 cdma_channel       /* The channel to examine */
)
{
   boolean response = FALSE;

   if ( !nas_003F_req_cm_jpn_band_is_supported() )
   {
      response = NAS_003F_REQ_PRL_IS_IN_CHAN_CELLULAR_SYS_B(cdma_channel);
   }

   return response;
} /* end of nas_003F_req_cellular_sys_is_b_channel */

/*=========================================================================== 
  FUNCTION QMI_NAS_003F_REQ_CHECK() 
 
  DESCRIPTION 
    Checks QMI_NAS_SET_3GPP2_SUBSCRIPTION_INFO in struct nas_003F_req_s for errors 
     
  RETURN VALUE 
    QMI_ERR_NONE for no error or appropriate error code. 
     
  DEPENDENCIES 
    None 
 
  SIDE EFFECTS 
    None 
===========================================================================*/ 
qmi_error_type_v01 qmi_nas_003F_req_check (nas_set_3gpp2_subscription_info_req_msg_v01 *p_msg)
{
  int count;

  /*-----------------------------------------------------------------------
    NAM
  -----------------------------------------------------------------------*/
  
  if (p_msg->nam_id > NV_MAX_NAMS && p_msg->nam_id != NASI_CURR_NAM_MASK)
  {
    QM_MSG_ERROR("Invalid NAM ID"); 
    return QMI_ERR_INVALID_ARG_V01;
  }
  
  /*-----------------------------------------------------------------------
    Directory number
  -----------------------------------------------------------------------*/
  
  //does not require bounds checking
  
  /*-----------------------------------------------------------------------
    Home SID/NID
  -----------------------------------------------------------------------*/
  
  if (p_msg->cdma_sys_id_valid)
  { 
    if (p_msg->cdma_sys_id_len > NV_MAX_HOME_SID_NID)
    {
      QM_MSG_ERROR_1("Number of home SID/NID instances provided exceeds max allowed (%d)", NV_MAX_HOME_SID_NID);
      return QMI_ERR_ARG_TOO_LONG_V01;
    }
    for (count = 0; count < p_msg->cdma_sys_id_len; count++)
    {
      if (p_msg->cdma_sys_id[count].sid & 0x8000)
      {
        QM_MSG_ERROR_1("SID index %d has bit 0x8000 set", count);
        return QMI_ERR_INVALID_ARG_V01;
      }
    }                  
  }

  /*-----------------------------------------------------------------------
    MIN based IMSI
  -----------------------------------------------------------------------*/
  if (p_msg->min_based_info_valid)
  {
    /*-----------------------------------------------------------------------
      IMSI M MCC
    -----------------------------------------------------------------------*/
    if (!qmi_nas_string_is_numeric((uint8*)p_msg->min_based_info.mcc_m, NASI_MCC_LEN))
    {
      QM_MSG_ERROR("MIN based IMSI MCC argument contains non-numeric characters");
      return QMI_ERR_INVALID_ARG_V01;
    }
    
    /*-----------------------------------------------------------------------
      IMSI M 11 12
    -----------------------------------------------------------------------*/
    if (!qmi_nas_string_is_numeric((uint8*)p_msg->min_based_info.imsi_m_11_12, NASI_11_12_LEN))
    {
      QM_MSG_ERROR("MIN based IMSI 11 12 argument contains non-numeric characters");
      return QMI_ERR_INVALID_ARG_V01;
    }
    
    /*-----------------------------------------------------------------------
      IMSI M MIN1
    -----------------------------------------------------------------------*/
    if (!qmi_nas_string_is_numeric((uint8*)p_msg->min_based_info.imsi_m_s1, NASI_MIN1_LEN))
    {
      QM_MSG_ERROR("MIN based IMSI MIN1 argument contains non-numeric characters");
      return QMI_ERR_INVALID_ARG_V01;
    }

    /*-----------------------------------------------------------------------
      IMSI M MIN2
    -----------------------------------------------------------------------*/
    if (!qmi_nas_string_is_numeric((uint8*)p_msg->min_based_info.imsi_m_s2, NASI_MIN2_LEN))
    {
      QM_MSG_ERROR("MIN based IMSI MIN2 argument contains non-numeric characters");
      return QMI_ERR_INVALID_ARG_V01;
    }
  }
  
  /*-----------------------------------------------------------------------
    True IMSI
  -----------------------------------------------------------------------*/
  if (p_msg->true_imsi_valid)
  {
    /*-----------------------------------------------------------------------
      IMSI T MCC
    -----------------------------------------------------------------------*/
    if (!qmi_nas_string_is_numeric((uint8*)p_msg->true_imsi.mcc_t, NASI_MCC_LEN))
    {
      QM_MSG_ERROR("True IMSI MCC argument contains non-numeric characters");
      return QMI_ERR_INVALID_ARG_V01;
    }
    
    /*-----------------------------------------------------------------------
      IMSI T 11 12
    -----------------------------------------------------------------------*/
    if (!qmi_nas_string_is_numeric((uint8*)p_msg->true_imsi.imsi_t_11_12, NASI_11_12_LEN))
    {
      QM_MSG_ERROR("True IMSI 11 12 argument contains non-numeric characters");
      return QMI_ERR_INVALID_ARG_V01;
    }
    
    /*-----------------------------------------------------------------------
      IMSI T MIN1
    -----------------------------------------------------------------------*/
    if (!qmi_nas_string_is_numeric((uint8*)p_msg->true_imsi.imsi_t_s1, NASI_MIN1_LEN))
    {
      QM_MSG_ERROR("True IMSI MIN1 argument contains non-numeric characters");
      return QMI_ERR_INVALID_ARG_V01;
    }

    /*-----------------------------------------------------------------------
      IMSI T MIN2
    -----------------------------------------------------------------------*/
    if (!qmi_nas_string_is_numeric((uint8*)p_msg->true_imsi.imsi_t_s2, NASI_MIN2_LEN))
    {
      QM_MSG_ERROR("True IMSI MIN2 argument contains non-numeric characters");
      return QMI_ERR_INVALID_ARG_V01;
    }
  
    /*-----------------------------------------------------------------------
        Address Number
    -----------------------------------------------------------------------*/
    //check bounds
    if ( ( p_msg->true_imsi.imsi_t_addr_num > 7) &&
        ( p_msg->true_imsi.imsi_t_addr_num != 0xFF ) )
    {
      QM_MSG_ERROR("True IMSI address number argument not in correct bounds");
      return QMI_ERR_INVALID_ARG_V01;
    }
  }
  
  /*-----------------------------------------------------------------------
    CDMA Channel
  -----------------------------------------------------------------------*/
  if (p_msg->cdma_channel_info_valid)
  {
    //verify that channels inputted are correct
    if (!nas_003F_req_cellular_sys_is_a_channel(p_msg->cdma_channel_info.pri_ch_a))
    {
        QM_MSG_ERROR("Primary channel A invalid");
        return QMI_ERR_INVALID_ARG_V01;
    }

    if (!nas_003F_req_cellular_sys_is_b_channel(p_msg->cdma_channel_info.pri_ch_b))
    {
        QM_MSG_ERROR("Primary channel B invalid");
        return QMI_ERR_INVALID_ARG_V01;
    }
   
    if (!nas_003F_req_cellular_sys_is_a_channel(p_msg->cdma_channel_info.sec_ch_a))
    {
        QM_MSG_ERROR("Secondary channel A invalid");
        return QMI_ERR_INVALID_ARG_V01;
    }

    if (!nas_003F_req_cellular_sys_is_b_channel(p_msg->cdma_channel_info.sec_ch_b))
    {
        QM_MSG_ERROR("Secondary channel B invalid");
        return QMI_ERR_INVALID_ARG_V01;
    }
  }

  /*-----------------------------------------------------------------------
    Mobile Directory Number
  -----------------------------------------------------------------------*/
  if(p_msg->mdn_valid)
  {
    for( count =0; count < p_msg->mdn_len ; count++)
    {
      if( p_msg->mdn[count] != '*' &&
          p_msg->mdn[count] != '#' &&
          (p_msg->mdn[count] < '0' || p_msg->mdn[count] > '9'))
      {
        QM_MSG_ERROR_1("Valid MDN values are *, #, and digits 0-9 only: %d", p_msg->mdn[count]);
        return QMI_ERR_INVALID_ARG_V01;
      }
    }
    }

  /*-----------------------------------------------------------------------
    Service Programming Code
  -----------------------------------------------------------------------*/
  if( p_msg->dir_num_valid || p_msg->cdma_sys_id_valid || p_msg->min_based_info_valid ||
      p_msg->cdma_channel_info_valid || p_msg->mdn_valid )
  {
    if(p_msg->spc_valid == FALSE)
    {
      return QMI_ERR_INVALID_ARG_V01;
  }

    // spc_valid is not checked when all of the TLVs 0x10, 0x11, 0x12, 0x14 and 0x16 are not present,
    //this allows processing of TLVs 0x01, 0x13 and 0x15 when client passes spc TLV along with them
    if( p_msg->spc_valid )
  {
      for (count = 0; count < NAS_SPC_MAX_V01 ; count++)
    {
        if (p_msg->spc[count] < '0' || p_msg->spc[count] > '9')
      {
          QM_MSG_ERROR("Invalid SPC");
          return QMI_ERR_INVALID_ARG_V01;
        }
      }
    }
  }

  return QMI_ERR_NONE_V01;
} /* qmi_nas_003F_req_check */

/*=========================================================================== 
  FUNCTION QMI_NAS_0040_RSP_WRITE() 
 
  DESCRIPTION 
    Writes QMI_NAS_GET_MOB_CAI_REV response in struct nas_0040_rsp_s
     
  RETURN VALUE 
    QMI_ERR_NONE for no error or appropriate error code. 
     
  DEPENDENCIES 
    None 
 
  SIDE EFFECTS 
    None 
===========================================================================*/ 
qmi_error_e_type qmi_nas_0040_rsp_write (struct nas_0040_rsp_s *p_msg, dsm_item_type **response)
{
  struct nas_0040_rsp_p_s  nas_0040_rsp_p;

  memset(&nas_0040_rsp_p, 0, sizeof(nas_0040_rsp_p));

  if (p_msg->t10_valid == TRUE)
  {
    memscpy(&nas_0040_rsp_p.t10.cai_rev, sizeof(nas_0040_rsp_p.t10.cai_rev),
            &p_msg->t10.cai_rev, sizeof(nas_0040_rsp_p.t10.cai_rev));

    if (FALSE == qmi_svc_put_param_tlv(response, 
                                      NAS_0040_REQ_MOB_CAI_REV,
                                      sizeof(nas_0040_rsp_p.t10),
                                      &nas_0040_rsp_p.t10))
    {
      qmi_err_write(NAS_0040_REQ_MOB_CAI_REV,&nas_0040_rsp_p.t10);
      dsm_free_packet(response);
      return QMI_ERR_NO_MEMORY;
    }
  }
    
  return QMI_ERR_NONE;
} /* qmi_nas_0040_rsp_write */



/*=========================================================================== 
  FUNCTION QMI_NAS_0041_RSP_WRITE() 
 
  DESCRIPTION 
    Writes QMI_NAS_GET_RTRE_CONFIG response in struct nas_0041_rsp_s
     
  RETURN VALUE 
    QMI_ERR_NONE for no error or appropriate error code. 
     
  DEPENDENCIES 
    None 
 
  SIDE EFFECTS 
    None 
===========================================================================*/ 
qmi_error_e_type qmi_nas_0041_rsp_write (struct nas_0041_rsp_s *p_msg, dsm_item_type **response)
{
  struct nas_0041_rsp_p_s  nas_0041_rsp_p;
  
  memset(&nas_0041_rsp_p, 0, sizeof(nas_0041_rsp_p)); 

  if (p_msg->t11_valid == TRUE)
  {
    memscpy(&nas_0041_rsp_p.t11.rtre_cfg_pref, sizeof(nas_0041_rsp_p.t11.rtre_cfg_pref),
            &p_msg->t11.rtre_cfg_pref, sizeof(nas_0041_rsp_p.t11.rtre_cfg_pref));

    if (FALSE == qmi_svc_put_param_tlv(response, 
                                      NAS_0041_RSP_RTRE_CONFIG_PREF,
                                      sizeof(nas_0041_rsp_p.t11),
                                      &p_msg->t11))
    {
      qmi_err_write(NAS_0041_RSP_RTRE_CONFIG_PREF, &p_msg->t11);
      dsm_free_packet(response);
      return QMI_ERR_NO_MEMORY;
    }
  }
  
  if (p_msg->t10_valid == TRUE)
  {
    memscpy(&nas_0041_rsp_p.t10.rtre_cfg, sizeof(nas_0041_rsp_p.t10.rtre_cfg),
            &p_msg->t10.rtre_cfg, sizeof(nas_0041_rsp_p.t10.rtre_cfg));

    if (FALSE == qmi_svc_put_param_tlv(response, 
                                      NAS_0041_RSP_RTRE_CONFIG,
                                      sizeof(nas_0041_rsp_p.t10),
                                      &p_msg->t10))
    {
      qmi_err_write(NAS_0041_RSP_RTRE_CONFIG, &p_msg->t10);
      dsm_free_packet(response);
      return QMI_ERR_NO_MEMORY;
    }
  }
   
  return QMI_ERR_NONE;
} /* qmi_nas_0041_rsp_write */


/*=========================================================================== 
  FUNCTION QMI_NAS_0042_REQ_CHECK()
 
  DESCRIPTION 
    Checks QMI_NAS_SET_RTRE_CONFIG in struct nas_set_rtre_config_req_v01 for errors
     
  RETURN VALUE 
    QMI_ERR_NONE for no error or appropriate error code.
     
  DEPENDENCIES 
    None 
 
  SIDE EFFECTS 
    None 
===========================================================================*/ 
qmi_error_type_v01 qmi_nas_0042_req_check (nas_set_rtre_config_req_v01 *p_msg)
{
   int i;
  
   if (p_msg->rtre_cfg_pref >= NASI_RTRE_MAX)
  {
     QM_MSG_ERROR("Invalid RTRE config value provided");
     return QMI_ERR_INVALID_ARG_V01;
    }

   if( p_msg->spc_valid )
   {
     for (i = 0; i < NAS_SPC_MAX_V01 ; i++)
     {
       if (p_msg->spc[i] < '0' || p_msg->spc[i] > '9')
       {
         QM_MSG_ERROR("Invalid SPC");
         return QMI_ERR_INVALID_ARG_V01;
       }
     }
   }
  
    return QMI_ERR_NONE_V01;
} /* qmi_nas_0042_req_check */

/*=========================================================================== 
  FUNCTION QMI_NAS_0044_REQ_CHECK() 
 
  DESCRIPTION 
    Checks QMI_NAS_GET_PLMN_NAME in struct nas_0044_req_s for errors 
     
  RETURN VALUE 
    QMI_ERR_NONE for no error or appropriate error code. 
     
  DEPENDENCIES 
    None 
 
  SIDE EFFECTS 
    None 
===========================================================================*/ 
qmi_error_type_v01 qmi_nas_0044_req_check (nas_get_plmn_name_req_msg_v01 *p_msg) 
{
  // Ensure mcc / mnc in valid range
  if( p_msg->plmn.mnc > 999 || p_msg->plmn.mcc > 999 )
  {
    return QMI_ERR_INVALID_ARG_V01;
  }

  if( p_msg->suppress_sim_error_valid )
  {
    if( p_msg->suppress_sim_error != FALSE &&
        p_msg->suppress_sim_error != TRUE )
    {
      return QMI_ERR_INVALID_ARG_V01;
    }
  }

  if ( p_msg->mnc_includes_pcs_digit_valid )
  {
    if (p_msg->mnc_includes_pcs_digit != FALSE &&
        p_msg->mnc_includes_pcs_digit != TRUE )
    {
      return QMI_ERR_INVALID_ARG_V01;
    }
  }
    
  if ( p_msg->always_send_plmn_name_valid )
  {
    if (p_msg->always_send_plmn_name != FALSE &&
        p_msg->always_send_plmn_name != TRUE )
    {
      return QMI_ERR_INVALID_ARG_V01;
    }
  }
    
  if ( p_msg->use_static_table_only_valid )
  {
    if (p_msg->use_static_table_only != FALSE &&
        p_msg->use_static_table_only != TRUE )
    {
      return QMI_ERR_INVALID_ARG_V01;
    }
  }

  if ( p_msg->rat_valid )
  {
    if (p_msg->rat != NAS_RADIO_IF_GSM_V01 &&
        p_msg->rat != NAS_RADIO_IF_UMTS_V01 &&
        p_msg->rat != NAS_RADIO_IF_LTE_V01 &&
        p_msg->rat != NAS_RADIO_IF_TDSCDMA_V01 )
    {
      return QMI_ERR_INVALID_ARG_V01;
    }
  }

  if ( p_msg->send_all_information_valid )
{
    if (p_msg->send_all_information != FALSE &&
        p_msg->send_all_information != TRUE)
  {
      return QMI_ERR_INVALID_ARG_V01;
    }
  }
  
  return QMI_ERR_NONE_V01;
} /* qmi_nas_0044_req_check */

/*=========================================================================== 
  FUNCTION QMI_NAS_0045_REQ_READ() 
 
  DESCRIPTION 
    Reads QMI_NAS_BIND_SUBSCRIPTION into struct nas_0045_req_s 
     
  RETURN VALUE 
    QMI_ERR_NONE for successful read. For failure, returns appropriate error 
    value. 
     
  DEPENDENCIES 
    None 
 
  SIDE EFFECTS 
    None 
===========================================================================*/ 
qmi_error_e_type qmi_nas_0045_req_read ( 
  struct nas_0045_req_s  *p_msg, 
  dsm_item_type         **sdu_in 
) 
{
  uint8             prm_type;
  uint16            len;
  uint16            expected_len;
  void *            value;
  struct nas_0045_req_p_s  nas_0045_req_p;

  memset(&nas_0045_req_p, 0, sizeof(nas_0045_req_p)); 
  
  /*-----------------------------------------------------------------------
    Get request TLVs
  -----------------------------------------------------------------------*/
  while (*sdu_in)
  {
    if (!qmi_svc_get_tl(sdu_in, &prm_type, &len))
    {
      continue;
    }

    expected_len = 0;
    value = NULL;

    // special value 0 = variable length or don't care (unrecognized TLV)
    switch (prm_type)
    {
      case NAS_0045_REQ_T01:
        expected_len = sizeof(nas_0045_req_p.t01);
        value = &nas_0045_req_p.t01;
        p_msg->t01_valid = TRUE;
        break;
        
      default:
        QM_MSG_ERROR_1("Unrecognized TLV type (%d)", prm_type);
        break;
    }

    if ((expected_len != 0) && (expected_len != len))
    {
      QM_MSG_ERROR_2("Invalid TLV len (%d) for type (%d)", len, prm_type);
      return QMI_ERR_MALFORMED_MSG;
    }

    if (len != dsm_pullup( sdu_in, value, len ))
    {
        qmi_nas_invalid_len_tlv();
      return QMI_ERR_MALFORMED_MSG;
    }
  }
  
  if ( !p_msg->t01_valid )
  {
    QM_MSG_ERROR("Subscription Type TLV missing");
    return QMI_ERR_INVALID_ARG;
  }
  
  memscpy(&p_msg->t01.subscription_type, sizeof(nas_0045_req_p.t01.subscription_type),
          &nas_0045_req_p.t01.subscription_type, sizeof(nas_0045_req_p.t01.subscription_type));
  
  return QMI_ERR_NONE;
} /* qmi_nas_0045_req_read */

/*=========================================================================== 
  FUNCTION QMI_NAS_0045_REQ_CHECK() 
 
  DESCRIPTION 
    Checks QMI_NAS_BIND_SUBSCRIPTION in struct nas_0045_req_s for errors 
     
  RETURN VALUE 
    QMI_ERR_NONE for no error or appropriate error code. 
     
  DEPENDENCIES 
    None 
 
  SIDE EFFECTS 
    None 
===========================================================================*/ 
qmi_error_e_type qmi_nas_0045_req_check (struct nas_0045_req_s *p_msg) 
{
  if ( p_msg->t01.subscription_type != NAS_0045_REQ_T01_PRIMARY_SUBSCRIPTION &&
       p_msg->t01.subscription_type != NAS_0045_REQ_T01_SECONDARY_SUBSCRIPTION  &&
       p_msg->t01.subscription_type != NAS_0045_REQ_T01_TERTIARY_SUBSCRIPTION )
  {
    QM_MSG_ERROR_1("Invalid subsscription type %d", p_msg->t01.subscription_type);
    return QMI_ERR_INVALID_ARG;
  }

  #ifndef FEATURE_DUAL_SIM
   if(p_msg->t01.subscription_type != NAS_0045_REQ_T01_PRIMARY_SUBSCRIPTION)
   {
     QM_MSG_ERROR_1("subscription type recieved in bind for Non-DSDS %d", p_msg->t01.subscription_type);
     return QMI_ERR_NO_SUBSCRIPTION;
   }
 #endif
#ifndef FEATURE_TRIPLE_SIM
  if(p_msg->t01.subscription_type == NAS_0045_REQ_T01_TERTIARY_SUBSCRIPTION)
  {
    MSG_ERROR( "subscription type recieved in bind for Non-TSTS %d", p_msg->t01.subscription_type, 0, 0 );
    return QMI_ERR_NO_SUBSCRIPTION;
  }
#endif

  return QMI_ERR_NONE;
} /* qmi_nas_0045_req_check */

/*=========================================================================== 
  FUNCTION QMI_NAS_0046_IND_WRITE() 
 
  DESCRIPTION 
    Writes QMI_NAS_MANAGED_ROAMING_IND response in struct nas_0046_ind_s
     
  RETURN VALUE 
    QMI_ERR_NONE for no error or appropriate error code.
===========================================================================*/
qmi_error_e_type qmi_nas_0046_ind_write (struct nas_0046_ind_s *p_msg, dsm_item_type **response)
{
  struct nas_0046_ind_p_s nas_0046_ind_p;

  if (p_msg == NULL || response == NULL)
  {
    return QMI_ERR_INTERNAL;
  }

  memset(&nas_0046_ind_p, 0, sizeof(nas_0046_ind_p));

  if ( p_msg->t10_valid )
  {
    memscpy((void*)&nas_0046_ind_p.t10.radio_if, sizeof(nas_0046_ind_p.t10.radio_if),
            (void*)&p_msg->t10.radio_if, sizeof(nas_0046_ind_p.t10.radio_if));

    if ( !qmi_svc_put_param_tlv( response, NAS_0046_IND_T10, sizeof(nas_0046_ind_p.t10), &nas_0046_ind_p.t10))
    {
      qmi_err_write(NAS_0046_IND_T10, &nas_0046_ind_p.t10);
      dsm_free_packet(response);
      return QMI_ERR_NO_MEMORY;
    }
  }

  return QMI_ERR_NONE;
} /* qmi_nas_0046_ind_write */

/*=========================================================================== 
  FUNCTION QMI_NAS_0049_RSP_WRITE() 
 
  DESCRIPTION 

     
  RETURN VALUE 
    QMI_ERR_NONE for no error or appropriate error code. 
     
  DEPENDENCIES 
    None 
 
  SIDE EFFECTS 
    None 
===========================================================================*/
qmi_error_e_type qmi_nas_0049_rsp_write( struct nas_0049_rsp_s *p_msg, dsm_item_type **response )
{
  struct nas_0049_rsp_p_s rsp_msg_p;

  if ( !p_msg || !response )
  {
    return QMI_ERR_INTERNAL;
  }

  memset( &rsp_msg_p, 0x00, sizeof(rsp_msg_p) );

  if ( p_msg->t11_valid )
  {
    rsp_msg_p.t11.mode_pref_1 = p_msg->t11.mode_pref_1;

    if ( !qmi_svc_put_param_tlv( response, NAS_0049_RSP_T11, sizeof(rsp_msg_p.t11), &rsp_msg_p.t11 ) )
    {
      /*Failed to put mode_pref_1 TLV*/
      qmi_err_write(NAS_0049_RSP_T11, &rsp_msg_p.t11);
      dsm_free_packet( response );
      return QMI_ERR_NO_MEMORY;
    }
  }
  
  if ( p_msg->t10_valid )
  {
    rsp_msg_p.t10.mode_pref_0 = p_msg->t10.mode_pref_0;

    if ( !qmi_svc_put_param_tlv( response, NAS_0049_RSP_T10, sizeof(rsp_msg_p.t10), &rsp_msg_p.t10 ) )
    {
      /*Failed to put mode_pref_0 TLV*/
      qmi_err_write(NAS_0049_RSP_T10, &rsp_msg_p.t10);
      dsm_free_packet( response );
      return QMI_ERR_NO_MEMORY;
    }
  }

  if ( p_msg->t02_valid )
  {
    memscpy((void*)&rsp_msg_p.t02.result_code, sizeof(p_msg->t02.result_code), 
            (void*)&p_msg->t02.result_code, sizeof(p_msg->t02.result_code));
    memscpy((void*)&rsp_msg_p.t02.error_code, sizeof(p_msg->t02.error_code),
            (void*)&p_msg->t02.error_code, sizeof(p_msg->t02.error_code));

    if ( !qmi_svc_put_param_tlv( response, NAS_0049_RSP_T02, sizeof(rsp_msg_p.t02), &rsp_msg_p.t02 ) )
    {
      /*Could not write serving system to message: out of memory");*/
      qmi_err_write(NAS_0049_RSP_T02, &rsp_msg_p.t02);
      dsm_free_packet(response);
      return QMI_ERR_NO_MEMORY;
    }
  }
  
  return QMI_ERR_NONE;
}

/*=========================================================================== 
  FUNCTION QMI_NAS_004A_IND_WRITE() 
 
  DESCRIPTION 
    Writes QMI_NAS_THERMAL_EMERGENCY_STATE_IND indication in struct nas_004A_ind_s
     
  RETURN VALUE 
    QMI_ERR_NONE for no error or appropriate error code. 
     
  DEPENDENCIES 
    None 
 
  SIDE EFFECTS 
    None 
===========================================================================*/ 
qmi_error_e_type qmi_nas_004A_ind_write( struct nas_004A_ind_s *p_msg, dsm_item_type **response )
{
  struct nas_004A_ind_p_s  ind_msg;

  if (p_msg == NULL || response == NULL)
  {
    return QMI_ERR_INTERNAL;
  }

  memset( &ind_msg, 0, sizeof(ind_msg) );

  if ( p_msg->t01_valid )
  {
    memscpy((void*)&ind_msg.t01.thermal_emergency_mode, sizeof(ind_msg.t01.thermal_emergency_mode),
            (void*)&p_msg->t01.thermal_emergency_mode, sizeof(ind_msg.t01.thermal_emergency_mode));

    if ( !qmi_svc_put_param_tlv(response, NAS_004A_IND_T01, sizeof(ind_msg.t01), &ind_msg.t01) )
    {
      qmi_err_write(NAS_004A_IND_T01,  &ind_msg.t01);
      dsm_free_packet(response);
      return QMI_ERR_NO_MEMORY;
    }
  }
  else
  {
    QM_MSG_ERROR("mandatory TLV missing");
    return QMI_ERR_MISSING_ARG;
  }

  return QMI_ERR_NONE;
} /* qmi_nas_004A_ind_write */

/*=========================================================================== 
  FUNCTION QMI_NAS_004B_REQ_CHECK() 
 
  DESCRIPTION 
    Checks QMI_NAS_SET_DUAL_STANDBY_PREF in struct nas_004B_req_s for errors 
     
  RETURN VALUE 
    QMI_ERR_NONE for no error or appropriate error code. 
     
  DEPENDENCIES 
    None 
 
  SIDE EFFECTS 
    None 
===========================================================================*/ 
qmi_error_type_v01 qmi_nas_004B_req_check (nas_set_dual_standby_pref_req_msg_v01 *p_msg)
{
  if ( !p_msg->standby_pref_valid && !p_msg->priority_subs_valid && !p_msg->default_data_subs_valid && !p_msg->default_voice_subs_valid && !p_msg->active_subs_mask_valid )
  {
    return QMI_ERR_INVALID_ARG_V01;
  }

  if ( p_msg->standby_pref_valid )
  {
    if ( p_msg->standby_pref != NAS_004B_REQ_T10_AUTOMATIC_WITH_TUNE_AWAY &&
         p_msg->standby_pref != NAS_004B_REQ_T10_AUTOMATIC_WITHOUT_TUNE_AWAY )
    {
      QM_MSG_ERROR_1("Invalid standby pref %d", p_msg->standby_pref);
      return QMI_ERR_INVALID_ARG_V01;
    }
  }

  if ( p_msg->priority_subs_valid )
  {
    if ( p_msg->priority_subs != NAS_004B_REQ_PRIMARY_SUBSCRIPTION &&
         p_msg->priority_subs != NAS_004B_REQ_SECONDARY_SUBSCRIPTION &&
         p_msg->priority_subs != NAS_004B_REQ_TERTIARY_SUBSCRIPTION )
    {
      QM_MSG_ERROR_1("Invalid priority subscription %d", p_msg->priority_subs);
      return QMI_ERR_INVALID_ARG_V01;
    }
  }

  if ( p_msg->default_data_subs_valid )
  {
    if ( p_msg->default_data_subs != NAS_004B_REQ_PRIMARY_SUBSCRIPTION &&
         p_msg->default_data_subs != NAS_004B_REQ_SECONDARY_SUBSCRIPTION &&
         p_msg->default_data_subs != NAS_004B_REQ_TERTIARY_SUBSCRIPTION )
    {
      QM_MSG_ERROR_1("Invalid default_data subscription %d", p_msg->default_data_subs);
      return QMI_ERR_INVALID_ARG_V01;
    }
  }

  if ( p_msg->default_voice_subs_valid )
{
    if ( p_msg->default_voice_subs != NAS_004B_REQ_PRIMARY_SUBSCRIPTION &&
         p_msg->default_voice_subs != NAS_004B_REQ_SECONDARY_SUBSCRIPTION &&
         p_msg->default_voice_subs != NAS_004B_REQ_TERTIARY_SUBSCRIPTION )
  {
      QM_MSG_ERROR_1("Invalid default_voice subscription %d", p_msg->default_voice_subs);
      return QMI_ERR_INVALID_ARG_V01;
    }
  }
  
  if ( p_msg->active_subs_mask_valid && p_msg->standby_pref_valid )
  {
    QM_MSG_ERROR("Cannot set both active subs mask and standby pref");
    return QMI_ERR_INVALID_ARG_V01;
  }

  if ( p_msg->active_subs_mask_valid )
  {
    if ( p_msg->active_subs_mask != 0 &&
         !(p_msg->active_subs_mask & 
            (QMI_NAS_ACTIVE_SUB_PRIMARY_V01
             | QMI_NAS_ACTIVE_SUB_SECONDARY_V01
#ifdef FEATURE_TRIPLE_SIM
             | QMI_NAS_ACTIVE_SUB_TERTIARY_V01
#endif
             ) ) )
    {
      return QMI_ERR_INVALID_ARG_V01;
    }
  }
  
  if( p_msg->dds_duration_valid && p_msg->dds_duration != NAS_DDS_DURATION_PERMANANT_V01 && p_msg->dds_duration != NAS_DDS_DURATION_TEMPORARY_V01)
  {
    QM_MSG_ERROR_1("Invalid dds_duration received %d", p_msg->dds_duration);
    return QMI_ERR_INVALID_ARG_V01;
  }
  
  return QMI_ERR_NONE_V01;
} /* qmi_nas_004B_req_check */

/*=========================================================================== 
  FUNCTION QMI_NAS_004C_IND_WRITE() 
 
  DESCRIPTION 
    Writes QMI_NAS_NETWORK_TIME_IND indication in struct nas_004C_ind_s
     
  RETURN VALUE 
    QMI_ERR_NONE for no error or appropriate error code. 
     
  DEPENDENCIES 
    None 
 
  SIDE EFFECTS 
    None 
===========================================================================*/ 
qmi_error_e_type qmi_nas_004C_ind_write( struct nas_004C_ind_s *p_msg, dsm_item_type **response )
{
  struct nas_004C_ind_p_s  ind_msg;

  if (p_msg == NULL || response == NULL)
  {
    return QMI_ERR_INTERNAL;
  }

  memset( &ind_msg, 0, sizeof(ind_msg) );

  if ( p_msg->t12_valid )
  {
    memscpy((void*)&ind_msg.t12.radio_if, sizeof(ind_msg.t12.radio_if),
            (void*)&p_msg->t12.radio_if, sizeof(ind_msg.t12.radio_if));

    if ( !qmi_svc_put_param_tlv(response, NAS_004C_IND_T12, sizeof(ind_msg.t12), &ind_msg.t12) )
    {
      /*ind_msg write error, radio_if*/
      qmi_err_write(NAS_004C_IND_T12, &ind_msg.t12);
      dsm_free_packet(response);
      return QMI_ERR_NO_MEMORY;
    }
  }

  if ( p_msg->t11_valid )
  {
    memscpy((void*)&ind_msg.t11.daylt_sav_adj, sizeof(ind_msg.t11.daylt_sav_adj),
            (void*)&p_msg->t11.daylt_sav_adj, sizeof(ind_msg.t11.daylt_sav_adj));

    if ( !qmi_svc_put_param_tlv(response, NAS_004C_IND_T11, sizeof(ind_msg.t11), &ind_msg.t11) )
    {
      qmi_err_write(NAS_004C_IND_T11, &ind_msg.t11);
      dsm_free_packet(response);
      return QMI_ERR_NO_MEMORY;
    }
  }
  
  if ( p_msg->t10_valid )
  {
    memscpy((void*)&ind_msg.t10.time_zone, sizeof(ind_msg.t10.time_zone),
            (void*)&p_msg->t10.time_zone, sizeof(p_msg->t10.time_zone));

    if ( !qmi_svc_put_param_tlv(response, NAS_004C_IND_T10, sizeof(ind_msg.t10), &ind_msg.t10) )
    {
      qmi_err_write(NAS_004C_IND_T10, &ind_msg.t10);
      dsm_free_packet(response);
      return QMI_ERR_NO_MEMORY;
    }
  }
  
  if ( p_msg->t01_valid )
  {
    memscpy((void*)&ind_msg.t01.year, sizeof(ind_msg.t01.year),
            (void*)&p_msg->t01.year, sizeof(ind_msg.t01.year));
    memscpy((void*)&ind_msg.t01.month, sizeof(ind_msg.t01.month),
            (void*)&p_msg->t01.month, sizeof(ind_msg.t01.month));
    memscpy((void*)&ind_msg.t01.day, sizeof(ind_msg.t01.day),
            (void*)&p_msg->t01.day, sizeof(ind_msg.t01.day));
    memscpy((void*)&ind_msg.t01.hour, sizeof(ind_msg.t01.hour),
            (void*)&p_msg->t01.hour, sizeof(ind_msg.t01.hour));
    memscpy((void*)&ind_msg.t01.minute, sizeof(ind_msg.t01.minute),
            (void*)&p_msg->t01.minute, sizeof(ind_msg.t01.minute));
    memscpy((void*)&ind_msg.t01.second, sizeof(ind_msg.t01.second),
            (void*)&p_msg->t01.second, sizeof(ind_msg.t01.second));
    memscpy((void*)&ind_msg.t01.day_of_week, sizeof(ind_msg.t01.day_of_week),
            (void*)&p_msg->t01.day_of_week, sizeof(ind_msg.t01.day_of_week));

    if ( !qmi_svc_put_param_tlv(response, NAS_004C_IND_T01, sizeof(ind_msg.t01), &ind_msg.t01) )
    {
      qmi_err_write(NAS_004C_IND_T01, &ind_msg.t01);
      dsm_free_packet(response);
      return QMI_ERR_NO_MEMORY;
    }
  }

  return QMI_ERR_NONE;
} /* qmi_nas_004C_ind_write */

/*=========================================================================== 
  FUNCTION QMI_NAS_0050_REQ_CHECK() 
 
  DESCRIPTION 
    Checks QMI_NAS_CONFIG_SIG_INFO_REQ_MSG in nas_config_sig_info_req_msg_v01 for errors
     
  RETURN VALUE 
    QMI_ERR_NONE_V01 for no error or appropriate error code.
     
  DEPENDENCIES 
    None 
 
  SIDE EFFECTS 
    None 
===========================================================================*/ 
qmi_error_type_v01 qmi_nas_0050_req_check(
  nas_config_sig_info_req_msg_v01 *p_msg
) 
{ 
  // If ALL optional TLVs are absent return error 
  if ( ! p_msg->rssi_threshold_list_valid     && ! p_msg->ecio_threshold_list_valid && ! p_msg->hdr_sinr_threshold_list_valid &&
       ! p_msg->lte_snr_threshold_list_valid  && ! p_msg->io_threshold_list_valid   && ! p_msg->lte_rsrq_threshold_list_valid &&
       ! p_msg->lte_rsrp_threshold_list_valid && ! p_msg->lte_sig_rpt_config_valid  && ! p_msg->rscp_threshold_list_valid     &&
       ! p_msg->tds_sinr_threshold_list_valid )
  { 
    return QMI_ERR_MISSING_ARG_V01;
  } 

  if ( ( p_msg->rssi_threshold_list_valid     && p_msg->rssi_threshold_list_len     ==  0) ||
       ( p_msg->ecio_threshold_list_valid     && p_msg->ecio_threshold_list_len     ==  0) ||
       ( p_msg->hdr_sinr_threshold_list_valid && p_msg->hdr_sinr_threshold_list_len ==  0) ||
       ( p_msg->lte_snr_threshold_list_valid  && p_msg->lte_snr_threshold_list_len  ==  0) ||
       ( p_msg->io_threshold_list_valid       && p_msg->io_threshold_list_len       ==  0) ||
       ( p_msg->lte_rsrq_threshold_list_valid && p_msg->lte_rsrq_threshold_list_len ==  0) ||
       ( p_msg->lte_rsrp_threshold_list_valid && p_msg->lte_rsrp_threshold_list_len ==  0) ||
       ( p_msg->rscp_threshold_list_valid     && p_msg->rscp_threshold_list_len     ==  0) ||
       ( p_msg->tds_sinr_threshold_list_valid && p_msg->tds_sinr_threshold_list_len ==  0) )
  {
    return QMI_ERR_NO_THRESHOLDS_V01;
  }

  if ( p_msg->lte_sig_rpt_config_valid )
  {
    if ( p_msg->lte_sig_rpt_config.rpt_rate   > NAS_LTE_SIG_RPT_RATE_5_SEC_V01 ||
         p_msg->lte_sig_rpt_config.avg_period > NAS_LTE_SIG_AVG_PRD_5_SEC_V01  )
    {
      return QMI_ERR_INVALID_ARG_V01;
    }
  }

  return QMI_ERR_NONE_V01;
}  // qmi_nas_0050_req_check

//=========================================================================== 
// FUNCTION: qmi_nas_0054_ind_write
//
// DESCRIPTION: 
//   Write QMI_NAS_HDR_SESSION_CLOSE_IND message
//    
// RETURN VALUE: 
//   QMI_ERR_NONE - success. 
//    
//=========================================================================== 
//
qmi_error_e_type qmi_nas_0054_ind_write (struct nas_0054_ind_s *p_msg, dsm_item_type **response)
{
   struct nas_0054_ind_p_s nas_0054_ind_p;

   if (p_msg == NULL || response == NULL) {
      QM_MSG_ERROR_2("Detected null pointer %p, %p", p_msg, response);
      return QMI_ERR_INTERNAL;
   }

   memset(&nas_0054_ind_p, 0, sizeof(nas_0054_ind_p));

   if ( p_msg->t01_valid ) {
      if (p_msg->t01.close_reason >= NAS_HDR_CLOSE_REASON_MAX) {
         QM_MSG_ERROR_1("Invalid HDR close reason %d", p_msg->t01.close_reason);
         return QMI_ERR_INTERNAL;
      }
      nas_0054_ind_p.t01.close_reason = p_msg->t01.close_reason;

      if ( !qmi_svc_put_param_tlv( response, 0x01, sizeof(nas_0054_ind_p.t01), &nas_0054_ind_p.t01))
      {
         /*Could not write uati ind: Out of memory*/
         qmi_err_write(0x01, &nas_0054_ind_p.t01); 
         dsm_free_packet(response);
         return QMI_ERR_NO_MEMORY;
      }
   }

    return QMI_ERR_NONE;
}

//=========================================================================== 
// FUNCTION: qmi_nas_0055_ind_write
//
// DESCRIPTION: 
//   Write QMI_NAS_HDR_UATI_IND message
//    
// RETURN VALUE: 
//   QMI_ERR_NONE - success. 
//    
//=========================================================================== 
//
qmi_error_e_type qmi_nas_0055_ind_write (struct nas_0055_ind_s *p_msg, dsm_item_type **response)
{
   struct nas_0055_ind_p_s nas_0055_ind_p;

   if (p_msg == NULL || response == NULL) {
      QM_MSG_ERROR_2("Detected null pointer %p, %p", p_msg, response);
      return QMI_ERR_INTERNAL;
   }

   memset(&nas_0055_ind_p, 0, sizeof(nas_0055_ind_p));

   if ( p_msg->t01_valid ) {
      memscpy((void*)&nas_0055_ind_p.t01.uati, sizeof(nas_0055_ind_p.t01.uati),
              (void*)&p_msg->t01.uati, sizeof(nas_0055_ind_p.t01.uati));

      if ( !qmi_svc_put_param_tlv( response, 0x01, sizeof(nas_0055_ind_p.t01), &nas_0055_ind_p.t01))
      {
         /*Could not write uati ind: Out of memory*/
         qmi_err_write(0x01, &nas_0055_ind_p.t01);
         dsm_free_packet(response);
         return QMI_ERR_NO_MEMORY;
      }
   }

    return QMI_ERR_NONE;
}

//=========================================================================== 
// FUNCTION: qmi_nas_0056_req_read
//
// DESCRIPTION: 
//   Reads QMI_NAS_GET_HDR_SUBTYPE input params into struct 
//    
// RETURN VALUE: 
//   QMI_ERR_NONE - success. 
//   QMI_ERR_MALFORMED_MSG - invalid tlvs
//   QMI_ERR_MISSING_ARG - missing required arg
//    
//=========================================================================== 
//
qmi_error_e_type qmi_nas_0056_req_read ( 
  struct nas_0056_req_s  *p_msg, 
  dsm_item_type         **sdu_in 
) 
{
  uint8                 prm_type;
  uint16                len;
  uint16                expected_len;
  void *                value;
  struct nas_0056_req_p_s req_p;

  memset(&req_p, 0, sizeof(req_p));
  
  while (*sdu_in)
  {
    if ( !qmi_svc_get_tl(sdu_in, &prm_type, &len) )
    {
      continue;
    }

    expected_len = 0;
    value = NULL;

    // special value 0 = variable length or don't care (unrecognzied TLV)
    switch (prm_type)
    {
      case 0x01:
        expected_len = sizeof(req_p.t01);
        value = &req_p.t01;
        p_msg->t01_valid = TRUE;
        break;

      default:
        QM_MSG_ERROR_1("Unrecognized TLV type (%d)", prm_type);
        break;
    }

    if ((expected_len != 0) && (expected_len != len))
    {
      QM_MSG_ERROR_2("Invalid TLV len (%d) for type (%d)", len, prm_type);
      return QMI_ERR_MALFORMED_MSG;
    }

    if (len != dsm_pullup( sdu_in, value, len ))
    {
        qmi_nas_invalid_len_tlv();
      return QMI_ERR_MALFORMED_MSG;
    }
  }
  
  if (!p_msg->t01_valid)
  {
    QM_MSG_ERROR("Missing mandatory parameter: protocol");
    return QMI_ERR_MISSING_ARG;
  }
  else
  {
    memscpy(&p_msg->t01.protocol, sizeof(req_p.t01.protocol), 
            (void*)&req_p.t01.protocol, sizeof(req_p.t01.protocol));
  }
    
  return QMI_ERR_NONE;
}

//=========================================================================== 
// FUNCTION: qmi_nas_0056_req_check
//
// DESCRIPTION: 
//   Validate request input parameter(s)
//    
// RETURN:
//   QMI_ERR_INTERNAL - internal error (arg struct not provided)
//   QMI_ERR_MISSING_ARG - missing required arg
//   QMI_ERR_INVALID_ARG - arg not in valid range
//   QMI_ERR_NONE  - success.
//    
//=========================================================================== 
//
qmi_error_e_type qmi_nas_0056_req_check (
  struct nas_0056_req_s *p_msg
)
{
   if (p_msg == NULL) {
      return QMI_ERR_INTERNAL;
   }

   if ( !p_msg->t01_valid )
   {
     return QMI_ERR_MISSING_ARG;
   }

   if ( p_msg->t01.protocol >=  HDRHAI_EXTERNAL_PROTOCOL_NAME_ENUM_MAX )
   {
     return QMI_ERR_INVALID_ARG;
   }

   return QMI_ERR_NONE;
}

//=========================================================================== 
// FUNCTION: qmi_nas_0056_rsp_write
//
// DESCRIPTION: 
//    Fill output response packet
//    
// RETURN:
//   QMI_ERR_NONE  - success
//   QMI_ERR_INTERNAL - invalid inputs
//   QMI_ERR_NO_MEMORY - failed to allocate memory for response
//    
//=========================================================================== 
//
qmi_error_e_type qmi_nas_0056_rsp_write( struct nas_0056_rsp_s *p_msg, dsm_item_type **response )
{
  struct nas_0056_rsp_p_s rsp_msg_p;

  if ( !p_msg || !response )
  {
    return QMI_ERR_INTERNAL;
  }

  memset( &rsp_msg_p, 0x00, sizeof(rsp_msg_p) );

  if ( p_msg->t10_valid )
  {
    rsp_msg_p.t10.subtype = p_msg->t10.subtype;

    if ( !qmi_svc_put_param_tlv( response, 0x10, sizeof(rsp_msg_p.t10), &rsp_msg_p.t10 ) )
    {
      QM_MSG_ERROR_1("Failed to put color code TLV %x", 0x10);
      dsm_free_packet( response );
      return QMI_ERR_NO_MEMORY;
    }
  }
  
  if ( p_msg->t02_valid )
  {
    memscpy((void*)&rsp_msg_p.t02.result_code, sizeof(p_msg->t02.result_code),
            (void*)&p_msg->t02.result_code, sizeof(p_msg->t02.result_code));
    memscpy((void*)&rsp_msg_p.t02.error_code, sizeof(p_msg->t02.error_code),
            (void*)&p_msg->t02.error_code, sizeof(p_msg->t02.error_code));

    if ( !qmi_svc_put_param_tlv( response, 0x02, sizeof(rsp_msg_p.t02), &rsp_msg_p.t02 ) )
    {
      /*Could not write serving system to message: out of memory*/
      qmi_err_write(0x02, &rsp_msg_p.t02);
      dsm_free_packet(response);
      return QMI_ERR_NO_MEMORY;
    }
  }
  
  return QMI_ERR_NONE;
}


//=========================================================================== 
// FUNCTION: qmi_nas_0057_rsp_write 
//
// DESCRIPTION: 
//    Fill output response packet
//    
// RETURN:
//   QMI_ERR_NONE  - success
//   QMI_ERR_INTERNAL - invalid inputs
//   QMI_ERR_NO_MEMORY - failed to allocate memory for response
//    
//=========================================================================== 
//
qmi_error_e_type qmi_nas_0057_rsp_write( struct nas_0057_rsp_s *p_msg, dsm_item_type **response )
{
  struct nas_0057_rsp_p_s rsp_msg_p;

  if ( !p_msg || !response )
  {
    return QMI_ERR_INTERNAL;
  }

  memset( &rsp_msg_p, 0x00, sizeof(rsp_msg_p) );

  if ( p_msg->t10_valid )
  {
    rsp_msg_p.t10.color_code = p_msg->t10.color_code;

    if ( !qmi_svc_put_param_tlv( response, 0x10, sizeof(rsp_msg_p.t10), &rsp_msg_p.t10 ) )
    {
      QM_MSG_ERROR_1("Failed to put color code TLV %x", 0x10);
      dsm_free_packet( response );
      return QMI_ERR_NO_MEMORY;
    }
  }
  
  if ( p_msg->t02_valid )
  {
    memscpy((void*)&rsp_msg_p.t02.result_code, sizeof(p_msg->t02.result_code),
            (void*)&p_msg->t02.result_code, sizeof(p_msg->t02.result_code));
    memscpy((void*)&rsp_msg_p.t02.error_code, sizeof(p_msg->t02.error_code),
            (void*)&p_msg->t02.error_code, sizeof(p_msg->t02.error_code));

    if ( !qmi_svc_put_param_tlv( response, 0x02, sizeof(rsp_msg_p.t02), &rsp_msg_p.t02 ) )
    {
      /*Could not write serving system to message: out of memory*/
      qmi_err_write(0x02, &rsp_msg_p.t02);
      dsm_free_packet(response);
      return QMI_ERR_NO_MEMORY;
    }
  }
  
  return QMI_ERR_NONE;
}


/*=========================================================================== 
  FUNCTION QMI_NAS_0059_REQ_READ() 
 
  DESCRIPTION 
    Reads QMI_NAS_SET_RX_DIVERSITY_REQ into struct nas_0059_req_s 
     
  RETURN VALUE 
    QMI_ERR_NONE for successful read. For failure, returns appropriate error 
    value. 
     
  DEPENDENCIES 
    None 
 
  SIDE EFFECTS 
    None 
===========================================================================*/ 
qmi_error_e_type qmi_nas_0059_req_read( struct nas_0059_req_s *p_msg, dsm_item_type **sdu_in ) 
{
  uint8   type; 
  uint16  len, expected_len; 
  void   *value; 
  struct nas_0059_req_p_s  nas_0059_req_p;
  
  qmi_error_e_type errval = QMI_ERR_NONE; 

  memset(&nas_0059_req_p, 0, sizeof(nas_0059_req_p));

  while (*sdu_in) 
  { 
    if ( !qmi_svc_get_tl(sdu_in, &type, &len) ) 
      continue; 
 
    // special value 0 = variable length or don't care (unrecognzied TLV) 
    expected_len = 0; 
    value = NULL; 
 
    switch (type) 
    { 
      case 0x01:
        value = &nas_0059_req_p.t01;
        p_msg->t01_valid = TRUE; 
        expected_len = sizeof(nas_0059_req_p.t01);
        break; 

      default: 
        QM_MSG_ERROR_1("Unrecognized TLV type (%d)", type);
        break; 
    } 
 
    if ((expected_len != 0) && (expected_len != len)) 
    { 
      QM_MSG_ERROR_2("Invalid TLV len (%d) for type (%d)", len, type);
      errval = QMI_ERR_MALFORMED_MSG; 
      break; 
    } 
 
    /*----------------------------------------------------------------------- 
      If type is unrecognized, value will be NULL, and dsm_pullup will  
      free the unrecognized value, i.e. we skip past unrecognized TLVs 
    -----------------------------------------------------------------------*/ 
    if (len != dsm_pullup( sdu_in, value, len )) 
    { 
      qmi_nas_invalid_len_tlv();
      errval = QMI_ERR_MALFORMED_MSG; 
      break; 
    } 
  }

  if ( errval == QMI_ERR_NONE && p_msg->t01_valid )
  {
    memscpy(&p_msg->t01.radio_if, sizeof(nas_0059_req_p.t01.radio_if),
           (void*)&nas_0059_req_p.t01.radio_if, sizeof(nas_0059_req_p.t01.radio_if));
    memscpy(&p_msg->t01.rx_chain_bitmask, sizeof(nas_0059_req_p.t01.rx_chain_bitmask),
           (void*)&nas_0059_req_p.t01.rx_chain_bitmask, sizeof(nas_0059_req_p.t01.rx_chain_bitmask));
  }
 
  return errval; 
} /* qmi_nas_0059_req_read */
 
/*=========================================================================== 
  FUNCTION QMI_NAS_0059_REQ_CHECK() 
 
  DESCRIPTION 
    Checks QMI_NAS_SET_RX_DIVERSITY_REQ in struct nas_0059_req_s for errors 
     
  RETURN VALUE 
    QMI_ERR_NONE for no error or appropriate error code. 
     
  DEPENDENCIES 
    None 
 
  SIDE EFFECTS 
    None 
===========================================================================*/ 
qmi_error_e_type qmi_nas_0059_req_check (struct nas_0059_req_s *p_msg) 
{ 
  if ( !p_msg->t01_valid ) // missing mandatory TLV
  { 
    return QMI_ERR_MISSING_ARG; 
  }
  else
  {
    if ( p_msg->t01.radio_if != QMI_NAS_RADIO_IF_CDMA20001X &&
         p_msg->t01.radio_if != QMI_NAS_RADIO_IF_CDMA20001XEVDO &&
         p_msg->t01.radio_if != QMI_NAS_RADIO_IF_GSM &&
         p_msg->t01.radio_if != QMI_NAS_RADIO_IF_UMTS &&
         p_msg->t01.radio_if != QMI_NAS_RADIO_IF_LTE )
    {
      QM_MSG_ERROR_1("qmi_nas_0059_req_check(): Invalid radio_if = %d", p_msg->t01.radio_if);
      return QMI_ERR_INVALID_ARG;
    }
    if( p_msg->t01.rx_chain_bitmask > (CMAPI_BM_RX_CHAIN_0 | CMAPI_BM_RX_CHAIN_1 | CMAPI_BM_RX_CHAIN_2 | CMAPI_BM_RX_CHAIN_3) )
    {
      QM_MSG_ERROR_1("qmi_nas_0059_req_check(): Invalid rx_chain_bitmask = %d", p_msg->t01.rx_chain_bitmask);
      return QMI_ERR_INVALID_ARG;
    }
  }

  return QMI_ERR_NONE; 
} /* qmi_nas_0059_req_check */

/*=========================================================================== 
  FUNCTION QMI_NAS_0059_RSP_WRITE() 

  DESCRIPTION 
  Writes QMI_NAS_SET_RX_DIVERSITY_RESP response in struct nas_0059_rsp_s

  RETURN VALUE 
  QMI_ERR_NONE for no error or appropriate error code. 

  DEPENDENCIES 
  None 

  SIDE EFFECTS 
  None 
  ===========================================================================*/ 
qmi_error_e_type qmi_nas_0059_rsp_write( struct nas_0059_rsp_s *p_msg, dsm_item_type **p_response )
{
  struct nas_0059_rsp_p_s rsp_p;

  if (p_msg == NULL || p_response == NULL)
  {
    return QMI_ERR_INTERNAL;
  }

  memset(&rsp_p, 0, sizeof(rsp_p));

  if ( p_msg->t02_valid )
  {
    memscpy((void*)&rsp_p.t02.result_code, sizeof(p_msg->t02.result_code),
            (void*)&p_msg->t02.result_code, sizeof(p_msg->t02.result_code));
    memscpy((void*)&rsp_p.t02.error_code, sizeof(p_msg->t02.error_code),
            (void*)&p_msg->t02.error_code, sizeof(p_msg->t02.error_code));

    if ( !qmi_svc_put_param_tlv( p_response, 0x02, sizeof(rsp_p.t02), &rsp_p.t02 ) )
    {
      /*Could not write signal strength result code to message: out of memory*/
      qmi_err_write(0x02, &rsp_p.t02);
      dsm_free_packet(p_response);
      return QMI_ERR_NO_MEMORY;
    }    
  }

  return QMI_ERR_NONE;
} /* qmi_nas_0059_rsp_write */

/*=========================================================================== 
  FUNCTION QMI_NAS_005A_REQ_READ() 
 
  DESCRIPTION 
    Reads QMI_NAS_GET_TX_RX_INFO_REQ into struct nas_005A_req_s 
     
  RETURN VALUE 
    QMI_ERR_NONE for successful read. For failure, returns appropriate error 
    value. 
     
  DEPENDENCIES 
    None 
 
  SIDE EFFECTS 
    None 
===========================================================================*/ 
qmi_error_e_type qmi_nas_005A_req_read( struct nas_005A_req_s *p_msg, dsm_item_type **sdu_in ) 
{ 
  uint8   type; 
  uint16  len, expected_len; 
  void   *value; 
  struct nas_005A_req_p_s  nas_005A_req_p;
  
  qmi_error_e_type errval = QMI_ERR_NONE; 

  memset(&nas_005A_req_p, 0, sizeof(nas_005A_req_p));

  while (*sdu_in) 
  { 
    if ( !qmi_svc_get_tl(sdu_in, &type, &len) ) 
      continue; 
 
    // special value 0 = variable length or don't care (unrecognzied TLV) 
    expected_len = 0; 
    value = NULL; 
 
    switch (type) 
    { 
      case 0x01:
        value = &nas_005A_req_p.t01;
        p_msg->t01_valid = TRUE; 
        expected_len = sizeof(nas_005A_req_p.t01);
        break; 

      default: 
        QM_MSG_ERROR_1("Unrecognized TLV type (%d)", type);
        break; 
    } 
 
    if ((expected_len != 0) && (expected_len != len)) 
    { 
      QM_MSG_ERROR_2("Invalid TLV len (%d) for type (%d)", len, type);
      errval = QMI_ERR_MALFORMED_MSG; 
      break; 
    } 
 
    /*----------------------------------------------------------------------- 
      If type is unrecognized, value will be NULL, and dsm_pullup will  
      free the unrecognized value, i.e. we skip past unrecognized TLVs 
    -----------------------------------------------------------------------*/ 
    if (len != dsm_pullup( sdu_in, value, len )) 
    { 
        qmi_nas_invalid_len_tlv();
      errval = QMI_ERR_MALFORMED_MSG; 
      break; 
    } 
  }

  if ( errval == QMI_ERR_NONE && p_msg->t01_valid )
  {
    memscpy(&p_msg->t01.radio_if, sizeof(nas_005A_req_p.t01.radio_if),
           (void*)&nas_005A_req_p.t01.radio_if, sizeof(nas_005A_req_p.t01.radio_if));
  }
 
  return errval; 
}

/*=========================================================================== 
  FUNCTION QMI_NAS_005A_REQ_CHECK() 
 
  DESCRIPTION 
    Checks QMI_NAS_GET_TX_RX_INFO_REQ in struct nas_005A_req_s for errors 
     
  RETURN VALUE 
    QMI_ERR_NONE for no error or appropriate error code. 
     
  DEPENDENCIES 
    None 
 
  SIDE EFFECTS 
    None 
===========================================================================*/ 
qmi_error_e_type qmi_nas_005A_req_check (nas_get_tx_rx_info_req_msg_v01 *p_msg) 
{ 

  if ( p_msg->radio_if != NAS_RADIO_IF_CDMA_1X_V01 &&
       p_msg->radio_if != NAS_RADIO_IF_CDMA_1XEVDO_V01 &&
       p_msg->radio_if != NAS_RADIO_IF_GSM_V01 &&
       p_msg->radio_if != NAS_RADIO_IF_UMTS_V01 &&
       p_msg->radio_if != NAS_RADIO_IF_LTE_V01 &&
       p_msg->radio_if != NAS_RADIO_IF_TDSCDMA_V01)
    {
      return QMI_ERR_INVALID_ARG;
    }
  return QMI_ERR_NONE; 
} /* qmi_nas_005A_req_check */

qmi_error_e_type qmi_nas_005A_rsp_write(struct nas_005A_rsp_s *p_msg, dsm_item_type **response);

/*=========================================================================== 
  FUNCTION QMI_NAS_005A_RSP_WRITE() 

  DESCRIPTION 
  Writes QMI_NAS_GET_TX_RX_INFO_RESP response in struct nas_005A_rsp_s

  RETURN VALUE 
  QMI_ERR_NONE for no error or appropriate error code. 

  DEPENDENCIES 
  None 

  SIDE EFFECTS 
  None 
  ===========================================================================*/ 
qmi_error_e_type qmi_nas_005A_rsp_write( struct nas_005A_rsp_s *p_msg, dsm_item_type **p_response )
{
  struct nas_005A_rsp_p_s rsp_p;
  size_t copy_size;

  if (p_msg == NULL || p_response == NULL)
  {
    return QMI_ERR_INTERNAL;
  }

  memset(&rsp_p, 0, sizeof(rsp_p));

  if ( p_msg->t16_valid )
  {
    memscpy((void*)&rsp_p.t16.is_radio_tuned, sizeof(p_msg->t16.is_radio_tuned),
            (void*)&p_msg->t16.is_radio_tuned, sizeof(p_msg->t16.is_radio_tuned));
    memscpy((void*)&rsp_p.t16.rx_pwr, sizeof(p_msg->t16.rx_pwr),
            (void*)&p_msg->t16.rx_pwr,         sizeof(p_msg->t16.rx_pwr));
    memscpy((void*)&rsp_p.t16.ecio, sizeof(p_msg->t16.ecio),
            (void*)&p_msg->t16.ecio, sizeof(p_msg->t16.ecio));
    memscpy((void*)&rsp_p.t16.rscp, sizeof(p_msg->t16.rscp),
            (void*)&p_msg->t16.rscp, sizeof(p_msg->t16.rscp));
    memscpy((void*)&rsp_p.t16.rsrp, sizeof(p_msg->t16.rsrp),
            (void*)&p_msg->t16.rsrp, sizeof(p_msg->t16.rsrp));
    memscpy((void*)&rsp_p.t16.phase, sizeof(rsp_p.t16.phase),
            (void*)&p_msg->t16.phase, sizeof(p_msg->t16.phase));

    if ( !qmi_svc_put_param_tlv( p_response, 0x16, sizeof(rsp_p.t16), &rsp_p.t16 ) )
    {
      /*Could not write tlv16 to message: out of memory*/
      qmi_err_write(0x16, &rsp_p.t16 ); 
      dsm_free_packet(p_response);
      return QMI_ERR_NO_MEMORY;
    }    
  }

  if ( p_msg->t15_valid )
  {
    memscpy((void*)&rsp_p.t15.is_radio_tuned, sizeof(p_msg->t15.is_radio_tuned),
            (void*)&p_msg->t15.is_radio_tuned, sizeof(p_msg->t15.is_radio_tuned));
    memscpy((void*)&rsp_p.t15.rx_pwr, sizeof(p_msg->t15.rx_pwr),
            (void*)&p_msg->t15.rx_pwr,         sizeof(p_msg->t15.rx_pwr));
    memscpy((void*)&rsp_p.t15.ecio, sizeof(p_msg->t15.ecio),
            (void*)&p_msg->t15.ecio, sizeof(p_msg->t15.ecio));
    memscpy((void*)&rsp_p.t15.rscp, sizeof(p_msg->t15.rscp),
            (void*)&p_msg->t15.rscp, sizeof(p_msg->t15.rscp));
    memscpy((void*)&rsp_p.t15.rsrp, sizeof(p_msg->t15.rsrp),
            (void*)&p_msg->t15.rsrp, sizeof(p_msg->t15.rsrp));
    memscpy((void*)&rsp_p.t15.phase, sizeof(rsp_p.t15.phase),
            (void*)&p_msg->t15.phase, sizeof(p_msg->t15.phase));

    if ( !qmi_svc_put_param_tlv( p_response, 0x15, sizeof(rsp_p.t15), &rsp_p.t15 ) )
    {
      /*Could not write tlv15 to message: out of memory*/
      qmi_err_write(0x15, &rsp_p.t15); 
      dsm_free_packet(p_response);
      return QMI_ERR_NO_MEMORY;
    }    
  }

  if ( p_msg->t14_valid )
  {
    copy_size = 0;
    copy_size += memscpy((void*)&rsp_p.t14.uplink_mod_len, sizeof(p_msg->t14.uplink_mod_len),
                         (void*)&p_msg->t14.uplink_mod_len, sizeof(p_msg->t14.uplink_mod_len));
    copy_size += memscpy((void*)&rsp_p.t14.uplink_mod, NAS_LTE_MAX_NUM_LAYER*sizeof(p_msg->t14.uplink_mod[0]),
                         (void*)&p_msg->t14.uplink_mod, p_msg->t14.uplink_mod_len*sizeof(p_msg->t14.uplink_mod[0]));

    if ( !qmi_svc_put_param_tlv( p_response, 0x14, copy_size, &rsp_p.t14 ) )
    {
      /*Could not write tlv14 to message: out of memory*/
      qmi_err_write(0x14, &rsp_p.t14); 
      dsm_free_packet(p_response);
      return QMI_ERR_NO_MEMORY;
    }
  }

  if ( p_msg->t13_valid )
  {
    copy_size = 0;
    copy_size += memscpy((void*)&rsp_p.t13.downlink_mod_len, sizeof(p_msg->t13.downlink_mod_len),
                         (void*)&p_msg->t13.downlink_mod_len, sizeof(p_msg->t13.downlink_mod_len));
    copy_size += memscpy((void*)&rsp_p.t13.downlink_mod, NAS_LTE_MAX_NUM_LAYER*sizeof(p_msg->t13.downlink_mod[0]),
                         (void*)&p_msg->t13.downlink_mod, p_msg->t13.downlink_mod_len*sizeof(p_msg->t13.downlink_mod[0]));

    if ( !qmi_svc_put_param_tlv( p_response, 0x13, copy_size, &rsp_p.t13 ) )
    {
      /*Could not write tlv13 to message: out of memory*/
      qmi_err_write(0x13, &rsp_p.t13);
      dsm_free_packet(p_response);
      return QMI_ERR_NO_MEMORY;
    }
  }

  if ( p_msg->t12_valid )
  {
    memscpy((void*)&rsp_p.t12.is_in_traffic, sizeof(p_msg->t12.is_in_traffic),
            (void*)&p_msg->t12.is_in_traffic, sizeof(p_msg->t12.is_in_traffic));
    memscpy((void*)&rsp_p.t12.tx_pwr, sizeof(p_msg->t12.tx_pwr),
            (void*)&p_msg->t12.tx_pwr, sizeof(p_msg->t12.tx_pwr));

    if ( !qmi_svc_put_param_tlv( p_response, 0x12, sizeof(rsp_p.t12), &rsp_p.t12 ) )
    {
      /*Could not write tlv12 to message: out of memory*/
      qmi_err_write(0x12, &rsp_p.t12); 
      dsm_free_packet(p_response);
      return QMI_ERR_NO_MEMORY;
    }    
  }

  if ( p_msg->t11_valid )
  {
    memscpy((void*)&rsp_p.t11.is_radio_tuned, sizeof(p_msg->t11.is_radio_tuned),
            (void*)&p_msg->t11.is_radio_tuned, sizeof(p_msg->t11.is_radio_tuned));
    memscpy((void*)&rsp_p.t11.rx_pwr, sizeof(p_msg->t11.rx_pwr),
            (void*)&p_msg->t11.rx_pwr,         sizeof(p_msg->t11.rx_pwr));
    memscpy((void*)&rsp_p.t11.ecio, sizeof(p_msg->t11.ecio),
            (void*)&p_msg->t11.ecio, sizeof(p_msg->t11.ecio));
    memscpy((void*)&rsp_p.t11.rscp, sizeof(p_msg->t11.rscp),
            (void*)&p_msg->t11.rscp, sizeof(p_msg->t11.rscp));
    memscpy((void*)&rsp_p.t11.rsrp, sizeof(p_msg->t11.rsrp),
            (void*)&p_msg->t11.rsrp, sizeof(p_msg->t11.rsrp));
    memscpy((void*)&rsp_p.t11.phase, sizeof(rsp_p.t11.phase),
            (void*)&p_msg->t11.phase, sizeof(p_msg->t11.phase));

    if ( !qmi_svc_put_param_tlv( p_response, 0x11, sizeof(rsp_p.t11), &rsp_p.t11 ) )
    {
      /*Could not write tlv11 to message: out of memory*/
      qmi_err_write(0x11,&rsp_p.t11);  
      dsm_free_packet(p_response);
      return QMI_ERR_NO_MEMORY;
    }    
  }

  if ( p_msg->t10_valid )
  {
    memscpy((void*)&rsp_p.t10.is_radio_tuned, sizeof(p_msg->t10.is_radio_tuned),
            (void*)&p_msg->t10.is_radio_tuned, sizeof(p_msg->t10.is_radio_tuned));
    memscpy((void*)&rsp_p.t10.rx_pwr, sizeof(p_msg->t10.rx_pwr),
            (void*)&p_msg->t10.rx_pwr, sizeof(p_msg->t10.rx_pwr));
    memscpy((void*)&rsp_p.t10.ecio, sizeof(p_msg->t10.ecio),
            (void*)&p_msg->t10.ecio, sizeof(p_msg->t10.ecio));
    memscpy((void*)&rsp_p.t10.rscp, sizeof(p_msg->t10.rscp), 
            (void*)&p_msg->t10.rscp, sizeof(p_msg->t10.rscp));
    memscpy((void*)&rsp_p.t10.rsrp, sizeof(p_msg->t10.rsrp),
            (void*)&p_msg->t10.rsrp, sizeof(p_msg->t10.rsrp));
    memscpy((void*)&rsp_p.t10.phase, sizeof(p_msg->t10.phase),
            (void*)&p_msg->t10.phase, sizeof(p_msg->t10.phase));

    if ( !qmi_svc_put_param_tlv( p_response, 0x10, sizeof(rsp_p.t10), &rsp_p.t10 ) )
    {
      /*Could not write tlv10 to message: out of memory*/
      qmi_err_write(0x10, &rsp_p.t10 );  
      dsm_free_packet(p_response);
      return QMI_ERR_NO_MEMORY;
    }    
  }

  if ( p_msg->t02_valid )
  {
    memscpy((void*)&rsp_p.t02.result_code, sizeof(p_msg->t02.result_code),
            (void*)&p_msg->t02.result_code, sizeof(p_msg->t02.result_code));
    memscpy((void*)&rsp_p.t02.error_code, sizeof(p_msg->t02.error_code),
            (void*)&p_msg->t02.error_code, sizeof(p_msg->t02.error_code));

    if ( !qmi_svc_put_param_tlv( p_response, 0x02, sizeof(rsp_p.t02), &rsp_p.t02 ) )
    {
      /*Could not write signal strength result code to message: out of memory*/
      qmi_err_write(0x02, &rsp_p.t02); 
      dsm_free_packet(p_response);
      return QMI_ERR_NO_MEMORY;
    }    
  }

  return QMI_ERR_NONE;
} /* qmi_nas_0059_rsp_write */

/*=========================================================================== 
  FUNCTION QMI_NAS_005B_REQ_READ() 
 
  DESCRIPTION 
    Reads QMI_NAS_UPDATE_AKEY_EXT req into struct nas_005B_req_s 
     
  RETURN VALUE 
    QMI_ERR_NONE for successful read. For failure, returns appropriate error 
    value. 
     
  DEPENDENCIES 
    None 
 
  SIDE EFFECTS 
    None 
===========================================================================*/ 
qmi_error_e_type qmi_nas_005B_req_read ( 
  struct nas_005B_req_s  *p_msg, 
  dsm_item_type         **sdu_in 
) 
{
#if defined (FEATURE_CDMA_800) || defined (FEATURE_CDMA_1900)
  uint16                     len;
  uint16                     expected_len;
  uint8                      prm_type;
  struct nas_005B_req_p_s    nas_005B_req_p;
  void                       *value;

  memset(&nas_005B_req_p, 0, sizeof(nas_005B_req_p));

  while (*sdu_in) 
  {
    if ( !qmi_svc_get_tl(sdu_in, &prm_type, &len) )
    {
      continue;
    }

    expected_len = 0;
    value = NULL;

    // special value 0 = variable length or don't care (unrecognized TLV)
    switch (prm_type)
    {
      case NAS_005B_REQ_AKEY_EXT:
        expected_len = sizeof(nas_005B_req_p.t01);
        value = &nas_005B_req_p.t01;
        p_msg->t01_valid = TRUE;
        break;

      default:
        QM_MSG_ERROR_1("Unrecognized TLV type (%d)", prm_type);
        break;
    }

    if ((expected_len != 0) && (expected_len != len))
    {
      QM_MSG_ERROR_2("Invalid TLV len (%d) for type (%d)", len, prm_type);
      return QMI_ERR_MALFORMED_MSG;
    }

    if (len != dsm_pullup( sdu_in, value, len ))
    {
        qmi_nas_invalid_len_tlv();
      return QMI_ERR_MALFORMED_MSG;
    }
  }
  
  /*-----------------------------------------------------------------------
    Transfer to unpacked struct
  -----------------------------------------------------------------------*/

  if( p_msg->t01_valid )
  {
   memscpy(p_msg->t01.spc, sizeof(nas_005B_req_p.t01.spc), 
           nas_005B_req_p.t01.spc, sizeof(nas_005B_req_p.t01.spc));
   memscpy(p_msg->t01.akey, sizeof(nas_005B_req_p.t01.akey), 
           nas_005B_req_p.t01.akey, sizeof(nas_005B_req_p.t01.akey));
  }

#endif //(FEATURE_CDMA_800) || defined (FEATURE_CDMA_1900)
  return QMI_ERR_NONE;
} /* qmi_nas_005B_req_read */

/*=========================================================================== 
  FUNCTION QMI_NAS_005B_REQ_CHECK() 
 
  DESCRIPTION 
    Checks QMI_NAS_UPDATE_AKEY_EXT req in struct nas_005B_req_s for errors 
     
  RETURN VALUE 
    QMI_ERR_NONE for no error or appropriate error code. 
     
  DEPENDENCIES 
    None 
 
  SIDE EFFECTS 
    None 
===========================================================================*/ 
qmi_error_e_type qmi_nas_005B_req_check (struct nas_005B_req_s *p_msg)
{
  int i;

  if (!p_msg->t01_valid)
  {
    QM_MSG_ERROR("AKEY not provided. This TLV is mandatory");
    return QMI_ERR_INVALID_ARG;
  }
  else
  {
    for (i = 0; i < NAS_005B_REQ_AKEY_EXT_SPC_SIZE ; i++)
    {
      if (p_msg->t01.spc[i] < '0' || p_msg->t01.spc[i] > '9')
      {
        QM_MSG_ERROR("Invalid SPC");
        return QMI_ERR_INVALID_ARG;
      }
    }
  }  

  return QMI_ERR_NONE;
} /* qmi_nas_005B_req_check */

/*=========================================================================== 
  FUNCTION QMI_NAS_005D_RSP_WRITE() 

  DESCRIPTION 
  Writes QMI_NAS_DETACH_LTE response in struct nas_005D_rsp_s

  RETURN VALUE 
  QMI_ERR_NONE for no error or appropriate error code. 

  DEPENDENCIES 
  None 

  SIDE EFFECTS 
  None 
  ===========================================================================*/
qmi_error_e_type qmi_nas_005D_rsp_write(struct nas_005D_rsp_s *p_msg, dsm_item_type **p_response)
{
  struct nas_005D_rsp_p_s rsp_p;

  if (p_msg == NULL || p_response == NULL)
  {
    return QMI_ERR_INTERNAL;
  }

  memset(&rsp_p, 0, sizeof(rsp_p));

  if ( p_msg->t02_valid )
  {
    memscpy((void*)&rsp_p.t02.result_code, sizeof(p_msg->t02.result_code),
            (void*)&p_msg->t02.result_code, sizeof(p_msg->t02.result_code));
    memscpy((void*)&rsp_p.t02.error_code, sizeof(p_msg->t02.error_code),
            (void*)&p_msg->t02.error_code, sizeof(p_msg->t02.error_code));

    if ( !qmi_svc_put_param_tlv( p_response, 0x02, sizeof(rsp_p.t02), &rsp_p.t02 ) )
    {
      qmi_err_write(0x02, &rsp_p.t02 );
      dsm_free_packet(p_response);
      return QMI_ERR_NO_MEMORY;
    }    
  }

  return QMI_ERR_NONE;
} /* qmi_nas_005D_rsp_write */

/*=========================================================================== 
  FUNCTION QMI_NAS_005E_REQ_READ() 
 
  DESCRIPTION 
    Reads QMI_NAS_BLOCK_LTE_PLMN req into struct nas_005E_req_s 
     
  RETURN VALUE 
    QMI_ERR_NONE for successful read. For failure, returns appropriate error 
    value. 
     
  DEPENDENCIES 
    None 
 
  SIDE EFFECTS 
    None 
===========================================================================*/
qmi_error_e_type qmi_nas_005E_req_read(struct nas_005E_req_s *p_msg, dsm_item_type **sdu_in)
{
  uint8   type; 
  uint16  len, expected_len; 
  void   *value; 
  struct nas_005E_req_p_s  nas_005E_req_p;
  
  qmi_error_e_type errval = QMI_ERR_NONE; 

  memset(&nas_005E_req_p, 0, sizeof(nas_005E_req_p));

  while (*sdu_in) 
  { 
    if ( !qmi_svc_get_tl(sdu_in, &type, &len) ) 
      continue; 
 
    // special value 0 = variable length or don't care (unrecognzied TLV) 
    expected_len = 0; 
    value = NULL; 
 
    switch (type) 
    { 
      case 0x01:
        value = &nas_005E_req_p.t01;
        p_msg->t01_valid = TRUE; 
        expected_len = sizeof(nas_005E_req_p.t01);
        break;

      case 0x10:
        value = &nas_005E_req_p.t10;
        p_msg->t10_valid = TRUE;
        expected_len = sizeof(nas_005E_req_p.t10);
        break;

      case 0x11:
        value = &nas_005E_req_p.t11;
        p_msg->t11_valid = TRUE;
        expected_len = sizeof(nas_005E_req_p.t11);
        break;

      default: 
        QM_MSG_ERROR_1("Unrecognized TLV type (%d)", type);
        break; 
    } 
 
    if ((expected_len != 0) && (expected_len != len)) 
    { 
      QM_MSG_ERROR_2("Invalid TLV len (%d) for type (%d)", len, type);
      errval = QMI_ERR_MALFORMED_MSG; 
      break; 
    } 
 
    /*----------------------------------------------------------------------- 
      If type is unrecognized, value will be NULL, and dsm_pullup will  
      free the unrecognized value, i.e. we skip past unrecognized TLVs 
    -----------------------------------------------------------------------*/ 
    if (len != dsm_pullup( sdu_in, value, len )) 
    { 
        qmi_nas_invalid_len_tlv();
      errval = QMI_ERR_MALFORMED_MSG; 
      break; 
    } 
  }

  if ( errval == QMI_ERR_NONE && p_msg->t10_valid && p_msg->t11_valid )
  {
    QM_MSG_ERROR("Cannot set both TLV10 and TLV11 in BLOCK_LTE_PLMN");
    errval = QMI_ERR_MALFORMED_MSG;
  }

  if ( errval == QMI_ERR_NONE && p_msg->t01_valid )
  {
    memscpy(&p_msg->t01.mcc, sizeof(nas_005E_req_p.t01.mcc),
            (void*)&nas_005E_req_p.t01.mcc, sizeof(nas_005E_req_p.t01.mcc) );
    memscpy(&p_msg->t01.mnc, sizeof(nas_005E_req_p.t01.mnc),
            (void*)&nas_005E_req_p.t01.mnc, sizeof(nas_005E_req_p.t01.mnc) );
    memscpy(&p_msg->t01.mnc_includes_pcs_digit, sizeof(nas_005E_req_p.t01.mnc_includes_pcs_digit),
            (void*)&nas_005E_req_p.t01.mnc_includes_pcs_digit, sizeof(nas_005E_req_p.t01.mnc_includes_pcs_digit) );
  }

  if ( errval == QMI_ERR_NONE && p_msg->t10_valid )
  {
    memscpy(&p_msg->t10.blocking_interval_abs, sizeof(nas_005E_req_p.t10.blocking_interval_abs),
           (void*)&nas_005E_req_p.t10.blocking_interval_abs, sizeof(nas_005E_req_p.t10.blocking_interval_abs) );
  }

  if ( errval == QMI_ERR_NONE && p_msg->t11_valid )
  {
    memscpy(&p_msg->t11.blocking_interval_mult, sizeof(nas_005E_req_p.t11.blocking_interval_mult),
           (void*)&nas_005E_req_p.t11.blocking_interval_mult, sizeof(nas_005E_req_p.t11.blocking_interval_mult) );
  }
 
  return errval;
} /* qmi_nas_005E_req_read */

/*=========================================================================== 
  FUNCTION QMI_NAS_005E_REQ_CHECK() 
 
  DESCRIPTION 
    Checks QMI_NAS_BLOCK_LTE_PLMN req in struct nas_005E_req_s for errors 
     
  RETURN VALUE 
    QMI_ERR_NONE for no error or appropriate error code. 
     
  DEPENDENCIES 
    None 
 
  SIDE EFFECTS 
    None 
===========================================================================*/
qmi_error_e_type qmi_nas_005E_req_check(struct nas_005E_req_s *p_msg)
{
  if (p_msg == NULL)
  {
    return QMI_ERR_INTERNAL;
  }

  if (!p_msg->t01_valid)
  {
    QM_MSG_ERROR("PLMN not provided. This TLV is mandatory");
    return QMI_ERR_MISSING_ARG;
  }

  if (!p_msg->t10_valid && !p_msg->t11_valid)
  {
    QM_MSG_ERROR("Blocking interval not provided. One of these two TLVs are required");
    return QMI_ERR_MISSING_ARG;
  }

  return QMI_ERR_NONE;
} /* qmi_nas_005E_req_check */

/*=========================================================================== 
  FUNCTION QMI_NAS_005E_RSP_WRITE() 

  DESCRIPTION 
  Writes QMI_NAS_BLOCK_LTE_PLMN response in struct nas_005E_rsp_s

  RETURN VALUE 
  QMI_ERR_NONE for no error or appropriate error code. 

  DEPENDENCIES 
  None 

  SIDE EFFECTS 
  None 
  ===========================================================================*/
qmi_error_e_type qmi_nas_005E_rsp_write(struct nas_005E_rsp_s *p_msg, dsm_item_type **p_response)
{
  struct nas_005E_rsp_p_s rsp_p;

  if (p_msg == NULL || p_response == NULL)
  {
    return QMI_ERR_INTERNAL;
  }

  memset(&rsp_p, 0, sizeof(rsp_p));

  if ( p_msg->t02_valid )
  {
    memscpy((void*)&rsp_p.t02.result_code, sizeof(p_msg->t02.result_code),
            (void*)&p_msg->t02.result_code, sizeof(p_msg->t02.result_code));
    memscpy((void*)&rsp_p.t02.error_code, sizeof(p_msg->t02.error_code),
            (void*)&p_msg->t02.error_code, sizeof(p_msg->t02.error_code));

    if ( !qmi_svc_put_param_tlv( p_response, 0x02, sizeof(rsp_p.t02), &rsp_p.t02 ) )
    {
      qmi_err_write(0x02, &rsp_p.t02);
      dsm_free_packet(p_response);
      return QMI_ERR_NO_MEMORY;
    }    
  }

  return QMI_ERR_NONE;
} /* qmi_nas_005E_rsp_write */

/*=========================================================================== 
  FUNCTION QMI_NAS_005F_REQ_READ() 
 
  DESCRIPTION 
    Reads QMI_NAS_UNBLOCK_LTE_PLMN req into struct nas_005F_req_s 
     
  RETURN VALUE 
    QMI_ERR_NONE for successful read. For failure, returns appropriate error 
    value. 
     
  DEPENDENCIES 
    None 
 
  SIDE EFFECTS 
    None 
===========================================================================*/
qmi_error_e_type qmi_nas_005F_req_read(struct nas_005F_req_s *p_msg, dsm_item_type **sdu_in)
{
  uint8   type; 
  uint16  len, expected_len; 
  void   *value; 
  struct nas_005F_req_p_s  nas_005F_req_p;
  
  qmi_error_e_type errval = QMI_ERR_NONE; 

  memset(&nas_005F_req_p, 0, sizeof(nas_005F_req_p));

  while (*sdu_in) 
  { 
    if ( !qmi_svc_get_tl(sdu_in, &type, &len) ) 
      continue; 
 
    // special value 0 = variable length or don't care (unrecognzied TLV) 
    expected_len = 0; 
    value = NULL; 
 
    switch (type) 
    { 
      case 0x01:
        value = &nas_005F_req_p.t01;
        p_msg->t01_valid = TRUE; 
        expected_len = sizeof(nas_005F_req_p.t01);
        break;

      default: 
        QM_MSG_ERROR_1("Unrecognized TLV type (%d)", type);
        break; 
    } 
 
    if ((expected_len != 0) && (expected_len != len)) 
    { 
      QM_MSG_ERROR_2("Invalid TLV len (%d) for type (%d)", len, type);
      errval = QMI_ERR_MALFORMED_MSG; 
      break; 
    } 
 
    /*----------------------------------------------------------------------- 
      If type is unrecognized, value will be NULL, and dsm_pullup will  
      free the unrecognized value, i.e. we skip past unrecognized TLVs 
    -----------------------------------------------------------------------*/ 
    if (len != dsm_pullup( sdu_in, value, len )) 
    { 
        qmi_nas_invalid_len_tlv();
      errval = QMI_ERR_MALFORMED_MSG; 
      break; 
    } 
  }

  if ( errval == QMI_ERR_NONE && p_msg->t01_valid )
  {
    memscpy(&p_msg->t01.mcc, sizeof(nas_005F_req_p.t01.mcc),
           (void*)&nas_005F_req_p.t01.mcc, sizeof(nas_005F_req_p.t01.mcc));
    memscpy(&p_msg->t01.mnc, sizeof(nas_005F_req_p.t01.mnc),
           (void*)&nas_005F_req_p.t01.mnc, sizeof(nas_005F_req_p.t01.mnc));
    memscpy(&p_msg->t01.mnc_includes_pcs_digit, sizeof(nas_005F_req_p.t01.mnc_includes_pcs_digit),
           (void*)&nas_005F_req_p.t01.mnc_includes_pcs_digit, sizeof(nas_005F_req_p.t01.mnc_includes_pcs_digit));
  }
 
  return errval;
} /* qmi_nas_005F_req_read */

/*=========================================================================== 
  FUNCTION QMI_NAS_005F_REQ_CHECK() 
 
  DESCRIPTION 
    Checks QMI_NAS_UNBLOCK_LTE_PLMN req in struct nas_005F_req_s for errors 
     
  RETURN VALUE 
    QMI_ERR_NONE for no error or appropriate error code. 
     
  DEPENDENCIES 
    None 
 
  SIDE EFFECTS 
    None 
===========================================================================*/
qmi_error_e_type qmi_nas_005F_req_check(struct nas_005F_req_s *p_msg)
{
  if (p_msg == NULL)
  {
    return QMI_ERR_INTERNAL;
  }

  if (!p_msg->t01_valid)
  {
    QM_MSG_ERROR("PLMN not provided. This TLV is mandatory");
    return QMI_ERR_MISSING_ARG;
  }

  return QMI_ERR_NONE;
} /* qmi_nas_005F_req_check */

/*=========================================================================== 
  FUNCTION QMI_NAS_005F_RSP_WRITE() 

  DESCRIPTION 
  Writes QMI_NAS_UNBLOCK_LTE_PLMN response in struct nas_005F_rsp_s

  RETURN VALUE 
  QMI_ERR_NONE for no error or appropriate error code. 

  DEPENDENCIES 
  None 

  SIDE EFFECTS 
  None 
  ===========================================================================*/
qmi_error_e_type qmi_nas_005F_rsp_write(struct nas_005F_rsp_s *p_msg, dsm_item_type **p_response)
{
  struct nas_005F_rsp_p_s rsp_p;

  if (p_msg == NULL || p_response == NULL)
  {
    return QMI_ERR_INTERNAL;
  }

  memset(&rsp_p, 0, sizeof(rsp_p));

  if ( p_msg->t02_valid )
  {
    memscpy((void*)&rsp_p.t02.result_code, sizeof(p_msg->t02.result_code),
            (void*)&p_msg->t02.result_code, sizeof(p_msg->t02.result_code));
    memscpy((void*)&rsp_p.t02.error_code, sizeof(p_msg->t02.error_code),
            (void*)&p_msg->t02.error_code, sizeof(p_msg->t02.error_code));

    if ( !qmi_svc_put_param_tlv( p_response, 0x02, sizeof(rsp_p.t02), &rsp_p.t02 ) )
    {
      qmi_err_write(0x02, &rsp_p.t02);
      dsm_free_packet(p_response);
      return QMI_ERR_NO_MEMORY;
    }    
  }

  return QMI_ERR_NONE;
} /* qmi_nas_005F_rsp_write */

/*=========================================================================== 
  FUNCTION QMI_NAS_0060_RSP_WRITE() 

  DESCRIPTION 
  Writes QMI_NAS_RESET_LTE_PLMN_BLOCKING response in struct nas_0060_rsp_s

  RETURN VALUE 
  QMI_ERR_NONE for no error or appropriate error code. 

  DEPENDENCIES 
  None 

  SIDE EFFECTS 
  None 
  ===========================================================================*/
qmi_error_e_type qmi_nas_0060_rsp_write(struct nas_0060_rsp_s *p_msg, dsm_item_type **p_response)
{
  struct nas_0060_rsp_p_s rsp_p;

  if (p_msg == NULL || p_response == NULL)
  {
    return QMI_ERR_INTERNAL;
  }

  memset(&rsp_p, 0, sizeof(rsp_p));

  if ( p_msg->t02_valid )
  {
    memscpy((void*)&rsp_p.t02.result_code, sizeof(p_msg->t02.result_code),
            (void*)&p_msg->t02.result_code, sizeof(p_msg->t02.result_code));
    memscpy((void*)&rsp_p.t02.error_code, sizeof(p_msg->t02.error_code), 
            (void*)&p_msg->t02.error_code, sizeof(p_msg->t02.error_code));

    if ( !qmi_svc_put_param_tlv( p_response, 0x02, sizeof(rsp_p.t02), &rsp_p.t02 ) )
    {
      qmi_err_write(0x02, &rsp_p.t02);
      dsm_free_packet(p_response);
      return QMI_ERR_NO_MEMORY;
    }    
  }

  return QMI_ERR_NONE;
} /* qmi_nas_0060_rsp_write */

/*=========================================================================== 
  FUNCTION QMI_NAS_0062_REQ_READ() 
 
  DESCRIPTION 
    Reads QMI_NAS_CONFIG_EMBMS req into struct nas_0062_req_s 
     
  RETURN VALUE 
    QMI_ERR_NONE for successful read. For failure, returns appropriate error 
    value. 
     
  DEPENDENCIES 
    None 
 
  SIDE EFFECTS 
    None 
===========================================================================*/
extern qmi_error_e_type qmi_nas_0062_req_read(struct nas_0062_req_s *p_msg, dsm_item_type **sdu_in)
{
  uint8   type; 
  uint16  len, expected_len; 
  void   *value; 
  struct nas_0062_req_p_s  nas_0062_req_p;
  
  qmi_error_e_type errval = QMI_ERR_NONE; 

  memset(&nas_0062_req_p, 0, sizeof(nas_0062_req_p));

  while (*sdu_in) 
  { 
    if ( !qmi_svc_get_tl(sdu_in, &type, &len) ) 
      continue; 
 
    // special value 0 = variable length or don't care (unrecognzied TLV) 
    expected_len = 0; 
    value = NULL; 
 
    switch (type) 
    { 
      case 0x01:
        value = &nas_0062_req_p.t01;
        p_msg->t01_valid = TRUE; 
        expected_len = sizeof(nas_0062_req_p.t01);
        break;

      case 0x10:
        value = &nas_0062_req_p.t10;
        p_msg->t10_valid = TRUE; 
        expected_len = sizeof(nas_0062_req_p.t10);
        break;

      default: 
        QM_MSG_ERROR_1("Unrecognized TLV type (%d)", type);
        break; 
    } 
 
    if ((expected_len != 0) && (expected_len != len)) 
    { 
      QM_MSG_ERROR_2("Invalid TLV len (%d) for type (%d)", len, type);
      errval = QMI_ERR_MALFORMED_MSG; 
      break; 
    } 
 
    /*----------------------------------------------------------------------- 
      If type is unrecognized, value will be NULL, and dsm_pullup will  
      free the unrecognized value, i.e. we skip past unrecognized TLVs 
    -----------------------------------------------------------------------*/ 
    if (len != dsm_pullup( sdu_in, value, len )) 
    { 
        qmi_nas_invalid_len_tlv();
      errval = QMI_ERR_MALFORMED_MSG; 
      break; 
    } 
  }

  if ( errval == QMI_ERR_NONE && p_msg->t01_valid )
  {
    memscpy(&p_msg->t01.enable, sizeof(nas_0062_req_p.t01.enable),
           (void*)&nas_0062_req_p.t01.enable, sizeof(nas_0062_req_p.t01.enable) );
  }
 
  if ( errval == QMI_ERR_NONE && p_msg->t10_valid )
  {
    memscpy(&p_msg->t10.trace_id, sizeof(nas_0062_req_p.t10.trace_id),
           (void*)&nas_0062_req_p.t10.trace_id, sizeof(nas_0062_req_p.t10.trace_id) );
  }
 
  return errval;
} /* qmi_nas_0062_req_read */

extern qmi_error_e_type qmi_nas_0062_req_check(struct nas_0062_req_s *p_msg);
/*=========================================================================== 
  FUNCTION QMI_NAS_0062_REQ_CHECK() 
 
  DESCRIPTION 
    Checks QMI_NAS_CONFIG_EMBMS req in struct nas_0062_req_s for errors 
     
  RETURN VALUE 
    QMI_ERR_NONE for no error or appropriate error code. 
     
  DEPENDENCIES 
    None 
 
  SIDE EFFECTS 
    None 
===========================================================================*/
extern qmi_error_e_type qmi_nas_0062_req_check(struct nas_0062_req_s *p_msg)
{
  if (p_msg == NULL)
  {
    return QMI_ERR_INTERNAL;
  }

  if (!p_msg->t01_valid)
  {
    QM_MSG_ERROR("TLV 0x01 missing in QMI_NAS_CONFIG_EMBMS_REQ");
    return QMI_ERR_MISSING_ARG;
  }
  else if ( p_msg->t01.enable != 0x01 && p_msg->t01.enable != 0x00 )
  {
    QM_MSG_ERROR_1("invalid TLV 0x01 val %d in QMI_NAS_CONFIG_EMBMS_REQ", p_msg->t01.enable);
    return QMI_ERR_INVALID_ARG;
  }

  if ( p_msg->t10_valid )
  {
    if ( p_msg->t10.trace_id < -1 )
    {
      return QMI_ERR_INVALID_ARG;
    }
  }

  return QMI_ERR_NONE;
} /* qmi_nas_0062_req_check */

extern qmi_error_e_type qmi_nas_0062_rsp_write(struct nas_0062_rsp_s *p_msg, dsm_item_type **response);

/*=========================================================================== 
  FUNCTION QMI_NAS_0062_RSP_WRITE() 

  DESCRIPTION 
  Writes QMI_NAS_CONFIG_EMBMS response in struct nas_0062_rsp_s

  RETURN VALUE 
  QMI_ERR_NONE for no error or appropriate error code. 

  DEPENDENCIES 
  None 

  SIDE EFFECTS 
  None 
  ===========================================================================*/
extern qmi_error_e_type qmi_nas_0062_rsp_write(struct nas_0062_rsp_s *p_msg, dsm_item_type **p_response)
{
  struct nas_0062_rsp_p_s rsp_p;

  if (p_msg == NULL || p_response == NULL)
  {
    return QMI_ERR_INTERNAL;
  }

  memset(&rsp_p, 0, sizeof(rsp_p));

  if ( p_msg->t10_valid )
  {
    memscpy((void*)&rsp_p.t10.trace_id, sizeof(p_msg->t10.trace_id),
            (void*)&p_msg->t10.trace_id, sizeof(p_msg->t10.trace_id));

    if ( !qmi_svc_put_param_tlv( p_response, 0x10, sizeof(rsp_p.t10), &rsp_p.t10 ) )
    {
      /*Could not write trace id*/
      qmi_err_write(0x10, &rsp_p.t10);   
      dsm_free_packet(p_response);
      return QMI_ERR_NO_MEMORY;
    }    
  }

  if ( p_msg->t02_valid )
  {
    memscpy((void*)&rsp_p.t02.result_code, sizeof(p_msg->t02.result_code), 
            (void*)&p_msg->t02.result_code, sizeof(p_msg->t02.result_code));
    memscpy((void*)&rsp_p.t02.error_code, sizeof(p_msg->t02.error_code),
            (void*)&p_msg->t02.error_code, sizeof(p_msg->t02.error_code));

    if ( !qmi_svc_put_param_tlv( p_response, 0x02, sizeof(rsp_p.t02), &rsp_p.t02 ) )
    {
      qmi_err_write(0x02, &rsp_p.t02);
      dsm_free_packet(p_response);
      return QMI_ERR_NO_MEMORY;
    }    
  }

  return QMI_ERR_NONE;
} /* qmi_nas_0062_rsp_write */

/*=========================================================================== 
  FUNCTION QMI_NAS_0064_IND_WRITE() 

  DESCRIPTION 
  Writes QMI_NAS_EMBMS_STATUS_IND in struct nas_0064_ind_s

  RETURN VALUE 
  QMI_ERR_NONE for no error or appropriate error code. 

  DEPENDENCIES 
  None 

  SIDE EFFECTS 
  None 
  ===========================================================================*/ 
qmi_error_e_type qmi_nas_0064_ind_write (
    struct nas_0064_ind_s *p_msg,
    dsm_item_type         **p_response
)
{
   struct nas_0064_ind_p_s nas_0064_ind_p;

  if ( p_msg == NULL || p_response == NULL )
  {
    return QMI_ERR_INTERNAL;
  }

  memset(&nas_0064_ind_p, 0, sizeof(nas_0064_ind_p));

  if ( p_msg->t10_valid )
  {
    memscpy((void*)&nas_0064_ind_p.t10.trace_id, sizeof(nas_0064_ind_p.t10.trace_id),
            (void*)&p_msg->t10.trace_id, sizeof(nas_0064_ind_p.t10.trace_id) );

    if ( !qmi_svc_put_param_tlv( p_response, 0x10, sizeof(nas_0064_ind_p.t10), &nas_0064_ind_p.t10))
    {
      /*Could not write tlv 0x10 for qmi_nas_embms_status_ind*/
      qmi_err_write(0x10, &nas_0064_ind_p.t10); 
      dsm_free_packet(p_response);
      return QMI_ERR_NO_MEMORY;
    }
  }

  if ( p_msg->t01_valid )
  {
    memscpy((void*)&nas_0064_ind_p.t01.enabled, sizeof(nas_0064_ind_p.t01.enabled),
            (void*)&p_msg->t01.enabled, sizeof(nas_0064_ind_p.t01.enabled));

    if ( !qmi_svc_put_param_tlv( p_response, 0x01, sizeof(nas_0064_ind_p.t01), &nas_0064_ind_p.t01))
    {
      /*Could not write tlv 0x01 for qmi_nas_embms_status_ind*/
      qmi_err_write(0x01,&nas_0064_ind_p.t01); 
      dsm_free_packet(p_response);
      return QMI_ERR_NO_MEMORY;
    }
  }

  return QMI_ERR_NONE;
} /* qmi_nas_0064_ind_write */

/*=========================================================================== 
  FUNCTION QMI_NAS_0065_RSP_WRITE() 

  DESCRIPTION 
  Writes QMI_NAS_GET_POSITION_INFO response in struct nas_0065_rsp_s

  RETURN VALUE 
  QMI_ERR_NONE for no error or appropriate error code. 

  DEPENDENCIES 
  None 

  SIDE EFFECTS 
  None 
  ===========================================================================*/
qmi_error_e_type qmi_nas_0065_rsp_write(struct nas_0065_rsp_s *p_msg, dsm_item_type **p_response)
{
  struct nas_0065_rsp_p_s rsp_p;

  if (p_msg == NULL || p_response == NULL)
  {
    return QMI_ERR_INTERNAL;
  }

  memset(&rsp_p, 0, sizeof(rsp_p));

  if ( p_msg->t02_valid )
  {
    memscpy((void*)&rsp_p.t02.result_code, sizeof(p_msg->t02.result_code),
            (void*)&p_msg->t02.result_code, sizeof(p_msg->t02.result_code));
    memscpy((void*)&rsp_p.t02.error_code, sizeof(p_msg->t02.error_code),
            (void*)&p_msg->t02.error_code, sizeof(p_msg->t02.error_code));

    if ( !qmi_svc_put_param_tlv( p_response, 0x02, sizeof(rsp_p.t02), &rsp_p.t02 ) )
    {
      qmi_err_write(0x02, &rsp_p.t02);  
      dsm_free_packet(p_response);
      return QMI_ERR_NO_MEMORY;
    }    
  }

  if ( p_msg->t10_valid )
  {
    int    i;
    uint16 size = 0;
    uint8  type = 0x10;

    i = MIN( p_msg->t10.bs_len - 1, QMI_NAS_CDMA_POSITION_INFO_MAX - 1);

    for ( ; i>=0; i-- )
    {
      memscpy((void*)&rsp_p.t10.bs[i].pilot_type, sizeof(p_msg->t10.bs[i].pilot_type),
              (void*)&p_msg->t10.bs[i].pilot_type, sizeof(p_msg->t10.bs[i].pilot_type) );
      memscpy((void*)&rsp_p.t10.bs[i].sid, sizeof(p_msg->t10.bs[i].sid),
              (void*)&p_msg->t10.bs[i].sid, sizeof(p_msg->t10.bs[i].sid) );
      memscpy((void*)&rsp_p.t10.bs[i].nid, sizeof(p_msg->t10.bs[i].nid),
              (void*)&p_msg->t10.bs[i].nid, sizeof(p_msg->t10.bs[i].nid) );
      memscpy((void*)&rsp_p.t10.bs[i].base_id, sizeof(p_msg->t10.bs[i].base_id),
              (void*)&p_msg->t10.bs[i].base_id, sizeof(p_msg->t10.bs[i].base_id) );
      memscpy((void*)&rsp_p.t10.bs[i].pilot_pn, sizeof(p_msg->t10.bs[i].pilot_pn),
              (void*)&p_msg->t10.bs[i].pilot_pn, sizeof(p_msg->t10.bs[i].pilot_pn) );
      memscpy((void*)&rsp_p.t10.bs[i].pilot_strength, sizeof(p_msg->t10.bs[i].pilot_strength),
              (void*)&p_msg->t10.bs[i].pilot_strength, sizeof(p_msg->t10.bs[i].pilot_strength) );
      memscpy((void*)&rsp_p.t10.bs[i].base_lat, sizeof(p_msg->t10.bs[i].base_lat),
              (void*)&p_msg->t10.bs[i].base_lat, sizeof(p_msg->t10.bs[i].base_lat) );
      memscpy((void*)&rsp_p.t10.bs[i].base_long, sizeof(p_msg->t10.bs[i].base_long),
              (void*)&p_msg->t10.bs[i].base_long, sizeof(p_msg->t10.bs[i].base_long) );
      memscpy((void*)&rsp_p.t10.bs[i].time_stamp, sizeof(p_msg->t10.bs[i].time_stamp),
              (void*)&p_msg->t10.bs[i].time_stamp, sizeof(p_msg->t10.bs[i].time_stamp) );

      if ( !QMI_SVC_PKT_PUSH( p_response, &rsp_p.t10.bs[i], sizeof(rsp_p.t10.bs[i]) ) )
      {
        QM_MSG_ERROR_1("Failed to put get cdma position info tlv 10 bs[%d]", i);
        dsm_free_packet( p_response );
        return QMI_ERR_NO_MEMORY;
      }
      else
      {
        size += sizeof(rsp_p.t10.bs[i]);
      }
    }

    if ( !QMI_SVC_PKT_PUSH( p_response, &p_msg->t10.bs_len,     sizeof(p_msg->t10.bs_len) ) ||
         !QMI_SVC_PKT_PUSH( p_response, &p_msg->t10.ue_in_idle, sizeof(p_msg->t10.ue_in_idle) ) )
    {
      QM_MSG_ERROR("Failed to put get cdma position info tlv 10 bs_len or ue_in_idle");
      dsm_free_packet( p_response );
      return QMI_ERR_NO_MEMORY;
    }
    else
    {
      size += sizeof(p_msg->t10.bs_len);
      size += sizeof(p_msg->t10.ue_in_idle);
    }

    if ( !QMI_SVC_PKT_PUSH( p_response, &size, sizeof(size) ) ||
         !QMI_SVC_PKT_PUSH( p_response, &type, sizeof(type) ) )
    {
      QM_MSG_ERROR("Failed to put get cdma position info tlv 10 type/len");
      dsm_free_packet( p_response );
      return QMI_ERR_NO_MEMORY;
    }
  }

  return QMI_ERR_NONE;
} /* qmi_nas_0065_rsp_write */

/*=========================================================================== 
  FUNCTION QMI_NAS_0066_IND_WRITE() 
 
  DESCRIPTION 
    Writes QMI_NAS_RF_BAND_INFO_IND in struct nas_0066_rsp_s
     
  RETURN VALUE 
    QMI_ERR_NONE for no error or appropriate error code. 
     
  DEPENDENCIES 
    None 
 
  SIDE EFFECTS 
    None 
===========================================================================*/ 
qmi_error_e_type qmi_nas_0066_ind_write (struct nas_0066_ind_s *p_msg, dsm_item_type **response)
{
  struct nas_0066_ind_p_s   nas_0066_ind_p;

  if (p_msg == NULL || response == NULL)
  {
    return QMI_ERR_INTERNAL;
  }

  memset(&nas_0066_ind_p, 0, sizeof(nas_0066_ind_p));

  if (p_msg->t11_valid == TRUE)
  {
    memscpy((void*) &nas_0066_ind_p.t11.radio_if, sizeof(nas_0066_ind_p.t11.radio_if),
            (void*) &p_msg->t11.radio_if, sizeof(nas_0066_ind_p.t11.radio_if));
    memscpy((void*) &nas_0066_ind_p.t11.active_band, sizeof(nas_0066_ind_p.t11.active_band),
            (void*) &p_msg->t11.active_band, sizeof(nas_0066_ind_p.t11.active_band));
    memscpy((void*) &nas_0066_ind_p.t11.active_channel, sizeof(nas_0066_ind_p.t11.active_channel),
            (void*) &p_msg->t11.active_channel, sizeof(nas_0066_ind_p.t11.active_channel));

    if (FALSE == qmi_svc_put_param_tlv(response, 
                                       NAS_0066_IND_T11,
                                       sizeof(nas_0066_ind_p.t11),
                                       &nas_0066_ind_p.t11))
    {
      /*Could not write RF band info (extended): Out of memory*/
      qmi_err_write(NAS_0066_IND_T11, &nas_0066_ind_p.t11 );
      dsm_free_packet(response);
      return QMI_ERR_NO_MEMORY;
    }
  }

  if (p_msg->t01_valid == TRUE)
  {
    memscpy((void*) &nas_0066_ind_p.t01.radio_if, sizeof(nas_0066_ind_p.t01.radio_if),
            (void*) &p_msg->t01.radio_if, sizeof(nas_0066_ind_p.t01.radio_if));
    memscpy((void*) &nas_0066_ind_p.t01.active_band, sizeof(nas_0066_ind_p.t01.active_band),
            (void*) &p_msg->t01.active_band, sizeof(nas_0066_ind_p.t01.active_band));
    memscpy((void*) &nas_0066_ind_p.t01.active_channel, sizeof(nas_0066_ind_p.t01.active_channel),
            (void*) &p_msg->t01.active_channel, sizeof(nas_0066_ind_p.t01.active_channel));
  }

  if (FALSE == qmi_svc_put_param_tlv(response, 
                                     NAS_0066_IND_T01,
                                     sizeof(nas_0066_ind_p.t01),
                                     &nas_0066_ind_p.t01))
  {
    qmi_err_write(NAS_0066_IND_T01, &nas_0066_ind_p.t01 );
    dsm_free_packet(response);
    return QMI_ERR_NO_MEMORY;
  }

  return QMI_ERR_NONE;
} /* qmi_nas_0031_rsp_write */

/*=========================================================================== 
  FUNCTION QMI_NAS_0068_IND_WRITE() 
 
  DESCRIPTION 
    Writes QMI_NAS_NETWORK_REJECT_IND in struct nas_0068_rsp_s
     
  RETURN VALUE 
    QMI_ERR_NONE for no error or appropriate error code. 
     
  DEPENDENCIES 
    None 
 
  SIDE EFFECTS 
    None 
===========================================================================*/ 
qmi_error_e_type qmi_nas_0068_ind_write (struct nas_0068_ind_s *p_msg, dsm_item_type **response)
{
  struct nas_0068_ind_p_s   nas_0068_ind_p;

  if (p_msg == NULL || response == NULL)
  {
    return QMI_ERR_INTERNAL;
  }

  memset(&nas_0068_ind_p, 0, sizeof(nas_0068_ind_p));

  if (p_msg->t01_valid == TRUE)
  {
    memscpy((void*) &nas_0068_ind_p.t01.radio_if, sizeof(nas_0068_ind_p.t01.radio_if),
            (void*) &p_msg->t01.radio_if, sizeof(nas_0068_ind_p.t01.radio_if));

    if ( !qmi_svc_put_param_tlv( response, NAS_0068_IND_T01, sizeof(nas_0068_ind_p.t01), &nas_0068_ind_p.t01 ) )
    {
      /*Could not write radio_if tlv 0x01 to message: out of memory*/
      qmi_err_write(NAS_0068_IND_T01, &nas_0068_ind_p.t01); 
      dsm_free_packet(response);
      return QMI_ERR_NO_MEMORY;
    }
  }

  if (p_msg->t02_valid == TRUE)
  {
    memscpy((void*) &nas_0068_ind_p.t02.reject_srv_domain, sizeof(nas_0068_ind_p.t02.reject_srv_domain),
            (void*) &p_msg->t02.reject_srv_domain, sizeof(nas_0068_ind_p.t02.reject_srv_domain));

    if ( !qmi_svc_put_param_tlv( response, NAS_0068_IND_T02, sizeof(nas_0068_ind_p.t02), &nas_0068_ind_p.t02 ) )
    {
      /*Could not write reject_srv_domain tlv 0x02 to message: out of memory*/
      qmi_err_write(NAS_0068_IND_T02,&nas_0068_ind_p.t02);
      dsm_free_packet(response);
      return QMI_ERR_NO_MEMORY;
    }
  }

  if (p_msg->t03_valid == TRUE)
  {
    memscpy((void*) &nas_0068_ind_p.t03.rej_cause, sizeof(nas_0068_ind_p.t03.rej_cause),
            (void*) &p_msg->t03.rej_cause, sizeof(nas_0068_ind_p.t03.rej_cause));

    if ( !qmi_svc_put_param_tlv( response, NAS_0068_IND_T03, sizeof(nas_0068_ind_p.t03), &nas_0068_ind_p.t03 ) )
    {
      /*Could not write rej_cause tlv 0x03 to message: out of memory*/
      qmi_err_write(NAS_0068_IND_T03, &nas_0068_ind_p.t03);  
      dsm_free_packet(response);
      return QMI_ERR_NO_MEMORY;
    }
  }

  if (p_msg->t10_valid == TRUE)
  {
    memscpy((void*) &nas_0068_ind_p.t10.mcc, sizeof(nas_0068_ind_p.t10.mcc),
            (void*) &p_msg->t10.mcc, sizeof(nas_0068_ind_p.t10.mcc));
    memscpy((void*) &nas_0068_ind_p.t10.mnc, sizeof(nas_0068_ind_p.t10.mnc),
            (void*) &p_msg->t10.mnc, sizeof(nas_0068_ind_p.t10.mnc));
    memscpy((void*) &nas_0068_ind_p.t10.mnc_includes_pcs_digit, sizeof(nas_0068_ind_p.t10.mnc_includes_pcs_digit),
            (void*) &p_msg->t10.mnc_includes_pcs_digit, sizeof(nas_0068_ind_p.t10.mnc_includes_pcs_digit));

    if ( !qmi_svc_put_param_tlv( response, NAS_0068_IND_T10, sizeof(nas_0068_ind_p.t10), &nas_0068_ind_p.t10 ) )
    {
      /*Could not write rej_cause tlv 0x10 to message: out of memory*/
      qmi_err_write(NAS_0068_IND_T10, &nas_0068_ind_p.t10);  
      dsm_free_packet(response);
      return QMI_ERR_NO_MEMORY;
    }
  }

  if (p_msg->t11_valid == TRUE)
  {
    memscpy((void*) &nas_0068_ind_p.t11.csg_id, sizeof(nas_0068_ind_p.t11.csg_id),
            (void*) &p_msg->t11.csg_id, sizeof(nas_0068_ind_p.t11.csg_id));

    if ( !qmi_svc_put_param_tlv( response, NAS_0068_IND_T11, sizeof(nas_0068_ind_p.t11), &nas_0068_ind_p.t11 ) )
    {
      /*Could not write rej_cause tlv 0x11 to message: out of memory*/
      qmi_err_write(NAS_0068_IND_T11, &nas_0068_ind_p.t11); 
      dsm_free_packet(response);
      return QMI_ERR_NO_MEMORY;
    }
  }

  return QMI_ERR_NONE;
} /* qmi_nas_0068_rsp_write */

/*=========================================================================== 
  FUNCTION QMI_NAS_0067_RSP_WRITE() 

  DESCRIPTION 
  Writes QMI_NAS_FORCE_NETWORK_SEARCH response in struct nas_0067_rsp_s

  RETURN VALUE 
  QMI_ERR_NONE for no error or appropriate error code. 

  DEPENDENCIES 
  None 

  SIDE EFFECTS 
  None 
  ===========================================================================*/
qmi_error_e_type qmi_nas_0067_rsp_write(struct nas_0067_rsp_s *p_msg, dsm_item_type **p_response)
{
  struct nas_0067_rsp_p_s rsp_p;

  if (p_msg == NULL || p_response == NULL)
  {
    return QMI_ERR_INTERNAL;
  }

  memset(&rsp_p, 0, sizeof(rsp_p));

  if ( p_msg->t02_valid )
  {
    memscpy((void*)&rsp_p.t02.result_code, sizeof(p_msg->t02.result_code),
            (void*)&p_msg->t02.result_code, sizeof(p_msg->t02.result_code));
    memscpy((void*)&rsp_p.t02.error_code, sizeof(p_msg->t02.error_code),
            (void*)&p_msg->t02.error_code, sizeof(p_msg->t02.error_code));

    if ( !qmi_svc_put_param_tlv( p_response, 0x02, sizeof(rsp_p.t02), &rsp_p.t02 ) )
    {
      qmi_err_write(0x02, &rsp_p.t02);
      dsm_free_packet(p_response);
      return QMI_ERR_NO_MEMORY;
    }    
  }

  return QMI_ERR_NONE;
} /* qmi_nas_0067_rsp_write */

/*=========================================================================== 
  FUNCTION QMI_NAS_006A_IND_WRITE() 

  DESCRIPTION 
  Writes QMI_NAS_RTRE_CONFIG_IND response in struct nas_006A_ind_s

  RETURN VALUE 
  QMI_ERR_NONE for no error or appropriate error code. 

  DEPENDENCIES 
  None 

  SIDE EFFECTS 
  None 
  ===========================================================================*/
qmi_error_e_type qmi_nas_006A_ind_write(struct nas_006A_ind_s *p_msg, dsm_item_type **p_response)
{
  struct nas_006A_ind_p_s ind_p;

  if (p_msg == NULL || p_response == NULL)
  {
    return QMI_ERR_INTERNAL;
  }  

  memset(&ind_p, 0x00, sizeof(struct nas_006A_ind_p_s));

  if( p_msg->t11_valid )
  {
    memscpy((void*) &ind_p.t11.rtre_cfg_pref, sizeof(ind_p.t11.rtre_cfg_pref),
            (void*) &p_msg->t11.rtre_cfg_pref, sizeof(ind_p.t11.rtre_cfg_pref) );

    // encode dsm type
    if ( !qmi_svc_put_param_tlv( p_response, NAS_006A_IND_RTRE_CONFIG_PREF_T11, sizeof(ind_p.t11), &ind_p.t11 ) )
    {
      /*Could not write rtre cfg ind tlv 0x11 to message: out of memory*/
      qmi_err_write(NAS_006A_IND_RTRE_CONFIG_PREF_T11, &ind_p.t11); 
      dsm_free_packet(p_response);
      return QMI_ERR_NO_MEMORY;
    }    
  }

  if ( p_msg->t10_valid )
  {
    memscpy((void*) &ind_p.t10.rtre_cfg, sizeof(ind_p.t10.rtre_cfg), 
            (void*) &p_msg->t10.rtre_cfg, sizeof(ind_p.t10.rtre_cfg) );

    // encode dsm type
    if ( !qmi_svc_put_param_tlv( p_response, NAS_006A_IND_RTRE_CONFIG_T10, sizeof(ind_p.t10), &ind_p.t10 ) )
    {
      /*Could not write rtre cfg ind tlv 0x10 to message: out of memory*/
      qmi_err_write(NAS_006A_IND_RTRE_CONFIG_T10, &ind_p.t10);  
      dsm_free_packet(p_response);
      return QMI_ERR_NO_MEMORY;
    }
  }  

  return QMI_ERR_NONE;  
} //qmi_nas_006A_ind_write

/*=========================================================================== 
  FUNCTION QMI_NAS_0069_RSP_WRITE() 

  DESCRIPTION 
  Writes QMI_NAS_GET_MANAGED_ROAMING_CONFIG_RESP response in struct nas_0069_rsp_s

  RETURN VALUE 
  QMI_ERR_NONE for no error or appropriate error code. 

  DEPENDENCIES 
  None 

  SIDE EFFECTS 
  None 
  ===========================================================================*/
qmi_error_e_type qmi_nas_0069_rsp_write (struct nas_0069_rsp_s *p_msg, dsm_item_type **response)
{
  struct nas_0069_rsp_p_s   nas_0069_rsp_p;

  if (p_msg == NULL || response == NULL)
  {
    return QMI_ERR_INTERNAL;
  }

  memset(&nas_0069_rsp_p, 0, sizeof(nas_0069_rsp_p));

  if ( p_msg->t10_valid)
  {
    memscpy((void *)&nas_0069_rsp_p.t10.managed_roaming_supported, sizeof(p_msg->t10.managed_roaming_supported),
            (void *)&p_msg->t10.managed_roaming_supported, sizeof(p_msg->t10.managed_roaming_supported));

    if ( !qmi_svc_put_param_tlv(response, NAS_0069_RSP_T10, sizeof(nas_0069_rsp_p.t10), &nas_0069_rsp_p.t10) )
    {
      qmi_err_write(NAS_0069_RSP_T10, &nas_0069_rsp_p.t10);
      dsm_free_packet( response );
      return QMI_ERR_NO_MEMORY;
    }
  }

  if(p_msg->t02_valid)
  {
    memscpy((void*)&nas_0069_rsp_p.t02.result_code, sizeof(p_msg->t02.result_code),
            (void*)&p_msg->t02.result_code, sizeof(p_msg->t02.result_code));
    memscpy((void*)&nas_0069_rsp_p.t02.error_code, sizeof(p_msg->t02.error_code),
            (void*)&p_msg->t02.error_code,  sizeof(p_msg->t02.error_code));

    if ( !qmi_svc_put_param_tlv( response, NAS_0069_RSP_T02, sizeof(nas_0069_rsp_p.t02), &nas_0069_rsp_p.t02 ) )
    {
      qmi_err_write(NAS_0069_RSP_T02, &nas_0069_rsp_p.t02);
      dsm_free_packet(response);
      return QMI_ERR_NO_MEMORY;
    }    
  }

  return QMI_ERR_NONE;
} /* qmi_nas_0069_rsp_write */

/*=========================================================================== 
  FUNCTION QMI_NAS_006B_RSP_WRITE() 

  DESCRIPTION 
  Writes QMI_NAS_GET_CENTRALIZED_EONS_SUPPORT_STATUS response in struct nas_006B_rsp_s

  RETURN VALUE 
  QMI_ERR_NONE for no error or appropriate error code. 

  DEPENDENCIES 
  None 

  SIDE EFFECTS 
  None 
  ===========================================================================*/
qmi_error_e_type qmi_nas_006B_rsp_write (struct nas_006B_rsp_s *p_msg, dsm_item_type **response)
{
  struct nas_006B_rsp_p_s   nas_006B_rsp_p;
	
  if (p_msg == NULL || response == NULL)
  {
    return QMI_ERR_INTERNAL;
  }
	
  memset(&nas_006B_rsp_p, 0, sizeof(nas_006B_rsp_p));
	
  if ( p_msg->t10_valid)
  {
    memscpy((void *)&nas_006B_rsp_p.t10.centralized_eons_supported, sizeof(p_msg->t10.centralized_eons_supported),
            (void *)&p_msg->t10.centralized_eons_supported, sizeof(p_msg->t10.centralized_eons_supported));

    if ( !qmi_svc_put_param_tlv(response, NAS_006B_RSP_T10, sizeof(nas_006B_rsp_p.t10), &nas_006B_rsp_p.t10) )
	{
      qmi_err_write(NAS_006B_RSP_T10,&nas_006B_rsp_p.t10);
      dsm_free_packet( response );
      return QMI_ERR_NO_MEMORY;
	}
  }
	
  if(p_msg->t02_valid)
  {
    memscpy((void*)&nas_006B_rsp_p.t02.result_code, sizeof(p_msg->t02.result_code),
            (void*)&p_msg->t02.result_code, sizeof(p_msg->t02.result_code));
	  memscpy((void*)&nas_006B_rsp_p.t02.error_code, sizeof(p_msg->t02.error_code),
            (void*)&p_msg->t02.error_code, sizeof(p_msg->t02.error_code));
	
    if ( !qmi_svc_put_param_tlv( response, NAS_006B_RSP_T02, sizeof(nas_006B_rsp_p.t02), &nas_006B_rsp_p.t02 ) )
    {
      qmi_err_write(NAS_006B_RSP_T02, &nas_006B_rsp_p.t02);
      dsm_free_packet(response);
      return QMI_ERR_NO_MEMORY;
    }    
  }
	
  return QMI_ERR_NONE;
}

/*=========================================================================== 
  FUNCTION QMI_NAS_006C_REQ_CHECK()
 
  DESCRIPTION 
    Checks QMI_NAS_CONFIG_SIG_INFO2_REQ_MSG in nas_config_sig_info2_req_msg_v01 for errors
     
  RETURN VALUE 
    QMI_ERR_NONE_V01 for no error or appropriate error code.
     
  DEPENDENCIES 
    None 
 
  SIDE EFFECTS 
    None 
===========================================================================*/ 
qmi_error_type_v01 qmi_nas_006C_req_check(
  nas_config_sig_info2_req_msg_v01 * p_msg
) 
{ 
  // If ALL optional TLVs are absent return error 
  if ( ! p_msg->cdma_rssi_threshold_list_valid    && ! p_msg->cdma_rssi_delta_valid           && ! p_msg->cdma_ecio_threshold_list_valid  &&
       ! p_msg->cdma_ecio_delta_valid             && ! p_msg->hdr_rssi_threshold_list_valid   && ! p_msg->hdr_rssi_delta_valid            &&
       ! p_msg->hdr_ecio_threshold_list_valid     && ! p_msg->hdr_ecio_delta_valid            && ! p_msg->hdr_sinr_threshold_list_valid   &&
       ! p_msg->hdr_sinr_delta_valid              && ! p_msg->hdr_io_threshold_list_valid     && ! p_msg->hdr_io_delta_valid              &&
       ! p_msg->gsm_rssi_threshold_list_valid     && ! p_msg->gsm_rssi_delta_valid            && ! p_msg->wcdma_rssi_threshold_list_valid &&
       ! p_msg->wcdma_rssi_delta_valid            && ! p_msg->wcdma_ecio_threshold_list_valid && ! p_msg->wcdma_ecio_delta_valid          &&
       ! p_msg->lte_rssi_threshold_list_valid     && ! p_msg->lte_rssi_delta_valid            && ! p_msg->lte_snr_threshold_list_valid    &&
       ! p_msg->lte_snr_delta_valid               && ! p_msg->lte_rsrq_threshold_list_valid   && ! p_msg->lte_rsrq_delta_valid            &&
       ! p_msg->lte_rsrp_threshold_list_valid     && ! p_msg->lte_rsrp_delta_valid            && ! p_msg->lte_sig_rpt_config_valid        &&
       ! p_msg->tdscdma_rscp_threshold_list_valid && ! p_msg->tdscdma_rscp_delta_valid        && ! p_msg->tds_rssi_threshold_list_valid   &&
       ! p_msg->tdscdma_rssi_delta_valid          && ! p_msg->tds_ecio_threshold_list_valid   && ! p_msg->tdscdma_ecio_delta_valid        &&
       ! p_msg->tds_sinr_threshold_list_valid     && ! p_msg->tdscdma_sinr_delta_valid        )
  { 
    return QMI_ERR_MISSING_ARG_V01;
  }

  // Ensure no delta and threshold sent for same field
  if ( ( p_msg->cdma_rssi_threshold_list_valid    && p_msg->cdma_rssi_delta_valid    ) ||
       ( p_msg->cdma_ecio_threshold_list_valid    && p_msg->cdma_ecio_delta_valid    ) ||
       ( p_msg->hdr_rssi_threshold_list_valid     && p_msg->hdr_rssi_delta_valid     ) ||
       ( p_msg->hdr_ecio_threshold_list_valid     && p_msg->hdr_ecio_delta_valid     ) ||
       ( p_msg->hdr_sinr_threshold_list_valid     && p_msg->hdr_sinr_delta_valid     ) ||
       ( p_msg->hdr_io_threshold_list_valid       && p_msg->hdr_io_delta_valid       ) ||
       ( p_msg->gsm_rssi_threshold_list_valid     && p_msg->gsm_rssi_delta_valid     ) ||
       ( p_msg->wcdma_rssi_threshold_list_valid   && p_msg->wcdma_rssi_delta_valid   ) ||
       ( p_msg->wcdma_ecio_threshold_list_valid   && p_msg->wcdma_ecio_delta_valid   ) ||
       ( p_msg->lte_rssi_threshold_list_valid     && p_msg->lte_rssi_delta_valid     ) ||
       ( p_msg->lte_snr_threshold_list_valid      && p_msg->lte_snr_delta_valid      ) ||
       ( p_msg->lte_rsrq_threshold_list_valid     && p_msg->lte_rsrq_delta_valid     ) ||
       ( p_msg->lte_rsrp_threshold_list_valid     && p_msg->lte_rsrp_delta_valid     ) ||
       ( p_msg->tdscdma_rscp_threshold_list_valid && p_msg->tdscdma_rscp_delta_valid ) ||
       ( p_msg->tds_rssi_threshold_list_valid     && p_msg->tdscdma_rssi_delta_valid ) ||
       ( p_msg->tds_ecio_threshold_list_valid     && p_msg->tdscdma_ecio_delta_valid ) ||
       ( p_msg->tds_sinr_threshold_list_valid     && p_msg->tdscdma_sinr_delta_valid )
    )
  {
    return QMI_ERR_INVALID_ARG_V01;
  }

  // Ensure if threshold sent values are present
  if ( ( p_msg->cdma_rssi_threshold_list_valid    && p_msg->cdma_rssi_threshold_list_len    == 0 ) ||
       ( p_msg->cdma_ecio_threshold_list_valid    && p_msg->cdma_ecio_threshold_list_len    == 0 ) ||
       ( p_msg->hdr_rssi_threshold_list_valid     && p_msg->hdr_rssi_threshold_list_len     == 0 ) ||
       ( p_msg->hdr_ecio_threshold_list_valid     && p_msg->hdr_ecio_threshold_list_len     == 0 ) ||
       ( p_msg->hdr_sinr_threshold_list_valid     && p_msg->hdr_sinr_threshold_list_len     == 0 ) ||
       ( p_msg->hdr_io_threshold_list_valid       && p_msg->hdr_io_threshold_list_len       == 0 ) ||
       ( p_msg->gsm_rssi_threshold_list_valid     && p_msg->gsm_rssi_threshold_list_len     == 0 ) ||
       ( p_msg->wcdma_rssi_threshold_list_valid   && p_msg->wcdma_rssi_threshold_list_len   == 0 ) ||
       ( p_msg->wcdma_ecio_threshold_list_valid   && p_msg->wcdma_ecio_threshold_list_len   == 0 ) ||
       ( p_msg->lte_rssi_threshold_list_valid     && p_msg->lte_rssi_threshold_list_len     == 0 ) ||
       ( p_msg->lte_snr_threshold_list_valid      && p_msg->lte_snr_threshold_list_len      == 0 ) ||
       ( p_msg->lte_rsrq_threshold_list_valid     && p_msg->lte_rsrq_threshold_list_len     == 0 ) ||
       ( p_msg->lte_rsrp_threshold_list_valid     && p_msg->lte_rsrp_threshold_list_len     == 0 ) ||
       ( p_msg->tdscdma_rscp_threshold_list_valid && p_msg->tdscdma_rscp_threshold_list_len == 0 ) ||
       ( p_msg->tds_rssi_threshold_list_valid     && p_msg->tds_rssi_threshold_list_len     == 0 ) ||
       ( p_msg->tds_ecio_threshold_list_valid     && p_msg->tds_ecio_threshold_list_len     == 0 ) ||
       ( p_msg->tds_sinr_threshold_list_valid     && p_msg->tds_sinr_threshold_list_len     == 0 )
    )
  {
    return QMI_ERR_NO_THRESHOLDS_V01;
  }

  // Ensure delta values sent are above 0
  if ( ( p_msg->cdma_rssi_delta_valid    && p_msg->cdma_rssi_delta    < NAS_006C_MIN_DELTA ) ||
       ( p_msg->cdma_ecio_delta_valid    && p_msg->cdma_ecio_delta    < NAS_006C_MIN_DELTA ) ||
       ( p_msg->hdr_rssi_delta_valid     && p_msg->hdr_rssi_delta     < NAS_006C_MIN_DELTA ) ||
       ( p_msg->hdr_ecio_delta_valid     && p_msg->hdr_ecio_delta     < NAS_006C_MIN_DELTA ) ||
       ( p_msg->hdr_sinr_delta_valid     && p_msg->hdr_sinr_delta     < NAS_006C_MIN_DELTA ) ||
       ( p_msg->hdr_io_delta_valid       && p_msg->hdr_io_delta       < NAS_006C_MIN_DELTA ) ||
       ( p_msg->gsm_rssi_delta_valid     && p_msg->gsm_rssi_delta     < NAS_006C_MIN_DELTA ) ||
       ( p_msg->wcdma_rssi_delta_valid   && p_msg->wcdma_rssi_delta   < NAS_006C_MIN_DELTA ) ||
       ( p_msg->wcdma_ecio_delta_valid   && p_msg->wcdma_ecio_delta   < NAS_006C_MIN_DELTA ) ||
       ( p_msg->lte_rssi_delta_valid     && p_msg->lte_rssi_delta     < NAS_006C_MIN_DELTA ) ||
       ( p_msg->lte_snr_delta_valid      && p_msg->lte_snr_delta      < NAS_006C_MIN_DELTA ) ||
       ( p_msg->lte_rsrq_delta_valid     && p_msg->lte_rsrq_delta     < NAS_006C_MIN_DELTA ) ||
       ( p_msg->lte_rsrp_delta_valid     && p_msg->lte_rsrp_delta     < NAS_006C_MIN_DELTA ) ||
       ( p_msg->tdscdma_rscp_delta_valid && p_msg->tdscdma_rscp_delta < NAS_006C_MIN_DELTA ) ||
       ( p_msg->tdscdma_rssi_delta_valid && p_msg->tdscdma_rssi_delta < NAS_006C_MIN_DELTA ) ||
       ( p_msg->tdscdma_ecio_delta_valid && p_msg->tdscdma_ecio_delta < NAS_006C_MIN_DELTA ) ||
       ( p_msg->tdscdma_sinr_delta_valid && p_msg->tdscdma_sinr_delta < NAS_006C_MIN_DELTA )
    )
  {
    return QMI_ERR_INVALID_ARG_V01;
  }

  if ( p_msg->lte_sig_rpt_config_valid )
  {
    if ( p_msg->lte_sig_rpt_config.rpt_rate   > NAS_LTE_SIG_RPT_RATE_5_SEC_V01 ||
         p_msg->lte_sig_rpt_config.avg_period > NAS_LTE_SIG_AVG_PRD_5_SEC_V01  )
    {
      return QMI_ERR_INVALID_ARG_V01;
    }
  }

  return QMI_ERR_NONE_V01;
  }  


/*=========================================================================== 
  FUNCTION QMI_NAS_006E_REQ_READ()   
 
  DESCRIPTION 
    Reads QMI_NAS_SET_HPLMN_IRAT_SEARCH_TIMER_MSG into struct nas_006E_req_s 
     
  RETURN VALUE 
    QMI_ERR_NONE for successful read. For failure, returns appropriate error 
    value. 
     
  DEPENDENCIES
    None 
 
  SIDE EFFECTS 
    None 
===========================================================================*/ 
qmi_error_e_type qmi_nas_006E_req_read ( struct nas_006E_req_s  *p_msg, dsm_item_type **sdu_in ) 
{ 
  uint8   type; 
  uint16  len, expected_len; 
  void   *value; 
  struct nas_006E_req_p_s  nas_006E_req_p;

  qmi_error_e_type errval = QMI_ERR_NONE; 

  memset(&nas_006E_req_p, 0, sizeof(nas_006E_req_p));

  while (*sdu_in) 
  { 
    if ( !qmi_svc_get_tl(sdu_in, &type, &len) ) 
    continue; 

    // special value 0 = variable length or don't care (unrecognzied TLV) 
    expected_len = 0; 
    value = NULL;

    switch (type) 
    { 
      case 0x01:
        value = &nas_006E_req_p.t01;
        p_msg->t01_valid = TRUE; 
        expected_len = sizeof(nas_006E_req_p.t01);
        break;

      default: 
        QM_MSG_ERROR_1("Unrecognized TLV type (%d)", type);
        break; 
    } 

    if ((expected_len != 0) && (expected_len != len)) 
    { 
      QM_MSG_ERROR_2("Invalid TLV len (%d) for type (%d)", len, type);
      errval = QMI_ERR_MALFORMED_MSG; 
      break; 
    }

    /*----------------------------------------------------------------------- 
       If type is unrecognized, value will be NULL, and dsm_pullup will  
       free the unrecognized value, i.e. we skip past unrecognized TLVs 
    -----------------------------------------------------------------------*/ 
    if (len != dsm_pullup( sdu_in, value, len )) 
    { 
        qmi_nas_invalid_len_tlv();
      errval = QMI_ERR_MALFORMED_MSG; 
      break; 
    }
  }

  if ( errval == QMI_ERR_NONE && p_msg->t01_valid )
  {
    memscpy(&p_msg->t01.timer_value, sizeof(nas_006E_req_p.t01.timer_value),
            (void*)&nas_006E_req_p.t01.timer_value, sizeof(nas_006E_req_p.t01.timer_value) );
  }

  return errval;
} /* qmi_nas_006E_req_read */

/*=========================================================================== 
  FUNCTION QMI_NAS_006E_REQ_CHECK()  
 
  DESCRIPTION 
    Checks QMI_NAS_SET_HPLMN_IRAT_SEARCH_TIMER_MSG in struct nas_006E_req_s for errors 
     
  RETURN VALUE 
    QMI_ERR_NONE for no error or appropriate error code. 
     
  DEPENDENCIES 
    None 
 
  SIDE EFFECTS 
    None 
===========================================================================*/ 
qmi_error_e_type qmi_nas_006E_req_check (struct nas_006E_req_s *p_msg) 
{ 
  if (p_msg == NULL)
  {
    return QMI_ERR_INTERNAL;
  }

  if (!p_msg->t01_valid)
  {
    QM_MSG_ERROR("TLV 0x01 missing in QMI_NAS_SET_HPLMN_IRAT_SEARCH_TIMER_REQ");
    return QMI_ERR_MISSING_ARG;
  }

  return QMI_ERR_NONE;
} /* qmi_nas_006E_req_check */

/*=========================================================================== 
  FUNCTION QMI_NAS_006E_RSP_WRITE()  
 
  DESCRIPTION 
    Writes QMI_NAS_SET_HPLMN_IRAT_SEARCH_TIMER_MSG response in struct nas_006E_rsp_s
     
  RETURN VALUE 
    QMI_ERR_NONE for no error or appropriate error code. 
     
  DEPENDENCIES 
    None 
 
  SIDE EFFECTS 
    None 
===========================================================================*/ 
qmi_error_e_type qmi_nas_006E_rsp_write ( struct nas_006E_rsp_s *p_msg, dsm_item_type **p_response )
{
  struct nas_006E_rsp_p_s rsp_p;

  if (p_msg == NULL || p_response == NULL)
  {
    return QMI_ERR_INTERNAL;
  }

  memset(&rsp_p, 0, sizeof(rsp_p));

  if ( p_msg->t02_valid )
  {
    memscpy((void*)&rsp_p.t02.result_code, sizeof(p_msg->t02.result_code),
            (void*)&p_msg->t02.result_code, sizeof(p_msg->t02.result_code));
    memscpy((void*)&rsp_p.t02.error_code, sizeof(p_msg->t02.error_code),
            (void*)&p_msg->t02.error_code, sizeof(p_msg->t02.error_code));

    if ( !qmi_svc_put_param_tlv( p_response, 0x02, sizeof(rsp_p.t02), &rsp_p.t02 ) )
    {
      qmi_err_write(0x02, &rsp_p.t02);
      dsm_free_packet(p_response);
      return QMI_ERR_NO_MEMORY;
    }    
  }

  return QMI_ERR_NONE;
} /* qmi_nas_006E_rsp_write */

 
/*=========================================================================== 
  FUNCTION QMI_NAS_006F_REQ_CHECK() 
 
  DESCRIPTION 
    Checks QMI_NAS_GET_EMBMS_SIG_REQ_MSG in struct nas_get_embms_sig_req_msg_v01 for errors 
     
  RETURN VALUE 
    QMI_ERR_NONE for no error or appropriate error code. 
     
  DEPENDENCIES 
    None 
 
  SIDE EFFECTS 
    None 
===========================================================================*/ 
qmi_error_e_type qmi_nas_006F_req_check (nas_get_embms_sig_req_msg_v01 *p_msg) 
{ 
  if ( p_msg->trace_id_valid )
  {
    if ( p_msg->trace_id < -1 )
    {
      return QMI_ERR_INVALID_ARG;
    }
  }

  return QMI_ERR_NONE; 
}

/*=========================================================================== 
  FUNCTION QMI_NAS_0070_REQ_READ() 

  DESCRIPTION 
  Reads QMI_NAS_LIMIT_SYS_INFO_IND_REPORTING into struct nas_0070_req_s

  RETURN VALUE 
  QMI_ERR_NONE for no error or appropriate error code. 

  DEPENDENCIES 
  None 

  SIDE EFFECTS 
  None 
  ===========================================================================*/
qmi_error_e_type qmi_nas_0070_req_read(
  struct nas_0070_req_s *p_msg, 
  dsm_item_type **sdu_in
)
{
  uint8   type; 
  uint16  len, expected_len; 
  void   *value; 
  struct nas_0070_req_p_s  nas_0070_req_p;
  
  qmi_error_e_type errval = QMI_ERR_NONE; 

  memset(&nas_0070_req_p, 0, sizeof(nas_0070_req_p));

  while (*sdu_in) 
  { 
    if ( !qmi_svc_get_tl(sdu_in, &type, &len) ) 
      continue; 
 
    // special value 0 = variable length or don't care (unrecognzied TLV) 
    expected_len = 0; 
    value = NULL; 
 
    switch (type) 
    { 
      case NAS_0070_REQ_T01:
        value = &nas_0070_req_p.t01;
        p_msg->t01_valid = TRUE; 
        expected_len = sizeof(nas_0070_req_p.t01);
        break;

      default: 
        QM_MSG_ERROR_1("Unrecognized TLV type (%d)", type);
        break; 
    } 
 
    if ((expected_len != 0) && (expected_len != len)) 
    { 
      QM_MSG_ERROR_2("Invalid TLV len (%d) for type (%d)", len, type);
      errval = QMI_ERR_MALFORMED_MSG; 
      break; 
    } 
 
    /*----------------------------------------------------------------------- 
      If type is unrecognized, value will be NULL, and dsm_pullup will  
      free the unrecognized value, i.e. we skip past unrecognized TLVs 
    -----------------------------------------------------------------------*/ 
    if (len != dsm_pullup( sdu_in, value, len )) 
    { 
        qmi_nas_invalid_len_tlv();
      errval = QMI_ERR_MALFORMED_MSG; 
      break; 
    } 
  }

  if ( errval == QMI_ERR_NONE && p_msg->t01_valid )
  {
    memscpy(&p_msg->t01.limit_sys_info_rpt_chg, sizeof(nas_0070_req_p.t01.limit_sys_info_rpt_chg),
            (void*)&nas_0070_req_p.t01.limit_sys_info_rpt_chg, sizeof(nas_0070_req_p.t01.limit_sys_info_rpt_chg) );
  }
 
  return errval;
}

/*=========================================================================== 
  FUNCTION QMI_NAS_0070_REQ_CHECK() 

  DESCRIPTION 
  Checks QMI_NAS_LIMIT_SYS_INFO_IND_REPORTING in struct nas_0070_req_s for errors

  RETURN VALUE 
  QMI_ERR_NONE for no error or appropriate error code. 

  DEPENDENCIES 
  None 

  SIDE EFFECTS 
  None 
  ===========================================================================*/
qmi_error_e_type qmi_nas_0070_req_check(struct nas_0070_req_s *p_msg)
{
  if (p_msg == NULL)
  {
    return QMI_ERR_INTERNAL;
  }

  if (!p_msg->t01_valid)
  {
    QM_MSG_ERROR("TLV 0x01 missing in QMI_NAS_LIMIT_SYS_INFO_IND_REPORTING");
    return QMI_ERR_MISSING_ARG;
  }
  else if ( p_msg->t01.limit_sys_info_rpt_chg & (uint64)~QMI_NAS_LIMIT_ALL )
  {
    QM_MSG_ERROR_1("invalid TLV 0x01 val %d in QMI_NAS_LIMIT_SYS_INFO_IND_REPORTING", p_msg->t01.limit_sys_info_rpt_chg);
    return QMI_ERR_INVALID_ARG;
  }

  return QMI_ERR_NONE;
}

/*=========================================================================== 
  FUNCTION QMI_NAS_0070_RSP_WRITE() 

  DESCRIPTION 
  Writes QMI_NAS_LIMIT_SYS_INFO_IND_REPORTING response in struct nas_0070_rsp_s

  RETURN VALUE 
  QMI_ERR_NONE for no error or appropriate error code. 

  DEPENDENCIES 
  None 

  SIDE EFFECTS 
  None 
  ===========================================================================*/
qmi_error_e_type qmi_nas_0070_rsp_write(struct nas_0070_rsp_s *p_msg, dsm_item_type **response)
{
  struct nas_0070_rsp_p_s rsp_p;

  if (p_msg == NULL || response == NULL)
  {
    return QMI_ERR_INTERNAL;
  }

  memset(&rsp_p, 0, sizeof(rsp_p));

  if ( p_msg->t02_valid )
  {
    memscpy((void*)&rsp_p.t02.result_code, sizeof(p_msg->t02.result_code),
            (void*)&p_msg->t02.result_code, sizeof(p_msg->t02.result_code));
    memscpy((void*)&rsp_p.t02.error_code, sizeof(p_msg->t02.error_code),
            (void*)&p_msg->t02.error_code, sizeof(p_msg->t02.error_code));

    if ( !qmi_svc_put_param_tlv( response, 0x02, sizeof(rsp_p.t02), &rsp_p.t02 ) )
    {
      qmi_err_write(0x02, &rsp_p.t02);
      dsm_free_packet(response);
      return QMI_ERR_NO_MEMORY;
    }    
  }

  return QMI_ERR_NONE;
}

/*=========================================================================== 
  FUNCTION QMI_NAS_0071_RSP_WRITE() 

  DESCRIPTION 
  Writes QMI_NAS_GET_SYS_INFO_IND_REPORTING_LIMIT response in struct nas_0071_rsp_s

  RETURN VALUE 
  QMI_ERR_NONE for no error or appropriate error code. 

  DEPENDENCIES 
  None 

  SIDE EFFECTS 
  None 
  ===========================================================================*/
qmi_error_e_type qmi_nas_0071_rsp_write(struct nas_0071_rsp_s *p_msg, dsm_item_type **response)
{
  struct nas_0071_rsp_p_s rsp_p;

  if (p_msg == NULL || response == NULL)
  {
    return QMI_ERR_INTERNAL;
  }

  memset(&rsp_p, 0, sizeof(rsp_p));

  if ( p_msg->t01_valid )
  {
    memscpy((void*)&rsp_p.t01.limit_sys_info_rpt_chg, sizeof(p_msg->t01.limit_sys_info_rpt_chg),
            (void*)&p_msg->t01.limit_sys_info_rpt_chg, sizeof(p_msg->t01.limit_sys_info_rpt_chg));

    if ( !qmi_svc_put_param_tlv( response, NAS_0071_RSP_T01, sizeof(rsp_p.t01), &rsp_p.t01 ) )
    {
      qmi_err_write(NAS_0071_RSP_T01, &rsp_p.t01);
      dsm_free_packet(response);
      return QMI_ERR_NO_MEMORY;
    }
  }

  if ( p_msg->t02_valid )
  {
    memscpy((void*)&rsp_p.t02.result_code, sizeof(p_msg->t02.result_code),
            (void*)&p_msg->t02.result_code, sizeof(p_msg->t02.result_code));
    memscpy((void*)&rsp_p.t02.error_code, sizeof(p_msg->t02.error_code),
            (void*)&p_msg->t02.error_code, sizeof(p_msg->t02.error_code));

    if ( !qmi_svc_put_param_tlv( response, 0x02, sizeof(rsp_p.t02), &rsp_p.t02 ) )
    {
      qmi_err_write(0x02, &rsp_p.t02);
      dsm_free_packet(response);
      return QMI_ERR_NO_MEMORY;
    }
  }

  return QMI_ERR_NONE;
}

/*=========================================================================== 
  FUNCTION QMI_NAS_0072_REQ_CHECK()
 
  DESCRIPTION 
    Checks QMI_NAS_UPDATE_IMS_STATUS_REQ_MSG in struct nas_update_ims_status_req_msg_v01
    for errors 
     
  RETURN VALUE 
    QMI_ERR_NONE_V01 for no error or appropriate error code. 
     
  DEPENDENCIES 
    None 
 
  SIDE EFFECTS 
    None 
===========================================================================*/ 
qmi_error_type_v01 qmi_nas_0072_req_check(nas_update_ims_status_req_msg_v01 *p_msg)
{
  int i;
  boolean voice_received = FALSE;
  boolean sms_received = FALSE;

  if (p_msg == NULL)
  {
    return QMI_ERR_INTERNAL_V01;
  }

  if (p_msg->sys_mode != NAS_RADIO_IF_CDMA_1XEVDO_V01 && 
      p_msg->sys_mode != NAS_RADIO_IF_LTE_V01 &&
      p_msg->sys_mode != NAS_RADIO_IF_GSM_V01 && 
      p_msg->sys_mode != NAS_RADIO_IF_UMTS_V01 &&
      p_msg->sys_mode != NAS_RADIO_IF_CDMA_1X_V01 &&
      p_msg->sys_mode != NAS_RADIO_IF_TDSCDMA_V01)
  {
    QM_MSG_ERROR_1("Invalid sys_mode %d", p_msg->sys_mode);
    return QMI_ERR_INVALID_ARG_V01;
  }

  if (p_msg->registration_state_len == 0)
  {
    QM_MSG_ERROR_1("Invalid registration length %d", p_msg->registration_state_len);
    return QMI_ERR_INVALID_ARG_V01;
  }
  else if (p_msg->registration_state_len > NAS_IMS_REG_STATUS_MAX_V01)
  {
    return QMI_ERR_ARG_TOO_LONG_V01;
  }
  else
  {
    for (i = 0; i < p_msg->registration_state_len; i++)
    {
      if (p_msg->registration_state[i].call_type == NAS_CALL_TYPE_E_VOICE_V01 )
      {
        if ( voice_received )
        {
          QM_MSG_ERROR("Already received voice call type information");
          return QMI_ERR_INVALID_ARG_V01;
        }
        else
        {
          voice_received = TRUE;
        }
      }
      else if (p_msg->registration_state[i].call_type == NAS_CALL_TYPE_E_SMS_V01 )
      {
        if ( sms_received )
        {
          QM_MSG_ERROR("Already received sms call type information");
          return QMI_ERR_INVALID_ARG_V01;
        }
        else
        {
          sms_received = TRUE;
        }
      }
      else
      {
        QM_MSG_ERROR_1("Invalid call type %d", p_msg->registration_state[i].call_type);
        return QMI_ERR_INVALID_ARG_V01;
      }

      if (p_msg->registration_state[i].is_registered != TRUE &&
          p_msg->registration_state[i].is_registered != FALSE)
      {
        QM_MSG_ERROR_1("Invalid registration %d", p_msg->registration_state[i].is_registered);
        return QMI_ERR_INVALID_ARG_V01;
      }
    }
  }

  return QMI_ERR_NONE_V01;
}

/*=========================================================================== 
  FUNCTION QMI_NAS_0075_REQ_CHECK()

  DESCRIPTION 
    Checks received data in nas_config_plmn_name_ind_reporting for errors

  RETURN VALUE 
    QMI_ERR_NONE_V01 for no error or appropriate error code.

  DEPENDENCIES 
  None 

  SIDE EFFECTS 
  None 
  ===========================================================================*/ 
qmi_error_type_v01 qmi_nas_0075_req_check ( 
  nas_config_plmn_name_ind_reporting_req_msg_v01 *p_msg 
)
{
  if ( p_msg->send_all_information != TRUE &&
       p_msg->send_all_information != FALSE )
  {
    QM_MSG_ERROR_1("invalid TLV 0x01 val %d in QMI_NAS_CONFIG_PLMN_NAME_IND_REPORTING", p_msg->send_all_information);
    return QMI_ERR_INVALID_ARG_V01;
  }

  return QMI_ERR_NONE_V01;
}

/*=========================================================================== 
  FUNCTION QMI_NAS_0076_REQ_CHECK()

  DESCRIPTION 
    Checks received data in nas_cdma_avoid_system_req_msg_v01 for errors

  RETURN VALUE 
    QMI_ERR_NONE_V01 for no error or appropriate error code.

  DEPENDENCIES 
  None 

  SIDE EFFECTS 
  None 
  ===========================================================================*/ 
qmi_error_type_v01 qmi_nas_0076_req_check(
  nas_cdma_avoid_system_req_msg_v01 * p_msg
)
{
  // check mandatory TLV
  if ( p_msg->avoid_type != NAS_AVOID_SYS_USERZONE_V01  &&
       p_msg->avoid_type != NAS_AVOID_SYS_IDLE_V01 && 
       p_msg->avoid_type != NAS_AVOID_SYS_CLR_LIST_V01 )
  {
    return QMI_ERR_INVALID_ARG_V01;
  }
  return QMI_ERR_NONE_V01;
}

/*=========================================================================== 
  FUNCTION QMI_NAS_0078_REQ_READ() 

  DESCRIPTION 
  Reads QMI_NAS_SET_HPLMN_SEARCH_TIMER into struct nas_0078_req_s

  RETURN VALUE 
  QMI_ERR_NONE for no error or appropriate error code. 

  DEPENDENCIES 
  None 

  SIDE EFFECTS 
  None 
  ===========================================================================*/
qmi_error_e_type qmi_nas_0078_req_read(
  struct nas_0078_req_s *p_msg,
  dsm_item_type **sdu_in
)
{
  uint8   type; 
  uint16  len, expected_len; 
  void   *value; 
  struct nas_0078_req_p_s  nas_0078_req_p;
  
  qmi_error_e_type errval = QMI_ERR_NONE; 

  memset(&nas_0078_req_p, 0, sizeof(nas_0078_req_p));

  while (*sdu_in) 
  { 
    if ( !qmi_svc_get_tl(sdu_in, &type, &len) ) 
      continue; 
 
    // special value 0 = variable length or don't care (unrecognzied TLV) 
    expected_len = 0; 
    value = NULL; 
 
    switch (type) 
    { 
      case NAS_0078_REQ_T01:
        value = &nas_0078_req_p.t01;
        p_msg->t01_valid = TRUE; 
        expected_len = sizeof(nas_0078_req_p.t01);
        break;

      default: 
        QM_MSG_ERROR_1("Unrecognized TLV type (%d)", type);
        break; 
    } 
 
    if ((expected_len != 0) && (expected_len != len)) 
    { 
      QM_MSG_ERROR_2("Invalid TLV len (%d) for type (%d)", len, type);
      errval = QMI_ERR_MALFORMED_MSG; 
      break; 
    } 
 
    /*----------------------------------------------------------------------- 
      If type is unrecognized, value will be NULL, and dsm_pullup will  
      free the unrecognized value, i.e. we skip past unrecognized TLVs 
    -----------------------------------------------------------------------*/ 
    if (len != dsm_pullup( sdu_in, value, len )) 
    { 
      qmi_nas_invalid_len_tlv();
      errval = QMI_ERR_MALFORMED_MSG; 
      break; 
    } 
  }

  if ( errval == QMI_ERR_NONE && p_msg->t01_valid )
  {
    memscpy(&p_msg->t01.timer_value, sizeof(nas_0078_req_p.t01.timer_value),
            (void*)&nas_0078_req_p.t01.timer_value, sizeof(nas_0078_req_p.t01.timer_value) );
  }
 
  return errval;
}

/*=========================================================================== 
  FUNCTION QMI_NAS_0078_REQ_CHECK() 

  DESCRIPTION 
  Checks QMI_NAS_SET_HPLMN_SEARCH_TIMER in struct nas_0078_req_s for errors

  RETURN VALUE 
  QMI_ERR_NONE for no error or appropriate error code. 

  DEPENDENCIES 
  None 

  SIDE EFFECTS 
  None 
  ===========================================================================*/
qmi_error_e_type qmi_nas_0078_req_check(struct nas_0078_req_s *p_msg)
{
  if (p_msg == NULL)
  {
    return QMI_ERR_INTERNAL;
  }

  if (!p_msg->t01_valid)
  {
    QM_MSG_ERROR("TLV 0x01 missing in QMI_NAS_SET_HPLMN_SEARCH_TIMER");
    return QMI_ERR_MISSING_ARG;
  }

  return QMI_ERR_NONE;
}

/*=========================================================================== 
  FUNCTION QMI_NAS_0078_RSP_WRITE() 

  DESCRIPTION 
  Writes QMI_NAS_SET_HPLMN_SEARCH_TIMER response in struct nas_0078_rsp_s

  RETURN VALUE 
  QMI_ERR_NONE for no error or appropriate error code. 

  DEPENDENCIES 
  None 

  SIDE EFFECTS 
  None 
  ===========================================================================*/
qmi_error_e_type qmi_nas_0078_rsp_write(struct nas_0078_rsp_s *p_msg, dsm_item_type **response)
{
  struct nas_0078_rsp_p_s rsp_p;

  if (p_msg == NULL || response == NULL)
  {
    return QMI_ERR_INTERNAL;
  }

  memset(&rsp_p, 0, sizeof(rsp_p));

  if ( p_msg->t02_valid )
  {
    memscpy((void*)&rsp_p.t02.result_code, sizeof(p_msg->t02.result_code),
            (void*)&p_msg->t02.result_code, sizeof(p_msg->t02.result_code));
    memscpy((void*)&rsp_p.t02.error_code, sizeof(p_msg->t02.error_code),
            (void*)&p_msg->t02.error_code, sizeof(p_msg->t02.error_code));

    if ( !qmi_svc_put_param_tlv( response, 0x02, sizeof(rsp_p.t02), &rsp_p.t02 ) )
    {
      qmi_err_write(0x02, &rsp_p.t02);
      dsm_free_packet(response);
      return QMI_ERR_NO_MEMORY;
    }    
  }

  return QMI_ERR_NONE;
}

/*=========================================================================== 
  FUNCTION QMI_NAS_0079_RSP_WRITE() 

  DESCRIPTION 
  Writes QMI_NAS_GET_HPLMN_SEARCH_TIMER response in struct nas_0079_rsp_s

  RETURN VALUE 
  QMI_ERR_NONE for no error or appropriate error code. 

  DEPENDENCIES 
  None 

  SIDE EFFECTS 
  None 
  ===========================================================================*/
qmi_error_e_type qmi_nas_0079_rsp_write(struct nas_0079_rsp_s *p_msg, dsm_item_type **response)
{
  struct nas_0079_rsp_p_s rsp_p;

  if (p_msg == NULL || response == NULL)
  {
    return QMI_ERR_INTERNAL;
  }

  memset(&rsp_p, 0, sizeof(rsp_p));

  if ( p_msg->t02_valid )
  {
    memscpy((void*)&rsp_p.t02.result_code, sizeof(p_msg->t02.result_code),
            (void*)&p_msg->t02.result_code, sizeof(p_msg->t02.result_code));
    memscpy((void*)&rsp_p.t02.error_code, sizeof(p_msg->t02.error_code),
            (void*)&p_msg->t02.error_code, sizeof(p_msg->t02.error_code));

    if ( !qmi_svc_put_param_tlv( response, 0x02, sizeof(rsp_p.t02), &rsp_p.t02 ) )
    {
      qmi_err_write(0x02, &rsp_p.t02);
      dsm_free_packet(response);
      return QMI_ERR_NO_MEMORY;
    }
  }

  if ( p_msg->t10_valid )
  {
    memscpy((void*)&rsp_p.t10.timer_value, sizeof(p_msg->t10.timer_value),
            (void*)&p_msg->t10.timer_value, sizeof(p_msg->t10.timer_value));

    if ( !qmi_svc_put_param_tlv( response, NAS_0079_RSP_T10, sizeof(rsp_p.t10), &rsp_p.t10 ) )
    {
      qmi_err_write(NAS_0079_RSP_T10, &rsp_p.t10 );
      dsm_free_packet(response);
      return QMI_ERR_NO_MEMORY;
    }
  }

  return QMI_ERR_NONE;
}
/*=========================================================================== 
  FUNCTION QMI_NAS_0080_REQ_CHECK()
 
  DESCRIPTION 
    Checks QMI_NAS_SET_LTE_BAND_PRIORITY_REQ_MSG in struct nas_set_lte_band_priority_req_msg_v01
    for errors 
     
  RETURN VALUE 
    QMI_ERR_NONE_V01 for no error or appropriate error code. 
     
  DEPENDENCIES 
    None 
 
  SIDE EFFECTS 
    None 
===========================================================================*/ 
qmi_error_type_v01 qmi_nas_0080_req_check(nas_set_lte_band_priority_req_msg_v01 *p_msg)
{
  if (p_msg == NULL)
  {
    return QMI_ERR_INTERNAL_V01;
  }
  else if ( p_msg->band_priority_list_len > NAS_LTE_BAND_PRIORITY_LIST_MAX_V01 )
  {
    return QMI_ERR_INVALID_ARG_V01;
  }

  return QMI_ERR_NONE_V01;
}

/*=========================================================================== 
  FUNCTION QMI_NAS_0081_REQ_CHECK() 
 
  DESCRIPTION 
    Checks QMI_NAS_GET_EMBMS_SIG_EXT_REQ_MSG in struct 
    nas_get_embms_sig_ext_req_msg_v01 for errors 
     
  RETURN VALUE 
    QMI_ERR_NONE for no error or appropriate error code. 
     
  DEPENDENCIES 
    None 
 
  SIDE EFFECTS 
    None 
===========================================================================*/ 
qmi_error_type_v01 qmi_nas_0081_req_check( nas_get_embms_sig_ext_req_msg_v01 *p_msg ) 
{ 
  if ( p_msg->trace_id_valid )
  {
    if ( p_msg->trace_id < -1 )
    {
      return QMI_ERR_INVALID_ARG_V01;
    }
  }

  return QMI_ERR_NONE_V01;
}

/*=========================================================================== 
  FUNCTION QMI_NAS_0084_REQ_CHECK()

  DESCRIPTION 
    Checks received data in nas_set_builtin_plmn_list_req_msg_v01 for errors

  RETURN VALUE 
    QMI_ERR_NONE for no error or appropriate error code.

  DEPENDENCIES 
  None 

  SIDE EFFECTS 
  None 
  ===========================================================================*/ 
qmi_error_e_type qmi_nas_0084_req_check(
  nas_set_builtin_plmn_list_req_msg_v01 * p_msg
)
{
  // check if PLMN List is present
  if ( !p_msg->oplmn_list_valid )
  {
    return QMI_ERR_MISSING_ARG;
  }

  // check for max entries
  if((p_msg->oplmn_list.total_list_entries > NAS_MAX_BUILTIN_OPLMN_ENTRIES_V01) || 
      (p_msg->oplmn_list.oplmn_len > NAS_MAX_BUILTIN_OPLMN_ENTRIES_V01) || 
      (p_msg->oplmn_list.oplmn_len > p_msg->oplmn_list.total_list_entries) )
  {
    return QMI_ERR_ARG_TOO_LONG;
  }

  return QMI_ERR_NONE;
}

/*=========================================================================== 
  FUNCTION qmi_nas_0085_req_check() 
 
  DESCRIPTION 
    Checks QMI_NAS_PERFORM_INCREMENTAL_NETWORK_SCAN in nas_perform_incremental_network_scan_req_msg_v01
    for errors 
     
  RETURN VALUE 
    QMI_ERR_NONE for no error or appropriate error code. 
     
  DEPENDENCIES 
    None 
 
  SIDE EFFECTS 
    None 
===========================================================================*/
qmi_error_type_v01 qmi_nas_0085_req_check( nas_perform_incremental_network_scan_req_msg_v01 *p_msg)
{
  if ( !p_msg )
  {
    return QMI_ERR_INTERNAL_V01;
  }

  if ( p_msg->network_type_valid )
  {
    if ( p_msg->network_type != 0 &&
         !(p_msg->network_type & NAS_NETWORK_TYPE_GSM_ONLY_V01) &&
         !(p_msg->network_type & NAS_NETWORK_TYPE_WCDMA_ONLY_V01) &&
         !(p_msg->network_type & NAS_NETWORK_TYPE_LTE_ONLY_V01) &&
         !(p_msg->network_type & NAS_NETWORK_TYPE_TDSCDMA_ONLY_V01) )
    {
      return QMI_ERR_INVALID_ARG_V01;
    }
  }

  if ( p_msg->scan_type_valid )
  {
    if ( p_msg->scan_type != NAS_SCAN_TYPE_PLMN_V01 &&
         p_msg->scan_type != NAS_SCAN_TYPE_CSG_V01  &&
         p_msg->scan_type != NAS_SCAN_TYPE_MODE_PREF_V01 )
    {
      return QMI_ERR_INVALID_ARG_V01;
    }
  }

  return QMI_ERR_NONE_V01;
} /* qmi_nas_0085_req_check */

/*=========================================================================== 
  FUNCTION QMI_NAS_0088_REQ_CHECK() 
 
  DESCRIPTION 
    Checks QMI_NAS_SET_DRX_REQ_MSG in struct 
    nas_set_drx_req_msg_v01 for errors 
     
  RETURN VALUE 
    QMI_ERR_NONE for no error or appropriate error code. 
     
  DEPENDENCIES 
    None 

  SIDE EFFECTS 
    None 
===========================================================================*/ 
qmi_error_type_v01 qmi_nas_0088_req_check( nas_set_drx_req_msg_v01 *p_msg ) 
{ 
  if ( p_msg->drx != NAS_DRX_UNKNOWN_V01 &&
       p_msg->drx != NAS_DRX_CN6_T32_V01 &&
       p_msg->drx != NAS_DRX_CN7_T64_V01 &&
       p_msg->drx != NAS_DRX_CN8_T128_V01 &&
       p_msg->drx != NAS_DRX_CN9_T256_V01 )
  {
    return QMI_ERR_INVALID_ARG_V01;
  }

  return QMI_ERR_NONE_V01;
}

qmi_error_type_v01 qmi_nas_008A_req_check ( nas_csg_search_selection_config_req_msg_v01 *p_msg)
{
  if ( p_msg == NULL )
  {
    return QMI_ERR_MALFORMED_MSG_V01;
  }

  if ( p_msg->network_type_valid )
  {
    if ( p_msg->network_type != 0 &&
         !(p_msg->network_type & NAS_NETWORK_TYPE_WCDMA_ONLY_V01) &&
         !(p_msg->network_type & NAS_NETWORK_TYPE_LTE_ONLY_V01)
       )
    {
      return QMI_ERR_INVALID_ARG_V01;
    }
  }

  if ( p_msg->selection_config_type_valid)
  {
    if ( p_msg->selection_config_type != NAS_CSG_SELECTION_CONFIG_1_V01 )
    {
      return QMI_ERR_INVALID_ARG_V01;
    }
  }

  if( !p_msg->network_type_valid || !p_msg->selection_config_type_valid )
  {
    return QMI_ERR_MISSING_ARG_V01;
  }

  return QMI_ERR_NONE_V01;

}


/*===========================================================================
  FUNCTION QMI_NAS_008F_REQ_CHECK()

  DESCRIPTION
    Checks QMI_NAS_GET_SERV_CELL_SIB_REQ_MSG in struct
    nas_get_serv_cell_sib_req_msg_v01 for errors

  RETURN VALUE
    QMI_ERR_NONE for no error or appropriate error code.

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
qmi_error_type_v01 qmi_nas_008F_req_check ( nas_get_serv_cell_sib_req_msg_v01 *p_msg)
{
  if(p_msg == NULL)
  {
    return QMI_ERR_MALFORMED_MSG_V01;
  }

  if(
      (p_msg->sib_num < NAS_SIB_NUM_MIN_VALID) ||
      (p_msg->sib_num > NAS_SIB_NUM_MAX_VALID)
    )
  {
    return QMI_ERR_INVALID_ARG_V01;
  }

  return QMI_ERR_NONE_V01;
}

/*=========================================================================== 
  FUNCTION QMI_NAS_0092_REQ_CHECK() 
 
  DESCRIPTION 
    Checks QMI_NAS_SET_PERIODIC_SEARCH_ALLOWED_REQ_MSG  in struct 
    nas_set_periodic_search_allowed_req_msg_v01 for errors 
     
  RETURN VALUE 
    QMI_ERR_NONE for no error or appropriate error code. 
     
  DEPENDENCIES 
    None 

  SIDE EFFECTS 
    None 
===========================================================================*/ 
qmi_error_type_v01 qmi_nas_0092_req_check( nas_set_periodic_search_allowed_req_msg_v01 *p_msg ) 
{ 
  if ( p_msg->allowed != 0 && p_msg->allowed != 1 )
  {
    return QMI_ERR_INVALID_ARG_V01;
  }

  return QMI_ERR_NONE_V01;
}

/*===========================================================================
  FUNCTION QMI_NAS_009A_REQ_CHECK()

  DESCRIPTION
    Checks QMI_NAS_SET_DATA_ROAMING_REQ_MSG in struct
    nas_set_data_roaming_req_msg_v01 for errors

  RETURN VALUE
    QMI_ERR_NONE for no error or appropriate error code.

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
qmi_error_type_v01 qmi_nas_009A_req_check ( nas_set_data_roaming_req_msg_v01 *p_msg)
{
  if(p_msg == NULL)
  {
    return QMI_ERR_MALFORMED_MSG_V01;
  }

  if(
      (p_msg->data_roam_status < NAS_DATA_ROAMING_ON_V01) ||
      (p_msg->data_roam_status > NAS_DATA_ROAMING_OFF_V01)
    )
  {
    return QMI_ERR_INVALID_ARG_V01;
  }

  return QMI_ERR_NONE_V01;
}

