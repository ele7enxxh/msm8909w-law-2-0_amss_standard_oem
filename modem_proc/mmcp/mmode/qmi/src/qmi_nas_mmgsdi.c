/*===========================================================================

                         QMI_NAS_MMGSDI.C

DESCRIPTION

 The Qualcomm Network Access Services MMGSDI Interface source file.

Copyright (c) 2010-2014 QUALCOMM Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary.
===========================================================================*/
/*===========================================================================

                      EDIT HISTORY FOR FILE

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/mmode/qmi/src/qmi_nas_mmgsdi.c#1 $ $DateTime: 2016/12/13 08:00:05 $ $Author: mplcsds1 $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
06/03/10    hs     Initial version
===========================================================================*/

#include "comdef.h"
#include "customer.h"

#include "msg_diag_service.h"
#include "qmi_nas.h"
#include "qmi_nas_internal.h"
#include "qmi_nas_utils.h"
#include "mmgsdisessionlib.h"
#include "mmgsdilib.h"
#include "modem_mem.h"
#include "qmi_mmode_task_cmd.h"
#include "qm_util.h"

#define CDMA_SPN_NAME_TEXT_BEGIN_IDX (3) // beginning of Service provider name field (index starts from 0)

#define QMI_NAS_VALUE_ADDED_SERVICE_CODE  (0xC0)  // per CPHS Phase2 ver 4.2 cl. B.4.7.1

const mmgsdi_file_enum_type refresh_file_list[] = {
  MMGSDI_GSM_AD,   MMGSDI_USIM_AD,
  MMGSDI_GSM_IMSI, MMGSDI_USIM_IMSI,
  MMGSDI_GSM_SPN,  MMGSDI_USIM_SPN,
  MMGSDI_GSM_OPL,  MMGSDI_USIM_OPL,
  MMGSDI_GSM_PNN,  MMGSDI_USIM_PNN,
  MMGSDI_GSM_ONS,  MMGSDI_USIM_ONS,
  MMGSDI_GSM_CSP,  MMGSDI_USIM_CSP
};

void qmi_nas_mmgsdi_deep_copy( void** dest, void* src, uint32 size );

/*===========================================================================
  FUNCTION qmi_nas_mmgsdi_is_valid_session()

  DESCRIPTION
    Verifies whether session is a valid mmgsdi session or not

  PARAMETERS
    mmgsdi_session_type_enum_type session

  RETURN VALUE 
    Returns TRUE if session matches, otherwise FALSE    
===========================================================================*/
boolean qmi_nas_mmgsdi_is_valid_session
(
  mmgsdi_session_type_enum_type session
)
{
  boolean retval = FALSE;

  switch(session)
  {
    case MMGSDI_GW_PROV_PRI_SESSION:
    case MMGSDI_1X_PROV_PRI_SESSION:
    case MMGSDI_GW_PROV_SEC_SESSION:
    case MMGSDI_GW_PROV_TER_SESSION:
      retval = TRUE;
      break;

    default:
      MSG_ERROR( "mmgsdi session id doesn't match %d", session, 0, 0 );
  }
  return retval;
}

/*===========================================================================
  FUNCTION qmi_nas_mmgsdi_is_valid_qmi_session()

  DESCRIPTION
    Verifies whether session is a valid qmi mmgsdi session or not

  PARAMETERS
    qmi_nas_mmgsdi_session_e_type session

  RETURN VALUE 
    Returns TRUE if session matches, otherwise FALSE    
===========================================================================*/
boolean qmi_nas_mmgsdi_is_valid_qmi_session
(
  qmi_nas_mmgsdi_session_e_type qmi_session
)
{
  boolean retval = FALSE;

  switch(qmi_session)
  {
    case QMI_NAS_MMGSDI_GW_PRI_SESSION:
    case QMI_NAS_MMGSDI_1x_PRI_SESSION:
    case QMI_NAS_MMGSDI_GW_SEC_SESSION:
#ifdef FEATURE_TRIPLE_SIM    
    case QMI_NAS_MMGSDI_GW_TER_SESSION:
#endif
      retval = TRUE;
      break;

    default:
      MSG_ERROR( "qmi session id doesn't match %d", qmi_session, 0, 0 );
  }
  return retval;
}

/*===========================================================================
  FUNCTION qmi_nas_mmgsdi_is_valid_3gpp_session()

  DESCRIPTION
    Verifies whether session is a valid 3GPP mmgsdi session or not

  PARAMETERS
    mmgsdi_session_type_enum_type session

  RETURN VALUE 
    Returns TRUE if session matches, otherwise FALSE    
===========================================================================*/
boolean qmi_nas_mmgsdi_is_valid_3gpp_session
(
  mmgsdi_session_type_enum_type session
)
{
  boolean retval = FALSE;

  switch(session)
  {
    case MMGSDI_GW_PROV_PRI_SESSION:
    case MMGSDI_GW_PROV_SEC_SESSION:
    case MMGSDI_GW_PROV_TER_SESSION:
      retval = TRUE;
      break;

    default:
      MSG_ERROR( "mmgsdi 3gpp session id doesn't match %d", session, 0, 0 );
  }
  return retval;
}

/*===========================================================================
  FUNCTION qmi_nas_map_mmgsdi_session()

  DESCRIPTION
    Maps QMI_NAS qmi_nas_mmgsdi_session_e_type to  mmgsdi_session_type_enum_type session.

  PARAMETERS
    mmgsdi_session_type_enum_type session 

  RETURN VALUE 
    Returns mapped enum of type mmgsdi_session_type_enum_type    
===========================================================================*/
mmgsdi_session_type_enum_type qmi_nas_map_mmgsdi_session
(
  qmi_nas_mmgsdi_session_e_type qmi_session
)
{
  mmgsdi_session_type_enum_type retval = MMGSDI_MAX_SESSION_TYPE_ENUM;

  switch(qmi_session)
  {
    case QMI_NAS_MMGSDI_GW_PRI_SESSION:
      retval = MMGSDI_GW_PROV_PRI_SESSION;
      break;
    case QMI_NAS_MMGSDI_1x_PRI_SESSION: 
      retval = MMGSDI_1X_PROV_PRI_SESSION;
      break;
    case QMI_NAS_MMGSDI_GW_SEC_SESSION:
      retval = MMGSDI_GW_PROV_SEC_SESSION;
      break;
#ifdef FEATURE_TRIPLE_SIM    
    case QMI_NAS_MMGSDI_GW_TER_SESSION:
      retval = MMGSDI_GW_PROV_TER_SESSION;
      break;
#endif
    default:
      MSG_ERROR( "qmi mmgsdi session id doesn't match %d, returning MAX session id", qmi_session, 0, 0 );
      break;
  }
  return retval;
}

/*===========================================================================
  FUNCTION qmi_nas_map_mmgsdi_session_to_qmi()

  DESCRIPTION
    Maps MMGSDI mmgsdi_session_type_enum_type to qmi_nas_mmgsdi_session_e_type session.

  PARAMETERS
    qmi_nas_mmgsdi_session_e_type session 

  RETURN VALUE 
    Returns mapped enum of type qmi_nas_mmgsdi_session_e_type    
===========================================================================*/
qmi_nas_mmgsdi_session_e_type qmi_nas_map_mmgsdi_session_to_qmi
(
  mmgsdi_session_type_enum_type session
)
{
  qmi_nas_mmgsdi_session_e_type qmi_session = QMI_NAS_MMGSDI_SESSION_MAX;

  switch(session)
  {
    case MMGSDI_GW_PROV_PRI_SESSION:
      qmi_session = QMI_NAS_MMGSDI_GW_PRI_SESSION;
      break;
    case MMGSDI_1X_PROV_PRI_SESSION: 
      qmi_session = QMI_NAS_MMGSDI_1x_PRI_SESSION;
      break;
    case MMGSDI_GW_PROV_SEC_SESSION:
      qmi_session = QMI_NAS_MMGSDI_GW_SEC_SESSION;
      break;
#ifdef FEATURE_TRIPLE_SIM    
    case MMGSDI_GW_PROV_TER_SESSION:
      qmi_session = QMI_NAS_MMGSDI_GW_TER_SESSION;
      break;
#endif
    default:
      QM_MSG_ERROR_1("mmgsdi session id doesn't match %d, returning MAX session id", qmi_session);
      break;
  }
  return qmi_session;
}

/*===========================================================================
  FUNCTION qmi_nas_mmgsdi_change_cache_state()

  DESCRIPTION
    Change state of all cached files

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
void qmi_nas_mmgsdi_change_cache_state( qmi_nas_mmgsdi_session_e_type qmi_session, enum qmi_nas_mmgsdi_cache_ef_e ef, enum qmi_nas_mmgsdi_tristate_s state )
{

  if ( ef <= QMI_NAS_MMGSDI_EF_NONE || ef > QMI_NAS_MMGSDI_EF_ALL || state == QMI_NAS_MMGSDI_VALID)
  {
    QM_MSG_ERROR_2("invalid ef(%d) or state(%d)", ef, state);
    return;
  }

  if ( !qmi_nas_mmgsdi_is_valid_qmi_session(qmi_session) )
  {
    QM_MSG_ERROR_1("invalid session %d", qmi_session);
    return;
  }

  if ( ef == QMI_NAS_MMGSDI_EF_AD || ef == QMI_NAS_MMGSDI_EF_ALL )
  {
    qmi_nasi_global.mmgsdi.cache[qmi_session].ef_ad_valid = state;
    if ( state == QMI_NAS_MMGSDI_ERROR )
    {
      qmi_nasi_global.mmgsdi.cache[qmi_session].ef_ad_size = 0;
      memset( qmi_nasi_global.mmgsdi.cache[qmi_session].ef_ad, 0x00, QMI_NAS_MMGSDI_LEN_EF_AD );
    }
  }

  if ( ef == QMI_NAS_MMGSDI_EF_IMSI || ef == QMI_NAS_MMGSDI_EF_ALL )
  {
    qmi_nasi_global.mmgsdi.cache[qmi_session].ef_imsi_valid = state;
    if ( state == QMI_NAS_MMGSDI_ERROR )
    {
      qmi_nasi_global.mmgsdi.cache[qmi_session].ef_imsi_size = 0;
      memset( qmi_nasi_global.mmgsdi.cache[qmi_session].ef_imsi, 0x00, QMI_NAS_MMGSDI_LEN_EF_IMSI );
    }
  }

  if ( ef == QMI_NAS_MMGSDI_EF_SPN || ef == QMI_NAS_MMGSDI_EF_ALL )
  {
    qmi_nasi_global.mmgsdi.cache[qmi_session].ef_spn_valid = state;
    if ( state == QMI_NAS_MMGSDI_ERROR )
    {
      qmi_nasi_global.mmgsdi.cache[qmi_session].ef_spn_size = 0;
      memset( qmi_nasi_global.mmgsdi.cache[qmi_session].ef_spn, 0x00, QMI_NAS_MMGSDI_LEN_EF_SPN );
    }
  }

  if ( ef == QMI_NAS_MMGSDI_EF_OPL || ef == QMI_NAS_MMGSDI_EF_ALL )
  {
    qmi_nasi_global.mmgsdi.cache[qmi_session].ef_opl_valid = state;
    if ( state == QMI_NAS_MMGSDI_ERROR )
    {
      qmi_nasi_global.mmgsdi.cache[qmi_session].ef_opl_num_recs = 0;
      memset( qmi_nasi_global.mmgsdi.cache[qmi_session].ef_opl, 0x00, QMI_NAS_MMGSDI_LEN_EF_OPL );
    }
  }

  if ( ef == QMI_NAS_MMGSDI_EF_PNN || ef == QMI_NAS_MMGSDI_EF_ALL )
  {
    qmi_nasi_global.mmgsdi.cache[qmi_session].ef_pnn_valid = state;
    if ( state == QMI_NAS_MMGSDI_ERROR )
    {
      qmi_nasi_global.mmgsdi.cache[qmi_session].ef_pnn_rec_size = 0;
      qmi_nasi_global.mmgsdi.cache[qmi_session].ef_pnn_num_recs = 0;
      memset( qmi_nasi_global.mmgsdi.cache[qmi_session].ef_pnn, 0x00, QMI_NAS_MMGSDI_LEN_EF_PNN );
    }
  }

  if ( ef == QMI_NAS_MMGSDI_EF_ONS || ef == QMI_NAS_MMGSDI_EF_ALL )
  {
    qmi_nasi_global.mmgsdi.cache[qmi_session].ef_ons_valid = state;
    if ( state == QMI_NAS_MMGSDI_ERROR )
    {
      qmi_nasi_global.mmgsdi.cache[qmi_session].ef_ons_size = 0;
      memset( qmi_nasi_global.mmgsdi.cache[qmi_session].ef_ons, 0x00, QMI_NAS_MMGSDI_LEN_EF_ONS );
    }
  }

  if ( ef == QMI_NAS_MMGSDI_EF_CSP || ef == QMI_NAS_MMGSDI_EF_ALL )
  {
    qmi_nasi_global.mmgsdi.cache[qmi_session].ef_csp_valid = state;
    if ( state == QMI_NAS_MMGSDI_ERROR )
    {
      qmi_nasi_global.mmgsdi.cache[qmi_session].ef_csp_size = 0;
      memset( qmi_nasi_global.mmgsdi.cache[qmi_session].ef_csp, 0x00, QMI_NAS_MMGSDI_LEN_EF_CSP );
    }
  }

  if ( ef == QMI_NAS_MMGSDI_EF_CDMA_SPN || ef == QMI_NAS_MMGSDI_EF_ALL )
  {
    qmi_nasi_global.mmgsdi.cache[qmi_session].ef_cdma_spn_valid = state;
    if ( state == QMI_NAS_MMGSDI_ERROR )
    {
      qmi_nasi_global.mmgsdi.cache[qmi_session].ef_cdma_spn_size = 0;
      memset( qmi_nasi_global.mmgsdi.cache[qmi_session].ef_cdma_spn, 0x00, QMI_NAS_MMGSDI_LEN_EF_CDMA_SPN );
    }
  }

  if ( ef == QMI_NAS_MMGSDI_EF_PLMNWACT || ef == QMI_NAS_MMGSDI_EF_ALL )
  {
    qmi_nasi_global.mmgsdi.cache[qmi_session].ef_plmnwact_valid = state;
    if ( state == QMI_NAS_MMGSDI_ERROR )
    {
      qmi_nasi_global.mmgsdi.cache[qmi_session].ef_plmnwact_size = 0;
    }
  }

  if ( ef == QMI_NAS_MMGSDI_EF_PLMN || ef == QMI_NAS_MMGSDI_EF_ALL )
  {
    qmi_nasi_global.mmgsdi.cache[qmi_session].ef_plmn_valid = state;
    if ( state == QMI_NAS_MMGSDI_ERROR )
    {
      qmi_nasi_global.mmgsdi.cache[qmi_session].ef_plmn_size = 0;
    }
  }
}

/*===========================================================================
  FUNCTION QMI_NAS_MMGSDI_INIT()

  DESCRIPTION
    Initialize struct qmi_nas_mmgsdi_info_s

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
void qmi_nas_mmgsdi_init( struct qmi_nas_mmgsdi_info_s *p_data)
{
  qmi_nas_mmgsdi_session_e_type qmi_session;

  if (!p_data)
    return;

  p_data->client_id    = (mmgsdi_client_id_type)-1;

  for ( qmi_session=QMI_NAS_MMGSDI_GW_PRI_SESSION; qmi_session<QMI_NAS_MMGSDI_SESSION_MAX; qmi_session++ )
  {
    p_data->session_id[qmi_session]      = (mmgsdi_session_id_type)-1;
    p_data->session_type[qmi_session]    = MMGSDI_MAX_SESSION_TYPE_ENUM;
    p_data->app_type[qmi_session]        = MMGSDI_APP_NONE;
    p_data->fcn_refresh[qmi_session]     = FALSE;
    p_data->latest_evt[qmi_session]      = MMGSDI_MAX_EVT_ENUM;
    p_data->operation_ready[qmi_session] = FALSE;
    p_data->session_active[qmi_session]  = FALSE;

    // set it to error and then not_init so all data is set to zero
    p_data->cache[qmi_session].updating = FALSE;
    qmi_nas_mmgsdi_change_cache_state( qmi_session, QMI_NAS_MMGSDI_EF_ALL, QMI_NAS_MMGSDI_ERROR );
  }
}

struct qmi_nas_mmgsdi_cmd_rsp_cb_s
{
  mmgsdi_return_enum_type status;
  mmgsdi_cnf_enum_type    cmd;
  mmgsdi_cnf_type         info;
};

/*===========================================================================
  FUNCTION QMI_NAS_MMGSDI_CMD_RSP_CB()

  DESCRIPTION
    Callback function called by mmgsdi to return command response

  PARAMETERS
    status  : return status
    cmd     : type of command mmgsdi is responding to
    p_data  : response data

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
void qmi_nas_mmgsdi_cmd_rsp_cb
(
  mmgsdi_return_enum_type  status,
  mmgsdi_cnf_enum_type     cmd,
  const mmgsdi_cnf_type   *p_info
)
{
  struct qmi_nas_mmgsdi_cmd_rsp_cb_s *ptr;

  QM_MSG_HIGH_2("response from mmgsdi: status %d cmd %d", status, cmd);

  if ( p_info )
  {
    ptr = modem_mem_alloc( sizeof(*ptr), MODEM_MEM_CLIENT_QMI_MMODE );

    if ( ptr )
    {
      ptr->status = status;
      ptr->cmd    = cmd;
      ptr->info   = *p_info;

      QM_MSG_HIGH_1("response type %d", p_info->response_header.response_type);

      // do deep copies
      switch (p_info->response_header.response_type)
      {
        case MMGSDI_READ_CNF:
          if ( p_info->read_cnf.response_header.mmgsdi_status == MMGSDI_SUCCESS )
          {
            qmi_nas_mmgsdi_deep_copy( (void**)&ptr->info.read_cnf.read_data.data_ptr,
                                      (void*)    p_info->read_cnf.read_data.data_ptr,
                                                 p_info->read_cnf.read_data.data_len );
          }
          else
          {
            ptr->info.read_cnf.read_data.data_ptr = NULL;
          }
          break;

        case MMGSDI_SESSION_GET_OPERATOR_NAME_CNF:
          if ( p_info->session_get_operator_name_cnf.response_header.mmgsdi_status == MMGSDI_SUCCESS )
          {
            uint32 cnt = 0;
            uint32 add_cnt = 0;
            qmi_nas_mmgsdi_deep_copy( (void**)&ptr->info.session_get_operator_name_cnf.plmn_list.plmn_info_ptr,
                                      (void*)    p_info->session_get_operator_name_cnf.plmn_list.plmn_info_ptr,
                                                 p_info->session_get_operator_name_cnf.plmn_list.num_of_plmn_ids * sizeof( mmgsdi_plmn_info_type ));
            if(p_info->session_get_operator_name_cnf.spn.eons_data.data_len > 0)
            {
              qmi_nas_mmgsdi_deep_copy( (void**)&ptr->info.session_get_operator_name_cnf.spn.eons_data.data_ptr,
                                        (void*)                 p_info->session_get_operator_name_cnf.spn.eons_data.data_ptr,
                                        p_info->session_get_operator_name_cnf.spn.eons_data.data_len);
            }
            for(cnt = 0; cnt < p_info->session_get_operator_name_cnf.plmn_list.num_of_plmn_ids; cnt++)
            {
              if(p_info->session_get_operator_name_cnf.plmn_list.plmn_info_ptr[cnt].plmn_long_name.plmn_name.eons_data.data_len > 0)
              {
                qmi_nas_mmgsdi_deep_copy( (void**)&ptr->info.session_get_operator_name_cnf.plmn_list.plmn_info_ptr[cnt].plmn_long_name.plmn_name.eons_data.data_ptr,
                                         (void*)                 p_info->session_get_operator_name_cnf.plmn_list.plmn_info_ptr[cnt].plmn_long_name.plmn_name.eons_data.data_ptr,
                                         p_info->session_get_operator_name_cnf.plmn_list.plmn_info_ptr[cnt].plmn_long_name.plmn_name.eons_data.data_len);
              }
              if(p_info->session_get_operator_name_cnf.plmn_list.plmn_info_ptr[cnt].plmn_short_name.plmn_name.eons_data.data_len > 0)
              {
                qmi_nas_mmgsdi_deep_copy( (void**)&ptr->info.session_get_operator_name_cnf.plmn_list.plmn_info_ptr[cnt].plmn_short_name.plmn_name.eons_data.data_ptr,
                                        (void*)                 p_info->session_get_operator_name_cnf.plmn_list.plmn_info_ptr[cnt].plmn_short_name.plmn_name.eons_data.data_ptr,
                                        p_info->session_get_operator_name_cnf.plmn_list.plmn_info_ptr[cnt].plmn_short_name.plmn_name.eons_data.data_len);
              }
              if(p_info->session_get_operator_name_cnf.plmn_list.plmn_info_ptr[cnt].plmn_additional_info.info_data.data_len > 0)
              {
                qmi_nas_mmgsdi_deep_copy( (void**)&ptr->info.session_get_operator_name_cnf.plmn_list.plmn_info_ptr[cnt].plmn_additional_info.info_data.data_ptr,
                                          (void*)    p_info->session_get_operator_name_cnf.plmn_list.plmn_info_ptr[cnt].plmn_additional_info.info_data.data_ptr,
                                          p_info->session_get_operator_name_cnf.plmn_list.plmn_info_ptr[cnt].plmn_additional_info.info_data.data_len);
              }
              for (add_cnt = 0; add_cnt < p_info->session_get_operator_name_cnf.plmn_list.plmn_info_ptr[cnt].num_plmn_additional_names; add_cnt++ )
              {
                if(p_info->session_get_operator_name_cnf.plmn_list.plmn_info_ptr[cnt].plmn_additional_names[add_cnt].plmn_long_name.plmn_name.eons_data.data_len > 0)
                {
                  qmi_nas_mmgsdi_deep_copy( (void**)&ptr->info.session_get_operator_name_cnf.plmn_list.plmn_info_ptr[cnt].plmn_additional_names[add_cnt].plmn_long_name.plmn_name.eons_data.data_ptr,
                                           (void*)                 p_info->session_get_operator_name_cnf.plmn_list.plmn_info_ptr[cnt].plmn_additional_names[add_cnt].plmn_long_name.plmn_name.eons_data.data_ptr,
                                           p_info->session_get_operator_name_cnf.plmn_list.plmn_info_ptr[cnt].plmn_additional_names[add_cnt].plmn_long_name.plmn_name.eons_data.data_len);
                }
                if(p_info->session_get_operator_name_cnf.plmn_list.plmn_info_ptr[cnt].plmn_additional_names[add_cnt].plmn_short_name.plmn_name.eons_data.data_len > 0)
                {
                  qmi_nas_mmgsdi_deep_copy( (void**)&ptr->info.session_get_operator_name_cnf.plmn_list.plmn_info_ptr[cnt].plmn_additional_names[add_cnt].plmn_short_name.plmn_name.eons_data.data_ptr,
                                          (void*)                 p_info->session_get_operator_name_cnf.plmn_list.plmn_info_ptr[cnt].plmn_additional_names[add_cnt].plmn_short_name.plmn_name.eons_data.data_ptr,
                                          p_info->session_get_operator_name_cnf.plmn_list.plmn_info_ptr[cnt].plmn_additional_names[add_cnt].plmn_short_name.plmn_name.eons_data.data_len);
                }
              }
            }
          }
          else
          {
            ptr->info.session_get_operator_name_cnf.plmn_list.plmn_info_ptr = NULL;
            ptr->info.session_get_operator_name_cnf.spn.eons_data.data_ptr = NULL;
          }
          break;

        case MMGSDI_GET_SE13_PLMN_NAMES_CNF:
          if ( p_info->get_se13_plmn_names_cnf.response_header.mmgsdi_status == MMGSDI_SUCCESS )
          {
            uint32 cnt = 0;
            uint32 add_cnt = 0;
            qmi_nas_mmgsdi_deep_copy( (void**)&ptr->info.get_se13_plmn_names_cnf.plmn_list.plmn_info_ptr,
                                      (void*)    p_info->get_se13_plmn_names_cnf.plmn_list.plmn_info_ptr,
                                                 p_info->get_se13_plmn_names_cnf.plmn_list.num_of_plmn_ids * sizeof( mmgsdi_plmn_info_type ));
            for(cnt = 0; cnt < p_info->get_se13_plmn_names_cnf.plmn_list.num_of_plmn_ids; cnt++)
            {
              if(p_info->get_se13_plmn_names_cnf.plmn_list.plmn_info_ptr[cnt].plmn_long_name.plmn_name.eons_data.data_len > 0)
              {
                qmi_nas_mmgsdi_deep_copy( (void**)&ptr->info.get_se13_plmn_names_cnf.plmn_list.plmn_info_ptr[cnt].plmn_long_name.plmn_name.eons_data.data_ptr,
                                          (void*)    p_info->get_se13_plmn_names_cnf.plmn_list.plmn_info_ptr[cnt].plmn_long_name.plmn_name.eons_data.data_ptr,
                                                     p_info->get_se13_plmn_names_cnf.plmn_list.plmn_info_ptr[cnt].plmn_long_name.plmn_name.eons_data.data_len);
              }
              if(p_info->get_se13_plmn_names_cnf.plmn_list.plmn_info_ptr[cnt].plmn_short_name.plmn_name.eons_data.data_len > 0)
              {
                qmi_nas_mmgsdi_deep_copy( (void**)&ptr->info.get_se13_plmn_names_cnf.plmn_list.plmn_info_ptr[cnt].plmn_short_name.plmn_name.eons_data.data_ptr,
                                          (void*)    p_info->get_se13_plmn_names_cnf.plmn_list.plmn_info_ptr[cnt].plmn_short_name.plmn_name.eons_data.data_ptr,
                                                     p_info->get_se13_plmn_names_cnf.plmn_list.plmn_info_ptr[cnt].plmn_short_name.plmn_name.eons_data.data_len);
              }
              if(p_info->get_se13_plmn_names_cnf.plmn_list.plmn_info_ptr[cnt].plmn_additional_info.info_data.data_len > 0)
              {
                qmi_nas_mmgsdi_deep_copy( (void**)&ptr->info.get_se13_plmn_names_cnf.plmn_list.plmn_info_ptr[cnt].plmn_additional_info.info_data.data_ptr,
                                          (void*)    p_info->get_se13_plmn_names_cnf.plmn_list.plmn_info_ptr[cnt].plmn_additional_info.info_data.data_ptr,
                                          p_info->get_se13_plmn_names_cnf.plmn_list.plmn_info_ptr[cnt].plmn_additional_info.info_data.data_len);
              }
              for ( add_cnt = 0; add_cnt < p_info->get_se13_plmn_names_cnf.plmn_list.plmn_info_ptr[cnt].num_plmn_additional_names; add_cnt++ )
              {
                if(p_info->get_se13_plmn_names_cnf.plmn_list.plmn_info_ptr[cnt].plmn_additional_names[add_cnt].plmn_long_name.plmn_name.eons_data.data_len > 0)
                {
                  qmi_nas_mmgsdi_deep_copy( (void**)&ptr->info.get_se13_plmn_names_cnf.plmn_list.plmn_info_ptr[cnt].plmn_additional_names[add_cnt].plmn_long_name.plmn_name.eons_data.data_ptr,
                                            (void*)    p_info->get_se13_plmn_names_cnf.plmn_list.plmn_info_ptr[cnt].plmn_additional_names[add_cnt].plmn_long_name.plmn_name.eons_data.data_ptr,
                                                       p_info->get_se13_plmn_names_cnf.plmn_list.plmn_info_ptr[cnt].plmn_additional_names[add_cnt].plmn_long_name.plmn_name.eons_data.data_len);
                }
                if(p_info->get_se13_plmn_names_cnf.plmn_list.plmn_info_ptr[cnt].plmn_additional_names[add_cnt].plmn_short_name.plmn_name.eons_data.data_len > 0)
                {
                  qmi_nas_mmgsdi_deep_copy( (void**)&ptr->info.get_se13_plmn_names_cnf.plmn_list.plmn_info_ptr[cnt].plmn_additional_names[add_cnt].plmn_short_name.plmn_name.eons_data.data_ptr,
                                            (void*)    p_info->get_se13_plmn_names_cnf.plmn_list.plmn_info_ptr[cnt].plmn_additional_names[add_cnt].plmn_short_name.plmn_name.eons_data.data_ptr,
                                                       p_info->get_se13_plmn_names_cnf.plmn_list.plmn_info_ptr[cnt].plmn_additional_names[add_cnt].plmn_short_name.plmn_name.eons_data.data_len);
                }
              }
            }
          }
          else
          {
            ptr->info.get_se13_plmn_names_cnf.plmn_list.plmn_info_ptr = NULL;
          }
          break;

        case MMGSDI_SESSION_OPEN_EXT_CNF:
          if ( p_info->session_open_ext_cnf.response_header.mmgsdi_status == MMGSDI_SUCCESS )
          {
            qmi_nas_mmgsdi_deep_copy( (void**)&ptr->info.session_open_ext_cnf.session_info_ptr,
                                      (void*)    p_info->session_open_ext_cnf.session_info_ptr,
                                                 p_info->session_open_ext_cnf.num_sessions * sizeof(mmgsdi_session_open_info_type) );
             
           QM_MSG_HIGH("deep copy for MMGSDI_SESSION_OPEN_EXT_CNF");
          }
          else
          {
            ptr->info.session_open_ext_cnf.session_info_ptr = NULL;
          }
          break;

        default:
          break;
      }

      qmi_mmode_send_cmd( QMI_MMODE_CMD_NAS_MMGSDI_CMD_CB, ptr);
    }
    else
    {
		qmi_voice_mem_error();
    }
  }
  else
  {
    QM_MSG_ERROR("qmi_nas_mmgsdi_cmd_rsp_cb() p_info is null");
  }
}

/*===========================================================================
  FUNCTION QMI_NAS_MMGSDI_DEEP_COPY()

  DESCRIPTION
    This function allocates memory and duplicates the content of a buffer.
    This is a utility function to make deep copies.

  PARAMETERS
    dest : pointer to the destination buffer. Output value.
    src  : pointer to the source buffer
    size : size of the source buffer

  RETURN VALUE
    None. Errors are ignored.

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
void qmi_nas_mmgsdi_deep_copy( void** dest, void* src, uint32 size )
{
  if (dest != NULL)
  {
    *dest = NULL;
    if (src != NULL && size > 0)
    {
      *dest = modem_mem_alloc(size, MODEM_MEM_CLIENT_QMI_MMODE);
      if (*dest != NULL)
      {
        memscpy(*dest, size, src, size);
      }
      else
      {
		  qmi_voice_mem_error();
      }
    }
    else
    {
      QM_MSG_ERROR_2("invalid arg %d %d", src, size);
    }
  }
  else
  {
    QM_MSG_ERROR_1("invalid arg %d", dest);
  }
}

/*===========================================================================
  FUNCTION QMI_NAS_MMGSDI_EVT_CB()

  DESCRIPTION
    Callback function called by mmgsdi to report events

  PARAMETERS
    event  : Info about the event generated

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
void qmi_nas_mmgsdi_evt_cb( const mmgsdi_event_data_type *p_evt )
{
  mmgsdi_event_data_type *ptr;
  int i = 0;

  if (p_evt)
  {
    QM_MSG_HIGH_1("event from mmgsdi %d", p_evt->evt);

    switch ( p_evt->evt )
    {
      case MMGSDI_CARD_INSERTED_EVT:
      case MMGSDI_CARD_ERROR_EVT:
      case MMGSDI_CARD_REMOVED_EVT:
      case MMGSDI_SAP_CONNECT_EVT:
      case MMGSDI_SAP_DISCONNECT_EVT:
      case MMGSDI_SESSION_CLOSE_EVT:
      case MMGSDI_TERMINAL_PROFILE_DL_EVT:
      case MMGSDI_SUBSCRIPTION_READY_EVT:
      case MMGSDI_SESSION_CHANGED_EVT:
      case MMGSDI_PIN1_EVT:
      case MMGSDI_UNIVERSAL_PIN_EVT:
      case MMGSDI_REFRESH_EVT:
      case MMGSDI_FDN_EVT:
      case MMGSDI_PERSO_EVT:
      case MMGSDI_HIDDENKEY_EVT:
      case MMGSDI_OTA_COMMIT_RSP_EVT:
      case MMGSDI_EONS_EVT:
        ptr = modem_mem_alloc( sizeof(*ptr), MODEM_MEM_CLIENT_QMI_MMODE );

        if ( ptr )
        {
          *ptr = *p_evt;

          switch ( p_evt->evt )
          {
            case MMGSDI_REFRESH_EVT:
              qmi_nas_mmgsdi_deep_copy( (void**)&ptr->data.refresh.refresh_files.file_list_ptr,
                                        (void*)p_evt->data.refresh.refresh_files.file_list_ptr,
                                        p_evt->data.refresh.refresh_files.num_files * sizeof(mmgsdi_file_enum_type));

              qmi_nas_mmgsdi_deep_copy( (void**)&ptr->data.refresh.refresh_files.file_path_ptr,
                                        (void*)p_evt->data.refresh.refresh_files.file_path_ptr,
                                        p_evt->data.refresh.refresh_files.file_path_len);
              break;

            case MMGSDI_EONS_EVT:
              qmi_nas_mmgsdi_deep_copy( (void**)&ptr->data.eons.spn.eons_data.data_ptr,
                                        (void*)                 p_evt->data.eons.spn.eons_data.data_ptr,
                                        p_evt->data.eons.spn.eons_data.data_len * sizeof(uint8) );
              if ( p_evt->data.eons.first_stack_info.is_valid )
              {
                qmi_nas_mmgsdi_deep_copy( (void**)&ptr->data.eons.first_stack_info.operator_info.plmn_long_name.plmn_name.eons_data.data_ptr,
                                          (void*)                 p_evt->data.eons.first_stack_info.operator_info.plmn_long_name.plmn_name.eons_data.data_ptr,
                                          p_evt->data.eons.first_stack_info.operator_info.plmn_long_name.plmn_name.eons_data.data_len * sizeof(uint8) );
                qmi_nas_mmgsdi_deep_copy( (void**)&ptr->data.eons.first_stack_info.operator_info.plmn_short_name.plmn_name.eons_data.data_ptr,
                                          (void*)                 p_evt->data.eons.first_stack_info.operator_info.plmn_short_name.plmn_name.eons_data.data_ptr,
                                          p_evt->data.eons.first_stack_info.operator_info.plmn_short_name.plmn_name.eons_data.data_len * sizeof(uint8) );
                if ( p_evt->data.eons.first_stack_info.operator_info.plmn_additional_info.info_data.data_len > 0 )
                {
                  qmi_nas_mmgsdi_deep_copy( (void**)&ptr->data.eons.first_stack_info.operator_info.plmn_additional_info.info_data.data_ptr,
                                            (void*)p_evt->data.eons.first_stack_info.operator_info.plmn_additional_info.info_data.data_ptr,
                                            p_evt->data.eons.first_stack_info.operator_info.plmn_additional_info.info_data.data_len * sizeof(uint8) );
                }
                for ( i = 0; i < p_evt->data.eons.first_stack_info.operator_info.num_plmn_additional_names; i++ )
                {
                  qmi_nas_mmgsdi_deep_copy( (void**)&ptr->data.eons.first_stack_info.operator_info.plmn_additional_names[i].plmn_long_name.plmn_name.eons_data.data_ptr,
                                            (void*)                 p_evt->data.eons.first_stack_info.operator_info.plmn_additional_names[i].plmn_long_name.plmn_name.eons_data.data_ptr,
                                            p_evt->data.eons.first_stack_info.operator_info.plmn_additional_names[i].plmn_long_name.plmn_name.eons_data.data_len * sizeof(uint8) );
                  qmi_nas_mmgsdi_deep_copy( (void**)&ptr->data.eons.first_stack_info.operator_info.plmn_additional_names[i].plmn_short_name.plmn_name.eons_data.data_ptr,
                                            (void*)                 p_evt->data.eons.first_stack_info.operator_info.plmn_additional_names[i].plmn_short_name.plmn_name.eons_data.data_ptr,
                                            p_evt->data.eons.first_stack_info.operator_info.plmn_additional_names[i].plmn_short_name.plmn_name.eons_data.data_len * sizeof(uint8) );
                }
              }
              if ( p_evt->data.eons.sec_stack_info.is_valid)
              {
                qmi_nas_mmgsdi_deep_copy( (void**)&ptr->data.eons.sec_stack_info.operator_info.plmn_long_name.plmn_name.eons_data.data_ptr,
                                          (void*)                 p_evt->data.eons.sec_stack_info.operator_info.plmn_long_name.plmn_name.eons_data.data_ptr,
                                          p_evt->data.eons.sec_stack_info.operator_info.plmn_long_name.plmn_name.eons_data.data_len * sizeof(uint8) );
                qmi_nas_mmgsdi_deep_copy( (void**)&ptr->data.eons.sec_stack_info.operator_info.plmn_short_name.plmn_name.eons_data.data_ptr,
                                          (void*)                 p_evt->data.eons.sec_stack_info.operator_info.plmn_short_name.plmn_name.eons_data.data_ptr,
                                          p_evt->data.eons.sec_stack_info.operator_info.plmn_short_name.plmn_name.eons_data.data_len * sizeof(uint8) );
                if ( p_evt->data.eons.sec_stack_info.operator_info.plmn_additional_info.info_data.data_len > 0 )
                {
                  qmi_nas_mmgsdi_deep_copy( (void**)&ptr->data.eons.sec_stack_info.operator_info.plmn_additional_info.info_data.data_ptr,
                                            (void*)p_evt->data.eons.sec_stack_info.operator_info.plmn_additional_info.info_data.data_ptr,
                                            p_evt->data.eons.sec_stack_info.operator_info.plmn_additional_info.info_data.data_len * sizeof(uint8) );
                }
                for ( i = 0; i < p_evt->data.eons.sec_stack_info.operator_info.num_plmn_additional_names; i++ )
                {
                  qmi_nas_mmgsdi_deep_copy( (void**)&ptr->data.eons.sec_stack_info.operator_info.plmn_additional_names[i].plmn_long_name.plmn_name.eons_data.data_ptr,
                                            (void*)                 p_evt->data.eons.sec_stack_info.operator_info.plmn_additional_names[i].plmn_long_name.plmn_name.eons_data.data_ptr,
                                            p_evt->data.eons.sec_stack_info.operator_info.plmn_additional_names[i].plmn_long_name.plmn_name.eons_data.data_len * sizeof(uint8) );
                  qmi_nas_mmgsdi_deep_copy( (void**)&ptr->data.eons.sec_stack_info.operator_info.plmn_additional_names[i].plmn_short_name.plmn_name.eons_data.data_ptr,
                                            (void*)                 p_evt->data.eons.sec_stack_info.operator_info.plmn_additional_names[i].plmn_short_name.plmn_name.eons_data.data_ptr,
                                            p_evt->data.eons.sec_stack_info.operator_info.plmn_additional_names[i].plmn_short_name.plmn_name.eons_data.data_len * sizeof(uint8) );
                }
              }
              break;

            default:
              break;
          }

          qmi_mmode_send_cmd( QMI_MMODE_CMD_NAS_MMGSDI_EVT_CB, ptr);
        }
        else
        {
			qmi_voice_mem_error();
        }
        break;

      case MMGSDI_SIM_BUSY_EVT:
        QM_MSG_HIGH_2("MMGSDI_SIM_BUSY_EVT recieved status=%d,slot=%d", p_evt->data.sim_busy.sim_busy, p_evt->data.sim_busy.slot);
        qmi_nasi_global.sim_busy_status.sim_busy = p_evt->data.sim_busy.sim_busy;
        qmi_nasi_global.sim_busy_status.slot = p_evt->data.sim_busy.slot;
        if(qmi_nasi_global.sim_busy_status.sim_busy)
        {
          if(mmgsdi_cancel_requests(p_evt->client_id) != MMGSDI_SUCCESS)
          {
            QM_MSG_ERROR("Failed Cancelling all pending requests in MMGSDI Queue()");
          }
        }
        break;

      default:
        QM_MSG_HIGH("qmi_nas_mmgsdi_evt_cb() skipping unused event");
        break;
    }
  }
  else
  {
    QM_MSG_ERROR("qmi_nas_mmgsdi_evt_cb() p_evt is null");
  }
}

/*===========================================================================
  FUNCTION QMI_NAS_MMGSDI_REGISTER_TO_AMSS()

  DESCRIPTION
    Register to MMGSDI

  RETURN VALUE
    TRUE for success, FALSE otherwise

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
boolean qmi_nas_mmgsdi_register_to_amss(void)
{
  mmgsdi_return_enum_type ret_val;

  ret_val = mmgsdi_client_id_and_evt_reg( qmi_nas_mmgsdi_evt_cb,
                                          qmi_nas_mmgsdi_cmd_rsp_cb,
                                          0 );

  if ( ret_val != MMGSDI_SUCCESS )
    QM_MSG_ERROR_1("not able to register mmgsdi client %d", ret_val);

  return (ret_val==MMGSDI_SUCCESS) ? TRUE : FALSE;
}

/*===========================================================================
  FUNCTION qmi_nas_mmgsdi_check_cmd_rsp()

  DESCRIPTION
    MMGSDI's cmd rsp cb param has cmd and status in two different places
    Checks MMGSDI command response handler for integrity and also check
    return status if the message is valid

  PARAMETERS
    p_rsp: pointer to nas structure in command queue item
    p_rsp_check: pointer to where message check results will be saved

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
void qmi_nas_mmgsdi_check_cmd_rsp(
    const struct qmi_nas_mmgsdi_cmd_rsp_cb_s *p_rsp,
    struct valid_success_s *p_rsp_check
)
{
  if (p_rsp && p_rsp_check)
  {
    if ( p_rsp->cmd == p_rsp->info.response_header.response_type )
    {
      p_rsp_check->valid   = TRUE;

      if ( p_rsp->status == p_rsp->info.response_header.mmgsdi_status )
        p_rsp_check->success = (p_rsp->status == MMGSDI_SUCCESS) ? TRUE : FALSE;
      else
        p_rsp_check->success = FALSE;
    }
    else
    {
      p_rsp_check->valid   = FALSE;
      p_rsp_check->success = FALSE;
    }

    QM_MSG_HIGH_2("mmgsdi cmd rsp %d %d", p_rsp_check->valid, p_rsp_check->success);
  }
  else
  {
    QM_MSG_ERROR_2("null pointer %d %d", p_rsp, p_rsp_check);
  }
}


/*===========================================================================
  FUNCTION qmi_nas_mmgsdi_get_mmgsdi_ef_enum()

  DESCRIPTION
    Get MMGSDI enum for a QMI EF based on app_type

  RETURN VALUE
    enums of EF for success. MMGSDI_NO_FILE_ENUM for failure.

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
mmgsdi_file_enum_type qmi_nas_mmgsdi_get_mmgsdi_ef_enum(
    enum qmi_nas_mmgsdi_cache_ef_e id,
    mmgsdi_app_enum_type app
    )
{
  if (app != MMGSDI_APP_SIM && app != MMGSDI_APP_USIM)
    return MMGSDI_NO_FILE_ENUM;

  switch (id)
  {
    case QMI_NAS_MMGSDI_EF_AD:
      return (app == MMGSDI_APP_SIM) ? MMGSDI_GSM_AD : MMGSDI_USIM_AD;

    case QMI_NAS_MMGSDI_EF_IMSI:
      return (app == MMGSDI_APP_SIM) ? MMGSDI_GSM_IMSI : MMGSDI_USIM_IMSI;

    case QMI_NAS_MMGSDI_EF_SPN:
      return (app == MMGSDI_APP_SIM) ? MMGSDI_GSM_SPN : MMGSDI_USIM_SPN;

    case QMI_NAS_MMGSDI_EF_OPL:
      return (app == MMGSDI_APP_SIM) ? MMGSDI_GSM_OPL : MMGSDI_USIM_OPL;

    case QMI_NAS_MMGSDI_EF_PNN:
      return (app == MMGSDI_APP_SIM) ? MMGSDI_GSM_PNN : MMGSDI_USIM_PNN;

    case QMI_NAS_MMGSDI_EF_ONS:
      return (app == MMGSDI_APP_SIM) ? MMGSDI_GSM_ONS : MMGSDI_USIM_ONS;

    case QMI_NAS_MMGSDI_EF_CSP:
      return (app == MMGSDI_APP_SIM) ? MMGSDI_GSM_CSP : MMGSDI_USIM_CSP;

    case QMI_NAS_MMGSDI_EF_PLMNWACT:
      return (app == MMGSDI_APP_SIM) ? MMGSDI_GSM_PLMNWACT : MMGSDI_USIM_PLMNWACT;

    case QMI_NAS_MMGSDI_EF_PLMN:
      return (app == MMGSDI_APP_SIM) ? MMGSDI_GSM_PLMN : MMGSDI_NO_FILE_ENUM;

    default:
      QM_MSG_ERROR_1("invalid qmi ef enum %d", id);
      return MMGSDI_NO_FILE_ENUM;
  }
}

/*===========================================================================
  FUNCTION qmi_nas_mmgsdi_get_qmi_ef_enum()

  DESCRIPTION
    Get QMI enum for a MMGSDI EF

  RETURN VALUE
    enums of EF for success. QMI_NAS_MMGSDI_EF_NONE for failure.

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
enum qmi_nas_mmgsdi_cache_ef_e qmi_nas_mmgsdi_get_qmi_ef_enum(
    mmgsdi_file_enum_type id )
{
  switch (id)
  {
    case MMGSDI_GSM_AD:   case MMGSDI_USIM_AD:   return QMI_NAS_MMGSDI_EF_AD;
    case MMGSDI_GSM_IMSI: case MMGSDI_USIM_IMSI: return QMI_NAS_MMGSDI_EF_IMSI;
    case MMGSDI_GSM_SPN:  case MMGSDI_USIM_SPN:  return QMI_NAS_MMGSDI_EF_SPN;
    case MMGSDI_GSM_OPL:  case MMGSDI_USIM_OPL:  return QMI_NAS_MMGSDI_EF_OPL;
    case MMGSDI_GSM_PNN:  case MMGSDI_USIM_PNN:  return QMI_NAS_MMGSDI_EF_PNN;
    case MMGSDI_GSM_ONS:  case MMGSDI_USIM_ONS:  return QMI_NAS_MMGSDI_EF_ONS;
    case MMGSDI_GSM_CSP:  case MMGSDI_USIM_CSP:  return QMI_NAS_MMGSDI_EF_CSP;
    case MMGSDI_CDMA_HOME_SVC_PVDR_NAME:  case MMGSDI_CSIM_SPN:  return QMI_NAS_MMGSDI_EF_CDMA_SPN;
    case MMGSDI_GSM_PLMNWACT:  case MMGSDI_USIM_PLMNWACT:  case MMGSDI_USIM_UPLMNSEL:  return QMI_NAS_MMGSDI_EF_PLMNWACT;
    case MMGSDI_GSM_PLMN:  return QMI_NAS_MMGSDI_EF_PLMN;

    default:
      QM_MSG_ERROR_1("invalid mmgsdi ef enum %d", id);
      return QMI_NAS_MMGSDI_EF_NONE;
  }
}

/*===========================================================================
  FUNCTION qmi_nas_mmgsdi_read_transparent()

  DESCRIPTION
    Attempt to read transparent EF

  RETURN VALUE
    TRUE for success, FALSE for failure. In case of success, one should listen
    to MMGSDI_READ_CNF. In case of failure, cache is marked as QMI_NAS_MMGSDI_ERROR

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
boolean qmi_nas_mmgsdi_read_transparent (
    qmi_nas_mmgsdi_session_e_type  qmi_session,
    enum qmi_nas_mmgsdi_cache_ef_e id,
    mmgsdi_app_enum_type app_type
    )
{
  mmgsdi_return_enum_type status;
  mmgsdi_access_type      file_access;
  boolean                 success = TRUE;
  mmgsdi_len_type         ef_len = 0;

  switch (id)
  {
    case QMI_NAS_MMGSDI_EF_AD:
    case QMI_NAS_MMGSDI_EF_IMSI:
    case QMI_NAS_MMGSDI_EF_SPN:
    case QMI_NAS_MMGSDI_EF_ONS:
    case QMI_NAS_MMGSDI_EF_CSP:
      ef_len = 0;
      break;

    default:
      QM_MSG_ERROR_1("invalid EF %d", id);
      success = FALSE;
      break;
  }

  if (success)
  {
    file_access.file.file_enum = qmi_nas_mmgsdi_get_mmgsdi_ef_enum( id, app_type );
    file_access.access_method  = MMGSDI_EF_ENUM_ACCESS;

    if ( file_access.file.file_enum != MMGSDI_NO_FILE_ENUM )
    {
      status = mmgsdi_session_read_transparent( qmi_nasi_global.mmgsdi.session_id[qmi_session],
                                                file_access,
                                                0,
                                                ef_len,
                                                qmi_nas_mmgsdi_cmd_rsp_cb,
                                                0 );
      if (status != MMGSDI_SUCCESS)
      {
        QM_MSG_ERROR_2("sim read fails, status = %d %d", status, qmi_session);
        success = FALSE;
      }
    }
    else
    {
      QM_MSG_ERROR("invalid qmi ef enum");
      qmi_nas_mmgsdi_change_cache_state( qmi_session, id, QMI_NAS_MMGSDI_ERROR );
      success = FALSE;
    }
  }

  return success;
}

/*===========================================================================
  FUNCTION qmi_nas_mmgsdi_read_record()

  DESCRIPTION
    Attempt to read Nth record of linear fixed EF

  RETURN VALUE
    TRUE for success, FALSE for failure. In case of success, one should listen
    to MMGSDI_READ_CNF. In case of failure, cache is marked as QMI_NAS_MMGSDI_ERROR

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
boolean qmi_nas_mmgsdi_read_record (
    qmi_nas_mmgsdi_session_e_type  qmi_session,
    enum qmi_nas_mmgsdi_cache_ef_e id,
    mmgsdi_app_enum_type app_type,
    unsigned int rec_num
    )
{
  mmgsdi_return_enum_type status;
  mmgsdi_access_type      file_access;
  boolean                 success = TRUE;
  mmgsdi_len_type         rec_size = 0;

  switch (id)
  {
    case QMI_NAS_MMGSDI_EF_OPL:
      rec_size = QMI_NAS_MMGSDI_REC_SIZE_EF_OPL;
      break;

    case QMI_NAS_MMGSDI_EF_PNN:
      rec_size = qmi_nasi_global.mmgsdi.cache[qmi_session].ef_pnn_rec_size;
      break;

    default:
      QM_MSG_ERROR_1("invalid EF %d", id);
      success = FALSE;
      break;
  }

  if (success)
  {
    file_access.file.file_enum = qmi_nas_mmgsdi_get_mmgsdi_ef_enum( id, app_type );
    file_access.access_method  = MMGSDI_EF_ENUM_ACCESS;

    if ( file_access.file.file_enum != MMGSDI_NO_FILE_ENUM )
    {
      status = mmgsdi_session_read_record( qmi_nasi_global.mmgsdi.session_id[qmi_session],
                                           file_access,
                                           rec_num,
                                           rec_size,
                                           qmi_nas_mmgsdi_cmd_rsp_cb,
                                           0 );
      if (status != MMGSDI_SUCCESS)
      {
        QM_MSG_ERROR_2("sim read fails, status = %d %d", status, qmi_session);
        success = FALSE;
      }
    }
    else
    {
      QM_MSG_ERROR("invalid qmi ef enum");
      qmi_nas_mmgsdi_change_cache_state( qmi_session, id, QMI_NAS_MMGSDI_ERROR );
      success = FALSE;
    }
  }

  return success;
}

/*===========================================================================
  FUNCTION qmi_nas_mmgsdi_get_file_attr()

  DESCRIPTION
    Attempt to read file attribute for an EF

  RETURN VALUE
    TRUE for success, FALSE for failure. In case of success, one should listen
    to MMGSDI_GET_FILE_ATTR_CNF. In case of failure, cache is marked as QMI_NAS_MMGSDI_ERROR

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
boolean qmi_nas_mmgsdi_get_file_attr (
    qmi_nas_mmgsdi_session_e_type qmi_session,
    enum qmi_nas_mmgsdi_cache_ef_e id,
    mmgsdi_app_enum_type app_type
    )
{
  mmgsdi_return_enum_type status;
  mmgsdi_access_type      file_access;
  boolean                 success = TRUE;

  file_access.file.file_enum = qmi_nas_mmgsdi_get_mmgsdi_ef_enum( id, app_type );
  file_access.access_method  = MMGSDI_EF_ENUM_ACCESS;

  if ( file_access.file.file_enum != MMGSDI_NO_FILE_ENUM )
  {
    status = mmgsdi_session_get_file_attr_ext( qmi_nasi_global.mmgsdi.session_id[qmi_session],
                                               file_access,
                                               qmi_nas_mmgsdi_cmd_rsp_cb,
                                               0,
                                               TRUE );
    if (status != MMGSDI_SUCCESS)
    {
      QM_MSG_ERROR_2("sim get file attr fails, status = %d %d", status, qmi_session);
      qmi_nas_mmgsdi_change_cache_state( qmi_session, id, QMI_NAS_MMGSDI_ERROR );
      success = FALSE;
    }
  }
  else
  {
    QM_MSG_ERROR_1("invalid qmi ef enum %d", qmi_session);
    qmi_nas_mmgsdi_change_cache_state( qmi_session, id, QMI_NAS_MMGSDI_ERROR );
    success = FALSE;
  }

  return success;
}

/*===========================================================================
  FUNCTION qmi_nas_mmgsdi_get_next_uninitialized_ef()

  DESCRIPTION
    Find next EF that's not been cached

  RETURN VALUE
    EF enum of the file. If there's none, QMI_NAS_MMGSDI_EF_NONE

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
enum qmi_nas_mmgsdi_cache_ef_e qmi_nas_mmgsdi_get_next_uninitialized_ef ( qmi_nas_mmgsdi_session_e_type qmi_session )
{
  if ( qmi_nasi_global.mmgsdi.cache[qmi_session].ef_ad_valid == QMI_NAS_MMGSDI_NOT_INIT )
    return QMI_NAS_MMGSDI_EF_AD;

  if ( qmi_nasi_global.mmgsdi.cache[qmi_session].ef_imsi_valid == QMI_NAS_MMGSDI_NOT_INIT )
    return QMI_NAS_MMGSDI_EF_IMSI;

  if ( qmi_nasi_global.mmgsdi.cache[qmi_session].ef_plmn_valid == QMI_NAS_MMGSDI_NOT_INIT )
    return QMI_NAS_MMGSDI_EF_PLMN;

  if ( qmi_nasi_global.mmgsdi.cache[qmi_session].ef_plmnwact_valid == QMI_NAS_MMGSDI_NOT_INIT )
    return QMI_NAS_MMGSDI_EF_PLMNWACT;

  if ( qmi_nasi_global.mmgsdi.cache[qmi_session].ef_spn_valid == QMI_NAS_MMGSDI_NOT_INIT )
    return QMI_NAS_MMGSDI_EF_SPN;

  if ( qmi_nasi_global.mmgsdi.cache[qmi_session].ef_opl_valid == QMI_NAS_MMGSDI_NOT_INIT )
    return QMI_NAS_MMGSDI_EF_OPL;

  if ( qmi_nasi_global.mmgsdi.cache[qmi_session].ef_pnn_valid == QMI_NAS_MMGSDI_NOT_INIT )
    return QMI_NAS_MMGSDI_EF_PNN;

  if ( qmi_nasi_global.mmgsdi.cache[qmi_session].ef_ons_valid == QMI_NAS_MMGSDI_NOT_INIT )
    return QMI_NAS_MMGSDI_EF_ONS;

  if ( qmi_nasi_global.mmgsdi.cache[qmi_session].ef_csp_valid == QMI_NAS_MMGSDI_NOT_INIT )
    return QMI_NAS_MMGSDI_EF_CSP;

  QM_MSG_HIGH("no file left to read");
  return QMI_NAS_MMGSDI_EF_NONE;
}

/*===========================================================================
  FUNCTION qmi_nas_mmgsdi_ef_cache_completed()

  DESCRIPTION
    Process procedures after all EFs get updated

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
void qmi_nas_mmgsdi_ef_cache_completed( qmi_nas_mmgsdi_session_e_type qmi_session )
{
  mmgsdi_session_type_enum_type session = qmi_nas_map_mmgsdi_session(qmi_session);
  sys_modem_as_id_e_type asubs_id = SYS_MODEM_AS_ID_NONE;
  
  QM_MSG_HIGH_6("udpates spn %d opl %d pnn %d ons %d csp %d csp_req_buffer_valid %d", qmi_nasi_global.mmgsdi.cache[qmi_session].ef_spn_changed, qmi_nasi_global.mmgsdi.cache[qmi_session].ef_opl_changed, 
       qmi_nasi_global.mmgsdi.cache[qmi_session].ef_pnn_changed, qmi_nasi_global.mmgsdi.cache[qmi_session].ef_ons_changed, qmi_nasi_global.mmgsdi.cache[qmi_session].ef_csp_changed,
       qmi_nasi_global.mmgsdi.csp_req_buffer_valid);

  if ( qmi_nasi_global.mmgsdi.cache[qmi_session].ef_spn_changed ||
       qmi_nasi_global.mmgsdi.cache[qmi_session].ef_opl_changed ||
       qmi_nasi_global.mmgsdi.cache[qmi_session].ef_pnn_changed ||
       qmi_nasi_global.mmgsdi.cache[qmi_session].ef_ons_changed )
  {
    qmi_nas_operator_name_data_ind( session, NULL );
    asubs_id = qmi_nas_map_mmgsdi_session_to_cm_asubs( session );
    if(asubs_id > SYS_MODEM_AS_ID_NONE)
#ifdef  FEATURE_DUAL_SIM
      qmi_nas_gen_serving_system_ind( &qmi_nasi_global.cm_msim_ss_info[asubs_id], &qmi_nasi_global.cm_ph_info, &qmi_nasi_global.cm_ph_info2 );
#else
      qmi_nas_gen_serving_system_ind( &qmi_nasi_global.cm_msim_ss_info[asubs_id], &qmi_nasi_global.cm_ph_info, NULL );
#endif

    qmi_nasi_global.mmgsdi.cache[qmi_session].ef_spn_changed = FALSE;
    qmi_nasi_global.mmgsdi.cache[qmi_session].ef_opl_changed = FALSE;
    qmi_nasi_global.mmgsdi.cache[qmi_session].ef_pnn_changed = FALSE;
    qmi_nasi_global.mmgsdi.cache[qmi_session].ef_ons_changed = FALSE;
  }

  if ( qmi_nasi_global.mmgsdi.cache[qmi_session].ef_csp_changed )
  {
    qmi_nas_csp_plmn_mode_bit_ind( session );

    qmi_nasi_global.mmgsdi.cache[qmi_session].ef_csp_changed = FALSE;
  }

  if(qmi_nasi_global.mmgsdi.csp_req_buffer_valid)
  {
    qmi_nasi_generate_get_csp_plmn_mode_bit_resp();
    qmi_nasi_global.mmgsdi.csp_req_buffer_valid = FALSE;
  }
}

/*===========================================================================
  FUNCTION qmi_nas_mmgsdi_trigger_next_ef_read()

  DESCRIPTION
    Triggers to read next EF. If nothing to read, trigger ind handlers

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
void qmi_nas_mmgsdi_trigger_next_ef_read( qmi_nas_mmgsdi_session_e_type qmi_session )
{
  enum qmi_nas_mmgsdi_cache_ef_e ef;
  boolean                        success = FALSE;

  if( qmi_nas_mmgsdi_is_valid_qmi_session( qmi_session ) )
  {
    while (!success)
    {
     ef = qmi_nas_mmgsdi_get_next_uninitialized_ef ( qmi_session );

     if ( ef == QMI_NAS_MMGSDI_EF_OPL || ef == QMI_NAS_MMGSDI_EF_PNN || ef == QMI_NAS_MMGSDI_EF_PLMN ||
          ef == QMI_NAS_MMGSDI_EF_PLMNWACT )
     {
       QM_MSG_HIGH_3("reading next file attribute %d %d %d", ef, qmi_nasi_global.mmgsdi.app_type[qmi_session], qmi_session);
       success = qmi_nas_mmgsdi_get_file_attr ( qmi_session, ef, qmi_nasi_global.mmgsdi.app_type[qmi_session] );
     }
     else if ( ef == QMI_NAS_MMGSDI_EF_AD || ef == QMI_NAS_MMGSDI_EF_IMSI || ef == QMI_NAS_MMGSDI_EF_SPN ||
               ef == QMI_NAS_MMGSDI_EF_ONS || ef == QMI_NAS_MMGSDI_EF_CSP )
     {
       QM_MSG_HIGH_3("reading next ef file %d, %d, %d", ef, qmi_nasi_global.mmgsdi.app_type[qmi_session], qmi_session);
       success = qmi_nas_mmgsdi_read_transparent( qmi_session , ef , qmi_nasi_global.mmgsdi.app_type[qmi_session] );
     }
     else
     {
       if ( qmi_nasi_global.mmgsdi.fcn_refresh[qmi_session] == TRUE )
       {
         mmgsdi_return_enum_type status;

         qmi_nasi_global.mmgsdi.fcn_refresh[qmi_session] = FALSE;

         status = mmgsdi_session_refresh_complete( qmi_nasi_global.mmgsdi.session_id[qmi_session],
                                                   TRUE,
                                                   qmi_nas_mmgsdi_cmd_rsp_cb,
                                                   0 );
         if (status != MMGSDI_SUCCESS)
         {
           QM_MSG_ERROR_2("refresh complete failed %d %d", status, qmi_session);
         }
       }

       qmi_nasi_global.mmgsdi.cache[qmi_session].updating = FALSE;

       qmi_nas_mmgsdi_ef_cache_completed ( qmi_session );
       success = TRUE;
     }
    }
  }
  else
  {
    QM_MSG_ERROR_1("session id out of range %d", qmi_session);
  }
}

/*===========================================================================
  FUNCTION qmi_nas_mmgsdi_register_refresh()

  DESCRIPTION
    Register files for FCN

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
boolean qmi_nas_mmgsdi_register_refresh( qmi_nas_mmgsdi_session_e_type qmi_session )
{
  mmgsdi_refresh_file_list_type refresh_files;
  mmgsdi_return_enum_type       status;
  mmgsdi_session_type_enum_type session = qmi_nas_map_mmgsdi_session( qmi_session );

  if ( qmi_nas_mmgsdi_is_valid_3gpp_session(session) )
  {
    refresh_files.num_files     = QMI_NAS_ARR_SIZE(refresh_file_list);
    refresh_files.file_list_ptr = (mmgsdi_file_enum_type*)refresh_file_list;
    refresh_files.file_path_ptr = NULL;
    refresh_files.file_path_len = 0;

    /* Register for the refresh event */
    status = mmgsdi_session_register_for_refresh( qmi_nasi_global.mmgsdi.session_id[qmi_session],
                                                  refresh_files,
                                                  FALSE,
                                                  qmi_nas_mmgsdi_cmd_rsp_cb,
                                                  0 );

    if (status !=  MMGSDI_SUCCESS)
    {
      QM_MSG_ERROR_2("refresh registration failed %d %d", status, qmi_session);
      return FALSE;
    }
  }

  return TRUE;
}

/*===========================================================================
  FUNCTION qmi_nas_mmgsdi_get_session_type()

  DESCRIPTION
    get session type from session id

  PARAMETERS
    session id

  RETURN VALUE
    session type

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
mmgsdi_session_type_enum_type qmi_nas_mmgsdi_get_session_type( mmgsdi_session_id_type session_id )
{
  qmi_nas_mmgsdi_session_e_type qmi_session;

  for ( qmi_session=QMI_NAS_MMGSDI_GW_PRI_SESSION; qmi_session<QMI_NAS_MMGSDI_SESSION_MAX; qmi_session++ )
  {
    if ( qmi_nasi_global.mmgsdi.session_id[qmi_session] == session_id )
    {
      return qmi_nasi_global.mmgsdi.session_type[qmi_session];
    }
  }

  return MMGSDI_MAX_SESSION_TYPE_ENUM;
}

/*===========================================================================
  FUNCTION qmi_nas_mmgsdi_process_cached_ef_read_cnf()

  DESCRIPTION
    handler for READ_CNF for cached EFs

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
void qmi_nas_mmgsdi_process_cached_ef_read_cnf( const struct qmi_nas_mmgsdi_cmd_rsp_cb_s *p_rsp )
{
  struct valid_success_s         rsp_check;
  boolean                        success = FALSE;
  int                            is_diff, idx;
  enum qmi_nas_mmgsdi_cache_ef_e ef;
  mmgsdi_session_type_enum_type  session;
  qmi_nas_mmgsdi_session_e_type  qmi_session;

  qmi_nas_mmgsdi_check_cmd_rsp( p_rsp, &rsp_check );

  session = qmi_nas_mmgsdi_get_session_type( p_rsp->info.read_cnf.response_header.session_id );

  if( qmi_nas_mmgsdi_is_valid_session(session))
  {
     qmi_session = qmi_nas_map_mmgsdi_session_to_qmi(session);
     if ( rsp_check.success && qmi_nasi_global.mmgsdi.session_active[qmi_session] )
     {
       QM_MSG_HIGH_1("Size read %d", p_rsp->info.read_cnf.read_data.data_len);

       switch (p_rsp->info.read_cnf.access.file.file_enum)
       {
         case MMGSDI_GSM_AD:
         case MMGSDI_USIM_AD:
           qmi_nasi_global.mmgsdi.cache[qmi_session].ef_ad_size = p_rsp->info.read_cnf.read_data.data_len;
           memset( qmi_nasi_global.mmgsdi.cache[qmi_session].ef_ad, 0x00, QMI_NAS_MMGSDI_LEN_EF_AD );

           memscpy( qmi_nasi_global.mmgsdi.cache[qmi_session].ef_ad,
                    MIN(QMI_NAS_MMGSDI_LEN_EF_AD,qmi_nasi_global.mmgsdi.cache[qmi_session].ef_ad_size),
                   p_rsp->info.read_cnf.read_data.data_ptr,
                   MIN(QMI_NAS_MMGSDI_LEN_EF_AD,qmi_nasi_global.mmgsdi.cache[qmi_session].ef_ad_size) );
           qmi_nasi_global.mmgsdi.cache[qmi_session].ef_ad_valid = QMI_NAS_MMGSDI_VALID;

           success = TRUE;
           qmi_nas_mmgsdi_trigger_next_ef_read( qmi_session );
           break;

         case MMGSDI_GSM_IMSI:
         case MMGSDI_USIM_IMSI:
             qmi_nasi_global.mmgsdi.cache[qmi_session].ef_imsi_size = MIN(p_rsp->info.read_cnf.read_data.data_len, QMI_NAS_MMGSDI_LEN_EF_IMSI);
             memset( qmi_nasi_global.mmgsdi.cache[qmi_session].ef_imsi, 0x00, QMI_NAS_MMGSDI_LEN_EF_IMSI );

             memscpy(qmi_nasi_global.mmgsdi.cache[qmi_session].ef_imsi, 
                     qmi_nasi_global.mmgsdi.cache[qmi_session].ef_imsi_size,
                     p_rsp->info.read_cnf.read_data.data_ptr,
                     qmi_nasi_global.mmgsdi.cache[qmi_session].ef_imsi_size);
             qmi_nasi_global.mmgsdi.cache[qmi_session].ef_imsi_valid = QMI_NAS_MMGSDI_VALID;
  
             success = TRUE;
             qmi_nas_mmgsdi_trigger_next_ef_read( qmi_session );
           break;

         case MMGSDI_GSM_SPN:
         case MMGSDI_USIM_SPN:
           QM_MSG_HIGH_3("new ef spn size: %d old cached size: %d session: %d", p_rsp->info.read_cnf.read_data.data_len, qmi_nasi_global.mmgsdi.cache[qmi_session].ef_spn_size, qmi_session);
           if ( qmi_nasi_global.mmgsdi.cache[qmi_session].ef_spn_size != MIN(p_rsp->info.read_cnf.read_data.data_len, QMI_NAS_MMGSDI_LEN_EF_SPN) )
           {
             qmi_nasi_global.mmgsdi.cache[qmi_session].ef_spn_changed = TRUE;
             qmi_nasi_global.mmgsdi.cache[qmi_session].ef_spn_size = MIN(p_rsp->info.read_cnf.read_data.data_len, QMI_NAS_MMGSDI_LEN_EF_SPN);
             memset( qmi_nasi_global.mmgsdi.cache[qmi_session].ef_spn, 0x00, QMI_NAS_MMGSDI_LEN_EF_SPN );
           }
           else
           {
             is_diff = memcmp( qmi_nasi_global.mmgsdi.cache[qmi_session].ef_spn,
                               p_rsp->info.read_cnf.read_data.data_ptr,
                               MIN(qmi_nasi_global.mmgsdi.cache[qmi_session].ef_spn_size, QMI_NAS_MMGSDI_LEN_EF_SPN) );
  
             if ( is_diff )
               qmi_nasi_global.mmgsdi.cache[qmi_session].ef_spn_changed = TRUE;
           }
  
           if ( qmi_nasi_global.mmgsdi.cache[qmi_session].ef_spn_changed )
           {
             memscpy(qmi_nasi_global.mmgsdi.cache[qmi_session].ef_spn,
                     MIN(qmi_nasi_global.mmgsdi.cache[qmi_session].ef_spn_size, QMI_NAS_MMGSDI_LEN_EF_SPN),
                     p_rsp->info.read_cnf.read_data.data_ptr,
                     MIN(qmi_nasi_global.mmgsdi.cache[qmi_session].ef_spn_size, QMI_NAS_MMGSDI_LEN_EF_SPN));
           }
           qmi_nasi_global.mmgsdi.cache[qmi_session].ef_spn_valid = QMI_NAS_MMGSDI_VALID;
  
           success = TRUE;
           qmi_nas_mmgsdi_trigger_next_ef_read( qmi_session );
           break;

         case MMGSDI_GSM_OPL:
         case MMGSDI_USIM_OPL:
           if ( !qmi_nasi_global.mmgsdi.cache[qmi_session].ef_opl_changed )
           {
             is_diff = memcmp( &qmi_nasi_global.mmgsdi.cache[qmi_session].ef_opl[qmi_nasi_global.mmgsdi.cache[qmi_session].ef_opl_bytes_read],
                               p_rsp->info.read_cnf.read_data.data_ptr,
                               QMI_NAS_MMGSDI_REC_SIZE_EF_OPL );

             if ( is_diff )
               qmi_nasi_global.mmgsdi.cache[qmi_session].ef_opl_changed = TRUE;
           }

           if ( qmi_nasi_global.mmgsdi.cache[qmi_session].ef_opl_changed )
           {
             memscpy(&qmi_nasi_global.mmgsdi.cache[qmi_session].ef_opl[qmi_nasi_global.mmgsdi.cache[qmi_session].ef_opl_bytes_read],
                     QMI_NAS_MMGSDI_REC_SIZE_EF_OPL,
                     p_rsp->info.read_cnf.read_data.data_ptr,
                     QMI_NAS_MMGSDI_REC_SIZE_EF_OPL );
           }

           qmi_nasi_global.mmgsdi.cache[qmi_session].ef_opl_bytes_read += QMI_NAS_MMGSDI_REC_SIZE_EF_OPL;

           if ( qmi_nasi_global.mmgsdi.cache[qmi_session].ef_opl_num_recs == p_rsp->info.read_cnf.accessed_rec_num || // final rec
                qmi_nasi_global.mmgsdi.cache[qmi_session].ef_opl_bytes_read + QMI_NAS_MMGSDI_REC_SIZE_EF_OPL > QMI_NAS_MMGSDI_LEN_EF_OPL ) // reading next rec will go over cache size. stop reading more
           {
             success = TRUE;
             qmi_nasi_global.mmgsdi.cache[qmi_session].ef_opl_valid = QMI_NAS_MMGSDI_VALID;
             qmi_nas_mmgsdi_trigger_next_ef_read( qmi_session );
           }
           else
           {
             ef = qmi_nas_mmgsdi_get_qmi_ef_enum ( p_rsp->info.read_cnf.access.file.file_enum );
             success = qmi_nas_mmgsdi_read_record ( qmi_session, ef, qmi_nasi_global.mmgsdi.app_type[qmi_session], p_rsp->info.read_cnf.accessed_rec_num+1 );
           }
           break;

         case MMGSDI_GSM_PNN:
         case MMGSDI_USIM_PNN:
           if ( !qmi_nasi_global.mmgsdi.cache[qmi_session].ef_pnn_changed )
           {
             is_diff = memcmp( &qmi_nasi_global.mmgsdi.cache[qmi_session].ef_pnn[qmi_nasi_global.mmgsdi.cache[qmi_session].ef_pnn_bytes_read],
                               p_rsp->info.read_cnf.read_data.data_ptr,
                               qmi_nasi_global.mmgsdi.cache[qmi_session].ef_pnn_rec_size );

             if ( is_diff )
               qmi_nasi_global.mmgsdi.cache[qmi_session].ef_pnn_changed = TRUE;
           }

           if ( qmi_nasi_global.mmgsdi.cache[qmi_session].ef_pnn_changed )
           {
             memscpy(&qmi_nasi_global.mmgsdi.cache[qmi_session].ef_pnn[qmi_nasi_global.mmgsdi.cache[qmi_session].ef_pnn_bytes_read],
                     qmi_nasi_global.mmgsdi.cache[qmi_session].ef_pnn_rec_size,
                     p_rsp->info.read_cnf.read_data.data_ptr,
                     qmi_nasi_global.mmgsdi.cache[qmi_session].ef_pnn_rec_size);
           }

           qmi_nasi_global.mmgsdi.cache[qmi_session].ef_pnn_bytes_read += qmi_nasi_global.mmgsdi.cache[qmi_session].ef_pnn_rec_size;

           if ( qmi_nasi_global.mmgsdi.cache[qmi_session].ef_pnn_num_recs == p_rsp->info.read_cnf.accessed_rec_num || // final rec
                qmi_nasi_global.mmgsdi.cache[qmi_session].ef_pnn_bytes_read + qmi_nasi_global.mmgsdi.cache[qmi_session].ef_pnn_rec_size > QMI_NAS_MMGSDI_LEN_EF_PNN ) // reading next rec will go over cache size. stop reading more
           {
             success = TRUE;
             qmi_nasi_global.mmgsdi.cache[qmi_session].ef_pnn_valid = QMI_NAS_MMGSDI_VALID;
             qmi_nas_mmgsdi_trigger_next_ef_read( qmi_session );
           }
           else
           {
             ef = qmi_nas_mmgsdi_get_qmi_ef_enum ( p_rsp->info.read_cnf.access.file.file_enum );
             success = qmi_nas_mmgsdi_read_record ( qmi_session, ef, qmi_nasi_global.mmgsdi.app_type[qmi_session], p_rsp->info.read_cnf.accessed_rec_num+1 );
           }
           break;

         case MMGSDI_GSM_ONS:
         case MMGSDI_USIM_ONS:
           QM_MSG_HIGH_3("new ef ons size: %d old cached size: %d session: %d", p_rsp->info.read_cnf.read_data.data_len, qmi_nasi_global.mmgsdi.cache[qmi_session].ef_ons_size, qmi_session);	   
           if ( qmi_nasi_global.mmgsdi.cache[qmi_session].ef_ons_size != MIN(p_rsp->info.read_cnf.read_data.data_len, QMI_NAS_MMGSDI_LEN_EF_ONS) )
             {
               qmi_nasi_global.mmgsdi.cache[qmi_session].ef_ons_changed = TRUE;
             qmi_nasi_global.mmgsdi.cache[qmi_session].ef_ons_size = MIN(p_rsp->info.read_cnf.read_data.data_len, QMI_NAS_MMGSDI_LEN_EF_ONS);
               memset( qmi_nasi_global.mmgsdi.cache[qmi_session].ef_ons, 0x00, QMI_NAS_MMGSDI_LEN_EF_ONS );
             }
             else
             {
               is_diff = memcmp( qmi_nasi_global.mmgsdi.cache[qmi_session].ef_ons,
                                 p_rsp->info.read_cnf.read_data.data_ptr,
                                 MIN(qmi_nasi_global.mmgsdi.cache[qmi_session].ef_ons_size, QMI_NAS_MMGSDI_LEN_EF_ONS) );
  
               if ( is_diff )
                 qmi_nasi_global.mmgsdi.cache[qmi_session].ef_ons_changed = TRUE;
             }
  
             if ( qmi_nasi_global.mmgsdi.cache[qmi_session].ef_ons_changed )
             {
               memscpy(qmi_nasi_global.mmgsdi.cache[qmi_session].ef_ons,
	               MIN(qmi_nasi_global.mmgsdi.cache[qmi_session].ef_ons_size, QMI_NAS_MMGSDI_LEN_EF_ONS),
                       p_rsp->info.read_cnf.read_data.data_ptr,
                       MIN(qmi_nasi_global.mmgsdi.cache[qmi_session].ef_ons_size, QMI_NAS_MMGSDI_LEN_EF_ONS));
             }
             qmi_nasi_global.mmgsdi.cache[qmi_session].ef_ons_valid = QMI_NAS_MMGSDI_VALID;
  
             success = TRUE;
             qmi_nas_mmgsdi_trigger_next_ef_read( qmi_session );
           break;

         case MMGSDI_GSM_CSP:
         case MMGSDI_USIM_CSP:
           qmi_nasi_global.mmgsdi.cache[qmi_session].ef_csp_size = 0;
           qmi_nasi_global.mmgsdi.cache[qmi_session].ef_csp_changed = FALSE;
           is_diff = 0;

           for( idx=0; idx<p_rsp->info.read_cnf.read_data.data_len; idx+=2 )
           {
             if( p_rsp->info.read_cnf.read_data.data_ptr[idx] == QMI_NAS_VALUE_ADDED_SERVICE_CODE )
             {
               is_diff = memcmp( qmi_nasi_global.mmgsdi.cache[qmi_session].ef_csp,
                                 &p_rsp->info.read_cnf.read_data.data_ptr[idx],
                                 QMI_NAS_MMGSDI_LEN_EF_CSP );
               qmi_nasi_global.mmgsdi.cache[qmi_session].ef_csp_size = QMI_NAS_MMGSDI_LEN_EF_CSP;

               if ( is_diff )
               {
                 qmi_nasi_global.mmgsdi.cache[qmi_session].ef_csp_changed = TRUE;
                 QM_MSG_HIGH("Found PLMN Mode information");
                 memscpy(qmi_nasi_global.mmgsdi.cache[qmi_session].ef_csp,
                         QMI_NAS_MMGSDI_LEN_EF_CSP,
                         &p_rsp->info.read_cnf.read_data.data_ptr[idx],
                         QMI_NAS_MMGSDI_LEN_EF_CSP );
               }
               break;
             }
           }

           qmi_nasi_global.mmgsdi.cache[qmi_session].ef_csp_valid = QMI_NAS_MMGSDI_VALID;

           if( qmi_nasi_global.mmgsdi.cache[qmi_session].ef_csp_size == 0 )
           {
             memset( qmi_nasi_global.mmgsdi.cache[qmi_session].ef_csp, 0x00, QMI_NAS_MMGSDI_LEN_EF_CSP );
           }

           success = TRUE;
           qmi_nas_mmgsdi_trigger_next_ef_read( qmi_session );
           break;

         case MMGSDI_CDMA_HOME_SVC_PVDR_NAME:
         case MMGSDI_CSIM_SPN:
           if ( p_rsp->info.read_cnf.read_data.data_len == QMI_NAS_MMGSDI_LEN_EF_CDMA_SPN )
           {
             int i = CDMA_SPN_NAME_TEXT_BEGIN_IDX;

             for ( ; i<QMI_NAS_MMGSDI_LEN_EF_CDMA_SPN; i++ )
               if ( *(p_rsp->info.read_cnf.read_data.data_ptr+i) == 0xFF )
                 break;

             qmi_nasi_global.mmgsdi.cache[qmi_session].ef_cdma_spn_size = i;
             memscpy(qmi_nasi_global.mmgsdi.cache[qmi_session].ef_cdma_spn, i,
                     p_rsp->info.read_cnf.read_data.data_ptr, i); 
             qmi_nasi_global.mmgsdi.cache[qmi_session].ef_cdma_spn_valid = QMI_NAS_MMGSDI_VALID;
           }
           else
           {
             QM_MSG_ERROR_1("invalid cdma spn len %d", p_rsp->info.read_cnf.read_data.data_len);
             qmi_nas_mmgsdi_change_cache_state( qmi_session, QMI_NAS_MMGSDI_EF_CDMA_SPN, QMI_NAS_MMGSDI_ERROR );
           }
           success = TRUE; // regardless of success or failure, cdma session only needs this EF for now
           break;

         default:
           QM_MSG_HIGH_2("unexpected EF read %d %d", p_rsp->info.read_cnf.access.file.file_enum, qmi_session);
           success = FALSE;
           break;
       }
   }
   else
   {
     ef = qmi_nas_mmgsdi_get_qmi_ef_enum( p_rsp->info.read_cnf.access.file.file_enum );
     qmi_nas_mmgsdi_change_cache_state( qmi_session, ef, QMI_NAS_MMGSDI_ERROR );
   }

   if (!success)
     qmi_nas_mmgsdi_trigger_next_ef_read( qmi_session );
  }
  else
  {
    QM_MSG_ERROR_1("session id out of range %d", session);
  }
}

/*===========================================================================
  FUNCTION qmi_nas_mmgsdi_process_regular_ef_read_cnf()

  DESCRIPTION
    handler for READ_CNF for non-cached EFs

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
void qmi_nas_mmgsdi_process_regular_ef_read_cnf( const struct qmi_nas_mmgsdi_cmd_rsp_cb_s *p_rsp )
{
  switch (p_rsp->info.read_cnf.access.file.file_enum)
  {
    case MMGSDI_GSM_PLMN:
    case MMGSDI_GSM_PLMNWACT:
    case MMGSDI_GSM_OPLMNWACT:
    case MMGSDI_USIM_PLMNWACT:
    case MMGSDI_USIM_OPLMNWACT:
    case MMGSDI_GSM_FPLMN:
    case MMGSDI_USIM_FPLMN:
      {
        mmgsdi_cnf_type *cnf_data =  modem_mem_alloc( sizeof(p_rsp->info), MODEM_MEM_CLIENT_QMI_MMODE );

        if(cnf_data != NULL)
        {
          memscpy( cnf_data, sizeof(p_rsp->info), &p_rsp->info,  sizeof(p_rsp->info));
          qmi_nas_process_mmgsdi_operation_complete( MMGSDI_READ_CNF, cnf_data );
          modem_mem_free ( cnf_data, MODEM_MEM_CLIENT_QMI_MMODE );
        }
        else
        {
			qmi_voice_mem_error();
        }
      }
      break;

    default:
      QM_MSG_HIGH_1("unexpected EF read %d", p_rsp->info.read_cnf.access.file.file_enum);
      break;
  }
}

/*===========================================================================
  FUNCTION qmi_nas_mmgsdi_cached_ef()

  DESCRIPTION
    check to see if the EF is cached EF or not

  RETURN VALUE
    TRUE if cached EF, FALSE if not

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
boolean qmi_nas_mmgsdi_cached_ef( mmgsdi_file_enum_type file_enum )
{
  boolean ret_val;

  switch ( file_enum )
  {
    case MMGSDI_GSM_AD:
    case MMGSDI_USIM_AD:
    case MMGSDI_GSM_IMSI:
    case MMGSDI_USIM_IMSI:
    case MMGSDI_GSM_SPN:
    case MMGSDI_USIM_SPN:
    case MMGSDI_GSM_OPL:
    case MMGSDI_USIM_OPL:
    case MMGSDI_GSM_PNN:
    case MMGSDI_USIM_PNN:
    case MMGSDI_GSM_ONS:
    case MMGSDI_USIM_ONS:
    case MMGSDI_GSM_CSP:
    case MMGSDI_USIM_CSP:
    case MMGSDI_CDMA_HOME_SVC_PVDR_NAME:
    case MMGSDI_CSIM_SPN:
      ret_val = TRUE;
      break;

    case MMGSDI_GSM_PLMN:
    case MMGSDI_GSM_PLMNWACT:
    case MMGSDI_GSM_OPLMNWACT:
    case MMGSDI_USIM_PLMNWACT:
    case MMGSDI_USIM_OPLMNWACT:
    case MMGSDI_GSM_FPLMN:
    case MMGSDI_USIM_FPLMN:
      ret_val = FALSE;
      break;

    default:
      QM_MSG_HIGH_1("unexpected EF %d", file_enum);
      ret_val = FALSE;
      break;
  }

  return ret_val;
}

/*===========================================================================
  FUNCTION qmi_nas_mmgsdi_process_cmd_rsp()

  DESCRIPTION
    MMGSDI command response handler

  PARAMETERS
    p_nas: pointer to nas structure in command queue item

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
void qmi_nas_mmgsdi_process_cmd_rsp( qmi_mmode_cmd_enum_type cmd_type, void *user_data_ptr )
{
  struct qmi_nas_mmgsdi_cmd_rsp_cb_s *ptr = (struct qmi_nas_mmgsdi_cmd_rsp_cb_s*)user_data_ptr;
  struct valid_success_s         rsp_check;
  mmgsdi_return_enum_type        status;
  enum qmi_nas_mmgsdi_cache_ef_e ef;
  boolean                        success = FALSE;
  mmgsdi_session_type_enum_type  session = MMGSDI_GW_PROV_PRI_SESSION;
  qmi_nas_mmgsdi_session_e_type qmi_session;

  if (!ptr)
  {
    QM_MSG_ERROR("NULL cmd param");
    return;
  }

  qmi_nas_mmgsdi_check_cmd_rsp( ptr, &rsp_check );

  QM_MSG_HIGH_7("mmgsdi client id %d, session id %d %d %d, session type %d %d %d", qmi_nasi_global.mmgsdi.client_id, qmi_nasi_global.mmgsdi.session_id[0], qmi_nasi_global.mmgsdi.session_id[1], qmi_nasi_global.mmgsdi.session_id[2],
                               qmi_nasi_global.mmgsdi.session_type[0], qmi_nasi_global.mmgsdi.session_type[1], qmi_nasi_global.mmgsdi.session_type[2]);
  QM_MSG_HIGH_6("mmgsdi app type %d %d %d, fcn refresh %d %d %d", qmi_nasi_global.mmgsdi.app_type[0], qmi_nasi_global.mmgsdi.app_type[1], qmi_nasi_global.mmgsdi.app_type[2],
                              qmi_nasi_global.mmgsdi.fcn_refresh[0], qmi_nasi_global.mmgsdi.fcn_refresh[1], qmi_nasi_global.mmgsdi.fcn_refresh[2]);
  QM_MSG_HIGH_6("mmgsdi latest evt %d %d %d, operation ready %d %d %d", qmi_nasi_global.mmgsdi.latest_evt[0], qmi_nasi_global.mmgsdi.latest_evt[1], qmi_nasi_global.mmgsdi.latest_evt[2],
                              qmi_nasi_global.mmgsdi.operation_ready[0], qmi_nasi_global.mmgsdi.operation_ready[1], qmi_nasi_global.mmgsdi.operation_ready[2]);

  if ( rsp_check.valid )
  {
    QM_MSG_HIGH_3("mmgsdi cmd resp %d %d %d", ptr->cmd, ptr->info.response_header.client_id, ptr->info.response_header.session_id);

    switch ( ptr->cmd )
    {
      case MMGSDI_CLIENT_ID_AND_EVT_REG_CNF:
        if ( rsp_check.success )
        {
          uint32    session_type_mask = MMGSDI_GW_PROV_PRI_SESSION_MASK | MMGSDI_GW_PROV_SEC_SESSION_MASK | MMGSDI_1X_PROV_PRI_SESSION_MASK;
#ifdef FEATURE_TRIPLE_SIM
          session_type_mask = session_type_mask | MMGSDI_GW_PROV_TER_SESSION_MASK;
#endif
		  
          // save client id
          qmi_nasi_global.mmgsdi.client_id = ptr->info.response_header.client_id;

          // request all 3 sessions to be opened "at the same time"
          status = mmgsdi_session_open_ext( qmi_nasi_global.mmgsdi.client_id,
                                            session_type_mask,
                                            qmi_nas_mmgsdi_evt_cb,
                                            0x00000000,
                                            qmi_nas_mmgsdi_cmd_rsp_cb,
                                            0 );

          if ( status == MMGSDI_SUCCESS )
          {
            QM_MSG_HIGH_2("mmgsdi_session_open_ext() success client_id %d, session_type_mask %d", qmi_nasi_global.mmgsdi.client_id, session_type_mask);
          }
          else
          {
            QM_MSG_ERROR_2("ERROR:  mmgsdi_session_open_ext() status %d, session_type_mask %d", status, session_type_mask);
          }
        }
        else
        {
          QM_MSG_ERROR("MMGSDI_CLIENT_ID_AND_EVT_REG_CNF requsting session failed");
        }
        break;

      case MMGSDI_SESSION_OPEN_CNF:
        QM_MSG_ERROR_1("Old MMGSDI_SESSION_OPEN_CNF command detected, IGNORED, cmd=%d", ptr->cmd);
        break;

      case MMGSDI_SESSION_OPEN_EXT_CNF:
        if ( rsp_check.success )
        {
          mmgsdi_session_type_enum_type session_type;
          mmgsdi_session_id_type        session_id;
          uint8                         session_num;
          uint8                         number_of_sessions;

          number_of_sessions = ptr->info.session_open_ext_cnf.num_sessions;

          for ( session_num = 0 ; session_num < number_of_sessions ; session_num++ )
          {
            session_type = ptr->info.session_open_ext_cnf.session_info_ptr[session_num].session_type;
            session_id   = ptr->info.session_open_ext_cnf.session_info_ptr[session_num].session_id;

            QM_MSG_HIGH_3("MMGSDI_SESSION_OPEN_EXT_CNF success, session_type %d, id %d, num %d", session_type, session_id, session_num);

            if ( qmi_nas_mmgsdi_is_valid_session(session_type) )
            {
              qmi_session = qmi_nas_map_mmgsdi_session_to_qmi(session_type);
              qmi_nasi_global.mmgsdi.session_id  [qmi_session] = session_id;
              qmi_nasi_global.mmgsdi.session_type[qmi_session] = session_type;

              (void)qmi_nas_mmgsdi_register_refresh( qmi_session );
            }
            else
            {
              QM_MSG_ERROR_3("MMGSDI_SESSION_OPEN_EXT_CNF invalid session_type %d (%d of %d)", session_type, session_num, number_of_sessions);
            }
          }

          // deep free
          if ( ptr->info.session_open_ext_cnf.session_info_ptr != NULL )
          {
            modem_mem_free ( ptr->info.session_open_ext_cnf.session_info_ptr, MODEM_MEM_CLIENT_QMI_MMODE );
            ptr->info.session_open_ext_cnf.session_info_ptr = NULL;
            QM_MSG_HIGH("mem free for MMGSDI_SESSION_OPEN_EXT_CNF");
          }
        }
        else
        {
          QM_MSG_ERROR_1("MMGSDI_SESSION_OPEN_EXT_CNF failed %d", ptr->status);
        }
        break;

      case MMGSDI_REFRESH_CNF: // nothing to do
        break;

      case MMGSDI_READ_CNF:
        QM_MSG_HIGH_1("file enum %d", ptr->info.read_cnf.access.file.file_enum);

        session = qmi_nas_mmgsdi_get_session_type( ptr->info.read_cnf.response_header.session_id );

        if ( qmi_nas_mmgsdi_is_valid_session(session) )
        {
          if ( qmi_nas_mmgsdi_cached_ef( ptr->info.read_cnf.access.file.file_enum ) )
          {
            qmi_nas_mmgsdi_process_cached_ef_read_cnf( ptr );
          }
          else
          {
            qmi_nas_mmgsdi_process_regular_ef_read_cnf( ptr );
          }
        }
        else
        {
          QM_MSG_ERROR_2("invalid session %d %d", session, ptr->info.read_cnf.response_header.session_id);
        }

        // free deep copied items
        if ( ptr->info.read_cnf.read_data.data_ptr )
          modem_mem_free ( ptr->info.read_cnf.read_data.data_ptr, MODEM_MEM_CLIENT_QMI_MMODE );

        break;

      case MMGSDI_GET_FILE_ATTR_CNF:
        session = qmi_nas_mmgsdi_get_session_type( ptr->info.get_file_attr_cnf.response_header.session_id );
        qmi_session = qmi_nas_map_mmgsdi_session_to_qmi(session);

        if ( !qmi_nas_mmgsdi_is_valid_session(session) )
        {
          rsp_check.success = FALSE;
          QM_MSG_ERROR_2("invalid session %d %d", session, ptr->info.get_file_attr_cnf.response_header.session_id);
        }
        else
        {
          QM_MSG_HIGH_1("file enum %d", ptr->info.get_file_attr_cnf.access.file.file_enum);
          ef = qmi_nas_mmgsdi_get_qmi_ef_enum ( ptr->info.get_file_attr_cnf.access.file.file_enum );
          if ( rsp_check.success )
          {
            switch ( ptr->info.get_file_attr_cnf.access.file.file_enum )
            {
              case MMGSDI_GSM_OPL:
              case MMGSDI_USIM_OPL:
                if ( qmi_nasi_global.mmgsdi.cache[qmi_session].ef_opl_num_recs != ptr->info.get_file_attr_cnf.file_attrib.file_info.linear_fixed_file.num_of_rec )
                {
                  qmi_nasi_global.mmgsdi.cache[qmi_session].ef_opl_changed = TRUE;
                  qmi_nasi_global.mmgsdi.cache[qmi_session].ef_opl_num_recs = ptr->info.get_file_attr_cnf.file_attrib.file_info.linear_fixed_file.num_of_rec;
                  memset( qmi_nasi_global.mmgsdi.cache[qmi_session].ef_opl, 0x00, QMI_NAS_MMGSDI_LEN_EF_OPL );
                }
                qmi_nasi_global.mmgsdi.cache[qmi_session].ef_opl_bytes_read = 0;
                success = qmi_nas_mmgsdi_read_record ( qmi_session, ef, qmi_nasi_global.mmgsdi.app_type[qmi_session], 1 );
                break;

              case MMGSDI_GSM_PNN:
              case MMGSDI_USIM_PNN:
                if ( qmi_nasi_global.mmgsdi.cache[qmi_session].ef_pnn_rec_size != ptr->info.get_file_attr_cnf.file_attrib.file_info.linear_fixed_file.rec_len ||
                     qmi_nasi_global.mmgsdi.cache[qmi_session].ef_pnn_num_recs != ptr->info.get_file_attr_cnf.file_attrib.file_info.linear_fixed_file.num_of_rec )
                {
                  qmi_nasi_global.mmgsdi.cache[qmi_session].ef_pnn_changed = TRUE;
                  qmi_nasi_global.mmgsdi.cache[qmi_session].ef_pnn_rec_size = ptr->info.get_file_attr_cnf.file_attrib.file_info.linear_fixed_file.rec_len;
                  qmi_nasi_global.mmgsdi.cache[qmi_session].ef_pnn_num_recs = ptr->info.get_file_attr_cnf.file_attrib.file_info.linear_fixed_file.num_of_rec;
                  memset( qmi_nasi_global.mmgsdi.cache[qmi_session].ef_pnn, 0x00, QMI_NAS_MMGSDI_LEN_EF_PNN );
                }
                qmi_nasi_global.mmgsdi.cache[qmi_session].ef_pnn_bytes_read = 0;
                success = qmi_nas_mmgsdi_read_record ( qmi_session, ef, qmi_nasi_global.mmgsdi.app_type[qmi_session], 1 );

                break;

              case MMGSDI_GSM_PLMNWACT:
              case MMGSDI_USIM_PLMNWACT:
              case MMGSDI_USIM_UPLMNSEL:
                qmi_nasi_global.mmgsdi.cache[qmi_session].ef_plmnwact_valid = QMI_NAS_MMGSDI_VALID;
                qmi_nasi_global.mmgsdi.cache[qmi_session].ef_plmnwact_size = ptr->info.get_file_attr_cnf.file_attrib.file_size;
                success = TRUE;
                qmi_nas_mmgsdi_trigger_next_ef_read( qmi_session );
                break;

              case MMGSDI_GSM_PLMN:
                qmi_nasi_global.mmgsdi.cache[qmi_session].ef_plmn_valid = QMI_NAS_MMGSDI_VALID;
                qmi_nasi_global.mmgsdi.cache[qmi_session].ef_plmn_size = ptr->info.get_file_attr_cnf.file_attrib.file_size;
                success = TRUE;
                qmi_nas_mmgsdi_trigger_next_ef_read( qmi_session );
                break;

              default:
                success = FALSE;
                QM_MSG_ERROR_2("unrequested file attribute %d %d", ptr->info.get_file_attr_cnf.access.file.file_enum, session);
                break;
            }
          }
          else
          {
            success = FALSE;
            qmi_nas_mmgsdi_change_cache_state( qmi_session, ef, QMI_NAS_MMGSDI_ERROR );
          }

          if (!success)
          {
            qmi_nas_mmgsdi_trigger_next_ef_read( qmi_session );
          }
        }

        break;

      case MMGSDI_WRITE_CNF:
        {
          mmgsdi_cnf_type *cnf_data =  modem_mem_alloc( sizeof(ptr->info), MODEM_MEM_CLIENT_QMI_MMODE );

          if(cnf_data != NULL)
          {
            memscpy( cnf_data, sizeof(ptr->info), &ptr->info,  sizeof(ptr->info));
            qmi_nas_process_mmgsdi_operation_complete( MMGSDI_WRITE_CNF, cnf_data );
            modem_mem_free ( cnf_data, MODEM_MEM_CLIENT_QMI_MMODE );
          }
          else
          {
			  qmi_voice_mem_error();
          }
        }
        break;

      case MMGSDI_SESSION_GET_OPERATOR_NAME_CNF:
        {
          uint32 cnt = 0;
          uint32 add_cnt = 0;
          qmi_nas_process_mmgsdi_get_operator_name( &ptr->info );

          if (  ptr->info.session_get_operator_name_cnf.response_header.mmgsdi_status == MMGSDI_SUCCESS )
          {
            // free deep copied items
            if ( ptr->info.session_get_operator_name_cnf.plmn_list.plmn_info_ptr )
            {
              for(cnt = 0; cnt < ptr->info.session_get_operator_name_cnf.plmn_list.num_of_plmn_ids; cnt++)
              {
                if ( ptr->info.session_get_operator_name_cnf.plmn_list.plmn_info_ptr[cnt].plmn_long_name.plmn_name.eons_data.data_ptr)
                {
                  modem_mem_free ( ptr->info.session_get_operator_name_cnf.plmn_list.plmn_info_ptr[cnt].plmn_long_name.plmn_name.eons_data.data_ptr, MODEM_MEM_CLIENT_QMI_MMODE );
                }

                if ( ptr->info.session_get_operator_name_cnf.plmn_list.plmn_info_ptr[cnt].plmn_short_name.plmn_name.eons_data.data_ptr)
                {
                  modem_mem_free ( ptr->info.session_get_operator_name_cnf.plmn_list.plmn_info_ptr[cnt].plmn_short_name.plmn_name.eons_data.data_ptr, MODEM_MEM_CLIENT_QMI_MMODE );
                }

                if ( ptr->info.session_get_operator_name_cnf.plmn_list.plmn_info_ptr[cnt].plmn_additional_info.info_data.data_ptr)
                {
                  modem_mem_free ( ptr->info.session_get_operator_name_cnf.plmn_list.plmn_info_ptr[cnt].plmn_additional_info.info_data.data_ptr, MODEM_MEM_CLIENT_QMI_MMODE );
                }

                for(add_cnt = 0; add_cnt < ptr->info.session_get_operator_name_cnf.plmn_list.plmn_info_ptr[cnt].num_plmn_additional_names; add_cnt++)
                {
                  if ( ptr->info.session_get_operator_name_cnf.plmn_list.plmn_info_ptr[cnt].plmn_additional_names[add_cnt].plmn_long_name.plmn_name.eons_data.data_ptr)
                  {
                    modem_mem_free ( ptr->info.session_get_operator_name_cnf.plmn_list.plmn_info_ptr[cnt].plmn_additional_names[add_cnt].plmn_long_name.plmn_name.eons_data.data_ptr, MODEM_MEM_CLIENT_QMI_MMODE );
                  }

                  if ( ptr->info.session_get_operator_name_cnf.plmn_list.plmn_info_ptr[cnt].plmn_additional_names[add_cnt].plmn_short_name.plmn_name.eons_data.data_ptr)
                  {
                    modem_mem_free ( ptr->info.session_get_operator_name_cnf.plmn_list.plmn_info_ptr[cnt].plmn_additional_names[add_cnt].plmn_short_name.plmn_name.eons_data.data_ptr, MODEM_MEM_CLIENT_QMI_MMODE );
                  }
                }
              }

              modem_mem_free ( ptr->info.session_get_operator_name_cnf.plmn_list.plmn_info_ptr, MODEM_MEM_CLIENT_QMI_MMODE );
            }

            if ( ptr->info.session_get_operator_name_cnf.spn.eons_data.data_ptr)
            {
              modem_mem_free ( ptr->info.session_get_operator_name_cnf.spn.eons_data.data_ptr, MODEM_MEM_CLIENT_QMI_MMODE );
            }
          }

        }
        break;

      case MMGSDI_GET_SE13_PLMN_NAMES_CNF:
        {
          uint32 cnt = 0;
          uint32 add_cnt = 0;
          qmi_nas_process_mmgsdi_get_se13_plmn_names( &ptr->info );

          if (  ptr->info.get_se13_plmn_names_cnf.response_header.mmgsdi_status == MMGSDI_SUCCESS )
          {
            // free deep copied items
            if ( ptr->info.get_se13_plmn_names_cnf.plmn_list.plmn_info_ptr )
            {
              for(cnt = 0; cnt < ptr->info.get_se13_plmn_names_cnf.plmn_list.num_of_plmn_ids; cnt++)
              {
                if ( ptr->info.get_se13_plmn_names_cnf.plmn_list.plmn_info_ptr[cnt].plmn_long_name.plmn_name.eons_data.data_ptr)
                {
                  modem_mem_free ( ptr->info.get_se13_plmn_names_cnf.plmn_list.plmn_info_ptr[cnt].plmn_long_name.plmn_name.eons_data.data_ptr, MODEM_MEM_CLIENT_QMI_MMODE );
                }

                if ( ptr->info.get_se13_plmn_names_cnf.plmn_list.plmn_info_ptr[cnt].plmn_short_name.plmn_name.eons_data.data_ptr)
                {
                  modem_mem_free ( ptr->info.get_se13_plmn_names_cnf.plmn_list.plmn_info_ptr[cnt].plmn_short_name.plmn_name.eons_data.data_ptr, MODEM_MEM_CLIENT_QMI_MMODE );
                }

                if ( ptr->info.get_se13_plmn_names_cnf.plmn_list.plmn_info_ptr[cnt].plmn_additional_info.info_data.data_ptr)
                {
                  modem_mem_free ( ptr->info.get_se13_plmn_names_cnf.plmn_list.plmn_info_ptr[cnt].plmn_additional_info.info_data.data_ptr, MODEM_MEM_CLIENT_QMI_MMODE );
                }

                for(add_cnt = 0; add_cnt < ptr->info.get_se13_plmn_names_cnf.plmn_list.plmn_info_ptr[cnt].num_plmn_additional_names; add_cnt++)
                {
                  if ( ptr->info.get_se13_plmn_names_cnf.plmn_list.plmn_info_ptr[cnt].plmn_additional_names[add_cnt].plmn_long_name.plmn_name.eons_data.data_ptr)
                  {
                    modem_mem_free ( ptr->info.get_se13_plmn_names_cnf.plmn_list.plmn_info_ptr[cnt].plmn_additional_names[add_cnt].plmn_long_name.plmn_name.eons_data.data_ptr, MODEM_MEM_CLIENT_QMI_MMODE );
                  }

                  if ( ptr->info.get_se13_plmn_names_cnf.plmn_list.plmn_info_ptr[cnt].plmn_additional_names[add_cnt].plmn_short_name.plmn_name.eons_data.data_ptr)
                  {
                    modem_mem_free ( ptr->info.get_se13_plmn_names_cnf.plmn_list.plmn_info_ptr[cnt].plmn_additional_names[add_cnt].plmn_short_name.plmn_name.eons_data.data_ptr, MODEM_MEM_CLIENT_QMI_MMODE );
                  }
                }
              }

              modem_mem_free ( ptr->info.get_se13_plmn_names_cnf.plmn_list.plmn_info_ptr, MODEM_MEM_CLIENT_QMI_MMODE );
            }
          }
        }
        break;
        case MMGSDI_SET_BUILTIN_PLMN_LIST_CNF:
        {
            enum qmi_nas_subs_e  as_id = ( enum qmi_nas_subs_e )ptr->info.set_builtin_plmn_list_cnf.response_header.client_data;
            mmgsdi_return_enum_type   mmgsdi_status = ptr->info.set_builtin_plmn_list_cnf.response_header.mmgsdi_status;

            QM_MSG_HIGH_2("MMGSDI_SET_BUILTIN_PLMN_LIST_CNF Status %d, as_id:%d", mmgsdi_status, as_id);
            if (  mmgsdi_status == MMGSDI_SUCCESS )
            {
               qmi_nas_generate_set_builtin_plmn_list_ind(QMI_ERR_NONE_V01,  as_id);
            }
            else
            {
              qmi_nas_generate_set_builtin_plmn_list_ind(QMI_ERR_INTERNAL_V01,  as_id);
            }
        }
        break;

      default:
        break;
    }
  }
  else
  {
    QM_MSG_ERROR_3("invalid mmgsdi msg %d %d %d", ptr->cmd, ptr->info.response_header.response_type, session);
  }
}

/*===========================================================================
  FUNCTION qmi_nas_mmgsdi_process_evt()

  DESCRIPTION
    MMGSDI event handler

  PARAMETERS
    p_nas: pointer to nas structure in command queue item

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
void qmi_nas_mmgsdi_process_evt( qmi_mmode_cmd_enum_type cmd_type, void *user_data_ptr )
{
  mmgsdi_event_data_type *ptr = (mmgsdi_event_data_type *)user_data_ptr;
  mmgsdi_return_enum_type       status;
  mmgsdi_session_type_enum_type session;
  qmi_nas_mmgsdi_session_e_type qmi_session;

  if (!ptr)
  {
    QM_MSG_ERROR("NULL cmd param");
    return;
  }

  QM_MSG_HIGH_7("mmgsdi client id %d, session id %d %d %d, session type %d %d %d",qmi_nasi_global.mmgsdi.client_id, qmi_nasi_global.mmgsdi.session_id[0], qmi_nasi_global.mmgsdi.session_id[1], qmi_nasi_global.mmgsdi.session_id[2],
                                qmi_nasi_global.mmgsdi.session_type[0], qmi_nasi_global.mmgsdi.session_type[1], qmi_nasi_global.mmgsdi.session_type[2]);
  QM_MSG_HIGH_6("mmgsdi app type %d %d %d, fcn refresh %d %d %d", qmi_nasi_global.mmgsdi.app_type[0], qmi_nasi_global.mmgsdi.app_type[1], qmi_nasi_global.mmgsdi.app_type[2],
                              qmi_nasi_global.mmgsdi.fcn_refresh[0], qmi_nasi_global.mmgsdi.fcn_refresh[1], qmi_nasi_global.mmgsdi.fcn_refresh[2]);
  QM_MSG_HIGH_6("mmgsdi latest evt %d %d %d, operation ready %d %d %d", qmi_nasi_global.mmgsdi.latest_evt[0], qmi_nasi_global.mmgsdi.latest_evt[1], qmi_nasi_global.mmgsdi.latest_evt[2],
                              qmi_nasi_global.mmgsdi.operation_ready[0], qmi_nasi_global.mmgsdi.operation_ready[1], qmi_nasi_global.mmgsdi.operation_ready[2]);

  session = qmi_nas_mmgsdi_get_session_type( ptr->session_id );

  QM_MSG_HIGH_3("mmgsdi event %d session %d %d", ptr->evt, session, ptr->session_id);

  if ( qmi_nas_mmgsdi_is_valid_session(session) )
  {
    qmi_session = qmi_nas_map_mmgsdi_session_to_qmi(session);
    qmi_nasi_global.mmgsdi.latest_evt[qmi_session] = ptr->evt;

    switch ( ptr->evt )
    {
      case MMGSDI_SUBSCRIPTION_READY_EVT:
        // save app_type
        qmi_nasi_global.mmgsdi.app_type[qmi_session] = ptr->data.subscription_ready.app_info.app_data.app_type;
        qmi_nasi_global.mmgsdi.operation_ready[qmi_session] = TRUE;
        qmi_nasi_global.mmgsdi.session_active[qmi_session]  = TRUE; // in case SESSION_CHANGED_EVT don't come

        if ( session == MMGSDI_1X_PROV_PRI_SESSION ) // for 1x, no updating cache
        {
          mmgsdi_access_type      file_access;

          file_access.access_method  = MMGSDI_EF_ENUM_ACCESS;
          file_access.file.file_enum = qmi_nasi_global.mmgsdi.app_type[qmi_session] == MMGSDI_APP_RUIM ? MMGSDI_CDMA_HOME_SVC_PVDR_NAME : MMGSDI_CSIM_SPN;

          QM_MSG_HIGH_1("Reading file enum %d", file_access.file.file_enum);

          status = mmgsdi_session_read_transparent( qmi_nasi_global.mmgsdi.session_id[qmi_session],
                                                    file_access,
                                                    0,
                                                    QMI_NAS_MMGSDI_LEN_EF_CDMA_SPN,
                                                    qmi_nas_mmgsdi_cmd_rsp_cb,
                                                    0 );
          if (status != MMGSDI_SUCCESS)
          {
            QM_MSG_ERROR_3("cdma spn read fail, status = %d %d %d", status, qmi_session, ptr->session_id);
            qmi_nas_mmgsdi_change_cache_state( qmi_session, QMI_NAS_MMGSDI_EF_CDMA_SPN, QMI_NAS_MMGSDI_ERROR );
          }
        }
        else
        {
          // invalidate EFs
          qmi_nas_mmgsdi_change_cache_state( qmi_session, QMI_NAS_MMGSDI_EF_ALL, QMI_NAS_MMGSDI_NOT_INIT );
          if ( !qmi_nasi_global.mmgsdi.cache[qmi_session].updating )
          {
            qmi_nasi_global.mmgsdi.cache[qmi_session].updating = TRUE;
            qmi_nas_mmgsdi_trigger_next_ef_read( qmi_session );
          }
        }
        break;

      case MMGSDI_SESSION_CHANGED_EVT:
        session = qmi_nas_mmgsdi_get_session_type( ptr->session_id );
        qmi_session = qmi_nas_map_mmgsdi_session_to_qmi(session);

        qmi_nasi_global.mmgsdi.operation_ready[qmi_session] = FALSE;

        if ( qmi_nas_mmgsdi_is_valid_session(session) )
        {
          QM_MSG_HIGH_1("activated %d", ptr->data.session_changed.activated);

          qmi_nasi_global.mmgsdi.session_active[qmi_session]  = ptr->data.session_changed.activated;

          if ( !ptr->data.session_changed.activated )
          {
            qmi_nas_mmgsdi_change_cache_state( qmi_session, QMI_NAS_MMGSDI_EF_ALL, QMI_NAS_MMGSDI_ERROR );
          }
        }
        else
        {
          QM_MSG_ERROR_2("unexpected session %d %d", qmi_session, ptr->session_id);
        }
        break;

      case MMGSDI_REFRESH_EVT:
        QM_MSG_HIGH_2("stage %d mode %d", ptr->data.refresh.stage, ptr->data.refresh.mode);
        qmi_nasi_global.mmgsdi.operation_ready[qmi_session] = TRUE;

        if ( qmi_nas_mmgsdi_is_valid_3gpp_session(session) ) // for 1x, no refresh
        {
          if ( ptr->data.refresh.stage == MMGSDI_REFRESH_STAGE_WAIT_FOR_OK_TO_FCN )
          {
            // no reason to hold on to ok to fcn. blindly send ok to fcn.
            status = mmgsdi_session_ok_to_refresh( qmi_nasi_global.mmgsdi.session_id[qmi_session],
                                                   TRUE,
                                                   qmi_nas_mmgsdi_cmd_rsp_cb,
                                                   0 );
            if (status != MMGSDI_SUCCESS)
              QM_MSG_ERROR_2("ok to refresh failure %d %d", status, qmi_session);
          }
          else if ( ptr->data.refresh.stage == MMGSDI_REFRESH_STAGE_START )
          {
            // trigger cache update only for NAA_FCN as it will not send separate MMGSDI_SUBSCRIPTION_READY_EVT
            if ( ptr->data.refresh.mode == MMGSDI_REFRESH_NAA_FCN )
            {
              uint32 i, j;

              /* invalidate cache for files listed Check if the FCN refresh is for ones we registered */
              for ( i=0; i<ptr->data.refresh.refresh_files.num_files; i++ )
              {
                for (j=0; j<QMI_NAS_ARR_SIZE(refresh_file_list); j++)
                {
                  if ( ptr->data.refresh.refresh_files.file_list_ptr[i] == refresh_file_list[j] )
                  {
                    enum qmi_nas_mmgsdi_cache_ef_e qmi_ef;

                    qmi_ef = qmi_nas_mmgsdi_get_qmi_ef_enum( ptr->data.refresh.refresh_files.file_list_ptr[i] );
                    qmi_nas_mmgsdi_change_cache_state( qmi_session, qmi_ef, QMI_NAS_MMGSDI_NOT_INIT );

                    QM_MSG_HIGH_1("EF %d FCN", ptr->data.refresh.refresh_files.file_list_ptr[i]);
                    break;
                  }
                }
              }

              status = mmgsdi_session_refresh_complete( qmi_nasi_global.mmgsdi.session_id[qmi_session], TRUE, qmi_nas_mmgsdi_cmd_rsp_cb, 0 );

              if ( status != MMGSDI_SUCCESS )
              {
                QM_MSG_ERROR_2("refresh complete failed %d %d", status, qmi_session);
              }

              if ( !qmi_nasi_global.mmgsdi.cache[qmi_session].updating )
              {
                qmi_nasi_global.mmgsdi.cache[qmi_session].updating = TRUE;
                qmi_nas_mmgsdi_trigger_next_ef_read( qmi_session );
              }
            }
            // for NAA_INIT_FCN and INIT_FULL_FCN, don't bother updating EFs yet as MMGSDI_SUBSCRIPTION_READY_EVT
            // will be sent later on which will trigger entire cache update
            else if ( ptr->data.refresh.mode == MMGSDI_REFRESH_NAA_INIT_FCN ||
                      ptr->data.refresh.mode == MMGSDI_REFRESH_NAA_INIT_FULL_FCN )
            {
              qmi_nasi_global.mmgsdi.fcn_refresh[qmi_session] = TRUE;
            }
          }
        }

        // free deep copied items
        if ( ptr->data.refresh.refresh_files.file_list_ptr )
          modem_mem_free ( ptr->data.refresh.refresh_files.file_list_ptr, MODEM_MEM_CLIENT_QMI_MMODE );
        if ( ptr->data.refresh.refresh_files.file_path_ptr )
          modem_mem_free ( ptr->data.refresh.refresh_files.file_path_ptr, MODEM_MEM_CLIENT_QMI_MMODE );

        break;

      case MMGSDI_PIN1_EVT:
        QM_MSG_HIGH_1("MMGSDI PIN1_EVT status %d", ptr->data.pin.pin_info.status);
        switch( ptr->data.pin.pin_info.status )
        {
          case MMGSDI_PIN_ENABLED_VERIFIED:
          case MMGSDI_PIN_DISABLED:
          case MMGSDI_PIN_UNBLOCKED:       // If card is not initialized, MMGSDI_SUBS_READY event will come later
          qmi_nasi_global.mmgsdi.operation_ready[qmi_session] = TRUE;
            break;
          case MMGSDI_PIN_BLOCKED:         // card is temporary blocked, PIN_UNBLOCKED status indicates card is ready to be used later
          case MMGSDI_PIN_PERM_BLOCKED:    // card is permanently blocked, ignore all request for SIM read
          qmi_nasi_global.mmgsdi.operation_ready[qmi_session] = FALSE;
            break;
          case MMGSDI_PIN_RETRY_COUNT_DECREASED:
          case MMGSDI_PIN_CHANGED:
          default:
            break;
        }
        break;

      case MMGSDI_PIN2_EVT:
        break;

      case MMGSDI_EONS_EVT:
      {
        uint32 i = 0;
        sys_modem_as_id_e_type asubs_id = qmi_nas_map_mmgsdi_session_to_cm_asubs( session );
        if(asubs_id == SYS_MODEM_AS_ID_NONE)
        {
          QM_MSG_HIGH_2("Invalid asubs_id returned by qmi_nas_map_mmgsdi_session_to_cm_asubs(). asubs_id: %d -> %d", asubs_id, SYS_MODEM_AS_ID_1);
          asubs_id = SYS_MODEM_AS_ID_1;
        }
        qmi_nas_nw_name_add( session, ptr->data.eons );

#ifdef  FEATURE_DUAL_SIM
        qmi_nas_gen_serving_system_ind( &qmi_nasi_global.cm_msim_ss_info[asubs_id], &qmi_nasi_global.cm_ph_info, &qmi_nasi_global.cm_ph_info2 );
#else
        qmi_nas_gen_serving_system_ind( &qmi_nasi_global.cm_msim_ss_info[asubs_id], &qmi_nasi_global.cm_ph_info, NULL );
#endif
        if( session == MMGSDI_GW_PROV_PRI_SESSION || session == MMGSDI_GW_PROV_SEC_SESSION || session == MMGSDI_GW_PROV_TER_SESSION)
        {
          qmi_nas_gen_current_plmn_name_ind( &qmi_nasi_global.cm_msim_ss_info[asubs_id], qmi_session );
        }

        // free deep copied items
        if( ptr->data.eons.spn.eons_data.data_ptr )
        {
          modem_mem_free( ptr->data.eons.spn.eons_data.data_ptr, MODEM_MEM_CLIENT_QMI_MMODE );
        }
        if ( ptr->data.eons.first_stack_info.is_valid )
        {
          if( ptr->data.eons.first_stack_info.operator_info.plmn_long_name.plmn_name.eons_data.data_ptr )
          {
            modem_mem_free( ptr->data.eons.first_stack_info.operator_info.plmn_long_name.plmn_name.eons_data.data_ptr, MODEM_MEM_CLIENT_QMI_MMODE );
          }
          if( ptr->data.eons.first_stack_info.operator_info.plmn_short_name.plmn_name.eons_data.data_ptr )
          {
            modem_mem_free( ptr->data.eons.first_stack_info.operator_info.plmn_short_name.plmn_name.eons_data.data_ptr, MODEM_MEM_CLIENT_QMI_MMODE );
          }
          if( ptr->data.eons.first_stack_info.operator_info.plmn_additional_info.info_data.data_ptr )
          {
            modem_mem_free( ptr->data.eons.first_stack_info.operator_info.plmn_additional_info.info_data.data_ptr, MODEM_MEM_CLIENT_QMI_MMODE );
          }
          for (i = 0; i < ptr->data.eons.first_stack_info.operator_info.num_plmn_additional_names; i++)
          {
            if( ptr->data.eons.first_stack_info.operator_info.plmn_additional_names[i].plmn_long_name.plmn_name.eons_data.data_ptr )
            {
              modem_mem_free( ptr->data.eons.first_stack_info.operator_info.plmn_additional_names[i].plmn_long_name.plmn_name.eons_data.data_ptr, MODEM_MEM_CLIENT_QMI_MMODE );
            }
            if( ptr->data.eons.first_stack_info.operator_info.plmn_additional_names[i].plmn_short_name.plmn_name.eons_data.data_ptr )
            {
              modem_mem_free( ptr->data.eons.first_stack_info.operator_info.plmn_additional_names[i].plmn_short_name.plmn_name.eons_data.data_ptr, MODEM_MEM_CLIENT_QMI_MMODE );
            }
          }
        }
        if ( ptr->data.eons.sec_stack_info.is_valid )
        {
          if( ptr->data.eons.sec_stack_info.operator_info.plmn_long_name.plmn_name.eons_data.data_ptr )
          {
            modem_mem_free( ptr->data.eons.sec_stack_info.operator_info.plmn_long_name.plmn_name.eons_data.data_ptr, MODEM_MEM_CLIENT_QMI_MMODE );
          }
          if( ptr->data.eons.sec_stack_info.operator_info.plmn_short_name.plmn_name.eons_data.data_ptr )
          {
            modem_mem_free( ptr->data.eons.sec_stack_info.operator_info.plmn_short_name.plmn_name.eons_data.data_ptr, MODEM_MEM_CLIENT_QMI_MMODE );
          }
          if( ptr->data.eons.sec_stack_info.operator_info.plmn_additional_info.info_data.data_ptr )
          {
            modem_mem_free( ptr->data.eons.sec_stack_info.operator_info.plmn_additional_info.info_data.data_ptr, MODEM_MEM_CLIENT_QMI_MMODE );
          }
          for (i = 0; i < ptr->data.eons.sec_stack_info.operator_info.num_plmn_additional_names; i++)
          {
            if( ptr->data.eons.sec_stack_info.operator_info.plmn_additional_names[i].plmn_long_name.plmn_name.eons_data.data_ptr )
            {
              modem_mem_free( ptr->data.eons.sec_stack_info.operator_info.plmn_additional_names[i].plmn_long_name.plmn_name.eons_data.data_ptr, MODEM_MEM_CLIENT_QMI_MMODE );
            }
            if( ptr->data.eons.sec_stack_info.operator_info.plmn_additional_names[i].plmn_short_name.plmn_name.eons_data.data_ptr )
            {
              modem_mem_free( ptr->data.eons.sec_stack_info.operator_info.plmn_additional_names[i].plmn_short_name.plmn_name.eons_data.data_ptr, MODEM_MEM_CLIENT_QMI_MMODE );
            }
          }
        }
      }
        break;

#if 0
      case MMGSDI_SESSION_CHANGED_EVT:
        // check boolean
      case MMGSDI_CARD_REMOVED_EVT:
        qmi_nas_mmgsdi_change_cache_state( QMI_NAS_MMGSDI_EF_ALL, QMI_NAS_MMGSDI_ERROR );
        break;

#endif

      case MMGSDI_SESSION_ILLEGAL_SUBSCRIPTION_EVT:
         // If (operation_ready == TRUE) at this time, EF files can still be read. 
        break;
        
      default:
        if ( qmi_nas_mmgsdi_is_valid_session(session) )
        {
          qmi_nasi_global.mmgsdi.operation_ready[qmi_session] = FALSE;
        }
        break;
    }
  }
  else
  {
    QM_MSG_ERROR_1("invalid session id %d", session);
  }

}

/*===========================================================================
  FUNCTION qmi_nas_mmgsdi_ef_ready()

  DESCRIPTION
    Checks if selected EFs are in a state to be read. If any of the EFs are
    being read from SIM return FALSE. If all EFs are already read but all are
    in ERROR state, return FALSE. Anything other than that, return TRUE.
===========================================================================*/
boolean qmi_nas_mmgsdi_ef_ready ( qmi_nas_mmgsdi_session_e_type qmi_session, uint32 bitmap )
{
  boolean all_error = TRUE;

#define QMI_NAS_MMGSDI_BITMAP_CHECK(a,b) \
  if ( bitmap & QMI_NAS_MMGSDI_BITMAP_EF_##a ) \
  { \
    switch ( qmi_nasi_global.mmgsdi.cache[qmi_session].ef_##b##_valid ) \
    { \
      case QMI_NAS_MMGSDI_NOT_INIT: \
        return FALSE; \
 \
      case QMI_NAS_MMGSDI_VALID: \
        all_error = FALSE; \
        break; \
 \
      default: \
        break; \
    } \
  }

  QMI_NAS_MMGSDI_BITMAP_CHECK (SPN, spn);
  QMI_NAS_MMGSDI_BITMAP_CHECK (OPL, opl);
  QMI_NAS_MMGSDI_BITMAP_CHECK (PNN, pnn);
  QMI_NAS_MMGSDI_BITMAP_CHECK (ONS, ons);
  QMI_NAS_MMGSDI_BITMAP_CHECK (CSP, csp);
  QMI_NAS_MMGSDI_BITMAP_CHECK (AD, csp);
  QMI_NAS_MMGSDI_BITMAP_CHECK (IMSI, csp);

  if ( all_error )
    return FALSE;
  else
    return TRUE;
}

