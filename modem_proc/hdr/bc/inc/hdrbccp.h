#ifndef HDRBCCP_H
#define HDRBCCP_H
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                H D R   B R O A D C A S T  C O N T R O L 
       
                             P R O T O C O L
       
                  E X T E R N A L   H E A D E R   F I L E       

GENERAL DESCRIPTION
  This file contains the external interface of the HDR Broadcast Control 
  Protocol.
  
EXTERNALIZED FUNCTIONS


INITIALIZATION AND SEQUENCING REQUIREMENTS
  The protocol must be initialized after system startup prior to activating 
  the protocol.  
  
  Copyright (c) 2004,2005,2006 by Qualcomm Technologies, Inc.  All Rights Reserved.
  Copyright (c) 2007,2008 by Qualcomm Technologies, Inc.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* EJECT */
/*===========================================================================

                           EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/hdr/bc/inc/hdrbccp.h#1 $ $DateTime: 2016/12/13 07:58:05 $ $Author: mplcsds1 $
   
when       who     what, where, why
--------   ---     ----------------------------------------------------------
09/05/08   mpa     Added support for BCMCS 2.0 phase 2
05/28/08   etv     Fixed featurization issue with hdrbccp_configure API.
05/14/08   etv     Added support for AT-init neg for supportEBCMCS attribute.
05/05/08   mpa     Added support for BCMCS 2.0 phase 1
02/13/06   mpa     Return status for test flow commands.
08/28/05   mpa     HDR Broadcast Control Protocol rearchitecture
08/05/04   kss     Add a data callback function to API; remove special case
                   for test mode.
08/05/04   mpa     Added hdrbccp_are_any_flows_monitored().
07/22/04   mpa     (ss) Added multifrequency support.
07/02/04   mpa     Added Session Negotiation + protocol events path.  
                   Code Cleanup.
                   Flow table handling moved here (from hdrbcovhd)
06/14/04   ss      (mpa for) Removed redundant flow handling.
06/07/04   kss     Moved enable/disable commands for data path to hdrbctask.c
04/26/04   kss     Ported from QC Corp.

===========================================================================*/

/* EJECT */
/*===========================================================================

                         INCLUDE FILES FOR MODULE
                     
===========================================================================*/
#include "hdr_variation.h"
#include "dsm.h"
#include "hdrhai.h"
#include "hdrind.h"
#include "hdrerrno.h"

/*===========================================================================

                      TYPE DECLARATIONS

===========================================================================*/


/*===========================================================================

FUNCTION HDRBCCP_DATA_FLOW_ENABLE

DESCRIPTION
  Installs the data callback and enables a broadcast flow.
  
DEPENDENCIES
  None.

PARAMETERS
  flow_id - The flow_id to enable.  
  
RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern void hdrbccp_data_flow_enable
(
  uint32 flow_id
);

/*===========================================================================

FUNCTION HDRBCCP_DATA_FLOW_DISABLE

DESCRIPTION
  Disables a broadcast flow.
  
DEPENDENCIES
  None.

PARAMETERS
  flow_id - The flow_id to disable.  
  
RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern void hdrbccp_data_flow_disable
(
  uint32 flow_id
);


/*=============================================================================

FUNCTION HDRBCCP_FLOW_ENABLE

DESCRIPTION
  Disables a broadcast flow.
  
DEPENDENCIES
  None.

PARAMETERS
  flow_id   - Flow ID of the flow to enable   
  data_cb   - The callback function that is used to deliver broadcast data.
  
RETURN VALUE
  None.

SIDE EFFECTS
  None.
=============================================================================*/
extern void hdrbccp_flow_enable
(
  uint32 flow_id, 
  void (*data_cb)(dsm_item_type * data_pkt)
);


/*=============================================================================

FUNCTION HDRBCCP_FLOW_DISABLE

DESCRIPTION
  Disables a broadcast flow.
  
DEPENDENCIES
  None.

PARAMETERS
  flow_id   - Flow ID of the flow to disable   
  
RETURN VALUE
  None.

SIDE EFFECTS
  None.

=============================================================================*/
extern void hdrbccp_flow_disable
(
  uint32 flow_id 
);

/*=============================================================================

FUNCTION HDRBCCP_TEST_FLOW_ENABLE

DESCRIPTION
  Sends a Test Flow Enable Cmd to HDRMC.
  
DEPENDENCIES
  Disallow a test flow enable cmd in CLOSE state.

PARAMETERS
  flow_id   - Flow ID of the test flow to enable 
              (No IP:Port# address provided for test flows)   
  data_cb   - The callback function that is used to deliver broadcast data.
  sender    - Protocol sending the command
  
RETURN VALUE
  E_SUCCESS - disable test flow command successful
  E_FAILURE - failure processing test flow command

SIDE EFFECTS
  - Test flows may only be enabled when no DS flows are currently desired.
  - All information left
   
=============================================================================*/
extern hdrerrno_enum_type hdrbccp_test_flow_enable
(
  sys_bcmcs_flow_id_type flow_id, 
  void (*data_cb)(dsm_item_type * data_pkt),
  hdrhai_protocol_name_enum_type sender
);

/*=============================================================================

FUNCTION HDRBCCP_TEST_FLOW_DISABLE

DESCRIPTION
  Disables a broadcast test flow.
  
DEPENDENCIES
  None.

PARAMETERS
  flow_id   - Flow ID of the test flow to disable  
  sender    - Protocol sending the command 
  
RETURN VALUE
  E_SUCCESS - disable test flow command successful
  E_FAILURE - failure processing test flow command

SIDE EFFECTS
  None.

=============================================================================*/
extern hdrerrno_enum_type hdrbccp_test_flow_disable
(
  sys_bcmcs_flow_id_type flow_id,
  hdrhai_protocol_name_enum_type sender
);

/*=============================================================================
FUNCTION HDRBCCP_POWERUP_INIT

DESCRIPTION
  This function initializes the Broadcast protocol Suite on power up.
  It mainly defines some timers.
  
DEPENDENCIES
  None.

PARAMETERS
  None

RETURN VALUE
  None.

SIDE EFFECTS
  None.
=============================================================================*/
extern void hdrbccp_powerup_init( void );

/*===========================================================================

FUNCTION HDRBCCP_IND_CB

DESCRIPTION
  This function is registered to be called whenever a protocol gives an
  indication of interest to BROADCAST OVHD.

DEPENDENCIES
  None.
  
PARAMETERS
  ind_name - Indication to process, is a global enum for all possible 
             indications in HDR signaling
  ind_data - Optional data pointer for indication specific data
 
RETURN VALUE
  None.
  
SIDE EFFECTS
  None.
===========================================================================*/
extern void hdrbccp_ind_cb
(
  hdrind_ind_name_enum_type ind_name, 
  void *ind_data_ptr 
);

/*===========================================================================
FUNCTION HDRBCCP_MSG_CB

DESCRIPTION
  This function is registered to be called whenever a protocol gives a
  message to the Broadcast Protocol

DEPENDENCIES
  None.
  
PARAMETERS
  msg_ptr - Pointer to the DSM item chain holding the message

RETURN VALUE
  None.
  
SIDE EFFECTS
  None.
===========================================================================*/
extern void hdrbccp_msg_cb
(
  dsm_item_type *msg_ptr
);

/*===========================================================================
FUNCTION HDRBCCP_CONFIG_MSG_CB

DESCRIPTION
  This function is registered to be called whenever a protocol gives a
  config message to the Broadcast Protocol

DEPENDENCIES
  None.
  
PARAMETERS
  msg_ptr - Pointer to the DSM item chain holding the message

RETURN VALUE
  None.
  
SIDE EFFECTS
  None.
===========================================================================*/
extern void hdrbccp_config_msg_cb
(
  dsm_item_type *msg_ptr
);

/*=============================================================================
FUNCTION HDRBCCP_TIMER_CB

DESCRIPTION
  This function is the callback for all broadcast protocol timers.
  This function is called by REX when the timer expires, and runs
  in the context of the task that defined the timer (HDRMC)    
  
DEPENDENCIES
  None.

PARAMETERS
  timer_id - Timer ID of the expired timer

RETURN VALUE
  None.

SIDE EFFECTS
  None.
=============================================================================*/
extern void hdrbccp_timer_cb
(
  uint32 timer_id
);

/*===========================================================================

FUNCTION HDRBCCP_UPDATE_FLOW_REQUEST

DESCRIPTION
  This function commands BCCP to update its requested flow list.

DEPENDENCIES
  None

PARAMETERS
  sender - The protocol giving this command (only for software trace)
  transaction_id - Transaction ID
  num_flows      - Number of bcmcs flows included in req_flow_list
  req_flow_list  - The list of requested flows (IP:port# address) 
                   and their statuses
  reg_setup_list - Registration setup flag for each of the requested flows

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void hdrbccp_update_flow_request
(
  hdrhai_protocol_name_enum_type sender,
  uint8 transaction_id,
  uint8 num_flows,
  sys_bcmcs_flow_addr_s_type *req_flow_list
#ifdef FEATURE_HDR_BCMCS_2p0
  #error code not present
#endif /* FEATURE_HDR_BCMCS_2p0 */
);

/*=============================================================================

FUNCTION HDRBCCP_DISCONTINUE_BROADCAST

DESCRIPTION
   This function 
  
DEPENDENCIES
  None.

PARAMETERS
  sender         - Protocol processing the command
  transaction_id - Transaction ID
  
RETURN VALUE
  None.

SIDE EFFECTS
  None.
=============================================================================*/
extern void hdrbccp_discontinue_broadcast
(
  hdrhai_protocol_name_enum_type sender,
  uint8 transaction_id
);

/*=============================================================================

FUNCTION HDRBCCP_ACTIVATE

DESCRIPTION
   This function commands BCCP to activate 
     
DEPENDENCIES
  None.

PARAMETERS
  sender - Protocol sending the command
  
RETURN VALUE
  None.
  
SIDE EFFECTS
  None.
=============================================================================*/
extern void hdrbccp_activate
(
  hdrhai_protocol_name_enum_type sender
);

/*=============================================================================

FUNCTION HDRBCCP_DEACTIVATE

DESCRIPTION
   This function commands BCCP to deactivate 
     
DEPENDENCIES
  None.

PARAMETERS
  sender - Protocol sending the command
  
RETURN VALUE
  None.
  
SIDE EFFECTS
  None.
=============================================================================*/

extern void hdrbccp_deactivate
(
  hdrhai_protocol_name_enum_type sender
);

#ifdef FEATURE_HDR_BCMCS_2p0
#error code not present
#endif /* FEATURE_HDR_BCMCS_2p0 */

/*===========================================================================
FUNCTION HDRBCCP_CONFIGURE                                   EXTERNAL

DESCRIPTION
  This function does the AT-init configuration of EBCMCSSupported attribute

DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  TRUE - If the configuration was needed.
  FALSE - Otherwise.

SIDE EFFECTS
  None.
===========================================================================*/
boolean hdrbccp_configure
(
  hdrhai_protocol_name_enum_type sender
);

#ifdef FEATURE_HDR_BCMCS_2p0
#error code not present
#endif /* FEATURE_HDR_BCMCS_2p0 */

#endif /* HDRBCCP_H */


