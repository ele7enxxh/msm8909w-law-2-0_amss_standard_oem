#ifndef HDRLMAC_H
#define HDRLMAC_H
/*===========================================================================

  L I N K   M A N A G E M E N T  A N D   C O N T R O L   P R O T O C O L
                           D E F I N I T I O N S

DESCRIPTION
  This contains the declarations for the LMAC Protocol.

Copyright (c) 2000 - 2015 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/hdr/mac/api/hdrlmac.h#1 $ $DateTime: 2016/12/13 07:58:05 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
07/15/16   sjj     FR 26262 : LTE-U band support
05/07/15   vko     Inform srch if D2L reselection is aborted.
02/02/15   vko     Pass srch window param during fastacq during cgi req
09/09/14   vke     Redesign fix for connection close during QTA gap
08/28/14   wsh     RTT:Updated RTT threshold, used seperate hysteresis flags
08/22/14   vke     Delay connection close during QTA gap
08/12/14   wsh     Enable RTT boost feature by default
08/12/14   wsh     Disable RTT in TAP call; force HiCap if boost, new threshold 
08/07/14   mbs     Added support for trm lock exchange during IRAT procedures
08/01/14   vko     Ignore fast connect TCA, when tune away already started
07/21/14   sat     Mainlined hybrid_during_overhead to always TRUE
06/25/14   wsh     Fixed compile issue on Jolokia due to missing FW API 
06/09/14   kss     Added support for handling TX_RESET_REQ_IND processing.
06/13/14   vlc     Added HDRLMAC_USER_MAC_DEBUG_DISABLE_ENH_DLNA. 
05/05/14   cnx     Update NSET and IDLE channel change in one command.
04/03/14  kss/wsh  Added T2P boost for RTT reduction
03/14/14  vko/dsp  FR:19338:Reduce call setup time by skipping RF Tx shutdown.
07/19/13   arm     Merged DSDS feature for triton.
11/29/13   sat     NV control for Lock Release Enhancement (LRE)
10/30/13  kss/rmv  FR3272 - Modem API for Cost of Power estimate changes 
12/23/13   cnx     Notify SRCH about Sector ID change.
11/27/13  dsp/cnx  FR17482 - Page blocking management.
10/10/13   rmg     Added T2P boost support.
07/23/13   rmg     Enabled D-LNA by default.
05/24/13   rmg     Added NV support for D-LNA.
04/10/13   smd     Stop using debug mask NV to control DTX and FlexConn. 
01/05/13   smd     Flexconn and dtx NV disabled by default for DIME.
12/20/12   vko     Fixed compiler warnings
10/31/12   smd     Enabled FlexConn and DTX by default for 4.1 build too.
09/18/12   smd     Disabled FlexConn and DTX by default for 4.1 build.
09/05/12   smd     Enabled FlexConn and DTX by default.
06/20/12   smd     Supported Device only DTX and FlexCONN.
03/08/12   wsh     Merged NLB support (several fixes)
03/08/12   wsh     Merged NLB support
12/05/11   kss     Remove unnecessary include of hdrlmaci.h.
11/18/11   kss     Default to hybrid access during overhead behavior.  
10/28/11   pba     Optimized handoff phase 2 support
10/10/11   arm     Supported second chain test call feature. 
09/01/11   lyl     Mainlined hdrlmac_get_user_mac_debug_mask().
05/31/11   cnx     Added hdrlmac_get_subpkt_protocol_data(). 
05/05/11   cnx     Added hdrlmac_get_band_subclass().
03/05/11   arm     New API added to provide Rx and Tx power info to CM.
11/18/10   kss     Added hdrlmac_get_user_mac_debug_mask(). 
09/02/10   pba     Added support for eHRPD to LTE reselection phase 1
08/26/10   cnx     Added Diag cmd to change diversity preference in runtime. 
08/06/10   rkc     Added hdrlmac_get_state() and added hdrlmac_state_enum_type
06/10/10   pba     IRAT Measurement feature.
03/24/10   pxu     Removed various FEATURE_HDR_QSM features 
03/02/10   pba     Added support for LTE to HDR reselection
12/04/09   sju     Addd support for back to back TCAs. 
03/23/09   wsh     CMI: featurized CMI under FEATURE_CMI
01/27/09   sju     Added support for intra scc sleep.
11/10/08   ljl     Added hdrlmac_acmac_disable_ta().
08/15/08   wsh     Fixed crash if TCA contains invalid channel
07/01/08   ljl     Added functions to get rx/tx frequency.
06/09/08   etv     Used sys_channel_type instead of uint16 for carrier_map type
05/14/08   etv     Return num subactive sets from hdrlmac_get_carrier_map.
05/09/08   sju     Added support to remove avoided channels from NSET
02/22/08   mpa     Fixed warning
02/20/08   ljl     Removed the parameter from hdrlmac_get_rssi().
02/15/08   wsh     Fixed rare crash caused by sending back-to-back AC msg in 
                   PSIST_NO_ACCCESS scenario
10/31/07   sju     Added FEATURE_HDR_REVB_7500
10/04/07   cws     Renamed parameters for consistency.
10/02/07   cws     Added HDRLMAC_INVALID_CARRIER_INDEX.
09/17/07   cws     Added get_carrier_map and abstracted some funcs for TAP.
08/15/07   sju     Added HDRLMAC_MAX_NUM_DEMOD_CARRIERS macro
08/08/07   mpa     Added hdrlmac_get_tc_mode().
07/18/07   sju     Added hdrlmac_get_schdgrp_info()
07/13/07   sju     Added support for RevB phase1.5
04/27/07   grl     Removed tc_mode from TC assignment for access hybrid ctrl.
02/13/07   mt      Added hdrlmac_rup_request().
10/06/06   grl     Added passing of the negotiated SCI value to hdrsrchsleep.
08/28/06   sq      Added FEATURE_HDR_QSM
05/22/06   ljl     Added hdrlmac_set_tap().
03/06/06   sq      Added hdrlmac_set_bcmcs_sleep_cycle
02/16/06   jyw     Added command routing to disable the TC ofs for TAP calls.
01/15/05   jyw     Added support to query of the exist of multiple rx chain.
12/15/05   mpa     Added channel_change_in_sleep flag to idle channel change cmd.
09/10/05   grl     Added dynamic TC mode support for HPTS mode.
07/28/05   sq      Added hdrlmac_set_bcmcs_page_cycle
07/11/05   dna     Report channel list and cached sectors with neighbor list.
07/07/05   dna     Added support for Enhanced Idle State protocol
           dna     Add support for Rel. A TCA message fields
11/03/04   mpa     Removed dependency on std.h (merged with sys.h)
07/30/04    sq     Added function hdrlmac_update_bc_info.
06/09/04    sq     Added function hdrlmac_get_pn_ecio().
06/08/04    sq     Added function hdrlmac_get_asp_pn_mapping().
04/02/04    sq     Returns Ec/Io and Io for HDR
02/27/04   mpa     Added function hdrlmac_color_code_update().
10/13/03    km     Added function hdrlmac_advice_unlock_rf
01/23/03   kss     Added function hdrlmac_get_data_indicator().
11/15/02   aaj     Support for acquisition modes
10/29/02   sq      Added function hdrlmac_get_rssi().
07/26/02   dna     New interface to inform srch of tc_established after tcc.
05/06/02   dna     Whether or not to tune away to 1x during HDR connection
03/29/02   dna     Take sleep duration as a parameter to hdrlmac_sleep()
05/08/01   dna     Re-naming for consistency.
03/02/01   aaj     removed unused HDRLMAC_SET_PARAMS function
09/08/00   dna     Created module

===========================================================================*/

/*===========================================================================

                    INCLUDE  FILES  FOR  MODULE

===========================================================================*/

#include "hdr_variation.h"
#include "customer.h"
#ifdef FEATURE_HDR_TO_LTE
#include "hdraddr.h"
#endif /* FEATURE_HDR_TO_LTE */
#include "hdrhai.h"
#include "hdrsrchtypes.h"
#include "sys.h"

#ifdef FEATURE_CMI
#include "sys_v.h"
#endif

#include "qw.h"
#include "hdrdec.h"
#include "hdrscmgmmcdp.h"
#include "hdrrup.h"
#include "hdrlog.h"

/* <EJECT> */
/*===========================================================================

             DEFINITIONS  FROM  THE  HDR  AIR  INTERFACE  SPEC

===========================================================================*/


/* <EJECT> */
/*===========================================================================

                 DEFINITIONS  FOR MESSAGE PARSING
             
===========================================================================*/


/* <EJECT> */
/*===========================================================================

                        DATA  DECLARATIONS

===========================================================================*/
#define HDRLMAC_MAX_NUM_DEMOD_CARRIERS  HDRSRCH_MAX_DEMOD_CARRIERS_CNT

/* LMAC User MAC Debug FAC Gain Calc NV control bit fields */

/* Bit  0:     Dynamic LNA and Enhanced Dynamic LNA enable/disable
               0  - enable (default)
               1  - disable         
   Bit  1:     FAC Gain enable/disable
               0  - enable (default)
               1  - disable
   Bit  2:     FAC Gain initial value enable/disable
               0  - disable (default)
               1  - enable
   Bits 3:4    T2P Boost
               0 - 7.5  dB boost (default)
               1 - 8  dB boost
               2 - 7 dB boost
               3 - no boost
   Bits 5:6    T2P boost Y1
               0 - 16 dB y1, 8 dB RTT start threshold (default)
               1 - 14 dB y1, 10 dB RTT start threshold
               2 - 12 dB y1, 12 dB RTT start threshold
               3 - 10 dB y1, 14 dB RTT start threshold
   Bit 7:      Dynamic LNA
               0 - enable (default)
               1 - disable
   Bits 23:8   FAC Gain initial value
               16 bit initial value for FAC Gain
   Bit 24      Pre-tuneaway optimization
               0 - enable (default)
               1 - disable
   Bit 25      Post-tuneback optimization
               0 - enable (default)
               1 - disable
   Bit 26      Permitted payload optimization
               0 - enable (default)
               1 - disable
   Bit 27      Skipping RF TX shutdown optimization
               0 - enable (default)
               1 - disable
   Bit 28      RTT Reduction optimization
               0 - disabled (default)
               1 - enabled
   Bit 30:29   RTT Reduction T2P Deboost
               0 - 3.75 dB (default)
               1 - 4 dB
               2 - 3.5 dB
               3 - 0 dB
   Bit 31:     Lock Release Enhancement
               0 - enable (default)
               1 - disable 
*/

/* FAC Gain control masks */
#define HDRLMAC_USER_MAC_DEBUG_DISABLE_FAC_GAIN_CALC_V  0x00000002
#define HDRLMAC_USER_MAC_DEBUG_FAC_GAIN_INITIAL_VALUE_M 0x00FFFF00
#define HDRLMAC_USER_MAC_DEBUG_FAC_GAIN_INITIAL_VALUE_S 8
#define HDRLMAC_USER_MAC_DEBUG_ENABLE_FAC_GAIN_INIT_V   0x00000004

/* LRE control mask */
#define HDRLMAC_USER_MAC_DEBUG_DISABLE_LRE              0x80000000

/* Skipping RF TX shutdown control mask */
#define HDRLMAC_USER_MAC_DEBUG_SKIP_DISABLE_TX          0x08000000

#ifdef FEATURE_HDR_DLNA
#define HDRLMAC_USER_MAC_DEBUG_DISABLE_DLNA             0x00000080
#define HDRLMAC_USER_MAC_DEBUG_DISABLE_ENH_DLNA         0x00000001
#endif /* FEATURE_HDR_DLNA */

#ifdef FEATURE_HDR_RMAC_TUNEAWAY_OPTIMIZATION_T2P_BOOST
/* T2P boost gain mask and shift value */
#define HDRLMAC_USER_MAC_DEBUG_T2P_BOOST_M              0x18
#define HDRLMAC_USER_MAC_DEBUG_T2P_BOOST_S              3

/* Pre-tuneaway optimization disable mask */
#define HDRLMAC_USER_MAC_DEBUG_PRE_TA_OPT_DIS           0x01000000

/* Post-tuneback optimization disable mask */
#define HDRLMAC_USER_MAC_DEBUG_POST_TB_OPT_DIS          0x02000000

/* Permitted payload optimization disable mask */
#define HDRLMAC_USER_MAC_DEBUG_PERM_PAYLD_OPT_DIS       0x04000000

#ifdef FEATURE_HDR_RMAC_RTT_REDUCTION_T2P_BOOST
/* RTT reduction T2P boostoptimization disable mask */
#define HDRLMAC_USER_MAC_DEBUG_RTT_REDUCT_OPT_DIS       0x10000000

#define HDRLMAC_USER_MAC_DEBUG_RTT_REDUCT_DEBOOST_M     0x60000000
#define HDRLMAC_USER_MAC_DEBUG_RTT_REDUCT_DEBOOST_S     29

#define HDRLMAC_USER_MAC_DEBUG_RTT_REDUCT_Y1_M          0x60
#define HDRLMAC_USER_MAC_DEBUG_RTT_REDUCT_Y1_S          5
#endif /* FEATURE_HDR_RMAC_RTT_REDUCTION_T2P_BOOST */

#endif /* FEATURE_HDR_RMAC_TUNEAWAY_OPTIMIZATION_T2P_BOOST */
/*--------------------------------------------------------------------------
  Structure to map sub-activeset information to carrier.
--------------------------------------------------------------------------*/
#ifdef FEATURE_HDR_IS890

#define HDRLMAC_INVALID_CARRIER_INDEX   0xFF
  /* Indicates that a carrier is not in use */

#define HDRLMAC_INVALID_CHANNEL         0xFFFF
  /* Invalid channel for all band classes */

typedef enum
{
  HDRLMAC_INACTIVE_STATE        = 0x00,
    /* Searcher is idle ... */

  HDRLMAC_ACQ_STATE             = 0x01,
    /* Attempting to acquire a system */

  HDRLMAC_SYNC_STATE            = 0x02,
    /* Acquired ... waiting for SYNC message */

  HDRLMAC_IDLE_STATE            = 0x03,
    /* Idle state, included monitoring control channel */

  HDRLMAC_ACCESS_STATE          = 0x3b,
    /* Idle state during access attempts */

  HDRLMAC_CONNECTED_STATE       = 0x04
    /* Traffic */
} hdrlmac_state_enum_type;
  /* HDR LMAC state */

typedef struct
{ 
  uint8  carrier_index;
    /* Physical carrier index for map entry */

  sys_channel_type rev_channel;
    /* Reverse CDMA channel for map entry */

  sys_channel_type fwd_channel;
    /* Forward CDMA channel for map entry */
} 
hdrlmac_carrier_map_type;
  /* Used to map carrier index to sub-activeset index */

#endif /* FEATURE_HDR_IS890 */


typedef hdrsrch_rx_div_enum_type hdrlmac_rx_div_enum_type;
  /* Diversity preference type: div_off, div_on, second_chain_only */ 

/* <EJECT> */
/*===========================================================================

                      FUNCTION  DECLARATIONS

===========================================================================*/

/*===========================================================================

FUNCTION HDRLMAC_IND_CB

DESCRIPTION
  This function is registered to be called whenever a protocol gives an
  indication of interest to LMAC.

DEPENDENCIES
  None
 
PARAMETERS
  ind_name - Indication to process, is a global enum for all
                    possible indications in HDR signaling
  ind_data - Optional data pointer for indication specific data
 
RETURN VALUE
  None
  
SIDE EFFECTS
  None
===========================================================================*/

extern void hdrlmac_ind_cb 
(
  hdrind_ind_name_enum_type ind_name, 
  void *ind_data
);

/*===========================================================================

FUNCTION HDRLMAC_POWERUP_INIT

DESCRIPTION
  This function performs one-time powerup initialization required for HMP

DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

extern void hdrlmac_powerup_init( void );

/*===========================================================================

FUNCTION HDRLMAC_ACTIVATE

DESCRIPTION
  This function commands LMAC to activate.

DEPENDENCIES
  None

PARAMETERS
  sender - Protocol calling the function, for debug

RETURN VALUE
  None

SIDE EFFECTS

===========================================================================*/

extern void hdrlmac_activate (hdrhai_protocol_name_enum_type sender);

/*===========================================================================

FUNCTION HDRLMAC_DEACTIVATE

DESCRIPTION
  This function commands LMAC to deactivate.

DEPENDENCIES
  None

PARAMETERS
  sender - Protocol calling the function, for debug

RETURN VALUE
  None

SIDE EFFECTS

===========================================================================*/

extern void hdrlmac_deactivate (hdrhai_protocol_name_enum_type sender);

/*===========================================================================

FUNCTION HDRLMAC_SET_SEARCH_PARAMS

DESCRIPTION
  The protocol setting the parameters should keep the parameters stored in a 
  structure of hdrsrch_search_params_struct_type and call LMAC with the 
  pointer to that structure.  This command directly calls 
  hdrsrch_set_search_params() (IS-856) to give the parameters to HDRSRCH, 
  who copies them into its own local storage.
  
DEPENDENCIES
  None

PARAMETERS
  params_ptr - pointer to searcher parameters
  sender - Protocol calling the function, for debug

RETURN VALUE
  None

SIDE EFFECTS

===========================================================================*/

extern void hdrlmac_set_search_params 
(
  hdrsrch_search_params_struct_type * params_ptr,
  hdrhai_protocol_name_enum_type sender
);

/*===========================================================================

FUNCTION HDRLMAC_SET_MGMT_SAME_CHANNEL_PARAMS

DESCRIPTION
  This function sets new parameters for the current channel to searcher.
  
DEPENDENCIES
  None

PARAMETERS
  params_ptr - pointer to searcher parameters
  sender - Protocol calling the function, for debug

RETURN VALUE
  None

SIDE EFFECTS

===========================================================================*/

extern void hdrlmac_set_mgmt_same_channel_params 
(
  hdrsrch_chan_mgmt_params_struct_type * params_ptr,
  hdrhai_protocol_name_enum_type sender
);

/*===========================================================================

FUNCTION HDRLMAC_SET_MGMT_DIFF_CHANNEL_PARAMS

DESCRIPTION
  This function sets new parameters for a different channel to searcher.
  
DEPENDENCIES
  None

PARAMETERS
  params_ptr - pointer to searcher parameters
  sender - Protocol calling the function, for debug

RETURN VALUE
  None

SIDE EFFECTS

===========================================================================*/

extern void hdrlmac_set_mgmt_diff_channel_params 
(
  hdrsrch_chan_mgmt_params_struct_type * params_ptr,
  hdrhai_protocol_name_enum_type sender
);


#ifdef FEATURE_HDR_BCMCS
/*===========================================================================

FUNCTION HDRLMAC_SET_BCMCS_PAGE_CYCLE

DESCRIPTION
  This command calls hdrsrch_set_bcmcs_page_cycle() to update the BCMCS page
  cycle value.

DEPENDENCIES
  None

PARAMETERS
  page_cycle    - The BCMCS paging channel period which the AT monitors, in
                  units of SCC ( 256 slots ).

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

void hdrlmac_set_bcmcs_page_cycle
(
  uint8 page_cycle
    /* In units of SCC ( 256 slots )  */
);


/*===========================================================================

FUNCTION HDRLMAC_SET_BCMCS_SLEEP_CYCLE

DESCRIPTION
  This command calls hdrsrch_set_bcmcs_sleep_cycle() to update the BCMCS sleep
  cycle value.

DEPENDENCIES
  None

PARAMETERS
  sleep_cycle   - The BCMCS sleep period, in units of SCC ( 256 slots ).

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

void hdrlmac_set_bcmcs_sleep_cycle
(
  uint8 sleep_cycle
    /* In units of SCC ( 256 slots )  */
);

#endif /* FEATURE_HDR_BCMCS */


/*===========================================================================

FUNCTION HDRLMAC_SET_CCC

DESCRIPTION
  This command calls hdrsrch_set_control_channel_cycle() to update the control
  channel cycle value.  This is a separate command from set parameters because
  the control channel cycle value is generated at a different time than the
  rest of the parameters.

DEPENDENCIES
  None

PARAMETERS
  ccc    - The paging channel slot which the AT hashed to.
  max_sleep_cycle - Maximum sleep cycle duration negotiated (SlotCycle3)
  sender - Protocol calling the function, for debug

RETURN VALUE
  None

SIDE EFFECTS

===========================================================================*/

extern void hdrlmac_set_ccc 
(
  uint8 ccc, 
  uint8 max_sleep_cycle,
  hdrhai_protocol_name_enum_type sender
);

/*===========================================================================

FUNCTION HDRLMAC_GET_PCCC

DESCRIPTION
  This command calls hdrsrch_get_preferred_control_channel_cycle() to get the
  preferred control channel cycle.  This function should only be called if the 
  1x page slot is known.  Otherwise the default HDR control channel cycle 
  should be used.  It is OK for the 1x system to assume a page slot based on
  IMSI_M when it is not sure, then give an indication if it acquires a system
  that uses IMSI_T.  This allows us to set the cycle value once during initial
  session negotiation and only change it in the rare case when IMSI_T is
  being used.

DEPENDENCIES
  None

PARAMETERS
  page_slot   - Assigned slot in IS-95A/B or IS2000 1x system
  max_hdr_slot_cycle - Largest slot cycle currently configured for HDR
  default_ccc - Default control channel index calculated by hash function, 
                given the maximum sleep cycle
  sender      - Protocol calling the function, for debug

RETURN VALUE
  None

SIDE EFFECTS

===========================================================================*/

extern uint8 hdrlmac_get_pccc
(
  uint16                          page_slot,
  uint8                           max_hdr_slot_cycle,
  uint8                           default_ccc,
  hdrhai_protocol_name_enum_type  sender
);

/*===========================================================================

FUNCTION HDRLMAC_ACQUIRE

DESCRIPTION
  The caller passes in the channel to acquire.  LMAC directly calls
  hdrsrch_acquire(), passing in the channel and two callback functions.

DEPENDENCIES
  None

PARAMETERS
  channel - The band and channel to attempt acquisition on
  mode    - Mode of acquisition (deep, shallow, full, etc.)
  pn_list_size - Number of pn's in list
  pilot_pn - Pointer to HDR pilot pn
  time_allowed_ms - Maximum time allowed by LTE for DO to do brute-force acq 
                    and sync
  sender - Protocol calling the function, for debug

RETURN VALUE
  None

SIDE EFFECTS

===========================================================================*/

void hdrlmac_acquire
(
  sys_channel_type channel,
  const sys_hdr_acq_mode_e_type mode,
#ifdef FEATURE_LTE_TO_HDR
  uint8 pn_list_size,
  sys_pilot_pn_type *pilot_pn,
  uint16 time_allowed_ms,
  hdrsrch_acq_reason_enum_type reason,
#endif /* FEATURE_LTE_TO_HDR */
  hdrhai_protocol_name_enum_type sender
);

/*===========================================================================

FUNCTION HDRLMAC_SYNCHRONIZE

DESCRIPTION
  LMAC is a passthrough interface for time synchronization.  It directly calls 
  hdrsrch_synchronize().

DEPENDENCIES
  None

PARAMETERS
 link_id - The link ID associated with the received sync message (not
           currently used)
 pilot_pn - The pilot PN that this sector is using
 sys_time - The time specified in the sync message, in 80ms units from 
            UTC=0 (00:00:00 Jan 6,  1980 GMT)
 half_slot - The half-slot the synchronous control channel capsule started on.
  sender - Protocol calling the function, for debug

RETURN VALUE
  None

SIDE EFFECTS

===========================================================================*/

extern void hdrlmac_synchronize 
(
  hdrsrch_link_id_type link_id,
  sys_pilot_pn_type pilot_pn,
  qword sys_time, 
  hdrdec_half_slot_time_type half_slot,
  hdrhai_protocol_name_enum_type sender
);

/*===========================================================================

FUNCTION HDRLMAC_IDLE_MODE
s
DESCRIPTION
  When commanded to Idle Mode, LMAC directly calls hdrsrch_idle_mode() passing 
  a callback function.

DEPENDENCIES
  None

PARAMETERS
  sender - Protocol calling the function, for debug

RETURN VALUE
  None

SIDE EFFECTS

===========================================================================*/

extern void hdrlmac_idle_mode (hdrhai_protocol_name_enum_type sender);


#ifdef FEATURE_HDR_BCMCS

/*===========================================================================

FUNCTION HDRLMAC_UPDATE_BC_INFO

DESCRIPTION

DEPENDENCIES
  None

PARAMETERS

RETURN VALUE
  None

SIDE EFFECTS

===========================================================================*/

extern void hdrlmac_update_bc_info ( boolean );

#endif /* FEATURE_HDR_BCMCS */

/*===========================================================================

FUNCTION HDRLMAC_NSET_UPDATE

DESCRIPTION
  The protocol who receives the neighbor list should keep the list stored in 
  an array of hdrsrch_nset_pilot_type structures.  If a new neighbor list is 
  received, the caller must first copy each pilot PN and channel from the 
  message into a hdrsrch_nset_pilot_type structure, then call LMAC and pass 
  the pointer to the head of the array and the total number of neighbors in 
  the list.  This command directly calls hdrsrch_nset_update() to give the 
  list to HDRSRCH.

DEPENDENCIES
  None

PARAMETERS
  num_nset_pilots - Number of pilots to add to neighbor list
  nset_list_ptr - Pointer to an array of nset structures
  num_channels - Number of channels in the channel list
  channel_list_ptr - Channel fields from the SectorParameters message
  num_sectors - Number of sectors in the overehad cache
  sector_list_ptr - List of sectors currently in the overhead cache
  update_type - NSET information from a BROADCAST or DIRECTED message?
  sender - Protocol calling this function, for debug.
  serving_pilot_gid - The group id of the pilot from which the nset update is received 

RETURN VALUE
  None

SIDE EFFECTS

===========================================================================*/

void hdrlmac_nset_update
(
  uint8                           num_nset_pilots,
  hdrsrch_nset_pilot_type         *nset_list_ptr,
  uint8                           num_channels,
  const sys_channel_type          *channel_list_ptr,
  uint8                           num_sectors,
  sys_link_type                   *sector_list_ptr,
  const uint8                     num_avoided_channels,
  const sys_avoided_channel_record_type   *avoided_list_ptr,
  hdrsrch_nset_update_enum_type   update_type,
  hdrhai_protocol_name_enum_type  sender,
  hdrsrch_pilot_gid_type          serving_pilot_gid
);

/*===========================================================================

FUNCTION HDRLMAC_IDLE_CHAN_CHANGE 

DESCRIPTION
  This command requires special processing by LMAC, so it is queued and 
  ndled in LMAC task context.

DEPENDENCIES
  None

PARAMETERS
  link - The band, channel and pilot to change channel to
  channel_change_in_sleep - channel change requested while sleeping
  sender - Protocol calling the function, for debug

RETURN VALUE
  None

SIDE EFFECTS

===========================================================================*/

extern void hdrlmac_idle_chan_change 
(
  sys_link_type *link_ptr,
  boolean channel_change_in_sleep,
  hdrhai_protocol_name_enum_type sender
);

/* <EJECT> */
/*===========================================================================

FUNCTION HDRLMAC_IDLE_CHANNEL_CHANGE_AND_NSET_UPDATE

DESCRIPTION
  This command takes idle channel change and nset update parameters and
  passes it to SRCH.

DEPENDENCIES
  None

PARAMETERS
  num_nset_pilots - Number of pilots to add to neighbor list
  nset_list_ptr - Pointer to an array of nset structures
  num_channels - Number of channels in the channel list
  channel_list_ptr - Channel fields from the SectorParameters message
  num_sectors - Number of sectors in the overehad cache
  sector_list_ptr - List of sectors currently in the overhead cache
  num_avoided_channels - Number of systems in the avoided channel list
  avoided_list_ptr - List of avoided channels
  update_type - NSET information from a BROADCAST or DIRECTED message?
  serving_pilot_gid - The group id of the pilot from which the nset update is received 
  link - The band, channel and pilot to change channel to
  channel_change_in_sleep - channel change requested while sleeping
  sender - Protocol calling the function, for debug

RETURN VALUE
  None

SIDE EFFECTS

===========================================================================*/
void hdrlmac_idle_channel_change_and_nset_update
(
  uint8                           num_nset_pilots,
  hdrsrch_nset_pilot_type         *nset_list_ptr,
  uint8                           num_channels,
  const sys_channel_type          *channel_list_ptr,
  uint8                           num_sectors,
  sys_link_type                   *sector_list_ptr,
  const uint8                     num_avoided_channels,
  const sys_avoided_channel_record_type   *avoided_list_ptr,
  hdrsrch_nset_update_enum_type   update_type,
  hdrsrch_pilot_gid_type          serving_pilot_gid,
  sys_link_type                   *link_ptr,
  boolean                         channel_change_in_sleep,
  hdrhai_protocol_name_enum_type  sender
);

/*===========================================================================

FUNCTION HDRLMAC_SLEEP

DESCRIPTION
  The Idle State Protocol gives this command to put the HDR hardware to sleep.
  LMAC directly calls hdrsrch_sleep(), passing a callback function.  If we
  are not allowed to sleep at this time, HDRSRCH will call the callback
  function immediately.

DEPENDENCIES
  None

PARAMETERS
  hdr_slot_cycle - How long to sleep for
  hdr_neg_slot_cycle - What is the current negotiated SCI which can be
                       different from what the protocols are asking for
                       this particular sleep cycle.
  sender - Protocol calling the function, for debug
  intra_scc_start_slot_valid -  Flag used to indicate whether 
                               intra_scc_start_slot is valid 
  intra_scc_start_slot - CC start in slots. 
  qpch_enabled - whether QPCH is enabled or not.
  other_rat_tx_cycle - Indicates the transmission cycle of the 
                       OtherRATNeighborList message
  session_is_open - whether EVDO session is open
  ovhd_is_updated - whether OVHD info is updated

RETURN VALUE
  None

SIDE EFFECTS

===========================================================================*/

extern void hdrlmac_sleep
(
  uint8   hdr_slot_cycle,  /* e.g. 6: 426ms, 9: 5.12s, 12: 40.96s */
  uint8   hdr_neg_slot_cycle,
  hdrhai_protocol_name_enum_type sender,
  boolean intra_scc_start_slot_valid,
  uint16 intra_scc_start_slot,
  boolean qpch_enabled
#ifdef FEATURE_HDR_TO_LTE
  , uint8 other_rat_tx_cycle
#endif /* FEATURE_HDR_TO_LTE*/
#ifdef FEATURE_HDR_PAGE_BLOCKING_MGMT
  ,boolean session_is_open
  ,boolean ovhd_is_updated
#endif /* FEATURE_HDR_PAGE_BLOCKING_MGMT */
);

/*===========================================================================

FUNCTION HDRLMAC_ACCESS_MODE 

DESCRIPTION
  HMP gives this command prior to starting an access attempt.  HMP waits for 
  an indication that the LMAC has entered access mode before checking overhead 
  messages, etc., to make sure the pilot will not change out from under it. 
  While in Access Mode, HDRSRCH must call hdrlmac_request_aho_cb() if it 
  decides a pilot change in needed.  HDRSRCH can not autonomously change the 
  pilot while in access mode
  
  To HDRSRCH, the access mode command is also an implied wakeup command, if 
  HDRSRCH is asleep when the command is received.  In this case HDRSRCH must 
  call hdrlmac_wakeup_cb() prior to calling hdrlmac_suspend_ho_cb().

DEPENDENCIES
  None

PARAMETERS
  sender - Protocol calling the function, for debug

RETURN VALUE
  None

SIDE EFFECTS

===========================================================================*/

extern void hdrlmac_access_mode (hdrhai_protocol_name_enum_type sender);

/*===========================================================================

FUNCTION HDRLMAC_PERFORM_AHO 

DESCRIPTION
  This command requires special processing by LMAC, so it is queued and 
  handled in LMAC task context.
  
DEPENDENCIES
  None

PARAMETERS
  sender - Protocol calling the function, for debug

RETURN VALUE
  None

SIDE EFFECTS

===========================================================================*/

extern void hdrlmac_perform_aho (hdrhai_protocol_name_enum_type sender);

/*===========================================================================

FUNCTION HDRLMAC_PILOT_REPORT_RESET 

DESCRIPTION
  This command is a passthrough interface for resetting the pilot reporting 
  mechanism during connected state operation.  It directly calls 
  hdrsrch_pilot_report_reset().
DEPENDENCIES
  None

PARAMETERS
  sender - Protocol calling the function, for debug

RETURN VALUE
  None

SIDE EFFECTS

===========================================================================*/

extern void hdrlmac_pilot_report_reset (hdrhai_protocol_name_enum_type sender);

/*===========================================================================

FUNCTION HDRLMAC_TC_ASSIGN

DESCRIPTION
  This command requires special processing by LMAC, so it is queued and 
  handled in LMAC task context.
  
DEPENDENCIES
  None

PARAMETERS
  initial_assignment - Whether this is the initial channel assignment
  link_id - Link ID for which the pilot information is needed
  tc_tune_away - If 1x tune-away is allowed
  cc_subactive_set_idx - index of sub-active set which carries CC
  num_aset_pilots - Number of pilots to add to active list
  aset_list_ptr - Pointer to an array of aset structures
  sender - Protocol calling the function, for debug
  rup_subtype - MC or Default RUP

RETURN VALUE
  None

SIDE EFFECTS

===========================================================================*/

extern void hdrlmac_tc_assign 
(
  boolean initial_assignment,
  hdrsrch_link_id_type link_id,
  hdrsrch_traffic_tune_away_enum_type  tc_tune_away,
  uint8 cc_subactive_set_idx,
  hdrrup_tca_msg_type * tca_msg_ptr, 
  hdrhai_protocol_name_enum_type sender, 
  uint16 rup_subtype
);

/*===========================================================================

FUNCTION HDRLMAC_CHAN_MEAS

DESCRIPTION
  This command is a passthrough interface for requesting HDRSRCH to perform 
  a measurement on a list of channels.  It directly calls hdrsrch_chan_meas() 
  passing a callback function.
  
DEPENDENCIES
  None

PARAMETERS
  meas_method - Rx power or time correlation measurement
  chan_list_ptr - List of channels to measure
  meas_value_list_ptr - Pointer to array in which the lower layer is to place 
                        its measurements
  num_chans - Number of channels to be measured
  sender - Protocol calling the function, for debug

RETURN VALUE
  None

SIDE EFFECTS

===========================================================================*/

extern void hdrlmac_chan_meas 
(
  hdrsrch_cmeas_method_enum_type meas_method,
  sys_channel_type * chan_list_ptr,
  hdrsrch_cmeas_value_type * meas_value_list_ptr, 
  uint8 num_chans,
  hdrhai_protocol_name_enum_type sender
);

/*===========================================================================

FUNCTION HDRLMAC_TC_ESTABLISHED

DESCRIPTION
  This command is a passthrough interface for informing hdr searcher 
  that the traffic channel has been established.

DEPENDENCIES
  None

PARAMETERS
  sender - Protocol calling the function, for debug

RETURN VALUE
  None

SIDE EFFECTS

===========================================================================*/

extern void hdrlmac_tc_established (hdrhai_protocol_name_enum_type sender);

/*===========================================================================

FUNCTION HDRLMAC_GET_PILOTS

DESCRIPTION
  This is just a wrapper for hdrsrch_get_pilot_report().

DEPENDENCIES
  None

PARAMETERS
  max_pilots - Less than or equal to size of array pointed to by pilots_ptr
  pilots_ptr - Pointer to array of structures comprising all the active and 
               candidate set pilots.  The first pilot is the reference pilot.
  caller - Protocol calling the function, for debug

RETURN VALUE
  int - number of pilot records in the array pointed to by pilots_ptr

SIDE EFFECTS

===========================================================================*/

extern int hdrlmac_get_pilots 
( 
  int max_pilots, 
  hdrsrch_pilot_rpt_type * pilots_ptr,
  hdrhai_protocol_name_enum_type caller
);

/*==========================================================================

FUNCTION HDRLMAC_GET_SCHDGRP_INFO

DESCRIPTION
  Get the scheduler group report.

DEPENDENCIES

PARAMETERS

RETURN VALUE
  None

SIDE EFFECTS
  May cause an immediate reschedule of searches.

==========================================================================*/

extern void hdrlmac_get_schdgrp_info
( 
  hdrsrch_schdgrp_rpt_type *schdgrp_info_ptr 
);

/* <EJECT> */
/*===========================================================================

FUNCTION HDRLMAC_RUP_REQ

DESCRIPTION
  This is just a wrapper for hdrsrch_rup_req.

DEPENDENCIES
  None

PARAMETERS
  route_update_request_ptr - Pointer to structure that cotains the parameters 
         for the route update message from protocols.

RETURN VALUE
  None

SIDE EFFECTS

===========================================================================*/

extern void hdrlmac_rup_request
(
  hdrsrch_rup_req_params_type     *route_update_request_ptr
    /* pointer to RUP request params structure */
);


/* <EJECT> */
/*===========================================================================

FUNCTION HDRLMAC_CONFIGURE_AT_OPERATIONAL_MODE

DESCRIPTION
  This is just a wrapper for hdrsrch_configure_at_operational_mode.

DEPENDENCIES
  None

PARAMETERS
  config - which at operational mode algorithm if any to run.

RETURN VALUE

SIDE EFFECTS

===========================================================================*/

extern void hdrlmac_configure_at_operational_mode
(
  hdrsrch_opmode_config_enum_type      config
    /* which at operational mode algorithm if any to run */
);


/* <EJECT> */
/*===========================================================================

FUNCTION HDRLMAC_GET_STATE

DESCRIPTION
  This function returns the current HDR state stored in hdrlmac.state.

DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  HDR state (hdrlmac_state_enum_type)

SIDE EFFECTS
  None

===========================================================================*/

extern hdrlmac_state_enum_type hdrlmac_get_state( void );

/*===========================================================================

FUNCTION HDRLMAC_GET_LINK_INFO

DESCRIPTION
  This is just a wrapper for hdrsrch_get_link_info().  Currently the upper
  layers can not recover from the link ID not being found, so we ERR_FATAL
  if searcher returns an error.

DEPENDENCIES
  None

PARAMETERS
  link_id - The searcher-assigned ID for the link we want info about
  link_ptr - A pointer to a description of the link with the given link_id

RETURN VALUE
  None

SIDE EFFECTS
  Calls ERR_FATAL if the link ID is not recognized by the searcher.

===========================================================================*/

extern void hdrlmac_get_link_info 
(
  hdrsrch_link_id_type link_id,
  sys_link_type * link_ptr
);

/*=========================================================================

FUNCTION     : HDRLMAC_GET_RSSI

DESCRIPTION  : This function returns the RSSI estimate for HDR to be used
               by the User Interface to indicate quality of HDR service.

DEPENDENCIES : None

INPUT        : None

RETURN VALUE : Estimated HDR RSSI.

SIDE EFFECTS : None

=========================================================================*/

extern byte hdrlmac_get_rssi
(
  void 
);

/*=========================================================================

FUNCTION     : HDRLMAC_GET_ECIO_AND_IO

DESCRIPTION
  Returns HDR Ec/Io and Io to the caller. It returns the Io estimate made by 
  rx AGC loops in the DSP and the Ec/Io estimate found by the searcher.

  Ec/Io range: from 5 ( -31.5 dB ) to 512 ( 0 dB )
  Io    range: from -106 dBm to -21 dBm 
    
DEPENDENCIES
  None

INPUTS
  Pointers to the locations where EcIo and Io will be stored

RETURN VALUE
  EcIo and Io are returned through the passed pointers.

SIDE EFFECTS
  None

=========================================================================*/

extern void hdrlmac_get_ecio_and_io( int16*, int* );


/*=========================================================================

FUNCTION     : HDRLMAC_GET_DATA_INDICATOR

DESCRIPTION  : This function returns the data level indicator. The data rate
               that the AT can receive is divided into 9 levels, 0 to 8. Level
               8 indicates highest data rate and level 0 indicates almost no
               data rate.

DEPENDENCIES : None

INPUT        : None

RETURN VALUE : Data activity indicator between 0 to 8.

SIDE EFFECTS : None

=========================================================================*/

extern byte hdrlmac_get_data_indicator( void );


/* <EJECT> */
/*=========================================================================

FUNCTION     : HDRLMAC_GET_ASP_PN_MAPPING

DESCRIPTION  : This function returns the PN offsets and ASP indices of all 
               the active set pilots, in an array passed by the caller.

DEPENDENCIES : None

INPUT        : A pointer to an array of type hdrsrch_asp_to_pn_info_type.

RETURN VALUE : Number of ASET pilots, from 0 to 6.
               PN and ASP info will be updated in the array passed by caller.

SIDE EFFECTS : None

=========================================================================*/

extern uint8 hdrlmac_get_asp_pn_mapping( hdrsrch_asp_to_pn_info_type * );


/* <EJECT> */
/*=========================================================================

FUNCTION     : HDRLMAC_GET_PN_ECIO

DESCRIPTION  : This function returns the PN offsets and EcIo of all the 
               active set pilots, in an array passed by the caller. The
               range of EcIo returned is from 0 to 63, in -0.5db unit.

DEPENDENCIES : None

INPUT        : A pointer to an array of type hdrsrch_pn_ecio_info_type.

RETURN VALUE : Number of ASET pilots, from 0 to 6.
               PN and EcIo info will be updated in the array passed by caller.

SIDE EFFECTS : None

=========================================================================*/

extern uint8 hdrlmac_get_pn_ecio( hdrsrch_pn_ecio_info_type* );

/*=========================================================================

FUNCTION     : HDRLMAC_ADVICE_UNLOCK_RF

DESCRIPTION  : This function advices SEARCH that 1x needs the RF lock

DEPENDENCIES : None

INPUT        : None

RETURN VALUE : None

SIDE EFFECTS : None

=========================================================================*/

extern void hdrlmac_advice_unlock_rf( void );

/*=========================================================================

FUNCTION     : HDRLMAC_COLOR_CODE_UPDATE

DESCRIPTION  : This function informs the searcher of the color code for the
               current link

DEPENDENCIES : None

PARAMETERS   :

  link_info  - link info for the current sector
  color_code - color code of the current subnet

RETURN VALUE : None

SIDE EFFECTS : None

=========================================================================*/

extern void hdrlmac_color_code_update
(
  sys_link_type link_info,
  uint8 color_code
);

#ifdef FEATURE_HDR_TO_LTE
/* <EJECT> */
/*=========================================================================

FUNCTION     : HDRLMAC_UPDATE_SECTOR

DESCRIPTION  : This function informs the searcher of current sector ID

DEPENDENCIES : None

PARAMETERS   :
  sector_id  - current sector ID

RETURN VALUE : None 

SIDE EFFECTS : None

=========================================================================*/
extern void hdrlmac_update_sector
(
  hdraddr_type sector_id
);
#endif /* FEATURE_HDR_TO_LTE */

/*=========================================================================

FUNCTION     : HDRLMAC_SET_TC_MODE

DESCRIPTION  : This function changes the HDR traffic mode during the
               middle of a traffic connection. Current modes
               supported are:

               HDRSRCH_DEFAULT_TC_MODE   - Standard HDR traffic connection. 

               HDRSRCH_S1XHDR_HP_TC_MODE - High Priority Traffic Mode where
                                           HDR preempts 1x paging onto 
                                           chain 1 and preempts acquisition.

DEPENDENCIES : None

PARAMETERS   :

  tc_mode    - Traffic mode that HDR should transition to.

RETURN VALUE : None

SIDE EFFECTS : None

=========================================================================*/

extern void hdrlmac_set_tc_mode
( 
  hdrsrch_tc_mode_enum_type tc_mode
    /* Traffic mode that HDR should transition to. */
);

/*=========================================================================

FUNCTION     : HDRLMAC_GET_TC_MODE

DESCRIPTION  : This function retrieves the HDR traffic mode. 

               HDRSRCH_DEFAULT_TC_MODE   - Standard HDR traffic connection. 

               HDRSRCH_S1XHDR_HP_TC_MODE - High Priority Traffic Mode where
                                           HDR preempts 1x paging onto 
                                           chain 1 and preempts acquisition.

DEPENDENCIES : None

PARAMETERS   : None

RETURN VALUE : HDRSRCH_DEFAULT_TC_MODE   - Standard HDR traffic connection. 

               HDRSRCH_S1XHDR_HP_TC_MODE - High Priority Traffic Mode where
                                           HDR preempts 1x paging onto 
                                           chain 1 and preempts acquisition.
SIDE EFFECTS : None

=========================================================================*/

extern hdrsrch_tc_mode_enum_type hdrlmac_get_tc_mode (void);

/*=========================================================================

FUNCTION     : HDRLMAC_GET_DEMOD0_SASET_ID

DESCRIPTION  : This function retrieves the sub-active set id that was assigned
               to demod index 0.

DEPENDENCIES : None

PARAMETERS   : None

RETURN VALUE : sub-active id which was assigned to demode index 0 

SIDE EFFECTS : None

=========================================================================*/

extern uint8 hdrlmac_get_demod0_saset_id (void);

/*=========================================================================

FUNCTION     : HDRLMAC_HAS_MULTI_RX_CAPABILITY

DESCRIPTION  : This function returns about the rx chain configurations

DEPENDENCIES : None

PARAMETERS   :

  rx_cap    - rx chain configuration defined in the SCP layer.

RETURN VALUE : None

SIDE EFFECTS : None

=========================================================================*/

extern boolean hdrlmac_has_multi_rx_capability
( 
  hdrscmgmmcdp_multi_rx_capability_type rx_cap
    /* Multiple receiver capability */
);

/*=========================================================================

FUNCTION     : HDRLMAC_SET_TC_HANDDOWN_MODE

DESCRIPTION  : This function sets handdown mode during  HDR connected
               state. Handdown can be enabled or disabled 
               during HDR traffic connection.

  HDRSRCH_1X_HANDDOWN_DISABLED_MODE - Handdown to is 1x disabled.

  HDRSRCH_1X_HANDDOWN_ENABLED_MODE  - Handdown to 1x is enabled.

DEPENDENCIES : None

PARAMETERS   : None

RETURN VALUE : None

SIDE EFFECTS : None

=========================================================================*/

extern void hdrlmac_set_tc_handdown_mode
( 
  hdrsrch_tc_handdown_mode_enum_type   handdown_mode
    /* the handdown mode selected */
);

/* <EJECT> */
#ifdef FEATURE_HDR_IS890
/*=========================================================================

FUNCTION     : HDRLMAC_DISABLE_TC_OFS

DESCRIPTION  : This function is called if any application ( so far FTAP or
               RTAP ) wants to disable TC OFS. TC OFS is enabled by default 
               whenever HDRSRCH enters traffic state. 

DEPENDENCIES : None

PARAMETERS   : None

RETURN VALUE : None

SIDE EFFECTS : None

=========================================================================*/

extern void hdrlmac_disable_tc_ofs( void );


/*=========================================================================

FUNCTION     : HDRLMAC_SET_TAP

DESCRIPTION  : This function is called if any TAP application is running. 

DEPENDENCIES : None

PARAMETERS   : 

  tap -  whether TAP is running

RETURN VALUE : None

SIDE EFFECTS : None

=========================================================================*/

void hdrlmac_set_tap
(
  boolean                    tap
    /* Whether tap is running */
);

/*=========================================================================

FUNCTION     : HDRLMAC_CLEAR_HANDOFF_COUNT

DESCRIPTION  : 
  This function clears out HO counter accessed via 
  hdrlmac_get_handoff_count(). 

DEPENDENCIES : None

PARAMETERS   : 
  tca_subaset_idx - SubActiveSet index to clear.

RETURN VALUE : None

SIDE EFFECTS : 
  Updates hdrsrchlog with ho count information.

=========================================================================*/

void hdrlmac_clear_handoff_count
( 
  uint8 tca_subaset_idx
    /* SubActiveSet Index to clear */
);

/*=========================================================================

FUNCTION     : HDRLMAC_GET_HANDOFF_COUNT

DESCRIPTION  : 
  This function gets the number of handoffs since the last time 
  hdrlmac_clear_handoff_count was called. 

DEPENDENCIES : None

PARAMETERS   : 
  tca_subaset_idx - SubActiveSet index to get handoff count.

RETURN VALUE : 
  Number of handoffs since last hdrlmac_clear_handoff_count.

SIDE EFFECTS : None
  
=========================================================================*/

uint32 hdrlmac_get_handoff_count
( 
  uint8 tca_subaset_idx
    /* SubActiveSet Index to get handoff count */
);

/*=========================================================================

FUNCTION     : HDRLMAC_GET_CARRIER_MAP

DESCRIPTION  : 
  This function gets the carrier map from the most recent TCAM.

DEPENDENCIES : None

PARAMETERS   : 
  num_carriers - Maximum number of elements in the carrier_map array.
  carrier_map - Pointer to an array of hdrlmac_carrier_map_type.

RETURN VALUE :
  num_sub_asets - Number of Sub Active Sets.

SIDE EFFECTS : None
  
=========================================================================*/

uint8 hdrlmac_get_carrier_map
( 
  uint8                     num_carriers, 
  hdrlmac_carrier_map_type  *carrier_map 
);

#endif /* FEATURE_HDR_IS890 */

/*=========================================================================

FUNCTION HDRLMAC_DIVERSITY_COMB_ENABLED

DESCRIPTION 
  This function returns if diversity combining is
  enabled.

DEPENDENCIES
  None

RETURN VALUE
  True  - if diversity combining is enabled.
  False - Otherwise. 
  
SIDE EFFECTS
  None

=========================================================================*/

boolean hdrlmac_diversity_comb_enabled( void );

/* <EJECT> */
/*===========================================================================

FUNCTION HDRLMAC_RUNTIME_DIVERSITY_PREF_CTRL

DESCRIPTION
  This function sets the diversity preference at runtime.
 
  It is the wrapper of the api hdrsrch_runtime_diversity_pref_ctrl

DEPENDENCIES
  None

PARAMETERS
  div_ctrl - Whether diversity is on.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void hdrlmac_runtime_diversity_pref_ctrl
(
  hdrlmac_rx_div_enum_type       div_ctrl
    /* Runtime diversity control */ 
);

/*========================================================================

FUNCTION     : hdrlmac_is_in_access_state

DESCRIPTION  : This function returns if lmac is in access state

DEPENDENCIES : None

PARAMETERS   : None

RETURN VALUE : Whether LMAC is in access state

SIDE EFFECTS : None

=========================================================================*/
boolean hdrlmac_is_in_access_state( void );

/*===========================================================================

FUNCTION HDRLMAC_GET_TX_CARRIER_FREQ

DESCRIPTION
  This function returns the tx carrier freq in KHz for the band and channel
  specified in the parameters.

DEPENDENCIES
  None

PARAMETERS
  band       - Band
  chan_num   - Channel number

RETURN VALUE
  Freq in KHz.

SIDE EFFECTS
  None

===========================================================================*/
uint32 hdrlmac_get_tx_carrier_freq
(
  sys_band_class_e_type      band,
    /* Band */

  sys_channel_num_type       chan_num
    /* Channel number */
);


/*===========================================================================

FUNCTION HDRLMAC_GET_RX_CARRIER_FREQ

DESCRIPTION
  This function returns the rx carrier freq in KHz for the band and channel
  specified in the parameter.

DEPENDENCIES
  None

PARAMETERS
  band       - Band
  chan_num   - Channel number

RETURN VALUE
  Freq in KHz.

SIDE EFFECTS
  None

===========================================================================*/
uint32 hdrlmac_get_rx_carrier_freq
(
  sys_band_class_e_type      band,
    /* Band */

  sys_channel_num_type       chan_num
    /* Channel number */
);

/*=========================================================================

FUNCTION HDRLMAC_IS_BAND_CHAN_SUPPORTED

DESCRIPTION 
  This function checks if the band and channel is support by RF driver.

DEPENDENCIES
  None

RETURN VALUE
  TRUE - If band/channel is supported.
  FALSE - otherwise.
  
SIDE EFFECTS
  None.

=========================================================================*/

boolean hdrlmac_is_band_chan_supported 
( 
  sys_band_class_e_type   band, 
    /* Band */
  word                    chan
    /* Channel number */
);


/*============================================================================

FUNCTION HDRLMAC_GET_BAND_SUBCLASS

DESCRIPTION
  This function gets the currently supported band subclasses of the given band.
  It is a wrapper of function hdrsrch_get_band_subclass().
  
DEPENDENCIES
  None
  
RETURN VALUE
  The mask of the current subclasses of the given band.

SIDE EFFECTS
  None

============================================================================*/

uint32 hdrlmac_get_band_subclass
(
  uint8    band_class
    /* Band Class */
);


#ifdef FEATURE_HDR_HYBRID_ACCESS
/*===========================================================================

FUNCTION HDRLMAC_ACMAC_DISABLE_TA

DESCRIPTION
  This function notifies HDR SRCH that ACACK has been received and 
  disables tune away for the duration specificed by no_ta_duration ms.
  
DEPENDENCIES
  None

PARAMETERS
  no_ta_duration - The duration that tune away should be disabled in ms.

RETURN VALUE
  None

SIDE EFFECTS
  None
  
===========================================================================*/

void hdrlmac_acmac_disable_ta ( uint16 no_ta_duration );
#endif /* FEATURE_HDR_HYBRID_ACCESS */

/*===========================================================================

FUNCTION HDRLMAC_CLEAR_PENDING_TCA_QUEUE

DESCRIPTION
  Clears pending TCA queue. 

DEPENDENCIES
  None

PARAMETERS
  None
                     
RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void hdrlmac_clear_pending_tca_queue( void );

#ifdef FEATURE_LTE_TO_HDR
/*===========================================================================

FUNCTION HDRLMAC_TIME_TRANSFER

DESCRIPTION
  This function allows HDR timing information provided to HDR MC by other RAT 
  to be transferred to HDR searcher.
  
DEPENDENCIES
  None

PARAMETERS
  lte_ostmr - LTE timing information.
  cdma_sys_time - CDMA system time.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void hdrlmac_time_transfer
(
  uint32 lte_ostmr,
  uint64 cdma_sys_time
);

/*===========================================================================

FUNCTION HDRLMAC_TIME_TRANSFER_DONE_CB

DESCRIPTION
  This function is called by searcher once the timing transfer is completed.
  
DEPENDENCIES
  None

PARAMETERS
  tran_sys_time_result - Time transfer success or failure status

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void hdrlmac_time_transfer_done_cb
(
  hdrsrch_tran_sys_time_enum_type tran_sys_time_result
);

/*===========================================================================

FUNCTION HDRLMAC_FAST_ACQUISITION

DESCRIPTION
  This function is called by HDRMC to initiate a fast acquisition procedure.
  
DEPENDENCIES
  HDR timing information should have already been provided to searcher.

PARAMETERS
  channel - Channel to acquire.
  pilot_pn - Pilot pn to acquire. 
  fast_acq_reason - reason for fast acqusition (reselection or active ho)

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void hdrlmac_fast_acquisition
( 
  sys_channel_type channel,
  sys_pilot_pn_type pilot_pn
#ifdef FEATURE_LTE_TO_HDR_OH
  ,hdrsrch_fast_acq_reason_enum_type fast_acq_reason
  ,uint8 srch_window_size
#endif /* FEATURE_LTE_TO_HDR_OH */
);

/*===========================================================================

FUNCTION HDRLMAC_FAST_ACQUISITION_DONE_CB

DESCRIPTION
  This function is called by searcher once the fast acquisition is completed.
  
DEPENDENCIES
  None

PARAMETERS
  fast_acq_result - Fast Acquisition success or failure status

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void hdrlmac_fast_acquisition_done_cb
(
  hdrsrch_fast_acq_enum_type       fast_acq_result
);
#endif /* FEATURE_LTE_TO_HDR */

#ifdef FEATURE_IRAT_PILOT_MEAS
/*===========================================================================

FUNCTION HDRLMAC_IRAT_PILOT_MEASUREMENTS_REQ

DESCRIPTION
  This function is called when IRAT pilot measurements are required 
  from searcher.
  
DEPENDENCIES
  None

PARAMETERS
  pilot_meas_req - DO Pilots for which measurement is requetsed.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void hdrlmac_irat_pilot_measurements_req
(   
  hdrsrch_pilot_meas_req_params_type pilot_meas_req
);
#endif /* FEATURE_IRAT_PILOT_MEAS */

#ifdef FEATURE_HDR_TO_LTE
/*===========================================================================

FUNCTION HDRLMAC_LTE_NSET_UPDATE

DESCRIPTION
  This function is called to update the searcher with LTE nset.
  
DEPENDENCIES
  None

PARAMETERS
  lte_nset_update_params - LTE frequency information.
  lte_band_pref - LTE band pref info from SD.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void hdrlmac_lte_nset_update
(   
  hdrsrch_lte_nset_update_params_type   *lte_nset_update_params,
#if (LTE_BAND_NUM == 256)
  sys_lte_band_mask_e_type lte_band_pref
#else
  sys_band_mask_type lte_band_pref
#endif /* (LTE_BAND_NUM == 256) */
);

/*===========================================================================

FUNCTION HDRLMAC_ABORT_RESELECT_TO_LTE

DESCRIPTION
  This function is called to inform the searcher that D2L reselection is 
  being aborted.
  
DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void hdrlmac_abort_reselect_to_lte( void );
#endif /* FEATURE_HDR_TO_LTE */

/*===========================================================================

FUNCTION HDRLMAC_GET_USER_MAC_DEBUG_MASK

DESCRIPTION
  This function retrieves the HDR MAC debug mask from the hdrl1_dbg_msk
  NV item.

DEPENDENCIES
  None

PARAMETERS
  mac_user_debug_mask - filled in with value from NV

RETURN VALUE
  Whether debug mask is valid.

SIDE EFFECTS
  None.

===========================================================================*/

extern boolean hdrlmac_get_user_mac_debug_mask
(
  uint64 * mac_debug_mask
    /* Mac debug mask from NV */
);

/*=========================================================================

FUNCTION HDRLMAC_GET_SUBPKT_PROTOCOL_DATA

DESCRIPTION
  This function gets value for the fields, which are related to HDRMAC, in
  subpacket protocol data. 

DEPENDENCIES
  None

PARAMETERS
  subpkt_ptr - pointer to the subpacket to be set

RETURN VALUE
  None

SIDE EFFECTS
  None

=========================================================================*/
void hdrlmac_get_subpkt_protocol_data
(
  hdrdiag_hdr_protocol_data_subpkt_protocol_data_type *subpkt_ptr
);

/*=========================================================================

FUNCTION     : HDRLMAC_GET_RX_TX_POWER_INFO

DESCRIPTION
  Returns HDR RX and TX AGC to the caller. 
 
  Rxagc and txagc will be reported in 1\10 dBm granularity.
 
  Ecio will only be reported for primary chain, in traffic it will be
  instantaneous primary energy, in idle state it will be total energy.
  Ecio will also be reported in 1\10 dBm.
    
DEPENDENCIES
  None

INPUTS
  Pointers to the location where Rx and Tx power info structure will be stored

RETURN VALUE
  Rx and Tx agc value are returned through the passed hdrsrch_rx_tx_power_info_type
  pointer.

SIDE EFFECTS
  None

=========================================================================*/

void hdrlmac_get_rx_tx_power_info
(
    hdrsrch_rx_pwr_info_struct_type   *rx_power_info,
    hdrsrch_tx_pwr_info_struct_type   *tx_power_info
);

#ifdef FEATURE_HDR_REVC
/*==========================================================================

FUNCTION HDRLMAC_GET_CC_MONITORING_CHAN

DESCRIPTION
  Checks if given channel is part of ASET channels

DEPENDENCIES
  None

PARAMETERS
  chan_ptr      - Pointer to channel structure to check if it is part of 
                  ASET channels

RETURN VALUE
Pointer to CC Carrier channel if found, otherwise NULL.

SIDE EFFECTS
  None

==========================================================================*/
sys_channel_type* hdrlmac_get_cc_monitoring_chan( void );


/*==========================================================================

FUNCTION HDRLMAC_PILOT_IS_IN_ASET

DESCRIPTION
  Check if the given pilot is part of ASET

DEPENDENCIES
  None

PARAMETERS
  chan - Channel to search for
  pn   - Pilot PN to search for
 
RETURN VALUE
  None

SIDE EFFECTS
  None

==========================================================================*/
boolean hdrlmac_pilot_is_in_aset
(
  const sys_channel_type* chan,
  const sys_pilot_pn_type pn
);


/*==========================================================================

FUNCTION HDRLMAC_IS_LINKID_ON_CC_MONITORING_CHAN

DESCRIPTION
  Checks if given link_id is on CC monitoring chan. This API is created
  as a faster alternative to calling hdrsrch_get_link_info() then compare
  the channel with hdrsrchset_get_cc_monitoring_chan().

DEPENDENCIES
  None

PARAMETERS 
  link_id - link_id to check 

RETURN VALUE
  True   - if link_id is on CC monitoring chan
  False  - otherwise

SIDE EFFECTS
  None

==========================================================================*/
boolean hdrlmac_is_linkid_on_cc_monitoring_chan
(
  const hdrsrch_link_id_type link_id
);

#endif /* FEATURE_HDR_REVC */

#ifdef FEATURE_HDR_DUAL_SIM
/*===========================================================================

FUNCTION HDRLMAC_STANDBY_PREF_CHGD

DESCRIPTION
  This function is called to inform HDR that standby pref has
  changed.
  
DEPENDENCIES
  Must be called when HDR is inactive

PARAMETERS
  ds_pref - standby preference

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void hdrlmac_standby_pref_chgd
(
  sys_modem_dual_standby_pref_e_type ds_pref
);
#endif /* FEATURE_HDR_DUAL_SIM */

#ifdef FEATURE_HDR_MODEM_API
/*=========================================================================

FUNCTION     : HDRLMAC_GET_MODEM_API_DATA

DESCRIPTION
  Retrieves Modem API data.
  During connection, data is collected at that instant.
  In idle, data collected is from last wakeup.
  Can be called from any task.
 
DEPENDENCIES
  None

INPUTS
  Structure with which to populate the Modem API data values
 
RETURN VALUE
  None

SIDE EFFECTS
  None

=========================================================================*/
void hdrlmac_get_modem_api_data
(
  hdrlmac_modem_api_data_type * modem_api_data
);
#endif /* FEATURE_HDR_MODEM_API */

/*===========================================================================

FUNCTION HDRLMAC_IS_LRE_ENABLED

DESCRIPTION
  This function returns enable/disable status of LRE feature. The feature
  can be turned on/off via NV.
 
DEPENDENCIES 
  None. 

PARAMETERS
  None.

RETURN VALUE
  TRUE  - if LRE feature is enabled
  FALSE - otherwise

SIDE EFFECTS
  None.

===========================================================================*/

boolean hdrlmac_is_lre_enabled( void );

/*===========================================================================

FUNCTION HDRLMAC_IS_IDLE_TA_STARTED

DESCRIPTION
  This function returns TA status from L1.
 
DEPENDENCIES 
  None. 

PARAMETERS
  None.

RETURN VALUE
  TRUE  - if tune away is started
  FALSE - otherwise

SIDE EFFECTS
  None.

===========================================================================*/

boolean hdrlmac_is_idle_ta_started( void );

#ifdef FEATURE_HDR_BOLT_MODEM
#ifndef FEATURE_HDR_UNIFIED_TX
/*===========================================================================

FUNCTION HDRLMAC_TRIGGER_SYSTEM_LOST

DESCRIPTION
  Triggers a system lost due to failure at lower layers (usually FW). 
 
DEPENDENCIES 
  None. 

PARAMETERS
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

void hdrlmac_trigger_system_lost( void );

/*===========================================================================

FUNCTION HDRLMAC_PROCESS_TRIGGER_SYSTEM_LOST

DESCRIPTION
  Processes the trigger system lost command in Tx context. 
 
DEPENDENCIES 
  None. 

PARAMETERS
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

void hdrlmac_process_trigger_system_lost( void );

#endif /* FEATURE_HDR_UNIFIED_TX */
#endif /* FEATURE_HDR_BOLT_MODEM */

/*===========================================================================
 
FUNCTION HDRLMAC_FEATURE_SKIP_DISABLE_TX_ENABLED
 
DESCRIPTION
  This function retrieves the HDR MAC debug mask from the hdrl1_dbg_msk
  NV item.
 
DEPENDENCIES
  None
 
PARAMETERS
  mac_user_debug_mask - filled in with value from NV
 
RETURN VALUE
  Whether debug mask is valid.
 
SIDE EFFECTS
  None.
 
===========================================================================*/
boolean hdrlmac_feature_skip_disable_tx_enabled(void);

#endif /* HDRLMAC_H */
