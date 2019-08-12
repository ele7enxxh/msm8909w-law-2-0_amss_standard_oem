#ifndef HDRSRCH_H
#define HDRSRCH_H

/*===*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                           H D R    S R C H

                          Search Header File

GENERAL DESCRIPTION
  This file provide all external interface functions for the HDR Searcher.
  These functions may directly handle the request, or queue the command
  on the HDR Searcher's command queue, for processing in the HDR Search
  context.

EXTERNALIZED FUNCTIONS
  hdrsrch_task
  hdrsrch_deactivate
  hdrsrch_set_params (deprecated)
  hdrsrch_set_search_params
  hdrsrch_set_mgmt_same_channel_params
  hdrsrch_set_control_channel_cycle
  hdrsrch_get_preferred_control_channel_cycle
  hdrsrch_acquire
  hdrsrch_synchronize
  hdrsrch_idle_mode
  hdrsrch_nset_update
  hdrsrch_idle_channel_change
  hdrsrch_sleep
  hdrsrch_access_mode
  hdrsrch_access_handoff
  hdrsrch_access_mode_ended
  hdrsrch_chan_and_aset_assign
  hdrsrch_aset_update
  hdrsrch_enable_pilot_reports
  hdrsrch_pilot_report_reset
  hdrsrch_get_pilot_report
  hdrsrch_get_drc_cover
  hdrsrch_get_link_info
  hdrsrch_at_set_drc
  hdrsrch_get_mac_index
  hdrsrch_set_ho_delays
  hdrsrch_register_roll_callback
  hdrsrch_deregister_roll_callback
  hdrsrch_advice_unlock_rf
  hdrsrch_get_ecio_and_io
  hdrsrch_get_sinr_and_rxagc
  hdrsrch_get_asp_pn_mapping
  hdrsrch_get_pn_ecio  
  hdrsrch_update_bc_info
  hdrsrch_update_qsm_info
  hdrsrch_set_user_equalizer_pref
  hdrsrch_set_user_debug_mask
  hdrsrch_set_user_debug_array
  hdrsrch_set_bcmcs_page_cycle
  hdrsrch_set_bcmcs_sleep_cycle
  hdrsrch_set_tc_mode
  hdrsrch_get_tc_mode
  hdrsrch_set_tap
  hdrsrch_acmac_ready  
  hdrsrch_acmac_ta_time_ok
  hdrsrch_rup_request
  hdrsrch_notify_cgps_status
  hdrsrch_clear_handoff_count
  hdrsrch_get_handoff_count
  hdrsrch_lte_nset_update
  hdrsrch_update_sector
  hdrsrch_is_in_suspend

REGIONAL FUNCTIONS
  None

INITIALIZATION AND SEQUENCING REQUIREMENTS
  hdrsrch_task( ) must be called to initialize the HDR Searcher.
  hdrsrch_task( ) will call mc_task_start( ) when it has initialized itself.

  Copyright (c) 2000 - 2015 by Qualcomm Technologies, Incorporated.  All Rights Reserved.

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

============================================================================

                        EDIT HISTORY FOR MODULE

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/hdr/srch/api/hdrsrch.h#1 $
$DateTime: 2016/12/13 07:58:05 $ $Author: mplcsds1 $

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

when         who     what, where, why
----------   ---     ------------------------------------------------------- 
06/12/2015   bb/vlc  Added hdrsrch_is_in_suspend(). 
05/07/2015   vlc     Added hdrsrch_abort_reselect_to_lte(). 
04/29/2015   vko     Inform TCA processing status, to HDRSRCH during IDLE_MODE_CMD
12/03/2014   ljl     Declared system lost if CC packet HSTR was incorrect.
10/22/2014   ljl     Fixed compiler warning. 
09/19/2014   sat     Added declaration for hdrsrch_is_idle_ta_started().
08/12/2014   wsh     Disable RTT in TAP call; force HiCap if boost, new threshold 
07/16/2014   vke     Set trm extension flag to hold the BG traffic 
                     so that GSM FW is awake before QTA
07/16/2014   arm     Added NV control to enable/disable QTA.
07/09/2014   cnx     Support NV refresh without reset.
05/05/2014   ljl     Added hdrsrch_idle_channel_change_and_nset_update().
11/29/2013   sat     NV control for Lock Release Enhancement (LRE)
12/20/2013   vlc     Added support for Tbackoff timer optimization. 
11/27/2013  dsp/cnx  FR17482 for Page blocking management.
07/31/2013   sat     Mainlining Modem Statistics Feature
05/02/2013   ljl     Added hdrsrch_set_scc_info().
02/26/2013   mbs     Fixed featurization for IRAT code
01/23/2013   wsh     Initialize DIV/ARD on timesync
07/09/2012   ljl     Added hdrsrch_acq_search_active().
03/19/2012   ljl     Fixed Dime compiler errors.
01/09/2012   smd     Supported dynamic acquisition diversity.
11/29/2011   arm     Removed compiler warnings.
11/11/2011   arm     HDR idle and access state diversity implemetation.
11/08/2011   ljl     Avoided 1x and hdr rf tune conflict.
10/24/2011   ljl     Supported RPM optimized timeline. 
10/10/2011   arm     Supported second chain test call feature.
01/08/2011   rkc     Supported returning SINR in 0.5 dB units. 
01/06/2011   kss     Added hdrsrch_is_grice_enabled(). 
06/27/2011   ssu     Added hdrsrch_get_band(). 
05/05/2011   cnx     Added hdrsrch_get_band_subclass().
03/22/2011   arm     Added support for thermal pa backoff feature. 
03/05/2011   arm     New API added to provide Rx and Tx power info to CM. 
04/12/2011   smd     Used a hdr abstract type for fw state. 
03/31/2011   smd     Exposed API to convert linear eng to db. 
01/17/2011   smd     Supported FTM for IRAT connected mode measurement.
09/01/2010   vlc     Added support for DO->LTE pilot measurements/reselection.
06/10/2010   lyl     Added hdrsrch_runtime_diversity_pref_ctrl().
05/01/2010   smd     Updated acquire and synchronization API for reverse time 
                     transfer.
03/24/2010   pxu     Removed FEATURE_HDR_QSM feature and FEATURE_HDR_EQ faeture 
                     checks. 
03/08/2010   lyl     Added hdrsrch_get_ftd_data().
03/05/2010   smd     Updated comments of hdrsrch_tran_sys_time().
02/26/2010   smd     Fixed the compiler error. 
02/09/2010   smd     Added APIs for LTE to DO reselection.
12/29/2009   ljl     Added API to enable firmware HDR App.
08/18/2009   etv     Changed hdrsrch_get_demod_carrier_info to 
                     hdrsrch_get_fl_carrier_info and added 
                     hdrsrch_get_subaset_index.
07/22/2009   ljl     Passed the frame offset to HDR SRCH.
07/16/2009   lyl     Supported CC config MSGR interface.
06/19/2009   grl     Mainlined per band diversity NV control.
03/23/2009   wsh     CMI: featurized CMI under FEATURE_CMI
03/20/2009   rkc     Added rup_subtype param to hdrsrch_nset_update().
03/18/2009   rkc     Added RF SVDO API changes.
03/09/2009   smd     Added support for pilot measurement velcro solution.
03/06/2009   ljl     Added hdrsrch_get_asp_index.
11/10/2008   ljl     Added hdrsrch_acmac_disable_ta().
11/03/2008   grl     Added ARDA enable/disable APIs.
10/09/2008   lyl     Added QPCH support.
09/18/2008   rkc     Updated hdrsrch_sleep() comments.
09/18/2008   ljl     Fixed the featurization issue.
09/08/2008   rkc     Added Intra-SCC sleep params to hdrsrch_sleep().
08/31/2008   lyl     Suppressed lint error in hdrsrch_set_user_debug_array().
08/15/2008   wsh     Fixed crash if TCA contains invalid channel
07/01/2008   ljl     Added functions to get rx/tx frequency.
04/17/2008   rkc     Added hdrsrch_notify_cgps_status().
03/27/2008   ljl     Added the avoided channel list to nset update function.
02/20/2008   ljl     Removed the parameter from hdrsrch_get_hdr_rssi().
01/30/2008   jyw     Modified the lmac/srch demod assignment interface.
01/30/2008   jyw     Obsoleted hdrsrch_if_chan_change.
10/31/2007   ljl     Merged Rev B changes from branch.
10/17/2007   grl     Merged Rev B changes from branch.
10/04/2007   rkc     Resolved Lint High warnings.
09/20/2007   mt      Added hdrsrch_update_num_sector_users().
09/18/2007   wsh     Report correct DiversityComb and SimultaneousCommonChannelReceive
                     value in Multi-Mode Discovery protocol
08/19/2007   grl     Resolved Lint High warnings.
08/08/2007   mpa     Added hdrsrch_get_tc_mode().
07/18/2007   mt      HDR per band diversity changes.
06/18/2007   rkc     Added hdrsrch_set_user_debug_array()
05/15/2007   mt      Added hdrsrch_rup_request().
04/27/2007   grl     Removed tc_mode from the traffic assignment cmd.
10/06/2006   grl     Added passing of the negotiated SCI value to hdrsrchsleep.
10/09/2006   mt      Added hdrsrch_notify_cc_packet().
08/28/2006   sq      Added FEATURE_HDR_QSM
07/10/2006   mt      Added hdrsrch_set_user_debug_mask().
06/30/2006   etv     Supported DSP Handoff algorithm changes.
06/07/2006   ljl     Added support for hybrid access
05/22/2006   ljl     Added hdrsrch_set_tap().
02/01/2006   sq      Added hdrsrch_set_bcmcs_sleep_cycle
01/15/2006   jyw     Added support to query the exist of multi rx chains.
12/14/2005   sq      Added support to disable the TC OFS in the TAP calls.
09/10/2005   grl     Added dynamic TC mode support for HPTS mode.
07/28/2005   ds      Renamed input variables for NSET update
07/27/2005   sq      Added support for BCMCS page monitoring
07/12/2005   sq      Added support for enhanced idle state protocol
07/12/2005   ds      Corrected NSET update interface sect list pointer
07/12/2005   ds      Add channel list, cached sectors to NSET update interface
04/15/2005   ds      Added function to set equalizer preference read from NV
02/01/2005   sq      Added function hdrsrch_get_sinr_and_rxagc
01/13/2005   sq      Mainlined FEATURE_HDR_NO_HO_DURING_SCC
11/03/2004   mpa     Removed dependency on std.h (merged with sys.h)
08/30/2004   aaj     Added enum type for bcmcs status
07/30/2004   sq      Added support for FEATURE_HDR_BCMCS
06/30/2004   dna     Inactive state support. Added hdrsrch_activate cmd
06/09/2004    sq     Added function hdrsrch_get_pn_ecio
06/08/2004    sq     Added function hdrsrch_get_asp_pn_mapping
04/02/2004    sq     Returns Ec/Io and Io for HDR
03/04/2004   aaj     Support for getting color_code from protocols
11/18/2003   aaj     Added system usability callback
10/09/2003   ajn     Added support for "Advise Unlock RF" command
03/11/2003   ajn     Updated Copyright notice
01/10/2002   ajn     Added FEATURE_HDR_NO_HO_DURING_SCC
11/28/2002   aaj     Added support hdr data level indicator
10/28/2002   aaj     Added support for acq mode for acquisition commands
09/18/2002   aaj     Added API for getting HDR RSSI for displaying signal bars
06/14/2002   ajn     Access H/O Threshold added to ACCESS_MODE_CMD
02/28/2002   aaj     Set external diversity control as per user pref thru NV
02/06/2002   ajn     hdrerrno->errno, suspend callback added to Idle/TC
                     Callbacks/#defines moved to hdrsrchtypes.
01/14/2002   aaj     Added traffic chan ack received cmd support for tc hybrid
12/07/2001   ajn     Added Pilot to Channel Change command.
10/08/2001   aaj     Added get drc cover function for test app interface
09/13/2001   aaj     Added hdrsrch_register/deregister_roll_callback functions
09/13/2001   aaj     Added hdrsrch_set_ho_delays() function
09/04/2001   ajn     hdrsrch_sleep() interface change.
07/05/2001   aaj     Added find MAC index function from Link ID
06/19/2001   aaj     Added AT command support for DRC rate change
02/27/2000   ajn     Deprecated functions removed.
02/23/2000   ajn     Added HDRSRCH_NO_CCC_HASH
11/30/2000   ajn     Code Review changes
10/23/2000   ajn     hdrsrch_synchronize expects a qword instead of uint64.
10/19/2000   ajn     NULL channel pointer in TCA means current ASET channel.
07/07/2000   ajn     Created this file

==========================================================================*/


/*==========================================================================

                     INCLUDE FILES FOR MODULE

==========================================================================*/
#include "hdr_variation.h"
#include "hdrsrchtypes.h"
#include "hdrlog.h"
#include "IxErrno.h"
#include "qw.h"
#ifndef FEATURE_RF_SVDO_API
#include "rfm.h"
#endif /* !FEATURE_RF_SVDO_API */

#ifndef FEATURE_CMI
#include "hdrtask.h"
#endif
#include "task.h"
#include "hdraddr.h"

/*==========================================================================

                     DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, typesdefs,
and other items needed by this module.

==========================================================================*/


/* EJECT */
/*==========================================================================

                    FUNCTION PROTOTYPES USED IN THE MODULE

This section contains function prototypes for the functions used in this
module.

==========================================================================*/


/* EJECT */
/*==========================================================================
                                   SUNDRY
==========================================================================*/


/*==========================================================================

FUNCTION HDRSRCH_ACTIVATE

DESCRIPTION
  Initialize hardware for HDR operation.  This is performed when HDR protocol
  is activated. This is supposed to be one time operation when HDR is enabled.
  Example: when HDR-only mode is entered from GSM only mode then MMoC will 
  activate HDR protocol by sending the activate command.  

DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

==========================================================================*/

void hdrsrch_activate(void);


/* EJECT */
/*==========================================================================

FUNCTION HDRSRCH_DEACTIVATE

DESCRIPTION
  Terminate all HDR processing.  To be used prior to Deep Sleep, transition
  back to 1x/AMPS.

DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  None

SIDE EFFECTS
  Exits (aborts) any HDR operations in progress.

==========================================================================*/

void hdrsrch_deactivate
(
  hdrsrch_deactivate_done_cb_type         deactivate_done_cb
    /* Callback function for when the deactivation is complete */
);


/* EJECT */
/*==========================================================================

FUNCTION HDRSRCH_SET_SEARCH_PARAMS

DESCRIPTION
  Sets various HDR Searcher Parameters, such as window size and pilot inc.

DEPENDENCIES
  None

PARAMETERS
  search_params_ptr

RETURN VALUE
  None

SIDE EFFECTS
  Regional data updated

==========================================================================*/

void hdrsrch_set_search_params
(
  const hdrsrch_search_params_struct_type  *search_params_ptr
    /* Parameter structure pointer */
);


/* EJECT */
/*==========================================================================

FUNCTION HDRSRCH_SET_MGMT_SAME_CHANNEL_PARAMS

DESCRIPTION
  Sets HDR Set Management Parameters for pilots on the ASET channel.

DEPENDENCIES
  None

PARAMETERS
  params_ptr

RETURN VALUE
  None

SIDE EFFECTS
  Regional data updated

==========================================================================*/

void hdrsrch_set_mgmt_same_channel_params
(
  const hdrsrch_chan_mgmt_params_struct_type  *params_ptr
    /* Parameter structure pointer */
);


/* EJECT */
/*==========================================================================

FUNCTION HDRSRCH_SET_MGMT_DIFF_CHANNEL_PARAMS

DESCRIPTION
  Sets HDR Set Management Parameters for pilots not on the ASET channel.

DEPENDENCIES
  None

PARAMETERS
  params_ptr

RETURN VALUE
  None

SIDE EFFECTS
  Regional data updated

==========================================================================*/

void hdrsrch_set_mgmt_diff_channel_params
(
  const hdrsrch_chan_mgmt_params_struct_type  *params_ptr
    /* Parameter structure pointer */
);


#ifdef FEATURE_HDR_BCMCS
/* <EJECT> */
/*===========================================================================

FUNCTION HDRSRCH_SET_BCMCS_PAGE_CYCLE

DESCRIPTION
  This command updates the BCMCS page cycle value.

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

void hdrsrch_set_bcmcs_page_cycle
(
  uint8 page_cycle
    /* In SCC unit ( 256 slots )  */
);


/*===========================================================================

FUNCTION HDRSRCH_SET_BCMCS_SLEEP_CYCLE

DESCRIPTION
  This command updates the BCMCS sleep cycle value.

DEPENDENCIES
  None

PARAMETERS
  sleep_cycle   - The BCMCS sleep period, in units of SCC ( 256 slots ).

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

void hdrsrch_set_bcmcs_sleep_cycle
(
  uint8 sleep_cycle
    /* In units of SCC ( 256 slots )  */
);

#endif /* FEATURE_HDR_BCMCS */


/* EJECT */
/*==========================================================================

FUNCTION HDRSRCH_SET_CONTROL_CHANNEL_CYCLE

DESCRIPTION
  Sets the control channel hash value, which defines the HDR slotting.
  Also sets the current maximum sleep cycle negotiated with AN.
  
DEPENDENCIES
  None

PARAMETERS
  control_channel_cycle - Value which defines HDR slot for mobile.
  max_sleep_cycle - negotiated max sleep cycle with AN.
  
RETURN VALUE
  None

SIDE EFFECTS
  Sleep in Idle Mode is enabled once the hash value is set.

===========================================================================*/

void hdrsrch_set_control_channel_cycle
(
  uint8   control_channel_cycle,
   /* Hash value "R" for "(C+R) mod N == 0"  */

  uint8   max_sleep_cycle
    /* final decided sleep cycle index between AT and AN */
);


/* EJECT */
/*==========================================================================

FUNCTION HDRSRCH_GET_PREFERRED_CONTROL_CHANNEL_CYCLE

DESCRIPTION
  Gets the preferred control channel cycle value for HDR slotting.

DEPENDENCIES
  None

PARAMETERS
  page_slot   - assigned slot in IS-95A/B or IS2000 1x system.
  max_hdr_sleep_cycle - current maximum HDR sleep cycle index
  default_scc - slot cycle index calculated by hashing
  
RETURN VALUE
  Hash value "R" for "(C+R) mod N == 0"

SIDE EFFECTS
  None

==========================================================================*/

uint8 hdrsrch_get_preferred_control_channel_cycle
(
  uint16                          page_slot,
    /* Assigned slot in IS-95A/B or IS2000 1x system */

  uint8                           max_hdr_sleep_cycle,
    /* maximum HDR sleep cycle index currently in effect  */

  uint8                           default_scc
    /* Default SCC index calculated by hash function, given the maximum
       sleep cycle above                                                */
);



/* EJECT */
/*==========================================================================
                 NETWORK ACQUISISTION FUNCTIONS
==========================================================================*/


/*==========================================================================

FUNCTION HDRSRCH_ACQUIRE

DESCRIPTION
  Attempts to acquire a given channel

DEPENDENCIES
  None

PARAMETERS
  acq_params      -  structure to hold parameters of acquire command

RETURN VALUE
  None

SIDE EFFECTS
  RF is Tuned to given channel.
  acq_cb() will be called some indeterminate time in the future.
  system_lost_cb( ) may be called when the HDR Searcher is in any state
  to indicate the HDR system has been lost.

==========================================================================*/

void hdrsrch_acquire
(
  hdrsrch_acquire_params_type      *acq_params
);


/* EJECT */
/*==========================================================================

FUNCTION HDRSRCH_SYNCHRONIZE

DESCRIPTION
  Once a system is acquired, and a synchronous control channel message is
  received, the HDR system must be "synchronized" to the system time that
  was specified in that message.  This function performs the required
  synchronization.

  The time specified in the SCC Message is for a point 160ms after
  the start of the SCC Capsule.

DEPENDENCIES
  hdrsrch_acquire( ) must have completed successfully.

PARAMETERS
  sync_time - Time specified in synchronous control channel (SCC) message.
  pilot_pn  - Pilot PN from SCC msg.
  half_slot - H/W "half slot" value at which SCC capsule started.
  return_timing_cb - callback to return CDMA system timing to upper layer.

RETURN VALUE
  None

SIDE EFFECTS
  A time discontinuity may occur.

==========================================================================*/

void hdrsrch_synchronize
(
  qword                          sync_time,
    /* Time from Synchronous Control Channel (SCC) message */

  sys_pilot_pn_type               pilot_pn,
    /* Pilot PN from SCC msg */

  uint16                          half_slot,
    /* The hardware half-slot value at the SCC message capsule start */

  hdrsrch_return_timing_cb_type   return_timing_cb
    /* Callback to return CDMA system time */
);


/* EJECT */
/*==========================================================================
                          IDLE MODE FUNCTIONS
==========================================================================*/


/*==========================================================================

FUNCTION HDRSRCH_IDLE_MODE

DESCRIPTION
  Causes the Searcher to enter Idle Mode processing.
  When a handoff to a nset pilot takes place, the given callback function
  will be called.

  RF will be taken out of Tx mode if it is in Tx mode.

  Note: The nset pilot information will only be valid for the duration
  of the callback.  The callback must copy the data if it is needed after
  the callback returns.

DEPENDENCIES
  hdrsrch_synchronize( ) must have completed successfully.

PARAMETERS
  idle_ho_cb - Callback function for idle handoff reports
  suspend_cb - Callback function for idle suspended mode event reports.
  is_tca_processing - is tca processing in progress.

RETURN VALUE
  None

SIDE EFFECTS
  Idle state processing is started.

==========================================================================*/

void hdrsrch_idle_mode
(
  hdrsrch_idle_ho_cb_type         idle_ho_cb,
    /* Callback function for idle handoff reports */

  hdrsrch_suspend_cb_type         suspend_cb,
    /* Callback function for suspend/resume events (RF lock loss/regain) */
  boolean                         is_tca_processing
    /* If this IDLE mode is due to processing of TCA */
);


/* EJECT */
/*==========================================================================

FUNCTION HDRSRCH_NSET_UPDATE

DESCRIPTION
  Update the neighbour list.

DEPENDENCIES
  In Idle (Monitor, Access or Sleep), or Connected mode

PARAMETERS
  num_nset_pilots   - Number of pilots in new NSET list
                      Must be <= HDRSRCH_NSET_MAX_SIZE
  nset_list         - Pointer to array of pilots
  struct_size       - Size (bytes) of the NSET structure, to allow passing a
                      pointer to an array of structures larger than
                      hdrsrch_nset_type
  update_type       - NSET info from a Broadcast or Directed message?
                      Directed info overrides Broadcast info
  num_channels      - Number of hash channels in the channel list 
  channel_list_ptr  - Pointer to list of hash channels from the SP message  
  num_sectors       - Number of sectors in the overehad cache
  sector_list_ptr   - List of sectors currently in the overhead cache
  serving_pilot_gid - Group id of the pilot from which the nset update is rx'ed
  hdr_subtype       - HDR protocol subtype

RETURN VALUE
  None

SIDE EFFECTS
  Sets the neighbour pilot list.  The search schedule will be updated to
  reflect the new neighbour pilot list.

==========================================================================*/

void hdrsrch_nset_update
(
  uint8                           num_nset_pilots,
    /* Number of pilots in the neighbour set */

  const hdrsrch_nset_pilot_type * nset_list,
    /* The neighbour set pilots */

  unsigned int                    struct_size,
    /* Size of the neighbour set pilot structure.  Allows an array of
       items larger than sizeof(hdrsrch_nset_pilot_type) to be used. */

  hdrsrch_nset_update_enum_type   update_type,
    /* NSET information from a BROADCAST or DIRECTED message? */

  uint8                           num_channels,
    /* Number of hash channels in the channel list */

  const sys_channel_type          *channel_list_ptr,
    /* List of hash channels from the BROADCAST message*/
 
  uint8                           num_avoided_channels,
    /* Number of avoided channels */

  const sys_channel_type          *avoided_channel_list_ptr,
    /* List of avoided channels */

  uint8                           num_sectors,
    /* Number of sectors in the overehad cache */
 
  sys_link_type                   *sector_list_ptr 
    /* List of sectors currently in the overhead cache */

#ifdef FEATURE_HDR_REVB
   , hdrsrch_pilot_gid_type       serving_pilot_gid,
    /* The group id of the pilot from which the nset update is received */

  sys_active_prot_e_type          hdr_subtype
    /* HDR Protocol subtype */
#endif /* FEATURE_HDR_REVB */
);


/* EJECT */
/*==========================================================================

FUNCTION HDRSRCH_IDLE_CHANNEL_CHANGE

DESCRIPTION
  Change to a new HDR channel.  This should be called when the HDR AT
  hashes to a new channel.

DEPENDENCIES
  Idle mode

PARAMETERS
  channel             - The new channel.
  chan_change_rpt_cb  - Function to call when channel change is complete.

RETURN VALUE
  None

SIDE EFFECTS
  RF is tuned to a new channel.

==========================================================================*/

void hdrsrch_idle_channel_change
(
  const sys_link_type *           link_ptr,
    /* New HDR Channel, and an initial pilot on that channel. */

  hdrsrch_chan_change_rpt_cb_type chan_change_rpt_cb
    /* Callback function for idle channel change reports */
);


/* EJECT */
/*==========================================================================

FUNCTION HDRSRCH_SLEEP

DESCRIPTION
  Enter Sleep.

  The access terminal will wakeup in time to demodulate its next assigned
  synchronous control channel capsule.

  The access terminal may be forced to wakeup by calling:
    hdrsrch_acquire( )
    hdrsrch_access_mode( )
    hdrsrch_traffic_channel_assignment( )

DEPENDENCIES
  For long-duration sleeps (ie, 5.12 seconds) the Control Channel Hash
  value must be set.  If this value has not been set, then the sleep
  duration will be no longer than 426.7ms.

PARAMETERS
  hdr_slot_cycle             - Indicates when to wakeup.  Not used if
                               cc_start_slot_valid is TRUE.
  hdr_neg_slot_cycle         - What is the current negotiated SCI which can be
                               different from what the protocols are asking for
                               this particular sleep cycle.
  qpch_enabled               - whether QPCH is enabled or not.
  wakeup_cb                  - Callback function to indicate when UT awakes.
  cc_start_slot_valid        - Flag whether cc_start_slot parameter is
                               valid or not.  If TRUE, use cc_start_slot
                               instead SCI to calculate  wake_slot.
  cc_start_slot              - CC start in slots.  This field is used if
                               cc_start_slot_valid is TRUE.
  ornl_cycle                 - Other RAT Neighbor List message has its own cycle
  session_is_open            - whether EVDO session is open
  ovhd_is_updated            - whether OVHD info is updated

RETURN VALUE
  None

SIDE EFFECTS
  Regardless of the reason for the wakeup, the wakeup callback will be
  called.

  The HDR Searcher may remain awake for part of the "sleep" period, so it
  may perform background searches (off-frequency &/or off-system pilots).

  5ms tick may be disabled - Rex timers may not expire at the "proper"
  time.

==========================================================================*/

void hdrsrch_sleep
(
  uint8                           hdr_slot_cycle,
    /* e.g. 6: 426ms, 9: 5.12s, 12: 40.96s
       Note: Not used if cc_start_slot_valid is TRUE. */

  uint8                           hdr_neg_slot_cycle,
    /* e.g. 6: 426ms, 9: 5.12s, 12: 40.96s */

  boolean                         qpch_enabled,
    /* whether QPCH is enabled or not */

  hdrsrch_wakeup_cb_type          wakeup_cb,
    /* Callback to indicate AT has woke up */

  boolean                         cc_start_slot_valid,
    /* Flag used to indicate whether param cc_start_slot is valid */

  uint16                          cc_start_slot
    /* CC start in slots. This field is valid if cc_start_slot_valid is TRUE */

#ifdef FEATURE_HDR_TO_LTE
  ,uint8                          ornl_cycle
    /* Other RAT (Radio Access Technologies) Neighbor List message has its own cycle */
#endif /* FEATURE_HDR_TO_LTE */

#ifdef FEATURE_HDR_PAGE_BLOCKING_MGMT
  ,boolean                         session_is_open
    /* whether EVDO session is open */

  ,boolean                         ovhd_is_updated
    /* whether OVHD info is updated */
#endif /* FEATURE_HDR_PAGE_BLOCKING_MGMT */

);


/* EJECT */
/*==========================================================================

FUNCTION HDRSRCH_REENABLE_IDLE_HANDOFFS

DESCRIPTION
  This function re-enables Idle Handoffs.

  Idle handoffs are disabled at the start of a Synchronous Control Capsule.
  They must be re-enabled when:
    - The control channel capsule is completely received.
    - No preamble is detected for the next packet of the SCC.
    - A CRC error occurs during the SCC.

  A sleep command implies "Ok to Handoff"

DEPENDENCIES
  Must only be sent when in IDLE state.

PARAMETERS
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None.

==========================================================================*/

void hdrsrch_reenable_idle_handoffs( void );


/* EJECT */
/*==========================================================================
                          ACCESS MODE FUNCTIONS
==========================================================================*/


/*==========================================================================

FUNCTION HDRSRCH_ACCESS_MODE

DESCRIPTION
  Enter Access Mode processing.  Automatic handoffs are suspended.
  RF will enter Tx mode.

  Note: May be called while in Sleep - will cause a wakeup (surprise!)

DEPENDENCIES
  Must be in Idle-Monitor or Idle-Sleep states.

PARAMETERS
  suspend_ho_cb       - Callback function to indicate when handoffs have
                        been suspended.
  handoff_request_cb  - Callback function to indicate that the searcher
                        has found a better pilot to be on.  The upper
                        layer may call hdrsrch_access_handoff( ) at a
                        convinenent time.
  thresh_0p5_db - Access handoff threshold, in 0.5 db units

RETURN VALUE
  None

SIDE EFFECTS
  Idle handoffs are suspended.

==========================================================================*/

void hdrsrch_access_mode
(
  hdrsrch_suspend_ho_cb_type      suspend_ho_cb,
    /* Callback function indicating idle handoff have been suspended. */

  hdrsrch_request_ho_cb_type      request_ho_cb,
    /* Callback function indicating a handoff is desired in access mode */

  int                             thresh_0p5_db
    /* Access Handoff threshold, in 0.5 dB */
);


/* EJECT */
/*==========================================================================

FUNCTION HDRSRCH_ACCESS_HANDOFF

DESCRIPTION
  Command a handoff to occur while in access mode.

  This should be called after the request_ho_cb( ) specified in the
  hdrsrch_access_mode( ) command has been called, and the current access
  probe has been suspended.

DEPENDENCIES
  Access mode, with no access probes underway.

PARAMETERS
  new_pilot     - Specifies the pilot to hand off to.  This may be NULL, to
                  allow Searcher to choose the best pilot to handoff to, as
                  conditions may have changed since it issued the
                  request_ho_cb( ) indication.

  access_ho_rpt - Function to call to report completion of access handoff.

RETURN VALUE
  None

SIDE EFFECTS
  The Access Mode access_ho_cb will be issued after the handoff is
  complete.

==========================================================================*/

void hdrsrch_access_handoff
(
  const sys_link_type *           new_pilot,
    /* Pilot to handoff to */

  hdrsrch_access_ho_cb_type       access_ho_rpt
    /* Report of handoff complete */
);


#ifdef FEATURE_HDR_HYBRID_ACCESS
/*==========================================================================

FUNCTION HDRSRCH_ACMAC_READY

DESCRIPTION
  This function notifies HDR SRCH that ACMAC is ready.

DEPENDENCIES
  None

PARAMETERS
  ta_time_cb - callback function for tune away time.
  stop_cb    - callback function to stop access.
  
RETURN VALUE
  None

SIDE EFFECTS
  None

==========================================================================*/

void hdrsrch_acmac_ready
(
  hdrsrch_acmac_ta_time_cb_type      ta_time_cb,
    /* Callback function indicating tune away time */

  hdrsrch_acmac_stop_cb_type         stop_cb
    /* Callback function to stop ACMAC */

);


/*==========================================================================

FUNCTION HDRSRCH_ACMAC_TA_TIME_OK

DESCRIPTION
  This function notifies HDR SRCH whether tune away time is ok.

DEPENDENCIES
  None

PARAMETERS
  ta_time_ok  - tune away time is ok or not.

RETURN VALUE
  None

SIDE EFFECTS
  None

==========================================================================*/

void hdrsrch_acmac_ta_time_ok
(
  boolean                         ta_time_ok
    /* Whether tune away time is ok */
);


/*==========================================================================

FUNCTION HDRSRCH_ACMAC_DISABLE_TA

DESCRIPTION
  This function notifies HDR SRCH that ACACK has been received and 
  disables tune away for the duration specificed by no_ta_duration ms.
  
DEPENDENCIES
  None

PARAMETERS
  no_ta_duration - The duration that tune away should be disabled.

RETURN VALUE
  None

SIDE EFFECTS
  None

==========================================================================*/

void hdrsrch_acmac_disable_ta
(
  uint16                     no_ta_duration
    /* The duration that tune away should be disabled */
);
#endif /* FEATURE_HDR_HYBRID_ACCESS */


/* EJECT */
/*==========================================================================

FUNCTION HDRSRCH_ACCESS_MODE_ENDED

DESCRIPTION
  Causes the Searcher to enter the Idle-Monitor state and transition the
  RF out of Tx mode if the Searcher is in the Idle-Access state.
  Otherwise it has no effect.

DEPENDENCIES
  hdrsrch_access_mode( ) must have completed successfully.

PARAMETERS
  idle_ho_cb - Callback function for idle handoff reports

RETURN VALUE
  None

SIDE EFFECTS
  Idle-Monitor state processing may be started.

==========================================================================*/

void hdrsrch_access_mode_ended
(
  hdrsrch_idle_ho_cb_type   idle_ho_cb
    /* Callback function for idle handoff reports */
);


/* EJECT */
/*==========================================================================
                       CONNECT (Traffic) MODE FUNCTIONS
==========================================================================*/

/*==========================================================================

FUNCTION HDRSRCH_CHAN_AND_ASET_ASSIGN

DESCRIPTION
  Enters Connected Mode, or Effect a Hard Handoff to a new frequency
  while in Connected Mode.

  Sets the channel and active set pilots for the Connected (Traffic) state.
  Used when a traffic channel is initially assigned, and when a handoff
  to a different frequency is required.

  RF will enter Tx mode.

DEPENDENCIES
  Idle or Connected Mode

PARAMETERS
  channel           - HDR Channel for the traffic channel
                      (NULL implies current ASET channel)
  tc_tune_away      - Whether HDR is willing to allow tuneaways.
  num_aset_pilots   - Number of Active Set pilots
                      Must be <= HDRSRCH_ASET_MAX_SIZE
  aset_list         - The Active Set pilots
  struct_size       - Size (bytes) of each aset_list structure, allowing an
                      array of structures larger than hdrsrch_aset_pilot_type
                      to be used.
  traffic_channel_assigned_rpt
                    - Callback used to indicate RF has tuned to req'd
                      frequency, and Search has assigned fingers to the
                      Active Set pilots.

  suspend_cb        - Callback for TC suspended mode event reports.

RETURN VALUE
  None

SIDE EFFECTS
  May cause Search to wakeup from sleep.

  hdrsrch_enable_pilot_reports( NULL ) is implied if transitioning from
  idle to connected state.  To enable pilot reports, call
  hdrsrch_enable_pilot_reports( ).

  hdrsrch_enable_pilot_reports( ) is unchanged if effecting a hard h/o
  to a new frequency.

==========================================================================*/

void hdrsrch_chan_and_aset_assign
(
  const sys_channel_type *        channel,
    /* HDR Channel to operate on (NULL -> Current ASet channel) */

  hdrsrch_traffic_tune_away_enum_type  tc_tune_away,
    /* What other systems we are allowed to tune away from HDR to visit */

  uint8                           num_aset_pilots,
    /* Number of pilots in the active set */

  const hdrsrch_aset_pilot_type * aset_list,
    /* The active set pilots */

  unsigned int                    struct_size,
    /* Size of the active set pilot structure.  Allows an array of
       items larger than sizeof(hdrsrch_aset_pilot_type) to be used. */

  hdrsrch_tca_cb_type             traffic_channel_assigned_rpt,
    /* Function to call when the RF has tuned to the required channel,
       and the search has set fingers on some subset of the active set.
       It is safe to turn on the reverse traffic channel */

  hdrsrch_suspend_cb_type         suspend_cb
    /* Callback function for suspend/resume events (RF lock loss/regain) */
);

/* EJECT */
/*==========================================================================

FUNCTION HDRSRCH_ASET_UPDATE

DESCRIPTION
  Sets the active set for the Connected (Traffic) state.

DEPENDENCIES
  hdrsrch_traffic_channel_assignment( ) must be called before
  hdrsrch_aset_update( ) calls are allowed.

PARAMETERS
  num_aset_pilots   - Number of Active Set pilots
                      Must be <= HDRSRCH_ASET_MAX_SIZE
  aset_list         - The Active Set pilots
  struct_size       - Size (bytes) of each aset_list structure, allowing an
                      array of structures larger than hdrsrch_aset_pilot_type
                      to be used.

RETURN VALUE
  None

SIDE EFFECTS
  May cause an immediate reschedule of searches.

==========================================================================*/

void hdrsrch_aset_update
(
  uint8                                   num_aset_pilots,
    /* Number of pilots in the active set */

  const hdrsrch_aset_pilot_type * aset_list,
    /* The active set pilots */

  unsigned int                          struct_size
    /* Size of the active set pilot structure.  Allows an array of
       items larger than sizeof(hdrsrch_aset_pilot_type) to be used. */
);


#ifdef FEATURE_HDR_REVB
/* EJECT */
/*==========================================================================

FUNCTION HDRSRCH_CHAN_AND_ASET_ASSIGN_REVB

DESCRIPTION
  RF will enter Tx mode.

DEPENDENCIES
  Idle or Connected Mode

PARAMETERS
  hdr_subtype          - The subtype of the HDR protocol.
  tc_tune_away         - Whether HDR is willing to allow tuneaways.
  num_subactive_set    - Number of subActive Sets 
                         Must be <= HDRSRCH_MAX_NUM_SUBASET
  subasetive_set_list  - The subActive Set list.
  cc_subactive_set_idx - The suActive Set index of the subActive Set
                         carring the control channel.
  traffic_channel_assigned_rpt
                       - Callback used to indicate RF has tuned to 
                         req'd frequency, and Search has assigned 
                         fingers to the Active Set pilots.

  suspend_cb           - Callback for TC suspended mode event reports.

  schdgrp_rpt_cb       - Callback for scheduler group changes.

RETURN VALUE
  None

SIDE EFFECTS
  May cause Search to wakeup from sleep.

  hdrsrch_enable_pilot_reports( NULL ) is implied if transitioning from
  idle to connected state.  To enable pilot reports, call
  hdrsrch_enable_pilot_reports( ).

  hdrsrch_enable_pilot_reports( ) is unchanged if effecting a hard h/o
  to a new frequency.

==========================================================================*/

void hdrsrch_chan_and_aset_assign_revb
(
  sys_active_prot_e_type               hdr_subtype,
    /* HDR Protocol subtype */

  hdrsrch_traffic_tune_away_enum_type  tc_tune_away,
    /* Whether HDR is permitted to tune away to the suspended state for 
       another technology.  This is set to FALSE for session negotiation.*/

  uint8                                num_subactive_sets,
    /* Number of subactive asets to add to active set */

  const hdrsrch_subactive_set_type     *subactive_set_list,
    /* Pointer to an array of subactive aset structures */

  uint8                                cc_subactive_set_idx,
    /* The subactive set which contains the control channel */

  uint8                                frame_offset,
    /* Frame offset */

  hdrsrch_tca_cb_type                  traffic_channel_assigned_rpt,
    /* Function to call when the RF has tuned to the required channel,and 
       the search has tried to assign fingers on some subset of the active set.  
       It is safe to turn on the reverse traffic channel */

  hdrsrch_suspend_cb_type              suspend_cb,
    /* Callback function for suspend/resume events (RF lock loss/regain). */

  hdrsrch_schdgrp_rpt_cb_type          schdgrp_rpt_cb
    /* Callback function for added and deleted scheduler groups. */
);


/* EJECT */
/*==========================================================================

FUNCTION HDRSRCH_ASET_UPDATE_REVB

DESCRIPTION
  Sets the active set for the Connected (Traffic) state.

DEPENDENCIES
  hdrsrch_chan_and_aset_assign_revb( ) must be called before
  hdrsrch_aset_update_revb( ) calls are allowed.

PARAMETERS
  hdr_subtype          - Subtype of the HDR protocol.
  num_subactive_sets   - Number of subActive Set pilots
  subactive_set_list   - The subActive Set list
  cc_subactive_set_idx - the index to the subaset which carries CC channel

RETURN VALUE
  None

SIDE EFFECTS
  May cause an immediate reschedule of searches.

==========================================================================*/

void hdrsrch_aset_update_revb
(
  sys_active_prot_e_type            hdr_subtype,
    /* HDR Protocol subtype */
  
  uint8                             num_subactive_sets,
    /* The number of subactive set to be added */

  const hdrsrch_subactive_set_type  *subactive_set_list,
    /* Sub active set list to be added */

  uint8                             cc_subactive_set_idx,
    /* The subactive set idx which have the control channel */

  uint8                              frame_offset
    /* Frame offset */
);

#ifdef FEATURE_HDR_REVC
/* EJECT */
/*==========================================================================

FUNCTION HDRSRCH_ASET_UPDATE_REVC

DESCRIPTION
  Sets the active set for the Connected (Traffic) state.

DEPENDENCIES
  hdrsrch_chan_and_aset_assign_revb( ) must be called before
  hdrsrch_aset_update_revb( ) calls are allowed.

PARAMETERS
  rup_subtype          - Subtype of the RUP protocol.
  num_subactive_sets   - Number of subActive Set pilots
  subactive_set_list   - The subActive Set list
  cc_subactive_set_idx - the index to the subaset which carries CC channel
  update_complete_cb   - Callback to inform ASET has been updated

RETURN VALUE
  None

SIDE EFFECTS
  May cause an immediate reschedule of searches.

==========================================================================*/
void hdrsrch_aset_update_revc
(
  sys_active_prot_e_type             hdr_subtype,
    /* HDR Protocol subtype */
 
  uint8                               num_subactive_sets,
    /* The number of subactive set to be added */

  const hdrsrch_subactive_set_type *  subactive_set_list,
    /* Sub active set list to be added */

  uint8                               cc_subactive_set_idx,
    /* The subactive set idx which have the control channel */

  uint8                              frame_offset,
    /* Frame offset */

  hdrsrch_aset_update_cb_type         update_complete_cb
);
#endif /* FEATURE_HDR_REVC */

/* EJECT */
/*==========================================================================

FUNCTION HDRSRCH_GET_SCHDGRP_RPT

DESCRIPTION
  Get the scheduler group report.

DEPENDENCIES

PARAMETERS
  schdgrp_rpt  - Pointor to the report struct.

RETURN VALUE
  None

SIDE EFFECTS

==========================================================================*/

void hdrsrch_get_schdgrp_rpt
( 
  hdrsrch_schdgrp_rpt_type *schdgrp_rpt 
);


/* <EJECT> */
/*===========================================================================

FUNCTION HDRSRCH_ASSIGN_DEMOD_IDXS_TO_SUBASETS

DESCRIPTION
  The function returns the demod idx assignment based on the current incoming
  TCA. 

DEPENDENCIES
  Connected State

PARAMETERS

RETURN VALUE
  Return the demod_idx assignment of each of the subActive Set according to the 
  order of the subActive Set in a TCA.

  The return boolean indicates if RF reconfiguration (RF tune) is required for 
  this TCA.

SIDE EFFECTS
  None

===========================================================================*/

boolean hdrsrch_assign_demod_idxes_to_subasets
(
  uint8 num_subactive_sets, 
    /* Number of subactive sets */

  const hdrsrch_subactive_set_type * subactive_set_list,
    /* The link list of subactive sets */

  hdrsrch_demod_idx_assignment_type * demod_idx_assigned
    /* The demod index assignments */
);


/* <EJECT> */
/*===========================================================================

FUNCTION HDRSRCH_GET_FL_CARRIER_INFO

DESCRIPTION
  The function returns the information about the forward link carrier.

DEPENDENCIES
  None

PARAMETERS
  subaset_idx - the sub aset idx associated with the carrier requested
  fl_carrier_info - The pointer to the info structure to fill.

RETURN VALUE
  TRUE if the carrier identified Sub ASET index is in use and FALSE otherwise.

SIDE EFFECTS
  None

===========================================================================*/

boolean hdrsrch_get_fl_carrier_info
(
  hdrsrch_subaset_idx_type           subaset_idx,
    /* The sub active set index */

  hdrsrch_fl_carrier_info_type*       fl_carrier_info
    /* The pointer to the table to fill */
);
#endif /* FEATURE_HDR_REVB */


#ifdef FEATURE_HDR_BCMCS

/* EJECT */
/*==========================================================================

FUNCTION HDRSRCH_UPDATE_BC_INFO

DESCRIPTION
  This function informs HDRSRCH about the current BCMCS status.

DEPENDENCIES
  None

PARAMETERS
  bcmcs_status - boolean

  TRUE means that we are monitoring BCMCS channels. 
  FALSE means that we stopped monitoring of BCMCS channels.

RETURN VALUE
  None

SIDE EFFECTS
  This command might cause a HDRSRCH state transition.

==========================================================================*/

void hdrsrch_update_bc_info
(
  hdrsrch_bcmcs_enum_type          bcmcs_status
    /* Flag indicates whether BCMCS is on/off       */
);

#endif /* FEATURE_HDR_BCMCS */

/* EJECT */
/*==========================================================================

FUNCTION HDRSRCH_ENABLE_PILOT_REPORTS

DESCRIPTION
  Enables or Disables generation of pilot reports.

DEPENDENCIES
  Connected Mode

PARAMETERS
  pilot_rpt_cb  - Function to call when a pilot report is required.
                  NULL to disable pilot reports.

RETURN VALUE
  None

SIDE EFFECTS
  May immediately cause pilot_rpt_cb( ) to be called.

==========================================================================*/

void hdrsrch_enable_pilot_reports
(
  hdrsrch_pilot_rpt_cb_type pilot_rpt_cb
    /* Pilot report indication callback, or NULL to disable */
);


/* EJECT */
/*==========================================================================

FUNCTION HDRSRCH_PILOT_REPORT_RESET

DESCRIPTION
  Reset the "Reported" flag for "interesting" pilots in the Active and
  Candidate sets ( C >> A, or A's Drop Timer expired)

DEPENDENCIES
  Connected Mode

PARAMETERS
  None

RETURN VALUE
  None

SIDE EFFECTS
  May immediately cause pilot_rpt_cb( ) to be called, if new pilot events
  have occurred since the last call to hdrsrch_get_pilot_report( ).

==========================================================================*/

void hdrsrch_pilot_report_reset( void );


/* EJECT */
/*==========================================================================

FUNCTION HDRSRCH_GET_PILOT_REPORT

DESCRIPTION
  Retrieves a snapshot of pilot information.

DEPENDENCIES
  Connected Mode

PARAMETERS
  max_pilots    - Maximum number of pilots which will be retrieved.
  pilots        - Array of length max_pilots, in which the pilot report
                  information will be stored.

RETURN VALUE
  None

SIDE EFFECTS
  Clears the "ReportPending" status for the reported sectors.

==========================================================================*/

int hdrsrch_get_pilot_report
(
  int                                       max_pilots,
    /* Maximum # of pilot entries which may be placed in "pilots" array */

  hdrsrch_pilot_rpt_type *        pilots
    /* Array to store pilot information in */
);

/* EJECT */
/*==========================================================================

FUNCTION HDRSRCH_RUP_REQUEST

DESCRIPTION
  Posts a RUP request command to searcher.

DEPENDENCIES
  None
  
PARAMETERS
  route_update_request   - Parameters of the route update message.
  pilot_rpt_cb           - Pilot report indication callback

RETURN VALUE
  None

SIDE EFFECTS
  None
  
==========================================================================*/

void hdrsrch_rup_request
(
  const hdrsrch_rup_req_params_type   *route_update_request,
    /* pointer to RUP request params structure */
    
  hdrsrch_pilot_rpt_cb_type           pilot_rpt_cb
    /* Pilot report indication callback, or NULL to disable */
);


/* EJECT */
/*==========================================================================

FUNCTION HDRSRCH_GET_LINK_INFO

DESCRIPTION
  Retrieves the stored information on a given link ID

DEPENDENCIES

PARAMETERS
  link_id       - The link ID in the DSM item of a received message
  link_info_ptr - Location where to write the link info corresponding to
                  the given link ID.

RETURN VALUE
  E_SUCCESS if the link ID is found, E_FAILURE if it is not found.

SIDE EFFECTS
  None

==========================================================================*/

errno_enum_type hdrsrch_get_link_info
(
  hdrsrch_link_id_type            link_id,
    /* Link id information is being requested for */

  sys_link_type *                 link_info_ptr
    /* Location to store the pilot info about the specified link id */
);


/* EJECT */
/*==========================================================================

FUNCTION HDRSRCH_AT_SET_DRC

DESCRIPTION
  Sets the DRC rate/mode from the AT command. These values are set only
  after ~30msec after the traffic channel has been initiated.

DEPENDENCIES
  None.

PARAMETERS
  drc_param = 0 to 12 for setting override mode with constant DRC rate
  drc_param = 0xf     for setting variable rate DRC

RETURN VALUE
  None

SIDE EFFECTS
  Changes the DRC rate or mode

==========================================================================*/

void hdrsrch_at_set_drc
(
#ifdef FEATURE_HDR_REVB
  hdrsrch_demod_idx_type demod_idx,
      /* The demod index on which the AT command is executed. */
#endif /* FEATURE_HDR_REVB */

  uint8       drc_param
    /* parameter to be used for setting the DRC from AT command */

);


/* EJECT */
/*==========================================================================

FUNCTION HDRSRCH_GET_MAC_INDEX

DESCRIPTION
  Retrieves the MAC index for the passed Link ID

DEPENDENCIES

PARAMETERS
  link_id       - The link ID in the DSM item of a received message

RETURN VALUE

  MAC_INDEX : 8 bit MAC Index for the given link ID (4 through 63)
  -1        : No MAC Index associated with the link ID

SIDE EFFECTS
  None

==========================================================================*/

int8 hdrsrch_get_mac_index
(
  hdrsrch_link_id_type            link_id
    /* Link id information is being requested for */
);


/* EJECT */
/*==========================================================================

FUNCTION HDRSRCH_GET_RL_MAC_INDEX

DESCRIPTION
  Retrieves the reverse link MAC index for the passed Link ID

DEPENDENCIES

PARAMETERS
  link_id       - The link ID in the DSM item of a received message

RETURN VALUE

  MAC_INDEX : 16 bit MAC Index for the given link ID (4 through 63)
  -1        : No MAC Index associated with the link ID

SIDE EFFECTS
  None

==========================================================================*/

int16 hdrsrch_get_rl_mac_index
(
  hdrsrch_link_id_type            link_id
    /* Link id information is being requested for */
);


/* EJECT */
/*==========================================================================

FUNCTION HDRSRCH_GET_DRC_COVER

DESCRIPTION
  Retrieves the DRC cover for the passed Link ID

  //TBD: Consolidate get_mac_index, get_link_info & get_drc_cover into a
  //single function.

DEPENDENCIES
  This function searches all the pilot sets (A, C & N) for the given link
  id and returns the DRC cover. It is recommended that this function be
  called only if the linkID is changed in the dsm header.

PARAMETERS
  link_id       - The link ID in the DSM item of a received message

RETURN VALUE

  DRC Cover : 4 bit Drc Index for the given link ID
  -1        : No DRC Cover associated with the link ID

SIDE EFFECTS
  None

==========================================================================*/

int8 hdrsrch_get_drc_cover
(
  hdrsrch_link_id_type            link_id
    /* Link id information is being requested for */
);


/* <EJECT> */
/*=========================================================================

FUNCTION     : HDRSRCH_REGISTER_ROLL_CALLBACK

DESCRIPTION  : This function registers PN ROLL callback function which will
               be called after receiving the PN ROLL ISR.

               The callback functions are executed in the PN ROLL ISR
               context so it is very important to have these callback
               functions quick and simple. Note that there is a limitation
               on maximum number of callbacks that are supported by the
               search task. Presently this number is set to "4"

               It is recommended that these callback functions just set a
               Rex Signal to the concerned task.

DEPENDENCIES : None

INPUT        : Callback function pointer

RETURN VALUE : None

SIDE EFFECTS : May begin periodic PN roll ISR

=========================================================================*/

void hdrsrch_register_roll_callback
(
  hdrsrch_roll_cb_type       cb
    /* callback function to be registered */
);



/*=========================================================================

FUNCTION     : HDRSRCH_DEREGISTER_ROLL_CALLBACK

DESCRIPTION  : This function unregisters PN ROLL callback function

DEPENDENCIES : None

INPUT        : Callback function pointer

RETURN VALUE : None

SIDE EFFECTS : May end periodic PN roll ISR

=========================================================================*/

void hdrsrch_deregister_roll_callback
(
  hdrsrch_roll_cb_type       cb
    /* callback function to be registered */
);


/* <EJECT> */
/*=========================================================================

FUNCTION     : HDRSRCH_ADVISE_UNLOCK_RF

DESCRIPTION  : This function is a hint from the upper layers that HDRSRCH
               should release its lock on the RF resources, if it has
               the lock.

DEPENDENCIES : None

RETURN VALUE : None

SIDE EFFECTS : None

=========================================================================*/

void hdrsrch_advise_unlock_rf( void );


/* <EJECT> */
/*=========================================================================

FUNCTION     : HDRSRCH_TC_ACK_RECEIVED

DESCRIPTION  : This function tells that TC ack is received by protocols

DEPENDENCIES : None

INPUT        : None

RETURN VALUE : None

SIDE EFFECTS : None

=========================================================================*/

void hdrsrch_tc_ack_received( void );


/* <EJECT> */
/*=========================================================================

FUNCTION     : HDRSRCH_SET_USER_DIVERSITY_PREF

DESCRIPTION  : This function sets the user diversity preference. HDRMC reads
               the diversity preference from the NV and calls this fucntion
               to set it.

               This is called during hdrmc powerup initialization

DEPENDENCIES : MDSP should already be downloaded and running by this time

INPUT        : Whether diversity is on and which bands it is on for.

RETURN VALUE : None

SIDE EFFECTS : None

=========================================================================*/

void hdrsrch_set_user_diversity_pref
(
  uint8       div_ctrl,
    /* User diversity control read from NV */

  uint32      div_band_mask
    /* HDR diversity enable band class mask read from NV */
);

/*=========================================================================

FUNCTION     : HDRSRCH_SET_USER_EQUALIZER_PREF

DESCRIPTION  : This function sets the user Equalizer/Rake/best selection
               preference. HDRMC reads the equalizer preference from the NV 
               and calls this function to set it.
               This is called during hdrmc powerup initialization

DEPENDENCIES : MDSP should already be downloaded and running by this time

INPUT        : User equalizer control value read from NV

RETURN VALUE : None

SIDE EFFECTS : None

=========================================================================*/

void hdrsrch_set_user_equalizer_pref
(
  uint8 eq_ctrl
    /* user equalizer control value read from NV */
);

/* <EJECT> */
/*=========================================================================

FUNCTION     : HDRSRCH_SET_USER_DEBUG_MASK

DESCRIPTION  : This function sets the user searcher debug mask. HDRMC reads
               the debug mask from the NV and calls this function
               to set it.

               This is called during hdrmc powerup initialization.

DEPENDENCIES : None

INPUT        : user HDR L1 searcher debug mask read from NV

RETURN VALUE : None

SIDE EFFECTS : None

=========================================================================*/

void hdrsrch_set_user_debug_mask
(
  uint64     debug_mask,
    /* user HDR L1 searcher debug mask read from NV */

  boolean    mask_valid
    /* Flag to indicate that NV item has been succesfully read */
);

/* <EJECT> */
/*=========================================================================

FUNCTION     : HDRSRCH_SET_USER_DEBUG_ARRAY

DESCRIPTION  : This function sets the user searcher debug array. HDRMC reads
               the debug array from the NV and calls this function
               to set it.  Since the array type and size are not exported
               byte nv_items, check if the size of the array is correct.
               If the input is larger, it is ok to copy less.

               This is called during hdrmc powerup initialization.

DEPENDENCIES : None

INPUT        : user HDR searcher debug array read from NV
               flag to indicate NV item was successfully read
               size of the input debug array in number of uint8 bytes

RETURN VALUE : None

SIDE EFFECTS : None

=========================================================================*/

/*lint -esym(18, hdrsrch_set_user_debug_array) */
/* Note: LINT error 18 is a false positive and not a real error. */

void hdrsrch_set_user_debug_array
(
  const uint8 *debug_ptr,
    /* user HDR searcher debug array read from NV */

  boolean     array_valid,
    /* Flag to indicate that NV item has been succesfully read */

  size_t      debug_array_size
    /* Size of input debug array in uint8 bytes */
);


/* <EJECT> */
/*=========================================================================

FUNCTION     : HDRSRCH_GET_HDR_RSSI

DESCRIPTION  : This function returns the RSSI estimate for HDR to be used
               by the User Interface to indicate quality of HDR service.

DEPENDENCIES : None

INPUT        : In feature revb, uses subaset index as input.

RETURN VALUE : None

SIDE EFFECTS : None

=========================================================================*/

byte hdrsrch_get_hdr_rssi
( 
  void 
);

/* <EJECT> */
/*=========================================================================

FUNCTION     : HDRSRCH_GET_ECIO_AND_IO

DESCRIPTION
  Returns HDR Ec/Io and Io to the caller. It returns the Io estimate made by 
  rx AGC loops in the DSP and the Ec/Io estimate found by the searcher.

  Ec/Io range: from 0 to 63 (Returned in -.5 dB units)
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

void hdrsrch_get_ecio_and_io( int16*, int* );


/* <EJECT> */

/*===========================================================================

FUNCTION HDRSRCH_GET_SINR_AND_RXAGC

DESCRIPTION
  This function returns SINR indicator and Rx AGC to the caller. SINR 
  indicator is from 0 to 8. Level 8 represents highest SINR and level 0 
  represents lowest SINR.
  
  SINR indicator range: from 0 to 8
  RxAgc range         : from -106 dBm to -21 dBm 

DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  None
  
SIDE EFFECTS
  None.

===========================================================================*/

void hdrsrch_get_sinr_and_rxagc 
(
  byte    *sinr, 
    /* pointer to SINR indicator value */

  int     *rxagc
    /* pointer to RxAgc value   */    
);

/*===========================================================================

FUNCTION HDRSRCH_GET_FTD_DATA

DESCRIPTION
  This function returns FTD data to the caller including:
  
  EcIo: from -31 to 0 dB;
  C/I: from -27 to 18 dB;
  rxAGC: from -115 dBm to -13 dBm;
  txAGC: from -64 dBm to 42 dBm;
  MAC index: Mac index of the cc monitoring pilot;
  ASET channel info.

DEPENDENCIES
  None

PARAMETERS
  ftd_data - Pointer of the FTD data;

RETURN VALUE
  None
  
SIDE EFFECTS
  None.

===========================================================================*/

void hdrsrch_get_ftd_data
(
  hdrlog_ftd_l1_stats_type* ftd_data
    /* Pointer of FTD L1 info */
);

/* <EJECT> */
/*===========================================================================

FUNCTION HDRSRCH_GET_SINR_AND_RXAGC_DB

DESCRIPTION
  This function returns SINR indicator and Rx AGC to the caller. SINR 
  indicator is from 0 to 75 in 0.5 dB units from -18dB to -19dB, e.g.
  0 = -9dB, 7=-5.5dB, 35 = +9dB.
  
  SINR indicator range: from 0 to 37 in 0.5 dB units from -18dB to -19 dB
  RxAgc range         : from -106 dBm to -21 dBm 

DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  None
  
SIDE EFFECTS
  None.

===========================================================================*/

void hdrsrch_get_sinr_and_rxagc_db
(
  byte    *sinr, 
    /* pointer to SINR indicator value */

  int     *rxagc
    /* pointer to RxAgc value   */    
);

/* <EJECT> */
/*=========================================================================

FUNCTION     : HDRSRCH_GET_DATA_INDICATOR

DESCRIPTION  : This function returns the data level indicator. The data rate
               that the AT can receive is divided into 9 levels, 0 to 8. Level
               8 indicates highest data rate and level 0 indicates almost no
               data rate.

DEPENDENCIES : None

INPUT        : None

RETURN VALUE : Data activity indicator between 0 to 8.

SIDE EFFECTS : None

=========================================================================*/

byte hdrsrch_get_data_indicator( void );


/* <EJECT> */
/*=========================================================================

FUNCTION     : HDRSRCH_GET_ASP_PN_MAPPING

DESCRIPTION  : This function returns the PN offsets and ASP indices of all 
               the active set pilots, in an array passed by the caller.

DEPENDENCIES : None

INPUT        : A pointer to an array of type hdrsrch_asp_to_pn_info_type.

RETURN VALUE : Number of ASET pilots, from 0 to 6.
               PN and ASP info will be updated in the array passed by caller.
               
SIDE EFFECTS : None

=========================================================================*/

uint8 hdrsrch_get_asp_pn_mapping
( 
  hdrsrch_asp_to_pn_info_type * aset_pn
);


/* <EJECT> */
/*=========================================================================

FUNCTION     : HDRSRCH_GET_PN_ECIO

DESCRIPTION  : This function returns the PN offsets and EcIo of all the 
               active set pilots, in an array passed by the caller. The
               range of EcIo returned is from 0 to 63, in -0.5db unit.

DEPENDENCIES : None

INPUT        : A pointer to an array of type hdrsrch_pn_ecio_info_type.

RETURN VALUE : Number of ASET pilots, from 0 to 6.
               PN and EcIo info will be updated in the array passed by caller.

SIDE EFFECTS : None

=========================================================================*/
 
uint8 hdrsrch_get_pn_ecio
( 
  hdrsrch_pn_ecio_info_type* pn_ecio
);


/* <EJECT> */
/*=========================================================================

FUNCTION     : HDRSRCH_COLOR_CODE_UPDATE

DESCRIPTION  : This function informs the searcher of the color code for the
               current link

DEPENDENCIES : None

RETURN VALUE : None

SIDE EFFECTS : None

=========================================================================*/

void hdrsrch_color_code_update
(
  sys_link_type link_info,
    /* link which needs to be updated */

  uint8         color_code
    /* color code value for the link */
);


/* <EJECT> */
/*=========================================================================

FUNCTION     : HDRSRCH_SET_TC_MODE

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

void hdrsrch_set_tc_mode
( 
  hdrsrch_tc_mode_enum_type  tc_mode
    /* Traffic mode that HDR should transition to. */
);

/* <EJECT> */
/*=========================================================================

FUNCTION     : HDRSRCH_GET_TC_MODE

DESCRIPTION  : This function gets the current traffic mode

DEPENDENCIES : None

PARAMETERS   : None
 
RETURN VALUE : 

   HDRSRCH_DEFAULT_TC_MODE   - Standard HDR traffic connection. 

   HDRSRCH_S1XHDR_HP_TC_MODE - High Priority Traffic Mode where
                               HDR preempts 1x paging onto 
                               chain 1 and preempts acquisition.

SIDE EFFECTS : None

=========================================================================*/

hdrsrch_tc_mode_enum_type hdrsrch_get_tc_mode ( void );

#ifdef FEATURE_HDR_IS890

/*=========================================================================

FUNCTION     : HDRSRCH_DIS_TC_OFS

DESCRIPTION  : This function is called if any application ( so far FTAP or
               RTAP ) wants to disable TC OFS. TC OFS is enabled by default 
               whenever HDRSRCH enters traffic state. 

DEPENDENCIES : None

PARAMETERS   : None

RETURN VALUE : None

SIDE EFFECTS : None

=========================================================================*/

void hdrsrch_disable_tc_ofs ( void );


/*=========================================================================

FUNCTION     : HDRSRCH_SET_TAP

DESCRIPTION  : This function is called if any TAP application is running. 

DEPENDENCIES : None

PARAMETERS   : 
               
  tap  - whether TAP is running.

RETURN VALUE : None

SIDE EFFECTS : None

=========================================================================*/

void hdrsrch_set_tap
(
  boolean                    tap
    /* Whether tap is running */
);

/*=========================================================================

FUNCTION     : HDRSRCH_CLEAR_HANDOFF_COUNT

DESCRIPTION  : 
  This function clears out the HO count in the dsp but makes sure
  hdrsrch ho count is maintained properly for logging purposes. 

DEPENDENCIES : None

PARAMETERS   : 
  subactive_set_idx - SubActiveSet index to clear.

RETURN VALUE : None

SIDE EFFECTS : 
  Updates hdrsrchlog with ho count information.

=========================================================================*/

void hdrsrch_clear_handoff_count
( 
  uint8 subactive_set_idx
    /* SubActiveSet Index to clear */
);

/*=========================================================================

FUNCTION     : HDRSRCH_GET_HANDOFF_COUNT

DESCRIPTION  : 
  This function gets the HO count from the dsp.

DEPENDENCIES : None

PARAMETERS   : 
  subactive_set_idx - SubActiveSet index to get handoff count.

RETURN VALUE : 
  Handoff count from the DSP.

SIDE EFFECTS : None
  
=========================================================================*/

uint32 hdrsrch_get_handoff_count
( 
  uint8 subactive_set_idx
    /* SubActiveSet Index to get handoff count */
);

#endif /* FEATURE_HDR_IS890 */

/* <EJECT> */
/*=========================================================================

FUNCTION     : HDRSRCH_HAS_MULTI_RX

DESCRIPTION  : This function queries if mulitple RX chains are supported.

DEPENDENCIES : None

PARAMETERS   : None

RETURN VALUE : 

  TRUE  - support multiple receive chains
  FALSE - support single receive chain

SIDE EFFECTS : None

=========================================================================*/

boolean hdrsrch_has_multi_rx( void );


/* <EJECT> */
/*=========================================================================

FUNCTION     : HDRSRCH_ASET_HAS_MORE_THAN_ONE_CELL

DESCRIPTION  : This function queries to find if the active set has more than
               one cell.

DEPENDENCIES : None

PARAMETERS   : subaset_idx - the subaset index of the subaset requseted.

RETURN VALUE : 

  TRUE  - ASET has sectors from more than one cell
  FALSE - ASET has sectors from only one cell

SIDE EFFECTS : None

=========================================================================*/

boolean hdrsrch_aset_has_more_than_one_cell
(
#ifdef FEATURE_HDR_REVB
  uint8 subaset_idx
    /* The subaset index of the query */
#else
  void 
#endif /* FEATURE_HDR_REVB */
);

#ifdef FEATURE_HDR_FORCE_PREAMBLE_DET
/* <EJECT> */
/*=========================================================================

FUNCTION     : HDRSRCH_NOTIFY_CC_PACKET

DESCRIPTION  : This function notifies searcher that a CC packet has been 
               received. It can be with good CRC or bad CRC.
                
DEPENDENCIES : None

PARAMETERS   : None

RETURN VALUE : None

SIDE EFFECTS : None

=========================================================================*/

void hdrsrch_notify_cc_packet( void );

#endif /* FEATURE_HDR_FORCE_PREAMBLE_DET */

/* <EJECT> */
/*=========================================================================

FUNCTION     : HDRSRCH_REGISTER_VOIP_OUTAGE_CB

DESCRIPTION  : This function registers a callback function with searcher 
               to be called when a VOIP outage notification event 
               is to be generated.
                
DEPENDENCIES : None

PARAMETERS   : None

RETURN VALUE : None

SIDE EFFECTS : None

=========================================================================*/

void hdrsrch_register_voip_outage_cb
( 
  hdrsrch_voip_outage_notify_cb_type    voip_outage_cb
);

/* <EJECT> */
/*=========================================================================

FUNCTION     : HDRSRCH_CONFIGURE_AT_OPERATIONAL_MODE

DESCRIPTION  : This function registers callback functions with searcher 
               to be called when an AT operational mode management
               notification event occurs. It also configures
               whether two enable poor DO coverage and good DO coverage
               algorithms.
                
DEPENDENCIES : None

PARAMETERS   : None

RETURN VALUE : None

SIDE EFFECTS : None

=========================================================================*/

void hdrsrch_configure_at_operational_mode
(
  hdrsrch_opmode_config_enum_type      config,
    /* which at operational mode algorithm if any to run */
    
  hdrsrch_opmode_indication_cb_type    poor_coverage_cb,
    /* cb to indicate poor HDR coverage */

  hdrsrch_opmode_indication_cb_type    good_coverage_cb
    /* cb to indicate good HDR coverage */
);

/* <EJECT> */
/*=========================================================================

FUNCTION     : HDRSRCH_SET_TC_HANDDOWN_MODE

DESCRIPTION  : This function sets the handdown mode during  HDR connected
               state. Handdown can be enabled or disabled 
               during HDR traffic connection.

  HDRSRCH_1X_HANDDOWN_DISABLED_MODE - Handdown to is 1x disabled.

  HDRSRCH_1X_HANDDOWN_ENABLED_MODE  - Handdown to 1x is enabled.

DEPENDENCIES : None

PARAMETERS   : None

RETURN VALUE : None

SIDE EFFECTS : None

=========================================================================*/

void hdrsrch_set_tc_handdown_mode
( 
  hdrsrch_tc_handdown_mode_enum_type   handdown_mode
    /* the handdown mode selected */
);

/* <EJECT> */
/*=========================================================================

FUNCTION HDRSRCH_DIVERSITY_COMB_ENABLED

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

boolean hdrsrch_diversity_comb_enabled( void );

/* <EJECT> */
/*=========================================================================

FUNCTION     : HDRSRCH_UPDATE_NUM_SECTOR_USERS

DESCRIPTION
  Notifies the searcher with the number of active users in serving cell.
  This number is retrieved from FTCValid bits information in
  sector's quick config message.
    
DEPENDENCIES
  None

INPUTS
  Number of active users in serving sector.

RETURN VALUE
  None
  
SIDE EFFECTS
  None

=========================================================================*/

void hdrsrch_update_num_sector_users
( 
  uint16 num_active_users
    /* number of active users in serving sector */
);

#ifdef FEATURE_HDR_CGPS_STATUS
/* EJECT */
/*===========================================================================

FUNCTION HDRSRCH_NOTIFY_CGPS_STATUS

DESCRIPTION
  This function is called to notify hdr srch of CGPS's active status.

DEPENDENCIES
  None

PARAMETERS
  cgps_isactive - TRUE if CGPS is active, FALSE otherwise

RETURN VALUE
  None

SIDE EFFECTS
  Sets hdr srch internal cgps status.

===========================================================================*/
extern void hdrsrch_notify_cgps_status( boolean cgps_isactive );
#endif /* FEATURE_HDR_CGPS_STATUS */


/*===========================================================================

FUNCTION HDRSRCH_GET_TX_CARRIER_FREQ

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
uint32 hdrsrch_get_tx_carrier_freq
(
  sys_band_class_e_type      band,
    /* Band */

  sys_channel_num_type       chan_num
    /* Channel number */
);


/*===========================================================================

FUNCTION HDRSRCH_GET_RX_CARRIER_FREQ

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
uint32 hdrsrch_get_rx_carrier_freq
(
  sys_band_class_e_type      band,
    /* Band */

  sys_channel_num_type       chan_num
    /* Channel number */
);

/*=========================================================================

FUNCTION HDRSRCH_IS_BAND_CHAN_SUPPORTED

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
boolean hdrsrch_is_band_chan_supported 
( 
  sys_band_class_e_type   band, 
    /* Band */
  word                    chan
    /* Channel number */
);

/*============================================================================

FUNCTION HDRSRCH_GET_BAND_SUBCLASS

DESCRIPTION
  This function gets the currently supported band subclasses of the given band.
  It is a wrapper of function hdrsrchrf_get_band_subclass().
  
DEPENDENCIES
  None
  
RETURN VALUE
  The mask of the current subclasses of the given band.

SIDE EFFECTS
  None

============================================================================*/
uint32 hdrsrch_get_band_subclass
(
  uint8    band_class
    /* Band Class */
);


#ifdef FEATURE_HDR_ADAPTIVE_DIV
#error code not present
#endif /* FEATURE_HDR_ADAPTIVE_DIV */


/*===========================================================================

FUNCTION HDRSRCH_GET_ASP_INDEX

DESCRIPTION
  The function returns the asp index that matches the drc cover.

DEPENDENCIES
  None

PARAMETERS
  demod_idx - demod carrier index
  drc_cover - drc cover

RETURN VALUE
  return asp index that matches drc cover or invalid asp index if no match.

SIDE EFFECTS
  None

===========================================================================*/

hdrsrch_demod_idx_type hdrsrch_get_asp_index
(
  uint8                      demod_idx,
    /* Demod carrier index */

  hdrmac_drc_cover_type      drc_cover
    /* drc cover */
);


/*==========================================================================

FUNCTION HDRSRCH_PILOT_MEAS_REQUEST

DESCRIPTION
  Posts a pilot measurement request command to searcher. This command allows
  the application to get pilot measurement of a list of PNs in one channel.
  It is similar to RUP request, but pilot measurement requests is more flexible.
  RUP request is trigger by Route Update Request Message from network. Here is 
  the list of difference of pilot measurement request command and RUP command:
     - RUP only measures pilots in CSET/ASET/NSET if the channel is same as
       aset channel. Pilot measurement request could measure any PN in the same
       channel_change.
     - RUO could be called in IDLE or Traffic state. Pilot measurement could be 
       only called in IDLE
     - hdrsrch_get_pilot_report returns the result for RUP request. h
       hdrsrch_get_pilot_meas_report returns the result for pilot measurement

DEPENDENCIES
  None
  
PARAMETERS
  pilot_meas_request   - Parameters of the pilot measurement request.
  pilot_meas_rpt_cb    - Pilot measurement report indication callback

RETURN VALUE
  None

SIDE EFFECTS
  None
  
==========================================================================*/

void hdrsrch_pilot_meas_request
(
  const hdrsrch_pilot_meas_req_params_type   *pilot_meas_request,
    /* pointer to pilot measurement request params structure */
    
  hdrsrch_pilot_rpt_cb_type                  pilot_meas_rpt_cb
    /* Pilot report indication callback, or NULL to disable */
);

/*==========================================================================

FUNCTION HDRSRCH_GET_PILOT_MEAS_REPORT

DESCRIPTION
  Retrieves a pilot measurement result.

DEPENDENCIES
  

PARAMETERS
  ref_pn          - Reference pilot pn
  ref_pn_channel  - Channel number of reference pilot pn
  ref_pn_strength - Pilot strenth of reference pilot pn
  max_pilots      - Maximum number of pilots which will be retrieved.
  pilots          - Array of length max_pilots, in which the pilot report
                    information will be stored.

RETURN VALUE
  number of returned pilot in pilots array.

SIDE EFFECTS
  

==========================================================================*/

int hdrsrch_get_pilot_meas_report
(
  sys_pilot_pn_type              * ref_pn,
    /* reference pilot pn */

  sys_channel_type               * ref_pn_channel,
    /* reference pilot pn channel number */

  int16                          * ref_pn_strength,
    /* reference pilot pn strength */

  int                             max_pilots,
    /* Maximum # of pilot entries which may be placed in "pilots" array */

  hdrsrch_pilot_meas_rpt_type    * pilots
    /* Array to store pilot information in */
);


/*===========================================================================

FUNCTION HDRSRCH_GET_SUB_ASET_INDEX

DESCRIPTION
  The function returns the subaset index of the given demod carrier.

DEPENDENCIES
  None

PARAMETERS
  demod_idx - Index of the demod carrier

RETURN VALUE
  Sub Active Set index

SIDE EFFECTS
  None

===========================================================================*/

hdrsrch_subaset_idx_type hdrsrch_get_subaset_index
(
  uint8                      demod_idx
    /* Demod index */
);

/*==========================================================================

FUNCTION      HDRSRCH_GET_CC_START

DESCRIPTION
  This function returns the CC start value in slot
  
DEPENDENCIES
  None 

INPUTS
  None

RETURN VALUE
  CC start in slot

SIDE EFFECTS
  None

==========================================================================*/

uint8 hdrsrch_get_cc_start( void );


/*==========================================================================

FUNCTION      HDRSRCH_SET_CC_START

DESCRIPTION
  This function sets the CC start and sends the message to tell firmware 
  when to look for CC or SCC preambles.
  
DEPENDENCIES
  This function needs to be called after AT wakes up. 

INPUTS
  cc_start - CC start in slot

RETURN VALUE
  None

SIDE EFFECTS
  None

==========================================================================*/

void hdrsrch_set_cc_start
(
  uint8               cc_start
    /* Time in slot that firmware expects to find CC/SCC premable */
);


/*==========================================================================

FUNCTION HDRSRCH_SET_SCC_OFFSET

DESCRIPTION
  This function sends the message to the firmware to set scc offset.

DEPENDENCIES
  None

PARAMETERS
  scc_offset - SCC offset
  
RETURN VALUE
  None 

SIDE EFFECTS
  None
  
==========================================================================*/

void hdrsrch_set_scc_offset
(
  uint16                scc_offset
    /* SCC offset */
);


/*==========================================================================

FUNCTION HDRSRCH_SET_SCC_INFO

DESCRIPTION
  This function sets CC info to hdr srch.

DEPENDENCIES
  None

PARAMETERS
  scc_offset - SCC offset
  cc_rate    - SCC rate
  link_id    - Link Id
  
RETURN VALUE
  None 

SIDE EFFECTS
  None
  
==========================================================================*/

void hdrsrch_set_scc_info
(
  uint16                scc_offset,
    /* SCC offset */

  uint8                 cc_rate,
    /* SCC rate */

  uint8                 link_id
    /* Link id */
);


/*==========================================================================

FUNCTION HDRSRCH_ENABLE_FW_HDR_APP

DESCRIPTION
  This function sends the command to HDR SRCH task to enable CDMA clocks and
  the firmware HDR App.

DEPENDENCIES
  None

PARAMETERS
  call_back - Call back function
  
RETURN VALUE
  None 

SIDE EFFECTS
  None
  
==========================================================================*/

void hdrsrch_enable_fw_hdr_app
(
  hdrsrch_enable_fw_hdr_app_cb_type  call_back,
    /* Call back function */
#ifdef FEATURE_RF_SVDO_API 
  hdrsrch_fw_state_type               hdr_fw_next_state
#else
  hdrfw_state_t                       hdr_fw_next_state
#endif /* FEATURE_RF_SVDO_API */
   /* hdr fw new state */
);

/*==========================================================================

FUNCTION HDRSRCH_SEND_IRAT_MSG_TO_FW

DESCRIPTION
  This function sends the PilotMeasCfgReq to HDR FW. It is an API for FTM.

DEPENDENCIES
  None

PARAMETERS
  call_back - Call back function
  
RETURN VALUE
  None 

SIDE EFFECTS
  None
  
==========================================================================*/

void hdrsrch_send_irat_msg_to_fw( void );

/*==========================================================================

FUNCTION HDRSRCH_FAST_ACQ

DESCRIPTION
  Attempts to acquire a given pn with CDMA system time transfer

DEPENDENCIES
  None

PARAMETERS
  fast_acq_params  -- structure to hold parameters         

RETURN VALUE
  None

SIDE EFFECTS
  None

==========================================================================*/

void hdrsrch_fast_acq
(
  hdrsrch_fast_acq_params_type    *fast_acq_params
    /* fast acq parameters */
);

#ifdef FEATURE_HDR_LTE_TO_EHRPD_IRAT
/*==========================================================================

FUNCTION HDRSRCH_TRAN_SYS_TIME

DESCRIPTION
  Attempts to transfer cdma system time

DEPENDENCIES
  None

PARAMETERS
  tran_sys_time_params  -  structure to hold parameters 
  tran_sys_time_cb      -  callback to indicate time transfer status

RETURN VALUE
  None

SIDE EFFECTS
  None

==========================================================================*/

void hdrsrch_tran_sys_time
(
  hdrsrch_tran_sys_time_params_type  *tran_sys_time_params,
    /* parameters  of trans_sys_time command */

  hdrsrch_tran_sys_time_cb_type      tran_sys_time_cb_func
    /* callback function to indicate time transfer done */
);
#endif /* FEATURE_HDR_LTE_TO_EHRPD_IRAT */


/*=========================================================================

FUNCTION     : HDRSRCH_RUNTIME_DIVERSITY_PREF_CTRL

DESCRIPTION  : This function provides the API for upper layers to set the 
               diversity preference at runtime. 
               By default, the preference is div on. If it's called to 
               disable div, diversity will be shut off immediately without 
               DRC ramp down.

DEPENDENCIES : 

INPUT        : TRUE-allow diversity. FALSE-force off. 
 
RETURN VALUE : None

SIDE EFFECTS : None

=========================================================================*/

void hdrsrch_runtime_diversity_pref_ctrl
(
  hdrsrch_rx_div_enum_type       div_ctrl
    /* Runtime diversity control */
);

#ifdef FEATURE_HDR_TO_LTE
/*==========================================================================

FUNCTION HDRSRCH_LTE_NSET_UPDATE

DESCRIPTION
  Update the LTE neighbor list from the OtherRATNeighborList OTA message

DEPENDENCIES
  OtherRATNeighborList OTA message has its own transmission cycle

PARAMETERS 
  LTE frequency information 

RETURN VALUE
  None

SIDE EFFECTS
  Update the LTE neighbor list for DO to request measurements.  If LTE
  pilot measurements meet reselection criteria, Mobile will reselect to LTE.

==========================================================================*/

void hdrsrch_lte_nset_update
(
  hdrsrch_lte_nset_update_params_type   *lte_nset_update_params
);

/*==========================================================================

FUNCTION HDRSRCH_UPDATE_SECTOR

DESCRIPTION
  This function provides the current sector ID to HDRSRCH for DO to LTE
  reselection purpose.

DEPENDENCIES
  None

PARAMETERS
  Sector ID of the current sector, including the Access Terminal 
    Identifier (ATI), Sector ID, and Subnet ID

RETURN VALUE
  TRUE:  Operation completes successfully
  FALSE:  Unexpected error, operation fails

SIDE EFFECTS
  None

==========================================================================*/
boolean hdrsrch_update_sector
(
  hdraddr_type                    hdr_sector_ptr
);

/*==========================================================================

FUNCTION HDRSRCH_ABORT_RESELECT_TO_LTE 
 
DESCRIPTION
  Inform SRCH that LTE reselection has been aborted

DEPENDENCIES
  None

PARAMETERS 
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

==========================================================================*/

void hdrsrch_abort_reselect_to_lte ( void );

#endif /* FEATURE_HDR_TO_LTE */

/*=========================================================================

FUNCTION     : HDRSRCH_LINEAR_ENG_TO_DB

DESCRIPTION  : This function converts linear energy to db in unit of -0.5db

DEPENDENCIES : None

INPUT        : None

RETURN VALUE : energy in db

SIDE EFFECTS : None

=========================================================================*/

int hdrsrch_linear_eng_to_db
( 
  int                             lin_eng
    /* Linear energy to be converted to -0.5dB units */
);

#ifdef FEATURE_HDR_THERMAL_BACKOFF

/*=========================================================================

FUNCTION     : HDRSRCH_THERMAL_DIVERSITY_PREF_CTRL

DESCRIPTION  : This function provides the API for CCM to set the 
               diversity preference at runtime according to thermal
               based flow control.
               By default, the preference is div on. If it's called to 
               disable div, diversity will be shut off immediately without 
               DRC ramp down.

DEPENDENCIES : 

INPUT        : Whether diversity is on.

RETURN VALUE : None

SIDE EFFECTS : None

=========================================================================*/

void hdrsrch_thermal_diversity_pref_ctrl
(
  boolean       div_ctrl
    /* Runtime diversity control */
);

#endif /* FEATURE_HDR_THERMAL_BACKOFF */


/*============================================================================

FUNCTION HDRSRCH_GET_BAND

DESCRIPTION
  This function gets the band which HDR is currently operating on.
  
DEPENDENCIES
  None
  
RETURN VALUE
  The band HDR is operating on.

SIDE EFFECTS
  None

============================================================================*/
sys_band_class_e_type hdrsrch_get_band( void );


#ifdef FEATURE_HDR_GRICE
/*=========================================================================

FUNCTION HDRSRCH_IS_GRICE_ENABLED

DESCRIPTION 
  This function checks if GRICE is enabled (does not have to be currently
  active to be enabled).

DEPENDENCIES
  None

RETURN VALUE
  TRUE -  If GRICE is enabled.
  FALSE - otherwise.
  
SIDE EFFECTS
  None.

=========================================================================*/
boolean hdrsrch_is_grice_enabled( void );
#endif /* FEATURE_HDR_GRICE */

/*=========================================================================

FUNCTION     : HDRLMAC_GET_RX_TX_POWER_INFO

DESCRIPTION
  Returns HDR RX and TX AGC to the caller. 
 
  Rxagc and txagc will be reported in 1\10 dBm granularity.
 
  Ecio will only be reported for primary chain, in traffic it will be
  instantaneous primar energy, in idle state it will be total energy.
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

void hdrsrch_get_rx_tx_power_info
(
    hdrsrch_rx_pwr_info_struct_type   *rx_power_info,
    hdrsrch_tx_pwr_info_struct_type   *tx_power_info
); /* hdrsrch_get_rx_tx_power_info */


/*=========================================================================

FUNCTION HDRSRCH_CHANGE_SLEEP_TIMELINE

DESCRIPTION 
  This function notifies HDR SRCH to use extended or optimized timeline.

DEPENDENCIES
  None

RETURN VALUE
  TRUE -  optimied timeline.
  FALSE - extended timeline.
  
SIDE EFFECTS
  None.

=========================================================================*/
void hdrsrch_change_sleep_timeline
(
  boolean       optimized
    /* Whether use the extended or optimized timeline */
);


/*==========================================================================

FUNCTION HDRSRCH_TIME_UNTIL_TUNE_COMPLETE

DESCRIPTION
  If the HDR tune is happening within 1x tune duration, return the time that tune will be 
  completed. Otherwise, return 0ms.

DEPENDENCIES
  None

PARAMETERS
 tune_duration_1x_us: 1x tune duration in usec

RETURN VALUE
  0: There is no conflict between 1x and HDR RF tune.
  >0 : There is conflict and the time in usec until the HDR tune completes 

SIDE EFFECTS
  None

==========================================================================*/
uint32 hdrsrch_time_until_tune_complete
(
  uint32 tune_duration_1x_us
    /* 1x tune duration in usec*/
);

#ifdef FEATURE_HDR_DYNAMIC_ACQ_DIV

/*=========================================================================

FUNCTION     : HDRSRCH_ACQ_DIV_CTRL

DESCRIPTION
    Enables or disables the acquisition diversity algorithm which dynamically
    uses the diversity antenna to improve acquisition performance in poor channels.
    If disabled, diversity will never be used in the acquisition state. If enabled,
    it will enable diversity selectively based on the channel condition and whether
    the diversity chain is available.

    A call to this API effects the next acquisition command that is accepted by the
    HDR searcher. It will not impact in progress search attempts.

DEPENDENCIES
  None

INPUTS
  Boolean which indicates whether to enable or disable the acquisition algorithm

RETURN VALUE
  None

SIDE EFFECTS
  None

=========================================================================*/

void hdrsrch_acq_div_ctrl
(
  boolean             acq_div_runtime_ctrl
);
#endif /* FEATURE_HDR_DYNAMIC_ACQ_DIV */

#ifdef FEATURE_HDR_DYNAMIC_IDLE_DIV
/*==========================================================================

FUNCTION HDRSRCH_RECORD_MISSED_CC

DESCRIPTION
  This function lets SRCH know that we missed sync cc.

DEPENDENCIES
  Must only be sent when in IDLE state.

PARAMETERS
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None.

==========================================================================*/

void hdrsrch_record_missed_cc( void );
#endif /* FEATURE_HDR_DYNAMIC_IDLE_DIV */

/*=========================================================================

FUNCTION HDRSRCH_ACQ_SEARCH_ACTIVE

DESCRIPTION 
  This function returns whether HDR Acq search is active.

DEPENDENCIES
  None

RETURN VALUE
  TRUE: Acq srch is active.
  FALSE: Otherwise.
  
SIDE EFFECTS
  None.

=========================================================================*/
boolean hdrsrch_acq_search_active(void);

#ifdef FEATURE_HDR_REVC
/* EJECT */
/*==========================================================================

FUNCTION HDRSRCH_UPDATE_LOADADJ

DESCRIPTION
  Sends loadadj information to FW from up to 3 carriers

DEPENDENCIES

PARAMETERS 
  load_info_ptr - Pointer loadadj information

RETURN VALUE 
  None

SIDE EFFECTS
  None

==========================================================================*/

void hdrsrch_update_loadadj
(
  const hdrsrch_loadadj_type* loadadj_ptr
);

/* EJECT */
/*==========================================================================

FUNCTION HDRSRCH_ENABLE_NLB

DESCRIPTION
  This function notify HDRSRCH to enable/disable NLB. When NLB is
  enabled, CC demod is enabled on all 3 carriers (in traffic), when
  it is disabled, 3 carrier CC demod is turned off, and any LoadAdj
  is reset.

DEPENDENCIES

PARAMETERS 
  enable_nlb - Whether to enable or disable NLB

RETURN VALUE 
  None

SIDE EFFECTS
  None

==========================================================================*/
void hdrsrch_enable_nlb
(
  const boolean enable_nlb
);
#endif /* FEATURE_HDR_REVC */

/*==========================================================================

FUNCTION        HDRSRCH_INIT_DIV_SM

DESCRIPTION
  This function initialize DIV/ARD state machine only once. Subsequent
  calls are no-ops

DEPENDENCIES
  None

INPUTS
  None

RETURN VALUE
  None

SIDE EFFECTS
  Many

==========================================================================*/
void hdrsrch_init_div_sm( void );

/*===========================================================================

FUNCTION HDRSRCH_SET_LRE_ENABLE

DESCRIPTION
  This function sets internal flag that is checked to see if LRE is enabled/disabled
 
DEPENDENCIES 
  None. 

PARAMETERS
  status - LRE enabled/disabled status

RETURN VALUE
  None.

SIDE EFFECTS
  If status flag is FALSE, then LRE feature will be disabled.

===========================================================================*/

void hdrsrch_set_lre_enable
(
  boolean status
);

/*===========================================================================

FUNCTION HDRSRCH_IS_LRE_ENABLED

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

boolean hdrsrch_is_lre_enabled( void );

/*==========================================================================

FUNCTION HDRSRCH_IDLE_CHANNEL_CHANGE_AND_NSET_UPDATE

DESCRIPTION 
  Change to a new HDR channel and update the nset.  This should be called when the HDR AT
  hashes to a new channel and update nset pilots.
  
DEPENDENCIES
  Idle mode

PARAMETERS
  idle_cc_params      - params for channel change.
  nset_update_params  - params for nset update.

RETURN VALUE
  None

SIDE EFFECTS
  RF is tuned to a new channel.

==========================================================================*/

void hdrsrch_idle_channel_change_and_nset_update
(
   hdrsrch_idle_channel_change_type *idle_cc_params,
     /* Idle channel change params*/

   hdrsrch_nset_update_type         *nset_update_params
     /* Nset update params*/
);

/*===========================================================================
FUNCTION HDRSRCH_READ_NV

DESCRIPTION
  Read values of NV.

DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

NOTE
  None
===========================================================================*/
void hdrsrch_read_nv( void );

/*===========================================================================

FUNCTION HDRSRCH_IS_IDLE_TA_STARTED

DESCRIPTION
  This function returns tune away status in Idle. This will be helpful in 
  stopping Fast connect TCA processing while tuning away in Idle.

DEPENDENCIES 
  None. 

PARAMETERS
  None.

RETURN VALUE
  TRUE  - if tune away processing has started.
  FALSE - otherwise

SIDE EFFECTS
  None.

===========================================================================*/

boolean hdrsrch_is_idle_ta_started( void );

#ifdef FEATURE_HDR_QTA
/*==========================================================================

FUNCTION        HDRSRCH_SET_QTA_NV

DESCRIPTION
  This function sets QTA NV status.

DEPENDENCIES
  None

INPUTS
  If QTA is enabled via NV or not.

RETURN VALUE
  None

SIDE EFFECTS
  None

==========================================================================*/
void hdrsrch_set_qta_nv( boolean is_enabled );

/*==========================================================================

FUNCTION        HDRSRCHTC_IS_QTA_NV_ENABLED

DESCRIPTION
  This function gets QTA NV status.

DEPENDENCIES
  None

INPUTS
  None

RETURN VALUE
  Status of QTA NV item

SIDE EFFECTS
  None

==========================================================================*/
boolean hdrsrchtc_is_qta_nv_enabled( void );


/*===========================================================================

FUNCTION HDRSRCH_IS_TA_STARTED

DESCRIPTION
  This function returns tune away status. This is interface function.

DEPENDENCIES 
  None. 

PARAMETERS
  None.

RETURN VALUE
  TRUE  - if tune away processing has started.
  FALSE - otherwise

SIDE EFFECTS
  None.

===========================================================================*/

boolean hdrsrch_is_ta_started( void );


/*==========================================================================

FUNCTION        HDRSRCH_IS_QTA_ENABLED

DESCRIPTION
  This function returns whether we are in QTA currently.

DEPENDENCIES
  None

INPUTS
  None

RETURN VALUE
  TRUE: in QTA
  FALSE: in LTA

SIDE EFFECTS
  None

==========================================================================*/
boolean hdrsrch_is_qta_enabled( void );


/*===========================================================================

FUNCTION HDRSRCH_CONNECTION_CLOSE_STATE

DESCRIPTION
  This function intimates the connection close state to HDR-SRCH
 
DEPENDENCIES 
  None. 

PARAMETERS
  Connection close state
  0 - No connetion close
  1 - Connection close inprogress

RETURN VALUE
  None.
  
SIDE EFFECTS
  None.

===========================================================================*/

void hdrsrch_connection_close_state( boolean connection_close_state );

#endif /* FEATURE_HDR_QTA */

/*============================================================================

FUNCTION HDRSRCH_IS_IN_SUSPEND

DESCRIPTION
  This function provide a boolean value indicating if HDR is in suspend
  states.  Currently, there are 3 suspend states as listed below.
  
DEPENDENCIES
  None
 
PARAMETERS
  None

RETURN VALUE
  TRUE:  HDR is in one of these states:
         HDRSRCH_SUSPENDED_TC_STATE    
         HDRSRCH_SUSPENDED_IDLE_STATE
         HDRSRCH_SUSPENDED_SYNC_STATE
  FALSE: HDR is in another state
 
SIDE EFFECTS
  None
 
============================================================================*/
boolean hdrsrch_is_in_suspend( void );

/*=========================================================================

FUNCTION     : HDRSRCH_SYSTEM_LOST

DESCRIPTION  : This function declares system lost, and transfers back to
               the inactive state

DEPENDENCIES : None

RETURN VALUE : None

SIDE EFFECTS : Exits to inactive.

=========================================================================*/

void hdrsrch_system_lost
(
  hdrsrch_system_lost_enum_type   reason
    /* Reason for declaring system lost */
);

#endif /* HDRSRCH_H */

