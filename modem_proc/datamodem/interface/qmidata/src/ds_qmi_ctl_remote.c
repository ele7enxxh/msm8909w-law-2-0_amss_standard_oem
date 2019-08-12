/*==========================================================================*/
/*!
  @file
  ds_qmi_ctl_remote.c

  @brief
  This file provides implementation for remoting certain QMI CTL
  messages to a different modem. 

  @details
  The following external methods are supported.

  qmi_ctl_remote_init()
    Initialize the QMI control remoting service
  
  qmi_ctl_remote_set_data_format()
    Sets the data format on the remote port.

  qmi_ctl_remote_release()
    Release the QMI control remoting services

  Copyright (c) 2011 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary
*/
/*=========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/qmidata/src/ds_qmi_ctl_remote.c#1 $
  $DateTime: 2016/12/13 08:00:03 $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
04/06/11    hm     Created module
===========================================================================*/

/*===========================================================================

                            INCLUDE FILES FOR MODULE

===========================================================================*/

#include "datamodem_variation.h"
#include "comdef.h"
#include "ds_qmi_ctl_remote.h"
#include "ds_Utils_DebugMsg.h"

#ifdef FEATURE_DATA_QMI_CTL_REMOTE

#include "ds_qmuxi.h"
#include "ds_qmi_ctl.h"

#include "qmi.h"
#include "qmi_i.h"
#include "sio.h"

/*===========================================================================

                           INTERNAL DATA DEFINITIONS 

===========================================================================*/

/*---------------------------------------------------------------------------
  QMI_CTL_REMOTE instance state structure
---------------------------------------------------------------------------*/
typedef struct
{
  qmi_instance_e_type     instance;
  sio_port_id_type        qmi_device_id;  
  qmi_client_handle_type  qmimsglib_handle;
  char                   *qmimsglib_conn_id;
} qmi_ctl_remotei_state_type;

static qmi_ctl_remotei_state_type qmi_ctl_remotei_state[QMI_INSTANCE_MAX];


/*---------------------------------------------------------------------------
  Mapping of remoted ports to QMIMSGLIB connection IDs. If remoting support
  is required over any port, this table should be updated to indicate 
  appropriately.
---------------------------------------------------------------------------*/
typedef struct
{
  sio_port_id_type         qmi_device_id;
  char                    *qmimsglib_conn_id;
} qmi_ctl_remotei_ports_type; 


static qmi_ctl_remotei_ports_type qmi_ctl_remotei_ports[] = 
{
  { SIO_PORT_SDIO_MUX_A2_RMNET_TETH_0, QMI_PORT_RMNET_1 }
};

#define QMI_CTL_REMOTEI_NUM_REMOTED_PORTS                                   \
  sizeof(qmi_ctl_remotei_ports)/sizeof(qmi_ctl_remotei_ports_type)


/*===========================================================================

                         INTERNAL FUNCTION DEFINITIONS

===========================================================================*/
static char *qmi_ctl_remotei_get_qmimsglib_conn_id 
(
  sio_port_id_type                qmi_device_id
)
{
  int index;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  for (index = 0; index < QMI_CTL_REMOTEI_NUM_REMOTED_PORTS; index++)
  {
    if (qmi_ctl_remotei_ports[index].qmi_device_id == qmi_device_id)
    {
      return qmi_ctl_remotei_ports[index].qmimsglib_conn_id;
    }
  }

  return NULL;

}



/*===========================================================================

                         EXTERNAL FUNCTION DEFINITIONS

===========================================================================*/
qmi_instance_e_type  
qmi_ctl_remote_init
(
  qmi_instance_e_type                qmi_inst
)
{
  qmi_ctl_remotei_state_type        *inst_p = NULL;
  int                                ret_val;
  int                                qmi_err_code;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  LOG_MSG_FUNCTION_ENTRY_1 ("QMI CTL REMOTE INIT, inst %d", qmi_inst);

  if (QMI_INSTANCE_MIN > qmi_inst || QMI_INSTANCE_MAX <= qmi_inst)
  {
    LOG_MSG_ERROR_1 ("QMI instance %d out of range", qmi_inst);
    return QMI_INSTANCE_MAX;
  }

  inst_p = &qmi_ctl_remotei_state[qmi_inst];
  memset (inst_p, 0, sizeof (qmi_ctl_remotei_state_type));

  inst_p->qmi_device_id = qmuxi_get_qmi_control_device_id(qmi_inst);
  if (SIO_PORT_NULL == inst_p->qmi_device_id)
  {
    LOG_MSG_INFO1_1 ("Unable to find device ID for inst %d", qmi_inst);
    return QMI_INSTANCE_MAX;
  }

  inst_p->qmimsglib_conn_id = 
    qmi_ctl_remotei_get_qmimsglib_conn_id (inst_p->qmi_device_id);
  if (NULL == inst_p->qmimsglib_conn_id)
  {
    LOG_MSG_INFO1_2 ("Unable to get QMIMSGLIB conn id for inst %d, device 0x%x",
                     qmi_inst, inst_p->qmi_device_id);
    return QMI_INSTANCE_MAX;
  }
  
  inst_p->qmimsglib_handle = qmi_init (NULL, NULL);
  if (inst_p->qmimsglib_handle < 0)
  {
    LOG_MSG_ERROR_1 ("Error %d initializing QMIMGSLIB",
                     inst_p->qmimsglib_handle);
    return QMI_INSTANCE_MAX;
  }

  ret_val = qmi_connection_init (inst_p->qmimsglib_conn_id,
                                 &qmi_err_code);
  if (0 != ret_val)
  {
    LOG_MSG_ERROR_2 ("Error %d, %d initializing qmi connection",
                     ret_val, qmi_err_code);
    return QMI_INSTANCE_MAX;
  } 

  inst_p->instance = qmi_inst;
  
  LOG_MSG_FUNCTION_EXIT_1 ("QMI CTL REMOTE INIT complete, inst %d", qmi_inst);

  return inst_p->instance;
}


void qmi_ctl_remote_set_data_format
(
  qmi_instance_e_type                qmi_inst,
  uint16                             link_prot,
  uint8                              qos_hdr_enabled,
  boolean                            ul_tlp
)
{
  qmi_ctl_remotei_state_type        *inst_p = NULL;
  qmi_data_format_qos_hdr_state_type qmimsglib_qos_hdr_format;
  int                                qmi_err_code;
  int                                ret_val;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  LOG_MSG_FUNCTION_ENTRY_3 ("SET DATA FORMAT: inst %d, link prot %d, qos %d",
                            qmi_inst, link_prot, qos_hdr_enabled);

  /* UL TLP is not supported by QMIMSGLIB currently, ignore */
  (void) ul_tlp;

  if (QMI_INSTANCE_MIN > qmi_inst || QMI_INSTANCE_MAX <= qmi_inst)
  {
    LOG_MSG_INFO1_1 ("Inst %d out of range", qmi_inst);
    return;
  }

  inst_p = &qmi_ctl_remotei_state[qmi_inst];
  if (inst_p->instance != qmi_inst || NULL == inst_p->qmimsglib_conn_id)
  {
    LOG_MSG_INFO1_1 ("Inst %d not in use, ignore set_data_format", qmi_inst);
    return;
  }

  if (0 == qos_hdr_enabled)
  {
    qmimsglib_qos_hdr_format = QMI_DATA_FORMAT_WITHOUT_QOS_HDR;
  }
  else
  {
    qmimsglib_qos_hdr_format = QMI_DATA_FORMAT_WITH_QOS_HDR;
  }

  ret_val = qmi_set_port_data_format (inst_p->qmimsglib_conn_id,
                                      qmimsglib_qos_hdr_format,
                                      &link_prot,
                                      &qmi_err_code);
  if (0 != ret_val)
  {
    LOG_MSG_ERROR_3 ("Unable to set data format, inst %d, ret %d, err %d",
                     qmi_inst, ret_val, qmi_err_code);
    return;
  }
  
  return;
}

void qmi_ctl_remote_release
(
  void 
)
{
  qmi_ctl_remotei_state_type        *inst_p = NULL;
  int                                index;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  LOG_MSG_FUNCTION_ENTRY_0 ("QMI CTL Remote Release");

  for (index = 0; index < QMI_INSTANCE_MAX; index++)
  {
    inst_p = &qmi_ctl_remotei_state[index];
    (void) qmi_release(inst_p->qmimsglib_handle);
  }

  return;
}


#else  /* ifndef FEATURE_DATA_QMI_CTL_REMOTE */

/*---------------------------------------------------------------------------
  Providing stubs if the FEATURE_DATA_QMI_CTL_REMOTE is not defined.
---------------------------------------------------------------------------*/
qmi_instance_e_type 
qmi_ctl_remote_init
(
  qmi_instance_e_type  qmi_inst
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  LOG_MSG_INFO2_0 ("QMI CTL REMOTE not supported");

  (void) qmi_inst;
  return QMI_INSTANCE_MAX;
}

void qmi_ctl_remote_set_data_format
(
  qmi_instance_e_type                qmi_inst,
  uint16                             link_prot,
  uint8                              qos_hdr_enabled,
  boolean                            ul_tlp
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  LOG_MSG_INFO2_0 ("QMI CTL REMOTE not supported");

  (void) qmi_inst;
  (void) link_prot;
  (void) qos_hdr_enabled;
  (void) ul_tlp;
  return;
}

void qmi_ctl_remote_release
(
  void 
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  LOG_MSG_INFO2_0 ("QMI CTL REMOTE not supported");
  return;
}

#endif /* FEATURE_DATA_QMI_CTL_REMOTE */


