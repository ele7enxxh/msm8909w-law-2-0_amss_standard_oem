/*===========================================================================

                          P S _ P P P _ S N P I P . C

DESCRIPTION

  This file provides implementation to support ppp device level snooping on
  all IP packets. The file contains the state machine implementation and
  actual snooping functions to snoop on all IP packets including - UDP,
  ICMP, non compresed TCP, VJUCH and VJCH.

EXTERNALIZED FUNCTIONS
  snoop_ip_init()
    Initialize the IP Snoop State Machine.
  snoop_ip_post_event()
    Post an event to IP Snoop State Machine.

Copyright (c) 2003-2012 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
===========================================================================*/


/*===========================================================================

                            EDIT HISTORY FOR FILE

  $PVCSPath: L:/src/asw/MM_DATA/vcs/ps_ppp_snpip.c_v   1.0   13 Feb 2003 14:21:22   ubabbar  $
  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/protocols/linklayer/src/ps_ppp_snpip.c#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $


when        who    what, where, why
--------    ---    ----------------------------------------------------------
10/25/12    fn     Remove usage of fatal error log messages
03/23/11    op     Data SU API cleanup
02/04/11    ss     Register processing callback for PPP_VJCH_PROTOCOL on 
                   transition to SNOOP_IP_NONCOMP_STATE.
08/31/09    mga    Merged from eHRPD branch
07/27/09    sp     Fixed IP Fragmentation issues.
05/06/09    ss     SLHC De-coupling effort.
09/24/08    scb    Replaced errno with ps_errno
09/12/08    pp     Metainfo optimizations.
07/01/08    dm     Fixed issue of packets getting dropped and long data
                   sessions observed on PLTS due to VJ TCP compression
04/27/07    scb    Removed LINT suppressions
03/27/07    scb    Added value check to prevent buffer overrun
03/16/07    scb    Fixed Klocwork High errors
11/03/06    msr    Caching meta info to optimize VJCH packet handling
06/23/06    msr    Fixed incorrect check to forward packets in
                   snpipi_process_vjch_cb()
12/15/05    msr    Using macros instead of directly looking in to fi_result of
                   meta info
10/18/05    msr    Conforming to change in IPFLTR_UPDATED_EV
04/20/05    sv     Modified IP filtering to use new filter pkt function.
04/17/05    msr    Changed fi_id of meta info to fi_result.
04/13/05    ifk    Replaced tracking of sockets with slots for determining
                   destination of VJ compressed packets
01/27/05    ssh    Changed include file name dsbyte.h to ps_byte.h
01/10/05    ifk    Added support for filtering fragments
10/14/04    ifk    Added meta info ** to snoop callbacks, when applying an
                   IP filter return its result in meta info.
10/13/04    vp     Replaced ntohip(), htonip() with ps_ip4_hdr_parse(),
                   ps_ip4_hdr_create(). Removal of byte order conversion of
                   ports as they are stored in network order.
06/11/04    vp     Removed inclusion of internet.h. Included ps_pkt_info.h.
                   Replaced use of protocol constants TCP_PTCL etc with
                   PS_IPPROTO_TCP etc
04/27/04    usb    Using named constant instead of 0 for return value.
03/19/04    mct    Renamed ip_filter_info_type to ip_pkt_info_type.
02/18/04    usb    IP filtering library interface change, client id support.}
11/11/03    aku/rc Passed additional parameter 'offset' in call to cksum().
10/08/03    usb    Misc fixes - incorrect ICMP header offset, bad assert,
                   removed redundant switch statement.
05/02/03    usb    created file
===========================================================================*/



/*===========================================================================

                          INCLUDE FILES FOR MODULE

===========================================================================*/

#include "comdef.h"
#include "customer.h"

#include "amssassert.h"
#include "err.h"
#include "msg.h"

#include "ps_ppp_snpip.h"
#include "ps_iface.h"
#include "ps_ifacei_utils.h"
#include "ps_svc.h"
#include "ps_ppp_fsm.h"
#include "ps_ppp_snoop.h"
#include "ps_slhc.h"
#include "ps_iputil.h"
#include "ps_byte.h"
#include "ps_ip4_hdr.h"
#include "ps_pppi.h"
#include "ps_pkt_info_utils.h"
#include "ps_ip_fraghdl.h"
#include "ds_Utils_DebugMsg.h"


/*= = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =

                            INTERNAL DEFINITIONS

= = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =*/
/*---------------------------------------------------------------------------
  Declaration of the states of the state machine.
---------------------------------------------------------------------------*/
typedef enum
{
  SNOOP_IP_MIN_STATE     = -1,
  SNOOP_IP_NULL_STATE    = 0,    /* Initial state, ppp dev down            */
  SNOOP_IP_NONE_STATE    = 1,    /* PPP dev up, IP filtering not enabled   */
  SNOOP_IP_NONCOMP_STATE = 2,    /* IP filtering enabled, no VJ compreesed */
  SNOOP_IP_ALL_STATE     = 3,    /* Filter all IP including VJ compressed  */
  SNOOP_IP_MAX_STATE
} snoop_ipi_state_type;

/*---------------------------------------------------------------------------
  The IP snoop state machine control block, one per PPP device
---------------------------------------------------------------------------*/
typedef struct
{
  ps_iface_type      * if_ptr;            /* rx iface for this PPP dev     */
  void               * ev_buf_ptr;        /* buffer for fltr_update ev cb  */
  uint32               tag_ip;            /* IP snoop callback tag         */
  uint32               tag_vjuch;         /* VJUCH snoop callback tag      */
  uint32               tag_vjch;          /* VJCH snoop callback tag       */
  uint16               n_local_slot;      /* num local slots doing VJCH    */
  uint8                c_slot;            /* Current VJ slot id for stack  */
  boolean              f_toss;            /* toss flag for local VJ slots  */

  boolean  l_slot_arr[SLHC_MAX_SLOTS]; /* is slot id for local sock */
  snoop_ipi_state_type state;             /* Current state                 */
} snoop_ipi_info_type;

static snoop_ipi_info_type snpipi_cb_arr[PPP_MAX_DEV];

/*---------------------------------------------------------------------------
  State machine is initialized or not?
---------------------------------------------------------------------------*/
static boolean inited = FALSE;

/*--------------------------------------------------------------------------
  Local declaration of new connection indicator bit of the SLHC module
--------------------------------------------------------------------------*/
#define NEW_C 0x40


/*===========================================================================

                            Forward Declarations

===========================================================================*/
/*---------------------------------------------------------------------------
  Callbacks registered with various modules
---------------------------------------------------------------------------*/
static snoop_cb_ret_val_enum_type snoop_ipi_process_ip_cb
(
  ppp_dev_enum_type    dev,
  uint16               protocol,
  dsm_item_type     ** item_head_ptr,
  ps_rx_meta_info_type ** meta_info_ref_ptr
);

static snoop_cb_ret_val_enum_type snoop_ipi_process_vjuch_cb
(
  ppp_dev_enum_type    dev,
  uint16               protocol,
  dsm_item_type     ** item_head_ptr,
  ps_rx_meta_info_type ** meta_info_ref_ptr
);

static snoop_cb_ret_val_enum_type snoop_ipi_process_vjch_cb
(
  ppp_dev_enum_type    dev,
  uint16               protocol,
  dsm_item_type     ** item_head_ptr,
  ps_rx_meta_info_type ** meta_info_ref_ptr
);

static void snoop_ipi_iface_ev_handler_cb
(
  ps_iface_type               * this_iface_ptr,
  ps_iface_event_enum_type      event,
  ps_iface_event_info_u_type    event_info,
  void                        * user_data_ptr
);

/*===========================================================================
  FUNCTION SNOOP_IPI_REG_IP_VJCH_VJUCH()

  DESCRIPTION
    This function registers the callbacks for snooping IP, VJCH and VJUCH 
    packets.

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static void snoop_ipi_reg_ip_vjch_vjuch
(
  ppp_dev_enum_type dev    /* device instance of SM */
);


/*= = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =

                          static FUNCTION DEFINTIONS

= = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =*/
/*===========================================================================
  FUNCTION SNOOP_IPI_TRANSITION_STATE()

  DESCRIPTION
    This function effects the transition of the SM with the specified
    instance to the specified state and performs any processing required to
    transition into the new state.

  RETURN VALUE
    None

  DEPENDENCIES
    The SM should have been initialized already.

  SIDE EFFECTS
    None
===========================================================================*/
static void snoop_ipi_transition_state
(
  ppp_dev_enum_type dev,                        /* device instance of SM   */
  snoop_ipi_state_type new_state                /* state to transition to  */
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if(dev >= PPP_MAX_DEV)
  {
    LOG_MSG_ERROR_1("snoop_ipi_transition_state: Invalid device %d", dev);
    ASSERT(0);
    return;
  }

  LOG_MSG_INFO2_3("Snoop IP dev %d state %d to %d",
          dev, snpipi_cb_arr[dev].state, new_state);

  /*-------------------------------------------------------------------------
    Perform action associated with entering into new state based upon the
    current state.
  -------------------------------------------------------------------------*/
  switch(new_state)
  {
    case SNOOP_IP_NULL_STATE:
      /*---------------------------------------------------------------------
        If we are snooping, deregister the snoop functions and reset all
        state variables required for VJCH detetcion. For all cases
        deregister ps_iface IPFLTR_UPDATED event.
      ---------------------------------------------------------------------*/
      switch(snpipi_cb_arr[dev].state)
      {
        case SNOOP_IP_ALL_STATE:
          snoop_unreg_event(snpipi_cb_arr[dev].tag_vjch);

          memset(snpipi_cb_arr[dev].l_slot_arr,
                 0,
                 sizeof(snpipi_cb_arr[dev].l_slot_arr));

          /* fall through for further cleanup */

        case SNOOP_IP_NONCOMP_STATE:
          snoop_unreg_event(snpipi_cb_arr[dev].tag_ip);
          snoop_unreg_event(snpipi_cb_arr[dev].tag_vjuch);

          snpipi_cb_arr[dev].c_slot       = 255;
          snpipi_cb_arr[dev].f_toss       = FALSE;
          snpipi_cb_arr[dev].n_local_slot = 0;

          /* fall through for further cleanup */

        case SNOOP_IP_NONE_STATE:
          ps_iface_event_cback_dereg(snpipi_cb_arr[dev].if_ptr,
                                     IFACE_IPFLTR_UPDATED_EV,
                                     snpipi_cb_arr[dev].ev_buf_ptr);

          /* reset the iface ptr since PPP dev is down */
          snpipi_cb_arr[dev].if_ptr = NULL;
          break;

        case SNOOP_IP_NULL_STATE:
          /* do nothing */
          break;

        default:
          LOG_MSG_ERROR_1("Invalid state %d", snpipi_cb_arr[dev].state);
          ASSERT(0);
          return;
      }

      break;

    case SNOOP_IP_NONE_STATE:
      /*---------------------------------------------------------------------
        If we are snooping, deregister the snoop functions and reset all
        state variables required for VJCH detetcion. If we were at NULL
        state register for IPFLTR_UPDATED ev to detect when the filtering
        is enabled on the iface.
      ---------------------------------------------------------------------*/
      switch(snpipi_cb_arr[dev].state)
      {
        case SNOOP_IP_ALL_STATE:
          snoop_unreg_event(snpipi_cb_arr[dev].tag_vjch);

          memset(snpipi_cb_arr[dev].l_slot_arr,
                 0,
                 sizeof(snpipi_cb_arr[dev].l_slot_arr));

          /* fall through for further cleanup */

        case SNOOP_IP_NONCOMP_STATE:
          snoop_unreg_event(snpipi_cb_arr[dev].tag_ip);
          snoop_unreg_event(snpipi_cb_arr[dev].tag_vjuch);

          snpipi_cb_arr[dev].c_slot       = 255;
          snpipi_cb_arr[dev].f_toss       = FALSE;
          snpipi_cb_arr[dev].n_local_slot = 0;

          break;

        case SNOOP_IP_NULL_STATE:
          /*-----------------------------------------------------------------
            Reg iface IPFLTR_UPDATED ev to detect filtering enable/disable.
            If event reg fails snooping won't work correctly, so we stay in
            NULL state. Snooping/IP filtering won't be operational.
          -----------------------------------------------------------------*/
          {
            DATA_ERR_FATAL("Event reg failed, cann't snoop IP");
            return;
          }
          break;

        case SNOOP_IP_NONE_STATE:
          /* do nothing */
          break;

        default:
          LOG_MSG_ERROR_1("Invalid state %d", snpipi_cb_arr[dev].state);
          ASSERT(0);
          return;
      }

      break;

    case SNOOP_IP_NONCOMP_STATE:
      switch(snpipi_cb_arr[dev].state)
      {
        case SNOOP_IP_ALL_STATE:
          /*-----------------------------------------------------------------
            No need to deregister the VJCH callback since we do state 
            state verification within the callback itself.
            Just reset the VJ decompression parameters.
          -----------------------------------------------------------------*/
          snpipi_cb_arr[dev].c_slot       = 255;
          snpipi_cb_arr[dev].f_toss       = FALSE;
          snpipi_cb_arr[dev].n_local_slot = 0;

          memset(snpipi_cb_arr[dev].l_slot_arr,
                 0,
                 sizeof(snpipi_cb_arr[dev].l_slot_arr));

          /*-----------------------------------------------------------------
            Reg iface IPFLTR_UPDATED ev to detect filtering enable/disable.
            If event reg fails snooping won't work correctly, so we stay in
            NULL state. Snooping/IP filtering won't be operational.
          -----------------------------------------------------------------*/
          if (ps_iface_event_cback_reg(snpipi_cb_arr[dev].if_ptr,
                                       IFACE_IPFLTR_UPDATED_EV,
                                       snpipi_cb_arr[dev].ev_buf_ptr) < 0)
          {
            DATA_ERR_FATAL("Event reg failed, can't snoop IP");
            return;
          }

          /*-----------------------------------------------------------------
            Now start snooping on IP and VJUCH
          -----------------------------------------------------------------*/
          snoop_ipi_reg_ip_vjch_vjuch(dev);
          break;

        case SNOOP_IP_NULL_STATE:

          /*-----------------------------------------------------------------
            Reg iface IPFLTR_UPDATED ev to detect filtering enable/disable.
            If event reg fails snooping won't work correctly, so we stay in
            NULL state. Snooping/IP filtering won't be operational.
          -----------------------------------------------------------------*/
          if (ps_iface_event_cback_reg(snpipi_cb_arr[dev].if_ptr,
                                       IFACE_IPFLTR_UPDATED_EV,
                                       snpipi_cb_arr[dev].ev_buf_ptr) < 0)
          {
            DATA_ERR_FATAL("Event reg failed, cann't snoop IP");
            return;
          }

          /*-----------------------------------------------------------------
            Now start snooping on IP, VJUCH and VJCH.
          -----------------------------------------------------------------*/
          snoop_ipi_reg_ip_vjch_vjuch(dev);
          break;

        case SNOOP_IP_NONE_STATE:
          /*-----------------------------------------------------------------
            Now start snooping on IP, VJUCH and VJCH.
          -----------------------------------------------------------------*/
          snoop_ipi_reg_ip_vjch_vjuch(dev);
          break;

        case SNOOP_IP_NONCOMP_STATE:
          /* do nothing */
          break;

        default:
          LOG_MSG_ERROR_1("Invalid state %d", snpipi_cb_arr[dev].state);
          ASSERT(0);
          return;
      }

      break;

    case SNOOP_IP_ALL_STATE:
      switch(snpipi_cb_arr[dev].state)
      {
        case SNOOP_IP_NONCOMP_STATE:
          break;

        case SNOOP_IP_NULL_STATE:
        case SNOOP_IP_NONE_STATE:
          /* not allowed */
          ASSERT(0);
          break;

        case SNOOP_IP_ALL_STATE:
          /* do nothing */
          break;

        default:
          LOG_MSG_ERROR_1("Invalid state %d", snpipi_cb_arr[dev].state);
          ASSERT(0);
          return;
      }
      break;

    default:
      LOG_MSG_ERROR_1("Invalid state %d", new_state);
      ASSERT(0);
      return;
  } /* switch(new state) */

  snpipi_cb_arr[dev].state = new_state;

} /* snoop_ipi_transition_state() */


/*===========================================================================
  FUNCTION SNOOP_IPI_PROCESS_PPP_DEV_UP_EV()

  DESCRIPTION
    This function processes SNOOP_IP_PPP_DEV_UP_EV which indicates that
    the underlying PPP device is up and ready to transfer IP packets.
    If the IP filtering is enabled on the associatd iface, the SM will
    start snooping.

  RETURN VALUE
    None

  DEPENDENCIES
    The SM should have been initialized already.

  SIDE EFFECTS
    None
===========================================================================*/
static void snoop_ipi_process_ppp_dev_up_ev
(
  ps_cmd_enum_type cmd,                          /* cmd to be processed    */
  void *user_data_ptr                            /* user parameter         */
)
{
  ppp_dev_enum_type dev;              /* device instance = user parameter  */
  ppp_type *ppp_cb_array = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  dev = (ppp_dev_enum_type) (uint32)user_data_ptr;

  if(dev >= PPP_MAX_DEV)
  {
    LOG_MSG_ERROR_1("snoop_ipi_process_ppp_dev_up_ev: Invalid device %d", dev);
    ASSERT(0);
    return;
  }

  LOG_MSG_INFO3_1("Recvd PPP_DEV_UP_EV, state %d", snpipi_cb_arr[dev].state);

  ppp_cb_array = fsm_get_ppp_cb_array();
  switch(snpipi_cb_arr[dev].state)
  {
    case SNOOP_IP_NULL_STATE:
      /*---------------------------------------------------------------------
        PPP device is up, save the iface ptr associated with this PPP dev.
        If the iface asscociated with this PPP device has IP filtering
        enabled we transition state to start snooping on all IP
        (except VJCH TCP) protocol packets. Otherwise just transition state
        to indicate that the device is up.
      ---------------------------------------------------------------------*/
      snpipi_cb_arr[dev].if_ptr =
                       ppp_cb_array[dev].fsm[IPCP].rx_iface_ptr;

      if(PS_IFACE_NUM_FILTERS(snpipi_cb_arr[dev].if_ptr,
                              IP_FLTR_CLIENT_SOCKETS) > 0)
      {
        snoop_ipi_transition_state(dev, SNOOP_IP_NONCOMP_STATE);
      }
      else
      {
        snoop_ipi_transition_state(dev, SNOOP_IP_NONE_STATE);
      }
      break;

    case SNOOP_IP_NONE_STATE:
    case SNOOP_IP_NONCOMP_STATE:
    case SNOOP_IP_ALL_STATE:
      /*---------------------------------------------------------------------
        Ignoring this event in all other states
      ---------------------------------------------------------------------*/
      LOG_MSG_INFO2_1("Ignoring PPP_DEV_UP_EV, state %d",
                      snpipi_cb_arr[dev].state );
      break;

    default:
      /*---------------------------------------------------------------------
        Invalid state
      ---------------------------------------------------------------------*/
      LOG_MSG_ERROR_2("Invalid state %d, dev %d",
                      snpipi_cb_arr[dev].state, dev);
      ASSERT(0);

  } /* switch(state) */
} /* snoop_ipi_process_ppp_dev_up_ev() */


/*===========================================================================
  FUNCTION SNOOP_IPI_PROCESS_PPP_DEV_DOWN_EV()

  DESCRIPTION
    This function processes SNOOP_IP_PPP_DEV_DOWN_EV which indicates that
    the underlying PPP device is down and SM can stop snooping.

  RETURN VALUE
    None

  DEPENDENCIES
    The SM should have been initialized already.

  SIDE EFFECTS
    None
===========================================================================*/
static void snoop_ipi_process_ppp_dev_down_ev
(
  ps_cmd_enum_type cmd,                          /* cmd to be processed    */
  void *user_data_ptr                            /* user parameter         */
)
{
  ppp_dev_enum_type dev;              /* device instance = user parameter  */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  dev = (ppp_dev_enum_type) (uint32)user_data_ptr;

  if(dev >= PPP_MAX_DEV)
  {
    LOG_MSG_ERROR_1("snoop_ipi_process_ppp_dev_down_ev: Invalid device %d",
                    dev);
    ASSERT(0);
    return;
  }

  LOG_MSG_INFO3_1("Recvd PPP_DEV_DOWN_EV, state %d", snpipi_cb_arr[dev].state);

  switch(snpipi_cb_arr[dev].state)
  {
    case SNOOP_IP_NONE_STATE:
    case SNOOP_IP_NONCOMP_STATE:
    case SNOOP_IP_ALL_STATE:
      /*---------------------------------------------------------------------
        PPP device is down, no data to snoop on, go ot NULL state
      ---------------------------------------------------------------------*/
      snoop_ipi_transition_state(dev, SNOOP_IP_NULL_STATE);
      break;

    case SNOOP_IP_NULL_STATE:
      /*---------------------------------------------------------------------
        Ignoring this event in all other states
      ---------------------------------------------------------------------*/
      LOG_MSG_INFO2_0("Ignoring PPP_DEV_DOWN_EV");
      break;

    default:
      /*---------------------------------------------------------------------
        Invalid state
      ---------------------------------------------------------------------*/
      LOG_MSG_ERROR_1("Invalid state, dev %d", dev);
      ASSERT(0);

  } /* switch(state) */
} /* snoop_ipi_process_ppp_dev_down_ev() */


/*===========================================================================
  FUNCTION SNOOP_IPI_PROCESS_IP_FLTR_ENABLED_EV()

  DESCRIPTION
    This function processes SNOOP_IP_FLTR_ENABLED_EV which indicates
    that IP filtering has been enabled on the associated iface, SM will
    start snooping if it was not already doing so.

  RETURN VALUE
    None

  DEPENDENCIES
    The SM should have been initialized already.

  SIDE EFFECTS
    None
===========================================================================*/
static void snoop_ipi_process_ip_fltr_enabled_ev
(
  ps_cmd_enum_type cmd,                          /* cmd to be processed    */
  void *user_data_ptr                            /* user parameter         */
)
{
  ppp_dev_enum_type dev;              /* device instance = user parameter  */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  dev = (ppp_dev_enum_type) (uint32)user_data_ptr;

  if(dev >= PPP_MAX_DEV)
  {
    LOG_MSG_ERROR_1("snoop_ipi_process_ip_fltr_enabled_ev: Invalid device %d",
                    dev);
    ASSERT(0);
    return;
  }

  LOG_MSG_INFO3_1("Recvd IP_FLTR_ENABLED_EV, state %d",
                  snpipi_cb_arr[dev].state);

  switch(snpipi_cb_arr[dev].state)
  {
    case SNOOP_IP_NONE_STATE:
      /*---------------------------------------------------------------------
        IP filtering is enabled on iface when underlying PPP device is
        already up (transferring data), start snooping.
      ---------------------------------------------------------------------*/
      snoop_ipi_transition_state(dev, SNOOP_IP_NONCOMP_STATE);
      break;

    case SNOOP_IP_NULL_STATE:
    case SNOOP_IP_NONCOMP_STATE:
    case SNOOP_IP_ALL_STATE:
      /*---------------------------------------------------------------------
        Ignoring this event in all other states
      ---------------------------------------------------------------------*/
      LOG_MSG_INFO2_1("Ignoring IP_FLTR_ENABLED_EV, state %d",
                      snpipi_cb_arr[dev].state );
      break;

    default:
      /*---------------------------------------------------------------------
        Invalid state
      ---------------------------------------------------------------------*/
      LOG_MSG_ERROR_2("Invalid state %d, dev %d",
                      snpipi_cb_arr[dev].state, dev);
      ASSERT(0);

  } /* switch(state) */
} /* snoop_ipi_process_ip_fltr_enabled_ev() */


/*===========================================================================
  FUNCTION SNOOP_IPI_PROCESS_IP_FLTR_DISABLED_EV()

  DESCRIPTION
    This function processes SNOOP_IP_FLTR_DISABLED_EV which indicates
    that IP filtering has been disabled on the associated iface, SM will not
    snoop anymore.

  RETURN VALUE
    None

  DEPENDENCIES
    The SM should have been initialized already.

  SIDE EFFECTS
    None
===========================================================================*/
static void snoop_ipi_process_ip_fltr_disabled_ev
(
  ps_cmd_enum_type cmd,                          /* cmd to be processed    */
  void *user_data_ptr                            /* user parameter         */
)
{
  ppp_dev_enum_type dev;              /* device instance = user parameter  */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  dev = (ppp_dev_enum_type) (uint32)user_data_ptr;

  if(dev >= PPP_MAX_DEV)
  {
    LOG_MSG_ERROR_1("snoop_ipi_process_ip_fltr_disabled_ev: Invalid device %d",
                    dev);
    ASSERT(0);
    return;
  }

  LOG_MSG_INFO3_1("Recvd IP_FLTR_DISABLED_EV,state %d",
                  snpipi_cb_arr[dev].state);

  switch(snpipi_cb_arr[dev].state)
  {
    case SNOOP_IP_NONCOMP_STATE:
    case SNOOP_IP_ALL_STATE:
      /*---------------------------------------------------------------------
        IP Filtering has been disabled on iface, stop snooping, don't go
        NULL since PPP device is still up, to handle the case when filtering
        gets enabled again
      ---------------------------------------------------------------------*/
      snoop_ipi_transition_state(dev, SNOOP_IP_NONE_STATE);
      break;

    case SNOOP_IP_NULL_STATE:
    case SNOOP_IP_NONE_STATE:
      /*---------------------------------------------------------------------
        Ignoring this event in all other states
      ---------------------------------------------------------------------*/
      LOG_MSG_INFO2_1("Ignoring IP_FLTR_DISABLED_EV, state %d",
                      snpipi_cb_arr[dev].state );
      break;

    default:
      /*---------------------------------------------------------------------
        Invalid state
      ---------------------------------------------------------------------*/
      LOG_MSG_ERROR_2("Invalid state %d, dev %d",
                      snpipi_cb_arr[dev].state, dev);
      ASSERT(0);

  } /* switch(state) */
} /* snoop_ipi_process_ip_fltr_disabled_ev() */


/*===========================================================================
  FUNCTION SNOOP_IPI_PROCESS_IP_COMP_ON_EV()

  DESCRIPTION
    This function processes SNOOP_IP_COMP_ON_EV which indicates that VJ
    compressed packets destined to the stack have been detected, SM will
    start snooping on VJCH protocol packets also.

  RETURN VALUE
    None

  DEPENDENCIES
    The SM should have been initialized already.

  SIDE EFFECTS
    None
===========================================================================*/
static void snoop_ipi_process_ip_comp_on_ev
(
  ps_cmd_enum_type cmd,                          /* cmd to be processed    */
  void *user_data_ptr                            /* user parameter         */
)
{
  ppp_dev_enum_type dev;              /* device instance = user parameter  */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  dev = (ppp_dev_enum_type) (uint32)user_data_ptr;

  if(dev >= PPP_MAX_DEV)
  {
    LOG_MSG_ERROR_1("snoop_ipi_process_ip_comp_on_ev: Invalid device %d", dev);
    ASSERT(0);
    return;
  }

  LOG_MSG_INFO3_1("Recvd IP_COMP_ON_EV, state %d", snpipi_cb_arr[dev].state);

  switch(snpipi_cb_arr[dev].state)
  {
    case SNOOP_IP_NONCOMP_STATE:
      /*---------------------------------------------------------------------
        VJ compressed packet detected for stack, sp we need to also snoop on
        VJCH along with all other IP protocols.
      ---------------------------------------------------------------------*/
      snoop_ipi_transition_state(dev, SNOOP_IP_ALL_STATE);
      break;

    case SNOOP_IP_NULL_STATE:
    case SNOOP_IP_NONE_STATE:
    case SNOOP_IP_ALL_STATE:
      /*---------------------------------------------------------------------
        Ignoring this event in all other states
      ---------------------------------------------------------------------*/
      LOG_MSG_INFO2_1("Ignoring IP_COMP_ON_EV, state %d",
                      snpipi_cb_arr[dev].state );
      break;

    default:
      /*---------------------------------------------------------------------
        Invalid state
      ---------------------------------------------------------------------*/
      LOG_MSG_ERROR_2("Invalid state %d, dev %d",
                      snpipi_cb_arr[dev].state, dev);
      ASSERT(0);

  } /* switch(state) */
} /* snoop_ipi_process_ip_comp_on_ev() */


/*===========================================================================
  FUNCTION SNOOP_IPI_PROCESS_IP_COMP_OFF_EV()

  DESCRIPTION
    This function processes SNOOP_IP_COMP_OFF_EV which indicates that stack
    will not be receiving anymore VJCH packets without getting any VJUCH
    packets, so snooping on compressed packets can be turned off until
    any VJUCH packets are detected for the stack.

  RETURN VALUE
    None

  DEPENDENCIES
    The SM should have been initialized already.

  SIDE EFFECTS
    None
===========================================================================*/
static void snoop_ipi_process_ip_comp_off_ev
(
  ps_cmd_enum_type cmd,                          /* cmd to be processed    */
  void *user_data_ptr                            /* user parameter         */
)
{
  ppp_dev_enum_type dev;              /* device instance = user parameter  */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  dev = (ppp_dev_enum_type) (uint32)user_data_ptr;

  if(dev >= PPP_MAX_DEV)
  {
    LOG_MSG_ERROR_1("snoop_ipi_process_ip_comp_off_ev: Invalid device %d",
                    dev);
    ASSERT(0);
    return;
  }

  LOG_MSG_INFO3_1("Recvd IP_COMP_OFF_EV, state %d", snpipi_cb_arr[dev].state);

  switch(snpipi_cb_arr[dev].state)
  {
    case SNOOP_IP_ALL_STATE:
      /*---------------------------------------------------------------------
        All local sockets doing VJCH have been closed, so stop snooping on
        VJCH for now.
      ---------------------------------------------------------------------*/
      snoop_ipi_transition_state(dev, SNOOP_IP_NONCOMP_STATE);
      break;

    case SNOOP_IP_NULL_STATE:
    case SNOOP_IP_NONE_STATE:
    case SNOOP_IP_NONCOMP_STATE:
      /*---------------------------------------------------------------------
        Ignoring this event in all other states
      ---------------------------------------------------------------------*/
      LOG_MSG_INFO2_1("Ignoring IP_COMP_OFF_EV, state %d",
                      snpipi_cb_arr[dev].state );
      break;

    default:
      /*---------------------------------------------------------------------
        Invalid state
      ---------------------------------------------------------------------*/
      LOG_MSG_ERROR_2("Invalid state %d, dev %d",
                      snpipi_cb_arr[dev].state, dev);
      ASSERT(0);

  } /* switch(state) */
} /* snoop_ipi_process_ip_comp_off_ev() */


/*===========================================================================
FUNCTION   SNOOP_IPI_PROCESS_IP_CB()

DESCRIPTION
  This callback function is called when any non compressed IP pkt (prot=0x21)
  is received on a PPP device for which IP filtering has been
  enabled. Pkt is processed through the filters currently installed in the
  iface associated with the PPP dev and based on the processing it is either
  passed up the stack or forwarded to the bridged PPP device. In case there
  is an error in processing the pkt, it is still forwarded to give TE a
  chance to look at the pkt (in case it is useful).

DEPENDENCIES
  This callback should only be registered in SNOOP_IP_NONCOMP_STATE or
  SNOOP_IP_ALL_STATE.

RETURN VALUE
  SNOOP_CB_FORWARD_PKT: The pkt is either bad, undetected or does not belong
                        to the stack,so forward it to the PPP bridge device.

  SNOOP_CB_PROCESS_PKT: the packet needs to be sent up the stack since it
                        is detected to be belonging to a local socket.

SIDE EFFECTS
  None
===========================================================================*/
static snoop_cb_ret_val_enum_type snoop_ipi_process_ip_cb
(
  ppp_dev_enum_type    dev,                 /* device on which pkt arrived */
  uint16               protocol,            /* protocol of packet          */
  dsm_item_type     ** pkt_ptr,             /* item containing pkt data    */
  ps_rx_meta_info_type ** meta_info_ref_ptr /* ptr to ref to PS meta info  */
)
{
  void              *fraghdl  = NULL;
  dsm_item_type     *frag_ptr = NULL;
  errno_enum_type    ps_errno;
  ps_iface_ipfltr_client_id_enum_type  client_id = IP_FLTR_CLIENT_SOCKETS;
  ip_pkt_info_type  *meta_pkt_info_ptr = NULL;
  ppp_type          *ppp_cb_array = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    Verify arguments
  -------------------------------------------------------------------------*/
  if( pkt_ptr == NULL || *pkt_ptr == NULL )
  {
     DATA_ERR_FATAL("Invalid DSM item passed");
    return SNOOP_CB_FORWARD_PKT;
  }

  if( dev >= PPP_MAX_DEV || protocol != PPP_IP_PROTOCOL ||
      meta_info_ref_ptr == NULL )
  {
    LOG_MSG_ERROR_2( "Invalid arguments: device %d, protocol %d,"
                     " meta_info_ref_ptr NULL", dev, protocol );
     ASSERT( 0 );
    return SNOOP_CB_FORWARD_PKT;
  }

  ppp_cb_array = fsm_get_ppp_cb_array();

  /*-------------------------------------------------------------------------
    Filter the packet.  In case of error or if no match found free the meta
    info
  -------------------------------------------------------------------------*/
  if( -1 == ps_pkt_info_filter_rx_pkt( snpipi_cb_arr[dev].if_ptr,
                                    client_id,
                                    pkt_ptr,
                                    meta_info_ref_ptr,
                                       &ps_errno ) )
  {
    /*-----------------------------------------------------------------------
      An error value of E_WOULD_BLOCK indicates that the packet is a
      fragment and is being held by the fragment layer. Set the bridged PPP
      Dev in the corresponding Frag handle and free the Meta Info.
    -----------------------------------------------------------------------*/
    if( E_WOULD_BLOCK == ps_errno )
    {
      meta_pkt_info_ptr = &(PS_RX_META_GET_PKT_INFO( *meta_info_ref_ptr ));
      ps_ip_fraghdl_set_bridge_device(meta_pkt_info_ptr->fraghdl,
                                      (void *)(ppp_cb_array[dev].bridge_dev),
                                      IP_FRAGHDL_BRIDGE_TYPE_PPP_BRIDGE,
                                      ps_pppi_frag_tx_cmd);
      PS_RX_META_INFO_FREE( meta_info_ref_ptr );
      return SNOOP_CB_IGNORE_PKT;
    }
    PS_RX_META_INFO_FREE( meta_info_ref_ptr );
    return SNOOP_CB_FORWARD_PKT;
  }

  if( NULL == *meta_info_ref_ptr )
  {
    return SNOOP_CB_FORWARD_PKT;
  }

  if( PS_IFACE_IPFLTR_NOMATCH ==
        PS_RX_META_GET_FILTER_RESULT(*meta_info_ref_ptr, (uint8)client_id))
  {
    fraghdl = (&(PS_RX_META_GET_PKT_INFO(*meta_info_ref_ptr)))->fraghdl;
    PS_RX_META_INFO_FREE( meta_info_ref_ptr );

    /*-----------------------------------------------------------------------
      If fraghdl is not NULL then there are fragments returned by filtering.
      Get these fragments by calling get_queued_fragments() and forward them
      all to the bridged device
    -----------------------------------------------------------------------*/
    if( NULL != fraghdl )
    {
      while( NULL != (frag_ptr = ip_fraghdl_get_fragment(fraghdl )) )
      {
        (void) pppi_outgoing(ppp_cb_array[dev].bridge_dev,
                             PPP_IP_PROTOCOL,
                             &frag_ptr,
                             NULL );
      }
    }
    return SNOOP_CB_FORWARD_PKT;
  }

  /*-------------------------------------------------------------------------
    If we come here it means the pkt belongs to the stack.
  -------------------------------------------------------------------------*/
  return SNOOP_CB_PROCESS_PKT;

} /* snoop_ipi_process_ip_cb() */


/*===========================================================================
FUNCTION   SNOOP_IPI_PROCESS_VJUCH_CB()

DESCRIPTION
  This callback function is called when any uncompressed TCP (prot=0x2f) is
  received on a PPP device for which IP filtering has been enabled. Pkt is
  processed through the filters currently installed in the iface associated
  with the PPP dev.If the pkt is passed up the stack the slot id is marked so
  that subsequent VJCH pkts with same slot id can also be sent to the stack
  and the recipient socket is added to the list of sockets doing VJCH.
  If pkt is forwarded to the bridged PPP device the slot id is unmarked. In
  case there is an error in processing the pkt, it is still forwarded to give
  TE a chance to look at the pkt (in case it is useful) but toss flag is set
  so that we donot reuse this slot id for subsequent VJCH pkt with C bit off.

DEPENDENCIES
  This callback should only be registered in SNOOP_IP_NONCOMP_STATE or
  SNOOP_IP_ALL_STATE.

RETURN VALUE
  SNOOP_CB_FORWARD_PKT: The pkt is either bad, undetected or does not belong
                        to the stack,so forward it to the PPP bridge device.

  SNOOP_CB_PROCESS_PKT: the packet needs to be sent up the stack since it
                        is destiined to a local socket.

SIDE EFFECTS
  None
===========================================================================*/
static snoop_cb_ret_val_enum_type snoop_ipi_process_vjuch_cb
(
  ppp_dev_enum_type    dev,                 /* device on which pkt arrived */
  uint16               protocol,            /* protocol of packet          */
  dsm_item_type     ** pkt_ptr,             /* item containing pkt data    */
  ps_rx_meta_info_type ** meta_info_ref_ptr /* ptr to ref to PS meta info  */
)
{
  dsm_item_type    * item_ptr;
  dsm_item_type    * frag_ptr = NULL;
  void             * fraghdl = NULL;
  uint16             offset;
  uint8              slot;
  errno_enum_type    ps_errno;
  ps_iface_ipfltr_client_id_enum_type  client_id = IP_FLTR_CLIENT_SOCKETS;
  ip_pkt_info_type * meta_pkt_info_ptr = NULL;
  ppp_type         * ppp_cb_array = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    Check arguments
  -------------------------------------------------------------------------*/
  if( pkt_ptr == NULL || *pkt_ptr == NULL )
  {
     DATA_ERR_FATAL("Invalid DSM item passed");
    return SNOOP_CB_FORWARD_PKT;
  }

  if( dev >= PPP_MAX_DEV || protocol != PPP_VJUCH_PROTOCOL ||
      meta_info_ref_ptr == NULL )
  {
     LOG_MSG_ERROR_3( "Invalid arguments:, device %d, protocol %d,"
               " meta_info_ref_ptr %d", dev, protocol, meta_info_ref_ptr );
     ASSERT( 0 );
    return SNOOP_CB_FORWARD_PKT;
  }

  /*-------------------------------------------------------------------------
    A VJ uncompressed packet contains the slot to be used in the protocol
    field i.e. 10th byte.  Extract the slot and verify it is in range
  -------------------------------------------------------------------------*/
  offset = 9;
  if(dsm_extract(*pkt_ptr, offset, &slot, sizeof(uint8)) != sizeof(uint8))
  {
    LOG_MSG_ERROR_0( "Truncated packet" );
    return SNOOP_CB_FORWARD_PKT;
  }

  if(slot >= SLHC_MAX_SLOTS)
  {
    /* The slot id is out of range */
    snpipi_cb_arr[dev].f_toss = TRUE;
    return SNOOP_CB_FORWARD_PKT;
  }

  /*-------------------------------------------------------------------------
    Replace slot with the protocol value for TCP
    TODO: use the new dsm function to write a byte when that is available
  -------------------------------------------------------------------------*/
  item_ptr = *pkt_ptr;
  while( item_ptr->used < offset )
  {
    offset -= item_ptr->used;
    item_ptr = item_ptr->pkt_ptr;
  }
  *(item_ptr->data_ptr + offset) = (uint8)PS_IPPROTO_TCP;

  /*-------------------------------------------------------------------------
    Store the slot id
  -------------------------------------------------------------------------*/
  snpipi_cb_arr[dev].c_slot = slot;
  snpipi_cb_arr[dev].f_toss = FALSE;

  ppp_cb_array = fsm_get_ppp_cb_array();
  /*-------------------------------------------------------------------------
    Filter the packet.  In case of error or if no match found free the meta
    info and restore the protocol value with the slot number
    TODO: use the new dsm function to write a byte when that is available
  -------------------------------------------------------------------------*/
  if( -1 == ps_pkt_info_filter_rx_pkt( snpipi_cb_arr[dev].if_ptr,
                                    client_id,
                                    pkt_ptr,
                                    meta_info_ref_ptr,
                                       &ps_errno ) )
  {
    /*-----------------------------------------------------------------------
      An error value of E_WOULD_BLOCK indicates that the packet is a
      fragment and is being held by the fragment layer. Set the bridged PPP
      Dev in the corresponding Frag handle and free the Meta Info.
    -----------------------------------------------------------------------*/
    if( E_WOULD_BLOCK == ps_errno )
    {
      meta_pkt_info_ptr = &(PS_RX_META_GET_PKT_INFO( *meta_info_ref_ptr ));
      ps_ip_fraghdl_set_bridge_device(meta_pkt_info_ptr->fraghdl,
                                      (void *)(ppp_cb_array[dev].bridge_dev),
                                      IP_FRAGHDL_BRIDGE_TYPE_PPP_BRIDGE,
                                      ps_pppi_frag_tx_cmd);
      PS_RX_META_INFO_FREE( meta_info_ref_ptr );
      return SNOOP_CB_IGNORE_PKT;
    }

    PS_RX_META_INFO_FREE( meta_info_ref_ptr );
    *(item_ptr->data_ptr + offset) = slot;
    return SNOOP_CB_FORWARD_PKT;
  }

  /*-------------------------------------------------------------------------
    Restore the slot in the packet.
  -------------------------------------------------------------------------*/
  *(item_ptr->data_ptr + offset) = slot;

  /*-------------------------------------------------------------------------
    If meta info returned is NULL we can't determine whom this packet
    belongs to.  Return from this function
  -------------------------------------------------------------------------*/
  if( NULL == *meta_info_ref_ptr )
  {
    return SNOOP_CB_FORWARD_PKT;
  }

  if( PS_IFACE_IPFLTR_NOMATCH ==
        PS_RX_META_GET_FILTER_RESULT(*meta_info_ref_ptr, (uint8)client_id) )
  {
    /*-----------------------------------------------------------------------
      If fraghdl is not NULL then there are fragments returned by filtering.
      Get these fragments by calling get_queued_fragments() and forward them
      all to the bridged device
    -----------------------------------------------------------------------*/
    if( NULL != (fraghdl = (&(PS_RX_META_GET_PKT_INFO(*meta_info_ref_ptr)))->fraghdl) )
    {
      while( NULL != (frag_ptr = ip_fraghdl_get_fragment(fraghdl )) )
      {
        (void) pppi_outgoing(ppp_cb_array[dev].bridge_dev,
                             PPP_IP_PROTOCOL,
                             &frag_ptr,
                             NULL );
      }
    }

    /*-----------------------------------------------------------------------
      Reset VJCH state if this slot was snooped earlier
    -----------------------------------------------------------------------*/
    if (TRUE == snpipi_cb_arr[dev].l_slot_arr[slot])
    {
      snpipi_cb_arr[dev].n_local_slot--;
      snpipi_cb_arr[dev].l_slot_arr[slot] = FALSE;

      if (snpipi_cb_arr[dev].n_local_slot == 0)
      {
        snoop_ip_post_event(dev, SNOOP_IP_COMP_OFF_EV);
      }
    }

    PS_RX_META_INFO_FREE( meta_info_ref_ptr );
    return SNOOP_CB_FORWARD_PKT;
  }

  /*-------------------------------------------------------------------------
    If we come here it means the pkt belongs to the stack. Mark the slot
    as belonging to us and post event to start snooping for VJCH if we are
    not doing so already.
  -------------------------------------------------------------------------*/
  if (FALSE == snpipi_cb_arr[dev].l_slot_arr[slot])
  {
    snpipi_cb_arr[dev].l_slot_arr[slot] = TRUE;
    snpipi_cb_arr[dev].n_local_slot++;

    /* if first local VJ slot, enable snooping */
    if (snpipi_cb_arr[dev].n_local_slot == 1)
    {
      snoop_ip_post_event(dev, SNOOP_IP_COMP_ON_EV);
    }
  }

  /* return appropriate code to pass the pkt up the stack */
  return SNOOP_CB_PROCESS_PKT;

} /* snoop_ipi_process_vjuch_cb() */


/*===========================================================================
FUNCTION   SNOOP_IPI_PROCESS_VJCH_CB()

DESCRIPTION
  This callback function is called when any compressed TCP (prot=0x2d) is
  received on a PPP device for which IP filtering has been enabled and at
  least one local socket is currenlty doing VJCH. Slot id is extracted from
  the pkt and based on whether slot id is marked or not the pkt is passed up
  the stack or forwarded to the PPP bridged device. Also if the pkt is passd up
  the stack the recipient socket is added to the list of sockets doing VJCH.

DEPENDENCIES
  This callback should only be registered in SNOOP_IP_ALL_STATE.

RETURN VALUE
  SNOOP_CB_FORWARD_PKT: The pkt is either bad, undetected or does not belong
                        to the stack,so forward it to the PPP bridge device.

  SNOOP_CB_PROCESS_PKT: the packet needs to be sent up the stack since it
                        is destined to a local socket.

SIDE EFFECTS
  None
===========================================================================*/
static snoop_cb_ret_val_enum_type snoop_ipi_process_vjch_cb
(
  ppp_dev_enum_type    dev,                 /* device on which pkt arrived */
  uint16               protocol,            /* protocol of packet          */
  dsm_item_type     ** pkt_ptr,             /* item containing pkt data    */
  ps_rx_meta_info_type ** meta_info_ref_ptr /* ptr to ref to PS meta info  */
)
{
  uint8  change_mask;
  uint8  slot;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if( pkt_ptr == NULL || *pkt_ptr == NULL )
  {
     DATA_ERR_FATAL("Invalid DSM item passed");
     return SNOOP_CB_FORWARD_PKT;
  }

  if( dev >= PPP_MAX_DEV || protocol != PPP_VJCH_PROTOCOL )
  {
     LOG_MSG_ERROR_2( "Invalid arguments:, device %d, protocol %d",
                      dev, protocol );
     ASSERT( 0 );
     return SNOOP_CB_FORWARD_PKT;
  }

  /*-------------------------------------------------------------------------
    Verify if we are in SNOOP_IP_ALL_STATE.
    If we are not, it means there is no local TCP socket doing VJ 
    compression. Hence no need to proceed with packet analysis.
    Simply return forward packet.
  -------------------------------------------------------------------------*/
  if( snpipi_cb_arr[dev].state != SNOOP_IP_ALL_STATE )
  {
    return SNOOP_CB_FORWARD_PKT;
  }

  /* we expect at least 3 bytes, 1 byte change mask, 2 bytes IP checksum */
  if(dsm_length_packet(*pkt_ptr) < 3)
  {
    /* The packet is shorter than a legal IP header */
    return SNOOP_CB_FORWARD_PKT;
  }

  /* Sneak at the change mask in the pkt */
  change_mask = (*pkt_ptr)->data_ptr[0];

  /*-------------------------------------------------------------------------
    If the slot id is explicitly included in the pkt (C bit set) use it. If
    not and if toss flag is set that means we had error in processing
    previous pkt, hence slot id cannot be assumed, forward the pkt.
  -------------------------------------------------------------------------*/
  if(change_mask & NEW_C)
  {
    if( dsm_extract(*pkt_ptr, 1, &slot, sizeof(uint8)) != sizeof(uint8) )
    {
      return SNOOP_CB_FORWARD_PKT;
    }

    if(slot >= SLHC_MAX_SLOTS)
    {
      /* The slot id is out of range */
      snpipi_cb_arr[dev].f_toss = TRUE;
      return SNOOP_CB_FORWARD_PKT;
    }

    /* got a valid slot id, use it */
    snpipi_cb_arr[dev].c_slot = slot;
    snpipi_cb_arr[dev].f_toss = FALSE;
  }
  else if(snpipi_cb_arr[dev].f_toss)
  {
    return SNOOP_CB_FORWARD_PKT;
  }
  else
  {
    slot = snpipi_cb_arr[dev].c_slot;
  }

  /*-------------------------------------------------------------------------
    We have a valid slot id, check to see if the slot id belongs to a local
    socket, if not forward the pkt.
  -------------------------------------------------------------------------*/
  if (slot >= SLHC_MAX_SLOTS)
  {
    return SNOOP_CB_FORWARD_PKT;
  }

  if(FALSE == snpipi_cb_arr[dev].l_slot_arr[slot])
  {
    return SNOOP_CB_FORWARD_PKT;
  }

  /* return appropriate code to pass the pkt up the stack */
  return SNOOP_CB_PROCESS_PKT;

} /* snoop_ipi_process_vjch_cb() */


/*===========================================================================
FUNCTION   SNOOP_IPI_IFACE_EV_HANDLER_CB()

DESCRIPTION
  This callback function is called when one of the events, registered with
  the iface associated with a PPP device, happen. The events are handled
  as needed and appropriate event is posted to IP snoop SM.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
static void snoop_ipi_iface_ev_handler_cb
(
  ps_iface_type               * this_iface_ptr, /* Ptr to the interface    */
  ps_iface_event_enum_type      event,          /* Event type occurred     */
  ps_iface_event_info_u_type    event_info,     /* Event specific info     */
  void                        * user_data_ptr   /* Data passed during event
                                                   registration            */
)
{
  ppp_dev_enum_type dev;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  LOG_MSG_INFO3_2("Recd event %d, i/f 0x%x", event, this_iface_ptr);

  dev = (ppp_dev_enum_type) (uint32)user_data_ptr;

  if(dev >= PPP_MAX_DEV)
  {
    LOG_MSG_ERROR_1("snoop_ipi_iface_ev_handler_cb: Invalid device %d", dev);
    ASSERT(0);
    return;
  }

  ASSERT(this_iface_ptr == snpipi_cb_arr[dev].if_ptr);

  if(event_info.ipfltr_info.client != IP_FLTR_CLIENT_SOCKETS)
  {
    LOG_MSG_INFO3_1("Ignoring filter update event for client %d",
                    event_info.ipfltr_info.client );
    return;
  }

  switch(event)
  {
    case IFACE_IPFLTR_UPDATED_EV:
      /*---------------------------------------------------------------------
        If the number of filters registered with the iface is > 0 notify SM
        that the filtering is enabled. Similarly if the number of filters
        is 0, notify SM that the filtering is disabled
      ---------------------------------------------------------------------*/
      if (event_info.ipfltr_info.curr_fltr_cnt > 0)
      {
        LOG_MSG_INFO1_2("IP fltr on, dev %d, i/f 0x%x", dev, this_iface_ptr);
        snoop_ip_post_event(dev, SNOOP_IP_FILTERING_ENABLED_EV);
      }
      else if (event_info.ipfltr_info.curr_fltr_cnt == 0)
      {
        LOG_MSG_INFO1_2("IP fltr off, dev %d, i/f 0x%x", dev, this_iface_ptr);
        snoop_ip_post_event(dev, SNOOP_IP_FILTERING_DISABLED_EV);
      }

      break;

    default:
      /*---------------------------------------------------------------------
        SM never registered for any other events
      ---------------------------------------------------------------------*/
      LOG_MSG_ERROR_2("Unexpected event %d, i/f 0x%x", event, this_iface_ptr);
      ASSERT(0);
      break;

  } /* switch(event) */
} /* snoop_ipi_iface_ev_handler_cb() */



/*= = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =

                        EXTERNAL FUNCTION DEFINTIONS

= = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =*/
/*===========================================================================
  FUNCTION SNOOP_IP_INIT()

  DESCRIPTION
    This function initializes all the instcances of the IP snooping state
    machine (one for each PPP device).  It also registers all the command
    handlers with PS task interface.

  PARAMETERS
    None

  RETURN VALUE
    None

  DEPENDENCIES
    Should only be called once at powerup

  SIDE EFFECTS
    Resets all the control block variables.
===========================================================================*/
void snoop_ip_init(void)
{
  /* PPP device instance, same as associated SM instance */
  ppp_dev_enum_type dev;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (inited == TRUE)
  {
    LOG_MSG_INFO2_0("Snoop IP SM already inited");
    return;
  }

  for( dev = PPP_MIN_DEV; dev < PPP_MAX_DEV; dev++ )
  {
    memset(snpipi_cb_arr + (uint8)dev, 0, sizeof(snoop_ipi_info_type));

    snpipi_cb_arr[dev].c_slot = 255;
    snpipi_cb_arr[dev].state = SNOOP_IP_NULL_STATE;
    snpipi_cb_arr[dev].ev_buf_ptr = ps_iface_alloc_event_cback_buf
                                    (
                                      snoop_ipi_iface_ev_handler_cb,
                                      (void *)dev
                                    );

    if(snpipi_cb_arr[dev].ev_buf_ptr == NULL)
    {
      LOG_MSG_ERROR_0("Snoop IP event buf alloc failed");
      ASSERT(0);
      return;
    }
  }

  /*-------------------------------------------------------------------------
    Now register the cmd handlers corresponding to all the supported events
  -------------------------------------------------------------------------*/

  (void) ps_set_cmd_handler( PS_SNOOP_IP_PPP_DEV_UP_CMD,
                             snoop_ipi_process_ppp_dev_up_ev);

  (void) ps_set_cmd_handler( PS_SNOOP_IP_PPP_DEV_DOWN_CMD,
                             snoop_ipi_process_ppp_dev_down_ev);

  (void) ps_set_cmd_handler( PS_SNOOP_IP_FLTR_ENABLED_CMD,
                             snoop_ipi_process_ip_fltr_enabled_ev);

  (void) ps_set_cmd_handler( PS_SNOOP_IP_FLTR_DISABLED_CMD,
                             snoop_ipi_process_ip_fltr_disabled_ev);

  (void) ps_set_cmd_handler( PS_SNOOP_IP_COMP_ON_CMD,
                             snoop_ipi_process_ip_comp_on_ev);

  (void) ps_set_cmd_handler( PS_SNOOP_IP_COMP_OFF_CMD,
                             snoop_ipi_process_ip_comp_off_ev);

  inited = TRUE;

  return;
} /* snoop_ip_init() */


/*===========================================================================
  FUNCTION SNOOP_IP_POST_EVENT()

  DESCRIPTION
    This function receives an event posted to the SM and enqueues the
    corresponding command to be processed in the PS task.

  PARAMETERS
    dev: PPP device type to indentify associated SM instance
    event: The event that is being posted to the SM.

  RETURN VALUE
    None

  DEPENDENCIES
    The SM should have been initialized already.

  SIDE EFFECTS
    None
===========================================================================*/
void snoop_ip_post_event
(
  ppp_dev_enum_type     dev,
  snoop_ip_event_type   event
)
{
  ps_cmd_enum_type cmd;                          /* PS Task cmd            */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if(!inited)
  {
    DATA_ERR_FATAL("Snoop IP SM not initialized");
    return;
  }

  if(dev >= PPP_MAX_DEV)
  {
    LOG_MSG_ERROR_1("Invalid device %d, post event failed", dev);
    ASSERT(0);
    return;
  }

  LOG_MSG_INFO1_3("Snoop IP dev %d, ev %d in state %d",
           dev, event, snpipi_cb_arr[dev].state);

  /*-------------------------------------------------------------------------
    For each event type post a corresponding cmd to PS
  -------------------------------------------------------------------------*/
  switch(event)
  {
    case SNOOP_IP_PPP_DEV_UP_EV:
      /*---------------------------------------------------------------------
        PPP device is up for traffic, start snooping if IP filtering enabled
      ---------------------------------------------------------------------*/
      cmd = PS_SNOOP_IP_PPP_DEV_UP_CMD;
      break;

    case SNOOP_IP_PPP_DEV_DOWN_EV:
      /*---------------------------------------------------------------------
        PPP device is going down, stop snooping
      ---------------------------------------------------------------------*/
      cmd = PS_SNOOP_IP_PPP_DEV_DOWN_CMD;
      break;

    case SNOOP_IP_FILTERING_ENABLED_EV:
      /*---------------------------------------------------------------------
        IP filtering has been enabled on the iface tied to this device
      ---------------------------------------------------------------------*/
      cmd = PS_SNOOP_IP_FLTR_ENABLED_CMD;
      break;

    case SNOOP_IP_FILTERING_DISABLED_EV:
      /*---------------------------------------------------------------------
        IP filtering has been disabled on the iface tied to this device
      ---------------------------------------------------------------------*/
      cmd = PS_SNOOP_IP_FLTR_DISABLED_CMD;
      break;

    case SNOOP_IP_COMP_ON_EV:
      /*---------------------------------------------------------------------
        First VJ compressed packet seen on this device, start snooping VJCH
      ---------------------------------------------------------------------*/
      cmd = PS_SNOOP_IP_COMP_ON_CMD;
      break;

    case SNOOP_IP_COMP_OFF_EV:
      /*---------------------------------------------------------------------
        All sockets doing VJCH have closed, don't snoop VJCH anymore
      ---------------------------------------------------------------------*/
      cmd = PS_SNOOP_IP_COMP_OFF_CMD;
      break;

    default:
      LOG_MSG_ERROR_1("Snoop IP invalid event %d", event );
      ASSERT(0);
      return;
  } /* switch(event) */

  /*-------------------------------------------------------------------------
    Now post the cmd and dev (=SM instance) to be executed in PS context
  -------------------------------------------------------------------------*/
  ps_send_cmd(cmd, (void *)dev);

  return;
} /* snoop_ip_post_event() */

/*===========================================================================
  FUNCTION SNOOP_IPI_REG_IP_VJCH_VJUCH()

  DESCRIPTION
    This function registers the callbacks for snooping IP, VJCH and VJUCH 
    packets.

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static void snoop_ipi_reg_ip_vjch_vjuch
(
  ppp_dev_enum_type dev    /* device instance of SM */
)
{
  /*-------------------------------------------------------------------------
    Start snooping for all three packet types (IP, VJCH, VJUCH)
  -------------------------------------------------------------------------*/
  snpipi_cb_arr[dev].tag_ip = snoop_reg_event(
                                dev,
                                PPP_IP_PROTOCOL,
                                SNOOP_ANY_MSG,
                                snoop_ipi_process_ip_cb
                              );

  snpipi_cb_arr[dev].tag_vjuch = snoop_reg_event(
                                   dev,
                                   PPP_VJUCH_PROTOCOL,
                                   SNOOP_ANY_MSG,
                                   snoop_ipi_process_vjuch_cb
                                 );

  snpipi_cb_arr[dev].tag_vjch = snoop_reg_event(
                                  dev,
                                  PPP_VJCH_PROTOCOL,
                                  SNOOP_ANY_MSG,
                                  snoop_ipi_process_vjch_cb
                                );
}

