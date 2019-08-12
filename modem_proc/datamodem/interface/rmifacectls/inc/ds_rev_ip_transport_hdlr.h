#ifndef _DS_REV_IP_TRANSPORT_HDLR_H
#define _DS_REV_IP_TRANSPORT_HDLR_H
/*===========================================================================

                         DS_REV_IP_TRANSPORT_HDLR.H

DESCRIPTION

  The Data Services Qualcomm MSM Interface reverse IP transport internal header file.

  This file externalizes information required by the QMI control service.

EXTERNALIZED FUNCTIONS

   rev_ip_transport_open_instance
   rev_ip_transport_close_instance
   rev_ip_transport_bringup
   rev_ip_transport_teardown

Copyright (c) 2012 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/
/*===========================================================================

                      EDIT HISTORY FOR FILE

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/rmifacectls/inc/ds_rev_ip_transport_hdlr.h#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
09/27/13    wc     Support dynamic port configuration
10/26/12    sj     Created module
===========================================================================*/

/*===========================================================================

                          INCLUDE FILES FOR MODULE

===========================================================================*/

#include "datamodem_variation.h"
#include "comdef.h"
#include "customer.h"

#include "ds_qmi_svc_ext.h"
#include "ps_iface_defs.h"
#include "dcc_task_defs.h"
#include "ps_meta_info.h"

/*===========================================================================

                                DATA TYPES

===========================================================================*/

typedef enum
{
  REV_IP_TRANSPORT_INSTANCE_MIN =  0,
  REV_IP_TRANSPORT_INSTANCE_1 = REV_IP_TRANSPORT_INSTANCE_MIN,
  REV_IP_TRANSPORT_INSTANCE_2,
  REV_IP_TRANSPORT_INSTANCE_3,
  REV_IP_TRANSPORT_INSTANCE_4,
  REV_IP_TRANSPORT_INSTANCE_5,
  REV_IP_TRANSPORT_INSTANCE_6,
  REV_IP_TRANSPORT_INSTANCE_7,
  REV_IP_TRANSPORT_INSTANCE_8,
  REV_IP_TRANSPORT_INSTANCE_9,   
  REV_IP_TRANSPORT_INSTANCE_MAX
} rev_ip_transport_instance_e_type;


/*---------------------------------------------------------------------------
TYPEDEF DS_REV_IP_TRANSPORT_BRINGUP_COMPLETE_F_PTR_TYPE

DESCRIPTION
  Definition for the function used to convey bringup success or failure

PARAMS
  result:   0 on Failure
            1 on Success
  iface_ptr : iface_ptr on which bringup succeeded/failed

RETURN VALUE
  None
---------------------------------------------------------------------------*/
typedef void (*ds_rev_ip_transport_bringup_complete_f_ptr_type)
(
  ps_iface_type * iface_ptr,
  boolean         result
);

/*---------------------------------------------------------------------------
TYPEDEF DS_REV_IP_TRANSPORT_TEARDOWN_COMPLETE_F_PTR_TYPE

DESCRIPTION
  Definition for the function used to convey teardown success or failure

PARAMS
  result:   0 on Failure
            1 on Success
  iface_ptr : iface_ptr on which teardown succeeded/failed

RETURN VALUE
  None
---------------------------------------------------------------------------*/
typedef void (*ds_rev_ip_transport_teardown_complete_f_ptr_type)
(
  ps_iface_type * iface_ptr,
  boolean         result
);

/*---------------------------------------------------------------------------
TYPEDEF DS_REV_IP_TRANSPORT_TX_CMD_F_PTR_TYPE

DESCRIPTION
  Definition for the function used to transmit IP packets

PARAMS
  iface_ptr     -  The interface on which to transmit the packet
  pkt_chain_ptr -  message (in dsm chain) to be transmitted
  client_info   -  user data passed that contains rev_ip_transport info ptr

RETURN VALUE
  0 on Success
---------------------------------------------------------------------------*/
typedef int (*ds_rev_ip_transport_tx_cmd_f_ptr_type)
(
  ps_iface_type *      iface_ptr,
  dsm_item_type **     pkt_chain_ptr,
  ps_meta_info_type*   meta_info_ptr,
  void*                client_info
);
/*===========================================================================

                        EXTERNAL FUNCTION DEFINTIONS

===========================================================================*/


/*===========================================================================
  FUNCTION REV_IP_TRANSPORT_BRINGUP

  DESCRIPTION
    Establishes reverse data path for ePDG or WLAN iface 

  PARAMETERS
    iface_ptr : v4 or v6 iface for which reverse IP transport is to be established
    companion_iface_ptr : associated v6 or v4 iface

  DEPENDENCIES
    None

  RETURN VALUE
    TRUE on success
    FALSE otherwise

  SIDE EFFECTS
    None
===========================================================================*/
boolean rev_ip_transport_bringup
(
  ps_iface_type*    iface_ptr,
  ps_iface_type*    companion_iface_ptr,
  ds_rev_ip_transport_bringup_complete_f_ptr_type bringup_cb
);

/*===========================================================================
  FUNCTION REV_IP_TRANSPORT_TEARDOWN

  DESCRIPTION
    Tears down reverse data path for ePDG or WLAN iface 

  PARAMETERS
    iface_ptr : iface_ptr corresponding to which Reverse IP transport is to be torn down

  DEPENDENCIES
    None

  RETURN VALUE
    TRUE on success
    FALSE otherwise

  SIDE EFFECTS
    None
===========================================================================*/
boolean rev_ip_transport_teardown
(
  ps_iface_type*    iface_ptr,
  ds_rev_ip_transport_teardown_complete_f_ptr_type teardown_cb
);

/*===========================================================================
  FUNCTION REV_IP_TRANSPORT_PROCESS_CMD()

  DESCRIPTION
    This function processes a Reverse IP transport command in DCC task.

  PARAMETERS
    cmd          :  Reverse IP transport command to be processed
    user_data_ptr:  private data buffer containing the Reverse IP transport command
                    information.

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
void rev_ip_transport_process_cmd
(
  dcc_cmd_enum_type cmd,        /* Actual command to be processed         */
  void *user_data_ptr
);

/*===========================================================================
  FUNCTION REV_IP_TRANSPORT_INIT()

  DESCRIPTION
    This function performs global initialization of Reverse IP transport module.

  PARAMETERS
    None
 
  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
void rev_ip_transport_init
(
  void 
);

/*===========================================================================
  FUNCTION REV_IP_TRANSPORT_RETURN_TX_FN_PTR()

  DESCRIPTION
    This function returns a pointer to the function that will be invoked as the
    PS Iface TX function.

  PARAMETERS
    None
 
  RETURN VALUE
    Function ptr to tx function

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
ds_rev_ip_transport_tx_cmd_f_ptr_type rev_ip_transport_return_tx_fn_ptr
(
  void 
);

/*===========================================================================
FUNCTION REV_IP_TRANSPORT_BIND

DESCRIPTION
  This function binds a logical rev rmnet instance to a physical port

PARAMETERS
  ep_id:   physical port end point id
  mux_id:  logical rmnet mux id

DEPENDENCIES
  None

RETURN VALUE
  TRUE : success
  FALSE: failure

SIDE EFFECTS
  None.
===========================================================================*/
boolean rev_ip_transport_bind
(
  uint32          ep_id,
  uint8           mux_id
);

/*===========================================================================
FUNCTION REV_IP_TRANSPORT_UNBIND

DESCRIPTION
  This function unbinds a logical rev rmnet instance from a physical port

PARAMETERS
  ep_id:   physical port end point id
  mux_id:  logical rmnet mux id

DEPENDENCIES
  None

RETURN VALUE
  TRUE : success
  FALSE: failure

SIDE EFFECTS
  None.
===========================================================================*/
boolean rev_ip_transport_unbind
(
  uint32          ep_id,
  uint8           mux_id
);
/*===========================================================================
  FUNCTION REV_IP_TRANSPORT_GET_IFACE()

  DESCRIPTION
    Returns the ps_iface controlled by the given rev IP transport instance

  PARAMETERS
    ep_id:   physical port end point id
    mux_id:  logical rmnet mux id
    is_v6 :  whether v4 or v6 iface

  RETURN VALUE
    ps_iface *  - Pointer to the ps_iface controlled by the spec'd ep id
                  mux id and family
    NULL        - Reverse IP transport not up or specific family call not up

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
ps_iface_type * rev_ip_transport_get_iface
(
  uint32          ep_id,
  uint8           mux_id,
  boolean         is_v6
);

#endif /* _DS_REV_IP_TRANSPORT_HDLR_H */

