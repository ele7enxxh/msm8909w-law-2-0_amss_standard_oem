/*===========================================================================

                         D S 7 0 7 _ W M K
GENERAL DESCRIPTION
  This file contains the configuration for the watermarks which go between
  PPP and RLP.  Includes the functions called at particular watermark
  events.

  Note that each watermark is associated with a particular PS FLow. When high
  or Low watermark events are triggered, action is taken on the corresponding
  PS Flow.

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

Copyright (c) 2002 - 2015 by Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary.
===========================================================================*/

/*===========================================================================
                        EDIT HISTORY FOR MODULE

  $PVCSPath: L:/src/asw/MM_DATA/vcs/ds707_wmk.c_v   1.2   19 Nov 2002 19:23:50   akhare  $
  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp2/ds707/src/ds707_wmk.c#1 $ 
  $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when       who        what, where, why
--------   ---        ------------------------------------------------------- 
03/23/15   sc         Initialize high and low sec WM during allocation
07/04/14   sc         Optimize the code to reduce the static memory
07/02/14   sd         Interfacing with CFCM for CPU and Thermal based flow
                      control events.
06/04/14   sd         Fix to utilize DSM memory event callbacks to enable and
                      disable flow control.
06/29/14   sc         Remove check for posting disable flow cmd to PS
07/17/12   ms         Dynamic back pressure removal support for TCP throughput
                      optimization.
05/25/12   msh        Remove write wmk values to NV 
06/25/12   msh        Remove check for posting enable flow cmd to PS
05/31/12   msh        Increase DNE wmk level for UM WMK 
11/18/11   vpk        HDR SU API cleanup
10/01/11   vpk        Post commands to PS for flow control
09/23/11   ms         Modifying feature to read wmk levels from NV  
08/22/11   ms         Added feature to read wmk levels from NV  
08/16/11   vpk        Support memory flow control logging
03/15/11   mg         Global variable cleanup
02/11/11   ack        Global Variable Cleanup
01/11/11   ssh        Featurize NPA code.
12/23/10   ls         Adjust DNE WM level for Fusion
10/13/10   op         Migrated to MSG 2.0 macros
09/27/10   ls         Flow control without posting DS command on Q6
08/23/10   ls         Adjust WM level for FWD WM
07/21/10   ssh        Added impulse client for data - kicks in on high fwd um wm.
06/17/10   ms         Set each_enqueue_fptr to NULL while de-allocating 
                      sec watermarks buffers.
06/10/10   pr         Fixed Compilation Warnings 
05/17/10   ls         Modify CCM featurization
04/26/10   gc         Lint Fixes
04/21/10   gc         Added code to store previous call counters, and move some
                      event processing to this module.

04/16/10   ls         Fix the featurization issues
02/05/10   ss         Remove q_init for the watermark queue as it is already 
                      initialized inside dsm_queue_init.
12/21/09   spn        Add Queue initializations.
04/21/09   ls         In ds707_wmki_hi_wm_disable_flow(), it will check 
                      current_cnt to see if it is less than low_watermark.
                      If it is, ignore the Hi WM to avoid delayed processing
                      Hi WM
03/13/08   psng       Fixed off target medium and low lint errors.
02/17/08   ms         Fixed stack overflow issue by removing local 
                      variables.
11/26/08   psng       Fixed off target lint errors and compiler warnings.
11/10/08   yz         Fixed TX/RX byte count issue with wmk approach, 
                      reverted previous changes in rlp
08/07/08   as         Lint error fixes.
07/18/08   spn        Increased Watermark levels when the Enh Revb Rates are
                      supported
06/04/08   spn        Featurization of watermark levels
07/02/08  mga         Merged and made changes to check if ps_flow is NULL and
                      also to check if wmk_buf_ptr->allocated is FALSE.
08/20/07   sk         API is provided to count free watermark
07/12/07   ssr        Make the LSB as zero while extracting wmk_buf_ptr
05/08/07   az         Made processing of Low and high watermarks more
                      modularized so that IPv6 is supported.
04/30/07   ks         Increased wmk levels.
11/09/06   vrk        Post command to DS to process tx high and low wmk
07/12/06   ksu        EMPA: RSP part 2 code review comments
07/11/06   sq         Updated DS707_WMK_PRI HI/LO wmk to the values suggested
                      by performance team.
05/30/06   ksu        EMPA: RLP init code review comments
05/04/06   ksu        EMPA: RSP part 2
03/16/06   sk         Updated watermark behavior
2/21/06    sq         increse hi/lo WM values to 5000/2000
12/23/05   ksu        EMPA RSP: two fwd rx queues
12/23/05   ksu        EMPA RSP: two rev watermarks per ip flow
12/16/05   TMR        Replaced RLP RX queue with watermark
10/03/05   vas        Fixed flow control of watermarks to be based per PS 
                      flow instead of phys_link based.
06/14/05   vas        Drastic rewrite of code to support watermarks for
                      Semi-dynamic QoS.
03/07/05   vas        Include sec_pkt_mgr.h instead of sec_pkt_mgri.h
10/21/04   kvd        cleaned up DS707 macros to be called with correct
                      pkt_inst.
09/08/04   vas        Changes to support QOS and multiple watermarks for
                      secondary TCs
04/22/04   ak         Made ds707_sdb_curr_pkt_needs_SDB global.
02/04/04   ak         Updated to use new pkt mgr event callbacks.
08/08/03   rsl        Support for DSM 2.0
05/21/02   vas        Removed AN watermarks. Moved these to separate file.
07/16/02   ak         Minor comment cleanup.
07/14/02   ak         Added cleanup_wmk function.
02/20/02   ak         First version of file.
===========================================================================*/


/*===========================================================================
                     INCLUDE FILES FOR MODULE
===========================================================================*/
#include "datamodem_variation.h"
#include "comdef.h"
#include "customer.h"
#include "data_msg.h"
#ifdef FEATURE_DATA_IS707
  #ifdef FEATURE_DATA_WMK_CONFIG
    #include "ds707_wmk_lvl.h"
  #endif /*FEATURE_DATA_WMK_CONFIG*/
  #define FEATURE_DSM_WM_CB
  #include "dstask.h"
  #include "ds707_wmk.h"
  #include "ds707_pkt_mgr.h"
  #include "ds_flow_control.h"
  #include "dsm.h"
  #include "queue.h"
  #include "rex.h"
  #include "ps_flow.h"
  #include "event_defs.h"
  #include "ds707_event_defs.h"
  #include "dsrsp.h"
  #include "ds707_hi_spd_clk.h"

  #ifdef FEATURE_HDR_QOS
    #include "ds707_sec_pkt_mgr.h"
  #endif
  #ifdef FEATURE_HDR_REVB
    #include "hdrcom_api.h"
  #endif
  #include "dsrlp_v.h"
  #include "dstaski.h"

#include "data_err_suppr.h"

#if defined(FEATURE_DATA_FL_FLOW_CONTROL)
#include "ds707_ccm.h"
#else
#include "hdrcom_api.h"
#endif

/*===========================================================================
                            TYPEDEFS
===========================================================================*/
/*---------------------------------------------------------------------------
 Structure maintained for each watermark. This contains
 1. a flag indicating whether this watermark list has been allocated.
 2. the actual watermark list,
 3. a queue list associated with the watermark list
 4. ps_flow associated with the watermark list

  NOTE: ps_flow_ptr needs to be the last element of the two structures  
  below. The Structure should also look the same
---------------------------------------------------------------------------*/
typedef struct
{
  boolean            allocated;
  dsm_watermark_type um_SN_rev_wmk_list[DSRSP_NUM_ROUTES];
  q_type             um_SN_rev_q_list[DSRSP_NUM_ROUTES];
  ps_flow_type       *ps_flow_ptr;
} ds707_wmk_buf_type;

/*---------------------------------------------------------------------------
  Same as above structure, but the only difference being an array 
  of ps_flows. This array is of default ps_flows for all PDNs that are 
  bound to the primary watermark. 

  NOTE: ps_flow_ptr needs to be the last element of the structures above 
  and below. The Structure should also look the same
---------------------------------------------------------------------------*/
typedef struct
{
  boolean            allocated;
  dsm_watermark_type um_SN_rev_wmk_list[DSRSP_NUM_ROUTES];
  q_type             um_SN_rev_q_list[DSRSP_NUM_ROUTES];
  ps_flow_type       *ps_flow_ptr[DS707_MAX_IFACES];
} ds707_pri_wmk_buf_type;

/*---------------------------------------------------------------------------
 counters for TX and RX byte count in a data call
---------------------------------------------------------------------------*/
typedef struct
{
  uint32 TX_byte_count;
  uint32 RX_byte_count;
  uint32 uPrev_call_TX_byte_count;
  uint32 uPrev_call_RX_byte_count;
} ds707_wmk_byte_count_type;


/*===========================================================================
                            VARIABLES
===========================================================================*/
/*---------------------------------------------------------------------------
  Define different Watermark Levels for Forward Link Tx Watermark based on
  whether RevB is defined or not.
---------------------------------------------------------------------------*/
#ifdef FEATURE_HDR_REVB
#ifdef FEATURE_HDR_REVB_ENH_RATES
    #define DS707_WMK_REVB_FWD_LNK_LO_WATERMARK 350000
    #define DS707_WMK_REVB_FWD_LNK_HI_WATERMARK 400000
#else /* FEATURE_HDR_REVB_ENH_RATES */
    #define DS707_WMK_REVB_FWD_LNK_LO_WATERMARK 500000
    #define DS707_WMK_REVB_FWD_LNK_HI_WATERMARK 550000
#endif /* FEATURE_HDR_REVB_ENH_RATES */

  #define DS707_WMK_REVB_PRI_LO_WATERMARK 150000
  #define DS707_WMK_REVB_PRI_HI_WATERMARK 200000
  #define DS707_WMK_REVB_PRI_DONT_EXCEED_CNT 500000

  #define DS707_WMK_REVB_SEC_LO_WATERMARK 20000
  #define DS707_WMK_REVB_SEC_HI_WATERMARK 40000
  #define DS707_WMK_REVB_SEC_DONT_EXCEED_CNT 50000
#endif

#define DS707_WMK_REVA_FWD_LNK_LO_WATERMARK 5000
#define DS707_WMK_REVA_FWD_LNK_HI_WATERMARK 20000

#define DS707_WMK_REVA_PRI_LO_WATERMARK 4500
#define DS707_WMK_REVA_PRI_HI_WATERMARK 7500
#define DS707_WMK_REVA_PRI_DONT_EXCEED_CNT 150000

#define DS707_WMK_REVA_SEC_LO_WATERMARK 4500
#define DS707_WMK_REVA_SEC_HI_WATERMARK 7500
#define DS707_WMK_REVA_SEC_DONT_EXCEED_CNT 32000

/*---------------------------------------------------------------------------
  Queue for all data received by the various RLPs on the forward link
---------------------------------------------------------------------------*/
static q_type ds707_wmk_um_SN_fwd_q_list[DSRSP_NUM_ROUTES];    /* rlp fwd link queues */
static dsm_watermark_type ds707_wmk_um_SN_fwd_wm_list[DSRSP_NUM_ROUTES]; /* rlp fwd link watermarks */

/*---------------------------------------------------------------------------
 Buffers for the secondary watermarks
---------------------------------------------------------------------------*/
static ds707_wmk_buf_type **ds707_wmk_sec_buf;

/*---------------------------------------------------------------------------
 Buffer for the primary watermark
---------------------------------------------------------------------------*/
static ds707_pri_wmk_buf_type ds707_wmk_pri_buf;

/*---------------------------------------------------------------------------
 Global structure to keep track of byte count across dormancies within a  call.
---------------------------------------------------------------------------*/
static ds707_wmk_byte_count_type ds707_wmk_byte_count;

/*---------------------------------------------------------------------------
 Global structure to keep track of FWD/REV link watermark 
---------------------------------------------------------------------------*/
static ds707_wmk_type ds707_wmk_value;


/*===========================================================================
                        INTERNAL FUNCTIONS
===========================================================================*/
LOCAL void ds707_wmki_process_um_tx_lo_wm
( 
  void *user_data_ptr 
);

LOCAL void ds707_wmki_process_um_tx_hi_wm
( 
  void *user_data_ptr 
);
/*===========================================================================
FUNCTION      DS707_WMK_UM_TX_LO_WM_FUNC

DESCRIPTION   Called (via function pointer) when the tx watermark reg'd
              with RLP hits the low watermark event. This function posts
              a command to the DS task

DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
/*lint -save -e715 */
LOCAL void ds707_wmki_um_tx_lo_wm_func
(
  struct dsm_watermark_type_s *wm_ptr,
  void *user_data_ptr
)
{
  ds707_wmk_buf_type *wmk_buf_ptr = NULL;
#ifndef FEATURE_DSM_QUBE
  ds_cmd_type        *ds_cmd_ptr;
#endif 
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  wmk_buf_ptr = (ds707_wmk_buf_type *) ((uint32)user_data_ptr & ~0x1);

  /*-------------------------------------------------------------------------
  Enable flow control for this PS Flow
  -------------------------------------------------------------------------*/
  if ((NULL == wmk_buf_ptr) || (NULL == wmk_buf_ptr->ps_flow_ptr))
  {
    DATA_IS707_MSG0(MSG_LEGACY_ERROR, "wmk_buf_ptr or ps_flow_ptr is NULL");
    return;
  }

  if ( ds707_get_dsm_fc_flag() != TRUE )
  {
    DATA_IS707_MSG3( MSG_LEGACY_HIGH,
                     "UM TX Low watermark reached for WM 0x%x ps_flow:0x%x "
                     "count = %d",
                     wm_ptr, 
                     wmk_buf_ptr->ps_flow_ptr, 
                     wm_ptr->current_cnt);
  }

#ifdef FEATURE_DSM_QUBE
  /*-------------------------------------------------------------------------
  This function is called as a callback when we reach the High watermark
  under qube mutex (dsmi_enqueue). We shall be able to call ps_disable_flow 
  which uses critical section. If FEATURE_DSM_QUBE is defined, DSM shall use
  mutex instead of INTER LOCK
  -------------------------------------------------------------------------*/
  ds707_wmki_process_um_tx_lo_wm( user_data_ptr );
#else
  /*-------------------------------------------------------------------------
  This function is called as a callback when we reach the low watermark in
  interrupt context. We inturn call the ps_flow_enable which will result in
  an enter critical section. We cannot enter a critical section in interrupt
  context. So post a command to DS and handle it out of the interrupt context
  -------------------------------------------------------------------------*/
  if ( (ds_cmd_ptr = ds_allocate_cmd_buf(DS_CMD_PAYLOAD_SIZE_ZERO)) == NULL )
  {
    ASSERT(0);
    return;
  }
  else
  {
   /*------------------------------------------------------------------------
     Populate the DS command buffer and post command to DS
   ------------------------------------------------------------------------*/
    (ds_cmd_ptr->hdr).cmd_id = DS_CMD_707_UM_TX_LO_WM;
    cmd_ptr->cmd_payload_ptr = user_data_ptr;
    ds_put_cmd( ds_cmd_ptr );
  }
#endif /* FEATURE_DSM_QUBE */

} /* ds707_wmki_um_tx_lo_wm_func() */
/*lint -restore */

/*===========================================================================
FUNCTION      DS707_WMK_UM_TX_GONE_EMPTY_FUNC

DESCRIPTION   Called (via function pointer) when the tx watermark reg'd
              with RLP gone empty event.

DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
/*lint -save -e715 */
LOCAL void ds707_wmki_um_tx_gone_empty_func
(
struct dsm_watermark_type_s *wm_ptr,
void *user_data_ptr
)
{
  DATA_MDM_ARG_NOT_CONST(user_data_ptr);

  DATA_IS707_MSG1(MSG_LEGACY_HIGH, "UM TX watermark gone empty for WM 0x%x", 
                  wm_ptr);

} /* ds707_wmki_um_tx_gone_empty_func() */
/*lint -restore */

/*===========================================================================
FUNCTION      DS707_WMKI_LO_WM_ENABLE_FLOW

DESCRIPTION   Helper function called for primary v4 and v6 flow to enable
              flow when LOW watermark is reached.

DEPENDENCIES

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
LOCAL void ds707_wmki_lo_wm_enable_flow
(
  ps_flow_type                 * ps_flow,
  struct dsm_watermark_type_s  * wm_list,
  byte                           route
)
{
  ds707_flow_type                 *ds_flow_ptr  = NULL;
  ds707_tc_state_type             *tc_state_ptr = NULL;
  ds_wmk_flow_enabled_event_type   enabled_wmk;

  /*-------------------------------------------------------------------------
  Enable flow only if this PS Flow was disabled. This avoids unnecessary
  calls and message printing if the level fluctates around low watermark
  level. Since, this is data path, it is ok to optimize.
  -------------------------------------------------------------------------*/

  if(ps_flow == NULL)
  {
    DATA_IS707_MSG0(MSG_LEGACY_HIGH, 
            "ps_flow is NULL and the flow may have already been cleaned up");
    return;
  }

  ds_flow_ptr = ds707_pkt_mgr_get_ds_flow(ps_flow);
  if ((NULL == ds_flow_ptr) || (NULL == ds_flow_ptr->rev_tc_ptr))
  {
    DATA_IS707_MSG0( MSG_LEGACY_MED, "ds707_wmki_lo_wm_enable_flow: "
                     "ds_flow or rev_tc_ptr is NULL. Igoring...");
    return;
  }

  tc_state_ptr = ds_flow_ptr->rev_tc_ptr;

  /*-----------------------------------------------------------------------
    RSP: the lowater func could be called because the setting route
    watermark has now been emptied by RLP, but the open route
    watermark may be still full, so check the level of the open
    route to make sure we should reenable the flow.  Note that since
    the free count is defined as "hi - current", we can use it to
    detect "current < lo".
    As an enhancement to back pressure removal feature, added a check 
    to use this function to enable flow again even if low watermark 
    has not reached in case we reach a high level for DSM items.
   ---------------------------------------------------------------------*/
  if ( (dsrsp_wm_free_cnt( &tc_state_ptr->rev_rlp.flow, 
                           ds_flow_ptr->tx_wm_list) >
        (ds_flow_ptr->tx_wm_list[DSRSP_ROUTE_A].hi_watermark -
         ds_flow_ptr->tx_wm_list[DSRSP_ROUTE_A].lo_watermark)) ||
       (ds707_get_dsm_fc_flag() == TRUE) )
  {
    ps_flow_enable_tx_ex(ps_flow, DS_FLOW_PS_FLOW_TX_WM_MASK);

    /*---------------------------------------------------------------------
        Fill up the event payload.
    ---------------------------------------------------------------------*/
    enabled_wmk.wmk_ptr = (void *)(&wm_list[route]);
    enabled_wmk.ps_flow_ptr = (void *) ps_flow;
    enabled_wmk.current_cnt = wm_list[route].current_cnt;
    event_report_payload (EVENT_DS_WMK_FLOW_ENABLED,
                          sizeof(ds_wmk_flow_enabled_event_type),
                          (void*)&enabled_wmk);
  }

} /* ds707_wmki_lo_wm_enable_flow() */

/*===========================================================================
FUNCTION      DS707_WMK_PROCESS_UM_TX_LO_WM

DESCRIPTION   This function is called when processing DS_707_UM_TX_HI_WM_CMD
              from the DS command buffer. This runs in the DS context.

DEPENDENCIES  Both the ds707_wmki_um_tx_lo_wm_func and ds707_wmki_um_tx_hi_wm_func
              should run in the same task context. Note that in the future
              ps_flow_disable_tx and ps_flow_enable_tx should also be called
              from the same task context (namely DS).

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
void ds707_wmk_process_um_tx_lo_wm( ds_cmd_type   *ds_cmd_ptr )
{
  struct dsm_watermark_type_s *wm_list;
  ds707_wmk_buf_type *wmk_buf_ptr = NULL;
  byte route;
  uint8 curr_ps_flow_index;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  DATA_MDM_ARG_NOT_CONST(ds_cmd_ptr);

  if (ds_cmd_ptr == NULL)
  {
    DATA_IS707_MSG0( MSG_LEGACY_ERROR, 
                     "Command pointer NULL in ds707_wmk_process_um_tx_lo_wm");
    return;
  }

  wmk_buf_ptr = (ds707_wmk_buf_type *)(ds_cmd_ptr->cmd_payload_ptr);
  if ((NULL == wmk_buf_ptr) || (FALSE == wmk_buf_ptr->allocated))
  {
    DATA_IS707_MSG0( MSG_LEGACY_ERROR, 
                     "wmk_buf_ptr is NULL or wmk_buf_ptr->allocated is FALSE");
    return;
  }

  /* Obtain the route information from the wmk_buf_ptr to report event and 
   * print diagnostic messages.
   */
  route = (byte)((uint32)wmk_buf_ptr & 0x1);

  /* Reset the LSB to 0 which was used to store route
   */
  wmk_buf_ptr = (ds707_wmk_buf_type *) ((uint32)wmk_buf_ptr & ~0x1);

  if ( wmk_buf_ptr == NULL )
  {
    DATA_IS707_MSG1(MSG_LEGACY_ERROR, "wmk_buf_ptr not set! No Flow control "
                    "possible for WM buf ptr: 0x%x", wmk_buf_ptr);
    return;
  }
  wm_list = wmk_buf_ptr->um_SN_rev_wmk_list;

  DATA_IS707_MSG0( MSG_LEGACY_HIGH, 
                   "UM TX low watermark-processing DS_707_UM_TX_LO_WM_CMD");

  /*-------------------------------------------------------------------------
  Enable flow only if this PS Flow was disabled. This avoids unnecessary
  calls and message printing if the level fluctates around low watermark
  level. Since, this is data path, it is ok to optimize.

  If this is the pri_wmk_buf, flow control all the ps_flows associated 
  with it. 

  NOTE: Type casting for pointer comparison only.
  -------------------------------------------------------------------------*/
  if ( wmk_buf_ptr == (ds707_wmk_buf_type*)&ds707_wmk_pri_buf )
  {
    for ( curr_ps_flow_index = 0;
          curr_ps_flow_index < DS707_MAX_IFACES;
          curr_ps_flow_index ++ )
    {      
      ds707_wmki_lo_wm_enable_flow(
                          ds707_wmk_pri_buf.ps_flow_ptr[curr_ps_flow_index],
                          ds707_wmk_pri_buf.um_SN_rev_wmk_list,
                          route);
    }                                 
  }
  else
  {
    ds707_wmki_lo_wm_enable_flow(wmk_buf_ptr->ps_flow_ptr,
                                 wm_list,
                                 route);
  }
}/*ds707_wmk_process_um_tx_lo_wm*/

/*===========================================================================
FUNCTION      DS707_WMKI_PROCESS_UM_TX_LO_WM

DESCRIPTION   This function is called when DSM hits lower WM. This function
              from dsmi_dequeue() function

DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
LOCAL void ds707_wmki_process_um_tx_lo_wm
( 
  void *user_data_ptr 
)
{
  struct dsm_watermark_type_s *wm_list;
  ds707_wmk_buf_type *wmk_buf_ptr = NULL;
  byte route;
  uint8 curr_ps_flow_index;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (user_data_ptr == NULL)
  {
    DATA_IS707_MSG0( MSG_LEGACY_ERROR, 
                     "Command pointer NULL in ds707_wmki_process_um_tx_lo_wm");
    return;
  }

  wmk_buf_ptr = (ds707_wmk_buf_type *) (user_data_ptr);

  if(wmk_buf_ptr->allocated == FALSE)
  {
    DATA_IS707_MSG0( MSG_LEGACY_ERROR, 
                     "wmk_buf_ptr->allocated is FALSE ");
    return;
  }

  /* Obtain the route information from the wmk_buf_ptr to report event and 
   * print diagnostic messages.
   */
  route = (byte)((uint32)wmk_buf_ptr & 0x1);

  /* Reset the LSB to 0 which was used to store route
   */
  wmk_buf_ptr = (ds707_wmk_buf_type *) ((uint32)wmk_buf_ptr & ~0x1);

  if ( wmk_buf_ptr == NULL )
  {
    DATA_IS707_MSG0( MSG_LEGACY_ERROR, "wmk_buf_ptr is NULL");
    return;
  }
  wm_list = wmk_buf_ptr->um_SN_rev_wmk_list;

  if ( ds707_get_dsm_fc_flag() != TRUE )
  {
    DATA_IS707_MSG0( MSG_LEGACY_HIGH, 
                     "UM TX low watermark-processing ");
  }

  /*-------------------------------------------------------------------------
  Enable flow only if this PS Flow was disabled. This avoids unnecessary
  calls and message printing if the level fluctates around low watermark
  level. Since, this is data path, it is ok to optimize.

  If this is the pri_wmk_buf, flow control all the ps_flows associated 
  with it. 

  NOTE: Type casting for pointer comparison only.
  -------------------------------------------------------------------------*/
  if ( wmk_buf_ptr == (ds707_wmk_buf_type*)&ds707_wmk_pri_buf )
  {
    for ( curr_ps_flow_index = 0;
          curr_ps_flow_index < DS707_MAX_IFACES;
          curr_ps_flow_index ++ )
    {      
      ds707_wmki_lo_wm_enable_flow(
                          ds707_wmk_pri_buf.ps_flow_ptr[curr_ps_flow_index],
                          ds707_wmk_pri_buf.um_SN_rev_wmk_list,
                          route);
    }                                 
  }
  else
  {
    ds707_wmki_lo_wm_enable_flow(wmk_buf_ptr->ps_flow_ptr,
                                 wm_list,
                                 route);
  }
}/*ds707_wmki_process_um_tx_lo_wm*/


/*===========================================================================
FUNCTION      DS707_WMK_UM_TX_HI_WM_FUNC

DESCRIPTION   Called (via function pointer) when the tx watermark reg'd
              with RLP hits the high watermark event. This function posts
              a command to the DS task.

DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
LOCAL void ds707_wmki_um_tx_hi_wm_func
(
struct dsm_watermark_type_s *wm_ptr,
void *user_data_ptr
)
{
  ds707_wmk_buf_type *wmk_buf_ptr = NULL;
#ifndef FEATURE_DSM_QUBE
  ds_cmd_type        *ds_cmd_ptr;
#endif 
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  wmk_buf_ptr = (ds707_wmk_buf_type *)user_data_ptr;

  /*-------------------------------------------------------------------------
  Enable flow control for this PS Flow
  -------------------------------------------------------------------------*/
  if ((NULL == wmk_buf_ptr) || (NULL == wmk_buf_ptr->ps_flow_ptr) || 
      (NULL == wm_ptr))
  {
    DATA_IS707_MSG0(MSG_LEGACY_ERROR, 
                    "wmk_buf_ptr/ps_flow_ptr/wm_ptr is NULL");
    return;
  }
  if ( ds707_get_dsm_fc_flag() != TRUE )
  {
    DATA_IS707_MSG2( MSG_LEGACY_HIGH, 
                     "UM TX Hi watermark reached for WM 0x%x - curr_cnt:%d"
                     "post command to DS",
                     wm_ptr,
                     wm_ptr->current_cnt);
  }

#ifdef FEATURE_DSM_QUBE
  /*-------------------------------------------------------------------------
  This function is called as a callback when we reach the High watermark
  under qube mutex (dsmi_enqueue). We shall be able to call ps_disable_flow 
  which uses critical section. If FEATURE_DSM_QUBE is defined, DSM shall use
  mutex instead of INTER LOCK
  -------------------------------------------------------------------------*/
  ds707_wmki_process_um_tx_hi_wm( user_data_ptr );
#else
  /*-------------------------------------------------------------------------
  This function is called as a callback when we reach the High watermark
  under INTLOCK (dsmi_enqueue). We inturn call the ps_flow_disable which
  will result in an enter critical section. We cannot enter a critical
  section in INTLOCK context. So post a command to DS and handle it DS 
  context
  -------------------------------------------------------------------------*/
  if ( (ds_cmd_ptr = ds_allocate_cmd_buf(DS_CMD_PAYLOAD_SIZE_ZERO)) == NULL )
  {
    ASSERT(0);
    return;
  }
  else
  {
    /*-----------------------------------------------------------------------
     Populate the DS command buffer and post command to DS
    -----------------------------------------------------------------------*/
    (ds_cmd_ptr->hdr).cmd_id = DS_707_UM_TX_HI_WM_CMD;
    cmd_ptr->cmd_payload_ptr = user_data_ptr;
    ds_put_cmd( ds_cmd_ptr );
  }
#endif /* FEATURE_DSM_QUBE */
} /* ds707_wmki_um_tx_hi_wm_func() */

/*===========================================================================
FUNCTION      DS707_WMKI_HI_WM_DISABLE_FLOW

DESCRIPTION   Helper function called for primary v4 and v6 flow to disable
              flow when HI watermark is reached.

DEPENDENCIES

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
LOCAL void ds707_wmki_hi_wm_disable_flow(
  ps_flow_type                * ps_flow,
  struct dsm_watermark_type_s * wm_list,
  byte                          route
  )
{
  ds_wmk_flow_disabled_event_type disable_wmk;

  /*-------------------------------------------------------------------------
  Disabling flow only if this PS Flow was enabled. This avoids unnecessary
  calls and message printing if the level fluctates around high watermark
  level. Since, this is data path, it is ok to optimize.
  -------------------------------------------------------------------------*/

  if(ps_flow == NULL)
  {
    DATA_IS707_MSG0( MSG_LEGACY_HIGH, 
                     "ps_flow is NULL and the flow may have already been "
                     "cleaned up");
    return;
  }
  /* ------------------------------------------------------------------
     Double check current_cnt of the watermark before disable flow.
     The request for high watermark may be out of date because of
     the processing delay
     ------------------------------------------------------------------*/
  if ( wm_list[route].current_cnt < wm_list[route].lo_watermark )
  {
    DATA_IS707_MSG2(MSG_LEGACY_HIGH, 
                    "current_cnt %d is less than lo_watermark %d, ignore Hi WM", 
                    wm_list[route].current_cnt, 
                    wm_list[route].lo_watermark);
    return;    
  }

  /*-----------------------------------------------------------------------
    RSP: since new data always goes into the open route, the hiwater
    func will only be called when the open route is full, so always
    disable the flow.  Since watermark insertions occur in the same
    task as RSP route changes (PS), there is no thread safety issue
    here.
  -----------------------------------------------------------------------*/
  ps_flow_disable_tx_ex(ps_flow,DS_FLOW_PS_FLOW_TX_WM_MASK);

  /*---------------------------------------------------------------------
    Fill up the event payload.
  ----------------------------------------------------------------------*/
  disable_wmk.wmk_ptr = (void *)(&wm_list[route]);
  disable_wmk.ps_flow_ptr = (void *) ps_flow;
  disable_wmk.current_cnt = wm_list[route].current_cnt;
  event_report_payload (EVENT_DS_WMK_FLOW_DISABLED,
                        sizeof(ds_wmk_flow_disabled_event_type),
                        (void*)&disable_wmk);

} /* ds707_wmki_hi_wm_disable_flow() */

/*===========================================================================
FUNCTION      DS707_WMK_PROCESS_UM_TX_HI_WM

DESCRIPTION   This function is called when processing DS_707_UM_TX_HI_WM_CMD
              from the DS command buffer. This runs in the DS context.

DEPENDENCIES  Both the ds707_wmki_um_tx_lo_wm_func and ds707_wmki_um_tx_hi_wm_func
              should run in the same task context. Note that in the future
              ps_flow_disable_tx and ps_flow_enable_tx should also be called
              from the same task context (namely DS).

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
void ds707_wmk_process_um_tx_hi_wm( ds_cmd_type   *ds_cmd_ptr )
{
  struct dsm_watermark_type_s *wm_list;
  ds707_wmk_buf_type *wmk_buf_ptr = NULL;
  byte                route;
  uint8 curr_ps_flow_index;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  DATA_MDM_ARG_NOT_CONST(ds_cmd_ptr);

  if(ds_cmd_ptr == NULL)
  {
    DATA_IS707_MSG0(MSG_LEGACY_ERROR, "Command pointer is NULL");
    return;
  }

  wmk_buf_ptr = (ds707_wmk_buf_type *)(ds_cmd_ptr->cmd_payload_ptr);

  if ((NULL == wmk_buf_ptr) || (FALSE == wmk_buf_ptr->allocated))
  {
    DATA_IS707_MSG0(MSG_LEGACY_ERROR, 
           "wmk_buf_ptr pointer is NULL or wmk_buf_ptr->allocated is FALSE");
    return;
  }

  /* Obtain the route information from the wmk_buf_ptr to report event and 
   * print diagnostic messages.
   */
  route = (byte)((uint32)wmk_buf_ptr & 0x1);

  /* Reset the LSB to 0 which was used to store route
   */
  wmk_buf_ptr = (ds707_wmk_buf_type *) ((uint32)wmk_buf_ptr & ~0x1);

  if ( wmk_buf_ptr == NULL )
  {
    DATA_IS707_MSG1(MSG_LEGACY_ERROR, "wmk_buf_ptr not set! No Flow control "
                    "possible for WM buf ptr: 0x%x", wmk_buf_ptr);
    return;
  }

  wm_list = wmk_buf_ptr->um_SN_rev_wmk_list;

  DATA_IS707_MSG0( MSG_LEGACY_HIGH, 
                   "UM TX high watermark - processing UM_TX_HI_WM_CMD");

  /*-------------------------------------------------------------------------
  Disabling flow only if this PS Flow was enabled. This avoids unnecessary
  calls and message printing if the level fluctates around high watermark
  level. Since, this is data path, it is ok to optimize.

  If this is the pri_wmk_buf, flow control all the ps_flows associated 
  with it. 

  Type casting for pointer comparison only.
  -------------------------------------------------------------------------*/
  if ( wmk_buf_ptr == (ds707_wmk_buf_type*)&ds707_wmk_pri_buf )
  {
    for ( curr_ps_flow_index = 0;
          curr_ps_flow_index < DS707_MAX_IFACES;
          curr_ps_flow_index ++ )
    {      
      ds707_wmki_hi_wm_disable_flow(
                      ds707_wmk_pri_buf.ps_flow_ptr[curr_ps_flow_index],
                      ds707_wmk_pri_buf.um_SN_rev_wmk_list,
                      route);
    }                                 
  }
  else
  {
    ds707_wmki_hi_wm_disable_flow(wmk_buf_ptr->ps_flow_ptr,
                                 wm_list,
                                 route);
  }
} /* ds707_wmk_process_um_tx_hi_wm */
/*===========================================================================
FUNCTION      DS707_WMKI_PROCESS_UM_TX_HI_WM

DESCRIPTION   This function is called when High WM is hit. This function
              is called by dsmi_dequeue()

DEPENDENCIES  None. 

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
LOCAL void ds707_wmki_process_um_tx_hi_wm
( 
  void *user_data_ptr 
)
{
  struct dsm_watermark_type_s *wm_list;
  ds707_wmk_buf_type *wmk_buf_ptr = NULL;
  byte                route;
  uint8 curr_ps_flow_index;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if(user_data_ptr == NULL)
  {
    DATA_IS707_MSG0(MSG_LEGACY_ERROR, 
                    "Command pointer NULL in ds707_wmki_process_um_tx_hi_wm");
    return;
  }

  wmk_buf_ptr = (ds707_wmk_buf_type *) (user_data_ptr);

  if(wmk_buf_ptr->allocated == FALSE)
  {
    DATA_IS707_MSG0(MSG_LEGACY_ERROR, "wmk_buf_ptr->allocated is FALSE ");
    return;
  }

  /* Obtain the route information from the wmk_buf_ptr to report event and 
   * print diagnostic messages.
   */
  route = (byte)((uint32)wmk_buf_ptr & 0x1);

  /* Reset the LSB to 0 which was used to store route
   */
  wmk_buf_ptr = (ds707_wmk_buf_type *) ((uint32)wmk_buf_ptr & ~0x1);

  if ( wmk_buf_ptr == NULL )
  {
    DATA_IS707_MSG1( MSG_LEGACY_ERROR, "wmk_buf_ptr not set! No Flow control "
                     "possible for WM buf ptr: 0x%x",
                     wmk_buf_ptr);
    return;
  }

  wm_list = wmk_buf_ptr->um_SN_rev_wmk_list;

  if ( ds707_get_dsm_fc_flag() != TRUE )
  {
    DATA_IS707_MSG0( MSG_LEGACY_HIGH, 
                     "UM TX high watermark - processing");
  }

  /*-------------------------------------------------------------------------
  Disabling flow only if this PS Flow was enabled. This avoids unnecessary
  calls and message printing if the level fluctates around high watermark
  level. Since, this is data path, it is ok to optimize.

  If this is the pri_wmk_buf, flow control all the ps_flows associated 
  with it. 

  Type casting for pointer comparison only.
  -------------------------------------------------------------------------*/
  if ( wmk_buf_ptr == (ds707_wmk_buf_type*)&ds707_wmk_pri_buf )
  {
    for ( curr_ps_flow_index = 0;
          curr_ps_flow_index < DS707_MAX_IFACES;
          curr_ps_flow_index ++ )
    {      
      ds707_wmki_hi_wm_disable_flow(
                      ds707_wmk_pri_buf.ps_flow_ptr[curr_ps_flow_index],
                      ds707_wmk_pri_buf.um_SN_rev_wmk_list,
                      route);
    }                                 
  }
  else
  {
    ds707_wmki_hi_wm_disable_flow(wmk_buf_ptr->ps_flow_ptr,
                                 wm_list,
                                 route);
  }
} /* ds707_wmki_process_um_tx_hi_wm */

/*===========================================================================
FUNCTION      DS707_WMK_UM_RX_HI_WM_FUNC

DESCRIPTION   Called (via function pointer) when the rx watermark reg'd
              with RLP/SMD hits the high watermark event. This function posts
              a command to the DS task.

DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
LOCAL void ds707_wmki_um_rx_hi_wm_func
(
  struct dsm_watermark_type_s *wm_ptr,
  void *user_data_ptr
)
{
  ds_cmd_type             *ds_cmd_ptr;             /* DS Task command type */
#ifdef FEATURE_NPA
  npa_client_handle        impulse_cpu_hdl = NULL;
#endif /* FEATURE_NPA */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  DATA_MDM_ARG_NOT_CONST(user_data_ptr);

  DATA_IS707_MSG2( MSG_LEGACY_HIGH, 
                   "UM FWD Hi watermark reached for WM 0x%x - curr_cnt:%d"
                   "post command to DS",
                   wm_ptr,
                   wm_ptr->current_cnt);

#ifdef FEATURE_NPA
  /*-------------------------------------------------------------------------
    Let's ask for more MIPs
  -------------------------------------------------------------------------*/
  impulse_cpu_hdl = ds707_get_impulse_cpu_hdl();
  if( impulse_cpu_hdl != NULL )
  {
    npa_issue_impulse_request( impulse_cpu_hdl );
    DATA_IS707_MSG0( MSG_LEGACY_HIGH, "Data issued IMPULSE CPU request" );
  }
#endif /* FEATURE_NPA */

  if ( (ds_cmd_ptr = ds_allocate_cmd_buf(DS_CMD_PAYLOAD_SIZE_ZERO)) == NULL )
  {
    ASSERT(0);
    return;
  }
  else
  {
    /*-----------------------------------------------------------------------
     Populate the DS command buffer and post command to DS
    -----------------------------------------------------------------------*/
    (ds_cmd_ptr->hdr).cmd_id = DS_CMD_707_UM_RX_HI_WM;
    ds_put_cmd( ds_cmd_ptr );
  }
} /* ds707_wmki_um_rx_hi_wm_func() */

/*===========================================================================
FUNCTION      DS707_WMK_UM_RX_LO_WM_FUNC

DESCRIPTION   Called (via function pointer) when the rx watermark reg'd
              with RLP/SMD hits the low watermark event. This function posts
              a command to the DS task.

DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
LOCAL void ds707_wmki_um_rx_lo_wm_func
(
  struct dsm_watermark_type_s *wm_ptr,
  void *user_data_ptr
)
{
  ds_cmd_type             *ds_cmd_ptr;             /* DS Task command type */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  DATA_MDM_ARG_NOT_CONST(user_data_ptr);
  if (NULL !=  wm_ptr)
  {
    DATA_IS707_MSG2( MSG_LEGACY_HIGH, "UM FWD Lo watermark reached for WM 0x%x "
                     "- post command to DS curr_cnt:%d",
                     wm_ptr, wm_ptr->current_cnt);
  }

  if ( (ds_cmd_ptr = ds_allocate_cmd_buf(DS_CMD_PAYLOAD_SIZE_ZERO)) == NULL )
  {
    ASSERT(0);
    return;
  }
  else
  {
    /*-----------------------------------------------------------------------
     Populate the DS command buffer and post command to DS
    -----------------------------------------------------------------------*/
    (ds_cmd_ptr->hdr).cmd_id = DS_CMD_707_UM_RX_LO_WM;
    ds_put_cmd( ds_cmd_ptr );
  }
} /* ds707_wmki_um_rx_lo_wm_func() */

/*===========================================================================
                        EXTERNAL FUNCTIONS
===========================================================================*/

/*===========================================================================
FUNCTION      DS707_WMK_EVENT_CBACK

DESCRIPTION   Handles some events that require calling wmk.c functions.
              The events are also handled in ds707_pkt_mgr.c

DEPENDENCIES  Triggered by the following events: DS707_CBACK_PHYS_LINK_UP_EV
              and DS707_CBACK_ABORT_SESSION_EV

RETURN VALUE  None.

SIDE EFFECTS  Either resets the counters or flushes watermarks, depending on
              the events.
===========================================================================*/

void ds707_wmk_event_cback
(
  ds707_cback_event_type               event,
  const ds707_cback_event_info_type   *event_info_ptr
)
{

  switch (event)
  {
    case DS707_CBACK_PHYS_LINK_UP_EV:
      ds707_wmk_reset_RX_TX_byte_count();
      break;
    case DS707_CBACK_ABORT_SESSION_EV:
      DATA_IS707_MSG0( MSG_LEGACY_HIGH, "Flushing primary watermarks");
      ds707_wmk_cleanup_pri_wmk();
#ifdef FEATURE_HDR_QOS
      DATA_IS707_MSG0( MSG_LEGACY_HIGH, "Flushing secondary watermarks");
      ds707_wmk_cleanup_sec_wmks();
#endif /* FEATURE_HDR_QOS */
      break;
    default:
      DATA_IS707_MSG1( MSG_LEGACY_MED, 
                       "ds707_wmk_event_cback: ignoring event %d", 
                       event );
      break;
  }

}

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
)
{
  int p;
  uint8 curr_ps_flow_index;

  /* Initialize the TX and RX byte count to 0 */
  ds707_wmk_byte_count.TX_byte_count = 0;
  ds707_wmk_byte_count.RX_byte_count = 0;
  ds707_wmk_byte_count.uPrev_call_TX_byte_count=0;
  ds707_wmk_byte_count.uPrev_call_RX_byte_count=0;
  if (ds707_cback_register( DS707_CBACK_PHYS_LINK_UP_EV, 
                            ds707_wmk_event_cback) == FALSE)
  {
    ERR_FATAL("Could not reg PHYS_LINK_UP_EV cback",0,0,0);
  }

  if (ds707_cback_register( DS707_CBACK_ABORT_SESSION_EV, 
                            ds707_wmk_event_cback) == FALSE)
  {
    ERR_FATAL("Could not reg ABORT SESSION cback",0,0,0);
  }

  /*-------------------------------------------------------------------------
  Initialize the data structure for the primary watermark. This includes
  initializing the primary watermark queue and setting the watermark to use
  this queue
  -------------------------------------------------------------------------*/
  for (p = 0; p < DSRSP_NUM_ROUTES; ++p)
  {
  /*-------------------------------------------------------------------------
    Initialize the queue for forward link received data from RLP
  -------------------------------------------------------------------------*/
    dsm_queue_init (&ds707_wmk_um_SN_fwd_wm_list[p],
                    0x7FFFFFFF,  /* No MAX queue size */
                    &ds707_wmk_um_SN_fwd_q_list[p]);

    dsm_queue_init ( &ds707_wmk_pri_buf.um_SN_rev_wmk_list[p],
                     0x7FFFFFFF,
                     &ds707_wmk_pri_buf.um_SN_rev_q_list[p] );
  }
  ds707_wmk_pri_buf.allocated = FALSE;
  for ( curr_ps_flow_index = 0;
        curr_ps_flow_index < DS707_MAX_IFACES;
        curr_ps_flow_index ++ )
  {      
    ds707_wmk_pri_buf.ps_flow_ptr[curr_ps_flow_index] = NULL;
  }  

  /* Allocate the secondary array to hold  secondary watermark buffers */
  ds707_wmk_sec_buf = 
          modem_mem_alloc((sizeof(ds707_wmk_buf_type*) * DS707_NUM_REV_PS_FLOWS), 
                          MODEM_MEM_CLIENT_DATA);
  if(ds707_wmk_sec_buf == NULL)
  {
    ASSERT(0);
    return;
  }
  memset((void*)ds707_wmk_sec_buf, 0, 
         (sizeof(ds707_wmk_buf_type*) * DS707_NUM_REV_PS_FLOWS));

} /* ds_wmk_init() */

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
)
{
  uint8 i;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  *free_wmk_count = 0;

  /*-------------------------------------------------------------------------
  Search through the secondary pool for unallocated watermarks.
  -------------------------------------------------------------------------*/
  for ( i = 0; i < DS707_NUM_REV_PS_FLOWS; i++ )
  {
    if ( ds707_wmk_sec_buf[i] == NULL)
    {
      *free_wmk_count = *free_wmk_count + 1;
    }
  }
  return;
}/* ds707_wmk_free_sec_wm_count() */

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
)
{

  ds707_wmk_buf_type *wmk_buf_ptr = NULL;
  int i;
  ds_wmk_allocated_event_type   allocated_wmk;
  int p;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (ps_flow_ptr == NULL)
  {
    return NULL;
  }

  /*-------------------------------------------------------------------------
  Search through the secondary pool for unallocated watermarks.
  -------------------------------------------------------------------------*/
  for ( i = 0; i < DS707_NUM_REV_PS_FLOWS; i++ )
  {
    if ( ds707_wmk_sec_buf[i]!= NULL)
    {
      continue;
    }
    ds707_wmk_sec_buf[i] = modem_mem_alloc(sizeof(ds707_wmk_buf_type), 
                                           MODEM_MEM_CLIENT_DATA);
    if(ds707_wmk_sec_buf[i] == NULL)
    {
      ASSERT(0);
    }
    ds707_wmk_sec_buf[i]->allocated = FALSE;
    ds707_wmk_sec_buf[i]->ps_flow_ptr = NULL;

    wmk_buf_ptr = ds707_wmk_sec_buf[i];
    for (p = 0; p < DSRSP_NUM_ROUTES; ++p)
    {
      dsm_queue_init ( &ds707_wmk_sec_buf[i]->um_SN_rev_wmk_list[p],
                       0x7FFFFFFF,
                       &ds707_wmk_sec_buf[i]->um_SN_rev_q_list[p] );
      /*-----------------------------------------------------------------------
            Intialize PPP -> RLP SN/1X secondary watermarks
      -----------------------------------------------------------------------*/
      ds707_wmk_sec_buf[i]->um_SN_rev_wmk_list[p].lowater_func_data = 
                                                                (void *)NULL;
      ds707_wmk_sec_buf[i]->um_SN_rev_wmk_list[p].lowater_func_ptr   = 
                                                 ds707_wmki_um_tx_lo_wm_func;

      ds707_wmk_sec_buf[i]->um_SN_rev_wmk_list[p].hiwater_func_data = 
                                                                (void *)NULL;
      ds707_wmk_sec_buf[i]->um_SN_rev_wmk_list[p].hiwater_func_ptr   = 
                                                 ds707_wmki_um_tx_hi_wm_func;
      ds707_wmk_sec_buf[i]->um_SN_rev_wmk_list[p].non_empty_func_ptr   = NULL;
      ds707_wmk_sec_buf[i]->um_SN_rev_wmk_list[p].gone_empty_func_ptr  = NULL;
      ds707_wmk_sec_buf[i]->um_SN_rev_wmk_list[p].each_enqueue_func_ptr= NULL;
      ds707_wmk_sec_buf[i]->um_SN_rev_wmk_list[p].lo_watermark  = 
                                         ds707_wmk_value.ds707_wmk_sec_lo_wmk;
      ds707_wmk_sec_buf[i]->um_SN_rev_wmk_list[p].hi_watermark  = 
                                         ds707_wmk_value.ds707_wmk_sec_hi_wmk;
      ds707_wmk_sec_buf[i]->um_SN_rev_wmk_list[p].dont_exceed_cnt = 
                                ds707_wmk_value.ds707_wmk_sec_dont_exceed_cnt;    

      /* Route p is stored in LSB of lowater_func_data and 
       * hiwater_func_data so that route information is available when 
       * the call back is called.
       */
      wmk_buf_ptr->um_SN_rev_wmk_list[p].lowater_func_data =
      (void*)((uint32)wmk_buf_ptr | (uint32)p);
      wmk_buf_ptr->um_SN_rev_wmk_list[p].hiwater_func_data =
      (void*)((uint32)wmk_buf_ptr | (uint32)p);
      /*-------------------------------------------------------------------
            Fill up the event payload
      -------------------------------------------------------------------*/
      allocated_wmk.hi_watermark = ds707_wmk_value.ds707_wmk_sec_hi_wmk;
      allocated_wmk.lo_watermark = ds707_wmk_value.ds707_wmk_sec_lo_wmk;
      allocated_wmk.ps_flow_ptr = (void *) ps_flow_ptr;
      allocated_wmk.wmk_ptr = 
                            (void *) (wmk_buf_ptr->um_SN_rev_wmk_list + p);
      event_report_payload (EVENT_DS_WMK_ALLOCATED,
                            sizeof(ds_wmk_allocated_event_type),
                            (void*)&allocated_wmk);
    }
    wmk_buf_ptr->ps_flow_ptr = ps_flow_ptr;
    wmk_buf_ptr->allocated = TRUE;
    DATA_IS707_MSG2(MSG_LEGACY_LOW, 
                    "Secondary watermark WM:0x%x allocated for PS flow: 0x%x",
                    wmk_buf_ptr->um_SN_rev_wmk_list,ps_flow_ptr);
    break;
  }

  /*-------------------------------------------------------------------------
  Did not find a free buffer
  -------------------------------------------------------------------------*/
  if (wmk_buf_ptr==NULL)
  {
    DATA_IS707_MSG0(MSG_LEGACY_ERROR, "No free sec wmk to allocate");
    return NULL;
  }

  return(wmk_buf_ptr->um_SN_rev_wmk_list);
} /* ds707_wmk_alloc_sec_wm_list() */

/*===========================================================================
FUNCTION      DS707_WMK_FREE_SEC_WM_LIST

DESCRIPTION   Deallocates a secondary watermark.

DEPENDENCIES

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
void ds707_wmk_free_sec_wm_list( dsm_watermark_type wm_list[] )
{
  int i;
  ds707_wmk_buf_type *wm_buf_ptr = NULL;
  ds_wmk_deallocated_event_type deallocated_wmk;
  int p;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (wm_list == NULL)
  {
    return;
  }

  /*-------------------------------------------------------------------------
  Find the buffer containing the particular watermark to add to the free list
  If it's not found, then it is not part of the watermarks maintained by this
  module and so cannot be managed by this module.
  -------------------------------------------------------------------------*/
  for ( i = 0; i < DS707_NUM_REV_PS_FLOWS; i++ )
  {
    if ( ds707_wmk_sec_buf[i] && 
         wm_list == ds707_wmk_sec_buf[i]->um_SN_rev_wmk_list )
    {
      wm_buf_ptr = ds707_wmk_sec_buf[i];
      for (p = 0; p < DSRSP_NUM_ROUTES; ++p)
      {
        /*-------------------------------------------------------------------
        Fill up the event payload
        -------------------------------------------------------------------*/
        deallocated_wmk.ps_flow_ptr = 
                                 (void *) (ds707_wmk_sec_buf[i]->ps_flow_ptr);
        deallocated_wmk.wmk_ptr = 
                                 (void *) (wm_buf_ptr->um_SN_rev_wmk_list+p);
        event_report_payload (EVENT_DS_WMK_DEALLOCATED,
                              sizeof(ds_wmk_deallocated_event_type),
                              (void*)&deallocated_wmk);
        dsm_queue_destroy(&wm_buf_ptr->um_SN_rev_wmk_list[p]);
      }
      ds707_wmk_cleanup_single_wmk( wm_list);
      DATA_IS707_MSG2(MSG_LEGACY_HIGH, 
                   "Secondary watermark WM:0x%x deallocated for PS flow: 0x%x",
                   wm_list,wm_buf_ptr->ps_flow_ptr);
      modem_mem_free(ds707_wmk_sec_buf[i], MODEM_MEM_CLIENT_DATA);
      ds707_wmk_sec_buf[i] = NULL;
      break;
    }
  }

  /*-------------------------------------------------------------------------
  Watermark not found, so it is not part of the watermarks maintained by this
  module and so cannot be managed by this module.
  -------------------------------------------------------------------------*/
  if (wm_buf_ptr == NULL)
  {
    DATA_IS707_MSG0(MSG_LEGACY_ERROR, "Did not find wm entry in wmk buffer "
                    "pool. Not adding to free Q");
    return;
  }

} /* ds707_wmk_alloc_sec_wm_list() */

/*===========================================================================
FUNCTION      DS707_WMK_GET_PRI_WM_LIST

DESCRIPTION   Returns a pointer to the primary watermark

DEPENDENCIES

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
dsm_watermark_type * ds707_wmk_get_pri_wm_list( void )
{
  int               p;
  uint8             curr_ps_flow_index;
  ps_iface_type*    iface_ptr;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  ds707_wmk_pri_buf.allocated = TRUE;

  /* Primary wmk buf ptr holds a pointer to only default IPv4 flow
     It does not hold a pointer to default IPv6 flow. Hence during throttling
     there are checks to make sure when primary v4 flow is throttled, primary
     v6 flow is also throttled.

     And when low watermark is processed, when we enable primary v4 flow,
     we also enable primary v6 flow.
  */
  /*lint -e506 constan boolean value check required in case ps_iface for ipv4
    is null */
  for ( curr_ps_flow_index = 0;
        curr_ps_flow_index < DS707_MAX_IFACES;
        curr_ps_flow_index ++ )
  {      
    /* Get the default ps_flow for each PDN Instance */
	iface_ptr = ds707_pdncntx_get_iface_ptr_from_iface_tbl_index( 
                                                       curr_ps_flow_index);
    ds707_wmk_pri_buf.ps_flow_ptr[curr_ps_flow_index] =
      PS_IFACE_GET_DEFAULT_FLOW( iface_ptr );
  }  
  /*lint -restore*/
  for (p = 0; p < DSRSP_NUM_ROUTES; ++p)
  {
    ds707_wmk_pri_buf.um_SN_rev_wmk_list[p].lowater_func_data = 
                                                          &ds707_wmk_pri_buf;
    ds707_wmk_pri_buf.um_SN_rev_wmk_list[p].hiwater_func_data =  
                                                          &ds707_wmk_pri_buf;
  }
  return(ds707_wmk_pri_buf.um_SN_rev_wmk_list);
} /* ds707_wmk_get_pri_wm_list() */

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
void ds707_wmk_setup_wmks(void)
{
  int p;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  ds707_wmk_update_wmk_value();

  /*-----------------------------------------------------------------------
  Intialize PPP -> RLP SN/1X for primary watermark
  -----------------------------------------------------------------------*/
  for (p = 0; p < DSRSP_NUM_ROUTES; ++p)
  {
    /*lint -save -e64*/
    ds707_wmk_pri_buf.um_SN_rev_wmk_list[p].lowater_func_data = (void *)NULL;
                           /* Set the func data to point to default value */
    ds707_wmk_pri_buf.um_SN_rev_wmk_list[p].lowater_func_ptr   = 
                                                 ds707_wmki_um_tx_lo_wm_func;

    ds707_wmk_pri_buf.um_SN_rev_wmk_list[p].hiwater_func_data = (void *)NULL;
                            /* Set the func data to point to default value */
    ds707_wmk_pri_buf.um_SN_rev_wmk_list[p].hiwater_func_ptr   = 
                                                ds707_wmki_um_tx_hi_wm_func;
    /*lint -restore */
    ds707_wmk_pri_buf.um_SN_rev_wmk_list[p].non_empty_func_ptr    = NULL;
    ds707_wmk_pri_buf.um_SN_rev_wmk_list[p].gone_empty_func_ptr   = 
                                          ds707_wmki_um_tx_gone_empty_func;
    ds707_wmk_pri_buf.um_SN_rev_wmk_list[p].each_enqueue_func_ptr  = NULL;

    /* Setup the hi and lo levels for FWD Link watermarks */
    ds707_wmk_um_SN_fwd_wm_list[p].hiwater_func_data = (void *)NULL;
    ds707_wmk_um_SN_fwd_wm_list[p].hiwater_func_ptr = 
                                                ds707_wmki_um_rx_hi_wm_func;

    ds707_wmk_um_SN_fwd_wm_list[p].lowater_func_data = (void *)NULL;
    ds707_wmk_um_SN_fwd_wm_list[p].lowater_func_ptr = 
                                                ds707_wmki_um_rx_lo_wm_func;
  }
  
} /* ds707_wmk_setup_wmks() */

/*===========================================================================
FUNCTION      DS707_WMK_CLEANUP_PRI_WMK

DESCRIPTION   This function cleans up the watermarks that are owned by
              data services.

DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
void ds707_wmk_cleanup_pri_wmk(void)
{

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /*-------------------------------------------------------------------------
    Empty SN watermarks (Rev Link)
  -------------------------------------------------------------------------*/
  ds707_wmk_update_tx_count(ds707_wmk_pri_buf.um_SN_rev_wmk_list);
  ds707_wmk_cleanup_single_wmk(ds707_wmk_pri_buf.um_SN_rev_wmk_list);
} /* ds707_wmk_cleanup_pri_wmk() */

/*===========================================================================
FUNCTION      DS707_WMK_CLEANUP_SEC_WMKS

DESCRIPTION   This function cleans up the watermarks that are owned by
              data services.

DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
void ds707_wmk_cleanup_sec_wmks(void)
{
  int i;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /*-------------------------------------------------------------------------
    Empty SN watermarks (Rev Link)
  -------------------------------------------------------------------------*/
  for (i=0; i < DS707_NUM_REV_PS_FLOWS; i++)
  {
    if (ds707_wmk_sec_buf[i] &&
        ds707_wmk_sec_buf[i]->allocated == TRUE)
    {
      /* update tx byte count before cleaning up */
      ds707_wmk_update_tx_count(ds707_wmk_sec_buf[i]->um_SN_rev_wmk_list);
      ds707_wmk_cleanup_single_wmk(ds707_wmk_sec_buf[i]->um_SN_rev_wmk_list);
    }
  }
} /* ds707_wmk_cleanup_sec_wmks() */


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
)
{
  ds_wmk_flushed_event_type flushed_wmk;
  int p;

  if (wmk_list == NULL)
  {
    return;
  }
  for (p = 0; p < DSRSP_NUM_ROUTES; ++p)
  {
    /*-------------------------------------------------------------------
    Fill up the event payload
    --------------------------------------------------------------------*/
    flushed_wmk.wmk_ptr = (void *) (wmk_list + p);
    flushed_wmk.current_cnt = wmk_list[p].current_cnt;
    event_report_payload (EVENT_DS_WMK_FLUSHED,
                          sizeof(ds_wmk_flushed_event_type),
                          (void*)&flushed_wmk);

    /*-----------------------------------------------------------------------
    Empty SN and AN watermarks (Fwd Link)
    -----------------------------------------------------------------------*/
    if (wmk_list[p].current_cnt > 0)
    {
      DATA_IS707_MSG2( MSG_LEGACY_HIGH, 
                       "Empty %d bytes from WM:0x%x", 
                       wmk_list[p].current_cnt, &wmk_list[p]);
      /*-------------------------------------------------------------------
      Empty the dsm queue
      --------------------------------------------------------------------*/
      dsm_empty_queue(&wmk_list[p]);
    }
    /* Reset the total receive count */
    wmk_list[p].total_rcvd_cnt = 0;
  }
} /* ds707_wmk_cleanup_wmks() */

/*===========================================================================
FUNCTION      DS707_WMK_CLEANUP_FWD_QUEUE

DESCRIPTION   This function cleans up the forward queue

DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
void ds707_wmk_cleanup_fwd_queue(void)
{
  dsm_item_type             *item_ptr;
  int p;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
  Empty Fwd queue
  -------------------------------------------------------------------------*/
  /*lint -save -e792
   *            did not like void* cast to dsm_item_type*
   */
  for (p = 0; p < DSRSP_NUM_ROUTES; ++p)
  {
    while ((item_ptr = 
              (dsm_item_type *)q_get(&ds707_wmk_um_SN_fwd_q_list[p])) != 
                                                                        NULL)
    {
      (void)dsm_free_packet(&item_ptr);
    }
  }

  /*lint +e792 */
} /* ds707_wmk_cleanup_fwd_queue() */

/*===========================================================================
FUNCTION      DS707_WMK_PROCESS_UM_RX_HI_WM

DESCRIPTION   This function is called when processing DS_707_UM_RX_HI_WM_CMD
              from the DS command buffer. This runs in the DS context.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/

void ds707_wmk_process_um_rx_hi_wm( ds_cmd_type   *ds_cmd_ptr )
{
#ifdef TEST_FRAMEWORK
  #error code not present
#endif /* TEST_FRAMEWORK */

  DATA_MDM_ARG_NOT_CONST(ds_cmd_ptr);

#if   defined(FEATURE_DATA_FL_FLOW_CONTROL)
  /*---------------------------------------------------------------------
    Send DOWN command to CCM
  ---------------------------------------------------------------------*/
  DATA_IS707_MSG0(MSG_LEGACY_MED, 
             "UM FWD Hi watermark reached - post command DS707_CCM_FLOW_DOWN");
  ds707_ccm_mem_send_flow_control_cmd(DS707_CCM_MEM_WMK_UM_FWD, DS707_CCM_FLOW_DOWN);

#elif defined(FEATURE_HDR)
  DATA_IS707_MSG0(MSG_LEGACY_MED, "UM FWD enable MAC flow control");
  hdrl1_fmac_enable_flow_control ((uint32)HDRFMAC_DRC_REQ_PSQ);
#endif
  }

/*===========================================================================
FUNCTION      DS707_WMK_PROCESS_UM_RX_LO_WM

DESCRIPTION   This function is called when processing DS_707_UM_RX_LO_WM_CMD
              from the DS command buffer. This runs in the DS context.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/

void ds707_wmk_process_um_rx_lo_wm( ds_cmd_type   *ds_cmd_ptr )
{
#ifdef TEST_FRAMEWORK
  #error code not present
#endif /* TEST_FRAMEWORK */

  DATA_MDM_ARG_NOT_CONST(ds_cmd_ptr);
#if   defined(FEATURE_DATA_FL_FLOW_CONTROL)
  /*---------------------------------------------------------------------
    Send OFF command to CCM
  ---------------------------------------------------------------------*/
  DATA_IS707_MSG0(MSG_LEGACY_MED, 
              "UM FWD Lo watermark reached - post command DS707_CCM_FLOW_OFF");
  ds707_ccm_mem_send_flow_control_cmd(DS707_CCM_MEM_WMK_UM_FWD, DS707_CCM_FLOW_OFF);

#elif defined(FEATURE_HDR)
  DATA_IS707_MSG0(MSG_LEGACY_MED, "UM FWD disable MAC flow control");
  hdrl1_fmac_disable_flow_control ((uint32)HDRFMAC_DRC_REQ_PSQ);
#endif
}

/*===========================================================================
FUNCTION      DS707_WMK_RESET_RX_TX_BYTE_COUNT

DESCRIPTION   This function resets RX and TX byte count for a PS call.

DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
void ds707_wmk_reset_RX_TX_byte_count(void)
{
  ds707_wmk_byte_count.TX_byte_count = 0;
  ds707_wmk_byte_count.RX_byte_count = 0;
} /* ds707_wmk_reset_RX_TX_byte_count */

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
)  
{
  *txed_bytes = ds707_wmk_byte_count.uPrev_call_TX_byte_count;
  *rxed_bytes = ds707_wmk_byte_count.uPrev_call_RX_byte_count;
}

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
)
{
  uint32 total_tx_bytes = 0;
  uint32 total_rx_bytes = 0;
  int i, j;

  DATA_MDM_ARG_NOT_CONST(tx_wmk_list);
  DATA_MDM_ARG_NOT_CONST(rx_wmk_list);

  /* Make sure input pointers are valid */
  if ((txed_bytes == NULL) || (rxed_bytes == NULL) 
      || (tx_wmk_list == NULL) || (rx_wmk_list == NULL)
     )
  {
    DATA_IS707_MSG0( MSG_LEGACY_HIGH, 
                     "Invalid input pointer in ds707_wmk_get_byte_counts");
    return;
  }
  /* Total TX byte is the sum of current accumulated value and tx'd counts in
     all TX watermarks */
  for (i = 0; i < DSRSP_NUM_ROUTES; ++i)
  {
    total_tx_bytes += (uint32)tx_wmk_list[i].total_rcvd_cnt;
  }
  
  /* Check if secondary flows are enabled, if so update the count */
  for (j=0; j < DS707_NUM_REV_PS_FLOWS; j++)
  {
    if (ds707_wmk_sec_buf[j] &&
        ds707_wmk_sec_buf[j]->allocated == TRUE)
    {
      for (i=0; i < DSRSP_NUM_ROUTES; i++)
      {
        total_tx_bytes += 
                   (uint32)ds707_wmk_sec_buf[j]->um_SN_rev_wmk_list[i].total_rcvd_cnt;
      }
    }
  }
  total_tx_bytes += ds707_wmk_byte_count.TX_byte_count;

  /* Total RX byte is the sum of current accumulated value and rx'd counts in
     all RX watermarks */
  for (i = 0; i < DSRSP_NUM_ROUTES; ++i)
  {
    total_rx_bytes += (uint32)rx_wmk_list[i].total_rcvd_cnt;
  }
  total_rx_bytes += ds707_wmk_byte_count.RX_byte_count;

  *txed_bytes = total_tx_bytes; 
  *rxed_bytes = total_rx_bytes;
}

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
)
{
    int i;
/*--------------------------------------------------------------*/
    DATA_MDM_ARG_NOT_CONST(wmk_list);

    for (i = 0; i < DSRSP_NUM_ROUTES; ++i)
    {
      ds707_wmk_byte_count.RX_byte_count += (uint32)wmk_list[i].total_rcvd_cnt;
    }

    ds707_wmk_byte_count.uPrev_call_RX_byte_count=ds707_wmk_byte_count.RX_byte_count;
}

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
)
{
  int i;

  DATA_MDM_ARG_NOT_CONST(wmk_list);

  for (i = 0; i < DSRSP_NUM_ROUTES; ++i)
  {
    ds707_wmk_byte_count.TX_byte_count += (uint32)wmk_list[i].total_rcvd_cnt;
  }

  ds707_wmk_byte_count.uPrev_call_TX_byte_count=ds707_wmk_byte_count.TX_byte_count;
}


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
)
{
  int i;
  int p;
  
#ifdef FEATURE_DATA_WMK_CONFIG
  int ret_val;
  /* Structure to hold the values read from NV item*/
  ds707_wmk_lvl_nv_struct wmk_nv_data;
  memset( &wmk_nv_data, 0, sizeof(wmk_nv_data) );
#endif /*FEATURE_DATA_WMK_CONFIG*/

#ifdef FEATURE_HDR_REVB   
  if (hdrcp_stream_stream_is_assigned(HDRHAI_MULTILINK_MULTIFLOW_PACKET_APP_SN))
  { 
    DATA_IS707_MSG0( MSG_LEGACY_MED, "RevB WaterMark used" );
    /* REVB protocol */
    ds707_wmk_value.ds707_wmk_fwd_lo_wmk          = DS707_WMK_REVB_FWD_LNK_LO_WATERMARK;
    ds707_wmk_value.ds707_wmk_fwd_hi_wmk          = DS707_WMK_REVB_FWD_LNK_HI_WATERMARK;
    ds707_wmk_value.ds707_wmk_pri_lo_wmk          = DS707_WMK_REVB_PRI_LO_WATERMARK;
    ds707_wmk_value.ds707_wmk_pri_hi_wmk          = DS707_WMK_REVB_PRI_HI_WATERMARK;
    ds707_wmk_value.ds707_wmk_pri_dont_exceed_cnt = DS707_WMK_REVB_PRI_DONT_EXCEED_CNT;
    ds707_wmk_value.ds707_wmk_sec_lo_wmk          = DS707_WMK_REVB_SEC_LO_WATERMARK;
    ds707_wmk_value.ds707_wmk_sec_hi_wmk          = DS707_WMK_REVB_SEC_HI_WATERMARK;
    ds707_wmk_value.ds707_wmk_sec_dont_exceed_cnt = DS707_WMK_REVB_SEC_DONT_EXCEED_CNT;
  }
  else
  {
#endif
    DATA_IS707_MSG0( MSG_LEGACY_MED, "RevA WaterMark used");
    /* REVA/REL0 protocol */
    ds707_wmk_value.ds707_wmk_fwd_lo_wmk          = DS707_WMK_REVA_FWD_LNK_LO_WATERMARK;
    ds707_wmk_value.ds707_wmk_fwd_hi_wmk          = DS707_WMK_REVA_FWD_LNK_HI_WATERMARK;
    ds707_wmk_value.ds707_wmk_pri_lo_wmk          = DS707_WMK_REVA_PRI_LO_WATERMARK;
    ds707_wmk_value.ds707_wmk_pri_hi_wmk          = DS707_WMK_REVA_PRI_HI_WATERMARK;
    ds707_wmk_value.ds707_wmk_pri_dont_exceed_cnt = DS707_WMK_REVA_PRI_DONT_EXCEED_CNT;
    ds707_wmk_value.ds707_wmk_sec_lo_wmk          = DS707_WMK_REVA_SEC_LO_WATERMARK;
    ds707_wmk_value.ds707_wmk_sec_hi_wmk          = DS707_WMK_REVA_SEC_HI_WATERMARK;
    ds707_wmk_value.ds707_wmk_sec_dont_exceed_cnt = DS707_WMK_REVA_SEC_DONT_EXCEED_CNT;
#ifdef FEATURE_HDR_REVB
  }
#endif
   
#ifdef FEATURE_DATA_WMK_CONFIG
  /* Use wmk level values from NV if present*/
  ret_val = ds707_wmk_lvl_read_wmks_nv(  &wmk_nv_data, sizeof(wmk_nv_data) );
  if ( 0 == ret_val ) /*Nv item is set*/
  {
    /* Check if fwd wmk lvls are valid*/
    if( (wmk_nv_data.um_fwd_lo_watermark_level > 0) &&
         (wmk_nv_data.um_fwd_hi_watermark_level > wmk_nv_data.um_fwd_lo_watermark_level) )
    {
      DATA_IS707_MSG0( MSG_LEGACY_MED, "Using um fwd wmk values from NV" );
      ds707_wmk_value.ds707_wmk_fwd_lo_wmk = wmk_nv_data.um_fwd_lo_watermark_level;
      ds707_wmk_value.ds707_wmk_fwd_hi_wmk = wmk_nv_data.um_fwd_hi_watermark_level;
    }
    else
    {
      DATA_IS707_MSG0( MSG_LEGACY_HIGH, 
                       "um fwd wmk values from NV invalid, using defaults" );
    }
    /* Check if rev wmk lvls are valid*/
    if( (wmk_nv_data.um_pri_lo_watermark_level  > 0) &&
        (wmk_nv_data.um_pri_hi_watermark_level  > wmk_nv_data.um_pri_lo_watermark_level) &&
        (wmk_nv_data.um_pri_dne_watermark_level > wmk_nv_data.um_pri_hi_watermark_level) )
    {
      DATA_IS707_MSG0( MSG_LEGACY_MED, "Using um rev wmk values from NV" );
      ds707_wmk_value.ds707_wmk_pri_lo_wmk          = wmk_nv_data.um_pri_lo_watermark_level;
      ds707_wmk_value.ds707_wmk_pri_hi_wmk          = wmk_nv_data.um_pri_hi_watermark_level;
      ds707_wmk_value.ds707_wmk_pri_dont_exceed_cnt = wmk_nv_data.um_pri_dne_watermark_level;
    }
    else
    {
      DATA_IS707_MSG0( MSG_LEGACY_HIGH, "um rev wmk values from NV are invalid, using defaults" );
    }

  }
  else
  {
    DATA_IS707_MSG0( MSG_LEGACY_HIGH, 
                     "UM Wmk values not set in NV, using defaults" );
  }
#endif /*FEATURE_DATA_WMK_CONFIG*/ 
  
  DATA_IS707_MSG5(MSG_LEGACY_HIGH, "um fwd lo wmk lvl: %d, hi wmk lvl: %d. "
                  "um pri lo wmk lvl: %d, hi wmk lvl: %d, dne wmk lvl: %d", 
                  ds707_wmk_value.ds707_wmk_fwd_lo_wmk, 
                  ds707_wmk_value.ds707_wmk_fwd_hi_wmk,
                  ds707_wmk_value.ds707_wmk_pri_lo_wmk,
                  ds707_wmk_value.ds707_wmk_pri_hi_wmk,
                  ds707_wmk_value.ds707_wmk_pri_dont_exceed_cnt);
   
  for (p = 0; p < DSRSP_NUM_ROUTES; ++p)
  {
    /*-----------------------------------------------------------------------
      Setup the watermark levels for primary watermark PPP -> RLP SN/1X
    -----------------------------------------------------------------------*/
    ds707_wmk_pri_buf.um_SN_rev_wmk_list[p].lo_watermark       = ds707_wmk_value.ds707_wmk_pri_lo_wmk;
    ds707_wmk_pri_buf.um_SN_rev_wmk_list[p].hi_watermark       = ds707_wmk_value.ds707_wmk_pri_hi_wmk;
    ds707_wmk_pri_buf.um_SN_rev_wmk_list[p].dont_exceed_cnt    = ds707_wmk_value.ds707_wmk_pri_dont_exceed_cnt;
    /*-----------------------------------------------------------------------
      Setup the watermark levels for FWD Link watermarks RLP -> PPP
    -----------------------------------------------------------------------*/
    ds707_wmk_um_SN_fwd_wm_list[p].lo_watermark       = ds707_wmk_value.ds707_wmk_fwd_lo_wmk;
    ds707_wmk_um_SN_fwd_wm_list[p].hi_watermark       = ds707_wmk_value.ds707_wmk_fwd_hi_wmk;
    ds707_wmk_um_SN_fwd_wm_list[p].dont_exceed_cnt    = 0xFFFFFFFF;
  }

  /*-----------------------------------------------------------------------
    Setup the watermark levels for secondary watermark PPP -> RLP SN/1X
  -----------------------------------------------------------------------*/
  if(ds707_wmk_sec_buf == NULL)  //Check added for debugging. TBR before check in
  {
    DATA_IS707_MSG0(MSG_LEGACY_HIGH,"Secondary WMK not yet initialized hence ignore wmk update");
    return;
  }  
  for (i=0; i < DS707_NUM_REV_PS_FLOWS; i++)
  {
    if(ds707_wmk_sec_buf[i] == NULL)
    {
      continue;
    }
    for (p = 0; p < DSRSP_NUM_ROUTES; ++p)
    {
      ds707_wmk_sec_buf[i]->um_SN_rev_wmk_list[p].lo_watermark       = ds707_wmk_value.ds707_wmk_sec_lo_wmk;
      ds707_wmk_sec_buf[i]->um_SN_rev_wmk_list[p].hi_watermark       = ds707_wmk_value.ds707_wmk_sec_hi_wmk;
      ds707_wmk_sec_buf[i]->um_SN_rev_wmk_list[p].dont_exceed_cnt    = ds707_wmk_value.ds707_wmk_sec_dont_exceed_cnt;
    }
  }
}

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
)
{
  return ds707_wmk_um_SN_fwd_wm_list;
}

/*===========================================================================
FUNCTION DS707_WMK_GET_WMK_LEVEL

DESCRIPTION
  This function returns watermarks low and high levele value

PARAMETERS
  None

RETURN VALUE
   NONE

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
void ds707_wmk_get_wmk_levels
(
  ds707_wmk_type *wmk_type_ptr
)
{
  if ( wmk_type_ptr )
  {
    *wmk_type_ptr = ds707_wmk_value;
  }
}

/*===========================================================================
FUNCTION      DS707_WMK_SET_UM_TX_HIGH_WM_FUNC

DESCRIPTION   If DISABLE is FALSE,then it restores BP enablement. 
              If DISABLE is TRUE,then overwrite hi_wmk_level with DNE values and
              set high wm func to NULL.

DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
void ds707_wmk_set_um_tx_high_wm_func 
(
  boolean disable
)
{
  /*----------------------------------------------------------------*/
  if( disable == FALSE )
  {
    DATA_IS707_MSG0(MSG_LEGACY_HIGH," Restoring high wm function ");
    ds707_wmk_pri_buf.um_SN_rev_wmk_list[0].hiwater_func_ptr   =
                                    ds707_wmki_um_tx_hi_wm_func;
    ds707_wmk_pri_buf.um_SN_rev_wmk_list[0].hi_watermark =
                                    ds707_wmk_value.ds707_wmk_pri_hi_wmk;	  
  }
  else
  {
    DATA_IS707_MSG0(MSG_LEGACY_HIGH," Disabling high wm function ");
    ds707_wmk_pri_buf.um_SN_rev_wmk_list[0].hiwater_func_ptr = NULL;
    ds707_wmk_pri_buf.um_SN_rev_wmk_list[0].hi_watermark =
                      ds707_wmk_value.ds707_wmk_pri_dont_exceed_cnt;
  }
}

/*===========================================================================
FUNCTION      DS707_GET_CURRENT_TX_WMK_CNT

DESCRIPTION   This function returns the current total byte count 
              in UM TX watermark.

DEPENDENCIES  None

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
uint32 ds707_get_current_tx_wmk_cnt(void)
{

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
  Use route A only. Route B is not implemented on EMPA.
  -------------------------------------------------------------------------*/
  return ds707_wmk_pri_buf.um_SN_rev_wmk_list[0].current_cnt;
}/* ds707_get_current_tx_wmk_cnt() */

/*===========================================================================
FUNCTION      DS707_GET_TOTAL_TX_WMK_CNT

DESCRIPTION   This function returns the current total byte count 
              in UM TX watermark.

DEPENDENCIES  None

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
uint64 ds707_get_total_tx_wmk_cnt(void)
{

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  return ds707_wmk_pri_buf.um_SN_rev_wmk_list[0].total_rcvd_cnt;
}/* ds707_get_total_tx_wmk_cnt() */

/*===========================================================================
FUNCTION      DS707_GET_CURRENT_RX_WMK_CNT

DESCRIPTION   This function returns the current total byte count 
              in UM RX watermark.

DEPENDENCIES  None

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
uint32 ds707_get_current_rx_wmk_cnt(void)
{

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
  Use route A only. Route B is not implemented on EMPA.
  -------------------------------------------------------------------------*/
  return ds707_wmk_um_SN_fwd_wm_list[0].current_cnt;
}/* ds707_get_current_rx_wmk_cnt() */

/*===========================================================================
FUNCTION      DS707_DSM_FC_ENABLE

DESCRIPTION   This function is called to enable flow control when DSM items
              reach DSM_MEM_LEVEL_LINK_LAYER_FEW level.

DEPENDENCIES  None

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
void ds707_dsm_fc_enable(void)
{
  DATA_IS707_MSG0(MSG_LEGACY_HIGH,
                  "Enable flow control");
  ds707_wmki_um_tx_hi_wm_func(ds707_wmk_pri_buf.um_SN_rev_wmk_list,
                              (void *)&ds707_wmk_pri_buf);
}

/*===========================================================================
FUNCTION      DS707_DSM_FC_DISABLE

DESCRIPTION   This function is called to disable flow control when DSM items
              reach DSM_MEM_LEVEL_LINK_LAYER_MANY level.

DEPENDENCIES  None

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
void ds707_dsm_fc_disable(void)
{
  DATA_IS707_MSG0(MSG_LEGACY_HIGH,
                  "Disable flow control");
  ds707_wmki_um_tx_lo_wm_func(ds707_wmk_pri_buf.um_SN_rev_wmk_list,
                              (void *)&ds707_wmk_pri_buf);
}

#endif /* FEATURE_DATA_IS707 */
