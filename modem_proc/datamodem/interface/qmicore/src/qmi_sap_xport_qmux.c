/*===========================================================================

                            Q M I _ S A P _ X P O R T _ Q M U X . C

DESCRIPTION

  The QMI Services QSAP interface file.
 
EXTERNALIZED FUNCTIONS
 
Copyright (c) 2012 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary.
===========================================================================*/
/*===========================================================================

                            EDIT HISTORY FOR FILE
   $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/qmicore/src/qmi_sap_xport_qmux.c#1 $ 
   $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
03/08/13    wc     Support QMUX powersave filtering
08/15/12    wc     Fix double memory free
06/26/12    wc     Reduce QSAP memory usage
06/11/12    wc     Support vendor service registration
===========================================================================*/

#include "comdef.h"
#include "customer.h"
#include <stringl/stringl.h>

#ifdef FEATURE_QSAP_SERVICE 

#include "stdint.h"
#include "qmi_sap.h"
#include "qmi_sap_xport.h"
#include "qmi_sap_target_ext.h"
#include "ds_qmux.h"
#include "ds_qmi_task.h"
#include "ds_qmi_svc.h"
#include "ds_Utils_DebugMsg.h"
#include "ps_system_heap.h"

#define QSAP_MAX_CLIDS          9

static boolean qmi_qsapi_is_valid_clid_cb(void *sp, uint8_t clid);
static void qmi_qsapi_toggle_clid_in_use(void *sp, uint8_t clid);
static void qmi_qsapi_get_next_valid_clid(void *sp, uint8_t *clid);
static uint8_t qmi_qsapi_alloc_clid_cb(void *sp);
static void qmi_qsapi_rx_cb
(
 void * sp,
 uint8_t clid,
 dsm_item_type * sdu_in
);
static uint32_t qsap_xport_addr_len(void);
static void qsap_xport_close(void *handle);
static qmi_sap_error qsap_xport_send
(
 void *handle,
 void *addr,
 uint8_t *buf,
 uint32_t len,
 void **client_data
 );
static qmi_sap_error qsap_xport_unreg
(
 void *handle,
 uint32_t service_id,
 uint32_t version
 );
static qmi_sap_error qsap_xport_reg
(
 void *handle,
 uint32_t service_id,
 uint32_t version
 );
static void *qsap_xport_open
(
 void *xport_data,
 void *xport,
 uint32_t max_rx_len
);

/*---------------------------------------------------------------------------
  Structure to maintain service state
---------------------------------------------------------------------------*/

typedef struct xport_handle
{
  qmi_common_svc_state_type  common; // must be first since qmux aliases

  void *xport;

  /* Bitmask of Active Clients */
  uint32_t active_clients[8];

  /* clid */
  uint8_t next_clid;
  
  /* QMUX instance info */
  qmi_instance_e_type qmux_instance;

  /* QMUX config structure */
  qmux_svc_config_type cfg;           /* QMUX config table */
}qmi_sap_xport_handle;

/* This Structure contains the function callbacks
   that QSAP Common code uses to communicate with
   the xport. This is a global structure */
qmi_sap_xport_ops_type qsap_qmux_ops = {
  qsap_xport_open,
  qsap_xport_reg,
  qsap_xport_unreg,
  qsap_xport_send,
  qsap_xport_close,
  qsap_xport_addr_len
};

/*===========================================================================
  FUNCTION QMI_QSAPI_GET_NEXT_CLIENT_SP()

  DESCRIPTION
    Gets the next client_sp
 
  PARAMETERS
    void *sp        State Pointer
    void *cl_sp     Client State Pointer

  RETURN VALUE
    void *          Next Client State Pointer

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static void *  qmi_qsapi_get_next_client_sp
(
  void *  sp,
  void *  cl_sp
)
{
  return NULL;
} /* qmi_qsapi_get_next_client_sp() */

/*===========================================================================
  FUNCTION QMI_QSAPI_RESET_CLIENT()

  DESCRIPTION
    Calls standard QMUX reset_client function
 
  PARAMETERS
    void *cl_sp_in    Client State Pointer

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static void qmi_qsapi_reset_client
(
  void * cl_sp_in
)
{
  return;
} /* qmi_qsapi_reset_client() */

/*===========================================================================
  FUNCTION QMI_QSAPI_TOGGLE_CLID_IN_USE()

  DESCRIPTION
    Set a particular clid in the active clients list to active or inactive
 
  PARAMETERS
    void *sp      Pointer to the xport handle
    uint8_t clid  Client ID

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static void qmi_qsapi_toggle_clid_in_use
(
  void *sp, 
  uint8_t clid
)
{
  qmi_sap_xport_handle *xh = (qmi_sap_xport_handle *)sp;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  uint8_t index = clid / 32;
  uint8_t offset = clid % 32;
  uint32_t bit = 0x01 << offset;

  xh->active_clients[index] ^= bit;
}/* qmi_qsapi_toggle_clid_in_use */

/*===========================================================================
  FUNCTION QMI_QSAPI_GET_NEXT_VALID_CLID()

  DESCRIPTION
   Search through the active clients list to find an inactive clid
 
  PARAMETERS
    void *sp       Pointer to the xport handle
    uint8_t *clid  Pointer to the client ID value

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static void qmi_qsapi_get_next_valid_clid
(
  void *sp, 
  uint8_t *clid
)
{
  qmi_sap_xport_handle *xh = (qmi_sap_xport_handle *)sp;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  *clid = xh->next_clid;
  while (qmi_qsapi_is_valid_clid_cb(sp, *clid))
  {
    if(*clid == ((xh->next_clid) - 1))
    {
      *clid = QMI_SVC_CLID_UNUSED;
      return;
    }
    (*clid)++;
    /*-----------------------------------------------------------------------
      After incrementing the next client ID to be assigned , ensure client id rolls 
      over to one (skip UNUSED = 0).

      NOTE this logic is premised on specific values for broadcast (255),
      unused (0) and clid field width (8 bits)
    -----------------------------------------------------------------------*/
    if(*clid == QMI_SVC_CLID_BROADCAST)
    *clid = QMI_SVC_CLID_UNUSED + 1;
  }
  xh->next_clid = *clid;
  return;
} /* qmi_qsapi_get_next_valid_clid */

/*===========================================================================
  FUNCTION QMI_QSAPI_RX_CB()

  DESCRIPTION
    Process QMUX header information and pass message to qmi_sap_common
  PARAMETERS
    void *sp                 Pointer to the xport handle
    uint8_t                  Client ID
    dsm_item_type *sdu_in    Message that was sent to the service

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static void qmi_qsapi_rx_cb
(
 void * sp, 
 uint8_t clid,
 dsm_item_type * sdu_in
 )
{
  qmi_sap_xport_handle *xh = (qmi_sap_xport_handle *)sp;
  uint32_t len;
  int rc;
  void *msg;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* linearize DSM items */
  len = dsm_length_packet(sdu_in);
  PS_SYSTEM_HEAP_MEM_ALLOC_ASSERT(msg, len, void*);
  if(msg)
  {
    len = dsm_pullup_long(&sdu_in, msg, len);  

    rc = qmi_sap_xport_recv(xh->xport,&clid,msg,len);
    if (rc != QMI_SAP_NO_ERR)
    {
      /* ERROR */
      LOG_MSG_ERROR_2("Error encountered receiving message from clid %d in QSAP! (%d)",
                      clid, rc);
      /* Send an error response */
    }
    PS_SYSTEM_HEAP_MEM_FREE(msg);
    //dsm_free_packet(&sdu_in);
  }

}/* qmi_qsapi_rx_cb */

/*===========================================================================
  FUNCTION QMI_QSAPI_ALLOC_CLID_CB()

  DESCRIPTION
    Allocate a clid that is currently not in use
 
  PARAMETERS
    void *sp       Pointer to xport handle

  RETURN VALUE
    Returns the client ID that was assigned, or 0 on an error

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static uint8_t qmi_qsapi_alloc_clid_cb
(
  void *sp
)
{
  uint8_t clid;
  qmi_sap_xport_handle *xh = (qmi_sap_xport_handle *)sp;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  qmi_qsapi_get_next_valid_clid(sp, &clid);
  if(clid != QMI_SVC_CLID_UNUSED)
  {
  if (qmi_sap_xport_connect(xh->xport, &clid) == QMI_SAP_NO_ERR)
  {
    qmi_qsapi_toggle_clid_in_use(sp,clid); 
  }else
  {
    clid = 0;
  }
  }
  return clid;
}/* qmi_qsapi_get_next_valid_clid */

/*===========================================================================
  FUNCTION QMI_QSAPI_IS_VALID_CLID_CB()

  DESCRIPTION
    Check if a clid is currently in use
 
  PARAMETERS
    void *sp     Pointer to xport handle
    uint8_t      Client ID

  RETURN VALUE
    True if the CLID is currently a valid client
    False otherwise

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static boolean qmi_qsapi_is_valid_clid_cb
(
  void *sp, 
  uint8_t clid
)
{
  qmi_sap_xport_handle *xh = (qmi_sap_xport_handle *)sp;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  uint8_t index = clid / 32;
  uint8_t offset = clid % 32;
  uint8_t valid = ((xh->active_clients[index]) >> offset) & 0x01;

  return valid;
}/* qmi_qsapi_is_valid_clid_cb */

/*===========================================================================
  FUNCTION QMI_QSAPI_DEALLOC_CLID_CB()

  DESCRIPTION
    Deallocates a clid and calls the appropriate
    disconnect function
  PARAMETERS
    void *sp     Pointer to xport handle
    uint8_t      Client ID

  RETURN VALUE
    True if clid deallocation was successful
    False otherwise

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static boolean qmi_qsapi_dealloc_clid_cb
(
  void *sp, 
  uint8_t clid
)
{
  qmi_sap_xport_handle *xh = (qmi_sap_xport_handle *)sp;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if (qmi_qsapi_is_valid_clid_cb(sp,clid))
  {
    if (qmi_sap_xport_disconnect(xh->xport, &clid) == QMI_SAP_NO_ERR)
    {
      qmi_qsapi_toggle_clid_in_use(sp,clid);   
    }else
    {
      return FALSE;
    }
  }
  return TRUE;
}/* qmi_qsapi_dealloc_clid_cb */

/*===========================================================================
  FUNCTION QMI_QSAPI_QMUX_CLOSED_CB()

  DESCRIPTION
    Deallocates all clids and calls the appropriate
    disconnect function
  PARAMETERS
    void *sp     Pointer to xport handle

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static void qmi_qsapi_qmux_closed_cb
(
  void *sp
)
{
  qmi_sap_xport_handle * xh = (qmi_sap_xport_handle *)sp;
  qmux_svc_info_type   * svc_info;
  uint8_t                clid;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if ( (xh == NULL) || (xh->common.qmux_svc_handle == NULL) )
  {
    LOG_MSG_ERROR_1("Invalid xport handle [%p] ", xh);
    return;
  }

  svc_info = (qmux_svc_info_type *)xh->common.qmux_svc_handle;

  // Deallocate all clients of this service
  clid = 0;
  do
  {
    if ( (clid != QMI_SVC_CLID_UNUSED) &&
         (clid != QMI_SVC_CLID_BROADCAST) &&
         qmi_qsapi_is_valid_clid_cb(sp, clid) )
    {
      if (!qmi_qsapi_dealloc_clid_cb(sp, clid))
      {
        LOG_MSG_ERROR_2("Failed to dealloc service [%d] clid [%d]",
                        svc_info->service_id, clid);
      }
      else
      {
        LOG_MSG_INFO2_2("Dealloc service [%d] clid [%d]",
                        svc_info->service_id, clid);
      }
    } 
  } while (clid++ != 0xFF);

}/* qmi_qsapi_qmux_closed_cb */

/*===========================================================================

                        EXTERNAL FUNCTION DEFINITIONS

===========================================================================*/

/*===========================================================================
  FUNCTION QSAP_XPORT_OPEN()

  DESCRIPTION
    Creates the xport data structure for handling communication on this XPORT
    *xport is the QMUX instance. Called from from qmi_sap_common layer

  PARAMETERS
    void *xport_data     Pointer to xport data, This hold the qmi instance
                          for this qmux xport
    void *xport          Pointer to xport information
    uint32_t max_rx_len  Maximum message size for the service

  RETURN VALUE
    A pointer to the xport handle

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static void *qsap_xport_open
(
 void *xport_data,
 void *xport,
 uint32_t max_rx_len
)
{
  qmi_sap_xport_handle *xh = NULL;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  PS_SYSTEM_HEAP_MEM_ALLOC(xh, sizeof(qmi_sap_xport_handle),
                           qmi_sap_xport_handle*);

  if(xh)
  {
    memset(xh,0,sizeof(qmi_sap_xport_handle));
    xh->xport = xport;
    xh->active_clients[0] = 0x01;
    /* store away xport_data before registering with qmux */
    xh->qmux_instance = (qmi_instance_e_type)xport_data;
    xh->next_clid = 1;
  }
  return xh;
}/* qsap_xport_open */

/*===========================================================================
  FUNCTION QSAP_XPORT_REG()

  DESCRIPTION
    Registers a service with qmux. Called from from qmi_sap_common layer

  PARAMETERS
    void *handle         Pointer to xport handle
    uint32_t service_id  Service ID
    uint32_t version     Major version of the service

  RETURN VALUE
    QMI_SAP_NO_ERR on success
    QMI_SAP_INTERNAL_ERR on failure

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_sap_error qsap_xport_reg
(
 void *handle,
 uint32_t service_id,
 uint32_t version
 )
{
  qmi_sap_xport_handle *xh = (qmi_sap_xport_handle *)handle;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  xh->common.next_clid = 1;

  xh->cfg.base_version.major = (version & 0xFF);
  xh->cfg.base_version.minor = (version >> 16);
  xh->cfg.addendum_version.major = 0;
  xh->cfg.addendum_version.minor = 0;

  xh->cfg.cbs.alloc_clid         = qmi_qsapi_alloc_clid_cb;
  xh->cfg.cbs.is_valid_clid      = qmi_qsapi_is_valid_clid_cb;
  xh->cfg.cbs.dealloc_clid       = qmi_qsapi_dealloc_clid_cb;
  xh->cfg.cbs.qmux_closed        = qmi_qsapi_qmux_closed_cb;
  xh->cfg.cbs.recv               = qmi_qsapi_rx_cb;
  xh->cfg.cbs.get_next_client_sp = qmi_qsapi_get_next_client_sp;
  xh->cfg.cbs.reset_client       = qmi_qsapi_reset_client;
  xh->cfg.cmd_hdlr_array  = NULL;
  xh->cfg.cmd_num_entries = 0;
  xh->cfg.sp              = xh;

  if (service_id < QMUX_SERVICE_MAX)
  {
    xh->common.qmux_svc_handle = qmux_reg_service(xh->qmux_instance, service_id, &xh->cfg);
  }
  else
  {
    LOG_MSG_ERROR_1("Unknown service ID %d", service_id);
    xh->common.qmux_svc_handle = NULL;
  }

  if(!xh->common.qmux_svc_handle)
  {
    LOG_MSG_ERROR_0 ("QSAP Call to qmux_reg_service failed!");
    return QMI_SAP_INTERNAL_ERR;
  }
  return QMI_SAP_NO_ERR;
}/* qsap_xport_reg */

/*===========================================================================
  FUNCTION QSAP_XPORT_UNREG()

  DESCRIPTION
    Unregisters a service from qmux

  PARAMETERS
    void *handle         Pointer to xport handle
    uint32_t service_id  Service ID
    uint32_t version     Service Major Version

  RETURN VALUE
    QMI_SAP_NO_ERR on success
    QMI_SAP_INTERNAL_ERR on failure

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_sap_error qsap_xport_unreg
(
 void *handle,
 uint32_t service_id,
 uint32_t version
 )
{
  /* No way to actually unregister a service from QMUX */
  return QMI_SAP_NO_ERR;
}/* qsap_xport_unreg */

/*===========================================================================
  FUNCTION QSAP_XPORT_SEND()

  DESCRIPTION
    Sends a QMUX message to a client, called from qmi_sap_common layer

  PARAMETERS
    void *handle        Pointer to xport handle
    void *addr          Pointer to address of message recipient
    uint8_t *buf        Pointer to message buffer
    uint32_t *len       Length of message being sent
    void **client_data  Pointer to client data

  RETURN VALUE
    QMI_SAP_NO_ERR on success
    QMI_SAP_INTERNAL_ERR on failure

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_sap_error qsap_xport_send
(
 void *handle,
 void *addr,
 uint8_t *buf,
 uint32_t len,
 void **client_data
 )
{
  qmi_sap_xport_handle *xh = (qmi_sap_xport_handle *)handle;
  dsm_item_type *msg = NULL;
  uint16_t msg_id;
  uint8_t msg_type;
  qmux_service_e_type svc_id;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if ( (xh == NULL) || (xh->common.qmux_svc_handle == NULL) ||
       (len < 5) )
  {
    LOG_MSG_ERROR_2("Invalid QSAP handle 0x%p or len %d", handle, len);
    ASSERT(0);
    return QMI_SAP_INTERNAL_ERR;
  }

  svc_id = ((qmux_svc_info_type *)xh->common.qmux_svc_handle)->service_id;

  memscpy(&msg_type, sizeof(uint8_t), buf, 1);
  memscpy(&msg_id, sizeof(uint16_t), buf + 3, 2);

  if (qmi_svc_filter_message(xh->qmux_instance, svc_id, msg_type, msg_id))
  {
    LOG_MSG_INFO2_3("Filtered QMI message: type=%d svc=%d msg=%d",
                    msg_type, svc_id, msg_id);
    return QMI_SAP_NO_ERR;
  }

  if(dsm_pushdown_tail_long(&msg, buf, len, DSM_DS_SMALL_ITEM_POOL) != len)
  {
    ASSERT(0);
    dsm_free_packet(&msg);
    return QMI_SAP_INTERNAL_ERR;
  }
  qmux_send(xh->common.qmux_svc_handle, msg, *((uint8_t *)addr));
  msg = NULL;
  return QMI_SAP_NO_ERR;
}/* qsap_xport_send */

/*===========================================================================
  FUNCTION QSAP_XPORT_CLOSE()

  DESCRIPTION
    Closes the connection to QMUX, destroying the xport data structure
    called from qmi_sap_common layer

  PARAMETERS
    void *handle    Pointer to xport handle

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static void qsap_xport_close
(
 void *handle
 )
{
  if (handle)
  {
    PS_SYSTEM_HEAP_MEM_FREE(handle);
  }
}/* qsap_xport_close */

/*===========================================================================
  FUNCTION QSAP_XPORT_ADDR_LEN()

  DESCRIPTION
    Gets the length of the field used for addressing a client on qmux

  PARAMETERS
    None

  RETURN VALUE
    Size of the address used for clients in this xport

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static uint32_t qsap_xport_addr_len
(
 void
 )
{
  return 1;
}/* qsap_xport_addr_len */


#endif /* FEATURE_QSAP_SERVICE */

