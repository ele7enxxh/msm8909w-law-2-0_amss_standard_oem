#ifndef PS_FLOW_IOCTL_H
#define PS_FLOW_IOCTL_H

/*===========================================================================

                          P S _ F L O W _ I O C T L. H

DESCRIPTION

  Defines ps_flow specific IOCTLs and API to use them.

EXTERNAL FUNCTIONS
  PS_FLOW_IOCTL()
    Function which supports ps_flow specific IOCTLs

  PS_FLOW_DEFAULT_LOGICAL_FLOW_IOCTL_HDLR()
    Function which performs the default handling of ioctls for logical flows

Copyright (c) 2005-2011 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE


  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/api/public/ps_flow_ioctl.h#1 $
  $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
08/28/11    aa     Added ioctl PS_FLOW_IOCTL_QOS_GET_GRANTED_FILTER_SPEC
12/28/08    pp     Common Modem Interface: Public/Private API split.
07/18/08    dm     Fixed compiler warnings
09/01/07    ssh    QoS support for logical ifaces
07/31/06    msr    Added support for PRIMARY_QOS_MODIFY
05/01/06    rt     Renamed PS_FLOW_IOCTL_GET_TX_STATUS to
                   PS_FLOW_IOCTL_707_GET_TX_STATUS
05/01/06    msr    Added support for DSS_IFACE_IOCTL_QOS_GET_GRANTED_FLOW_SPEC2
04/30/06    rt     Added code for PS_FLOW_IOCTL_HDR_GET_RMAC3_INFO and
                   PS_FLOW_IOCTL_GET_TX_STATUS.
11/07/05    mct    Change ptr to ptr ptr.
10/12/05    mct    Updated some ioctl naming.
10/04/05    msr    Changed qos in ps_iface_ioctl_qos_modify_type to a pointer
08/09/05    mct    Added QOS_RELEASE_EX, QOS_SUSPEND_EX, and QOS_RESUME_EX.
08/15/05    msr    Renamed PS_FLOW_IOCTL_GET_FLOW_SPEC to
                   PS_FLOW_IOCTL_GET_GRANTED_FLOW_SPEC
08/10/05    msr    Added support for QOS_MODIFY
07/25/05    ks     Changed fields of ps_flow_ioctl_tx_queue_level_type struct
05/12/05    ks     Fixed Lint errors.
05/03/05    msr    Moved flow IOCTLs from ps_iface_ioctl.h to this file
04/18/05    ks     Added ps_flow_ioctl_tx_queue_level_type struct.
04/17/05    msr    created file
===========================================================================*/

#include "comdef.h"

#include "ps_iface_defs.h"
#include "ps_qos_defs.h"
#include "ps_iface_ipfltr.h"


/*=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

                         FLOW IOCTL Definitions

=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/
/*---------------------------------------------------------------------------
  Definition of various ps_flow control operations. Please do NOT change
  the values of the existing enumerated types when adding new ones (to ensure
  that object code released with previous versions of the code do not break).
  Each of the following control operations have associated tyepdef (with
  corresponding names) that are passed in or returned for get/set operations.

  The second most significant bit of the enum value MUST be set for all these
  IOCTLs. This is used as a distinguishing factor between ps_iface, ps_flow
  and phys_link releated IOCTLs.

  Note:
    GET operations are assigned even numbers
    SET operations are assigned odd numbers.

    Also ensure that symetric operations are grouped.
---------------------------------------------------------------------------*/
#define PS_FLOW_IOCTL_MASK           0x40000000

typedef enum
{
  /* Modify an existing QOS    */
  PS_FLOW_IOCTL_QOS_MODIFY                        = 0x40000010,
  /* Release QOS               */
  PS_FLOW_IOCTL_QOS_RELEASE                       = 0x40000011,
  /* Get granted QOS flow spec */
  PS_FLOW_IOCTL_QOS_GET_GRANTED_FLOW_SPEC         = 0x40000012,
  /* Get QOS status            */
  PS_FLOW_IOCTL_QOS_GET_STATUS                    = 0x40000014,
  /* Resume QOS                */
  PS_FLOW_IOCTL_QOS_RESUME                        = 0x40000015,
  /* Suspend QOS               */
  PS_FLOW_IOCTL_QOS_SUSPEND                       = 0x40000016,
  /* Send bundled QOS release  */
  PS_FLOW_IOCTL_QOS_RELEASE_EX                    = 0x40000017,
  /* Send bundled QOS suspend  */
  PS_FLOW_IOCTL_QOS_SUSPEND_EX                    = 0x40000018,
  /* Send bundled QOS resume   */
  PS_FLOW_IOCTL_QOS_RESUME_EX                     = 0x40000019,
  /* Get TX Queue level        */
  PS_FLOW_IOCTL_GET_TX_QUEUE_LEVEL                = 0x40000020,
  /* Get granted QOS flow spec in all flow states */
  PS_FLOW_IOCTL_QOS_GET_GRANTED_FLOW_SPEC2        = 0x40000022,
  /* Modify QoS on default ps_flow */
  PS_FLOW_IOCTL_PRIMARY_QOS_MODIFY                = 0x40000023,
  /* Get granted QOS flow spec for default ps_flow */
  PS_FLOW_IOCTL_PRIMARY_QOS_GET_GRANTED_FLOW_SPEC = 0x40000024,
  /* Get HDR RMAC3 Information */
  PS_FLOW_IOCTL_HDR_GET_RMAC3_INFO                = 0x40000026,
  /* Get the Transmission Status */
  PS_FLOW_IOCTL_707_GET_TX_STATUS                 = 0x40000028,
  /* IOCTLs to set inactivity timer on a ps_flow */
  PS_FLOW_IOCTL_707_GET_INACTIVITY_TIMER          = 0x40000030,
  PS_FLOW_IOCTL_707_SET_INACTIVITY_TIMER          = 0x40000031,

  /* IOCTLs to get the cached event infos */
  PS_FLOW_IOCTL_GET_MODIFY_RESULT                 = 0x40000032,
  PS_FLOW_IOCTL_GET_FLOW_UPDATED_INFO_CODE        = 0x40000033,
  PS_FLOW_IOCTL_GET_PRIMARY_QOS_MODIFY_RESULT     = 0x40000034,
  PS_FLOW_IOCTL_QOS_GET_GRANTED_FILTER_SPEC       = 0x40000035,

} ps_flow_ioctl_type;

/*---------------------------------------------------------------------------
  Argument to be passed with PS_FLOW_IOCTL_QOS_BUNDLE_CHANGE
---------------------------------------------------------------------------*/
typedef struct ps_flow_ioctl_qos_release_ex_s
{
  uint8           num_flows;                        /* Num flows passed    */
  ps_flow_type ** flows_ptr;
} ps_flow_ioctl_qos_release_ex_type;

typedef struct ps_flow_ioctl_qos_release_ex_s ps_flow_ioctl_qos_suspend_ex_type;
typedef struct ps_flow_ioctl_qos_release_ex_s ps_flow_ioctl_qos_resume_ex_type;

/*---------------------------------------------------------------------------
  Argument to be passed with PS_FLOW_IOCTL_QOS_MODIFY
  Fields are
    qos_ptr   : Ptr to QOS specification
    subset_id : Subset ID associated with filters in qos
---------------------------------------------------------------------------*/
typedef struct
{
  qos_spec_type                   * qos_ptr;
  ps_iface_ipfltr_subset_id_type    subset_id;
} ps_flow_ioctl_qos_modify_type;

/*---------------------------------------------------------------------------
  Argument to be passed with PS_FLOW_IOCTL_QOS_RELEASE,
  PS_FLOW_IOCTL_QOS_SUSPEND, or PS_FLOW_IOCTL_QOS_RESUME should be NULL
---------------------------------------------------------------------------*/
typedef void ps_flow_ioctl_qos_release_type;
typedef void ps_flow_ioctl_qos_suspend_type;
typedef void ps_flow_ioctl_qos_resume_type;

/*---------------------------------------------------------------------------
  Argument to be passed with PS_FLOW_IOCTL_PRIMARY_QOS_MODIFY
  Fields are
    primary_qos_spec_ptr : Ptr to QOS specification
---------------------------------------------------------------------------*/
typedef struct
{
  primary_qos_spec_type  * primary_qos_spec_ptr;
} ps_flow_ioctl_primary_qos_modify_type;

/*---------------------------------------------------------------------------
  Data type used for retrieving the granted QOS flow spec on default ps_flow
  (PS_FLOW_IOCTL_PRIMARY_QOS_GET_GRANTED_FLOW_SPEC)
---------------------------------------------------------------------------*/
typedef struct
{
  ip_flow_type      rx_ip_flow;    /* OUT PARAM, Rx Flow spec              */
  ip_flow_type      tx_ip_flow;    /* OUT PARAM, Tx Flow spec              */
} ps_flow_ioctl_primary_qos_get_granted_flow_spec_type;

/**
  @brief Data type used for retrieving the QOS filter spec

  @see PS_FLOW_IOCTL_QOS_GET_GRANTED_FILTER_SPEC
*/
typedef struct
{
  ip_filter_spec_type    rx_filter_spec;         /**< [out] Rx Filter spec */
  ip_filter_spec_type    tx_filter_spec;         /**< [out] Tx Filter spec */
} ps_flow_ioctl_qos_get_granted_filter_spec_type;

/*---------------------------------------------------------------------------
  Data type used for getting current and max iface Tx queue size
  Fields are
    current_new_data_cnt : Pending new data in the default watermark
    wm_free_cnt          : Empty space on the queue (which is the hi watermark
                           the pending new data in the queue)
    total_pending_cnt    : Total amount of data pending New data +
                           retransmission queue
---------------------------------------------------------------------------*/
typedef struct ps_flow_ioctl_tx_queue_level_s
{
  uint32  current_new_data_cnt;
  uint32  wm_free_cnt;
  uint32  total_pending_cnt;
} ps_flow_ioctl_tx_queue_level_type;

/*---------------------------------------------------------------------------
  Data type used for getting HDR RMAC3 Information
  Fields are
    ps_headroom_payload_size  : PA headroom limited payload size in bytes
    bucket_level_payload_size : BucketLevel equivalent payload size in bytes
    t2p_inflow_payload_size   : T2PInfolw equivalent payload size in bytes
---------------------------------------------------------------------------*/
typedef struct
{
  uint16  ps_headroom_payload_size;
  uint16  bucket_level_payload_size;
  uint16  t2p_inflow_payload_size;
} ps_flow_ioctl_707_hdr_rmac3_info_type;

/*---------------------------------------------------------------------------
  Data type used for getting Transmission Status Information
---------------------------------------------------------------------------*/
typedef boolean ps_flow_ioctl_707_tx_status_type;

/*---------------------------------------------------------------------------
  Data type used for 707 inactivity timer value
---------------------------------------------------------------------------*/
typedef uint32  ps_flow_ioctl_707_inactivity_timer_type;


/*===========================================================================
TYPEDEF PS_FLOW_IOCTL_F_PTR_TYPE

DESCRIPTION
  This function is a call back to the mode handler when mode handler specific
  FLOW IOCTLs are issued by an application.

PARAMETERS
  flow_ptr   : flow to be operated upon
  ioctl_name : the operation name
  argval_ptr : pointer to operation specific structure
  ps_errno   : error code returned in case of failure (Error values are those
               defined in dserrno.h)

                 DS_EINVAL - Returned when the specified IOCTL does not
                             belong to the common set of IOCTLs and there is
                             no IOCTL mode handler registered for the
                             specified flow.

                 DS_EOPNOTSUPP - Returned by the lower level IOCTL mode
                                 handler when specified IOCTL is not
                                 supported by the flow. For instance,
                                 this would be returned by interfaces that
                                 do not support a certain
                                 "flow specific common IOCTL"
                                 (i.e. these are common IOCTLs, but the
                                 implementation is mode specific)

                 DS_EFAULT - This error code is returned if the specified
                             arguments for the IOCTL are correct but an error
                             is encountered while executing the IOCTL.

                 DS_NOMEMORY - This error code is returned if we run out of
                               memory buffers during execution.

RETURN VALUE
   0 : on success
  -1 : on failure

DEPENDENCIES
  None.

SIDE EFFECTS
  None.
===========================================================================*/
typedef int (*ps_flow_ioctl_f_ptr_type)
(
  ps_flow_type         * flow_ptr,
  ps_flow_ioctl_type     ioctl_name,
  void                 * argval_ptr,
  int16                * ps_errno
);


#ifdef __cplusplus
extern "C" {
#endif
/*===========================================================================

                           EXTERNALIZED FUNCTIONS

===========================================================================*/
/*===========================================================================
FUNCTION PS_FLOW_IOCTL()

DESCRIPTION
  This function performs various operations on the given flow.
  Typically, these operations are to get or set a value.

PARAMETERS
  flow_ptr   : ps_flow on which the specified operation is to be performed
  ioctl_name : the operation name
  argval_ptr : pointer to operation specific structure
  ps_errno   : error code returned in case of failure (Error values are
               those defined in dserrno.h)

                 DS_EINVAL - Returned when the specified IOCTL does not
                 belong to the common set of IOCTLs and there is no IOCTL
                 mode handler registered for the specified interface.

                 DS_EOPNOTSUPP - Returned by the lower level IOCTL mode
                 handler when specified IOCTL is not supported by the
                 interface. For instance, this would be returned by
                 interfaces that do not support a certain
                 "ps_flow specific common IOCTL" (i.e. these are common
                 IOCTLs, but the implementation is mode specific)

                 DS_EFAULT - This error code is returned if the specified
                 arguments for the IOCTL are correct but an error is
                 encountered while executing the IOCTL.

                 DS_NOMEMORY - This error code is returned if we run out of
                 mempory buffers during execution.

RETURN VALUE
   0 : on success
  -1 : on failure

DEPENDENCIES
  None.

SIDE EFFECTS
  None.
===========================================================================*/
int ps_flow_ioctl
(
  ps_flow_type         * flow_ptr,
  ps_flow_ioctl_type     ioctl_name,
  void                 * argval_ptr,
  int16                * ps_errno
);

/*===========================================================================
FUNCTION PS_FLOW_DEFAULT_LOGICAL_FLOW_IOCTL_HDLR()

DESCRIPTION
  This function performs the default handling of ioctls for logical flows.

PARAMETERS
  flow_ptr   : ps_flow on which the specified operation is to be performed
  ioctl_name : the operation name
  argval_ptr : pointer to operation specific structure
  ps_errno   : error code returned in case of failure (Error values are
               those defined in dserrno.h)

                 DS_EINVAL - Returned when the specified IOCTL does not
                 belong to the common set of IOCTLs and there is no IOCTL
                 mode handler registered for the specified interface.

                 DS_EOPNOTSUPP - Returned by the lower level IOCTL mode
                 handler when specified IOCTL is not supported by the
                 interface. For instance, this would be returned by
                 interfaces that do not support a certain
                 "ps_flow specific common IOCTL" (i.e. these are common
                 IOCTLs, but the implementation is mode specific)

                 DS_EFAULT - This error code is returned if the specified
                 arguments for the IOCTL are correct but an error is
                 encountered while executing the IOCTL.

                 DS_NOMEMORY - This error code is returned if we run out of
                 mempory buffers during execution.

RETURN VALUE
   0 : on success
  -1 : on failure

DEPENDENCIES
  None

SIDE EFFECTS
  None.
===========================================================================*/
int ps_flow_default_logical_flow_ioctl_hdlr
(
  ps_flow_type         * flow_ptr,
  ps_flow_ioctl_type     ioctl_name,
  void                 * argval_ptr,
  int16                * ps_errno
);


#ifdef __cplusplus
}
#endif

#endif /* PS_FLOW_IOCTL_H */
