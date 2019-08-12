#ifndef DS707_WMK_H
#define DS707_WMK_H
/*===========================================================================
                         D S 7 0 7 _ W M K
GENERAL DESCRIPTION
  This file contains the configuration for the watermarks which go between
  PPP and RLP.  Includes the functions called at particular watermark 
  events. 
  
  This is specific to packet data calls.

EXTERNALIZED FUNCTIONS
 DS707_WMK_UM_TX_LO_WM_FUNC
   Registered with watermark.  Called when low watermark setting reached.
   Enables flow control on the 1x ps_iface (basically allows PS to start
   sending data to RLP).
   
 DS707_WMK_UM_TX_HI_WM_FUNC
   Registered with watermark.  Called when hi watermark setting reached.
   Disables flow control on the 1x ps_iface (basically tells PS to stop
   sending data to RLP).
   
 DS707_WMK_INIT
   Called at startup.  Initializes the watermarks and underlying 
   queues.
 
 DS707_WMK_SETUP_WMKS
   Called to setup the watermarks and set various levels, etc.
 
 DS707_WMK_CLEANUP_WMKS
   Cleans up the watermarks at the end of a call.  Frees up any
   remaining DSM items.
 
 INITIALIZATION AND SEQUENCING REQUIREMENTS
   Must call ds707_wmk_init() at startup.

 Copyright (c) 2002 - 2014 by Qualcomm Technologies Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================
                        EDIT HISTORY FOR MODULE

  $PVCSPath: L:/src/asw/MM_DATA/vcs/ds707_wmk.h_v   1.2   19 Nov 2002 19:23:58   akhare  $
  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp2/ds707/inc/ds707_wmk.h#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $
  
when       who        what, where, why
--------   ---        -------------------------------------------------------
06/04/14   sd         Fix to utilize DSM memory event callbacks to enable and
                      disable flow control.
07/17/12   ms         Dynamic back pressure removal support for TCP throughput
                      optimization.
08/16/11   vpk        Support memory flow control logging
03/15/11   mg         Global variable cleanup
02/11/11   ack        Global Variable Cleanup
01/11/11   ssh        Featurize NPA code
04/26/10   gc         Added function get_prev_call_byte_counts
11/10/08   yz         Added a few functions for RX/TX byte count fix
08/20/07   sk         API is provided to count free watermark
11/09/06   vrk        Post command to DS to process tx high and low wmk
03/16/06   sk         Updated watermark behavior
12/23/05   ksu        EMPA RSP: two fwd rx queues
12/23/05   ksu        EMPA RSP: two rev watermarks per ip flow
12/16/05   TMR        Replaced RLP RX queue with watermark
10/03/05   vas        Changed prototype of ds707_wmk_alloc_sec_wm to take PS 
                      Flow as argument
06/13/05   vas        Rewrite of code to support Semi-dynamic QoS. Added new
                      external functions.
09/10/04   vas        QOS related changes
05/21/03   vas        Removed AN watermarks. moved them to separate file.
07/22/02   ak         Added ds707_cleanup_wmk to file.
02/20/02   ak         First version of file.
===========================================================================*/


/*===========================================================================
                     INCLUDE FILES FOR MODULE
===========================================================================*/

#include "datamodem_variation.h"
#include "comdef.h"
#include "customer.h"
#ifdef FEATURE_DATA_IS707
#include "dsm.h"
#include "queue.h"
#include "ds707_pkt_mgri.h"
#include "ps_iface_defs.h"
#include "dsm_queue.h"

/*---------------------------------------------------------------------------
 Watermark for UM FWD and REV links
---------------------------------------------------------------------------*/
typedef struct
{
  uint32 ds707_wmk_fwd_lo_wmk;
  uint32 ds707_wmk_fwd_hi_wmk;
  uint32 ds707_wmk_pri_lo_wmk;
  uint32 ds707_wmk_pri_hi_wmk;
  uint32 ds707_wmk_pri_dont_exceed_cnt;
  uint32 ds707_wmk_sec_lo_wmk;
  uint32 ds707_wmk_sec_hi_wmk;
  uint32 ds707_wmk_sec_dont_exceed_cnt;
} ds707_wmk_type;
/*===========================================================================
                        EXTERNAL FUNCTIONS
===========================================================================*/
/*===========================================================================
FUNCTION      DS707_WMK_INIT

DESCRIPTION   Called at code startup (i.e, only once).  Sets up queues with
              watermarks, etc.

DEPENDENCIES  Called only when the phone powers up.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
void ds707_wmk_init
( 
  void
);

/*===========================================================================
FUNCTION      DS707_WMK_FREE_SEC_WM_COUNT

DESCRIPTION   Counts free secondary watermarks
                            
DEPENDENCIES  

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
void ds707_wmk_free_sec_wm_count
(
  uint8 *free_wmk_count
);

/*===========================================================================
FUNCTION      DS707_WMK_ALLOC_SEC_WM_LIST

DESCRIPTION   Allocates a secondary watermark. Gets this watermark entry
              from the sec wm free queue.

              The function takes a ps_flow_ptr as an arguement and binds the
              watermark to that particular PS Flow. Events such as high or
              low watermark count being reach trigger action on the 
              corresponding PS Flow

DEPENDENCIES  

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
dsm_watermark_type * ds707_wmk_alloc_sec_wm_list
( 
  ps_flow_type  * ps_flow_ptr
);

/*===========================================================================
FUNCTION      DS707_WMK_FREE_SEC_WM_LIST

DESCRIPTION   Frees a secondary watermark. Returns this watermark entry
              to the sec wm free queue.

DEPENDENCIES  

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
void ds707_wmk_free_sec_wm_list
( 
  dsm_watermark_type wm_list[] 
);

/*===========================================================================
FUNCTION      DS707_WMK_GET_PRI_WM_LIST

DESCRIPTION   Returns a pointer to the primary watermark

DEPENDENCIES  

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
dsm_watermark_type * ds707_wmk_get_pri_wm_list
( 
  void 
);

/*===========================================================================
FUNCTION      DS707_WMK_SETUP_WMKS

DESCRIPTION   This function will initialize the service-configurable DS task
              shared watermark items to operate for the current data service.
              The 'dsi_srvc_mode' variable determines the service mode to
              setup the Watermark items for.

DEPENDENCIES  Hi/Lo Watermark Settings: The Lo and Hi watermark values and
              the don't exceed values for each watermark is inter-dependent
              with the DSM small and large item counts (DSM_LARGE_ITEM_CNT
              and DSM_SMALL_ITEM_CNT) in the DSM module. Any change to lo/hi
              watermark or don't exceed values can cause ERR_FATALs due to
              insufficient memory items available.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
void ds707_wmk_setup_wmks
(
  void
);

/*===========================================================================
FUNCTION      DS707_WMK_CLEANUP_PRI_WMK

DESCRIPTION   This function cleans up the watermarks that are owned by 
              data services.

DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
void ds707_wmk_cleanup_pri_wmk
(
  void
);

/*===========================================================================
FUNCTION      DS707_WMK_CLEANUP_SEC_WMKS

DESCRIPTION   This function cleans up the watermarks that are owned by 
              data services.

DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
void ds707_wmk_cleanup_sec_wmks
(
  void
);

/*===========================================================================
FUNCTION      DS707_WMK_CLEANUP_SINGLE_WMK

DESCRIPTION   This function cleans up the watermark specified in the input
              parameter.

DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
void ds707_wmk_cleanup_single_wmk
(
  dsm_watermark_type wmk_list[]
);

/*===========================================================================
FUNCTION      DS707_WMK_CLEANUP_FWD_QUEUE

DESCRIPTION   This function cleans up the forward queue

DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
void ds707_wmk_cleanup_fwd_queue
(
  void
);


/*===========================================================================
FUNCTION      DS707_WMK_PROCESS_UM_TX_LO_WM

DESCRIPTION   This function is called when processing DS_707_UM_TX_HI_WM_CMD
              from the DS command buffer. This runs in the DS context.

DEPENDENCIES  Both the ds707_wmki_um_tx_lo_wm_func and 
              ds707_wmki_um_tx_hi_wm_func
              should run in the same task context. Note that in the future 
              ps_flow_disable_tx and ps_flow_enable_tx should also be called 
              from the same task context (namely DS). 

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
void ds707_wmk_process_um_tx_hi_wm
( 
  ds_cmd_type   *ds_cmd_ptr 
);

/*===========================================================================
FUNCTION      DS707_WMK_PROCESS_UM_TX_HI_WM

DESCRIPTION   This function is called when processing DS_707_UM_TX_HI_WM_CMD
              from the DS command buffer. This runs in the DS context.

DEPENDENCIES  Both the ds707_wmki_um_tx_lo_wm_func and 
              ds707_wmki_um_tx_hi_wm_func
              should run in the same task context. Note that in the future 
              ps_flow_disable_tx and ps_flow_enable_tx should also be called 
              from the same task context (namely DS). 

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/

void ds707_wmk_process_um_tx_lo_wm
( 
  ds_cmd_type   *ds_cmd_ptr 
);

/*===========================================================================
FUNCTION      DS707_WMKI_PROCESS_UM_RX_HI_WM

DESCRIPTION   This function is called when processing DS_707_UM_RX_HI_WM_CMD
              from the DS command buffer. This runs in the DS context.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/

void ds707_wmk_process_um_rx_hi_wm
( 
  ds_cmd_type   *ds_cmd_ptr 
);

/*===========================================================================
FUNCTION      DS707_WMK_PROCESS_UM_RX_LO_WM

DESCRIPTION   This function is called when processing DS_707_UM_RX_LO_WM_CMD
              from the DS command buffer. This runs in the DS context.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/

void ds707_wmk_process_um_rx_lo_wm 
( 
  ds_cmd_type   *ds_cmd_ptr 
);
/*===========================================================================
FUNCTION      DS707_WMK_RESET_RX_TX_BYTE_COUNT

DESCRIPTION   This function resets RX and TX byte count for a PS call.

DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
void ds707_wmk_reset_RX_TX_byte_count
(
  void
);

/*===========================================================================
FUNCTION DS707_WMK_GET_BYTE_COUNTS()

DESCRIPTION
  This function returns the # of txed and rxed bytes from the TX/RX 
  Watermarks
  
PARAMETERS
  tx_bytes, out parameter, filled with the # of bytes received from TE2
  rx_bytes, out parameter, filled with the # of bytes transmitted to TE2

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
void ds707_wmk_get_byte_counts
(
  uint32 *txed_bytes,
  uint32 *rxed_bytes,
  dsm_watermark_type tx_wmk_list[],
  dsm_watermark_type rx_wmk_list[]
);

/*===========================================================================
FUNCTION DS707_WMK_UPDATE_RX_COUNT()

DESCRIPTION
  This function updates the RX byte count for a data call
  
PARAMETERS
  wmk_list - list of RX watermark 

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
void ds707_wmk_update_rx_count
(
  dsm_watermark_type wmk_list[]
);
/*===========================================================================
FUNCTION DS707_WMK_UPDATE_TX_COUNT()

DESCRIPTION
  This function updates the TX byte count from primary tx wmk for a data call
  
PARAMETERS
  wmk_list - list of TX watermark 

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
void ds707_wmk_update_tx_count
(
  dsm_watermark_type wmk_list[]
);

/*===========================================================================
FUNCTION DS707_WMK_UPDATE_WMK_VAULE()

DESCRIPTION
  This function updates the UM FWD/REV link Watermark value based on 
  technology info getting from HDR
  
PARAMETERS
  None 

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
void ds707_wmk_update_wmk_value
(
  void
);


/*===========================================================================
FUNCTION DS707_WMK_GET_PREV_CALL_BYTE_COUNTS()

DESCRIPTION
  This function returns the # of txed and rxed bytes from the previous call
  
PARAMETERS
  tx_bytes, out parameter, filled with the # of bytes received in the previous call
  rx_bytes, out parameter, filled with the # of bytes transmitted in the previous call
  
RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
void ds707_wmk_get_prev_call_byte_counts
(
  uint32 * txed_bytes,
  uint32 * rxed_bytes
);
  
/*===========================================================================
FUNCTION DS707_GET_WMK_UM_SN_FWD_WM_LIST()

DESCRIPTION
  Accessor function to ds707_wmk_um_SN_fwd_wm_list
  
PARAMETERS
  None
  
RETURN VALUE
  Returns a pointer to ds707_wmk_um_SN_fwd_wm_list

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
dsm_watermark_type* ds707_get_wmk_um_SN_fwd_wm_list
(
  void
);  


/*===========================================================================
FUNCTION DS707_WMK_GET_WMK_LEVEL

DESCRIPTION
  This function returns watermarks low and high levele value

PARAMETERS
  None

RETURN VALUE
   None

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
void ds707_wmk_get_wmk_levels
(
  ds707_wmk_type *wmk_type_ptr
);

/*===========================================================================
FUNCTION      DS707_WMK_SET_UM_TX_HIGH_WM_FUNC

DESCRIPTION   If DISABLE is FALSE,then it restores BP enablement. 
              If DISABLE is TRUE,then overwrite hi_wmk_level with DNE values and
              set high wm func to NULL.

DEPENDENCIES  None

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
void ds707_wmk_set_um_tx_high_wm_func 
(
  boolean disable
);

/*===========================================================================
FUNCTION      DS707_GET_CURRENT_TX_WMK_CNT

DESCRIPTION   This function returns the current total byte count 
              in UM TX watermark.

DEPENDENCIES  None

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
uint32 ds707_get_current_tx_wmk_cnt
(
  void
);

/*===========================================================================
FUNCTION      DS707_GET_CURRENT_RX_WMK_CNT

DESCRIPTION   This function returns the current total byte count 
              in UM RX watermark.

DEPENDENCIES  None

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
uint32 ds707_get_current_rx_wmk_cnt
(
  void
);
/*===========================================================================
FUNCTION      DS707_GET_TOTAL_TX_WMK_CNT

DESCRIPTION   This function returns the current total byte count 
              in UM TX watermark.

DEPENDENCIES  None

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
uint64 ds707_get_total_tx_wmk_cnt
(
   void
);

/*===========================================================================
FUNCTION      DS707_DSM_FC_ENABLE

DESCRIPTION   This function is called to enable flow control when DSM items
              reach DSM_MEM_LEVEL_LINK_LAYER_FEW level.

DEPENDENCIES  None

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
void ds707_dsm_fc_enable
(
  void
);

/*===========================================================================
FUNCTION      DS707_DSM_FC_DISABLE

DESCRIPTION   This function is called to disable flow control when DSM items
              reach DSM_MEM_LEVEL_LINK_LAYER_MANY level.

DEPENDENCIES  None

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
void ds707_dsm_fc_disable
(
  void
);

#endif /* FEATURE_DATA_IS707 */
#endif /* DS707_WMK_H        */
