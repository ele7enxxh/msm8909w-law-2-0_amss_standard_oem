#ifndef HDRSRCHUTIL_H
#define HDRSRCHUTIL_H

/*===*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

           H D R   S E A R C H   U T I L I T Y   F U N C T I O N S

GENERAL DESCRIPTION
  This module provides general purpose routines used in various states.


EXTERNALIZED FUNCTIONS (Global)


EXTERNALIZED FUNCTIONS (Regional)

  hdrsrchutil_init

  hdrsrchutil_program_search_set
  hdrsrchutil_program_searches
  hdrsrchutil_enable_roll
  hdrsrchutil_disable_roll
  
  hdrsrchutil_suspend_rpt

  hdrsrchutil_calc_time_to_tune_away
  hdrsrchacq_convert_ppm_to_hz
  hdrsrchutil_set_cgps_status
  hdrsrchutil_get_cgps_status
  hdrsrchutil_send_cgps_ack

INITIALIZATION AND SEQUENCING REQUIREMENTS


  Copyright (c) 2000 - 2015 by Qualcomm Technologies, Incorporated.  All Rights Reserved.

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

============================================================================

                        EDIT HISTORY FOR MODULE

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/hdr/srch/inc/hdrsrchutil.h#1 $ $DateTime: 2016/12/13 07:58:05 $ $Author: mplcsds1 $

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

when         who     what, where, why
----------   ---     --------------------------------------------------------- 
10/27/2015   vke     Moved IIR_FILT_N macro definition to header file
11/04/2014   arm     Band avoidance: move power info saving to hdrsrch task and
                     enable BA only for DSDA mode.
10/13/2013   arm     Updated hdrsrchutil_calc_time_to_tune_away to return if
                     TA time was updated or not
10/07/2014   rmv     For Bringup of JO hw at reduced clks, limit the max fingers
10/01/2014   wsh     Added Full PN search for reacq and IRAT searches
06/13/2014   vlc     Added support for Enhanced DLNA. 
05/05/2014   ljl     Added HDRSRCH_CHANNEL_CHANGE_AND_NSET_UPDATE_CMD. 
09/25/2013   wsh     Sleep changes for bolt
08/30/2013   dsp     Removed mcdo_check()from NPA featurization.
08/06/2013   rmg     Added support to convey power down reason in reverse link
                     power down command.
07/31/2013   sat     Mainlining Modem Statistics Feature
07/16/2013   wsh     Disable QPCH upon access 
06/26/2013   wsh     Added Selective TxD feature
05/24/2013   rmg     Added D-LNA support.
04/22/2013   arm     Added changes for DSDA coexistence manager. 
05/01/2013   arm     Increased number of slots needed for reacq diversity. 
03/17/2013   arm     Reduced number of slots needed for reacq diversity.
02/26/2013   mbs     Fixed featurization for IRAT code
02/16/2013   arm     Do default search if enabling div for reacq timeouts
                     or current rtc and pilot pos are not within 1 ms. 
04/02/2012   arm     Supported diversity in reacq. 
03/08/2012   wsh     Merged NLB support
02/14/2012   smd     Moved pwr_strb_event_type from modem/api/hdr/hdrl1.h.
12/08/2011   arm     Removed magic number for default psinr value.
12/07/2011   ljl     Added hdrsrchutil_convert_linear_sinr_to_db().
11/29/2011   arm     Removed compiler warnings.
10/25/2011   ljl     Handled the failure cases during optimized HO.
10/24/2011   wsh     Updated cancel FPD procedure 
04/27/2011   rmg     Added HDR MCPM support changes.
04/25/2011   ljl     Added hdrsrchutil_powerdown_tx().
03/03/2011   arm     Fixed NPA crash due to overlapping NPA calls.  
01/08/2011   rkc     Supported returning SINR in 0.5 dB units. 
11/21/2010   pxu     Added functions for dynamic sleep time algorithm. 
06/25/2010   rkc     Added NPA MDSP clock speed support.
06/21/2010   cnx     Fixed compiler warnings.
04/21/2010   smd     Added hdrsrchutil_get_rup_chan().
04/08/2010   smd     Added hdrsrch_wait_for_sigs() in mainline.
03/08/2010   lyl     Added hdrsrchutil_get_txagc().
01/06/2010   rkc     Added NPA support for traffic state and removed
                     HDRSRCHUTIL_NPA_CPU_ACTIVATED support.
11/11/2009   rkc     Added NPA support.
10/12/2009   grl     Stated new preconditions for deactivating the decoder.
12/17/2009   smd     Added LTE pilot measurement.
10/04/2009   kss     Fixed ifdef issue with hdrsrchutil_rab_init_ctrl_enum_t. 
08/12/2009   wsh     Reverted CMI related change due to restructuring
07/21/2009   wsh     Exposed datatypes for sleepmod
07/06/2009   ljl     Removed the MDSP cmd interface to access RTC.
05/18/2009   lyl     Updated for firmware MSGR interface.
03/20/2009   rkc     Added rup_subtype param to hdrsrchutil_update_nset_pilots().
03/11/2009   rkc     Added hdrsrch_wait_for_sigs().
03/09/2009   smd     Added support for pilot measurement velcro solution.
02/02/2009   smd     Included NSET pilots when counting strong pilots for small
                     SCIs (SCI <= 6).
01/26/2009   lyl     Logged the number of reacq search pilots in enh_sleep_info.
12/19/2008   lyl     Added hdrsrchutil_set_handoff_control();
11/03/2008   grl     Moved ARDA code into hdrsrchrf module.
08/22/2008   lyl     Supported primary chain ASET search.
08/19/2008   smd     Added idle handoff improvement.
07/25/2008   rkc     Made sure duplicate NSET pilot is not added to srch list.
07/07/2008   ljl     Supported 1x handdown in Rev B.
05/09/2008   rkc     Added hdrsrchutil_get_demod_carrier_mask()
04/28/2008   ljl     Removed hdrsrchutil_diversity_init().
04/17/2008   rkc     Added CGPS set/get status functions.
03/27/2008   ljl     Added the avoided channel list to nset update function.
02/21/2008   ljl     Preferred the same subnet handoff.
02/20/2008   ljl     Removed the parameter from hdrsrchutil_get_hdr_rssi().
01/30/2008   jyw     Modified the hdrsrchutil_match_aset_channels.
12/26/2007   jyw     Fixed compiling warnings.
12/17/2007   jyw     Added hdrsrchutil_get_ref_sector_ptr.
11/13/2007   jyw     Added hdrsrchutil_read_rtc() and hdrsrchutil_restore_rtc()
11/13/2007   jyw     Added the duration info into the power strobe logs.
10/17/2007   jyw     Merged Rev B changes from branch.
09/20/2007   mt      Added hdrsrchutil_update_num_sector_users().
09/05/2007   grl     Set mac indexes to be invalid during idle.
08/08/2007   mt      Added macro MS_TO_HALFSLOTS.
06/18/2007   rkc     Added hdrsrchacq_convert_ppm_to_hz()
04/10/2007   mt      Enhanced OFS, RUP request and ATOM changes.
10/27/2006   mt      Added hdrsrchutil_ofs_count_strong_pilots().
09/08/2006   ljl     Removed hdrsrchutil_fast_dec_deactivate().
08/23/2006   grl     Added API for reging the missed preamble isr.
08/16/2006   mt      Added macro to convert halfslots into microseconds.
08/08/2006   mt      Added hdrsrchutil_get_agc_rtcoffset() function prototype.
08/08/2006   sq      Moved #define to header file
08/01/2006   grl     Perform fast decoder deactivation only when sleeping.
07/19/2006   jyw     Added Added hdrsrchutil_force_wincenter_to_mstr.
06/30/2006   etv     Supported DSP Handoff algorithm changes.
06/07/2006   ljl     Added SBHO optimization
09/28/2005   ljl     Added support of RA channel gain
09/06/2005   ljl     Saved cached sector info
09/06/2005   ljl     Added optimization after system loss
09/01/2005   grl     Added hdrsrchutil_calc_time_to_tune_away function
07/28/2005   ds      Added hash channel list inputs to nset update
07/25/2005   ljl     Added broadcast and directed nset update 
06/30/2005   sq      Added hdrsrchutil_check_ofs_condition
06/28/2005   jyw     Added definitions for HDR sleep/power time line profiling 
05/31/2005   grl     Added SLOTS_TO_SCLK_TICKS macro.
05/12/2005   ds      Added return value for hdrsrchutil_program_search_set
03/23/2005   hrk     Added function hdrsrchutil_dec_deactivate.
02/01/2005   sq      Added function hdrsrchutil_get_sinr_and_rxagc
01/25/2005   sq      Added debug feature DEBUG_HDR_SAMPLE_RAM_DUMP
01/04/2005   sq      Mainlined FEATURE_HDR_TC_OFS
11/09/2004   sq      Added function hdrsrchutil_reset_srch_filters
11/04/2004   kss     Added hdrsrchutil_handle_pn_roll().
11/03/2004   mpa     Removed dependency on std.h (merged with sys.h)
07/30/2004   sq      Support for FEATURE_HDR_BCMCS. Internal API changes.
05/05/2004   aaj     Implemented DSP halt work-around to halt on known codes
04/02/2004   sq      Returns Ec/Io and Io for HDR
04/05/2004   aaj     Added typedef for color code
03/04/2004   aaj     Support for subnet handoff utility function
01/06/2004   aaj     Pass back the A/C/N energies in pilot metric utility func.
11/26/2003   aaj     Handle Lost Dump 
11/21/2003   sq      Added start/stop timer functions
11/18/2003   aaj     Created a separate function to get pilot metric
10/14/2003   aaj     Added validation of searcher results to avoid bogus PNs
10/06/2003   aaj     Mainlined HDRSRCH_MULTIPATH_WIN_CENTER
09/16/2003   sq      Added function hdrsrchutil_tc_ofs_allowed()
03/11/2003   ajn     Added hdrsrchutil_limit_window_size( ) function
03/11/2003   ajn     Updated Copyright notice
11/28/2002   aaj     Added support for data level indicator
11/25/2002   ajn     Added suspend report utility function
11/07/2002   ajn     Roll handler moved to hdrts.
09/18/2002   aaj     Support for multipath window centering for HDR
09/18/2002   aaj     Added support for HDR RSSI for displaying signal bars
04/23/2002   aaj     Comment cleanup
02/21/2002   ajn     Altered signature of various search_list functions.
02/14/2002   sst     Added functions to handle filling of search list
02/08/2002   sst     Modified hdrsrchutil_prepare_reacq_list() to support
                      multiple reacq searches
01/17/2002   aaj     support for RF band to the hdr sector structure type
12/18/2001   aaj     Added general purpose stop watch mechanism
11/26/2001   aaj     Undefine TIMETEST from formal builds
11/09/2001   aaj     Timetest support for srch profiling
11/07/2001   ajn     Added prototype for Prepare search reacq list
10/08/2001   ajn     Added hdrsrchutil_system_lost().
09/15/2001   aaj     Added PN roll registration callbacks
09/13/2001   aaj     Added callback to set handoff delay params from FMAC
08/21/2001   aaj     Moved hdrsrchutil_get_sector_report to hdrsrchset.h
07/31/2001   aaj     Added function to handle MDSP halts. diversity support.
04/11/2001   ajn     Added reacquisition search function.
03/02/2001   aaj     External control for antenna diversity
01/05/2001   ajn     TC -> IDLE ASET transition.
11/30/2000   ajn     Code review changes
10/23/2000   ajn     Added pn_x8 normalize function
10/11/2000   ajn     Added aset_idle_handoff func
10/05/2000   ajn     Created this file

==========================================================================*/


/*==========================================================================

                     INCLUDE FILES FOR MODULE

==========================================================================*/

#include "hdr_variation.h"
#include "sys.h"

#ifdef FEATURE_CMI
#include "sys_v.h"
#endif

#include "qw.h"

#include "timetick.h"

#include "hdrl1.h"

#include "hdrsrch_msg.h"
#include "hdrsrchtypes.h"
#include "hdrsrchsect.h"
#include "hdrsrchcmd.h"
#include "hdrsrch_profile.h"
#include "hdrsrchrf.h"
#include "hdrsrchi.h"

/*--------------------------------------------------------------------------

--------------------------------------------------------------------------*/


/*==========================================================================

                     DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, typesdefs,
and other items needed by this module.

==========================================================================*/

/* Runs an "(n-1)/n" style IIR filter. */
#define  IIR_FILT_N( val, new_val, n ) \
  ((word) (( ((dword) (val) * ((n)-1)) + (dword) (new_val) ) / (n)))

#define MS_TO_SLOTS(ms)               (((ms)*3+4)/5)
  /* Convert milliseconds into slots */

#define MS_TO_HALFSLOTS(ms)           (((ms)*6)/5)
  /* Convert milliseconds into slots */
  
#define SLOTS_TO_MS( slots )          (((slots)*5)/3)
  /* Convert slots to milliseconds */

#define HALFSLOTS_TO_MS( halfslots )  (((halfslots)*5)/6)
  /* Convert halfslots to milliseconds */

#define SLOTS_TO_SCLK_TICKS( slots ) ( SCLK_FROM_MS ( SLOTS_TO_MS ( slots ) ) )
  /* Convert slots to slow clock ticks */

#define MS_TO_CHIPX2  2458 
  /* 2458 Cx2 = 1ms*1.2288 Mcps*2 */

/* Macros for easy handling of uint64 */
#define I64_HI(n)                   (((n) >> 32) & 0xFFFFFFFF)
#define I64_LO(n)                   ((n) & 0xFFFFFFFF)

#ifdef FEATURE_HDR_REVA_L1
#define HDRSRCH_RA_CHANNEL_GAIN_LIN_DEFAULT    1646
  /* The default of RA channel gain is -12dB */
#endif /* FEATURE_HDR_REVA_L1 */

#define HDRSRCH_INVALID_TRAFFIC_MAC_INDEX      0
  /* Invalid traffic MAC index */


#ifdef FEATURE_NPA
/*---------------------------------------------------------------------
  NPA CPU request type bitmap
    Bit 0 - Wakeup
    Bit 1 - Traffic
---------------------------------------------------------------------*/
#define HDRSRCHUTIL_NPA_CPU_WAKEUP            1
#define HDRSRCHUTIL_NPA_CPU_TRAFFIC           1 << 1

/* NPA MDSP CLOCK SPEEDS*/
typedef enum
{
  HDRSRCHUTIL_NPA_MDSP_CLK_72MHZ        = 72,
  HDRSRCHUTIL_NPA_MDSP_CLK_144MHZ       = 144
} hdrsrchutil_npa_mdsp_clk_type;

#endif /* FEATURE_NPA */


/*---------------------------------------------------------------------
  Stop watch to keep track of time spent between any two events
---------------------------------------------------------------------*/

typedef struct
{
  qword                      start_ms;
    /* start time of some event */

  qword                      end_ms;
    /* end time of some event */

} hdrsrchutil_stop_watch_struct_type;

/* The enum defines the SoftRab init control methods */
typedef enum
{
  HDRSRCH_RAB_NO_CHANGE   = 0,
  HDRSRCH_RESET_QRAB_ONLY = 1,
  HDRSRCH_RESET_QRAB_FRAB = 2
}
hdrsrchutil_rab_init_ctrl_enum_t;

#if defined FEATURE_HDR_DYNAMIC_IDLE_DIV || defined FEATURE_HDR_ACCESS_DIV
#define HDRSRCHUTIL_IDLE_STATE_DIV_PSINR_DEFAULT_VALUE           -27

#define HDRSRCHUTIL_SLOTS_TO_ENABLE_DIV_FOR_REACQ                 5
  /* Number of slots to backup timeline to enable diversity
     for reacq (if first reacq has failed). */

#define HDRSRCHUTIL_DIVERSITY_ENABLE_TIME          \
        SLOTS_TO_MS( HDRSRCHUTIL_SLOTS_TO_ENABLE_DIV_FOR_REACQ )
#endif /* FEATURE_HDR_DYNAMIC_IDLE_DIV */

#ifdef FEATURE_HDRSRCH_POWER_PROFILE
/*--------------------------------------------------------------------------
                  HDR POWER PROFILING
  Following functions and macros are used for power profiling 
  along the HDR sleep line
---------------------------------------------------------------------------*/
#define HDR_PWR_PROFILE_STRB_ENABLE(event_id)    hdrsrchutil_enable_pwr_strb(event_id)
#define HDR_PWR_PROFILE_STRB_TOGGLE(event_id)    hdrsrchutil_toggle_pwr_strb(event_id)

typedef enum
{
  HDRSRCH_PWR_EVENT_TCXO_ON = 0,
  HDRSRCH_PWR_EVENT_RFWU_ISR,
  HDRSRCH_PWR_EVENT_RACQ_INIT,
  HDRSRCH_PWR_EVENT_RF_TUNE_START,
  HDRSRCH_PWR_EVENT_RFWU_DONE_ISR,
  HDRSRCH_PWR_EVENT_RXCHIPX16_ON,
  HDRSRCH_PWR_EVENT_RF_TUNE_DONE,
  HDRSRCH_PWR_EVENT_TUNE_DONE,
  HDRSRCH_PWR_EVENT_RACQ_DUMP,
  HDRSRCH_PWR_EVENT_TIME_VALID,
  HDRSRCH_PWR_EVENT_MON_INIT,
  HDRSRCH_PWR_EVENT_CC_ISR_START,
  HDRSRCH_PWR_EVENT_CC_ISR_END,
  HDRSRCH_PWR_EVENT_SLEEP_INIT,
  HDRSRCH_PWR_EVENT_START_SLEEP,
  HDRSRCH_PWR_EVENT_TCXO_OFF,
  HDRSRCH_PWR_EVENT_MAX 
} hdrsrchutil_pwr_strb_event_type;

/* The enum defines the power strobe events along the sleep time line */

/*===========================================================================

FUNCTION        hdrsrchutil_enable_pwr_strb

DESCRIPTION     
  init the GPIO and the power_strob_status

DEPENDENCIES   
  None

RETURN VALUE   
  None.

SIDE EFFECTS   
  None.

===========================================================================*/

void hdrsrchutil_enable_pwr_strb
( 
  hdrsrchutil_pwr_strb_event_type event_id 
    /* The event id associated with the strobe */
);


/*===========================================================================

FUNCTION        hdrsrchutil_toggle_pwr_strb 

DESCRIPTION     
  If the pwr strobe is enable, then toggle the output
  To not affect the power consumption, before go to sleep, the status should
  toggle to OFF state;

DEPENDENCIES   
  None

RETURN VALUE   
  None.

SIDE EFFECTS   
  None.

===========================================================================*/

void hdrsrchutil_toggle_pwr_strb
( 
  hdrsrchutil_pwr_strb_event_type event_id 
    /* The event id associated with the strobe */
);

#endif /* FEATURE_HDRSRCH_POWER_PROFILE */


/*==========================================================================

                      FUNCTION  DEFINITIONS

==========================================================================*/

/*==========================================================================

FUNCTION HDRSRCH_RF_TUNE

DESCRIPTION
  This function tunes the RF to the specified channel

DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  None

SIDE EFFECTS
  Adds the "TUNE" signal to the "wait mask"

==========================================================================*/

void hdrsrchutil_rf_tune
(
  const sys_channel_type  *channel
    /* Channel to tune to */
);


/*==========================================================================

FUNCTION HDRSRCH_RF_TUNE_DONE

DESCRIPTION
  Used to cleanup after tuning is complete.

DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  None

SIDE EFFECTS
  Removes the "TUNE" signal to the "wait mask"

==========================================================================*/

void hdrsrchutil_rf_tune_done( void );


/* <EJECT> */
/*=========================================================================

FUNCTION     : HDRSRCHUTIL_INIT_SEARCH_LIST

DESCRIPTION  : This function prepares for the creation of a search list

DEPENDENCIES : None

RETURN VALUE : None

SIDE EFFECTS : None

=========================================================================*/

void hdrsrchutil_init_search_list( void );



/* EJECT */
/*=========================================================================

FUNCTION     : HDRSRCHUTIL_ADD_ASET_TO_SEARCH_LIST

DESCRIPTION  : This function adds the aset to the current search list

DEPENDENCIES : None

INPUT        : None

RETURN VALUE : The number of aset sectors added into the search list. 

SIDE EFFECTS : Fills search list

=========================================================================*/

#ifndef FEATURE_HDR_REVB
uint8 hdrsrchutil_add_aset_to_search_list
(
  const sys_channel_type          * chan
    /* Channel from which aset pilots must be selected */
);
#else
uint8 hdrsrchutil_add_aset_to_search_list
( 
  const sys_channel_type *          chan,
    /* Channel from which aset pilots must be selected */

  uint8                             chan_cnt
    /* Channel Count */
);
#endif /* FEATURE_HDR_REVB */


/*=========================================================================

FUNCTION     : HDRSRCHUTIL_ADD_CSET_TO_SEARCH_LIST

DESCRIPTION  : This function adds the cset to the current search list

DEPENDENCIES : None

INPUT        : chan - Ptr to channel from which cset pilots must be selected

RETURN VALUE : The number of cset sectors added into the search list.

SIDE EFFECTS : Fills search list

=========================================================================*/
#ifdef FEATURE_HDR_REVB
uint8 hdrsrchutil_add_cset_to_search_list
(
  const sys_channel_type*          chan_list,
    /* Channel from which nset pilots must be selected */

  uint8                            chan_cnt,
    /* Number of channels in the channel list */

  uint8                            max_allowed
    /* Maximum allowed srch cnt */
);
#else
uint8 hdrsrchutil_add_cset_to_search_list
(
  const sys_channel_type          * chan
  /* Channel from which cset pilots must be selected */
);
#endif /* FEATURE_HDR_REVB */


/*=========================================================================

FUNCTION        HDRSRCHUTIL_ADD_BEST_NSET_TO_SEARCH_LIST

DESCRIPTION
    Adds the top "n" nset pilots to the search list.

    The criterion for nset selection is:
      1. On the same frequency as reacq_channel
      2. Crosses the minimum pilot energy threshold
      3. Top "n" pilots

DEPENDENCIES
  None

RETURN VALUE
  Number of nset pilots selected

SIDE EFFECTS
  None

==========================================================================*/

uint8 hdrsrchutil_add_best_nset_to_search_list
(
  const sys_channel_type          * chan,
    /* Channel from which the best nset pilots must be selected */

  uint8                             max_pilots
    /* Maximum number of best pilots to add */
);


/*==========================================================================

FUNCTION        HDRSRCHUTIL_ADD_NSET_TO_SEARCH_LIST

DESCRIPTION  : This function adds a subset of the nset to the current
                search list

DEPENDENCIES
  None

INPUT        : *chan           - Channel array
               chan_cnt        - Channel count
               max_pilots      - Maximum NSET pilots to add
               check_nset_dups - Check for NET dups flag

RETURN VALUE
  Number of nset pilots selected for reacq

SIDE EFFECTS
  Fills search list

==========================================================================*/

uint8 hdrsrchutil_add_nset_to_search_list
(
  const sys_channel_type          * chan,
    /* Channel from which the nset pilots must be selected */

#ifdef FEATURE_HDR_REVB
  uint8                             chan_cnt,
    /* Number of channels in the channel list */
#endif /* FEATURE_HDR_REVB */

  uint8                             max_pilots,
    /* Number of best peaks to accumulate */

  boolean                           check_nset_dups
    /* Flag to check search list for NSET duplicates.  This is useful
       when best NSET pilots have already been added to the search list. */
);


/* <EJECT> */

/*=========================================================================

FUNCTION     : HDRSRCHUTIL_LIMIT_WINDOW_SIZE

DESCRIPTION  : This function checks the window size of the given
               window against the given limits.

DEPENDENCIES : None

RETURN VALUE : Window size

SIDE EFFECTS : None

=========================================================================*/

int hdrsrchutil_limit_window_size
(
  int                               window_size,
    /* Maximum window size, in chips */

  hdrsrch_winsize_enum_type         max_winsize,
    /* Maximum window size */

  hdrsrch_winsize_enum_type         min_winsize
    /* Minimum window size */
);


/*=========================================================================

FUNCTION     : HDRSRCHUTIL_LIMIT_SEARCH_WINDOW

DESCRIPTION  : This function checks the window size of the current
                search list to ensure that the max and min window
                sizes are set properly.

DEPENDENCIES : None

INPUT        : Maximum and Minimum window size

RETURN VALUE : None

SIDE EFFECTS : Window sizes changed

=========================================================================*/

void hdrsrchutil_limit_search_window
(
  hdrsrch_winsize_enum_type         max_winsize,
    /* Maximum window size */

  hdrsrch_winsize_enum_type         min_winsize
    /* Minimum window size */
);


/* <EJECT> */
/*=========================================================================

FUNCTION     : HDRSRCHUTIL_PROGRAM_REACQ_SEARCH

DESCRIPTION  : This function programs ASET & CSET sector searches

DEPENDENCIES : None

INPUT        : To do default search or not.

RETURN VALUE : None

SIDE EFFECTS : None

=========================================================================*/

void hdrsrchutil_program_reacq_search( boolean search_with_diversity );


/* <EJECT> */
/*==========================================================================

FUNCTION HDRSRCH_PROGRAM_SEARCH_SET

DESCRIPTION
  Program a collection of searches, based on the current sets and channel.

DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  True if atleast one pilot was submitted for search else false

SIDE EFFECTS
  Starts lost dump timer.

==========================================================================*/

boolean hdrsrchutil_program_search_set( void );


/* <EJECT> */
/*==========================================================================

FUNCTION HDRSRCHUTIL_PROGRAM_PRI_CHAIN_ASET_SEARCH

DESCRIPTION
  Program a collection of ASET search. It adds ASET to search list and 
  submits the search list to the hardware.

DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  True if at least one pilot was submitted for search else false

SIDE EFFECTS
  None

==========================================================================*/

boolean hdrsrchutil_program_pri_chain_aset_search( void );


/* <EJECT> */
/*===========================================================================

FUNCTION HDRSRCHUTIL_PRI_CHAIN_ASET_DUMP

DESCRIPTION
  This function processes primary chain ASET dump.

DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/

void hdrsrchutil_pri_chain_aset_dump( void );


/* <EJECT> */
/*=========================================================================

FUNCTION     : HDRSRCHUTIL_PROGRAM_OFS_SEARCH_SET

DESCRIPTION
  Program a collection of searches, based on the current sets and channel.

DEPENDENCIES
  None

PARAMETERS
  system_type - type of system the search has to be programmed for. 

RETURN VALUE
  True if at least one pilot was submitted for search else false.

SIDE EFFECTS
  Starts lost dump timer.

=========================================================================*/

boolean hdrsrchutil_program_ofs_search_set
(
  hdrsrch_system_enum_type    system_type
    /* system type for the current channel */
);

/* EJECT */
/*===========================================================================

FUNCTION HDRSRCHUTIL_RUP_REQ_CMD

DESCRIPTION
  This function passes RUP request command to searcher.

DEPENDENCIES
  None

PARAMETERS
  params - parameters for the RUP request command.

RETURN VALUE
  Status of the command.

SIDE EFFECTS
  None

===========================================================================*/

errno_enum_type hdrsrchutil_rup_req_cmd
(
  const hdrsrch_rup_req_params_type    *params
    /* RUP Request parameters */
);

/* <EJECT> */
/*=========================================================================

FUNCTION     : HDRSRCHUTIL_RESET_RUP_SET

DESCRIPTION
  Resets RUP search set.

DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  None
    
SIDE EFFECTS
  None
  
=========================================================================*/

void hdrsrchutil_reset_rup_set( void );

/* <EJECT> */
/*=========================================================================

FUNCTION     : HDRSRCHUTIL_PROGRAM_RUP_SEARCH_SET

DESCRIPTION
  Program a collection of searches, based on the current sets and channel.

DEPENDENCIES
  None

PARAMETERS
  system_type - type of system for the rup search. 

RETURN VALUE
  True if at least one pilot was submitted for search else false.

SIDE EFFECTS
  Starts lost dump timer.

=========================================================================*/

boolean hdrsrchutil_program_rup_search_set
(
  hdrsrch_system_enum_type    system_type
    /* system type for the current channel */
);

/* <EJECT> */
/*=========================================================================

FUNCTION     : HDRSRCHUTIL_RESET_RUP_SRCH_STATE

DESCRIPTION
  Reset RUP search set generation related state variables.

DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  None
  
SIDE EFFECTS
  None
  
=========================================================================*/

void hdrsrchutil_reset_rup_srch_state( void );

/* <EJECT> */
/*=========================================================================

FUNCTION     : HDRSRCHUTIL_GENERATE_RUP_SET

DESCRIPTION
  Generate the RUP search sectors from RUP Request.

DEPENDENCIES
  None

PARAMETERS
  system_type - type of system for the rup search. 

RETURN VALUE
  Whether the generated RUP set is the last one for the RUP request.
  
SIDE EFFECTS
  None
  
=========================================================================*/

boolean hdrsrchutil_generate_rup_set
(
  hdrsrch_system_enum_type    system_type
    /* system type for the rup search */
);

/* EJECT */
/*===========================================================================

FUNCTION HDRSRCHUTIL_SEND_RUP_PILOT_REPORT

DESCRIPTION
  This function calls the call back function for RUP pilot reporting.

DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void hdrsrchutil_send_rup_pilot_report( void );

/* <EJECT> */
/*=========================================================================

FUNCTION     : HDRSRCHUTIL_RESET_OFS_FILTER

DESCRIPTION  : This function resets the OFS filter

DEPENDENCIES : None

RETURN VALUE : None

SIDE EFFECTS : None

=========================================================================*/

void hdrsrchutil_reset_ofs_filter( void );

/* <EJECT> */
/*==========================================================================

FUNCTION HDRSRCH_PROCESS_SEARCH_SET

DESCRIPTION
  Processes the search results for the just completed search.

DEPENDENCIES
  None

PARAMETERS
  aset_mgmt - Active Set Management routine

RETURN VALUE
  None

SIDE EFFECTS
  Calls hdrsrchutil_process_search_dump, hdrsrchutil_process_search_triage
  Clears lost dump timer.

==========================================================================*/

typedef
void (*hdrsrch_aset_mgmt_cb_type)
(
  int new_rpts
    /* Number of new pilot report events (R/N to C Set, C>A, Weak A) */
);

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

void hdrsrchutil_process_search_set
(
  hdrsrch_aset_mgmt_cb_type   aset_mgmt
    /* Callback for Active Set management */
);



/*==========================================================================

FUNCTION HDRSRCHUTIL_PROCESS_SEARCH_DUMP

DESCRIPTION
  Processes the search results for the just completed search.
  Active set processing and triage is not performed.

DEPENDENCIES
  An active search dump

PARAMETERS
  None

RETURN VALUE
  Number of new reports

SIDE EFFECTS
  Clears lost dump timer.

==========================================================================*/

int hdrsrchutil_process_search_dump( void );

/* <EJECT> */
/*==========================================================================

FUNCTION HDRSRCHUTIL_PROCESS_OFS_SEARCH_DUMP

DESCRIPTION
  Processes the search results for the just completed OFS search.

DEPENDENCIES
  None

PARAMETERS
  Flag to whether perform set management or not. 

RETURN VALUE
  Number of new reports

SIDE EFFECTS
  Clears lost dump timer.

===========================================================================*/

int hdrsrchutil_process_ofs_search_dump( boolean manage_sets );

/*==========================================================================

FUNCTION HDRSRCHUTIL_PROCESS_SEARCH_TRIAGE

DESCRIPTION
  Assigns fingers to search peaks

DEPENDENCIES
  An active search dump

PARAMETERS
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

==========================================================================*/

void hdrsrchutil_process_search_triage( void );


/* <EJECT> */
/*==========================================================================

FUNCTION HDRSRCH_UPDATE_NSET_PILOTS

DESCRIPTION
  Updates the neighbour list with new pilots.
  Pilots already in the Active Set or the Candidate Set are not demoted
  to the neighbour set.

DEPENDENCIES
  None

PARAMETERS
  nset_pilots          - Array of pilots to put into NSET
  num_nset_pilots      - Number of pilots in the array
  update_type          - Type of update - BROADCAST or DIRECTED message
  channel_list         - List of hash channels from SP broadcast message
  num_channels         - Number of hash channels in channel list
  avoided_channel_list - List of avoided channels
  num_avoided_channels - Number of avoided channels in the channel list
  serving_pilot_gid    - Group id of the pilot from which the nset update is rx'ed
  hdr_subtype          - HDR protocol subtype

RETURN VALUE
  TRUE  - Set transitions performed.
  FALSE - No set transitions occurred.

SIDE EFFECTS
  None

==========================================================================*/

boolean hdrsrchutil_update_nset_pilots
(
  const hdrsrch_nset_pilot_type *nset_pilots,
    /* Array of pilots to put in neighbour set */

  int                            num_nset_pilots,
    /* Number of pilots to put into neighbour set */

  hdrsrch_nset_update_enum_type  update_type,
    /* Broadcast or directed update */

  const sys_channel_type         *channel_list,
    /* List of hash channels from the SP (BROADCAST) message*/

  uint8                          num_channels,
    /* Number of hash channels in the channel list */

  const sys_channel_type         *avoided_channel_list,
    /* List of avoided channels */

  uint8                          num_avoided_channels
    /* Number of avoided channels in the channel list */

#ifdef FEATURE_HDR_REVB
  , hdrsrch_pilot_gid_type       serving_pilot_gid,

  sys_active_prot_e_type         hdr_subtype
    /* HDR subtype */
#endif /* FEATURE_HDR_REVB */

  , const sys_channel_type       *pref_chan
    /* The prefered channel for nset update */

);



/*==========================================================================

FUNCTION HDRSRCH_UPDATE_ASET_PILOTS

DESCRIPTION
  Updates the active set list with new active set pilots.

DEPENDENCIES
  None

PARAMETERS
  aset_pilots     - Array of pilots to put into ASET
  num_aset_pilots - Number of pilots in the array

RETURN VALUE
  TRUE  - Set transitions performed.
  FALSE - No set transitions occurred.

SIDE EFFECTS
  None

==========================================================================*/

boolean hdrsrchutil_update_aset_pilots
(
  const hdrsrch_aset_pilot_type *aset_pilots,
    /* Array of pilots to put in neighbour set */

  int                            num_aset_pilots
    /* Number of pilots to put into neighbour set */
);


/* <EJECT> */
/*===========================================================================

FUNCTION HDRSRCHUTIL_FORCE_WINCENTER_TO_MSTR

DESCRIPTION
  Update the srch window center to the current mstr.

DEPENDENCIES

PARAMETERS
  sect  - Sector to update

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void hdrsrchutil_force_wincenter_to_mstr
(
  hdrsrch_sect_struct_type  *sect
    /* Sector to update */
);


/* <EJECT> */
/*===========================================================================

FUNCTION HDRSRCHUTIL_ASET_IDLE_HANDOFF

DESCRIPTION
  Sets the ASET pilot to the given sector

DEPENDENCIES
  Idle State (Monitor / Acess)
  New pilot is assumed to be on the current channel

PARAMETERS
  sect  - Sector to become the new "Active Sector"

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void hdrsrchutil_aset_idle_handoff
(
  hdrsrch_sect_struct_type  *sect
    /* New active set sector */
);

#ifdef FEATURE_HDR_BCMCS

/* <EJECT> */
/*=============================================================================
FUNCTION:     hdrsrchutil_promote_to_bc_aset

DESCRIPTION:
  Adds the sector to ASET & initializes the fields of the structure to the 
  default values for IDLE state. The init value for these fields are based on
  what is being done in hdrsrchutil_aset_idle_handoff() function for the new 
  pilot.
   
RETURN VALUE:
  None
   
SIDE EFFECTS:
  None
  
=============================================================================*/

void hdrsrchutil_promote_to_bc_aset
(
   hdrsrch_sect_struct_type  *sect
  /* New active set sector */
);
#endif /* FEATURE_HDR_BCMCS */


/* <EJECT> */
/*===========================================================================

FUNCTION HDRSRCHUTIL_ENTER_IDLE

DESCRIPTION
  Drops pilots from the ASET until only the best pilot remains.

DEPENDENCIES
  Traffic State

PARAMETERS
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void hdrsrchutil_aset_enter_idle( void );


/* <EJECT> */

/*=========================================================================

FUNCTION     : HDRSRCHUTIL_GET_WINSIZE

DESCRIPTION  : This function determines the window size for a pilot sector

DEPENDENCIES : None

RETURN VALUE : Window size in chips

SIDE EFFECTS : None

=========================================================================*/

uint2 hdrsrchutil_get_winsize
(
  hdrsrch_sect_struct_type      * sect
);


/*=========================================================================

FUNCTION     : HDRSRCHUTIL_SYSTEM_LOST

DESCRIPTION  : This function declares system lost, and transfers back to
               the inactive state

DEPENDENCIES : None

RETURN VALUE : None

SIDE EFFECTS : Exits to inactive.

=========================================================================*/

void hdrsrchutil_system_lost
(
  hdrsrch_system_lost_enum_type   reason
    /* Reason for declaring system lost */
);


/*=========================================================================

FUNCTION     : HDRSRCHDRV_HANDLE_MDSP_HALT

DESCRIPTION  : This function handles the case when the modem DSP halts.
               There is nothing search can do now except re-starting the
               DSP and declare system loss to the protocols.

DEPENDENCIES : None

INPUT        : None

RETURN VALUE : None

SIDE EFFECTS : Declares System Lost.

=========================================================================*/
void hdrsrchutil_handle_mdsp_halt( void );


/* <EJECT> */
/*===========================================================================

FUNCTION HDRSRCHUTIL_ENABLE_ROLL

DESCRIPTION
  This function installs and enables the PN ROLL signal

DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void hdrsrchutil_enable_roll ( void );


/*===========================================================================

FUNCTION HDRSRCHUTIL_DISABLE_ROLL

DESCRIPTION
  This function un-installs and disables the PN ROLL signal

DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void hdrsrchutil_disable_roll ( void );


/*===========================================================================

FUNCTION HDRSRCHUTIL_HANDLE_PN_ROLL

DESCRIPTION
  This function performs the default PN roll processing.

DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void hdrsrchutil_handle_pn_roll ( void );


/* <EJECT> */
/*=========================================================================

FUNCTION     : HDRSRCHUTIL_INIT

DESCRIPTION  : This function initializes HDRSRCH utilities related stuff

DEPENDENCIES : None

INPUT        : None

RETURN VALUE : None

SIDE EFFECTS : None

=========================================================================*/

void hdrsrchutil_init( void );

/* EJECT */
/*=========================================================================

FUNCTION     : HDRSRCHUTIL_PREPARE_REACQ_LIST

DESCRIPTION  : This function prepares the reacquisition list of pilots to
               be searched after the wakeup. It fills in the following
               sectors in the search driver's search list.

               1. All Active Set Sectors [a_cnt]
               2. All Candidate Set Sectors [c_cnt]
               3. Maximum [16-a_cnt-c_cnt] number of neighbors


DEPENDENCIES : None

INPUT        : None

RETURN VALUE : None

SIDE EFFECTS : Updates the hdrsrchdrv.srch_list and count

=========================================================================*/

void hdrsrchutil_prepare_reacq_list( void );

/* EJECT */
/*==========================================================================
FUNCTION        HDRSRCHUTIL_START_STOP_WATCH

DESCRIPTION
  Starts stop watch timing. This is implemented using time stamps
  for better accuracy. Call hdrsrchutil_read_stop_watch() to get
  the difference between start and stop timings.

DEPENDENCIES
  None.

INPUTS
  None

RETURN VALUE

SIDE EFFECTS
  none

==========================================================================*/

void hdrsrchutil_start_stop_watch( void );


/*==========================================================================

FUNCTION        HDRSRCHUTIL_READ_STOP_WATCH

DESCRIPTION
  Returns the time between start and end events on the stop watch

DEPENDENCIES
  hdrsrchutil_start_stop_watch() should have been called before

INPUTS
  None

RETURN VALUE
 dword : Acquisition duration in msec

SIDE EFFECTS
  none

==========================================================================*/

dword hdrsrchutil_read_stop_watch( void );


/* EJECT */
#ifdef FEATURE_HDR_SYNC_CHECK
/*=========================================================================

FUNCTION        HDRSRCHUTIL_SYNC_CHECK

DESCRIPTION
  This function validates time against the sync message

DEPENDENCIES
  None

INPUT
  Sync Messages

RETURN VALUE
  None

SIDE EFFECTS
  May adjust system time

=========================================================================*/

errno_enum_type hdrsrchutil_sync_check
(
  const hdrsrch_synchronize_params_type *sync
    /* Synchronization message */
);
#endif /* FEATURE_HDR_SYNC_CHECK */

/* EJECT */
/*==========================================================================

FUNCTION        HDRSRCHUTIL_GET_HDR_RSSI

DESCRIPTION
  Returns HDR signal estimate to the caller

DEPENDENCIES
  TBD

INPUTS
  None

RETURN VALUE
 byte : Quality of HDR signal

SIDE EFFECTS
  none

==========================================================================*/
byte hdrsrchutil_get_hdr_rssi( void );

/* EJECT */
/*==========================================================================

FUNCTION        HDRSRCHUTIL_GET_ECIO_AND_IO

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

==========================================================================*/

void hdrsrchutil_get_ecio_and_io( int16 *, int * );

/* EJECT */
/*==========================================================================

FUNCTION        HDRSRCHUTIL_GET_TXAGC

DESCRIPTION
  Returns HDR txAGC in dBm unit to the caller. 
  txAGC range: from -64 to 42 dBm
      
DEPENDENCIES
  None

INPUTS
  None

RETURN VALUE
  tx_dbm - txAGC

SIDE EFFECTS
  None

==========================================================================*/

int hdrsrchutil_get_txagc( void );

/* <EJECT> */
/*===========================================================================

FUNCTION HDRSRCH_UPDATE_WIN_CENTER

   This function updates the window center for different sectors in the
   search list. If just woke up from sleep then it slams the window center
   to the earliest peak position other wise it tracks window center towards
   earliest peak at a slow pace.

DESCRIPTION

DEPENDENCIES
  None

PARAMETERS
  Sector for which to update search window center

RETURN VALUE
  None

SIDE EFFECTS
  Updates win_cenx2 parameter in the sector table

===========================================================================*/

void hdrsrchutil_update_win_center( void );
/* <EJECT> */

/*===========================================================================

FUNCTION HDRSRCH_SET_WIN_CENTER_TO_MSTR

DESCRIPTION

DEPENDENCIES
  None

PARAMETERS

RETURN VALUE
  None

SIDE EFFECTS
  Updates win_cenx2 parameter in the sector table

===========================================================================*/

void hdrsrchutil_set_win_center_to_mstr( void );

/* <EJECT> */
/*=========================================================================

FUNCTION     : HDRSRCHUTIL_GET_WIN_OFFSET

DESCRIPTION  : This function determines the window offset given the window
                size and the SearchWindowOffset value [6.6.6.2-4]

DEPENDENCIES : None

RETURN VALUE : Window offset in chips

SIDE EFFECTS : None

=========================================================================*/

int16 hdrsrchutil_get_win_offset
(
  hdrsrch_sect_struct_type        *sect
    /* sector we need a search window offset for */
);

/* <EJECT> */
/*=========================================================================

FUNCTION     : HDRSRCHUTIL_ADJUST_WINDOW_SIZE

DESCRIPTION  : This function adjusts the search window size according to a 
               variable from hdrsrchreacq 

DEPENDENCIES : None

RETURN VALUE : None

SIDE EFFECTS : Window size of search list might be altered.

=========================================================================*/

void hdrsrchutil_adjust_window_size
(
  uint8                              win_size_fctr,
    /* adjustment factor to search window size */
  hdrsrch_winsize_enum_type          max_winsize,
    /* Maximum window size */
  hdrsrch_winsize_enum_type          min_winsize
    /* Minimum window size */
);

/*=========================================================================

FUNCTION     : HDRSRCHUTIL_GET_WINSIZE_X1

DESCRIPTION  : This function translates the window size from enum to chipx1

DEPENDENCIES : None

RETURN VALUE : Window offset in chips

SIDE EFFECTS : None

=========================================================================*/
uint16 hdrsrchutil_get_winsize_x1
(
  hdrsrch_winsize_enum_type    winsize
    /* Window Size in enum */
);

/* <EJECT> */
/*==========================================================================

FUNCTION        HDRSRCHUTIL_SUSPEND_RPT

DESCRIPTION
  Report to the upper layers (if interested) gain/loss of RF.
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

==========================================================================*/

void hdrsrchutil_suspend_rpt
(
  hdrsrch_suspend_enum_type     suspend_report
    /* Gain or Loss of RF resources */
);
/* EJECT */
/*==========================================================================

FUNCTION        HDRSRCHUTIL_UPDATE_DATA_IND_SINR

DESCRIPTION
  This function updates the average SINR of the best sector that AT is
  listening to. This SINR can directly be mapped to the data rate that
  AT can request at this time.
    
DEPENDENCIES
  None

INPUTS
  None

RETURN VALUE
  None

SIDE EFFECTS
  updates hdrsrchutil_data_ind_sinr

==========================================================================*/

void hdrsrchutil_update_data_ind_sinr ( void );


/* EJECT */
/*==========================================================================

FUNCTION        HDRSRCHUTIL_GET_DATA_INDICATOR

DESCRIPTION
             This function returns the data level indicator. The data rate
             that the AT can receive is divided into 9 levels, 0 to 8. Level
             8 indicates highest data rate and level 0 indicates almost no
             data rate.
    
DEPENDENCIES
  None

INPUTS
  None

RETURN VALUE
  0-8 equivalent to 9 levels of data activity. Meant to be mapped to bar 
  display 

SIDE EFFECTS
  none

==========================================================================*/

byte hdrsrchutil_get_data_indicator ( void );


/*==========================================================================

FUNCTION        HDRSRCHUTIL_TC_OFS_ALLOWED

DESCRIPTION
  This routine services as the rate control timer for OFS in traffic state.
  
DEPENDENCIES
  None

INPUTS
  None

RETURN VALUE
  Returns TRUE if there have been more than HDRSRCH_OFS_INTERVAL number of 
  PN rolls happened, since the last time that the PN roll count was recored.
  Returns FALSE otherwise.

SIDE EFFECTS
  none

==========================================================================*/

boolean hdrsrchutil_tc_ofs_allowed ( void );

/*==========================================================================

FUNCTION        HDRSRCHUTIL_TC_OFS_START_GOV

DESCRIPTION
  This routine starts the OFS rate governer

DEPENDENCIES
  None

INPUTS
  None

RETURN VALUE
  None

SIDE EFFECTS
  none

==========================================================================*/

void hdrsrchutil_tc_ofs_start_gov ( void );


/*=========================================================================

FUNCTION     : HDRSRCHUTIL_VALIDATE_SEARCHER_RESULTS

DESCRIPTION  : This function will be called after a status dump to validate
               if HDR AGC is working accurately. This function adds the valid
               path energies in the searched pilots and if the total Ec/Io
               exceeds the max allowed limit then it ignore the searcher 
               results.

               This function is being added to resolve the "bogus PN issue"
               seen occasionally on 1xEVDO where there is delay in AGC 
               compensation for deep fade resulting in increased gain for normal
               signal level. This causes elevated noise floor and hence we see
               bogus PNs. The following algorithm attempts to solve this 
               problem.

DEPENDENCIES : This function should be called only after dump.

INPUT        : None

RETURN VALUE : TRUE = Search Results look ok
               FALSE= Search results exceed max allowed range

SIDE EFFECTS : May ignore searcher results if AGC seems to be saturating

=========================================================================*/

boolean hdrsrchutil_validate_searcher_results ( void );

/* <EJECT> */
/*===========================================================================

FUNCTION HDRSRCHUTIL_INVALIDATE_SEARCH_DUMP

DESCRIPTION
  this function invalidates the searcher results. It performs the processing
  needed for the dump except for using the search results. This function will
  be called when the search results are found to be invalid (due to incorrect
  AGC resulting in elevated noise floor). This function should ensure that the
  search results are not used as well as all the necessary processing should
  be done to issue a new search.

DEPENDENCIES
  Results in ignoring the searcher results for the search cycle

PARAMETERS
  None

RETURN VALUE
  None

SIDE EFFECTS
  Clears lost dump timer.

===========================================================================*/

void hdrsrchutil_invalidate_search_dump( void );

/* <EJECT> */
/*===========================================================================

FUNCTION HDRSRCHUTIL_GET_HDR_PILOT_METRIC

DESCRIPTION
  This function returns the HDR pilot metric based on A,C,N set pilots

DEPENDENCIES
  None

PARAMETERS
  Minimum pilot thershold to be used for consideration in the metric

RETURN VALUE
  Total Metric Ec/Io computed.

SIDE EFFECTS
  None.

===========================================================================*/

uint16 hdrsrchutil_get_hdr_pilot_metric
(
  uint16                     min_eng_thresh,
    /* minimum ec/io threshold to include the pilot in the metric */

  uint16                     *aeng, 
  uint16                     *ceng, 
  uint16                     *neng
    /* pass back the aset/cset/nset energies to the calling function */
#ifdef FEATURE_HDR_REVB
  ,uint8                      *sa
    /* The subaset index with the max energy */
#endif /* FEATURE_HDR_REVB */
);

/* <EJECT> */
/*===========================================================================

FUNCTION HDRSRCHUTIL_START_TIMER

DESCRIPTION
  This function reads the current sleep timertick count into the passed timer.

DEPENDENCIES
  None
  
PARAMETERS
  None

RETURN VALUE
  None

SIDE EFFECTS

===========================================================================*/

void hdrsrchutil_start_timer( timetick_type *);

/*===========================================================================

FUNCTION HDRSRCHUTIL_STOP_TIMER_MS

DESCRIPTION
  Compute the return time elapsed from a previous timetick value

DEPENDENCIES
  None
  
PARAMETERS
  None

RETURN VALUE
  Return the elapsed time in millisecond

SIDE EFFECTS

===========================================================================*/

timetick_type hdrsrchutil_stop_timer_ms( timetick_type );

/* <EJECT> */
/*=========================================================================

FUNCTION     : HDRSRCHUTIL_CHECK_MDSP_HALT

DESCRIPTION  : This function checks if the DSP has halted

DEPENDENCIES : None

SIDE EFFECTS : None

=========================================================================*/

boolean hdrsrchutil_check_mdsp_halt( void );

/* EJECT */
/*=========================================================================

FUNCTION     : HDRSRCHUTIL_HANDLE_LOST_DUMP

DESCRIPTION  : This function handles the lost dump interrupt for MSM6500.
               For now it just declares system loss.

DEPENDENCIES : None

INPUT        : None

RETURN VALUE : None

SIDE EFFECTS : Declares System Lost.

=========================================================================*/

void hdrsrchutil_handle_lost_dump( void );


/* <EJECT> */
/*===========================================================================

FUNCTION HDRSRCHUTIL_ADJUST_THRESH_FOR_SBHO

DESCRIPTION
  This function adjusts the idle handoff threshold depending upon whether or
  not the pilots have different subnets (denoted by the color code)

DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

uint16 hdrsrchutil_adjust_thresh_for_sbho
(
  uint16                     aset_eng,
    /* Energy of the active set pilot which may be replaced */

  uint16                     comp_eng,
    /* Energy threshold that needs to be adjusted for Subnet handoff */

  hdrsrch_color_code_type    aset_cc,
  hdrsrch_color_code_type    cand_cc
    /* Color codes for two sectors */
);


/* <EJECT> */
/*=========================================================================

FUNCTION     : HDRSRCHUTIL_RESET_SRCH_FILTERS

DESCRIPTION  : This function resets the search EcIo filter of the sector 
               to be searched. The intended use of this function is to 
               reset the filters before OFS searches.

DEPENDENCIES : None

INPUT        : None

RETURN VALUE : None

SIDE EFFECTS : None

=========================================================================*/

void hdrsrchutil_reset_srch_filters( void );


#ifdef DEBUG_HDR_SAMPLE_RAM_DUMP 
/*===========================================================================

FUNCTION        HDRSRCHUTIL_READ_SAMPLE_RAM

DESCRIPTION     
  This function reads the sample ram buffer and stores the samples in an array

DEPENDENCIES   
  None

RETURN VALUE   
  None.

SIDE EFFECTS   
  None.

===========================================================================*/

void hdrsrchutil_read_sample_ram( void );

#endif /* DEBUG_HDR_SAMPLE_RAM_DUMP */


/* <EJECT> */
/*===========================================================================

FUNCTION HDRSRCHUTIL_GET_SINR_AND_RXAGC

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

void hdrsrchutil_get_sinr_and_rxagc
( 
  byte    *sinr, 
    /* pointer to SINR indicator value */

  int     *rxagc
    /* pointer to RxAgc value   */    
);


/* <EJECT> */
/*===========================================================================

FUNCTION HDRSRCHUTIL_GET_SINR_AND_RXAGC_DB

DESCRIPTION
  This function returns SINR indicator and Rx AGC to the caller. SINR 
  indicator is from 0 to 75 in 0.5 dB units from 18.0dB to -19.0dB, e.g.
  0 = 18dB, 7=-14.5dB, 75 = -19dB.
  
  SINR indicator range: from 0 to 75 in 0.5 dB units from 18.0dB to -19.0dB
  RxAgc range         : from -106 dBm to -21 dBm 

DEPENDENCIES
  None

PARAMETERS
  *sinr - Pointer to return SINR value
  *rxagc - Pointer to return RxAGC

RETURN VALUE
  None
  
SIDE EFFECTS
  None.

===========================================================================*/

void hdrsrchutil_get_sinr_and_rxagc_db
( 
  byte    *sinr, 
    /* pointer to SINR indicator value in db units */

  int     *rxagc
    /* pointer to RxAgc value   */    
);

/* <EJECT> */
/*===========================================================================

FUNCTION HDRSRCHUTIL_DEC_DEACTIVATE

DESCRIPTION
  This function prepares decoder for deactivation, performs a timed wait 
  and then deactivates the decoder.

DEPENDENCIES
  A request to deassign fingers must be sent to the FW before calling this API.

PARAMETERS
  None

RETURN VALUE
  None
  
SIDE EFFECTS
  None
===========================================================================*/

void hdrsrchutil_dec_deactivate( void );

#ifdef FEATURE_HDR_FORCE_PREAMBLE_DET
/* <EJECT> */
/*===========================================================================

FUNCTION HDRSRCHUTIL_CANCEL_FPD

DESCRIPTION
  This function cancels pending FPD.

DEPENDENCIES
  A request to deassign fingers must be sent to the FW before calling this API.

PARAMETERS
  None

RETURN VALUE
  None
  
SIDE EFFECTS
  None
===========================================================================*/
void hdrsrchutil_cancel_fpd( void );
#endif /* FEATURE_HDR_FORCE_PREAMBLE_DET */

/* EJECT */
/*==========================================================================

FUNCTION HDRSRCHUTIL_CHECK_OFS_CONDITION

DESCRIPTION
  This function checks if all the aset and cset pilots are below a certain
  threshold. The pilot energy used for each sector is the output of the 
  OFS IIR filter.

DEPENDENCIES
  None

PARAMETERS
  Energy threshold

RETURN VALUE
  True if all the aset and cset pilots are below the passed threshold.
  False otherwise.

SIDE EFFECTS
  None

==========================================================================*/

boolean hdrsrchutil_check_ofs_condition
(
  uint16            energy_threshold
    /* Threshold for energy comparison, in linear scale  */
);

/* EJECT */
/*==========================================================================

FUNCTION HDRSRCHUTIL_COUNT_STRONG_PILOTS

DESCRIPTION
  This function counts the number of aset and cset pilots that are above 
  the given threshold. It counts nset pilots also for small SCIs.

DEPENDENCIES
  None

PARAMETERS
  Energy threshold

RETURN VALUE
  The number of aset and cset pilots that are above 
  the given threshold. And nset pilots above the threshold
  for small SCIs.
  
SIDE EFFECTS
  None

==========================================================================*/

uint16 hdrsrchutil_count_strong_pilots
(
  uint16            energy_threshold
    /* Threshold for energy comparison, in linear scale */
);

/*===========================================================================

FUNCTION HDRSRCHUTIL_IS_SBHO_CANDIDATE

DESCRIPTION
  This function checks whether a cset pilot is a subnet HO candidate.

DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  TRUE: cset pilot satisfies subnet HO condition.
  FALSE: No.

SIDE EFFECTS
  None.

===========================================================================*/
boolean hdrsrchutil_is_sbho_candidate
(
  uint16           aset_eng,
    /* Active set energy */

  uint16           cset_eng
    /* Candidate set energy */
);


/*===========================================================================

FUNCTION HDRSRCHUTIL_SECT_IS_HO_CANDIDATE

DESCRIPTION
  This function determines whether a sector is a handoff candidate. The same
  subnet handoff is preferred. If a sector in the same subnet passes handoff
  criteria, then no cross subnet handoff no matter what its pilot energy is.

DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE : TRUE = the sector is handoff candidate. 

SIDE EFFECTS
  None.

===========================================================================*/

boolean hdrsrchutil_sect_is_ho_candidate
(

  hdrsrch_sect_struct_type   *aset,
    /* aset sector */

  hdrsrch_sect_struct_type   *cset,
    /* candidate sector */

  boolean                    use_inst_eng,
    /* Whether to use instantaneous energy to pick ho candidate */

  uint16                     best_eng,
    /* Best energy so far */

  uint16                     min_thresh,
    /* minimum threshold eng */

  boolean                    *same_subnet_ho
    /* Whether this is same subnet handoff */
);

/*===========================================================================

FUNCTION HDRSRCHUTIL_SECT_IS_HO_CANDIDATE_DYNAMIC_THRESH

DESCRIPTION
  This function determines whether a sector is a handoff candidate. The same
  subnet handoff is preferred. It considers different thresholds for cached
  sector and not-cached sector.

DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE : 
  TRUE  = the sector is handoff candidate. 
  FALSE = the sector is not a handoff candidate.

SIDE EFFECTS
  None.

===========================================================================*/

boolean hdrsrchutil_sect_is_ho_candidate_dynamic_thresh
(

  hdrsrch_sect_struct_type   *aset,
    /* aset sector */

  hdrsrch_sect_struct_type   *cset,
    /* candidate sector */

  boolean                    use_inst_eng,
    /* Whether to use instantaneous energy to pick ho candidate */

  uint16                     min_thresh_cached,
    /* Minimum threshold eng */

  uint16                     min_thresh_notcached,
    /* Minimum threshold eng */

  uint16                     best_eng,
   /* Best energy so far */

  hdrsrch_sect_struct_type   *best_sect,
    /* Strongest sector so far */

  boolean                    *candidate_cached,
    /* Whether the handoff candidate is cached */

  boolean                    *same_subnet_ho
    /* Whether this is same subnet handoff */
);

/*===========================================================================

FUNCTION HDRSRCHUTIL_CALC_DYNAMIC_HO_THRESHOLDS

DESCRIPTION
  This function calculates idle handoff thresholds. It considers factors as
  the sector is cached or not, how strong is the current aset pilot.


DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE : 
  None

SIDE EFFECTS
  None.

===========================================================================*/

void hdrsrchutil_calc_dynamic_ho_thresholds
(
  uint16                     aset_eng,
   /* Aset energy */

  uint16                     *min_thresh_cached,
    /* handoff threshold for cached sector */

  uint16                     *min_thresh_notcached
    /* handoff threshold for not cached sector */
);

/*===========================================================================

FUNCTION HDRSRCHUTIL_IS_DIFFERENT_SUBNET

DESCRIPTION
  This function checks whether two sectors are on different subnets.

DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  TRUE: On different subnets.
  FALSE: On the same subnet or unknown.

SIDE EFFECTS
  None.

===========================================================================*/
boolean hdrsrchutil_is_different_subnet
(
  hdrsrch_color_code_type aset_cc,
   /* color code of aset */

  hdrsrch_color_code_type cset_cc
    /* color code of cset */
);


/* <EJECT> */

/*==========================================================================

FUNCTION        HDRSRCHUTIL_GET_SYSTEM_LOSS_CHAN

DESCRIPTION
  This function returns the channel of last system loss.
  
DEPENDENCIES
  None

INPUTS
  None
  
RETURN VALUE
  None

SIDE EFFECTS
  None

==========================================================================*/

sys_channel_type *hdrsrchutil_get_system_loss_chan( void );

/*==========================================================================

FUNCTION        HDRSRCHUTIL_GET_SYSTEM_LOSS_COSET_GROUP

DESCRIPTION
  This function returns the coset group of the last system loss.
  
DEPENDENCIES
  None

INPUTS
  None
  
RETURN VALUE
  None

SIDE EFFECTS
  None

==========================================================================*/

uint8 hdrsrchutil_get_system_loss_coset_group( void );

/*==========================================================================

FUNCTION        HDRSRCHUTIL_RESET_SYSTEM_LOSS_INFO

DESCRIPTION
  This function initializes the parameters related system loss.
  
DEPENDENCIES
  None

INPUTS
  None
  
RETURN VALUE
  None

SIDE EFFECTS
  Change hdrsrchacq.system_loss structre.

==========================================================================*/

void hdrsrchutil_reset_system_loss_info( void );

/*===========================================================================

FUNCTION HDRSRCHUTIL_UPDATE_CACHED_SECTORS

DESCRIPTION
  Updates the cached sectors in protocol. The information is used by IHO.

DEPENDENCIES
  None

PARAMETERS
  cached_sects     - Array of cached sectors
  num_sects        - Number of cached sectors

RETURN VALUE
  None
SIDE EFFECTS
  none

===========================================================================*/

void hdrsrchutil_update_cached_sectors
( 
  const sys_link_type        *cached_sects,
    /* The cached sectors */

  uint8                      num_sects
    /* The number of cached sectors */
);

/*===========================================================================

FUNCTION HDRSRCHUTIL_SECTOR_IS_CACHED

DESCRIPTION
  Check whether a sector is cached.

DEPENDENCIES
  None

PARAMETERS
  sector       - The sector to check whether it is cached

RETURN VALUE
  TRUE         - The sector is cached
  FALSE        - The sector is not cached
SIDE EFFECTS
  none

===========================================================================*/

boolean hdrsrchutil_sector_is_cached
(
  hdrsrch_sect_struct_type   *sector
    /* Sector to check whethe it is cached */
);

/*===========================================================================

FUNCTION HDRSRCHUTIL_CALC_TIME_TO_TUNE_AWAY

DESCRIPTION
  This function determines the appropriate tune away time given the 
  previously decided tune away time and the current request. Here are the
  rules:

  1) If the requested time is later than the currently selected tune away 
  time, always choose the requested time.
  2) If the requested time is earlier than the currently selected tune away
  time but later than the earliest time allowed to support DRC rampdown, choose
  the requested time.
  3) If the requested time is earlier than the currently selected tune away
  time but earlier than the earliest time allowed to support DRC rampdown, 
  choose the earliest time between the currently selected tune away time and
  the earliest time allowed to support DRC rampdown.

DEPENDENCIES
  None

RETURN VALUE
  The time remaining until tune away in sclks

SIDE EFFECTS
  None

===========================================================================*/

uint32 hdrsrchutil_calc_time_to_tune_away
(
  uint32                                now_sclk_ts,
    /* Current time as an sclk timestamp */

  int32                                 min_ta_duration,
    /* What is the minimum time needed to tune away. */

  hdrsrchrf_retain_lock_state_t*        rl_state,
    /* How much time is left on the retain lock state */

  boolean*                              no_end,
    /* There is no time the lock must be release. This is both an 
       [in] parameter to indicate the previous state and an [out] 
       parameter to indicate the decided state. */

  uint32*                               end_sclk_ts,
    /* Tune away time as an sclk timestamp. This is both an 
       [in] parameter to indicate the previous value and an [out] 
       parameter to indicate the decided value. Note this is irrelevant
       if no_end is returned as TRUE. */

  boolean*                             is_ta_updated
    /* This flag will indicate if this function was able to update the
       tuneaway time to meet the (new) required time. */ 
);

#ifdef FEATURE_HDR_REVA_L1
/*===========================================================================

FUNCTION HDRSRCHUTIL_RA_CHANNEL_GAIN_DB_TO_LIN

DESCRIPTION
  This function converts RA channel gain from dB to linear.

DEPENDENCIES
  None

PARAMETERS
  ra_channel_gain_db     - RA channel gain in dB unit

RETURN VALUE
  Linear RA channel gain

SIDE EFFECTS
  None

===========================================================================*/

uint16 hdrsrchutil_ra_channel_gain_db_to_lin
( 
  int8                       ra_channel_gain_db 
    /* RA channel gain in dB unit */
);
#endif /* FEATURE_HDR_REVA_L1 */


#ifdef FEATURE_HDR_REVB
/* EJECT */
/*===========================================================================

FUNCTION HDRSRCHUTIL_GET_NUM_UNIQUE_CELLS_IN_SUBASET

DESCRIPTION
  This function is used to find the number of unique cells in the sub active set.
  This number should be the same across all the subactive set ideally.

DEPENDENCIES
  None

LIMITATIONS
  The rpc_cell_idx can only be from 0 to 31. 

RETURN VALUE
  The number of unique cells in a SUBASET.  

SIDE EFFECTS
  None

===========================================================================*/

uint8 hdrsrchutil_get_num_unique_cells_in_subaset( uint8 subaset_idx );
#else
/* EJECT */
/*===========================================================================

FUNCTION HDRSRCHUTIL_GET_NUM_UNIQUE_CELLS_IN_ASET

DESCRIPTION
  This function is used to find the number of unique cells in the active set.

DEPENDENCIES
  None

RETURN VALUE
  The number of unique cells in ASET.  

SIDE EFFECTS
  None

===========================================================================*/

uint8 hdrsrchutil_get_num_unique_cells_in_aset (void);
#endif /* FEATURE_HDR_REVB */

#ifdef FEATURE_HDR_AGC_OPTIMIZATION_P1

/* EJECT */
/*===========================================================================

FUNCTION HDRSRCHUTIL_GET_AGC_RTCOFFET

DESCRIPTION
  This function is used to calculate AGC RTCoffset 
  to be used in new fast AGC acquisition algorithm.

DEPENDENCIES
  None

LIMITATIONS
  None

RETURN VALUE
  The AGC RTCoffset to be used in new fast AGC acquisition algorithm.

SIDE EFFECTS
  None

===========================================================================*/

uint32 hdrsrchutil_get_agc_rtcoffset (void);

#endif /* FEATURE_HDR_AGC_OPTIMIZATION_P1 */


/*===========================================================================

FUNCTION HDRSRCHUTIL_ENABLE_MISSED_SCC_DETECTION

DESCRIPTION
  Enables missed sync CC detection. Will clear pending missed scc
  interrupts as parameter indicates.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void hdrsrchutil_enable_missed_scc_detection
( 
  boolean clr_pending_int 
    /* Clear any pending ISR */
);


/* EJECT */
/*===========================================================================

FUNCTION HDRSRCHUTIL_READ_RTC

DESCRIPTION
  The function issues the rtc read command to firmware and latches the RTC 
  count and RTC RXTX compare count from the firmware interfaces.

DEPENDENCIES
  DSP clock should be enabled.

RETURN VALUE
  None.

SIDE EFFECTS
  The function should be called when RTC is off and hdr firmware is on

===========================================================================*/
void hdrsrchutil_read_rtc
( 
  uint32*    rtc_cnt,
    /* Stored RTC value */

  uint32*    rxtx_cmp_cnt
    /* HDRRXTX compare cnt */
);


#ifdef FEATURE_ATOM

/* <EJECT> */
/*===========================================================================

FUNCTION HDRSRCHUTIL_OPERATIONAL_MODE_ALGORITHM_INIT

DESCRIPTION
  The function initializes the structure for 
  AT operational mode management algorithm.

DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  None

SIDE EFFECTS
  None.

===========================================================================*/

void hdrsrchutil_operational_mode_algorithm_init( void );

/* <EJECT> */
/*===========================================================================

FUNCTION HDRSRCHUTIL_OPERATIONAL_MODE_GET_NV_CONFIGURATION

DESCRIPTION
  The function reads the NV item configuration for  
  AT operational mode management algorithm. If necessary sets
  the algorithm mode and algorithm active flag. 

DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  None

SIDE EFFECTS
  None.

===========================================================================*/

void hdrsrchutil_operational_mode_get_nv_configuration( void );

/*===========================================================================

FUNCTION HDRSRCHUTIL_CONFIGURE_OPMODE

DESCRIPTION
  The function configures callback and algorithm enable flags for 
  AT operational mode management algorithm.

DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  None

SIDE EFFECTS
  None.

===========================================================================*/

void hdrsrchutil_configure_opmode
(
  hdrsrch_opmode_config_enum_type      config,
    /* which at operational mode algorithm if any to run */
    
  hdrsrch_opmode_indication_cb_type    poor_coverage_cb,
    /* cb to indicate poor HDR coverage */

  hdrsrch_opmode_indication_cb_type    good_coverage_cb
    /* cb to indicate good HDR coverage */
);

#endif /* FEATURE_ATOM */

#ifdef FEATURE_XO
/*==========================================================================

FUNCTION     : HDRSRCHACQ_CONVERT_PPM_TO_HZ 

DESCRIPTION  : Convert PPM (Parts Per Million) to Hz
   
   rfm_get_rx_carrier_freq() return value has KHz LSB
   So freq_offset_hz
       = ppm * rx_carrier_freq_khz * 1000 (Convert to freq to hz)
       = part/1000000 * rx_carrier_freq_khz * 1000 (ppm == part/1000000)
       = part * rx_carrier_freq_khz / 1000 (Simple reduction)
   Need to divide by 4.7 hz, the frequency resolution of the searcher4 rotator,
   where 1/4.7 = 109/2^9.
   So freq_offset-hz = ( 109 * (part * rx_carrier_freq_khz / 1000) ) >> 9

   Note: Return value in int16 since we need in that type for freq_offset

INPUT : ppm      - parts per million
        band     - Band class
        chan_num - Channel Number

DEPENDENCIES : None

RETURN VALUE : ppm convert to hz

SIDE EFFECTS :

==========================================================================*/

int16 hdrsrchacq_convert_ppm_to_hz
(
  int16                   ppm,
    /* Parts Per Million */

  sys_band_class_e_type   band,
    /* Band class - 0, 1, etc */

  sys_channel_num_type    chan_num
    /* CDMA Channel Number */
);
#endif /* FEATURE_XO */

/* <EJECT> */
/*=========================================================================

FUNCTION     : HDRSRCHUTIL_UPDATE_NUM_SECTOR_USERS

DESCRIPTION
  Updates the number of active users in serving cell information.
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

void hdrsrchutil_update_num_sector_users
( 
  uint16 num_active_users
    /* number of active users in serving sector */
);


#ifdef FEATURE_HDR_REVB
/* <EJECT> */
/*===========================================================================

FUNCTION HDRSRCHUTIL_UPDATE_ASET_PILOTS_REVB

DESCRIPTION
  Updates the active set list with new active set pilots. The function is used
  only for revB HDR which porcesses multiple sub active ASETs.

DEPENDENCIES
  Connected State

PARAMETERS
  subaset_list - Array of pilots to put into ASET
  num_aset_pilots - Number of pilots in the array

RETURN VALUE
  TRUE  - Set transitions performed.
  FALSE - No set transitions occurred.

SIDE EFFECTS
  None

===========================================================================*/

boolean hdrsrchutil_update_aset_pilots_revb
(
  const hdrsrch_subactive_set_type* subaset_list,
    /* Array of pilots to put in active set */

  uint8                              num_subaset,
    /* Number of pilots to put into active set */

  uint8                              cc_subactive_set_idx
    /* The index to the subaset which carries CC channel */
);


/* <EJECT> */
/*===========================================================================

FUNCTION HDRSRCHUTIL_ASSIGN_DEMOD_IDXS_TO_SUBASETS

DESCRIPTION

DEPENDENCIES
  Connected State

PARAMETERS

RETURN VALUE
  Return an arry containing demod_idx assignment to each of the subaset in
  the order of the tca subaset list.

SIDE EFFECTS
  None

===========================================================================*/

void hdrsrchutil_assign_demod_idxes_to_subasets
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

FUNCTION HDRSRCHSET_INTEGRATE_ASET

DESCRIPTION
  The function works on the hdrsrchset.subaset to remove empty subaset.

DEPENDENCIES
  Connected State

PARAMETERS

RETURN VALUE
  TRUE  - Set transitions performed.
  FALSE - No set transitions occurred.

SIDE EFFECTS
  None

===========================================================================*/

void hdrsrchutil_integrate_aset( void );


/* EJECT */
/*==========================================================================

FUNCTION HDRSRCHUTIL_MATCH_ASET_CHANNELS

DESCRIPTION
  The function will check if the channel list matches identically to the current 
  aset channels

DEPENDENCIES
  Idle or Connected Mode

PARAMETERS
  chan_cnt  - Number of channels in the channel list.
  chan_list - The channel list.

RETURN VALUE
  TRUE      - The channel list matches identically to the current ASET channels.
  FALSE     - The channel list does not matches to the current ASET channels.

SIDE EFFECTS

==========================================================================*/

boolean hdrsrchutil_match_aset_channels
(
  uint8                         chan_cnt,
    /* The number of channels in the channel list */

  const sys_channel_type*       chan_list,
    /* Channel list */

  const hdrsrch_demod_idx_type* demod_list
    /* Demod assignment list */
);


/*===========================================================================

FUNCTION HDRSRCHUTIL_HANDOFF_OVERRIDE_PENDING

DESCRIPTION
  This function checks whether handoff override pending for any subaset.

DEPENDENCIES
  Connected State

PARAMETERS
  None
  
RETURN VALUE
  TRUE  - Handoff override pending.
  FALSE - No handoff override pending.

SIDE EFFECTS
  None

===========================================================================*/
boolean hdrsrchutil_handoff_override_pending( void );


/*===========================================================================

FUNCTION HDRSRCHUTIL_CLEAR_HANDOFF_OVERRIDE_

DESCRIPTION
  This function clears handoff override for all subasets.

DEPENDENCIES
  Connected State

PARAMETERS
  None
  
RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void hdrsrchutil_clear_handoff_override( void );

/*===========================================================================

FUNCTION HDRSRCH_GET_DEMOD_CARRIER_MASK

DESCRIPTION
  This function returns the demod carrier mask that indicates which demod 
  carrier is enabled.

DEPENDENCIES
  Connected State

PARAMETERS
  None

RETURN VALUE
  mask

SIDE EFFECTS
  None

===========================================================================*/
uint8 hdrsrchutil_get_demod_carrier_mask( void );

#endif /* FEATURE_HDR_REVB */

#ifdef FEATURE_HDR_CGPS_STATUS
/* EJECT */
/*===========================================================================

FUNCTION HDRSRCHUTIL_SET_CGPS_STATUS

DESCRIPTION
  This function is called to set hdr srch CGPS's active status.

DEPENDENCIES
  None

PARAMETERS
  cgps_status - TRUE if CGPS is active, FALSE otherwise

RETURN VALUE
  None

SIDE EFFECTS
  Sets hdr srch internal cgps status.

===========================================================================*/
void hdrsrchutil_set_cgps_status
( 
  boolean cgps_status
    /* TRUE if CGPS is active, FALSE otherwise */
);

/* EJECT */
/*===========================================================================

FUNCTION HDRSRCHUTIL_GET_CGPS_STATUS

DESCRIPTION
  This function is called to get hdr srch CGPS's active status.

DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  cgps_status - TRUE if CGPS is active, FALSE otherwise

SIDE EFFECTS
  None

===========================================================================*/
boolean hdrsrchutil_get_cgps_status( void );

/* EJECT */
/*===========================================================================

FUNCTION HDRSRCHUTIL_SEND_CGPS_ACK

DESCRIPTION
  This function is called to send ack back to CGPS run but
  only if this interface is defined by CGPS.

DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void hdrsrchutil_send_cgps_ack( void );

#endif /* FEATURE_HDR_CGPS_STATUS */

/* <EJECT> */
/*===========================================================================

FUNCTION HDRSRCHUTIL_SET_HANDOFF_CONTROL

DESCRIPTION
  This function sets ASP handoff override if override flag is set.
  
DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void hdrsrchutil_set_handoff_control( void );

/*===========================================================================

FUNCTION HDRSRCHUTIL_GET_REF_PILOT_INFO()

DESCRIPTION
  The function returns the current tracking sector pointor. If there  is no 
  tracking sector, the function returns the ASET sector pointer.

DEPENDENCIES
  

PARAMETERS

RETURN VALUE
  The sector pointor of the current tracking sector.

SIDE EFFECTS
  None

===========================================================================*/

hdrsrch_sect_struct_type* hdrsrchutil_get_ref_sector_ptr( void );

/*===========================================================================

FUNCTION HDRSRCHUTIL_PILOT_MEAS_REQ_CMD

DESCRIPTION
  This function passes pilot measurement request command to searcher.

DEPENDENCIES
  None

PARAMETERS
  params - parameters for the pilot measurement request command.

RETURN VALUE
  Status of the command.

SIDE EFFECTS
  None

===========================================================================*/

errno_enum_type hdrsrchutil_pilot_meas_req_cmd
(
  const hdrsrch_rup_req_params_type     *params
    /* pilot measurement request parameters */
);

/*=========================================================================

FUNCTION     : HDRSRCHUTIL_GENERATE_PILOT_MEAS_SET

DESCRIPTION
  Generate the search sectors from pilot measurement request.

DEPENDENCIES
  None

PARAMETERS
  system_type - type of the system for the pilot measurement search. 

RETURN VALUE
  Whether the generated search set is the last one for the pilot
  measurement request

SIDE EFFECTS
  None
  
=========================================================================*/

boolean hdrsrchutil_generate_pilot_meas_search_set
(
  hdrsrch_system_enum_type    system_type
    /* system type for the rup search */
);

/*===========================================================================

FUNCTION HDRSRCHUTIL_PROCESS_OFS_SEARCH_DUMP_FOR_PILOT_MEAS

DESCRIPTION
  Processes the search results for the just completed OFS search.
  This search result is for pilot measurement request

DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  None

SIDE EFFECTS
  Clears lost dump timer.

===========================================================================*/

void hdrsrchutil_process_ofs_search_dump_for_pilot_meas( void );

#ifdef FEATURE_HDR_LTE_TO_EHRPD_IRAT
/*===========================================================================

FUNCTION HDRSRCHUTIL_LTE_PILOT_MEAS_REQ_CMD

DESCRIPTION
  This function passes pilot measurement request command to searcher.

DEPENDENCIES
  None

PARAMETERS
  params - parameters for the pilot measurement request command.

RETURN VALUE
  Status of the command.

SIDE EFFECTS
  None

===========================================================================*/

hdrsrch_lte_pilot_meas_status_type hdrsrchutil_lte_pilot_meas_req_cmd
(
  const hdrsrch_lte_pilot_meas_params_type     *params
    /* pilot measurement request parameters */
);
#endif /* FEATURE_HDR_LTE_TO_EHRPD_IRAT */

/*===========================================================================

FUNCTION HDRSRCH_WAIT_FOR_SIGS

DESCRIPTION
  This procedure waits for specified signals to be set.  This routine also
  checks any queues that are being waited on.  If the queue has an item, 
  this routine sets the signal without waiting.  This allows for the queue
  signal to be cleared after each item is removed.  If a queue signal is set,
  this routine checks the queue and clears the signal if there is nothing
  on it.  This centralizes the queue checking.

DEPENDENCIES
  None.

RETURN VALUE
  For non-queue signals, a signal bit is set in the return mask if the signal 
  was in the requested mask and is also set in the REX TCB.
  
  For queue signals, a signal bit is set in the return mask if the signal was 
  in the requested mask and the queue associated with the signal has an item
  on it.

SIDE EFFECTS
  None.

===========================================================================*/
rex_sigs_type hdrsrch_wait_for_sigs
(
  rex_sigs_type  requested_mask         /* Mask of REX signals to wait for */
);


/*===========================================================================

FUNCTION HDRSRCHUTIL_MCDO_CHECK

DESCRIPTION
  The function does the following:

  MCDO mode:
    Increase MDSP clock speed to 144 MHz

  Not MCDO (SCDO/Idle):
    Decrease MDSP clock speed to 72 MHz

DEPENDENCIES
  hdrsrchutil_npa.mdsp_clk_handle must be non-null.

PARAMETERS
  speed_mhz     - MDSP Clock speed in MHz.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void hdrsrchutil_mcdo_check
(
  void
);

#ifdef FEATURE_NPA
/*===========================================================================

FUNCTION HDRSRCHUTIL_NPA_CREATE_SYNC_CLIENT()

DESCRIPTION
  The function creates and stores a local copy of an NPA client handle.

DEPENDENCIES

PARAMETERS

RETURN VALUE

SIDE EFFECTS
  hdrsrchutil_npa_cpu_handle is setup

===========================================================================*/

void hdrsrchutil_npa_create_sync_client( void );


/*===========================================================================

FUNCTION HDRSRCHUTIL_NPA_DESTROY_CLIENT()

DESCRIPTION
  This will cancel the client request (if any) and free the client
  structure. The client handle can not be used to issue requests after
  being destroyed

DEPENDENCIES

PARAMETERS

RETURN VALUE

SIDE EFFECTS
  hdrsrchutil_npa_cpu_handle is destroyed

===========================================================================*/

void hdrsrchutil_npa_destroy_client( void );


/*===========================================================================

FUNCTION HDRSRCHUTIL_NPA_ISSUE_CPU_REQUEST()

DESCRIPTION
  The function issues an NPA request for CPU/MIPS

DEPENDENCIES
  hdrsrchutil_npa.cpu_handle must be non-null.

PARAMETERS
  cpu_request     - Bit map of cpu requested.

RETURN VALUE
  Success/Failure - if the request was successful or not.

SIDE EFFECTS
  hdrsrchutil_npa.cpu_mask updated

===========================================================================*/

errno_enum_type  hdrsrchutil_npa_issue_cpu_request
(
  uint32          cpu_request
    /* Bitmap of CPU request */
);


/*===========================================================================

FUNCTION HDRSRCHUTIL_NPA_CANCEL_CPU_REQUEST()

DESCRIPTION
  The function cancels an NPA request for CPU/MIPS.

DEPENDENCIES
  hdrsrchutil_npa.cpu_handle must be non-null.

PARAMETERS
  cpu_request     - Bit map of cpu request cancelled.

RETURN VALUE
  Success/Failure - if the request was successful or not. 

SIDE EFFECTS
  hdrsrchutil_npa.cpu_mask updated

===========================================================================*/

errno_enum_type hdrsrchutil_npa_cancel_cpu_request
(
  uint32          cpu_request
    /* Bitmap of CPU request to cancel */
);


/*===========================================================================

FUNCTION HDRSRCHUTIL_NPA_ISSUE_MDSP_CLK_REQUEST

DESCRIPTION
  The function issues an NPA request for MDSP CLK SPEED

DEPENDENCIES
  hdrsrchutil_npa.mdsp_clk_handle must be non-null.

PARAMETERS
  speed_mhz     - MDSP Clock speed in MHz.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void hdrsrchutil_npa_issue_mdsp_clk_request
(
  hdrsrchutil_npa_mdsp_clk_type          speed_mhz
    /* MDSP Clock speed in MHz */
);

/*===========================================================================

FUNCTION HDRSRCHUTIL_NPA_CANCEL_MDSP_CLK_REQUEST()

DESCRIPTION
  The function cancels an NPA request for MDSP CLK.

DEPENDENCIES
  hdrsrchutil_npa.mdsp_clk_handle must be non-null.

PARAMETERS
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void hdrsrchutil_npa_cancel_mdsp_clk_request
(
  void
);

#endif /* FEATURE_NPA */

/*==========================================================================

FUNCTION        HDRSRCHUTIL_GET_RUP_CHAN

DESCRIPTION
  This function returns the channel being used for RUPrequest/PilotMeasReq/
  LTEPilotMeasReq.
  
DEPENDENCIES
  None

RETURN VALUE
  channel used for RUP

SIDE EFFECTS
  The channel when system loss occured.

==========================================================================*/

sys_channel_type hdrsrchutil_get_rup_chan( void );

/*==========================================================================

FUNCTION        HDRSRCHUTIL_IS_BRUTE_FORCE_ACQ

DESCRIPTION
  This function returns whether acquisition is a normal acq or brute-force
  acq
  
DEPENDENCIES
  None

RETURN VALUE
  TRUE    :      it is a brute-force acq
  FALSE   :      it is a normal acq

SIDE EFFECTS
  The channel when system loss occured.

==========================================================================*/

boolean hdrsrchutil_is_brute_force_acq( void );


/*===========================================================================

FUNCTION HDRSRCHUTIL_POWERDOWN_TX

DESCRIPTION
  This function powers down tx, performs a timed wait for tx powerdown.

DEPENDENCIES
  None.

PARAMETERS
  Tx powerdown reason 

RETURN VALUE
  None
  
SIDE EFFECTS
  None
===========================================================================*/

void hdrsrchutil_powerdown_tx
(
  hdrrmac_tx_stop_reason_enum_type powerdown_reason
);

#ifdef FEATURE_HDR_REVC
/*===========================================================================

FUNCTION HDRSRCHUTIL_UPDATE_LOADADJ()

DESCRIPTION
  The function sends loadadj to FW for up to 3 carriers
               
DEPENDENCIES
  None

PARAMETERS
  loadadj_ptr     - LoadAdjust to send

RETURN VALUE
  E_SUCCESS       - LoadAdj successfully updated for all subaset
  E_FAILURE       - Error while updating any subaset

SIDE EFFECTS
  None

===========================================================================*/
errno_enum_type hdrsrchutil_update_loadadj
(
  const hdrsrch_loadadj_type* loadadj_ptr
);

/*===========================================================================

FUNCTION HDRSRCHUTIL_ENABLE_NLB()

DESCRIPTION
  The function enable/disable NLB.

DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void hdrsrchutil_enable_nlb
(
  boolean enabled
);
#endif /* FEATURE_HDR_REVC */


/*=========================================================================

FUNCTION     : HDRSRCHUTIL_RESTORE_ASET_WIN_SIZE

DESCRIPTION  : This function restores the aset win size. 

DEPENDENCIES : None

RETURN VALUE : None

SIDE EFFECTS : Window size of search list might be altered.

=========================================================================*/
void hdrsrchutil_restore_aset_win_size( void );


/*=========================================================================

FUNCTION     : HDRSRCHUTIL_SAVE_AND_SET_ASET_WIN_SIZE

DESCRIPTION  : This function saves the current aset win size and set to new one. 

DEPENDENCIES : None

RETURN VALUE : None

SIDE EFFECTS : Window size of search list might be altered.

=========================================================================*/
void hdrsrchutil_save_and_set_aset_win_size
(
  hdrsrch_winsize_enum_type new_win_size
    /* Aset win size */
);


#if defined FEATURE_HDR_DYNAMIC_IDLE_DIV || defined FEATURE_HDR_ACCESS_DIV
/*=========================================================================

FUNCTION     : HDRSRCHUTIL_RESET_PREV_WAKEUP_INFO

DESCRIPTION  : This function resets all prev wakeup info to default value.
               

DEPENDENCIES : None.

INPUT        : None.

RETURN VALUE : None

SIDE EFFECTS : None

=========================================================================*/

void hdrsrchutil_reset_prev_wakeup_info( void );
 /* hdrsrchutil_reset_prev_wakeup_info */
#endif /* FEATURE_HDR_DYNAMIC_IDLE_DIV || FEATURE_HDR_ACCESS_DIV */

/*==========================================================================

FUNCTION        HDRSRCHUTIL_CONVERT_LINEAR_SINR_TO_DB

DESCRIPTION
             This function converts a linear sinr from 0 - 75
             in 0.5 db units with a range from 18 dB to -19 dB.
    
DEPENDENCIES
  None

RETURN VALUE
  0-75 in 0.5 db units with a range from 18 dB to -19 dB.

SIDE EFFECTS
  none

==========================================================================*/

byte hdrsrchutil_convert_linear_sinr_to_db
( 
  uint16     sinr_lin
    /* linear SINR */
);

#ifdef FEATURE_HDR_MODEM_COEXISTENCE_SW
/*===========================================================================

FUNCTION HDRSRCHUTIL_REPORT_COEX_POWER

DESCRIPTION
  This function is call back function to report rx/tx coes power

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void hdrsrchutil_report_coex_power
( 
   timer_cb_data_type              data
    /* Arbitrary data given to timer_def( ) to pass to this callback function */
);

#ifdef FEATURE_HDR_BAND_AVOIDANCE
/*===========================================================================

FUNCTION HDRSRCHUTIL_SAVE_BA_POWER

DESCRIPTION
  This function posts a command to HDR SRCH to save power for band avoidance.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void hdrsrchutil_save_ba_power
( 
   timer_cb_data_type              data
    /* Arbitrary data given to timer_def( ) to pass to this callback function */
);
#endif /* FEATURE_HDR_BAND_AVOIDANCE */

#endif /* FEATURE_HDR_MODEM_COEXISTENCE_SW */

/*===========================================================================

FUNCTION HDRSRCHUTIL_DO_ATOMIC_SEARCH

DESCRIPTION
  This function returns whether to do atomic search or not

DEPENDENCIES
  None

RETURN VALUE
  TRUE: if to do atomic search
  FALSE: if we can reschedule searches.

SIDE EFFECTS
  None

===========================================================================*/
boolean hdrsrchutil_do_atomic_search( void );


#ifdef FEATURE_HDR_DLNA
/*===========================================================================

FUNCTION HDRSRCHUTIL_SET_DLNA_ENABLE

DESCRIPTION
  This function sets internal flag that is checked to enable/disable DLNA. 
 
  Note: The function is called by CMAC (HDRRX task ) on powerup to set DLNA
  enable flag. The enable flag is used in HDRRX and HDRSRCH tasks.  Since
  the setting and the access occurs at two different times ( setting at
  powerup vs access post-sync ), there should not be any race condition 
  while accessing hdrsrchutil.dlna_enabled.
 
DEPENDENCIES 
  None. 

PARAMETERS
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

void hdrsrchutil_set_dlna_enable
(
  boolean enable
);

/*===========================================================================

FUNCTION HDRSRCHUTIL_DLNA_IS_ENABLED

DESCRIPTION
  This function returns enable/disable status of DLNA feature. The feature
  can be turned on/off via NV.
 
  Note: The function is executed from caller task context and is expected
  to be called from HDRRX and HDRSRCH tasks.
 
DEPENDENCIES 
  None. 

PARAMETERS
  None.

RETURN VALUE
  TRUE  - if DLNA feature is enabled
  FALSE - otherwise

SIDE EFFECTS
  None.

===========================================================================*/

boolean hdrsrchutil_dlna_is_enabled( void );

/*===========================================================================

FUNCTION HDRSRCHUTIL_ENH_DLNA_IS_ENABLED

DESCRIPTION
  This function returns enable/disable status of Enhanced DLNA feature.
  The feature can be turned on/off via NV. Enhanced DLNA can only be enabled
  if DLNA is enabled.
 
  Note: The function is executed from caller task context and is expected
  to be called from HDRRX and HDRSRCH tasks.
 
DEPENDENCIES 
  None. 

PARAMETERS
  None.

RETURN VALUE
  TRUE  - if DLNA feature is enabled
  FALSE - otherwise

SIDE EFFECTS
  None.

===========================================================================*/

boolean hdrsrchutil_enh_dlna_is_enabled( void );

/*===========================================================================

FUNCTION HDRSRCHUTIL_SET_ENH_DLNA_ENABLE

DESCRIPTION
  This function sets internal flag that is checked to enable/disable Enhanced
  DLNA feature. Enhanced DLNA can only be enabled if DLNA is enabled.
 
DEPENDENCIES 
  None. 

PARAMETERS
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

void hdrsrchutil_set_enh_dlna_enable ( boolean enable );

/*===========================================================================

FUNCTION HDRSRCHUTIL_SEND_DLNA_CTL

DESCRIPTION
  This function sends D-LNA enable/disable command to FW.
  
  Note: The function is executed from caller task context and is expected
  to be called from HDRRX and HDRSRCH tasks.
 
DEPENDENCIES 
  None. 

PARAMETERS
  enable - TRUE  => Enable D-LNA
         - FALSE => Disable D-LNA

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/

void hdrsrchutil_send_dlna_ctl
(
  boolean  enable
);


/*===========================================================================

FUNCTION HDRSRCHUTIL_SEND_EARLY_PREAM_CTL

DESCRIPTION
  This function sends early preamble enable/disable command to FW.
  
  Note: The function is executed from caller task context and is expected
  to be called from HDRRX and HDRSRCH tasks.
 
DEPENDENCIES 
  None. 

PARAMETERS
  enable - TRUE  => Enable early preamble decodes
         - FALSE => Disable early preamble decodes

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/

void hdrsrchutil_send_early_pream_ctl
(
  boolean  enable
);


/*===========================================================================

FUNCTION HDRSRCHUTIL_SCHEDULE_ODD_PREAM_DET

DESCRIPTION
  This function sends message to FW to schedule odd preamble detect at 
  requested slot for the requested duration.
  
  Note: The function is executed from caller task context and is expected
  to be called from HDRRX and HDRSRCH tasks.
 
DEPENDENCIES 
  None. 

PARAMETERS
  start_slot - Slot with the current CC cycle where odd preamble detection 
               is requested
  duration   - Duration in number of slots for odd preamble detection

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/

void hdrsrchutil_schedule_odd_pream_det
(
  uint8  start_slot,
  uint8  duration  
);

#endif /* FEATURE_HDR_DLNA */

#ifdef FEATURE_HDR_SELECTIVE_TXD
/*===========================================================================

FUNCTION HDRSRCHTC_SUBMIT_DELAYED_SEARCH

DESCRIPTION
  This schedules a search after given delay

DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/
void hdrsrchutil_submit_delayed_search
( 
  uint8 delay
);
#endif /* FEATURE_HDR_SELECTIVE_TXD */

#ifdef FEATURE_HDR_QUICK_IDLE
/*===========================================================================

FUNCTION HDRSRCHUTIL_DISABLE_QPCH()

DESCRIPTION
  The function disables QPCH detection.

DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void hdrsrchutil_disable_qpch( void );
#endif /* FEATURE_HDR_QUICK_IDLE */

/*===========================================================================

FUNCTION HDRSRCHUTIL_CHK_TRIAGE_FULL_PN()

DESCRIPTION
  The function should be called after a full PN search constructed by
  hdrsrchutil_chk_convert_to_full_pn(). This function will pick
  the best peaks and assign back to the first search task.

DEPENDENCIES
  None

PARAMETERS
  None
              
RETURN VALUE
  None

SIDE EFFECTS
  Search tasks will be set to 1

===========================================================================*/
void hdrsrchutil_chk_triage_full_pn( void );

#ifdef FEATURE_HDR_JOLOKIA_MODEM_BRINGUP
/*===========================================================================

FUNCTION HDRSRCHUTIL_NPA_QUERY_MAX_Q6_CLK_SPEED()

DESCRIPTION
  The function gets the Max Q6 supported frequency using NPA API and stores
  in local data structure.

DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void hdrsrchutil_npa_query_max_q6_clk_speed( void );

/*===========================================================================

FUNCTION HDRSRCHUTIL_GET_MAX_Q6_CLK_SPEED()

DESCRIPTION
  The function gets the Max Q6 supported frequency from locally stored data
  structure.

DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  Max Q6 supported frequency

SIDE EFFECTS
  None

===========================================================================*/

uint32 hdrsrchutil_get_max_q6_clk_speed( void );

/*===========================================================================

FUNCTION HDRSRCHUTIL_RESET_MAX_Q6_CLK_SPEED()

DESCRIPTION
  The function resets the value to ZERO (called on srchst powerdown).

DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void hdrsrchutil_reset_max_q6_clk_speed( void );

#endif /* FEATURE_HDR_JOLOKIA_MODEM_BRINGUP */

#endif /* HDRSRCHUTILS_H */

