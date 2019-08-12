#ifndef PS_IFACE_LOGICAL_FLOW_H
#define PS_IFACE_LOGICAL_FLOW_H
/*===========================================================================
  @file ps_iface_logical_flow.h

  This module defines the API for logical flow framework
  TODO Enhance documentation

  Copyright (c) 2010 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary
===========================================================================*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/netiface/inc/ps_iface_logical_flow.h#1 $
  $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

===========================================================================*/

/*===========================================================================

                                INCLUDE FILES

===========================================================================*/
#include "ps_iface_defs.h"
#include "ps_iface_ioctl.h"
#include "ps_flow_ioctl.h"
#include "ps_iface.h"
#include "ps_flow.h"
#include "ps_iface_flow.h"


/*===========================================================================

                        PUBLIC FUNCTION DECLARATIONS

===========================================================================*/
/*===========================================================================
FUNCTION PS_IFACE_CREATE_LOGICAL_FLOWS

DESCRIPTION
  This function is intended to be used by logical iface handlers for
  "default" handling of the QoS Request and QoS Request Ex
  ioctls. Iface handlers requiring special handling need to implement
  similar functionality on their own by making use of related functions in
  this module.

  The steps performed by this function are outlined below.
  (1) Creates the appropriate number of logical flows (L_flow(s)) on the
      requested iface (L_iface) by invoking ps_iface_create_flow()
  (2) Invokes ps_flow_activate_cmd() to activate all L_flow(s)
  (3) Invokes the requested ioctl (PS_IFACE_IOCTL_QOS_REQUEST or
      PS_IFACE_IOCTL_QOS_REQUEST_EX) on A_iface - the iface
      associated with L_iface, creating requested number of flows
      (A_flow(s)) on A_iface. This ioctl call uses the QoS parameters
      contained in ioctl_argval_ptr. Note that these QoS parameters may
      be either parameters that the application requested (i.e. same
      as qos_req_param) or the translated ones.
  (4) Associates each L_flow with the corresponding A_flow
  (5) Registers for events on A_flow(s)

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
int ps_iface_create_logical_flows
(
  ps_iface_type                    * iface_ptr,
  ps_iface_ioctl_type                ioctl_name,
  const ps_flow_create_param_type  * qos_req_param,
  void                             * ioctl_argval_ptr,
  ps_flow_type                    ** out_flow_ptr_ptr,
  int16                            * ps_errno
);

/*===========================================================================
FUNCTION PS_IFACE_MODIFY_LOGICAL_FLOWS

DESCRIPTION
  This function is intended to be used by logical iface handlers for
  "default" handling of the logical QoS modify ioctls. Iface handlers
  require special handling need to implement similar functionality on
  their own by making use of related functions in this module.

  The steps performed by this function are outlined below.
  (1) Validate the passed in parameters.
  (2) Invokes the ps_iface_modify_flow function on the logical flow.
  (3) Calls the PS_FLOW_IOCTL_QOS_MODIFY IOCTL on the underlying A_flow

PARAMETERS
  iface_ptr         : corresponding iface on which QOS is requested.
                      This MUST be a logical iface, else an error is
                      returned
  modify_flow_ptr   : the flow pointer which is to be modified
  ioctl_name        : ioctl to invoke on the underlying iface
                      Only PS_FLOW_IOCTL_QOS_MODIFY is allowed.
  qos_modify_param  : pointer to structure containing the qos_spec to be
                      used by the logical flow for modification
                      . This MUST not be NULL.
  ioctl_argval_ptr  : ioctl paramters for the ioctl call on
                      underlying iface. This parameter MUST not
                      be NULL.
  ps_errno          : specific error code in case operation fails

RETURN VALUE
   0 : on success
  -1 : on failure

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
int ps_iface_modify_logical_flows
(
  ps_iface_type                    * iface_ptr,
  ps_flow_type                     * modify_flow_ptr,
  ps_flow_ioctl_type                 ioctl_name,
  const ps_flow_modify_param_type  * qos_modify_param,
  void                             * ioctl_argval_ptr,
  int16                            * ps_errno
);

#endif /* PS_IFACE_LOGICAL_FLOW_H */

