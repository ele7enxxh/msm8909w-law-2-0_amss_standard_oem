#ifndef IPSEC_IFACE_FLOW_H
#define IPSEC_IFACE_FLOW_H

/*===========================================================================
                   I P S E C _ I F A C E _ F L O W . H

DESCRIPTION
  IPsec Flow handling functions.

Copyright (c) 2009-2010 Qualcomm Technologies Incorporated. 
All Rights Reserved.
Qualcomm Confidential and Proprietary
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/ifacectls/src/ipsec_iface_flow.h#1 $
  $Author: mplcsds1 $
  $DateTime: 2016/12/13 08:00:03 $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
02/26/09    sp     Created module - Moved IPsec Mode Handler to Data
===========================================================================*/

/*===========================================================================

                            INCLUDE FILES FOR MODULE

===========================================================================*/

#include "datamodem_variation.h"
#include "comdef.h"
#include "customer.h"

#ifdef FEATURE_SEC_IPSEC

#include "secips.h"
#include "ps_iface_ioctl.h"
#include "ps_system_heap.h"

#ifdef FEATURE_SEC_IPSEC_CHILD_SA

#include "ps_iface_defs.h"
#include "ps_iface_flow.h"
#include "ps_flow.h"
#include "ps_flowi.h"

/*===========================================================================

                        FUNCTION PROTOTYPES

===========================================================================*/

/*===========================================================================
FUNCTION  IPSEC_IFACE_FLOW_INIT()

DESCRIPTION
  Initializes the IPSEC flow module.

DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void ipsec_iface_flow_init
(
  void
);


/*===========================================================================
FUNCTION  IPSEC_IFACE_FLOW_CREATE_FLOW()

DESCRIPTION
  Handles QOS_REQUEST and QOS_REQUEST_EX ioctls.

PARAMETERS
  iface_ptr         : corresponding iface on which QOS is requested.
                      This MUST be a logical iface, else an error is
                      returned
  ioctl_name        : ioctl to invoke on the underlying iface
                      Only PS_IFACE_IOCTL_QOS_REQUEST and
                      PS_IFACE_IOCTL_QOS_REQUEST_EX are allowed.
  qos_req_param     : pointer to structure containing the qos_spec for
                      the logical flows to be created. This MUST not
                      be NULL.
  ioctl_argval_ptr  : ioctl paramters for the ioctl call on
                      underlying iface. This parameter MUST not
                      be NULL.
  out_flow_ptr_ptr  : flow(s) that are being created. (OUT param)
  ps_errno          : specific error code in case operation fails

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
int ipsec_iface_flow_create_flow
(
  ps_iface_type                        * iface_ptr,
  ps_iface_ioctl_type                    ioctl_name,
  const ps_flow_create_param_type      * qos_req_param,
  void                                 * ioctl_argval_ptr,
  ps_flow_type                        ** out_flow_ptr_ptr,
  int16                                * ps_errno
);


/*===========================================================================
FUNCTION  IPSEC_IFACE_FLOW_ACTIVATE_CMD_CBACK

DESCRIPTION
  Callback function registered with PS FLOW module for FLOW_ACTIVATE_CMD.
  Posts a command to IPSEC task to establish the flow.

DEPENDENCIES
  ipsec_iface must be UP.

PARAMETERS
  flow_ptr - IPSEC flow pointer
  client_data_ptr - User data

RETURN VALUE
  0  : on success
  -1 : on failure

SIDE EFFECTS
  None
===========================================================================*/
int ipsec_iface_flow_activate_cmd_cback
(
  ps_flow_type  * flow_ptr,
  void          * client_data_ptr
);


/*===========================================================================
FUNCTION  IPSEC_IFACE_FLOW_EVENT_CBACK

DESCRIPTION
  Processes WLAN flow specific events such as:
  FLOW_ACTIVE_EV  - Post ACTIVE indication on IPSEC flow
  FLOW_SUSPEND_EV - Post SUSPEND indication on IPSEC flow
  FLOW_NULL_EV    - Post a command to IPSEC task to delete child SAs
                    and post GO_NULL indication on IPSEC flow

DEPENDENCIES
  None

PARAMETERS
  assoc_flow_ptr - Underlying flow
  event - Type of event on underlying flow
  event_info - Event info
  user_data_ptr - Client data, refers to IPSEC flow

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void ipsec_iface_flow_event_cback
(
  ps_flow_type                * assoc_flow_ptr,
  ps_iface_event_enum_type      event,
  ps_iface_event_info_u_type    event_info,
  void                        * user_data_ptr
);


/*===========================================================================
FUNCTION  IPSEC_IFACE_FLOW_PROCESS_CHILD_SA_EV

DESCRIPTION
  This function does the following:
  We have already established Child SAs for the flow.
  Request QOS_IOCTL on the underlying flow.
  Associate with underlying flow and register for events.
  Post enable indication on IPSEC flow.

DEPENDENCIES
  ipsec_iface must be UP.

PARAMETERS
  client_data_ptr - Pointer to IPSEC Flow control block.
  event - SECIPSIFACE_CHILD_SA_UP_EV or SECIPSIFACE_CHILD_SA_DOWN_EV

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void ipsec_iface_flow_process_child_sa_ev
(
  void                         * client_data_ptr,
  secipsiface_event_enum_type    event
);

#endif /* FEATURE_SEC_IPSEC_CHILD_SA */


/*===========================================================================
FUNCTION  IPSEC_IFACE_FLOW_TRANSLATE_QOS_PARAMS

DESCRIPTION
  Translates the QOS parameters.

DEPENDENCIES
  ipsec_iface must be UP.

PARAMETERS
  qos_req_ptr         - Original QOS request
  xlated_qos_req_ptr  - Out value, translated QOS params
  ipsec_iface_ptr     - Points to the IPSEC iface
  ipsec_flow_cb_ptr   - Ponter to IPSEC Flow Control Block

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void ipsec_iface_flow_translate_qos_params
(
  ps_iface_ioctl_qos_request_type   * qos_req_ptr,
  ps_iface_ioctl_qos_request_type   * xlated_qos_req_ptr,
  ps_iface_type                     * ipsec_iface_ptr,
  secipsflow_ipsec_cb_type          * ipsec_flow_cb_ptr
);

#endif /* FEATURE_SEC_IPSEC */

#endif /* IPSEC_IFACE_FLOW_H */
