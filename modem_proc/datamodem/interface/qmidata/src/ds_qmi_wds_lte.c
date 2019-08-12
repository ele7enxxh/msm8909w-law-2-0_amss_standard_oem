/*===========================================================================

                         D S _ Q M I _ W D S _ L T E . C

DESCRIPTION

 The Data Services Qualcomm Wireless Data Services MSM Interface source file
 that contains LTE specific functions.
EXTERNALIZED FUNCTIONS

  QMI_WDS_SET_LTE_ATTACH_PDN_LIST()
     Set the lte attach pdn profile list by calling into the PS SYS layer
  QMI_WDS_GET_LTE_ATTACH_PDN_LIST()
     Retrieves lte attach pdn list by calling into the PS SYS layer
  QMI_WDS_GET_LTE_MAX_ATTACH_PDN_LIST_NUM()
     Retrieves max lte attach pdn list size
  QMI_WDS_SET_LTE_DATA_RETRY()
     Modifies the LTE Data Retry setting.
  QMI_WDS_GET_LTE_DATA_RETRY()
     Retrieves the LTE Data Retry setting.
  QMI_WDS_SET_LTE_ATTACH_TYPE()
     Modifies the LTE Attach Type setting.
  QMI_WDS_GET_LTE_ATTACH_TYPE()
     Retrieves the LTE Attach Type setting.
  QMI_WDS_UPDATE_LTE_ATTACH_PDN_LIST_PROFILES()
     Updates the LTE attach PDN list profile params
Copyright (c) 2013 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary.
===========================================================================*/
/*===========================================================================

                      EDIT HISTORY FOR FILE
                      
  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/qmidata/src/ds_qmi_wds_lte.c#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $
  
when        who    what, where, why
--------    ---    ----------------------------------------------------------
05/13/13    sj     Created module
===========================================================================*/

/*===========================================================================

                          INCLUDE FILES FOR MODULE

===========================================================================*/

#include "datamodem_variation.h"
#include "comdef.h"
#include "customer.h"
#if defined (FEATURE_DATA_LTE)
#include "amssassert.h"
#include "dsm.h"
#include "ds_Utils_DebugMsg.h"

#include "ds_qmi_wds_lte.h"
#include "qmi_svc_defs.h"
#include "qmi_svc_utils.h"

#include "ps_sys.h"
#include "ps_sys_conf.h"
#include "ps_sys_ioctl.h"
#include "ps_sys_event.h"
#include "dserrno.h"
#include "ds_qmi_wds_profile.h"
#include "ds_qmi_wds.h"
#include "ds_qmi_svc.h"
#include "qmi_idl_lib.h"
#include "common_v01.h"
#include "wireless_data_service_v01.h"
#include "ps_utils.h"

/*===========================================================================

                               DEFINITIONS

===========================================================================*/


/*===========================================================================
            
                        EXTERNAL FUNCTION DEFINITIONS

===========================================================================*/

/*===========================================================================
  FUNCTION QMI_WDS_SET_LTE_ATTACH_PDN_LIST()

  DESCRIPTION
    Set the lte attach pdn profile list by calling into the PS SYS layer

  PARAMETERS
    sp        : service provided state pointer
    cmd_buf_p : ptr to cmd buffer
    cl_sp     : client state pointer
    sdu_in    : incoming request

  RETURN VALUE
    ptr to response

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
dsm_item_type*  qmi_wds_set_lte_attach_pdn_list
(
  void *            sp,
  void *            cmd_buf_p,
  void *            cl_sp,
  dsm_item_type **  sdu_in
)
{
  dsm_item_type *       response;
  wds_set_lte_attach_pdn_list_req_msg_v01 req_msg;
  wds_set_lte_attach_pdn_list_resp_msg_v01 resp_msg;
  qmi_error_type_v01    errval;
  int                   return_val;
  int16                 ps_errno = DS_ENOERR;
  int                   cnt_pdn;
  ps_sys_3gpp_lte_attach_pdn_list_type        attach_pdn_list;
  uint32                                      subs_id;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  memset(&attach_pdn_list, 0, sizeof(attach_pdn_list));
  memset(&req_msg, 0, sizeof(req_msg));
  memset(&resp_msg, 0, sizeof(resp_msg));

  response = NULL;
  errval = QMI_ERR_NONE_V01;

  errval = QMI_WDSI_DECODE_REQUEST_MSG(QMI_WDS_SET_LTE_ATTACH_PDN_LIST_REQ_V01,
                                       sdu_in,
                                       &req_msg,
                                       sizeof(req_msg));
  if(errval != QMI_ERR_NONE_V01)
  {
    goto send_result;
  }

  attach_pdn_list.attach_pdn_profile_list_num =
     req_msg.attach_pdn_list_len;

  for (cnt_pdn = 0; cnt_pdn < attach_pdn_list.attach_pdn_profile_list_num; cnt_pdn++)
  {
    attach_pdn_list.attach_pdn_profile_list[cnt_pdn] =
      req_msg.attach_pdn_list[cnt_pdn];
  }

  subs_id = qmi_wdsi_resolve_client_subs(cl_sp);
  return_val = ps_sys_ioctl_ex( PS_SYS_TECH_3GPP,
                                PS_SYS_IOCTL_3GPP_SET_LTE_ATTACH_PDN_LIST,
                                (ps_sys_subscription_enum_type)subs_id,
                                (void *)&attach_pdn_list,
                                &ps_errno );
  
  if (-1 == return_val )
  {
    LOG_MSG_ERROR_1("Set LTE attach pdn list failed, ps_errno:%d ", ps_errno);

    switch (ps_errno)
    {                                   
      case DS_EOPNOTSUPP:
        errval = QMI_ERR_OP_DEVICE_UNSUPPORTED_V01;
        break;

      case DS_EDBOVERFLOW:
        errval = QMI_ERR_REQUESTED_NUM_UNSUPPORTED_V01;
        break;

      case DS_EINVAL:
        errval = QMI_ERR_INVALID_PROFILE_V01;
        break;

      default:
        errval = QMI_ERR_INTERNAL_V01;
        break;
    } 

    goto send_result;
  }

  send_result:
    QMI_WDSI_ENCODE_RESPONSE_MSG(QMI_WDS_SET_LTE_ATTACH_PDN_LIST_RESP_V01,
                               &resp_msg,
                               sizeof(resp_msg),
                               errval,
                               &response);
    return response;
} /* qmi_wds_set_lte_attach_pdn_list() */

/*===========================================================================
  FUNCTION QMI_WDS_GET_LTE_ATTACH_PDN_LIST()

  DESCRIPTION
    Retrieves lte attach pdn list by calling into the PS SYS layer

  PARAMETERS
    sp        : service provided state pointer
    cmd_buf_p : ptr to cmd buffer
    cl_sp     : client state pointer
    sdu_in    : incoming request

  RETURN VALUE
    ptr to response

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
dsm_item_type * qmi_wds_get_lte_attach_pdn_list
(
  void *            sp,
  void *            cmd_buf_p,
  void *            cl_sp,
  dsm_item_type **  sdu_in
)
{
  dsm_item_type *                     response;
  wds_get_lte_attach_pdn_list_resp_msg_v01 resp_msg;
  qmi_error_type_v01                  errval;
  int16                               ps_errno = 0;
  int                                 cnt_pdn;
  int16                               ps_result;

  ps_sys_3gpp_lte_attach_pdn_list_type  attach_pdn_list;
  uint32                               subs_id;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  response = NULL;
  errval = QMI_ERR_NONE_V01;
  
  memset(&attach_pdn_list, 0, sizeof(attach_pdn_list));
  memset(&resp_msg, 0, sizeof(resp_msg));

  subs_id = qmi_wdsi_resolve_client_subs(cl_sp);
  /*-------------------------------------------------------------------------
    Retrieve attach pdn list information by calling into PS Sys Layer
  -------------------------------------------------------------------------*/
  ps_result = ps_sys_ioctl_ex ( PS_SYS_TECH_3GPP,
                                PS_SYS_IOCTL_3GPP_GET_LTE_ATTACH_PDN_LIST,
                                (ps_sys_subscription_enum_type)subs_id,
                                (void *)&attach_pdn_list,
                                &ps_errno);
  if ( ps_result  < 0 )
  {
    LOG_MSG_ERROR_2("ps_sys_ioctl %d operation returned : (%d)",
                    PS_SYS_IOCTL_3GPP_GET_LTE_ATTACH_PDN_LIST, ps_errno);
    switch (ps_errno)
    {                                   
      case DS_NOMEMORY:
        errval = QMI_ERR_NO_MEMORY_V01;
        break;

      case DS_EOPNOTSUPP:
        errval = QMI_ERR_OP_DEVICE_UNSUPPORTED_V01;
        break;

      default:
        errval = QMI_ERR_INTERNAL_V01;
        break;
    } 
    goto send_result;
  }
  
  resp_msg.attach_pdn_list_len = 
    attach_pdn_list.attach_pdn_profile_list_num;
  if(resp_msg.attach_pdn_list_len > 0)
  {
    resp_msg.attach_pdn_list_valid = TRUE;
  }
  for (cnt_pdn = 0; cnt_pdn < attach_pdn_list.attach_pdn_profile_list_num; cnt_pdn++)
  {
    resp_msg.attach_pdn_list[cnt_pdn] = 
      attach_pdn_list.attach_pdn_profile_list[cnt_pdn];
  }
  
send_result:
  QMI_WDSI_ENCODE_RESPONSE_MSG(QMI_WDS_GET_LTE_ATTACH_PDN_LIST_RESP_V01,
                               &resp_msg,
                               sizeof(resp_msg),
                               errval,
                               &response);
  return response;
} /* qmi_wds_get_lte_attach_pdn_list() */

/*===========================================================================
  FUNCTION QMI_WDS_GET_LTE_MAX_ATTACH_PDN_LIST_NUM()

  DESCRIPTION
    Retrieves lte attach pdn list

  PARAMETERS
    sp        : service provided state pointer
    cmd_buf_p : ptr to cmd buffer
    cl_sp     : client state pointer
    sdu_in    : incoming request

  RETURN VALUE
    ptr to response

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
dsm_item_type * qmi_wds_get_lte_max_attach_pdn_list_num
(
  void *            sp,
  void *            cmd_buf_p,
  void *            cl_sp,
  dsm_item_type **  sdu_in
)
{
  dsm_item_type *                     response;
  wds_get_lte_max_attach_pdn_num_resp_msg_v01 resp_msg;
  qmi_error_type_v01                  errval;
  int16                               ps_errno = 0;
  int16                               ps_result;
  ps_sys_ioctl_3gpp_lte_max_attach_pdn_num_type  max_attach_pdn_num_info;
  uint32                               subs_id;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  response = NULL;
  errval = QMI_ERR_NONE_V01;

  memset(&resp_msg, 0, sizeof(resp_msg));
  subs_id = qmi_wdsi_resolve_client_subs(cl_sp);
  /*-------------------------------------------------------------------------
    Retrieve max attach pdn list number by calling into PS Sys Layer
  -------------------------------------------------------------------------*/
  ps_result = ps_sys_ioctl_ex ( PS_SYS_TECH_3GPP,
                                PS_SYS_IOCTL_3GPP_GET_LTE_MAX_ATTACH_PDN_NUM,
                                (ps_sys_subscription_enum_type)subs_id,
                                (void *)&max_attach_pdn_num_info,
                                &ps_errno);
  if (ps_result < 0)
  { 
    LOG_MSG_ERROR_2("ps_sys_ioctl %d operation returned : (%d)",
                    PS_SYS_IOCTL_3GPP_GET_LTE_MAX_ATTACH_PDN_NUM, ps_errno);
    switch (ps_errno)
    {                                   
      case DS_NOMEMORY:
        errval = QMI_ERR_NO_MEMORY_V01;
        break;

      case DS_EOPNOTSUPP:
        errval = QMI_ERR_OP_DEVICE_UNSUPPORTED_V01;
        break;

      default:
        errval = QMI_ERR_INTERNAL_V01;
        break;
    } 
    goto send_result;
  }
  
  resp_msg.max_attach_pdn_num = max_attach_pdn_num_info;
  resp_msg.max_attach_pdn_num_valid = TRUE;

send_result:

  QMI_WDSI_ENCODE_RESPONSE_MSG(QMI_WDS_GET_LTE_MAX_ATTACH_PDN_NUM_RESP_V01,
                               &resp_msg,
                               sizeof(resp_msg),
                               errval,
                               &response);
  return response;
} /* qmi_wds_get_lte_max_attach_pdn_list_num() */

/*===========================================================================
  FUNCTION QMI_WDS_SET_LTE_DATA_RETRY()

  DESCRIPTION
    Modifies the LTE Data Retry setting.

  PARAMETERS
    sp        : service provided state pointer
    cmd_buf_p : ptr to cmd buffer
    cl_sp     : client state pointer
    sdu_in    : incoming request

  RETURN VALUE
    ptr to response

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
dsm_item_type*  qmi_wds_set_lte_data_retry
(
  void *            sp,
  void *            cmd_buf_p,
  void *            cl_sp,
  dsm_item_type **  sdu_in
)
{
  dsm_item_type *	 response = NULL;
  wds_set_lte_data_retry_req_msg_v01 req_msg;
  wds_set_lte_data_retry_resp_msg_v01 resp_msg;
  qmi_error_type_v01	 errval = QMI_ERR_NONE_V01;
  
  int16 return_val = DSS_SUCCESS, ps_errno;

  ps_sys_ioctl_lte_data_retry_type           lte_data_retry_val;
  uint32                               subs_id;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  response = NULL;
  errval = QMI_ERR_NONE_V01;
  memset(&req_msg, 0, sizeof(req_msg));
  memset(&resp_msg, 0, sizeof(resp_msg));
  subs_id = qmi_wdsi_resolve_client_subs(cl_sp);

  memset(&lte_data_retry_val, 0, sizeof(lte_data_retry_val));
  errval = QMI_WDSI_DECODE_REQUEST_MSG(QMI_WDS_SET_LTE_DATA_RETRY_REQ_V01,
                                       sdu_in,
                                       &req_msg,
                                       sizeof(req_msg));
  if(errval != QMI_ERR_NONE_V01)
  {
    goto send_result;
  }
  
  lte_data_retry_val.lte_data_retry = req_msg.lte_data_retry;
  
  if ((lte_data_retry_val.lte_data_retry != TRUE) &&
      (lte_data_retry_val.lte_data_retry != FALSE))
  {
    errval = QMI_ERR_INVALID_ARG_V01;
    goto send_result;
  }

  /*-------------------------------------------------------------------------
    Call the ioctl and check result
  -------------------------------------------------------------------------*/
  return_val = ps_sys_ioctl_ex ( PS_SYS_TECH_3GPP,
                                 PS_SYS_IOCTL_3GPP_SET_LTE_DATA_RETRY,
                                 (ps_sys_subscription_enum_type)subs_id,
                                 &lte_data_retry_val,
                                 &ps_errno );

  if (return_val != DSS_SUCCESS)
  {
    LOG_MSG_ERROR_2("ps_sys_ioctl ret [%d] errno [%d]", return_val, ps_errno);
    switch (ps_errno)
    {
      case DS_NOMEMORY:
        errval = QMI_ERR_NO_MEMORY_V01;
        break;
      case DS_EOPNOTSUPP:
      case DS_EINVAL:
         errval = QMI_ERR_OP_DEVICE_UNSUPPORTED_V01;
      default:
        errval = QMI_ERR_INTERNAL_V01;
        break;
    }
  }

send_result:
  QMI_WDSI_ENCODE_RESPONSE_MSG(QMI_WDS_SET_LTE_DATA_RETRY_RESP_V01,
                               &resp_msg,
                               sizeof(resp_msg),
                               errval,
                               &response);
  return response;
} /* qmi_wds_set_lte_data_retry() */

/*===========================================================================
  FUNCTION QMI_WDS_GET_LTE_DATA_RETRY()

  DESCRIPTION
    Retrieves the LTE Data Retry setting.

  PARAMETERS
    sp        : service provided state pointer
    cmd_buf_p : ptr to cmd buffer
    cl_sp     : client state pointer
    sdu_in    : incoming request

  RETURN VALUE
    ptr to response

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
dsm_item_type*  qmi_wds_get_lte_data_retry
(
  void *            sp,
  void *            cmd_buf_p,
  void *            cl_sp,
  dsm_item_type **  sdu_in
)
{
  dsm_item_type *	 response = NULL;
  wds_get_lte_data_retry_resp_msg_v01 resp_msg;
  qmi_error_type_v01	 errval = QMI_ERR_NONE_V01;
  
  int16 return_val = DSS_SUCCESS, ps_errno;

  ps_sys_ioctl_lte_data_retry_type           lte_data_retry_val;
  uint32                               subs_id;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  response = NULL;
  subs_id = qmi_wdsi_resolve_client_subs(cl_sp);

  errval = QMI_ERR_NONE_V01;

  memset(&lte_data_retry_val, 0, sizeof(lte_data_retry_val));
  memset(&resp_msg, 0, sizeof(resp_msg));

  /*-------------------------------------------------------------------------
    Call the ioctl and check result
  -------------------------------------------------------------------------*/
  return_val = ps_sys_ioctl_ex ( PS_SYS_TECH_3GPP,
                                 PS_SYS_IOCTL_3GPP_GET_LTE_DATA_RETRY,
                                 (ps_sys_subscription_enum_type)subs_id,
                                 &lte_data_retry_val,
                                 &ps_errno );
 
  if (return_val != DSS_SUCCESS)
  {
    LOG_MSG_ERROR_2("ps_sys_ioctl ret [%d] errno [%d]", return_val, ps_errno);
    switch (ps_errno)
    {
      case DS_NOMEMORY:
        errval = QMI_ERR_NO_MEMORY_V01;
        break;
      case DS_EOPNOTSUPP:
      case DS_EINVAL:
         errval = QMI_ERR_OP_DEVICE_UNSUPPORTED_V01;
      default:
        errval = QMI_ERR_INTERNAL_V01;
        break;
    }
    goto send_result;
  }

  resp_msg.lte_data_retry_valid = TRUE;
  resp_msg.lte_data_retry = lte_data_retry_val.lte_data_retry;
  
send_result:
  QMI_WDSI_ENCODE_RESPONSE_MSG(QMI_WDS_GET_LTE_DATA_RETRY_RESP_V01,
                               &resp_msg,
                               sizeof(resp_msg),
                               errval,
                               &response);
  return response;
} /* qmi_wds_get_lte_data_retry() */
/*===========================================================================
  FUNCTION QMI_WDS_SET_LTE_ATTACH_TYPE()

  DESCRIPTION
    Modifies the LTE Attach Type setting.

  PARAMETERS
    sp        : service provided state pointer
    cmd_buf_p : ptr to cmd buffer
    cl_sp     : client state pointer
    sdu_in    : incoming request

  RETURN VALUE
    ptr to response

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
dsm_item_type*  qmi_wds_set_lte_attach_type
(
  void *            sp,
  void *            cmd_buf_p,
  void *            cl_sp,
  dsm_item_type **  sdu_in
)
{
  dsm_item_type *	 response = NULL;
  wds_set_lte_attach_type_req_msg_v01 req_msg;
  wds_set_lte_attach_type_resp_msg_v01 resp_msg;
  qmi_error_type_v01	 errval = QMI_ERR_NONE_V01;
 int16 return_val = DSS_SUCCESS, ps_errno;
  ps_sys_lte_attach_enum_type           lte_attach_type;
  uint32                               subs_id;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  response = NULL;

  errval = QMI_ERR_NONE_V01;
  subs_id = qmi_wdsi_resolve_client_subs(cl_sp);
  memset(&req_msg, 0, sizeof(req_msg));
  memset(&resp_msg, 0, sizeof(resp_msg));
  errval = QMI_WDSI_DECODE_REQUEST_MSG(QMI_WDS_SET_LTE_ATTACH_TYPE_REQ_V01,
                                       sdu_in,
                                       &req_msg,
                                       sizeof(req_msg));
  if(errval != QMI_ERR_NONE_V01)
  {
    goto send_result;
  }
  lte_attach_type = (ps_sys_lte_attach_enum_type)req_msg.lte_attach_type;
  /* Validate the input */
  if ((lte_attach_type != PS_SYS_LTE_ATTACH_TYPE_INITIAL) &&
      (lte_attach_type != PS_SYS_LTE_ATTACH_TYPE_HANDOFF))
  {
    errval = QMI_ERR_INVALID_ARG_V01;
    goto send_result;
  }

  /*-------------------------------------------------------------------------
    Call ps_sys_conf_set and check result
  -------------------------------------------------------------------------*/
  return_val = ps_sys_conf_set_ex ( PS_SYS_TECH_3GPP,
                                    PS_SYS_CONF_3GPP_LTE_ATTACH_TYPE,
                                    (ps_sys_subscription_enum_type)subs_id,
                                    &lte_attach_type,
                                    &ps_errno );

  if (return_val != DSS_SUCCESS)
  {
    LOG_MSG_ERROR_2("ps_sys_conf_set ret [%d] errno [%d]",
                    return_val, ps_errno);
    switch (ps_errno)
    {
      case DS_NOMEMORY:
        errval = QMI_ERR_NO_MEMORY_V01;
        break;
      case DS_EOPNOTSUPP:
      case DS_EINVAL:
         errval = QMI_ERR_OP_DEVICE_UNSUPPORTED_V01;
      default:
        errval = QMI_ERR_INTERNAL_V01;
        break;
    }
  }

send_result:
  QMI_WDSI_ENCODE_RESPONSE_MSG(QMI_WDS_SET_LTE_ATTACH_TYPE_RESP_V01,
                               &resp_msg,
                               sizeof(resp_msg),
                               errval,
                               &response);
  return response;
} /* qmi_wds_set_lte_attach_type() */

/*===========================================================================
  FUNCTION QMI_WDS_GET_LTE_ATTACH_TYPE()

  DESCRIPTION
    Retrieves the LTE Attach Type setting.

  PARAMETERS
    sp        : service provided state pointer
    cmd_buf_p : ptr to cmd buffer
    cl_sp     : client state pointer
    sdu_in    : incoming request

  RETURN VALUE
    ptr to response

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
dsm_item_type*  qmi_wds_get_lte_attach_type
(
  void *            sp,
  void *            cmd_buf_p,
  void *            cl_sp,
  dsm_item_type **  sdu_in
)
{
  dsm_item_type *	 response = NULL;
  wds_get_lte_attach_type_resp_msg_v01 resp_msg;
  qmi_error_type_v01	 errval = QMI_ERR_NONE_V01;
  
  int16 return_val = DSS_SUCCESS, ps_errno;
  ps_sys_lte_attach_enum_type           lte_attach_type;
  uint32                               subs_id;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  response = NULL;

  errval = QMI_ERR_NONE_V01;
  subs_id = qmi_wdsi_resolve_client_subs(cl_sp);

  memset(&lte_attach_type, 0, sizeof(lte_attach_type));
  memset(&resp_msg, 0, sizeof(resp_msg));

  /*-------------------------------------------------------------------------
    Call the ioctl and check result
  -------------------------------------------------------------------------*/
  return_val = ps_sys_conf_get_ex ( PS_SYS_TECH_3GPP,
                                    PS_SYS_CONF_3GPP_LTE_ATTACH_TYPE,
                                    (ps_sys_subscription_enum_type)subs_id,
                                    &lte_attach_type,
                                    &ps_errno );

  if (return_val != DSS_SUCCESS)
  {
    LOG_MSG_ERROR_2("ps_sys_conf_get ret [%d] errno [%d]",
                    return_val, ps_errno);
    switch (ps_errno)
    {
      case DS_NOMEMORY:
        errval = QMI_ERR_NO_MEMORY_V01;
        break;
      case DS_EOPNOTSUPP:
      case DS_EINVAL:
         errval = QMI_ERR_OP_DEVICE_UNSUPPORTED_V01;
      default:
        errval = QMI_ERR_INTERNAL_V01;
        break;
    }
    goto send_result;
  }

  resp_msg.lte_attach_type = (wds_lte_attach_type_enum_v01)lte_attach_type;
  resp_msg.lte_attach_type_valid =  TRUE;
send_result:
  QMI_WDSI_ENCODE_RESPONSE_MSG(QMI_WDS_GET_LTE_ATTACH_TYPE_RESP_V01,
                               &resp_msg,
                               sizeof(resp_msg),
                               errval,
                               &response);
  return response;
} /* qmi_wds_get_lte_attach_type() */

/*===========================================================================
  FUNCTION QMI_WDS_UPDATE_LTE_ATTACH_PDN_LIST_PROFILES()

  DESCRIPTION
    Updates the LTE attach PDN list profile params

  PARAMETERS
    sp        : service provided state pointer
    cmd_buf_p : ptr to cmd buffer
    cl_sp     : client state pointer
    sdu_in    : incoming request

  RETURN VALUE
    ptr to response

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
dsm_item_type * qmi_wds_update_lte_attach_pdn_list_profiles
(
  void *            sp,
  void *            cmd_buf_p,
  void *            cl_sp,
  dsm_item_type **  sdu_in
)
{
  uint32 subs_id = 0;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  subs_id = qmi_wdsi_resolve_client_subs(cl_sp);
  /* 3gpp Specific Function */
  return qmi_wds_profile_update_lte_attach_pdn_list_params(WDS_PROFILE_TYPE_3GPP, subs_id);

} /* qmi_wds_update_lte_attach_pdn_list_profiles() */
#endif /* defined(FEATURE_DATA_LTE)*/
