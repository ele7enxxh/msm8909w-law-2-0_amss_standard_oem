/*===========================================================================

                             D S S _ N E T _ M G R . C

DESCRIPTION
  This file contains functions for bringing up and tearing down the network
  interface.

EXTERNALIZED FUNCTIONS
  dss_net_mgr_init()
    Initializes the module

  dss_net_mgr_bring_up_net_iface()
    Bring up the desired network interface

  dss_net_mgr_tear_down_net_iface()
    Tear down the network interface

Copyright (c) 2007-2012 Qualcomm Technologies Incorporated. 
All Rights Reserved.
Qualcomm Confidential and Proprietary
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/dss/src/dss_net_mgr.c#1 $
  $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
01/03/12    ea     Move the iface events registration outside net_mgr code.
10/20/10    ea     Add to the bring-up function an option to open a netlib
                   without opening the network. In addition, add option for
                   registration to iface events.
05/22/09    kk     Q6 compiler warning fixes.
11/14/08    pp     Compile warning fixes.
02/14/07    ss     Created.
===========================================================================*/

/*===========================================================================

                      INCLUDE FILES FOR MODULE

===========================================================================*/
#include "datamodem_variation.h"
#include "comdef.h"
#include "target.h"
#include "customer.h"

#include "msg.h"
#include "err.h"
#include "amssassert.h"
#include "ps_mem_ext.h"
#include "ps_mem.h"
#include "ps_svc.h"
#include "dssocket.h"
#include "dss_net_mgr.h"
#include "ps_handle_mgr.h"
#include "ds_Utils_DebugMsg.h"



/*===========================================================================

                        LOCAL DATA DEFINITIONS

===========================================================================*/
/*---------------------------------------------------------------------------
  Tuning the number of dss net manager callback buffers needed by this module
---------------------------------------------------------------------------*/
#define DSS_NET_MGR_CB_BUF_SIZE  ((sizeof(dss_net_mgr_cb_type) + 3) & ~3)

#ifndef FEATURE_DATA_PS_LOW_MEM_CHIPSET

#define DSS_NET_MGR_CB_BUF_NUM        5
#define DSS_NET_MGR_CB_BUF_HIGH_WM    4
#define DSS_NET_MGR_CB_BUF_LOW_WM     2

#else

#define DSS_NET_MGR_CB_BUF_NUM        3
#define DSS_NET_MGR_CB_BUF_HIGH_WM    3
#define DSS_NET_MGR_CB_BUF_LOW_WM     1
#endif /* FEATURE_DATA_PS_LOW_MEM_CHIPSET */

/*----------------------------------------------------------------------------
  Allocate memory to hold dss_net_mgr along with ps_mem header
----------------------------------------------------------------------------*/
static int dss_net_mbr_cb_buf_mem[PS_MEM_GET_TOT_SIZE_OPT
                                  (
                                   DSS_NET_MGR_CB_BUF_NUM,
                                   DSS_NET_MGR_CB_BUF_SIZE
                                  )];

#ifdef FEATURE_DATA_PS_MEM_DEBUG
/*----------------------------------------------------------------------------
  Array of pointers used to facilitate easy debugging.  The first one points
  to the ps_mem header and the latter ponts to ps_dss_net_mgr_buf
----------------------------------------------------------------------------*/                        
static ps_mem_buf_hdr_type * dss_net_mgr_cb_buf_hdr[DSS_NET_MGR_CB_BUF_NUM];
static dss_net_mgr_cb_type * dss_net_mgr_cb_buf_ptr[DSS_NET_MGR_CB_BUF_NUM];
#endif /* FEATURE_DATA_PS_MEM_DEBUG */

/*===========================================================================

                             INTERNAL FORWARD DECLARATIONS

===========================================================================*/
static void dss_net_mgri_net_ev_cback
(
  int16                dss_net_handle,
  dss_iface_id_type    iface_id,
  int16                dss_errno,
  void               * user_data_ptr
);

static void dss_net_mgri_process_net_ev_cback
(
  ps_cmd_enum_type    cmd_name,
  void              * user_data_ptr
);
/*===========================================================================

                      INTERNAL FUNCTION DEFINITIONS

===========================================================================*/
/*===========================================================================
FUNCTION DSS_NET_MGRI_NET_EV_CBACK()

DESCRIPTION
  This callback function handles the net events posted by DSS module

PARAMETERS
  dss_net_handle : handle to the app that brought up the interface
  iface_id       : iface_id of the interface for which event is posted
  dss_errno      : errno that indicates the network status
  user_data_ptr  : user data that is passed back

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
static void dss_net_mgri_net_ev_cback
(
  int16                dss_net_handle,
  dss_iface_id_type    iface_id,
  int16                dss_errno,
  void               * user_data_ptr
)
{
  dss_net_mgr_cb_type  * net_mgr_cb_ptr;
  ps_cmd_enum_type       cmd;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  LOG_MSG_INFO2_3("dss_net_mgri_net_ev_cback(): "
                  "Recvd event %d for Net Mgr 0x%p on iface 0x%x",
                  dss_errno, user_data_ptr, iface_id);

  if (user_data_ptr == NULL)
  {
    DATA_ERR_FATAL("dss_net_mgri_net_ev_cback(): NULL user data");
    return;
  }

  if (!ps_mem_is_valid( user_data_ptr, PS_MEM_DSS_NET_MGR_CB_TYPE))
  {
    LOG_MSG_ERROR_1("dss_net_mgri_net_ev_cback(): "
                    "Invalid user data 0x%p", user_data_ptr);
    ASSERT(0);
    return;
  }

  net_mgr_cb_ptr = (dss_net_mgr_cb_type *) user_data_ptr;
  if (net_mgr_cb_ptr->dss_net_handle != dss_net_handle)
  {
    LOG_MSG_ERROR_2("dss_net_mgri_net_ev_cback(): "
                    "Invalid user data, dss_net_handle is %d, exp %d",
                    dss_net_handle, net_mgr_cb_ptr->dss_net_handle);
    ASSERT(0);
    return;
  }

  /*-------------------------------------------------------------------------
    Store the iface id returned in the net mgr control block.
  -------------------------------------------------------------------------*/
  net_mgr_cb_ptr->iface_id = iface_id;

  if (dss_errno == DS_ENETISCONN)
  {
    cmd = PS_DSS_NET_MGR_NET_UP_CMD;
  }
  else if (dss_errno == DS_ENETNONET)
  {
    cmd = PS_DSS_NET_MGR_NET_DOWN_CMD;
  }
  else
  {
    LOG_MSG_ERROR_1("dss_net_mgri_net_ev_cback(): "
                    "Unexpected errno %d", dss_errno);
    ASSERT(0);
    return;
  }

  ps_send_cmd( cmd, net_mgr_cb_ptr);
  return;

} /* dss_net_mgri_net_ev_cback() */



/*===========================================================================
FUNCTION DSS_NET_MGRI_PROCESS_NET_EV_CBACK()

DESCRIPTION
  This command handler processes net events posted by DSS module

PARAMETERS
  cmd_name      : cmd that is posted by PS
  user_data_ptr : user data that is passed back

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
static void dss_net_mgri_process_net_ev_cback
(
  ps_cmd_enum_type    cmd_name,
  void              * user_data_ptr
)
{
  dss_net_mgr_cb_type              * net_mgr_cb_ptr;
  dss_net_mgr_net_event_enum_type    net_mgr_event;
  int16                              dss_errno;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  LOG_MSG_INFO2_1("dss_net_mgri_process_net_ev_cback(): "
                  "Processing Net mgr cmd %d", (int)cmd_name);

  if (user_data_ptr == NULL)
  {
    DATA_ERR_FATAL("dss_net_mgri_process_net_ev_cback(): NULL user data");
    return;
  }

  if (!ps_mem_is_valid( user_data_ptr, PS_MEM_DSS_NET_MGR_CB_TYPE))
  {
    LOG_MSG_ERROR_1("dss_net_mgri_process_net_ev_cback(): "
                    "Invalid user data 0x%p", user_data_ptr);
    return;
  }

  net_mgr_cb_ptr = (dss_net_mgr_cb_type *) user_data_ptr;

  if (cmd_name == PS_DSS_NET_MGR_NET_UP_CMD)
  {
    net_mgr_event = DSS_NET_MGR_NET_EVENT_UP;
  }
  else if (cmd_name == PS_DSS_NET_MGR_NET_DOWN_CMD)
  {
    net_mgr_event = DSS_NET_MGR_NET_EVENT_DOWN;
  }
  else
  {
    LOG_MSG_ERROR_1("dss_net_mgri_process_net_ev_cback(): "
                    "Unexpected cmd %d", cmd_name);
    ASSERT(0);
    return;
  }

  net_mgr_cb_ptr->cback_f_ptr( net_mgr_cb_ptr->net_mgr_handle,
                               net_mgr_cb_ptr->iface_id,
                               net_mgr_event,
                               net_mgr_cb_ptr->cback_data);

  if (cmd_name == PS_DSS_NET_MGR_NET_DOWN_CMD)
  {
    (void) dss_close_netlib( net_mgr_cb_ptr->dss_net_handle, &dss_errno);
    memset(net_mgr_cb_ptr, 0, sizeof(dss_net_mgr_cb_type));
    net_mgr_cb_ptr->net_mgr_handle = PS_HANDLE_MGR_INVALID_HANDLE;
    PS_MEM_FREE(net_mgr_cb_ptr);
  }

  return;
} /* dss_net_mgri_process_net_ev_cback() */



/*===========================================================================

                      EXTERNAL FUNCTION DEFINITIONS

===========================================================================*/
/*===========================================================================
FUNCTION DSS_NET_MGR_INIT()

DESCRIPTION
  Initializes dss net mgr module

PARAMETERS
  None

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
void dss_net_mgr_init
(
  void
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /*-------------------------------------------------------------------------
    Initialize Pool 
  -------------------------------------------------------------------------*/
  if (PS_MEM_POOL_INIT_OPT(PS_MEM_DSS_NET_MGR_CB_TYPE,
                           dss_net_mbr_cb_buf_mem,
                           DSS_NET_MGR_CB_BUF_SIZE, 
                           DSS_NET_MGR_CB_BUF_NUM,
                           DSS_NET_MGR_CB_BUF_HIGH_WM,
                           DSS_NET_MGR_CB_BUF_LOW_WM,
                           NULL,
#ifdef FEATURE_DATA_PS_MEM_DEBUG                   
                           (int *) dss_net_mgr_cb_buf_hdr,
                           (int *) dss_net_mgr_cb_buf_ptr
#else
                           NULL,
                           NULL
#endif /* FEATURE_DATA_PS_MEM_DEBUG */                   
                          ) == -1)
  {
    LOG_MSG_ERROR_0("dss_net_mgr_init(): "
                    "Can't init the module");
  }
    
  ps_handle_mgr_init_client( PS_HANDLE_MGR_CLIENT_NET_MGR,
                             DSS_NET_MGR_MAX_SESSIONS,
                             0,
                             0);

  /*-------------------------------------------------------------------------
    Register command handlers
  -------------------------------------------------------------------------*/
  (void) ps_set_cmd_handler( PS_DSS_NET_MGR_NET_UP_CMD,
                             dss_net_mgri_process_net_ev_cback);
  (void) ps_set_cmd_handler( PS_DSS_NET_MGR_NET_DOWN_CMD,
                             dss_net_mgri_process_net_ev_cback);
  return;

} /* dss_net_mgr_init() */

/*===========================================================================
FUNCTION DSS_NET_MGR_BRING_UP_NET_IFACE()

DESCRIPTION
  This function brings up the network interface given a network policy

PARAMETERS
  net_policy_ptr : policy describing the kind of interface, that the client
                   is interested in. If NULL, default network policy is chosen
  iface_id_ptr   : OUT PARAM; iface_id of the brought up interface
  user_cb_f_ptr  : User callback function
  user_cb_data   : Data passed back with the callback function
  dss_errno      : OUT PARAM; updated with reason code on error

RETURN VALUE
  A handle to the session : On success
  DSS_ERROR               : On error

DEPENDENCIES
  Net mgr module must have been initialized
  Clients of this module must be executed in PS context

SIDE EFFECTS
  None
===========================================================================*/
int32 dss_net_mgr_bring_up_net_iface
(
  dss_net_policy_info_type  * net_policy_ptr,
  dss_iface_id_type         * iface_id_ptr,
  dss_net_mgr_cback_f_type    user_cback_f_ptr,
  void                      * user_data_ptr,
  int16                     * dss_errno
)
{
  dss_net_mgr_cb_type  * net_mgr_cb_ptr;
  int16                  ret_val;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  LOG_MSG_INFO2_5("dss_net_mgr_bring_up_net_iface(): "
                  "Bringing up iface for policy 0x%p, cback 0x%p "
                  "udata 0x%p errnoPtr 0x%p  ifaceIdPtr 0x%p",
                  net_policy_ptr, user_cback_f_ptr, user_data_ptr,
                  dss_errno, iface_id_ptr);
  /*-------------------------------------------------------------------------
    Validate arguments
  -------------------------------------------------------------------------*/
  if (dss_errno == NULL)
  {
    return DSS_ERROR;
  }

  if (user_cback_f_ptr == NULL)
  {
    *dss_errno = DS_EFAULT;
    return DSS_ERROR;
  }

  if (iface_id_ptr == NULL)
  {
    *dss_errno = DS_EFAULT;
    return DSS_ERROR;
  }

  *iface_id_ptr = DSS_IFACE_INVALID_ID;

  /*-------------------------------------------------------------------------
    Create DSS Net Mgr control block
  -------------------------------------------------------------------------*/
  net_mgr_cb_ptr =
    (dss_net_mgr_cb_type *) ps_mem_get_buf( PS_MEM_DSS_NET_MGR_CB_TYPE);
  if (net_mgr_cb_ptr == NULL)
  {
    *dss_errno = DS_ENOMEM;
    return DSS_ERROR;
  }

  /*-----------------------------------------------------------------------
    Bring up network library
  -----------------------------------------------------------------------*/
  ret_val = dss_open_netlib2( dss_net_mgri_net_ev_cback,
                              net_mgr_cb_ptr,
                              NULL,
                              NULL,
                              net_policy_ptr,
                              dss_errno);

  if (ret_val == DSS_ERROR)
  {
    LOG_MSG_ERROR_0("dss_net_mgr_bring_up_net_iface(): "
                    "dss_open_netlib2() failed");
    PS_MEM_FREE(net_mgr_cb_ptr);
    return DSS_ERROR;
  }

  net_mgr_cb_ptr->dss_net_handle = ret_val;
  net_mgr_cb_ptr->cback_f_ptr    = user_cback_f_ptr;
  net_mgr_cb_ptr->cback_data     = user_data_ptr;

  /*-------------------------------------------------------------------------
    Update the control block
  -------------------------------------------------------------------------*/
  net_mgr_cb_ptr->net_mgr_handle =
    ps_handle_mgr_get_handle( PS_HANDLE_MGR_CLIENT_NET_MGR,
                                ps_mem_buf_to_index( net_mgr_cb_ptr));
  if (net_mgr_cb_ptr->net_mgr_handle == PS_HANDLE_MGR_INVALID_HANDLE)
  {
    LOG_MSG_ERROR_0("dss_net_mgr_bring_up_net_iface(): "
                    "Failed to get a handle");
    (void) dss_close_netlib( net_mgr_cb_ptr->dss_net_handle, dss_errno);
    PS_MEM_FREE(net_mgr_cb_ptr);
    return DSS_ERROR;
  }

  /*-------------------------------------------------------------------------
    Bring up interface
  -------------------------------------------------------------------------*/
  ret_val = dss_pppopen( net_mgr_cb_ptr->dss_net_handle, dss_errno);
  if (ret_val == DSS_ERROR && *dss_errno != DS_EWOULDBLOCK)
  {
    LOG_MSG_ERROR_0("dss_net_mgr_bring_up_net_iface(): "
                    "dss_pppopen() failed");
    (void) dss_close_netlib( net_mgr_cb_ptr->dss_net_handle, dss_errno);
    PS_MEM_FREE(net_mgr_cb_ptr);
    return DSS_ERROR;
  }

  *iface_id_ptr = dss_get_iface_id( net_mgr_cb_ptr->dss_net_handle);
  return net_mgr_cb_ptr->net_mgr_handle;
} /* dss_net_mgr_bring_up_net_iface() */


/*===========================================================================
FUNCTION DSS_NET_MGR_TEAR_DOWN_NET_IFACE()

DESCRIPTION
  This function tears down the network interface

PARAMETERS
  net_mgr_handle : A handle which identifies the session
  dss_errno      : OUT PARAM; updated with reason code on error

RETURN VALUE
  DSS_SUCCESS : On success
  DSS_ERROR   : On error

DEPENDENCIES
  Net mgr module must have been initialized

SIDE EFFECTS
  None
===========================================================================*/
int16 dss_net_mgr_tear_down_net_iface
(
  int32    net_mgr_handle,
  int16  * dss_errno
)
{
  dss_net_mgr_cb_type  * net_mgr_cb_ptr;
  int16                  index;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  LOG_MSG_INFO2_2("dss_net_mgr_tear_down_net_iface(): "
                  "Tearing down net iface for net mgr handle 0x%d errnoPtr 0x%x",
                  net_mgr_handle, dss_errno);

  if (dss_errno == NULL)
  {
    return DSS_ERROR;
  }

  index = ps_handle_mgr_get_index( PS_HANDLE_MGR_CLIENT_NET_MGR,
                                   net_mgr_handle);
  if (index == PS_HANDLE_MGR_INVALID_INDEX)
  {
    LOG_MSG_INFO1_1("dss_net_mgr_tear_down_net_iface(): "
                    "Invalid handle %d", net_mgr_handle);
    *dss_errno = DS_EFAULT;
    return DSS_ERROR;
  }

  net_mgr_cb_ptr =
    (dss_net_mgr_cb_type *) ps_mem_index_to_buf( index,
                                                 PS_MEM_DSS_NET_MGR_CB_TYPE);
  if (net_mgr_cb_ptr == NULL)
  {
    LOG_MSG_INFO1_1("dss_net_mgr_tear_down_net_iface(): "
                    "Invalid handle %d", net_mgr_handle);
    *dss_errno = DS_EFAULT;
    return DSS_ERROR;
  }

  (void) dss_pppclose( net_mgr_cb_ptr->dss_net_handle, dss_errno);
  return DSS_SUCCESS;

} /* dss_net_mgr_tear_down_net_iface() */

