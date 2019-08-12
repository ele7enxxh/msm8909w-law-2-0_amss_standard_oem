/*===========================================================================

                         Q M I _ C M D _ P R O C E S S . C

DESCRIPTION

 The Qualcomm MSM Interface(QMI) command process header file.

EXTERNALIZED FUNCTIONS

        
Copyright (c) 2012-2015 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
===========================================================================*/
/*===========================================================================

                      EDIT HISTORY FOR FILE

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/qmicore/src/qmi_cmd_process.c#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when        who    what, where, why
--------    ---    ---------------------------------------------------------- 
09/27/13    wc     Support dynamic port configuration
01/31/11    rk    Created module 
===========================================================================*/

/*===========================================================================

                          INCLUDE FILES FOR MODULE

===========================================================================*/

#include "comdef.h"
#include "customer.h"

#include "modem_mem.h"
#include "amssassert.h"

#include "dcc_task_defs.h"
#include "dcc_task_svc.h"

#include "qmi_svc_defs.h"
#include "qmi_cmd_process.h"

#include "qmi_frameworki.h"
#include "ds_qmi_svc.h"
#include "ds_qmi_ctl.h"
#include "ds_qmux.h"
#include "ds_rmnet_xport.h"
#include "ds_qmi_svc_ext.h"

#include "ds_qmi_cflog.h"
#include "ds_Utils_DebugMsg.h"

/*===========================================================================

                               DEFINITIONS

===========================================================================*/
/*===========================================================================
  FUNCTION QMI_CMD_RECV_LEGACY_SVC_REQ()

  DESCRIPTION
    Send the legacy service request to the DCC Task. 

  PARAMETERS
    svci      : service info for the incoming request
    clid      : clinet ID where the request come from.
    qmux_sdu  : Raw QMUX SDU

  RETURN VALUE
    None
    
  DEPENDENCIES
    None

  SIDE EFFECTS
    None
==========================================================================*/
boolean qmi_cmd_recv_legacy_svc_req
( 
  qmux_svc_info_type * svci,
  uint8                qmux_hdr_clid, 
  dsm_item_type *      sdu_in
)
{
  qmi_cmd_recv_legacy_svc_req_type *  cmd_ptr;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ASSERT(svci);
  ASSERT(sdu_in);

  /*-------------------------------------------------------------------------
    Check if the sending client is valid.  

    If a callback is registered, call it to find out.
    If no callback is registered, call the common service library api to
      find out. 
  -------------------------------------------------------------------------*/
  if( ((svci->cfg.cbs.is_valid_clid) &&
      (FALSE == (svci->cfg.cbs.is_valid_clid( svci->cfg.sp, qmux_hdr_clid )))) ||
      (!(svci->cfg.cbs.is_valid_clid) &&
      (FALSE == (qmi_svc_is_valid_clid( svci->cfg.sp, qmux_hdr_clid )))) )
  {
    dsm_free_packet (&sdu_in);
    return FALSE;
  }
  cmd_ptr = (qmi_cmd_recv_legacy_svc_req_type *) modem_mem_alloc(
                                         sizeof(qmi_cmd_recv_legacy_svc_req_type),
                                         MODEM_MEM_CLIENT_DATACOMMON );
  if(NULL == cmd_ptr)
  {
    DATA_ERR_FATAL("QMI can't get modem memory heap allocation");
    dsm_free_packet (&sdu_in);
    return FALSE;
  }
  memset(cmd_ptr, 0, sizeof(qmi_cmd_recv_legacy_svc_req_type));
  cmd_ptr->clid = qmux_hdr_clid;
  cmd_ptr->svc_handle = svci;
  cmd_ptr->sdu_in = sdu_in;
  dcc_send_cmd_ex(DCC_QMI_RECV_LEGACY_SERVICES_REQ_CMD, (void *)cmd_ptr);

  return TRUE;

} /* qmi_cmd_recv_legacy_svc_req */

/*===========================================================================
  FUNCTION QMI_CMD_PROCESS_LEGACY_SVC_RESPONSE()

  DESCRIPTION
    Process the dcc response and send to qmux to client.

  PARAMETERS
    svci      : service info for the incoming request
    clid      : clinet ID where the request come from.
    qmux_pdu  : Raw QMUX PDU

  RETURN VALUE
    None
    
  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
void qmi_cmd_process_legacy_svc_response
(
  qmux_svc_info_type * svci,
  uint8                qmux_hdr_clid, 
  dsm_item_type *      qmux_pdu
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  ASSERT(svci);
  ASSERT(qmux_pdu);

  qmux_send( svci, qmux_pdu, qmux_hdr_clid );

} /* qmi_cmd_process_legacy_svc_response() */

/*===========================================================================
  FUNCTION QMI_PROCESS_CMD()

  DESCRIPTION
    This function processes a QMI DCC cmd.

    It is called by the serving task main loop and will dispatch the
    associated event handler function.

  PARAMETERS
    cmd          :  QMI command to be processed
    user_data_ptr:  private data buffer containing the QMI command
                    information.

  RETURN VALUE
    None

  DEPENDENCIES
    QMI module must already have been initialized

  SIDE EFFECTS
    None
===========================================================================*/
void qmi_process_cmd
(
  qmi_cmd_enum_type        cmd,
  void                   * user_data_ptr
)
{
  qmi_cmd_data_buf_type  * cmd_ptr;
  uint32                   ep_id;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  cmd_ptr = (qmi_cmd_data_buf_type *)user_data_ptr;
  ASSERT (cmd_ptr);
  ASSERT (cmd == QMI_CMD);
  ASSERT (cmd_ptr->cmd.qmi.id < QMI_CMD_MAX);

  switch (cmd_ptr->cmd.qmi.id)
  {
    case QMI_CMD_PROCESS_POWER_EVT:
      qmi_svc_pwrsave_evt_hdlr( cmd_ptr->cmd.qmi.data.qmi_svc_pwrsave.state_handle,
                                cmd_ptr->cmd.qmi.data.qmi_svc_pwrsave.data );
      break;

    case QMI_CMD_CTL_SEND_SYNC_IND:
      qmi_ctl_sync_ind_cmd_hdlr(cmd_ptr->cmd.qmi.data.ctl.sp);
      break;
#ifdef FEATURE_DATA_QMI_SVC_AVAILABILITY
     case QMI_CMD_CTL_SVC_AVAILABLE:
       qmi_ctl_process_service_availability
                          (cmd_ptr->cmd.qmi.data.ctl_avail_ind.service_id,
                           cmd_ptr->cmd.qmi.data.ctl_avail_ind.qmi_inst,
                           cmd_ptr->cmd.qmi.data.ctl_avail_ind.is_legacy_svc );
      break;
#endif /*FEATURE_DATA_QMI_SVC_AVAILABILITY*/
    case QMI_CMD_GET_CLIENT_ID_RESP:
    case QMI_CMD_RELEASE_CLIENT_ID_RESP:
      qmi_framework_process_client_registration_result( 
                                    cmd_ptr->cmd.qmi.data.fw_cl_resp.result, 
                                    &cmd_ptr->cmd.qmi.data.fw_cl_resp.cl_reg_info,
                                    cmd_ptr->cmd.qmi.id,
                                    cmd_ptr->cmd.qmi.data.fw_cl_resp.errval);
      break;
    case QMI_CMD_FRAMEWORK_LEGACY_SVC_RESPONSE:
      qmi_cmd_process_legacy_svc_response( 
                                    cmd_ptr->cmd.qmi.data.legacy_svc_resp.svc_handle,
                                    cmd_ptr->cmd.qmi.data.legacy_svc_resp.clid,
                                    cmd_ptr->cmd.qmi.data.legacy_svc_resp.sdu_in);
      break;
    case QMI_CMD_FRAMEWORK_SEND_RESPONSE:
    case QMI_CMD_FRAMEWORK_SEND_IND:
      (void) qmi_framework_process_svc_send( cmd_ptr->cmd.qmi.data.fw.user_data_ptr,
                                      cmd_ptr->cmd.qmi.id );
      break;
    case QMI_CMD_RMNET_OPEN_INSTANCE:
    case QMI_CMD_RMNET_CLOSE_INSTANCE:
      qmux_process_rmnet_inst_open_close_result( 
              cmd_ptr->cmd.qmi.data.rmnet_inst.result,
              (qmi_cmd_id_e_type)cmd_ptr->cmd.qmi.id,
              cmd_ptr->cmd.qmi.data.rmnet_inst.ep_id);
      break;
    case QMI_CMD_DTR_STATUS:
      // Send DTR status to RMNET
      ep_id = qmux_get_default_ep_id_from_inst(
                 cmd_ptr->cmd.qmi.data.dtr_status.qmi_inst);
      if (ep_id > 0)
      {
        rmnet_phys_xport_dtr_cmd(ep_id,
              cmd_ptr->cmd.qmi.data.dtr_status.asserted);
      }

      // Cleanup QMI if DTR is LOW
      if (cmd_ptr->cmd.qmi.data.dtr_status.asserted == FALSE)
      {
        /* This is similar to qmux_link_down, but without fluashing RX wmk.
         * The RX wmk is already flushed in DTR callback.
         */
        qmux_clean_instance_resources(cmd_ptr->cmd.qmi.data.dtr_status.qmi_inst);
      }
      break;
    default:
      LOG_MSG_INFO2_1 ("Ignoring QMI cmd %d", cmd_ptr->cmd.qmi.id);
      break;
  }
} /* qmi_process_cmd() */



