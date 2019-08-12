#ifndef _HDRRSP_H
#define _HDRRSP_H

/*===========================================================================

                        H I G H     D A T A     R A T E

                  R O U T E   S E L E C T I O N   P R O T O C O L

GENERAL DESCRIPTION
  This file defines the interface of the EV-DO Rev A Route Selection Protocol


 INITIALIZATION AND SEQUENCING REQUIREMENTS
   

 Copyright (c) 2005-2011 by Qualcomm Technologies Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================
                        EDIT HISTORY FOR MODULE

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp2/hdrdata/inc/hdrrsp.h#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $
  
when       who     what, where, why
--------   ---     -------------------------------------------------------
11/18/11   vpk     HDR SU API cleanup
05/30/06   ksu     EMPA: RLP init code review comments
05/04/06   ksu     EMPA: RSP part 2
03/27/06   ksu     EMPA: add route to fb_cookie
03/24/06   ksu     split out dsrsp from hdrrsp
12/29/05   ksu     EMPA: framework for route protocol (RP)
12/29/05   ksu     EMPA: trivial route selection protocol (RSP)
12/23/05   ksu     EMPA RSP: two rev watermarks per ip flow

===========================================================================*/


/*===========================================================================
                     INCLUDE FILES FOR MODULE
===========================================================================*/
#include "datamodem_variation.h"
#include "customer.h"

#ifdef FEATURE_HDR_EMPA
#include "dsrlp_v.h"
#include "hdrcom_api.h"
#include "hdrcp_api.h"
#include "ds1x_meta_info.h"
/*===========================================================================
                   
                   DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains definitions and declarations for constants, macros,
types, variables and other items needed by this module.

===========================================================================*/

/*---------------------------------------------------------------------------
  MESSAGE ID VALUES
---------------------------------------------------------------------------*/
#define HDRRSP_ROUTE_SELECT_MSG_ID       0x1e
#define HDRRSP_ACTIVATE_ROUTE_MSG_ID     0x20


/*===========================================================================

FUNCTION HDRRSP_STARTUP

DESCRIPTION
  Called once at powerup for initialization
  
DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
  
===========================================================================*/
extern void hdrrsp_startup( void );

/*===========================================================================

FUNCTION HDRRSP_INIT

DESCRIPTION
  Called at connection open
  
DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  Since this can be called from various tasks, a command is forwarded
  to the PS task for actual initilization.
  
===========================================================================*/
extern void hdrrsp_init( void );

/*===========================================================================

FUNCTION HDRRSP_INIT_FLOW

DESCRIPTION
  Called at flow activation
  
PARAMETERS
  rlp_id_ptr     - identifies the flow and direction

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  Since this can be called from various tasks, a command is forwarded
  to the PS task for actual initilization.
  
===========================================================================*/
extern void hdrrsp_init_flow
( 
  const dsrlp_rlp_identity_type* rlp_id_ptr
);

/*===========================================================================

FUNCTION HDRRSP_UPDATE_FLOW

DESCRIPTION
  Called when a GAUP message is received for this flow.  Update
  configuration attributes that may have changed.
  
PARAMETERS
  rlp_id_ptr     - identifies the flow and direction

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  Since this can be called from various tasks, a command is forwarded
  to the PS task for actual initilization.
  
===========================================================================*/
extern void hdrrsp_update_flow
( 
  const dsrlp_rlp_identity_type* rlp_id_ptr
);

/*===========================================================================

FUNCTION HDRRSP_MSG_CB

DESCRIPTION
  This callback function queues incoming messages for this protocol.
  
PARAMETERS
  msg_ptr_ptr - handle to received message buffer

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
  
===========================================================================*/
extern void hdrrsp_msg_cb 
( 
  dsm_item_type** msg_ptr_ptr
);

/*===========================================================================

FUNCTION HDRRSP_SET_ROUTE_PROTOCOL

DESCRIPTION
  Sets the route protocol and optional handle list for a flow

DEPENDENCIES
  The handle list (if non-NULL) must have at least DSRSP_NUM_ROUTES
  entries, each of which must have been properly initialized.

PARAMETERS
  rlp_id_ptr     - identifies the flow and direction
  route_protocol - one of HDRMRLPC_ROUTE_PROT_ID_*
  rp_handle_list - optional list of RP handles (may be NULL)

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern void hdrrsp_set_route_protocol
(
  dsrlp_rlp_identity_type* rlp_id_ptr,
  hdrcp_mrlpc_route_prot_id_type route_protocol,
  void *rp_handle_list[]
);

/*===========================================================================

FUNCTION HDRRSP_GET_OPEN_ROUTE

DESCRIPTION
  Returns the active route

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
byte hdrrsp_get_open_route(void);

/*===========================================================================

FUNCTION HDRRSP_Q_GET

DESCRIPTION
  This function removes an item from the head of a specified queue
  list, and passes it through any configured route protocol.

DEPENDENCIES
  The queue list must have at least DSRSP_NUM_ROUTES entries, each of
  which must have been initialized previously via a call to q_init.

PARAMETERS
  rlp_id_ptr - identifies the flow and direction
  wm_list     - the list of watermarks to get from

RETURN VALUE
  A pointer to the dequeued item. If the specified queue list is empty, then
  NULL is returned.

SIDE EFFECTS
  The head item, if any, is removed from the specified queue list.

===========================================================================*/
extern dsm_item_type* hdrrsp_q_get
(
  const dsrlp_rlp_identity_type* rlp_id_ptr,
  dsm_watermark_type wm_list[]
);

/*===========================================================================
FUNCTION HDRRSP_DSM_ENQUEUE

DESCRIPTION
  This function will put the passed DSM item to the passed shared queue list 
  then check for and perform any 'put' events.

DEPENDENCIES
  The watermark list must have at least DSRSP_NUM_ROUTES entries, each
  of which must have been properly initialized.

PARAMETERS
  rlp_id_ptr   - identifies the flow and direction
  wm_list      - pointer to watermark list to put to
  pkt_head_ptr - pointer to pointer to item to add to queue

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern void hdrrsp_dsm_enqueue
(
  const dsrlp_rlp_identity_type* rlp_id_ptr,
  dsm_watermark_type wm_list[],
  dsm_item_type **pkt_head_ptr,
  ds1x_meta_info_type *ds1x_meta_info_ptr
);

/*===========================================================================

FUNCTION HDRRSP_WM_FREE_CNT

DESCRIPTION
  Returns the total number of bytes free in the current route

DEPENDENCIES
  The watermark list must have at least DSRSP_NUM_ROUTES entries, each
  of which must have been properly initialized.

PARAMETERS
  rlp_id_ptr - identifies the flow and direction
  wm_list    - pointer to watermark list to count free bytes

RETURN VALUE
  uint32 - the current free count in the current route

SIDE EFFECTS
  None
===========================================================================*/
extern uint32 hdrrsp_wm_free_cnt
(
  dsrlp_rlp_identity_type* rlp_id_ptr,
  dsm_watermark_type wm_list[]
);

/*===========================================================================

FUNCTION HDRRSP_WM_CURRENT_CNT

DESCRIPTION
  Returns the total number of bytes ready for tx in all routes.

DEPENDENCIES
  The watermark list must have at least DSRSP_NUM_ROUTES entries, each
  of which must have been properly initialized.

PARAMETERS
  rlp_id_ptr - identifies the flow and direction
  wm_list    - pointer to watermark list to count

RETURN VALUE
  uint32 - the current count in all routes

SIDE EFFECTS
  None
===========================================================================*/
extern uint32 hdrrsp_wm_current_cnt
(
  dsrlp_rlp_identity_type* rlp_id_ptr,
  dsm_watermark_type wm_list[]
);

/*===========================================================================

FUNCTION HDRRSP_CLEAR_ROUTE_PROTOCOL

DESCRIPTION
  Unsets the route protocol and optional handle list for a flow

DEPENDENCIES
  None

PARAMETERS
  rlp_id_ptr - identifies the flow and direction

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern void hdrrsp_clear_route_protocol
(
  const dsrlp_rlp_identity_type* rlp_id_ptr
);

/*===========================================================================

FUNCTION HDRRSP_CLEANUP

DESCRIPTION
  Called at connection close
  
DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  Since this can be called from various tasks, a command is forwarded
  to the PS task for actual cleanup.
  
===========================================================================*/
extern void hdrrsp_cleanup( void );

#endif /* FEATURE_HDR_EMPA */
#endif /* _HDRRSP_H */
