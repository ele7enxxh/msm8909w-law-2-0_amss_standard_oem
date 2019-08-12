/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                        D A T A     S E R V I C E S

                  R O U T E   S E L E C T I O N   P R O T O C O L

GENERAL DESCRIPTION
  This file contains the implementation of the DS Route Selection Protocol.

  When FEATURE_HDR_EMPA is not defined, or EMPA is not negotiated,
  this file is a trivial pass through; else the heavy lifting is
  delegated to HDRRSP.

INITIALIZATION AND SEQUENCING REQUIREMENTS

 Copyright (c) 2006-2011 by Qualcomm Technologies Incorporated. All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*=====*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp2/ds707/src/dsrsp.c#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------
11/18/11   vpk     HDR SU API cleanup
11/22/10   sn      Removed REX premption macros as it is not required.
05/30/06   ksu     EMPA: RLP init code review comments
05/04/06   ksu     EMPA: RSP part 2
03/27/06   ksu     EMPA: add route to fb_cookie
03/24/06   ksu     split out dsrsp from hdrrsp

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "datamodem_variation.h"
#include "dsrsp.h"
#if (defined(FEATURE_HDR_EMPA) || defined(FEATURE_HDR_REVB))
#include "hdrrsp.h"
#include "hdrcom_api.h"
#include "ds707_sec_pkt_mgr_handoff.h"
#endif /* (defined(FEATURE_HDR_EMPA) || defined(FEATURE_HDR_REVB)) */

#ifdef FEATURE_DATA_PKT_CLASSIFIER
#include "ds707_pkt_classifier.h"
#endif/* FEATURE_DATA_PKT_CLASSIFIER */
/*===========================================================================

FUNCTION DSRSP_WM_CURRENT_CNT

DESCRIPTION
  Returns the total number of bytes ready for tx in all routes.

DEPENDENCIES
  The watermark list must have at least DSRSP_NUM_ROUTES entries, each
  of which must have been properly initialized.

RETURN VALUE
  uint32 - the current count in all routes

SIDE EFFECTS
  None
===========================================================================*/
uint32 dsrsp_wm_current_cnt
(
  dsrlp_rlp_identity_type* rlp_id_ptr,
  dsm_watermark_type wm_list[]         /* Tx wm list[DSRSP_NUM_ROUTES]    */
)
{
#if (defined(FEATURE_HDR_EMPA) || defined(FEATURE_HDR_REVB))
  if ((rlp_id_ptr->ver == DSRLP_VER_HDR_MULTIFLOW_REVERSE) && 
      ((ds707_sec_pkt_mgr_handoff_last_pkt_app() == HDRHAI_ENHANCED_MULTIFLOW_PACKET_APP_SN) ||
      (ds707_sec_pkt_mgr_handoff_last_pkt_app() == HDRHAI_MULTILINK_MULTIFLOW_PACKET_APP_SN)
       ))
  {
    return hdrrsp_wm_current_cnt(rlp_id_ptr, wm_list);
  }
  else
#endif /* FEATURE_HDR_EMPA || FEATURE_HDR_REVB */
  {
    return wm_list[DSRSP_ROUTE_A].current_cnt;
  }
}

/*===========================================================================

FUNCTION DSRSP_WM_FREE_CNT

DESCRIPTION
  Returns the total number of bytes free in the selected route

DEPENDENCIES
  The watermark list must have at least DSRSP_NUM_ROUTES entries, each
  of which must have been properly initialized.

RETURN VALUE
  uint32 - the free count in the selected route

SIDE EFFECTS
  None
===========================================================================*/
uint32 dsrsp_wm_free_cnt
(
  dsrlp_rlp_identity_type* rlp_id_ptr,
  dsm_watermark_type wm_list[]         /* Tx wm list[DSRSP_NUM_ROUTES]    */
)
{
#if (defined(FEATURE_HDR_EMPA) || defined(FEATURE_HDR_REVB))
  if ((rlp_id_ptr->ver == DSRLP_VER_HDR_MULTIFLOW_REVERSE) && 
      ((ds707_sec_pkt_mgr_handoff_last_pkt_app() == HDRHAI_ENHANCED_MULTIFLOW_PACKET_APP_SN) ||
      (ds707_sec_pkt_mgr_handoff_last_pkt_app() == HDRHAI_MULTILINK_MULTIFLOW_PACKET_APP_SN)
       ))
  {
    return hdrrsp_wm_free_cnt(rlp_id_ptr, wm_list);
  }
  else
#endif /* FEATURE_HDR_EMPA || FEATURE_HDR_REVB */
  {
    /*-----------------------------------------------------------------------
    Number of bytes that can still be filled in the watermark.
    If hi_watermark < current_cnt then the arithmetic would result in a
    wrong value, since wm_free_cnt is a uint32, and since there is no free
    space left we return the wm_free_cnt as "zero"
    -----------------------------------------------------------------------*/
    if (wm_list[DSRSP_ROUTE_A].hi_watermark < 
        wm_list[DSRSP_ROUTE_A].current_cnt)
    {
      return 0;
    }
    else
    {
      return wm_list[DSRSP_ROUTE_A].hi_watermark -
        wm_list[DSRSP_ROUTE_A].current_cnt;
    }
  }
}

/*===========================================================================
FUNCTION DSRSP_DSM_ENQUEUE()

DESCRIPTION
  This function will put the passed DSM item to the passed shared queue list 
  then check for and perform any 'put' events.

DEPENDENCIES
  The watermark list must have at least DSRSP_NUM_ROUTES entries, each
  of which must have been properly initialized.

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void dsrsp_dsm_enqueue
(
  const dsrlp_rlp_identity_type* rlp_id_ptr,
  dsm_watermark_type wm_list[],
  dsm_item_type **pkt_head_ptr,
  ds1x_meta_info_type *ds1x_meta_info_ptr
)
{
#if (defined(FEATURE_HDR_EMPA) || defined(FEATURE_HDR_REVB))
  if ((rlp_id_ptr->ver == DSRLP_VER_HDR_MULTIFLOW_REVERSE) && 
      ((ds707_sec_pkt_mgr_handoff_last_pkt_app() == HDRHAI_ENHANCED_MULTIFLOW_PACKET_APP_SN) ||
      (ds707_sec_pkt_mgr_handoff_last_pkt_app() == HDRHAI_MULTILINK_MULTIFLOW_PACKET_APP_SN)
       ))
  {
    hdrrsp_dsm_enqueue(rlp_id_ptr, wm_list, pkt_head_ptr, ds1x_meta_info_ptr);
  }
  else
#endif /* FEATURE_HDR_EMPA || FEATURE_HDR_REVB */
  {
    if (ds1x_meta_info_ptr) 
    {
      (void) dsm_pushdown( pkt_head_ptr,
                    ds1x_meta_info_ptr,
                    sizeof(ds1x_meta_info_type),
                    DSM_DS_SMALL_ITEM_POOL);
    }

    /*-----------------------------------------------------------------------
      DSM enqueue function has WM lock and unlock. Critical section is not 
      required.
    -----------------------------------------------------------------------*/
    dsm_enqueue(&wm_list[DSRSP_ROUTE_A], pkt_head_ptr);

#ifdef FEATURE_DATA_PKT_CLASSIFIER
    ds707_pkt_classifier_route_um_tx_sn_data(&wm_list[DSRSP_ROUTE_A]);
#endif/* FEATURE_DATA_PKT_CLASSIFIER */
  }
}

/*===========================================================================

FUNCTION DSRSP_Q_GET

DESCRIPTION
  This function removes an item from the head of a specified watermark
  list, and passes it through any configured route protocol.

DEPENDENCIES
  The watermark list must have at least DSRSP_NUM_ROUTES entries, each of
  which must have been initialized previously via a call to dsm_queue_init / 
  q_init.

RETURN VALUE
  A pointer to the dequeued item. If the specified queue list is empty, then
  NULL is returned.

SIDE EFFECTS
  The head item, if any, is removed from the specified queue list.

===========================================================================*/
dsm_item_type* dsrsp_q_get
(
  const dsrlp_rlp_identity_type* rlp_id_ptr,
  dsm_watermark_type wm_list[]
)
{
#if (defined(FEATURE_HDR_EMPA) || defined(FEATURE_HDR_REVB))
  if ((rlp_id_ptr->ver == DSRLP_VER_HDR_MULTIFLOW_FORWARD) && 
      ((ds707_sec_pkt_mgr_handoff_last_pkt_app() == HDRHAI_ENHANCED_MULTIFLOW_PACKET_APP_SN) ||
      (ds707_sec_pkt_mgr_handoff_last_pkt_app() == HDRHAI_MULTILINK_MULTIFLOW_PACKET_APP_SN)
       ))
  {
    return hdrrsp_q_get(rlp_id_ptr, wm_list);
  }
  else
#endif /* FEATURE_HDR_EMPA || FEATURE_HDR_REVB */
  {
    return dsm_dequeue(&wm_list[DSRSP_ROUTE_A]);
  }
}

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
byte dsrsp_get_open_route(void)
{
#if (defined(FEATURE_HDR_EMPA) || defined(FEATURE_HDR_REVB))
  if ((ds707_sec_pkt_mgr_handoff_last_pkt_app() == HDRHAI_ENHANCED_MULTIFLOW_PACKET_APP_SN) ||
      (ds707_sec_pkt_mgr_handoff_last_pkt_app() == HDRHAI_MULTILINK_MULTIFLOW_PACKET_APP_SN)
       )
  {
    return hdrrsp_get_open_route();
  }
  else
#endif /* FEATURE_HDR_EMPA || FEATURE_HDR_REVB */  
  {
    return DSRSP_ROUTE_A;
  }
}
