#ifndef _DSRSP_H
#define _DSRSP_H

/*===========================================================================

                        D A T A     S E R V I C E S

                  R O U T E   S E L E C T I O N   P R O T O C O L

GENERAL DESCRIPTION
  This file defines the interface of the DS Route Selection Protocol

  When FEATURE_HDR_EMPA is not defined, or EMPA is not negotiated,
  this file is a trivial pass through; else the heavy lifting is
  delegated to HDRRSP.

 INITIALIZATION AND SEQUENCING REQUIREMENTS
   
 Copyright (c) 2006 by Qualcomm Technologies Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================
                        EDIT HISTORY FOR MODULE

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp2/ds707/inc/dsrsp.h#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $
  
when       who     what, where, why
--------   ---     -------------------------------------------------------
05/30/06   ksu     EMPA: RLP init code review comments
05/04/06   ksu     EMPA: RSP part 2
03/27/06   ksu     EMPA: add route to fb_cookie
03/24/06   ksu     split out dsrsp from hdrrsp

===========================================================================*/



/*===========================================================================
                     INCLUDE FILES FOR MODULE
===========================================================================*/

#include "datamodem_variation.h"
#include "customer.h"
#include "dsrlp_v.h"
#include "ds1x_meta_info.h"

/* RSP is always in the data stack, but is a simple pass through unless */
/* EMPA is negotiated (EV-DO Rev A only). */
enum 
{
  DSRSP_ROUTE_A = 0,
#ifdef FEATURE_HDR_EMPA
  DSRSP_ROUTE_B = 1,
#endif /* FEATURE_HDR_EMPA */
  DSRSP_NUM_ROUTES
};


/*===========================================================================

FUNCTION DSRSP_Q_GET

DESCRIPTION
  This function removes an item from the head of a specified queue
  list, and passes it through any configured route protocol.

DEPENDENCIES
  The watermark list must have at least DSRSP_NUM_ROUTES entries, each of
  which must have been initialized previously via a call to q_init.

PARAMETERS
  rlp_id_ptr - identifies the flow and direction
  wm_list     - the list of queues to get from

RETURN VALUE
  A pointer to the dequeued item. If the specified watermark list is empty, then
  NULL is returned.

SIDE EFFECTS
  The head item, if any, is removed from the queue of the specified 
  watermark list.

===========================================================================*/
extern dsm_item_type* dsrsp_q_get
(
  const dsrlp_rlp_identity_type* rlp_id_ptr,
  dsm_watermark_type wm_list[] 
);

/*===========================================================================
FUNCTION DSRSP_DSM_ENQUEUE

DESCRIPTION
  This function will put the passed DSM item to the appropriate queue of the
  shared watermark list then check for and perform any 'put' events.

DEPENDENCIES
  The watermark list must have at least DSRSP_NUM_ROUTES entries, each
  of which must have been properly initialized.

PARAMETERS
  rlp_id_ptr   - identifies the flow and direction
  wm_list      - pointer to watermark list to put to
  pkt_head_ptr - pointer to pointer to item to add to watermark

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern void dsrsp_dsm_enqueue
(
  const dsrlp_rlp_identity_type* rlp_id_ptr,
  dsm_watermark_type wm_list[],
  dsm_item_type **pkt_head_ptr,
  ds1x_meta_info_type *ds1x_meta_info_ptr
);

/*===========================================================================

FUNCTION DSRSP_WM_FREE_CNT

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
extern uint32 dsrsp_wm_free_cnt
(
  dsrlp_rlp_identity_type* rlp_id_ptr,
  dsm_watermark_type wm_list[]
);

/*===========================================================================

FUNCTION DSRSP_WM_CURRENT_CNT

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
extern uint32 dsrsp_wm_current_cnt
(
  dsrlp_rlp_identity_type* rlp_id_ptr,
  dsm_watermark_type wm_list[]
);

/*===========================================================================

FUNCTION DSRSP_GET_OPEN_ROUTE

DESCRIPTION
  Returns the active route.

DEPENDENCIES
  None

RETURN VALUE
  Route

SIDE EFFECTS
  None
===========================================================================*/
byte dsrsp_get_open_route(void);

#endif /* _DSRSP_H */
