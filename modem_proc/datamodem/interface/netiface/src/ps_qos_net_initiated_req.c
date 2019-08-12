/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

            P S _ Q O S _ N E T _ I N I T I A T E D _ R E Q . C

DESCRIPTION
  File defining functions that can be used to manipulate a network initiated
  QoS request

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None for the module.  Each flow is created by calling
  ps_iface_create_flow().

Copyright (c) 2011 - 2014 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                            EDIT HISTORY FOR FILE

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/netiface/src/ps_qos_net_initiated_req.c#1 $
  $Author: mplcsds1 $ $DateTime: 2016/12/13 08:00:03 $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
06/24/15    pg     Added to support qos handle when registration to net initiated qos.
06/17/11    szr    Created the file

===========================================================================*/
#include "datamodem_variation.h"
#include "comdef.h"
#include "customer.h"

#ifdef FEATURE_DATA_PS_QOS
/*===========================================================================

                                INCLUDE FILES

===========================================================================*/
#include "ps_qos_net_initiated_req.h"
#include "ps_iface_defs.h"
#include "ps_qos_defs.h"
#include "ps_mem.h"
#include "ps_flowi.h"
#include "ds_Utils_DebugMsg.h"
#include "ps_ifacei_event.h"
#include "ps_ip6_addr.h"
#include "ps_system_heap.h"
#include "ps_ifacei.h"

/*=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

                           LOCAL DATA DECLARATIONS

=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/
typedef struct  
{
  list_link_type link;
  ip_filter_type filter;
} ps_qos_net_initiated_req_filter_type;

/*---------------------------------------------------------------------------
  Tuning the number of PS net initiated qos requests buffers 
  needed by this module
---------------------------------------------------------------------------*/
#define PS_QOS_NET_INITIATED_REQ_BUF_SIZE  ((sizeof(ps_qos_net_initiated_req_type) + 3) & ~3)
#define PS_QOS_NET_INITIATED_REQ_BUF_NUM       PS_QOS_NET_INITIATED_MAX_REQS
#define PS_QOS_NET_INITIATED_REQ_BUF_HIGH_WM   (PS_QOS_NET_INITIATED_MAX_REQS - 5)
#define PS_QOS_NET_INITIATED_REQ_BUF_LOW_WM    (5)

/*----------------------------------------------------------------------------
  Allocate memory to hold ps_qos_net_initiated_req along with ps_mem header
----------------------------------------------------------------------------*/
static int ps_qos_net_initiated_req_buf_mem[
  PS_MEM_GET_TOT_SIZE_OPT(PS_QOS_NET_INITIATED_REQ_BUF_NUM,
                          PS_QOS_NET_INITIATED_REQ_BUF_SIZE)];

#ifdef FEATURE_DATA_PS_MEM_DEBUG
/*----------------------------------------------------------------------------
  Array of pointers used to facilitate easy debugging.  The first one points
  to the ps_mem header and the latter points to ps_qos_net_initiated_req_buf
----------------------------------------------------------------------------*/
static ps_mem_buf_hdr_type           * ps_qos_net_initiated_req_buf_hdr[PS_QOS_NET_INITIATED_REQ_BUF_NUM];
static ps_qos_net_initiated_req_type * ps_qos_net_initiated_req_buf_ptr[PS_QOS_NET_INITIATED_REQ_BUF_NUM];
#endif /* FEATURE_DATA_PS_MEM_DEBUG */

/*---------------------------------------------------------------------------
  Tuning the number of ps net initiated qos INFO requests buffers 
  needed by this module
---------------------------------------------------------------------------*/
#define PS_QOS_NET_INITIATED_REQ_INFO_BUF_SIZE  ((sizeof(qos_net_initiated_req_info_type) + 3) & ~3)
#define PS_QOS_NET_INITIATED_REQ_INFO_BUF_NUM       PS_QOS_NET_INITIATED_MAX_REQS
#define PS_QOS_NET_INITIATED_REQ_INFO_BUF_HIGH_WM   (PS_QOS_NET_INITIATED_MAX_REQS - 5)
#define PS_QOS_NET_INITIATED_REQ_INFO_BUF_LOW_WM    (5)

/*----------------------------------------------------------------------------
  Allocate memory to hold ps_qos_net_initiated_req_info 
  along with ps_mem header
----------------------------------------------------------------------------*/
static int ps_qos_net_initiated_req_info_buf_mem[
  PS_MEM_GET_TOT_SIZE_OPT(PS_QOS_NET_INITIATED_REQ_INFO_BUF_NUM,
                          PS_QOS_NET_INITIATED_REQ_INFO_BUF_SIZE)];

#ifdef FEATURE_DATA_PS_MEM_DEBUG
/*----------------------------------------------------------------------------
  Array of pointers used to facilitate easy debugging.  The first one points
  to the ps_mem header and the latter points to 
  ps_qos_net_initiated_req_info_buf
----------------------------------------------------------------------------*/
static ps_mem_buf_hdr_type                * ps_qos_net_initiated_req_info_buf_hdr[PS_QOS_NET_INITIATED_REQ_INFO_BUF_NUM];
static qos_net_initiated_req_info_type    * ps_qos_net_initiated_req_info_buf_ptr[PS_QOS_NET_INITIATED_REQ_INFO_BUF_NUM];
#endif /* FEATURE_DATA_PS_MEM_DEBUG */

/*---------------------------------------------------------------------------
  Tuning the number of PS net initiated qos filter buffers
  needed by this module
---------------------------------------------------------------------------*/
#define PS_QOS_NET_INITIATED_REQ_FILTER_BUF_SIZE  ((sizeof(ps_qos_net_initiated_req_filter_type) + 3) & ~3)
/*---------------------------------------------------------------------------
  There are in average not more than MAX_FLTR_PER_REQ / 4
  filters in every request
---------------------------------------------------------------------------*/
#define PS_QOS_NET_INITIATED_REQ_FILTER_BUF_NUM     ((uint32)(PS_QOS_NET_INITIATED_MAX_REQS * (MAX_FLTR_PER_REQ / 4)))
#define PS_QOS_NET_INITIATED_REQ_FILTER_BUF_HIGH_WM (PS_QOS_NET_INITIATED_REQ_FILTER_BUF_NUM - 5)  
#define PS_QOS_NET_INITIATED_REQ_FILTER_BUF_LOW_WM  (5)

/*----------------------------------------------------------------------------
  Allocate memory to hold ps_qos_net_initiated_req_filter_buf_mem 
  along with ps_mem header
----------------------------------------------------------------------------*/
static int ps_qos_net_initiated_req_filter_buf_mem[
  PS_MEM_GET_TOT_SIZE_OPT(PS_QOS_NET_INITIATED_REQ_FILTER_BUF_NUM,
                          PS_QOS_NET_INITIATED_REQ_FILTER_BUF_SIZE)];

#ifdef FEATURE_DATA_PS_MEM_DEBUG
  /*----------------------------------------------------------------------------
    Array of pointers used to facilitate easy debugging.  The first one points
    to the ps_mem header and the latter points to 
    ps_qos_net_initiated_req_info_buf
  ----------------------------------------------------------------------------*/
  static ps_mem_buf_hdr_type                   * ps_qos_net_initiated_req_filter_buf_hdr[PS_QOS_NET_INITIATED_REQ_FILTER_BUF_NUM];
  static ps_qos_net_initiated_req_filter_type  * ps_qos_net_initiated_req_filter_buf_ptr[PS_QOS_NET_INITIATED_REQ_FILTER_BUF_NUM];
#endif /* FEATURE_DATA_PS_MEM_DEBUG */

/*===========================================================================

                                INDICATIONS

===========================================================================*/

/*===========================================================================
FUNCTION PS_IFACE_QOS_NET_INITIATED_REQ_INIT()

DESCRIPTION
  Initializes QoS network initiated data structures

PARAMETERS
  None
RETURN VALUE
  None
DEPENDENCIES
  None
SIDE EFFECTS
  None
===========================================================================*/
void ps_iface_qos_net_initiated_req_init
(
  void
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /*-------------------------------------------------------------------------
    Initialize Pools
  -------------------------------------------------------------------------*/
   /* Init the Network Initiated QoS objects buffer */ 
   if (PS_MEM_POOL_INIT_OPT(PS_MEM_PS_QOS_NET_INITIATED_REQ_TYPE,
      ps_qos_net_initiated_req_buf_mem,
      PS_QOS_NET_INITIATED_REQ_BUF_SIZE,
      PS_QOS_NET_INITIATED_REQ_BUF_NUM,
      PS_QOS_NET_INITIATED_REQ_BUF_HIGH_WM,
      PS_QOS_NET_INITIATED_REQ_BUF_LOW_WM,
      NULL,
#ifdef FEATURE_DATA_PS_MEM_DEBUG
      (int *) ps_qos_net_initiated_req_buf_hdr,
      (int *) ps_qos_net_initiated_req_buf_ptr
#else
      NULL,
      NULL
#endif /* FEATURE_DATA_PS_MEM_DEBUG */
      ) == -1)
   {
      LOG_MSG_ERROR_0("ps_iface_qos_net_initiated_req_init(): "
                      "Can't init the module");
   }

   /* Init the Network Initiated QoS info objects buffer */ 
   if (PS_MEM_POOL_INIT_OPT(PS_MEM_PS_QOS_NET_INITIATED_REQ_INFO_TYPE,
      ps_qos_net_initiated_req_info_buf_mem,
      PS_QOS_NET_INITIATED_REQ_INFO_BUF_SIZE,
      PS_QOS_NET_INITIATED_REQ_INFO_BUF_NUM,
      PS_QOS_NET_INITIATED_REQ_INFO_BUF_HIGH_WM,
      PS_QOS_NET_INITIATED_REQ_INFO_BUF_LOW_WM,
      NULL,
#ifdef FEATURE_DATA_PS_MEM_DEBUG
      (int *) ps_qos_net_initiated_req_info_buf_hdr,
      (int *) ps_qos_net_initiated_req_info_buf_ptr
#else
      NULL,
      NULL
#endif /* FEATURE_DATA_PS_MEM_DEBUG */
      ) == -1)
   {
      LOG_MSG_ERROR_0("ps_iface_qos_net_initiated_req_init(): "
                      "Can't init the module");
   }

   /* Init the Network Initiated QoS filters objects buffer */                            
   if (PS_MEM_POOL_INIT_OPT(PS_MEM_PS_QOS_NET_INITIATED_FLTR_TYPE, 
      ps_qos_net_initiated_req_filter_buf_mem,
      PS_QOS_NET_INITIATED_REQ_FILTER_BUF_SIZE,
      PS_QOS_NET_INITIATED_REQ_FILTER_BUF_NUM,
      PS_QOS_NET_INITIATED_REQ_FILTER_BUF_HIGH_WM,
      PS_QOS_NET_INITIATED_REQ_FILTER_BUF_LOW_WM,
      NULL,
#ifdef FEATURE_DATA_PS_MEM_DEBUG
      (int *) ps_qos_net_initiated_req_filter_buf_hdr,
      (int *) ps_qos_net_initiated_req_filter_buf_ptr
#else
      NULL,
      NULL
#endif /* FEATURE_DATA_PS_MEM_DEBUG */
      ) == -1)
   {
      LOG_MSG_ERROR_0("ps_iface_qos_net_initiated_req_init(): "
                      "Can't init the module");
   }
} /* ps_iface_qos_net_initiated_req_init() */

/*===========================================================================
FUNCTION PS_QOS_NET_INITIATED_STORE_FLTR_SPEC()

DESCRIPTION
  Stores Rx fltr spec at specified location

PARAMETERS
  qos_info_ptr : Address where Rx fltr spec needs to be stored. Could be
                 qos_info_ptr or modify_qos_info_ptr of ps_flow
  qos_spec_ptr : QOS spec containing Rx fltr spec to be stored

RETURN VALUE
  None

DEPENDENCIES
  Parameters must be valid. Validation is not performed as this is an internal
  function

SIDE EFFECTS
  None
===========================================================================*/
static int32 ps_qos_net_initiated_store_fltr_spec
(
  qos_net_initiated_req_info_type    * qos_info_ptr,
  const net_initiated_qos_spec_type  * qos_spec_ptr,
  int16                              * ps_errno
)
{
  ps_qos_net_initiated_req_filter_type  * new_filter_buf_ptr;
  uint8                                   fltr_index;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    Store Rx filter spec
  -------------------------------------------------------------------------*/
  for (fltr_index = 0;
       fltr_index < qos_spec_ptr->rx_fltr_template.num_filters;
       fltr_index++)
  {
    new_filter_buf_ptr = (ps_qos_net_initiated_req_filter_type *)
                           ps_mem_get_buf(PS_MEM_PS_QOS_NET_INITIATED_FLTR_TYPE);
    if (new_filter_buf_ptr == NULL)
    {
      *ps_errno = DS_ENOMEM;
      return -1;
    }

    new_filter_buf_ptr->filter =
      qos_spec_ptr->rx_fltr_template.list_ptr[fltr_index];
    list_push_back(&(qos_info_ptr->rx.fltr_list),
      &(new_filter_buf_ptr->link));
    
    LOG_MSG_INFO1_2("ps_qos_net_initiated_store_fltr_spec()"
                    "App RX filter: %d obj 0x%p",
                    fltr_index, new_filter_buf_ptr);
    
    ps_iface_log_qos_spec_filter_params(&qos_spec_ptr->rx_fltr_template.list_ptr[fltr_index]);
  }

  /*-------------------------------------------------------------------------
    Store Tx filter spec
  -------------------------------------------------------------------------*/
  for (fltr_index = 0;
       fltr_index < qos_spec_ptr->tx_fltr_template.num_filters;
       fltr_index++)
  {
    new_filter_buf_ptr = (ps_qos_net_initiated_req_filter_type *)
                           ps_mem_get_buf(PS_MEM_PS_QOS_NET_INITIATED_FLTR_TYPE);
    if (new_filter_buf_ptr == NULL)
    {
      *ps_errno = DS_ENOMEM;
      return -1;
    }

    /*-----------------------------------------------------------------------
      Precedence is populated through ps_ifacei_tx_qos_fltr_add()
    -----------------------------------------------------------------------*/
    new_filter_buf_ptr->filter =
      qos_spec_ptr->tx_fltr_template.list_ptr[fltr_index];
    list_push_back(&(qos_info_ptr->tx.fltr_list),
      &(new_filter_buf_ptr->link));

    LOG_MSG_INFO1_2("ps_qos_net_initiated_store_fltr_spec()"
                    "App TX filter: %d obj 0x%p",
                    fltr_index, new_filter_buf_ptr);

    ps_iface_log_qos_spec_filter_params(&qos_spec_ptr->tx_fltr_template.list_ptr[fltr_index]);
  }

  return 0;

} /* ps_qos_net_initiated_store_fltr_spec() */

/*===========================================================================
FUNCTION PS_QOS_NET_INITIATED_DELETE_FILTER_SPEC()

DESCRIPTION
Deletes fltr spec from specified location in qos net initiated req spec

PARAMETERS
qos_info_ptr : Address where Rx fltr spec is stored. Could be qos_info_ptr
or modify_qos_info_ptr of ps_flow

RETURN VALUE
None

DEPENDENCIES
Parameters must be valid. Validation is not performed as this is an internal
function

SIDE EFFECTS
None
===========================================================================*/
void ps_qos_net_initiated_delete_filter_spec
(
  list_type filters_list 
)
{
  ps_qos_net_initiated_req_filter_type   * curr_fltr = NULL;
  ps_qos_net_initiated_req_filter_type   * next_fltr = NULL;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  LOG_MSG_INFO1_0("ps_qos_net_initiated_delete_filter_spec()");

  curr_fltr = list_peek_front(&filters_list);
  while (NULL != curr_fltr)
  {
    next_fltr = list_peek_next(&filters_list,
      &(curr_fltr->link));

    list_pop_item(&filters_list, &(curr_fltr->link));
    PS_MEM_FREE(curr_fltr);

    curr_fltr = next_fltr;
  } /* while there is a fltr in the list */

} /* ps_qos_net_initiated_delete_filter_spec() */

/*===========================================================================
FUNCTION PS_QOS_NET_INITIATED_DELETE_QOS_SPEC()

DESCRIPTION
  This function deletes previously stored qos net initiated spec and
  frees up any memory buffers used.

PARAMETERS
  qos_net_initiated_req_ptr : ptr qos net initiated spec

RETURN VALUE
  None

DEPENDENCIES
  Flow must not be a default flow as QOS is not supported on default flows.

SIDE EFFECTS
  None
===========================================================================*/
void ps_qos_net_initiated_delete_qos_spec
(
 ps_qos_net_initiated_req_type    * qos_net_initiated_req_ptr   
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  LOG_MSG_INFO1_0("ps_qos_net_initiated_delete_qos_spec()");

  if ( NULL == qos_net_initiated_req_ptr || 
       NULL == qos_net_initiated_req_ptr->qos_net_initiated_req_private.qos_info_ptr )
  {
    DATA_ERR_FATAL("ps_qos_net_initiated_delete_qos_spec(): QOS info pointer of qos net initiated req NULL");
    return;
  }

  /*-------------------------------------------------------------------------
    Free Rx filters and add them to ps_mem pool
  -------------------------------------------------------------------------*/
  ps_qos_net_initiated_delete_filter_spec(
    qos_net_initiated_req_ptr->qos_net_initiated_req_private.qos_info_ptr->rx.fltr_list
    );
  
  /*-------------------------------------------------------------------------
    Free Tx filters and add them to ps_mem pool
  -------------------------------------------------------------------------*/
  ps_qos_net_initiated_delete_filter_spec(
    qos_net_initiated_req_ptr->qos_net_initiated_req_private.qos_info_ptr->tx.fltr_list
    );

  /*-----------------------------------------------------------------------
    Destroy various lists used to store QOS info
  -----------------------------------------------------------------------*/
  list_destroy(&(qos_net_initiated_req_ptr->qos_net_initiated_req_private.qos_info_ptr->rx.fltr_list));
  list_destroy(&(qos_net_initiated_req_ptr->qos_net_initiated_req_private.qos_info_ptr->tx.fltr_list));

  /*-------------------------------------------------------------------------
    Free the qos spec
  -------------------------------------------------------------------------*/  
  PS_MEM_FREE(qos_net_initiated_req_ptr->qos_net_initiated_req_private.qos_info_ptr);

  LOG_MSG_INFO1_1("ps_qos_net_initiated_delete_qos_spec(): "
                  "QoS Net Initiated 0x%p: QOS released",
                  qos_net_initiated_req_ptr);

} /* ps_qos_net_initiated_delete_qos_spec() */

/*===========================================================================
FUNCTION PS_QOS_NET_INITIATED_STORE_QOS_SPEC()

DESCRIPTION
  This function stores the requested network initiated qos spec on the
  qos_net_initiated_req object on the iface.

PARAMETERS
  qos_net_initiated_req : Network Initiated Request object where QOS spec is to be stored on the iface.
  qos_net_initiated_spec: QOS spec to be stored.
  ps_errno              : error code passed back when storage fails.

RETURN VALUE
   0 : on success
  -1 : on failure

DEPENDENCIES
  TODO: verify the need for locks
  Must be called inside TASKLOCK()

SIDE EFFECTS
  If error occurs during storage, none of QOS spec is stored, i.e semantics
  is all or nothing

===========================================================================*/
int ps_qos_net_initiated_store_qos_spec
(
  ps_qos_net_initiated_req_type        * qos_net_initiated_req,
  const net_initiated_qos_spec_type    * qos_net_initiated_spec,
  int16                                * ps_errno
)
{
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  LOG_MSG_INFO1_0("ps_qos_net_initiated_store_qos_spec()");
  ASSERT(NULL != ps_errno);

  if (NULL == qos_net_initiated_spec)
  {
    DATA_ERR_FATAL("ps_qos_net_initiated_store_qos_spec(): QOS_SPEC is NULL");
    *ps_errno = DS_EFAULT;
    return -1;
  }

  if (NULL == qos_net_initiated_req || 
      NULL == qos_net_initiated_req->qos_net_initiated_req_private.qos_info_ptr)
  {
    DATA_ERR_FATAL("ps_qos_net_initiated_store_qos_spec(): qos_net_initiated_req or qos_info_ptr is NULL");
    *ps_errno = DS_EFAULT;
    return -1;
  }

  /*-------------------------------------------------------------------------
    Store filters
  -------------------------------------------------------------------------*/
  LOG_MSG_INFO2_0("ps_qos_net_initiated_store_qos_spec(): "
                  "Storing fltr spec");

  /*-------------------------------------------------------------------------
    Store filter spec on the qos net initiated object
  -------------------------------------------------------------------------*/
  if ( -1 == ps_qos_net_initiated_store_fltr_spec
             (
                qos_net_initiated_req->qos_net_initiated_req_private.qos_info_ptr,
                qos_net_initiated_spec,
                ps_errno
              ) )
  {
    LOG_MSG_ERROR_1("ps_qos_net_initiated_store_qos_spec(): "
                    "QoS Net Initiated Flow 0x%p: Couldn't store filter spec",
                    qos_net_initiated_req);
    ps_qos_net_initiated_delete_qos_spec(qos_net_initiated_req);
    return -1;
  }

  return 0;

} /* ps_qos_net_initiated_store_qos_spec() */

/*===========================================================================
FUNCTION PS_IFACE_QOS_NET_INITIATED_REQUEST_CREATE()

DESCRIPTION
  This function creates a new qos net initiated request obejct and
  initializes it.

PARAMETERS  
  qos_net_initiated_req_ptr_ptr : OUT PARAM. QoS Net Initiated that is created
  ps_errno        : error returned back in case of an error

RETURN VALUE
   0 : if created successfully
  -1 : otherwise

DEPENDENCIES  
  None

SIDE EFFECTS
  None
===========================================================================*/
int32 ps_iface_qos_net_initiated_request_create
(  
  ps_qos_net_initiated_req_type ** qos_net_initiated_req_ptr_ptr,
  int16                          * ps_errno
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  *qos_net_initiated_req_ptr_ptr = 
    (ps_qos_net_initiated_req_type *) ps_mem_get_buf(PS_MEM_PS_QOS_NET_INITIATED_REQ_TYPE);
  if (NULL == *qos_net_initiated_req_ptr_ptr)
  {
    *ps_errno = DS_ENOMEM;
    return -1;
  }

  memset(*qos_net_initiated_req_ptr_ptr, 0, sizeof(ps_qos_net_initiated_req_type));

  /*-------------------------------------------------------------------------
    Allocate qos net initiated info buffer from ps_mem pool
  -------------------------------------------------------------------------*/
  (*qos_net_initiated_req_ptr_ptr)->qos_net_initiated_req_private.qos_info_ptr = 
    (qos_net_initiated_req_info_type *)ps_mem_get_buf(PS_MEM_PS_QOS_NET_INITIATED_REQ_INFO_TYPE);

  if (NULL == (*qos_net_initiated_req_ptr_ptr)->qos_net_initiated_req_private.qos_info_ptr)
  {
    LOG_MSG_ERROR_1("ps_iface_qos_net_initiated_request_create(): "
                    "Couldn't create qos_net_initiated_info_ptr for qos_net_initiated_req, 0x%p",
                    *qos_net_initiated_req_ptr_ptr);
    *ps_errno = DS_ENOMEM;
    PS_MEM_FREE(*qos_net_initiated_req_ptr_ptr);
    return -1;
  }

  memset((*qos_net_initiated_req_ptr_ptr)->qos_net_initiated_req_private.qos_info_ptr,
          0,
          sizeof(qos_net_initiated_req_info_type));

  /*-----------------------------------------------------------------------
    Initialize various lists used to store QOS info
  -----------------------------------------------------------------------*/
  list_init(&((*qos_net_initiated_req_ptr_ptr)->qos_net_initiated_req_private.qos_info_ptr->rx.fltr_list));
  list_init(&((*qos_net_initiated_req_ptr_ptr)->qos_net_initiated_req_private.qos_info_ptr->tx.fltr_list));
  
  LOG_MSG_INFO1_2("ps_iface_qos_net_initiated_request_create(): "
                  "new NIQ req 0x%p new req info 0x%p",
                  *qos_net_initiated_req_ptr_ptr,
                  (*qos_net_initiated_req_ptr_ptr)->qos_net_initiated_req_private.qos_info_ptr);
  
  return 0;

} /* ps_iface_qos_net_initiated_request_create() */

/*===========================================================================
FUNCTION PS_IFACE_QOS_NET_INITIATED_REQUEST_CLEAN()

DESCRIPTION
  This function frees memory allocated for the provided qos net initiated
  request object (including the filters list if any were allocated)

PARAMETERS
  is_qos_supported : is QOS supported on the flow being created?
  flow_ptr         : OUT PARAM. Flow that is created
  ps_errno         : error returned back in case of an error

RETURN VALUE
   0 : if flow can be created
  -1 : otherwise

DEPENDENCIES
  ps_iface_qos_net_initiated_request_create must be successfully completed
  before calling this function.
  Cleanup within ps_iface_qos_net_initiated_request_create does not use
  this function.

SIDE EFFECTS
  None
===========================================================================*/
void ps_iface_qos_net_initiated_request_clean
(  
  ps_qos_net_initiated_req_type        * qos_net_initiated_req    
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  LOG_MSG_INFO1_0("ps_iface_qos_net_initiated_request_clean()");

  if (NULL == qos_net_initiated_req) 
  {
     return;
  }
  
  ps_qos_net_initiated_delete_qos_spec(qos_net_initiated_req);    
  PS_MEM_FREE(qos_net_initiated_req);
  
  return;

} /* ps_iface_qos_net_initiated_request_clean() */


/*===========================================================================
FUNCTION  PS_IFACE_CREATE_QOS_NET_INITIATED_REQUEST()

DESCRIPTION
  Handles PS_IFACE_IOCTL_QOS_NET_INITIATED_REQUEST ioctl.

PARAMETERS
  iface_ptr                    : corresponding iface on which QOS is requested.
                                 This MUST be a logical iface, else an error is
                                 returned
  qos_net_initiated_request_ptr: ptr to qos net initiated request
  ps_errno                     : specific error code in case operation fails

RETURN VALUE
   0 : on success
  -1 : on failure

DEPENDENCIES
  For the PS_IFACE_IOCTL_QOS_REQUEST_EX case, it is assumed that the
  number of flows requested via ioctl_argval_ptr are the same as number
  of logical flows to be created on the top-level iface.

SIDE EFFECTS
  None
===========================================================================*/
int ps_iface_create_qos_net_initiated_request
(
  ps_iface_type                                 * iface_ptr,
  ps_iface_ioctl_qos_net_initiated_request_type * qos_net_initiated_request_ptr, 
  int16                                         * ps_errno
)
{
  ps_qos_net_initiated_req_type        * qos_net_initiated_req = NULL;  
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  LOG_MSG_INFO1_0("ps_iface_create_qos_net_initiated_request()");

  if (NULL == iface_ptr ||
      NULL == qos_net_initiated_request_ptr ||
      NULL == ps_errno)
  {
    DATA_ERR_FATAL("ps_iface_create_qos_net_initiated_request: Invalid args");
    return -1;    
  }
  
  if (ps_flowi_validate_net_initiated_qos_spec(&qos_net_initiated_request_ptr->net_initiated_qos_spec,
                                               ps_iface_get_addr_family(iface_ptr),
                                               ps_errno) != 0)
  {
    LOG_MSG_ERROR_0("ps_iface_create_qos_net_initiated_request(): "
                    "Invalid QOS spec is sent by application");
    /* ps_errno is populated in ps_flowi_validate_net_initiated_qos_spec() */
    return -1;
  }
                                                 
  if (ps_iface_qos_net_initiated_request_create(&qos_net_initiated_req, ps_errno) != 0)
  {
    LOG_MSG_ERROR_0("ps_iface_create_qos_net_initiated_request(): "
                    "Couldn't create a buffer for PS flow");
    /* ps_errno is populated in ps_flowi_create() */
    return -1;
  }
  
  if (ps_qos_net_initiated_store_qos_spec(
     qos_net_initiated_req,                // net initiated qos request to be installed on the iface
     &qos_net_initiated_request_ptr->net_initiated_qos_spec,
     ps_errno) != 0)
  {
    LOG_MSG_ERROR_0("ps_iface_create_qos_net_initiated_request(): "
                    "Couldn't store qos spec");
    ps_iface_qos_net_initiated_request_clean(qos_net_initiated_req);      
    /* ps_errno is populated in ps_qos_net_initiated_store_qos_spec() */
    return -1;
  }      

  /* Install the new qos net initiated request object */
  PS_ENTER_CRIT_SECTION(&global_ps_crit_section);

  if (PS_QOS_NET_INITIATED_MAX_REQS == list_size(&iface_ptr->iface_private.rx_qos_fltr_node_list))
  {
     PS_BRANCH_LEAVE_CRIT_SECTION(&global_ps_crit_section);
     LOG_MSG_INFO1_3("ps_iface_create_qos_net_initiated_request(): "
                     "Iface, 0x%x:%d, already has maximum possible number of "
                     "ps_qos_net_initiated_req (%d). Can't add one more request",
                     iface_ptr->name,
                     iface_ptr->instance,
                     PS_QOS_NET_INITIATED_MAX_REQS);

     ps_iface_qos_net_initiated_request_clean(qos_net_initiated_req);      
     *ps_errno = DS_ENOMEM;
     return -1;
  }

  list_push_front(&(iface_ptr->iface_private.net_initiated_qos_req_list),
     &(qos_net_initiated_req->link));

  PS_LEAVE_CRIT_SECTION(&global_ps_crit_section);

  /* Provide back a pointer to the qos net initiated request object installed on the iface */
  qos_net_initiated_request_ptr->handle = (ps_iface_net_initiated_qos_req_handle_type)qos_net_initiated_req;

  return 0;
} /* ps_iface_create_qos_net_initiated_request() */

/*===========================================================================
FUNCTION  PS_IFACE_RELEASE_QOS_NET_INITIATED()

DESCRIPTION
  Handles PS_IFACE_IOCTL_QOS_NET_INITIATED_RELEASE ioctl.

PARAMETERS
  iface_ptr                     : corresponding iface on which QOS is requested.
                                  This MUST be a logical iface, else an error is
                                  returned
  qos_net_initiated_release_ptr : ptr to qos net initiated release whih contains
                                  the qos net initiated handle.
  ps_errno                      : specific error code in case operation fails

RETURN VALUE
   0 : on success
  -1 : on failure

DEPENDENCIES
  For the PS_IFACE_IOCTL_QOS_REQUEST_EX case, it is assumed that the
  number of flows requested via ioctl_argval_ptr are the same as number
  of logical flows to be created on the top-level iface.

SIDE EFFECTS
  None
===========================================================================*/
int ps_iface_release_qos_net_initiated
(
  ps_iface_type                                 * iface_ptr,
  ps_iface_ioctl_qos_net_initiated_release_type * qos_net_initiated_release_ptr,
  int16                                         * ps_errno
)
{
  ps_qos_net_initiated_req_type        * qos_net_initiated_req = NULL;  
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  LOG_MSG_INFO1_0("ps_iface_release_qos_net_initiated()");

  if (NULL == iface_ptr || 
      NULL == qos_net_initiated_release_ptr ||
      0    == qos_net_initiated_release_ptr->handle ||
      NULL == ps_errno)
  {
    DATA_ERR_FATAL("Invalid args to ps_iface_release_qos_net_initiated");
    return -1;    
  }

  qos_net_initiated_req = (ps_qos_net_initiated_req_type *)qos_net_initiated_release_ptr->handle;
  
  /* Remove the qos net initiated request object */
  PS_ENTER_CRIT_SECTION(&global_ps_crit_section);

  /* Remove the QoS Net Initiated object from the iface's list */
  list_pop_item(&(iface_ptr->iface_private.net_initiated_qos_req_list),
                &(qos_net_initiated_req->link));

  PS_LEAVE_CRIT_SECTION(&global_ps_crit_section);

  /* Free memory of the QoS Net Initiated request object, QoS spec and filters */
  ps_iface_qos_net_initiated_request_clean(qos_net_initiated_req);

  return 0;
} /* ps_iface_release_qos_net_initiated() */

/*===========================================================================
FUNCTION  PS_IFACE_QOS_NET_INITIATED_REAL_TIME_CHECK()

DESCRIPTION
  Handles PS_IFACE_IOCTL_QOS_NET_INITIATED_REAL_TIME_CHECK ioctl.

PARAMETERS
  iface_ptr         : corresponding iface on which QOS is requested.
                      This MUST be a logical iface, else an error is
                      returned
  qos_net_initiated_real_time_check_ptr : pointer to structure containing the
                                          qos_spec for the logical flows to be
                                          checked. This MUST not be NULL.
  ps_errno          : specific error code in case operation fails

RETURN VALUE
   0 : on success
  -1 : on failure

DEPENDENCIES

SIDE EFFECTS
  If there is a flow that fits the QoS spec requirements, the request callback
  will be called.
===========================================================================*/
int ps_iface_qos_net_initiated_real_time_check
(
  ps_iface_type                                         * iface_ptr,
  ps_iface_ioctl_qos_net_initiated_real_time_check_type * qos_net_initiated_real_time_check_ptr,
  int16                                                 * ps_errno
)
{
  ps_flow_type                         * flow_ptr              = NULL;
  ps_iface_ioctl_matching_flow_type *    matching_flow_ptr     = NULL;
  int                                    retval = -1;
  int16                                  temp_ps_errno = 0;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  LOG_MSG_INFO1_0("ps_iface_qos_net_initiated_real_time_check()");

  if (NULL == iface_ptr || 
      NULL == qos_net_initiated_real_time_check_ptr ||
      NULL == ps_errno)
  {
    DATA_ERR_FATAL("Invalid args to ps_iface_release_qos_net_initiated");
    return -1;    
  }
  
  if (!PS_IFACE_IS_VALID(iface_ptr))
  {
    LOG_MSG_ERROR_1("Invalid iface, 0x%p, is passed", iface_ptr);
    ASSERT(0);
    *ps_errno = DS_EFAULT;
    return -1;
  }                                                
    
  PS_ENTER_CRIT_SECTION(&global_ps_crit_section);

  flow_ptr = list_peek_front(&(iface_ptr->iface_private.flow.sec_flow_list));
  temp_ps_errno = DS_EWOULDBLOCK;
  while (flow_ptr != NULL)
  {
    LOG_MSG_INFO1_1 ("ps_iface_qos_net_initiated_real_time_check : "
                     "Comparing Flow 0x%p for NIQ request", flow_ptr);
    if ( PS_FLOW_IS_NW_INITIATED(flow_ptr) &&
         0    == ps_qos_net_initiated_check_flow(flow_ptr,
                                              &qos_net_initiated_real_time_check_ptr->net_initiated_qos_spec) )
    {
       LOG_MSG_INFO1_1("ps_iface_qos_net_initiated_real_time_check : "
                       "Flow 0x%p has a match for NIQ request. Notifying application",
                       flow_ptr);
       PS_SYSTEM_HEAP_MEM_ALLOC(matching_flow_ptr, sizeof(ps_iface_ioctl_matching_flow_type),
                                ps_iface_ioctl_matching_flow_type*);
       retval = 0;
       temp_ps_errno = 0;
       if (NULL == matching_flow_ptr)
       {
         /*------------------------------------------------------------------
           Free the memory already allocated 
         ------------------------------------------------------------------*/
         matching_flow_ptr = (ps_iface_ioctl_matching_flow_type*)
           list_pop_front(&qos_net_initiated_real_time_check_ptr->matching_flow_list);
         while ( NULL != matching_flow_ptr )
         {
            PS_SYSTEM_HEAP_MEM_FREE(matching_flow_ptr);
            matching_flow_ptr = (ps_iface_ioctl_matching_flow_type*)
              list_pop_front(&qos_net_initiated_real_time_check_ptr->matching_flow_list);
         }
         PS_LEAVE_CRIT_SECTION(&global_ps_crit_section);
         *ps_errno = DS_ENOMEM;
         return -1;
       }
       
       matching_flow_ptr->flow_ptr = flow_ptr;
       list_push_back(
         &qos_net_initiated_real_time_check_ptr->matching_flow_list,
         &matching_flow_ptr->link);
    }
    
    flow_ptr = list_peek_next(&iface_ptr->iface_private.flow.sec_flow_list,
                              &flow_ptr->link);
  }

  PS_LEAVE_CRIT_SECTION(&global_ps_crit_section);

  if ((DS_EWOULDBLOCK == temp_ps_errno) && (*ps_errno != DS_ENOMEM))
  {
    retval = -1;
    *ps_errno = DS_EWOULDBLOCK;
  }
  return retval;
} /* ps_iface_qos_net_initiated_real_time_check() */

/*===========================================================================
FUNCTION PS_QOS_NET_INITIATED_CHECK_REQUESTS()

DESCRIPTION
  Goes over all Network Initiated QoS application requests.
  For each one checks if the filters of the provided QoS Spec matches the filters
  of the application request. If yes notifies the application on the existence
  of matching Network initiated QoS.

PARAMETERS
  iface_ptr         : corresponding iface on which QOS is requested
  flow_ptr          : flow to be modified
  qos_spec_ptr      : pointer to the qos spec of the flow

RETURN VALUE
   0 : on success
  -1 : on failure

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
int32 ps_qos_net_initiated_check_requests
(
  ps_iface_type                    * iface_ptr,
  ps_flow_type                     * flow_ptr,
  qos_spec_type                    * qos_spec_ptr  
)
{
  ps_qos_net_initiated_req_type    * ps_qos_net_initiated_req;
  ps_iface_event_info_u_type       event_info;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  
  LOG_MSG_INFO1_3("ps_qos_net_initiated_check_requests()"
                  "Iface_ptr 0x%p, flow_pta 0x%p, qos_spec_ptr 0x%p",
                  iface_ptr, flow_ptr, qos_spec_ptr);

  /* skipping parameters validation as this is already done in calling function */

  event_info.iface_net_initiated_qos_available_info.flow_ptr = flow_ptr;
  PS_ENTER_CRIT_SECTION(&global_ps_crit_section);
  
  /*-------------------------------------------------------------------------
     Go over application requests to be notified on matching network initiated
     QoS. For each one check the filters of the new flow.
  -------------------------------------------------------------------------*/
  ps_qos_net_initiated_req = 
    list_peek_front(&(iface_ptr->iface_private.net_initiated_qos_req_list));
  while (NULL != ps_qos_net_initiated_req)
  {
    if ( 0 == ps_qos_net_initiated_check_one_request(ps_qos_net_initiated_req,
                                                     qos_spec_ptr) )
    {
      /*--------------------------------------------------------------------- 
        Notify application on Network Initiated QoS matching application
        specified filters 
        -------------------------------------------------------------------*/
       LOG_MSG_INFO1_1("ps_qos_net_initiated_check_requests(): "
                       "NIQ Request 0x%p has a match for flow. Notifying application",
                       ps_qos_net_initiated_req);
       event_info.iface_net_initiated_qos_available_info.handle =
         (int32)ps_qos_net_initiated_req;
       ps_ifacei_invoke_event_cbacks( 
          iface_ptr,     /* although registration for this event is done on 
                            the global queue */
          NULL,          /* Physlink pointer - not applicable to this  event */
          IFACE_NET_INITIATED_QOS_AVAILABLE_EV,
          event_info);
    }

    ps_qos_net_initiated_req =
      list_peek_next( &(iface_ptr->iface_private.net_initiated_qos_req_list),
                      &(ps_qos_net_initiated_req->link) );
  }

  PS_LEAVE_CRIT_SECTION(&global_ps_crit_section);

  return 0;
} /* ps_qos_net_initiated_check_requests() */

/*===========================================================================
FUNCTION PS_QOS_NET_INITIATED_CHECK_ONE_REQUEST()

DESCRIPTION
  This function receives filters specification of Network Initiated QoS
  and checks if there is any corresponding application request, i.e.
  if any application registered a request to be notified when QoS with such
  filters specification is established by the network.

PARAMETERS
  ps_qos_net_initiated_req : pointer to an application request object
  qos_spec_ptr             : pointer to filters specifications of the
                             Network Initiated QoS

RETURN VALUE
   0 : on success
  -1 : on failure

DEPENDENCIES
  This function is called while PS global lock is locked

SIDE EFFECTS
  None
===========================================================================*/
int32 ps_qos_net_initiated_check_one_request
(
  ps_qos_net_initiated_req_type    * ps_qos_net_initiated_req,
  qos_spec_type                    * qos_spec_ptr
)
{
  ps_qos_net_initiated_req_filter_type * filter_buf_ptr;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  LOG_MSG_INFO1_2("ps_qos_net_initiated_check_one_request(): "
                  "App NIQ req obj 0x%p NIQ obj 0x%p",
                  ps_qos_net_initiated_req, qos_spec_ptr);

  /*-------------------------------------------------------------------------
    skipping parameters validation as this is already done in calling
    function
  -------------------------------------------------------------------------*/

  /*-------------------------------------------------------------------------
    Loop over all tx filters specified by application.
    For each one we look for a matching filter in the provided qos
    specification of the flow.
    All tx filters specified by application must have matching filters
    in the qos spec of the flow. Otherwise there is no match.
    qos spec of the flow may have additional filters not specified by the app.
    In such case the qos spec of the flow is a subset of the spec specified 
    by the application so the application should be interested to use that 
    flow.
  -------------------------------------------------------------------------*/
  
  filter_buf_ptr = list_peek_front(
    &(ps_qos_net_initiated_req->qos_net_initiated_req_private.qos_info_ptr->tx.fltr_list)
  );
  while (filter_buf_ptr != NULL)
  {
    if ( -1 ==  ps_qos_net_initiated_check_one_filter(&filter_buf_ptr->filter,
                                                      &qos_spec_ptr->tx.fltr_template) )
    {
       /* Did not find any filter in the qos_spec of the new flow that matches
          this filter that the application specified for network initiated QOS
          notification => The new flow does not match application's
          specification */          
       LOG_MSG_INFO1_2("ps_qos_net_initiated_check_one_request(): "
                       "App NIQ req TX Filter 0x%p doesn't match new NIQ Spec 0x%p",
                       filter_buf_ptr, qos_spec_ptr);
       return -1;       
    }

    filter_buf_ptr = list_peek_next( 
      &(ps_qos_net_initiated_req->qos_net_initiated_req_private.qos_info_ptr->tx.fltr_list),
      &(filter_buf_ptr->link) 
    );
  }    

  /*-------------------------------------------------------------------------
    Loop over all rx filters specified by application.
  -------------------------------------------------------------------------*/
  
  filter_buf_ptr = list_peek_front(
    &(ps_qos_net_initiated_req->qos_net_initiated_req_private.qos_info_ptr->rx.fltr_list)
  );
  while (filter_buf_ptr != NULL)
  {
    if ( -1 == ps_qos_net_initiated_check_one_filter(&filter_buf_ptr->filter,
                                                     &qos_spec_ptr->rx.fltr_template) )
    {
       /* Did not find any filter in the qos_spec of the new flow that matches
          this filter that the application specified for network initiated QOS
          notification => The new flow does not match application's
          specification */          
       LOG_MSG_INFO1_2("ps_qos_net_initiated_check_one_request(): "
                       "App NIQ req RX Filter 0x%p doesn't match new NIQ Spec 0x%p",
                       filter_buf_ptr, qos_spec_ptr);
       return -1;       
    }

    filter_buf_ptr = list_peek_next( 
      &(ps_qos_net_initiated_req->qos_net_initiated_req_private.qos_info_ptr->rx.fltr_list),
      &(filter_buf_ptr->link) 
    );
  }    

  /* We went over all filters specified by the application and for each one we
     found a matching filter in the new flow's QoS Specification */
  
  LOG_MSG_INFO1_0("ps_qos_net_initiated_check_one_request(): "
                  "SUCCESS");
  return 0;

} /* ps_qos_net_initiated_check_one_request() */

/*===========================================================================
FUNCTION PS_QOS_NET_INITIATED_CHECK_FLOW()

DESCRIPTION
  Check if a specific flow can fulfill a request's requirements

PARAMETERS
  flow_ptr                   : pointer to the flow
  net_initiated_qos_spec_ptr : pointer to the request

RETURN VALUE
   0 : on success
  -1 : on failure

DEPENDENCIES
  This function is called while PS global lock is locked

SIDE EFFECTS
  None
===========================================================================*/
int32 ps_qos_net_initiated_check_flow
(
  ps_flow_type                * flow_ptr,
  net_initiated_qos_spec_type * net_initiated_qos_spec_ptr
)
{
  ip_filter_spec_type rx_filter_spec, tx_filter_spec;
  ip_filter_type * rx_ip_filter_ptr = NULL, * tx_ip_filter_ptr = NULL;
  boolean                result = FALSE;
  int                    num_rx_filters = 0;
  int                    num_tx_filters = -1;
  int16                  ps_errno;
  int                    i;
  int32                  retval = -1;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  LOG_MSG_INFO1_0("ps_qos_net_initiated_check_flow()");
  
  /* skipping parameters validation as this is already done in calling
     function */

  memset(&rx_filter_spec,0,sizeof(ip_filter_spec_type));
  memset(&tx_filter_spec,0,sizeof(ip_filter_spec_type));
  /*-------------------------------------------------------------------------
    Generate rx_filter_spec.
  -------------------------------------------------------------------------*/
  num_rx_filters = 
    ps_flow_get_rx_fltr_cnt(flow_ptr, 
                            IP_V4, 
                            FALSE);
  num_rx_filters += 
    ps_flow_get_rx_fltr_cnt(flow_ptr, 
                            IP_V6, 
                            FALSE);
  if(0 != num_rx_filters)
  {
    PS_SYSTEM_HEAP_MEM_ALLOC(rx_filter_spec.list_ptr, num_rx_filters * sizeof(ip_filter_type),
                             ip_filter_type*);
    if(NULL == rx_filter_spec.list_ptr)
    {
      retval = -1;
      goto bail;
    }

    /* pre-condition for the following function */
    rx_filter_spec.num_filters = 0;

    result = ps_flow_get_rx_fltr_spec
      (
        flow_ptr->flow_private.iface_ptr,
        flow_ptr,
        &rx_filter_spec,
        FALSE,
        num_rx_filters,
        &ps_errno
      );
    if(FALSE == result)
    {
      LOG_MSG_ERROR_2("get filter spec failed, ps_flow 0x%p, result %d",
                      flow_ptr, result);
      retval = -1;
      goto bail;
    }
  /*-------------------------------------------------------------------------
    Loop over all rx filters specified by flow.
  -------------------------------------------------------------------------*/
    for (i = 0; i < net_initiated_qos_spec_ptr->rx_fltr_template.num_filters; i++)
    {
    rx_ip_filter_ptr = &(net_initiated_qos_spec_ptr->rx_fltr_template.list_ptr[i]);
    if ( -1 == ps_qos_net_initiated_check_one_filter(rx_ip_filter_ptr,
                                                     &rx_filter_spec) )
    {
      /* Did not find any filter in the qos_spec of the flow that matches
         this filter that the application specified for network initiated QOS
         notification => The flow does not match application's
         specification */
        LOG_MSG_INFO1_1("ps_qos_net_initiated_check_flow : "
                        "RX Filter 0x%p in NIQ application request does not match new flow's QoS Spec",
                        rx_ip_filter_ptr);
      retval = -1;
      goto bail;
    }
  }
  }
  else
  {
    LOG_MSG_INFO1_1("ps_qos_net_initiated_check_flow() "
                    "Zero RX filters, ps_flow 0x%p", flow_ptr);
    if (0 < net_initiated_qos_spec_ptr->rx_fltr_template.num_filters) {
      retval = -1;
      goto bail;
    }
  }

  /*-------------------------------------------------------------------------
    Generate tx_filter_spec.
  -------------------------------------------------------------------------*/
  num_tx_filters = 
    ps_iface_flow_get_tx_fltr_cnt_ex(flow_ptr->flow_private.iface_ptr, 
                                     flow_ptr, 
                                     FALSE);
  if(0 != num_tx_filters)
  {
    PS_SYSTEM_HEAP_MEM_ALLOC( tx_filter_spec.list_ptr,
                              num_tx_filters * sizeof(ip_filter_type),
                              ip_filter_type*);
    if(NULL == tx_filter_spec.list_ptr)
    {
      retval = -1;
      goto bail;
    }

    /* pre-condition for the following function */
    tx_filter_spec.num_filters = 0;
    
    result = ps_iface_flow_get_tx_fltr_spec(flow_ptr->flow_private.iface_ptr,
        flow_ptr,
        &tx_filter_spec,
        FALSE,
        num_tx_filters,
                                            &ps_errno);
    if(FALSE == result)
    {
      LOG_MSG_ERROR_2("get filter spec failed, ps_flow 0x%p, result %d",
                      flow_ptr, result);
      retval = -1;
      goto bail;
    }
  /*-------------------------------------------------------------------------
    Loop over all tx filters specified by flow.
  -------------------------------------------------------------------------*/
    for (i = 0; i < net_initiated_qos_spec_ptr->tx_fltr_template.num_filters; i++)
    {
    tx_ip_filter_ptr = &(net_initiated_qos_spec_ptr->tx_fltr_template.list_ptr[i]);
    if ( -1 == ps_qos_net_initiated_check_one_filter(tx_ip_filter_ptr,
                                                     &tx_filter_spec) )
    {
      /* Did not find any filter in the qos_spec of the flow that matches
         this filter that the application specified for network initiated QOS
         notification => The flow does not match application's
         specification */
        LOG_MSG_INFO1_1("ps_qos_net_initiated_check_flow(): "
                        "TX Filter 0x%p in NIQ application request does not match new flow's QoS Spec",
                        tx_ip_filter_ptr);
      retval = -1;
      goto bail;
    }
  }
  }
  else
  {
    LOG_MSG_INFO1_1("ps_qos_net_initiated_check_flow(): "
                    "Zero RX filters, ps_flow 0x%p", flow_ptr);
    if (0 < net_initiated_qos_spec_ptr->tx_fltr_template.num_filters) {
      retval = -1;
      goto bail;
    }
  }


  /* We went over all filters specified by the application and for each one we
     found a matching filter in the new flow's QoS Specification */
  retval = 0;

bail:
  if (0 != num_rx_filters) {
    PS_SYSTEM_HEAP_MEM_FREE(rx_filter_spec.list_ptr);
  }
  if (0 != num_tx_filters) {
    PS_SYSTEM_HEAP_MEM_FREE(tx_filter_spec.list_ptr);
  }
  LOG_MSG_INFO1_4("ps_qos_net_initiated_check_flow(): "
                  "Flow filters rx:%d, tx:%d, App filters rx:%d, tx:%d",
                  num_rx_filters,
                  num_tx_filters,
                  net_initiated_qos_spec_ptr->rx_fltr_template.num_filters,
                  net_initiated_qos_spec_ptr->tx_fltr_template.num_filters);
  return retval;

} /* ps_qos_net_initiated_check_flow() */

/*===========================================================================
FUNCTION ps_qos_net_initiated_check_port_range

DESCRIPTION
  This function checks if specified port and range of modem filter matches
  a specified port and range of application request.

PARAMETERS
  modem_port  - Modem filter port number
  modem_range - Modem filter port range
  app_port    - Application filter port number
  app_range   - Application filter port range
  
DEPENDENCIES
  None.

RETURN VALUE
  TRUE for a successful match
  FALSE for no match

SIDE EFFECTS
  None.

===========================================================================*/
boolean ps_qos_net_initiated_check_port_range
(
  uint16 modem_port,
  uint16 modem_range,
  uint16 app_port,
  uint16 app_range
)
{
  LOG_MSG_INFO1_2("ps_qos_net_initiated_check_port_range() "
                  "modem_port: %d, modem_range: %d",
                  (int)modem_port, (int)modem_range);
  LOG_MSG_INFO1_2("ps_qos_net_initiated_check_port_range() "
                  "app_port: %d, app_range: %d",
                  (int)app_port, (int)app_range);

  if (modem_port > app_port)
  {
    LOG_MSG_INFO1_0("ps_qos_net_initiated_check_port_range() "
                    "modem_port > app_port");
    return FALSE;
  }

  if (modem_port + modem_range < app_port + app_range)
  {
    LOG_MSG_INFO1_0("ps_qos_net_initiated_check_port_range()" 
                    "modem_port + modem_range < app_port + app_range");
    return FALSE;
  }

  return TRUE;
}

/*===========================================================================
FUNCTION ps_qos_net_initiated_match_fltr

DESCRIPTION
  This function matches a specified filter with another specified filter.

PARAMETERS
  fltr_ptr1 - Filter 1 which needs to be matched. This filter spec cannot have a range of UDP/TCP ports
  fltr_ptr2 - Filter 2 against the filter1 is matched. This filter spec may have a range of UDP/TCP ports
                
DEPENDENCIES
  - Filters must have the same IP version.

RETURN VALUE
  TRUE for a successful match
  FALSE for no match

SIDE EFFECTS
  None.

===========================================================================*/
boolean ps_qos_net_initiated_match_fltr
(
  ip_filter_type *fltr_ptr1,
  ip_filter_type *fltr_ptr2
)
{
  boolean result = FALSE;
  uint32 tmp_mask;
  uint8 next_hdr_prot = 0;
  uint8 tmp_prot;

  LOG_MSG_INFO1_2("ps_qos_net_initiated_match_fltr(): "
                  "trying to match filter 0x%x with 0x%x",
                  fltr_ptr1, fltr_ptr2);
  if(fltr_ptr1->ip_vsn != fltr_ptr2->ip_vsn)
  {
      LOG_MSG_INFO1_2("ps_qos_net_initiated_match_fltr(): "
                      "fltr_ptr1->ip_vsn != fltr_ptr2->ip_vsn. 0x%x , 0x%x",
                      fltr_ptr1->ip_vsn, fltr_ptr2->ip_vsn);
      return result;
  }

  switch(fltr_ptr1->ip_vsn)
  {
    case IP_V4:
    {
        tmp_mask = fltr_ptr1->ip_hdr.v4.field_mask;

        while(tmp_mask)
        {
          if(tmp_mask & IPFLTR_MASK_IP4_SRC_ADDR)
          {
            if((fltr_ptr1->ip_hdr.v4.src.addr.ps_s_addr &
                fltr_ptr1->ip_hdr.v4.src.subnet_mask.ps_s_addr) !=
            (fltr_ptr2->ip_hdr.v4.src.addr.ps_s_addr &
                fltr_ptr2->ip_hdr.v4.src.subnet_mask.ps_s_addr))
            {
              LOG_MSG_INFO1_2("ps_qos_net_initiated_match_fltr(): "
                              "IPv4 :: Fltr1 SRC mask 0x%x and subnetmask 0x%x",
                              fltr_ptr1->ip_hdr.v4.src.addr.ps_s_addr,
                              fltr_ptr1->ip_hdr.v4.src.subnet_mask.ps_s_addr);
              LOG_MSG_INFO1_2("ps_qos_net_initiated_match_fltr(): "
                              "IPv4 :: Fltr1 SRC mask 0x%x and subnetmask 0x%x",
                              fltr_ptr2->ip_hdr.v4.src.addr.ps_s_addr,
                              fltr_ptr2->ip_hdr.v4.src.subnet_mask.ps_s_addr);			
              return FALSE;
            }

            tmp_mask &= ~IPFLTR_MASK_IP4_SRC_ADDR;
            continue;
          }

          if(tmp_mask & IPFLTR_MASK_IP4_DST_ADDR)
          {
            if((fltr_ptr2->ip_hdr.v4.field_mask & IPFLTR_MASK_IP4_DST_ADDR)
                   && (fltr_ptr1->ip_hdr.v4.dst.addr.ps_s_addr &
                fltr_ptr1->ip_hdr.v4.dst.subnet_mask.ps_s_addr) !=
            (fltr_ptr2->ip_hdr.v4.dst.addr.ps_s_addr &
                fltr_ptr2->ip_hdr.v4.dst.subnet_mask.ps_s_addr))
            {
              LOG_MSG_INFO1_0("ps_qos_net_initiated_match_fltr(): "
                              "IPv4 :: DST mask and subnetmask don't match");
              return FALSE;
            }

            tmp_mask &= ~IPFLTR_MASK_IP4_DST_ADDR;
            continue;
          }

          if(tmp_mask & IPFLTR_MASK_IP4_NEXT_HDR_PROT)
          {
            if (PS_IPPROTO_TCP_UDP == fltr_ptr1->ip_hdr.v4.next_hdr_prot)
            {
              if ( PS_IPPROTO_TCP != fltr_ptr2->ip_hdr.v4.next_hdr_prot &&
                   PS_IPPROTO_UDP != fltr_ptr2->ip_hdr.v4.next_hdr_prot )
              {
                LOG_MSG_INFO1_2("ps_qos_net_initiated_match_fltr(): "
                                "Application filter is neither TCP or UDP, however NIQ protocol is TCP_UDP"
                                "0x%x, 0x%x",
                                fltr_ptr1->ip_hdr.v4.next_hdr_prot,
                                fltr_ptr2->ip_hdr.v4.next_hdr_prot);
                return FALSE;
              }
            }
            else if (fltr_ptr1->ip_hdr.v4.next_hdr_prot != fltr_ptr2->ip_hdr.v4.next_hdr_prot)
            {
              LOG_MSG_INFO1_2("ps_qos_net_initiated_match_fltr(): "
                              "fltr_ptr1->ip_hdr.v4.next_hdr_prot != fltr_ptr2->ip_hdr.v4.next_hdr_prot "
                              "0x%x, 0x%x",
                              fltr_ptr1->ip_hdr.v4.next_hdr_prot,
                              fltr_ptr2->ip_hdr.v4.next_hdr_prot);
              return FALSE;
            }

            next_hdr_prot = fltr_ptr1->ip_hdr.v4.next_hdr_prot;

            tmp_mask &= ~IPFLTR_MASK_IP4_NEXT_HDR_PROT;
            continue;
          }

          if(tmp_mask & IPFLTR_MASK_IP4_TOS)
          {
            if((fltr_ptr1->ip_hdr.v4.tos.val & fltr_ptr1->ip_hdr.v4.tos.mask) !=
               (fltr_ptr2->ip_hdr.v4.tos.val & fltr_ptr2->ip_hdr.v4.tos.mask))
            {
              LOG_MSG_INFO1_2("ps_qos_net_initiated_match_fltr(): "
                              "IPv4 :: TOS values don't match 0x%x, 0x%x",
                              fltr_ptr1->ip_hdr.v4.tos.val,
                              fltr_ptr2->ip_hdr.v4.tos.val);
              return FALSE;
            }

            tmp_mask &= ~IPFLTR_MASK_IP4_TOS;
            continue;
          }

          if (0 != tmp_mask)
          {
            LOG_MSG_INFO1_1("ps_qos_net_initiated_match_fltr(): "
                            "Unknown v4 filter param in mask 0x%x", tmp_mask);
          }
        }
    }
    break;

#ifdef FEATURE_DATA_PS_IPV6
    case IP_V6:
    {
      tmp_mask = fltr_ptr1->ip_hdr.v6.field_mask;
      while(tmp_mask)
      {
        if (tmp_mask & IPFLTR_MASK_IP6_SRC_ADDR)
        {
          if (FALSE == IN6_ARE_PREFIX_EQUAL(
                         &fltr_ptr1->ip_hdr.v6.src.addr,
                         &fltr_ptr2->ip_hdr.v6.src.addr,
                         fltr_ptr1->ip_hdr.v6.src.prefix_len  ))
          {
            LOG_MSG_INFO1_2("ps_qos_net_initiated_match_fltr(): "            
                            "IN6_ARE_PREFIX_EQUAL :: SRC prefix. len 1: 0x%x len 2: 0x%x",
                            fltr_ptr1->ip_hdr.v6.src.prefix_len,
                            fltr_ptr2->ip_hdr.v6.src.prefix_len);
            return FALSE;
          }

          tmp_mask &= ~IPFLTR_MASK_IP6_SRC_ADDR;
          continue;
        }

        if (tmp_mask & IPFLTR_MASK_IP6_DST_ADDR)
        {
          if ((fltr_ptr2->ip_hdr.v6.field_mask & IPFLTR_MASK_IP6_DST_ADDR)
                 && (FALSE == IN6_ARE_PREFIX_EQUAL(
                         &fltr_ptr1->ip_hdr.v6.dst.addr,
                         &fltr_ptr2->ip_hdr.v6.dst.addr,
                         fltr_ptr1->ip_hdr.v6.dst.prefix_len  )))
          {
            LOG_MSG_INFO1_2("ps_qos_net_initiated_match_fltr(): "              
                            "IN6_ARE_PREFIX_EQUAL :: DST prefix. len 1: 0x%x len2: 0x%x",
                            fltr_ptr1->ip_hdr.v6.dst.prefix_len,
                            fltr_ptr2->ip_hdr.v6.dst.prefix_len);
            return FALSE;
          }

          tmp_mask &= ~IPFLTR_MASK_IP6_DST_ADDR;
          continue;
        }

        /* Next hdr for IPv6 is transport protocol type */
        if (tmp_mask & IPFLTR_MASK_IP6_NEXT_HDR_PROT)
        {
          if (PS_IPPROTO_TCP_UDP == fltr_ptr1->ip_hdr.v6.next_hdr_prot)
          {
            if ( PS_IPPROTO_TCP != fltr_ptr2->ip_hdr.v6.next_hdr_prot &&
                 PS_IPPROTO_UDP != fltr_ptr2->ip_hdr.v6.next_hdr_prot )
            {
              LOG_MSG_INFO1_2("ps_qos_net_initiated_match_fltr(): "
                              "Application filter is neither TCP or UDP on v6, "
                              "however NIQ protocol is TCP_UDP on v6"
                              "0x%x, 0x%x",
                              fltr_ptr1->ip_hdr.v6.next_hdr_prot,
                              fltr_ptr2->ip_hdr.v6.next_hdr_prot);
              return FALSE;
            }
          }
          else if (fltr_ptr1->ip_hdr.v6.next_hdr_prot !=
                   fltr_ptr2->ip_hdr.v6.next_hdr_prot)
          {
            LOG_MSG_INFO1_2("ps_qos_net_initiated_match_fltr(): "
                            "IPv6 ext hdr mismatch 0x%x  0x%x",
                            fltr_ptr1->ip_hdr.v6.next_hdr_prot,
                            fltr_ptr2->ip_hdr.v6.next_hdr_prot);
            return FALSE;
          }

          next_hdr_prot = fltr_ptr1->ip_hdr.v6.next_hdr_prot;
          tmp_mask &= ~IPFLTR_MASK_IP6_NEXT_HDR_PROT;
          continue;
        }

        if (tmp_mask & IPFLTR_MASK_IP6_TRAFFIC_CLASS)
        {
          if ((fltr_ptr1->ip_hdr.v6.trf_cls.val &
                 fltr_ptr1->ip_hdr.v6.trf_cls.mask) !=
              (fltr_ptr2->ip_hdr.v6.trf_cls.val &
                 fltr_ptr2->ip_hdr.v6.trf_cls.mask))
          {
            LOG_MSG_INFO1_2("ps_qos_net_initiated_match_fltr(): "
                            "TRF mismatch :: 0x%x , 0x%x ",
                            fltr_ptr1->ip_hdr.v6.trf_cls.val,
                            fltr_ptr2->ip_hdr.v6.trf_cls.val);
            return FALSE;
          }

          tmp_mask &= ~IPFLTR_MASK_IP6_TRAFFIC_CLASS;
          continue;
        }

        if (tmp_mask & IPFLTR_MASK_IP6_FLOW_LABEL)
        {
          if (fltr_ptr1->ip_hdr.v6.flow_label !=
               fltr_ptr2->ip_hdr.v6.flow_label)
          {
            LOG_MSG_INFO1_2("ps_qos_net_initiated_match_fltr(): "
                            "FLOW label mismatch :: 0x%x , 0x%x ",
                            fltr_ptr1->ip_hdr.v6.flow_label,
                            fltr_ptr2->ip_hdr.v6.flow_label);
            return FALSE;
          }

          tmp_mask &= ~IPFLTR_MASK_IP6_FLOW_LABEL;
          continue;
        }

        if (0 != tmp_mask)
        {
          LOG_MSG_ERROR_1("ps_qos_net_initiated_match_fltr(): "
                          "Unknown v6 filter param in mask 0x%x", tmp_mask);
        }
      }
    }
    break;
#endif /* FEATURE_DATA_PS_IPV6 */

    default:
      /* unsupported IP version */
      LOG_MSG_ERROR_1("Unsupported IP Version %d", fltr_ptr1->ip_vsn);
      ASSERT(0);
      return FALSE;
  }

  if(next_hdr_prot == 0)
  {
    LOG_MSG_INFO1_0("ps_qos_net_initiated_match_fltr(): "
                    "next_hdr_prot == 0, Filters Match");
    return TRUE;
  }

  /*-------------------------------------------------------------------------
    Optimization done for IP hdrs above may also be used for protocol hdr
    processing when more than 2 parameters can be specified for a protocol.
    For 2 or less parameters the scheme is less optimal and hence avoided.
  -------------------------------------------------------------------------*/
  switch(next_hdr_prot)
  {
    case PS_IPPROTO_TCP:
    {
      if (0 == (tmp_mask = fltr_ptr1->next_prot_hdr.tcp.field_mask))
      {
        break;
      }

      if((tmp_mask & IPFLTR_MASK_TCP_SRC_PORT) &&
         FALSE == ps_qos_net_initiated_check_port_range (
          ps_ntohs(fltr_ptr1->next_prot_hdr.tcp.src.port),
          fltr_ptr1->next_prot_hdr.tcp.src.range,
          ps_ntohs(fltr_ptr2->next_prot_hdr.tcp.src.port),
          fltr_ptr2->next_prot_hdr.tcp.src.range))
      {
        LOG_MSG_INFO1_0("ps_qos_net_initiated_match_fltr(): "
                        "TCP src port/range mismatch");
        return FALSE;
      }

      if((tmp_mask & IPFLTR_MASK_TCP_DST_PORT) &&
         (fltr_ptr2->next_prot_hdr.tcp.field_mask & IPFLTR_MASK_TCP_DST_PORT) &&
         FALSE == ps_qos_net_initiated_check_port_range (
          ps_ntohs(fltr_ptr1->next_prot_hdr.tcp.dst.port),
          fltr_ptr1->next_prot_hdr.tcp.dst.range,
          ps_ntohs(fltr_ptr2->next_prot_hdr.tcp.dst.port),
          fltr_ptr2->next_prot_hdr.tcp.dst.range))
      {
        LOG_MSG_INFO1_0("ps_qos_net_initiated_match_fltr(): "        
                        "TCP dst port/range mismatch");
        return FALSE;
      }
    }
    break;

    case PS_IPPROTO_UDP:
    {
      if (0 == (tmp_mask = fltr_ptr1->next_prot_hdr.udp.field_mask))
      {
        break;
      }

      if((tmp_mask & IPFLTR_MASK_UDP_SRC_PORT) &&
         FALSE == ps_qos_net_initiated_check_port_range (
           ps_ntohs(fltr_ptr1->next_prot_hdr.udp.src.port),
           fltr_ptr1->next_prot_hdr.udp.src.range,
           ps_ntohs(fltr_ptr2->next_prot_hdr.udp.src.port),
           fltr_ptr2->next_prot_hdr.udp.src.range))
      {
        LOG_MSG_INFO1_0("ps_qos_net_initiated_match_fltr(): "
                        "UDP src port/range mismatch");
        return FALSE;
      }

      if((tmp_mask & IPFLTR_MASK_UDP_DST_PORT) &&
           (fltr_ptr2->next_prot_hdr.udp.field_mask & IPFLTR_MASK_UDP_DST_PORT) &&
           FALSE == ps_qos_net_initiated_check_port_range (
             ps_ntohs(fltr_ptr1->next_prot_hdr.udp.dst.port),
             fltr_ptr1->next_prot_hdr.udp.dst.range,
             ps_ntohs(fltr_ptr2->next_prot_hdr.udp.dst.port),
             fltr_ptr2->next_prot_hdr.udp.dst.range))
      {
        LOG_MSG_INFO1_0("ps_qos_net_initiated_match_fltr(): "
                        "UDP dst port/range mismatch");
        return FALSE;
      }
    }
    break;

    case PS_IPPROTO_ICMP:
    case PS_IPPROTO_ICMP6:
    {
      if (0 == (tmp_mask = fltr_ptr1->next_prot_hdr.icmp.field_mask))
      {
        break;
      }

      if(tmp_mask & IPFLTR_MASK_ICMP_MSG_TYPE &&
         fltr_ptr1->next_prot_hdr.icmp.type != fltr_ptr2->next_prot_hdr.icmp.type)
      {
        LOG_MSG_INFO1_2("ps_qos_net_initiated_match_fltr(): "
                        "ICMP MSG TYPE mismatch 0x%x , 0x%x",
                        fltr_ptr1->next_prot_hdr.icmp.type,
                        fltr_ptr2->next_prot_hdr.icmp.type);
        return FALSE;
      }

      if(tmp_mask & IPFLTR_MASK_ICMP_MSG_CODE &&
         fltr_ptr1->next_prot_hdr.icmp.code != fltr_ptr2->next_prot_hdr.icmp.code)
      {
        LOG_MSG_INFO1_2("ps_qos_net_initiated_match_fltr(): "        
                        "ICMP MSG CODE mismatch 0x%x , 0x%x",
                        fltr_ptr1->next_prot_hdr.icmp.code,
                        fltr_ptr2->next_prot_hdr.icmp.code);
        return FALSE;
      }
    }
    break;

    case PS_IPPROTO_ESP:
    {
      if (0 == (tmp_mask = fltr_ptr1->next_prot_hdr.esp.field_mask))
      {
        break;
      }
      
      if(tmp_mask & IPFLTR_MASK_ESP_SPI &&
         fltr_ptr1->next_prot_hdr.esp.spi != fltr_ptr2->next_prot_hdr.esp.spi)
      {
        LOG_MSG_INFO1_2("ps_qos_net_initiated_match_fltr(): "
                        "ESP SPI mismatch 0x%x  0x%x",
                        fltr_ptr1->next_prot_hdr.esp.spi,
                        fltr_ptr2->next_prot_hdr.esp.spi);
        return FALSE;
      }
    }
    break;

    case PS_IPPROTO_AH:
    {
      if (0 == (tmp_mask = fltr_ptr1->next_prot_hdr.ah.field_mask))
      {
        break;
      }
      
      if(tmp_mask & IPFLTR_MASK_AH_SPI &&
         fltr_ptr1->next_prot_hdr.ah.spi != fltr_ptr2->next_prot_hdr.ah.spi)
      {
        LOG_MSG_INFO1_2("ps_qos_net_initiated_match_fltr(): "
                        "AH SPI mismatch 0x%x  0x%x",
                        fltr_ptr1->next_prot_hdr.ah.spi,
                        fltr_ptr2->next_prot_hdr.ah.spi);
        return FALSE;
      }
    }
    break;

    case PS_IPPROTO_TCP_UDP:
    {
      if (0 == (tmp_mask = fltr_ptr1->next_prot_hdr.tcp_udp_port_range.field_mask))
      {
        break;
      }

      if (fltr_ptr1->ip_vsn == IP_V4)
      {
        tmp_prot = fltr_ptr2->ip_hdr.v4.next_hdr_prot;
      }
      else
      {
        tmp_prot = fltr_ptr2->ip_hdr.v6.next_hdr_prot;
      }

      switch (tmp_prot)
      {
        case PS_IPPROTO_TCP:
        case PS_IPPROTO_UDP:
          if((tmp_mask & IPFLTR_MASK_TCP_UDP_SRC_PORT) &&
               FALSE == ps_qos_net_initiated_check_port_range (
                 ps_ntohs(fltr_ptr1->next_prot_hdr.tcp_udp_port_range.src.port),
                 fltr_ptr1->next_prot_hdr.tcp_udp_port_range.src.range,
                 ps_ntohs(fltr_ptr2->next_prot_hdr.tcp_udp_port_range.src.port),
                 fltr_ptr2->next_prot_hdr.tcp_udp_port_range.src.range))
          {
            LOG_MSG_INFO1_0("ps_qos_net_initiated_match_fltr(): "
                            "TCP_UDP src port/range mismatch");
            return FALSE;
          }

          if ((tmp_mask & IPFLTR_MASK_TCP_UDP_DST_PORT) &&
              (fltr_ptr2->next_prot_hdr.tcp_udp_port_range.field_mask & IPFLTR_MASK_TCP_UDP_DST_PORT) &&
              FALSE == ps_qos_net_initiated_check_port_range (
                ps_ntohs(fltr_ptr1->next_prot_hdr.tcp_udp_port_range.dst.port),
                fltr_ptr1->next_prot_hdr.tcp_udp_port_range.dst.range,
                ps_ntohs(fltr_ptr2->next_prot_hdr.tcp_udp_port_range.dst.port),
                fltr_ptr2->next_prot_hdr.tcp_udp_port_range.dst.range))
          {
            LOG_MSG_INFO1_0("ps_qos_net_initiated_match_fltr(): "
                            "TCP_UDP dst port/range mismatch");
            return FALSE;
          }
          break;

        default:
          break;
      }
    }
    break;

    default:
      /* unsupported protocol */
      LOG_MSG_ERROR_1("Unsupported protocol %d", next_hdr_prot);
      ASSERT(0);
      return FALSE;
  }

  /*-------------------------------------------------------------------------
    If we reach here, all the specified parameters matched
  -------------------------------------------------------------------------*/
  LOG_MSG_INFO1_0("ps_qos_net_initiated_match_fltr(): "
                  "Filters Match");
  return TRUE;
} /*ps_qos_net_initiated_match_fltr*/

/*===========================================================================
FUNCTION PS_QOS_NET_INITIATED_CHECK_ONE_FILTER()

DESCRIPTION
  This function receives one filter specification of Network Initiated QoS
  and checks if it matches a filter from an application request.

PARAMETERS
  app_specified_filter : pointer to a filter specified by application
  filter_spec_of_flow  : pointer to one of the filters of Network Initiated QoS

RETURN VALUE
   0 : on success
  -1 : on failure

DEPENDENCIES
  This function is called while PS global lock is locked

SIDE EFFECTS
  None
===========================================================================*/
int32 ps_qos_net_initiated_check_one_filter
(  
  ip_filter_type                   * app_specified_filter,
  ip_filter_spec_type              * filter_spec_of_flow
)
{
  int                              index;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  LOG_MSG_INFO1_0("ps_qos_net_initiated_check_one_filter()");

  /* skipping parameters validation as this is already done in calling
     function */

  /*-------------------------------------------------------------------------
    Loop over list of filters. 
    For each one check if it matches the given app specified filter.
  -------------------------------------------------------------------------*/
  
   for (index = 0; index < filter_spec_of_flow->num_filters; index++)
      
   {
      if ( TRUE == ps_qos_net_initiated_match_fltr(
                     filter_spec_of_flow->list_ptr + index,
                     app_specified_filter
                   ) )
      {
         /* Match found */
         LOG_MSG_INFO1_2("ps_qos_net_initiated_check_one_filter(): "
                         "Match: App NIQ req filter obj 0x%p matched NIQ filter obj 0x%p",
                         app_specified_filter,
                         filter_spec_of_flow->list_ptr + index);
         return 0;
      }
   }
  /* We went over a list of NIQ filters but
     none of them matched the application NIQ request filter. */
   LOG_MSG_INFO1_1("ps_qos_net_initiated_check_one_filter(): "
                   "No Match: App NIQ req filter obj 0x%p",
                   app_specified_filter);
   return -1;       
} /* ps_qos_net_initiated_check_one_filter() */

/*===========================================================================
FUNCTION PS_QOS_CHECK_ONE_FILTER()

DESCRIPTION
  This function receives one filter specification of Network Initiated QoS
  and checks if it matches a filter from an application request.

PARAMETERS
  app_specified_filter : pointer to a filter specified by application
  filter_spec_of_flow  : pointer to one of the filters of Network Initiated QoS
  ps_fltr_direction_enum_type  : Uplink or downlink direction

RETURN VALUE
   0 : on success
  -1 : on failure

DEPENDENCIES
  This function is called while PS global lock is locked

SIDE EFFECTS
  None
===========================================================================*/
int32 ps_qos_check_one_filter
(  
  ip_filter_type                               * app_specified_filter,
  ps_iface_ioctl_filter_match_request_type     * filter_info,
  ps_fltr_direction_enum_type                    direction
)
{
  int                              index;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  LOG_MSG_INFO1_1("ps_qos_check_one_filter() direction %d", direction);

  /* skipping parameters validation as this is already done in calling
     function */

  /*-------------------------------------------------------------------------
    Loop over list of filters. 
    For each one check if it matches the given app specified filter.
  -------------------------------------------------------------------------*/
  
   for (index = 0; index < filter_info->num_filters; index++)
      
   {
      if(direction != (filter_info->list_fitr_ptr + index)->direction)
      {
        LOG_MSG_INFO1_0("ps_qos_check_one_filter() direction no match");
        continue;
      }
      if ( TRUE == ps_qos_net_initiated_match_fltr(                     
                     app_specified_filter,
                     &((filter_info->list_fitr_ptr + index)->filtr)
                   ) )
      {
         /* Match found */
         LOG_MSG_INFO1_2("ps_qos_check_one_filter(): "
                         "Match: App NIQ req filter obj 0x%p matched NIQ filter obj 0x%p",
                         app_specified_filter,
                         filter_info->list_fitr_ptr + index);
	 (filter_info->list_fitr_ptr + index)->is_match = TRUE;
      }
   }
   return 0;
} /* ps_qos_net_initiated_check_one_filter() */


int ps_iface_check_fltr_match
(
  ps_iface_type                            * iface_ptr,
  ps_iface_filter_match_info               * filtr_match_info
)
{

  ps_iface_ioctl_filter_match_request_type * filtr_info_ptr = filtr_match_info->fltr_info_ptr;
  ip_filter_type filter_buf;
  ps_flow_type * flow_ptr = NULL;
  ps_iface_ipfilteri_type              * tmp_tx_fltr_buf = NULL;
  q_type                               * ipfltr_q_ptr = NULL;
  ps_iface_ipfltr_handle_type          fltr_handle;
  qos_info_type 		               * qos_info_ptr;
  ps_flow_rx_fltr_buf_type	           * tmp_rx_fltr_buf;
  
  PS_ENTER_CRIT_SECTION(&global_ps_crit_section);
  flow_ptr = (ps_flow_type *)list_peek_front(&(iface_ptr->iface_private.flow.sec_flow_list));
  while((NULL != flow_ptr) && (flow_ptr != (ps_flow_type *)filtr_match_info->flow_handle))
  {
    flow_ptr = (ps_flow_type *)list_peek_next(&(iface_ptr->iface_private.flow.sec_flow_list), &(flow_ptr->link) );
  }

  
  ipfltr_q_ptr = &(iface_ptr->iface_private.ipfltr_info[IP_FLTR_CLIENT_QOS_OUTPUT]);
  
  tmp_tx_fltr_buf = q_check(ipfltr_q_ptr);

  if(NULL == flow_ptr)
  {
    PS_LEAVE_CRIT_SECTION(&global_ps_crit_section);
    return 0;
  }
  
  if(NULL != flow_ptr)
  {
    fltr_handle = flow_ptr->flow_private.qos_info_ptr->tx.fltr_handle;
    
    while (NULL != tmp_tx_fltr_buf)
    {
      if(tmp_tx_fltr_buf->fi_handle == fltr_handle)
      {
         filter_buf = tmp_tx_fltr_buf->filter.fi_default;
         if ( -1 ==  ps_qos_check_one_filter(&filter_buf,
                                                   filtr_info_ptr, PS_FLTR_DIRECTION_UPLINK) )
        {
          PS_LEAVE_CRIT_SECTION(&global_ps_crit_section);
          return -1;       
        }	
      }
	  tmp_tx_fltr_buf = q_next(ipfltr_q_ptr, &(tmp_tx_fltr_buf->link));
    }
  }

  qos_info_ptr = ps_flowi_get_qos_info_ptr(flow_ptr, FALSE);
  if (qos_info_ptr == NULL)
  {
    PS_BRANCH_LEAVE_CRIT_SECTION(&global_ps_crit_section);
    return 0;
  }
  
  
  tmp_rx_fltr_buf = list_peek_front(&(qos_info_ptr->rx.fltr_list));
  while (tmp_rx_fltr_buf != NULL)
  {
    filter_buf = (tmp_rx_fltr_buf->filter);
    if ( -1 ==  ps_qos_check_one_filter(&filter_buf,
										 filtr_info_ptr, PS_FLTR_DIRECTION_DOWNLINK) )
	{
	  PS_LEAVE_CRIT_SECTION(&global_ps_crit_section);
	  return -1;		 
	}   
	  tmp_rx_fltr_buf =
		list_peek_next(&(qos_info_ptr->rx.fltr_list), &(tmp_rx_fltr_buf->link));
  }
  PS_LEAVE_CRIT_SECTION(&global_ps_crit_section);

  return 0;
  
}/* ps_iface_check_fltr_match() */


#endif /* FEATURE_DATA_PS_QOS */

