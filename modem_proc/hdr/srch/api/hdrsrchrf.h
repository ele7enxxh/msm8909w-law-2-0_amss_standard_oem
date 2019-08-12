#ifndef HDRSRCHRF_H
#define HDRSRCHRF_H
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*=

                        HDR Search RF Manager

                   HDR Search RF Manager Header File

GENERAL DESCRIPTION

  This file provides management of the RF Rx/Tx resources for HDR.


EXTERNALIZED FUNCTIONS

  Resource Management Functions:

  hdrsrchrf_init() - Must be called before any other function, to initialize
    module.


  hdrsrchrf_reserve_at() - Registers the need to access an RF chain at a 
    specific moment in the future for the indicated duration, such as for
    a paging slot.  hdrsrchrf_request() must be used at the specified moment,
    to actually obtain the RF resource.  Implicitly performs a 
    hdrsrchrf_release().

  hdrsrchrf_request() - Request access to an RF chain immediately, for the 
    indicated duration.  Implicitly performs a hdrsrchrf_release().

  hdrsrchrf_request_and_notify() - Request access to an RF chain, when it 
    becomes available.  Implicitly performs a hdrsrchrf_release().


  hdrsrchrf_retain_lock() - Requests that the lock be retained indefinitely
    so that the client can be asked to release the lock when needed.

  hdrsrchrf_get_retain_lock_state() - Retrieves information about when 
    the lock should be released if retain_lock was called.

  hdrsrchrf_extend_duration() - Requests an increase in the duration the 
    RF is held.  If the increase cannot be granted, the original grant 
    duration remains in effect.

  hdrsrchrf_change_duration() - Requests a change in the duration the RF 
    is held for.  If the full extension cannot be granted, a partial 
    extension may result.

  hdrsrchrf_change_priority() - Specifies a change in the reason (and 
    consequently priority) for a hold on an RF resource.


  hdrsrchrf_cancel_reserve_notify() - Indicates one no-longer wants an RF
    resource, specified in hdrsrchrf_reserve_at() or 
    hdrsrchrf_request_and_notify(). If the caller has been granted an RF
    resource IT IS NOT RELEASED.

  hdrsrchrf_release() - Releases the hold on an RF resource.  If no
    one else is waiting for the RF resource, the RF will be turned off.

  hdrsrchrf_has_lock() - Whether HDR has the lock on the given resource.

  hdrsrchrf_is_shdr_supported() - Whether the hardware supports SHDR operation.

  
  RF Tune Related Functions:

  hdrsrchrf_tune_to_chan() - This function tunes the RF to the specified 
    band and channel and then calls the given callback when done.

  hdrsrchrf_prep_to_sleep() - This function makes sure mDSP is in ACTIVE
    state before putting RF to sleep.

  hdrsrchrf_prep_to_wakeup() - This function does any activities it needs to 
    do to prepare for wakeup.  This currently entails settings a timer which
    will enable AGC processing one hs after the wakeup.

  hdrsrchrf_report_rx_power() - This function reports the RX power through 
    diag.

  hdrsrchrf_pdm_capture_values() - Capture AGC PDMs so that they can be
    restored later.

  hdrsrchrf_pdm_restore_values() - Restore AGC PDMs after a tune away.

  hdrsrchrf_get_rf_dev() - Returns the identifier associated with the 
    granted RF chain.

  hdrsrchrf_cancel_in_progress_tunes() - Cancel any tunes that are currently
    in process.  This causes callbacks waiting for the end of a tune never
    to be called.

  hdrsrchrf_get_rx_power() - Get the current RX power for chain 0 and chain 1.

  hdrsrchrf_get_channel() - Get the current channel.

  hdrsrchrf_has_multi_rx() - Queries TRM if mulitple RX chains are supported.

  hdrsrchrf_set_agc_acq_params() - Sets the RF AGC acquisition parameters. 

  External RF Control Functions in Support of DAST:

  hdrsrchrf_reset_agc_est() - This function resets the AGC stability channel. 
    estimator.

  hdrsrchrf_est_agc_stability() - Estimates the stability of the AGC.

  hdrsrchrf_force_resume_agc_off() - Indicates that for reasons other than 
    the AGC estimator can determine, the resume AGC acquisition algorithm
    shouldn't be used for the next wakeup.
  
  hdrsrchrf_get_prev_agc() - Returns the last good AGC value seen while 
    awake on the same channel.

  hdrsrchrf_agc_is_stable() - Returns whether the AGC is stable enough 
    to use the resume AGC acquisition algorithm for DAST.

  hdrsrchrf_get_pri_rxlm_buffer_handle() - Returns the primary chain RXLM buffer
    handle, if one has been allocated.

  hdrsrchrf_request_to_sleep() - This function gives the RF module a chance
  to do any activities it needs to do to prepare for sleep.  For now that
  means stopping the AGC.
 
  hdrsrchrf_change_reason() - Specifies a change in the reason (LTE measurements)
    for a hold on an RF resource.
 
  hdrsrchrf_prep_for_irat_meas() - Prepares TRM for D2L measurements.
 
REGIONAL FUNCTIONS

  hdrsrchrf_tune_sig() - Handles the tune timer signal.


INITIALIZATION AND SEQUENCING REQUIREMENTS

  hdrsrchrf_init() must be called before any other function.

  Before using any RF functions, a user must be granted permission via
  hdrsrchrf_request( ) or hdrsrchrf_request_and_notify().

  When the user is finished using the RF, it must release its hold on the
  RF chain either directly via a call to hdrsrchrf_release(), or indirectly by
  calling hdrsrchrf_reserve_at(), hdrsrchrf_request_and_notify(), or 
  hdrsrchrf_request().


Copyright (c) 2005 - 2016 by Qualcomm Technologies, Inc.  All Rights Reserved.

=*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*=



==============================================================================

                           EDIT HISTORY FOR MODULE

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/hdr/srch/api/hdrsrchrf.h#1 $ $DateTime: 2016/12/13 07:58:05 $ $Author: mplcsds1 $

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

when         who     what, where, why
----------   ---     ---------------------------------------------------------
07/07/2016   vko     Featurize out ACQ_INV/SYNC_INV changes
06/30/2016   vko     Update subreason type to sys_proc_type_e_type
06/29/2016   vko     Pass subreason also, while calling the TRM API
06/29/2016   vke     Changes to implement sync trm inversion priority 
06/29/2016   mbs/vke Supported ACQ/ACQ_INV mechanism to be compliant with TRMU
06/29/2016   vke     Added TRM unification changes
06/29/2016   vke     Map HDRSRCHRF_HP_BROADCAST to TRM_ACCESS_URGENT
05/04/2015   mbs     Supported QTA using TRM API
03/26/2015   kss     Added hdrsrchrf_get_reason(). 
01/27/2015   arm     Removed deprecated TRM API (trm_get_simult_cap)
01/22/2015   arm     Cancel band grant request if it is pending for OFS state.
12/17/2014   arm     Removed deprecated TRM APIs in HDR
11/19/2014   vke/sat Changes to store QTA winning client id and share with TRM
10/20/2014   arm     Handled band grant pending from TRM.
09/29/2014   arm     DR-DSDS code cleanup
09/22/2014   arm     HDR QTA code cleanup
08/26/2014   bb      Changes to handle diversity release when unlock event is 
                     ignored by primary client during Access state
08/15/2014   mbs     Supported fall-back to TRM R&N when trm_exchange fails
08/12/2014   tnp     Fix compiler warnings
08/12/2014   wsh     Disable RTT in TAP call; force HiCap if boost, new threshold 
08/03/2014   arm     Changes to request RX_ANY chain in DR mode for idle.
08/07/2014   mbs     Added support for trm lock exchange during IRAT procedures
07/29/2014   tnp     FR22375: Fix Medium compiler warnings in HDR
12/05/2013   smd     Requested the correct priority for access diversity.
07/03/2014   arm     Used BG TRAFFIC reason for DR-DSDS traffic.
06/30/2014   arm     Changes to support DR-DSDS.
06/04/2014   arm     Changes to support DO to G QTA
06/24/2014   dsp     Corrected FEATURE_HDR_DISABLE_ACCESS_TA_GSM
05/15/2014   arm     TRM API changes for DR-DSDS.
05/14/2014   vke     Fix to prevent blocking of TAs for 1x and G due to 
                     bug in the HDR TRM-interface
05/08/2014   dsp     Disabled Access TA for GSM.
02/12/2014   dsp     Set HDRSRCHRF_OVHD_HIGH to TRM_CHANNEL_MAINTENANCE_INV.
04/03/2014   kss/wsh Added T2P boost for RTT reduction
04/17/2014   sat     Added hdrsrchrf_get_last_valid_rf_dev() 
02/21/2014   vlc     Added hdrsrchrf_prep_for_irat_meas(). 
11/27/2013   dsp     FR17482 for Page blocking management.
05/15/2013   vke     Fix for not changing the TRM request priority for idle
                     state from traffic suspend state
04/22/2013   arm     Added changes for DSDA coexistence manager.
03/04/2013   vko     Fixed compiler warnings
01/02/2013   mbs     Released primary chain only if HDR owns it
02/19/2013   vlc     Added new parameters for Pilot Meas Req message from LTE.
01/16/2013   vlc     Changed number of HS to settle RxAGC according to mode.
12/04/2012   mbs     Fixed compilation errors due to removal of LTE on Triton
09/27/2012   ljl     Added 4 HS to settle RxAGC.
09/25/2012   rmg     Added support for including freq info in TRM interaction.
09/05/2012   rmg     Added support for antenna tuner feature.
07/25/2012   wsh     Reset chain state before calling Req&Notify
04/09/2012   vlc     Added hdrsrchrf_change_reason() to work with D2L meas. 
04/12/2012   wsh     Use prev valid RF dev for band checking 
03/09/2012   wsh     Fixed incorrected hdrsrchrf_reason_type 
02/06/2012   rkc/smd Added rf reasons for small and large SCI continuation.
02/03/2012   ljl     Checked RF cal before acquisition.
12/22/2011   vlc     Added hdrsrchrf_request_to_sleep() to work with D2L meas.
11/28/2011   ljl     Changed the RxAGC settling time to 0.
01/05/2012   wsh     Added support for SVLTE+SHDR
11/14/2011   wsh     Support Oppurtunistic Diversity TRM
07/19/2011   smd     Mainlined FEATURE_IRAT_USE_MEAS_RF_API.
06/22/2011   vlc     Added hdrsrchrf_get_pri_rxlm_buffer_handle(). 
05/05/2011   cnx     Added hdrsrchrf_get_band_subclass().
03/05/2011   arm     New API added to provide Rx and Tx power info to CM.  
09/17/2010   smd     Used new rf APIs for connected mode. 
08/23/2010   smd     Fixed return type of hdrsrchrf_meas_setup_and_build_script.
06/21/2010   cnx     Fixed compiler warnings.
06/15/2010   ljl     Supported RF MSGR interface.
06/12/2010   smd     Added hdrsrchfc_force_tune_to_chan().
05/12/2010   ljl     Disabled rx before hdr fw standby.
04/26/2010   lyl     Added brute force acq support.
04/05/2010   smd     Added RF APIs for pilot measurement.
03/25/2010   pxu     Rmoved FEATURE_HDR_IDLE_TIMELINE_V3 checks. 
03/18/2010   smd     Added hdrsrchrf_force_chain_unlock() API for IRAT.
03/18/2010   smd     Added hdrsrchrf_force_chain_lock() API for IRAT.
11/09/2009   lyl     Added hdrsrchrf_force_demod_carrier_zero().
06/30/2009   lyl     Updated for rxAGC unlocked mode interface.
06/19/2009   grl     Moved the RF diversity state machine to its own file.
04/08/2009   grl     Updated func name to hdrsrchrf_is_shdr_supported.
03/18/2009   rkc     Added RF SVDO API changes.
02/26/2009   etv     Floor RxAGC1 during diversity tune away.
01/28/2009   lyl     Added hdrsrchrf_init_tune_timer().
11/03/2008   grl     Moved ARDA code into module.
09/18/2008   rkc     Added HDRSRCHRF_DEMOD_PAGE_CONTINUATION in rf reason enum.
08/15/2008   wsh     Fixed crash if TCA contains invalid channel
08/06/2008   ljl     Added hdrsrchrf_is_tuned().
06/16/2008   lyl     Added HDRSRCHRF_HP_BROADCAST reason.
06/08/2008   grl     Added hdrsrchrf_is_hpt_supported API for FTS.
05/09/2008   etv     Created hdrsrchrf_rf_chain_tune_done to find if tuning is
                     done on the given chain.
04/28/2008   ljl     Supported dynamic diversity.
03/28/2008   grl     Supported preping for wakeups with the RTC already on.
01/30/2008   jyw     Added hdrsrchrf_channel_reconfig_is_needed
01/30/2008   jyw     Added the demod assignment list in the RF interfaces.
10/17/2007   grl     Merged Rev B changes from branch.
09/20/2007   mt      HDR Adaptive diversity implementation.
09/18/2007   wsh     Report correct DiversityComb and SimultaneousCommonChannelReceive
                     value in Multi-Mode Discovery protocol
07/18/2007   mt      HDR per band diversity changes.
02/20/2007   grl     Linted clean.
12/27/2006   grl     Added APIs to stop div tune aways
11/10/2006   mt      Increased HDRSRCHRF_RESUME_LOOPS_SETTLING_TIME_HS to 5.
10/26/2006   kss     Added hdrsrchrf_prep_to_wakeup().
10/03/2006   grl     Added small sci paging reason with lower priority.
09/29/2006   grl     Converted acq time to half slots from ms.
09/22/2006   ljl     Added FEATURE_TRM_API_V2
08/21/2006   grl     Added support for the DAST algorithm
08/01/2006   mt      Added enum for HDR AGC ACQ mode.
06/14/2006   sq      Made RF loop settling time programmable
01/15/2006   jyw     Added support to query the exist of multple rx chains.
09/10/2005   grl     Added broadcast access/traffic reasons.
08/29/2005   grl     Removed TRM origination reason.
05/31/2005   grl     Initial AMSS version from trm.h and the old hdrsrchrf.h

============================================================================*/



/*============================================================================

                           INCLUDE FILES FOR MODULE

============================================================================*/

#include "hdr_variation.h"
#include "customer.h"
#include "comdef.h"
#include "rex.h"
#include "timetick.h"

#ifndef FEATURE_HDR_REVB
#include "rf.h"
#include "rfcom.h"
#else

#include "rfm_hdr.h"

#include "rfm_meas.h"

#include "rxlm_intf.h"
#include "txlm_intf.h"

#include "hdrsrchtypes.h"
#endif /* FEATURE_HDR_REVB */
#include "trm.h"
#include "sys_v.h"

#ifdef FEATURE_HDR_MODEM_COEXISTENCE_SW
#include "hdrmultirat.h"
#endif /* FEATURE_HDR_MODEM_COEXISTENCE_SW */

/*============================================================================

                   DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, typesdefs,
and other items needed by this module.

============================================================================*/

#define HDRSRCHRF_MULTI_CHAIN_API
  /* For modules that need to know whether to call the old API or the new
     multi-chain API, this define indicates which one to call. */

/* Time to allow AGC, I/Q OFFSET, and TRK_LO_ADJ loops to stabilize, in
   initial acquisition state and re-acq state, respectively           */
/* Changed to 0 for now since RxAGC settled in RF code */
#define HDRSRCHRF_ACQ_LOOPS_SETTLING_TIME_HS     1
#define HDRSRCHRF_FAST_LOOPS_SETTLING_TIME_HS    0
#define HDRSRCHRF_RESUME_LOOPS_SETTLING_TIME_HS  1

#define HDRSRCH_AGC_SETTLING_ADJ_NV_BITS \
        (( ((uint32) hdrsrch_dbg_msk) >> 8) & 0xF)
  /* Bits [12:9] from HDR L1 debug mask to indicate 
     configurable Reacq AGC settling time in half slots */

#define HDRSRCH_NV_FAST_AGC_SETTLING_TIME_HS \
        (HDRSRCH_AGC_SETTLING_ADJ_NV_BITS ? \
         HDRSRCH_AGC_SETTLING_ADJ_NV_BITS : \
         HDRSRCHRF_FAST_LOOPS_SETTLING_TIME_HS)
  /* HDRSRCH_AGC_SETTLING_ADJ_NV_BITS is not all zeros use it otherwise use 
     the default value for the AGC settling time when using the fast AGC
     acq algorithm. Quantity is in half slots. */

#define HDRSRCH_RESUME_AGC_SETTLING_ADJ_NV_BITS \
        (( ((uint32) hdrsrch_dbg_msk) >> 20) & 0xF)
  /* Bits [23:20] from HDR L1 debug mask to configure the reacq AGC
     settling time in half slots when using the resume AGC acq algorithm. */

#define HDRSRCH_NV_RESUME_AGC_SETTLING_TIME_HS \
        (HDRSRCH_RESUME_AGC_SETTLING_ADJ_NV_BITS ? \
         HDRSRCH_RESUME_AGC_SETTLING_ADJ_NV_BITS : \
         HDRSRCHRF_RESUME_LOOPS_SETTLING_TIME_HS)
  /* HDRSRCH_AGC_SETTLING_ADJ_NV_BITS is not all zeros use it otherwise use 
     the default value for the AGC settling time when using the resume AGC
     acq algorithm. Quantity is in half slots. */

#define HDRSRCHRF_IS_SHDR_ENABLED() \
        ((hdrsrchrf_get_simul_cap() & TRM_SHDR_IS_ENABLED) != 0x0)

#define HDRSRCHRF_IS_SVDO_ENABLED() \
        ((hdrsrchrf_get_simul_cap() & TRM_SVDO_IS_ENABLED) != 0x0)

#define HDRSRCHRF_IS_SVLTE_ENABLED() \
        ((hdrsrchrf_get_simul_cap() & TRM_SVLTE_IS_ENABLED) != 0x0)

/*----------------------------------------------------------------------------
  A logical chain of the HDRSRCHRF module
  ----------------------------------------------------------------------------*/

typedef enum
{      
  HDRSRCHRF_FIRST_CHAIN = 0,
    /* The first chain enum used for iterating through the chains. */

  HDRSRCHRF_PRI_CHAIN = HDRSRCHRF_FIRST_CHAIN,
    /* The primary chain used for any purpose. */

  HDRSRCHRF_DIV_CHAIN,
    /* The diversity chain. */

  HDRSRCHRF_NUM_CHAINS
    /* Total number of chains. */
}
hdrsrchrf_logical_chain_enum_t;


/*----------------------------------------------------------------------------
  What the physical chain which can be associated with a logical chain.
----------------------------------------------------------------------------*/
typedef enum
{
  HDRSRCHRF_DENIED,
    /* Sorry, you don't have the requested resource. */

  HDRSRCHRF_CHAIN0,
    /* You have been granted usage of the primary RF chain */

  HDRSRCHRF_CHAIN1
    /* You have been granted usage of the secondary RF chain */
}
hdrsrchrf_phys_chain_enum_t;


/*----------------------------------------------------------------------------
  What kind of chain is being requested. These represent what the callers
  can ask for.
----------------------------------------------------------------------------*/

typedef enum
{
  /*------------------------------------------------------------------------
    Request a receive chain (Tx chain not required)
  ------------------------------------------------------------------------*/

  HDRSRCHRF_RX_ANY = TRM_RX_ANY,
    /* Any available Rx chain */

  HDRSRCHRF_RX_BEST = TRM_RX_BEST,
    /* ... with the highest sensitivity ("Any" if equal sensitivity) */

  HDRSRCHRF_RX_SECONDARY = TRM_RX_SECONDARY,
    /* The secondary (diversity) receive chain */

  HDRSRCHRF_RX_GPS = TRM_RX_GPS,
    /* The receive chain associated with GPS antenna */


  /*------------------------------------------------------------------------
    Request a transceivier chain (both Rx & Tx)
  ------------------------------------------------------------------------*/

  HDRSRCHRF_RXTX_ANY = TRM_RXTX_ANY,
    /* Any RF chain with both Rx and Tx capability */

  HDRSRCHRF_RXTX_BEST = TRM_RXTX_BEST,
    /* The RXTX RF chain the best sensitivity */


  HDRSRCHRF_LAST_RESOURCE = HDRSRCHRF_RXTX_BEST
    /* For internal bound-checking and array-sizing */
}
hdrsrchrf_resource_enum_t;

/* If we are in DR mode then request RX_any for idle state */
#ifdef FEATURE_HDR_DR_DSDS
#define HDRSRCHRF_GET_TRM_RESOURCE ( hdrmultirat_is_dr_dsds_enabled()? HDRSRCHRF_RX_ANY : HDRSRCHRF_RXTX_BEST )
#else
#define HDRSRCHRF_GET_TRM_RESOURCE HDRSRCHRF_RXTX_BEST
#endif /* FEATURE_HDR_DR_DSDS */

/*----------------------------------------------------------------------------
  RF chain request times
    When a caller needs a resource is composed of two parts:
    1) A start time, when the resource is needed (could be ASAP).
    2) The duration the resource is needed for.
----------------------------------------------------------------------------*/

typedef timetick_type             hdrsrchrf_time_t;
  /* Timestamp type used by HDRSRCHRF (sclks)  See: timetick_get() */


typedef timetick_type             hdrsrchrf_duration_t;
  /* Time duration type used by HDRSRCHRF (sclks) */



/*----------------------------------------------------------------------------
  The reason an RF chain is requested.
    Is used, with the requesting caller, to determine the priority of the
    request.
----------------------------------------------------------------------------*/

typedef enum
{
  HDRSRCHRF_ACCESS = TRM_ACCESS,
    /* Any access attempt. */

  HDRSRCHRF_ACCESS_URGENT = TRM_ACCESS_URGENT,
    /* Access attempts which require fast response time. */

#ifdef FEATURE_HDR_BCMCS
#ifdef FEATURE_HDR_TRM_UNIFICATION
  HDRSRCHRF_BROADCAST_ACCESS = TRM_ACCESS,
    /* Access while in Broadcast */
#else
  HDRSRCHRF_BROADCAST_ACCESS = TRM_BROADCAST_ACCESS,
    /* Access while in Broadcast */
#endif /* FEATURE_HDR_TRM_UNIFICATION */
#endif /* FEATURE_HDR_BCMCS */

  HDRSRCHRF_TRAFFIC = TRM_TRAFFIC,
    /* In a traffic call */

  HDRSRCHRF_DEMOD_PAGE = TRM_DEMOD_PAGE,
    /* Monitoring paging channel */

#ifdef FEATURE_HDR_BCMCS
#ifdef FEATURE_HDR_TRM_UNIFICATION
  HDRSRCHRF_DEMOD_BROADCAST = TRM_SMALL_SCI_PAGE,
    /* Monitoring Broadcast page/data  */
#else
  HDRSRCHRF_DEMOD_BROADCAST = TRM_DEMOD_BROADCAST,
    /* Monitoring Broadcast page/data  */
#endif /* FEATURE_HDR_TRM_UNIFICATION */
#endif /* FEATURE_HDR_BCMCS */

  HDRSRCHRF_ACQUISITION = TRM_ACQUISITION,
    /* Acquiring or reacquiring the system  */

#ifdef FEATURE_HDR_TRM_UNIFICATION

  HDRSRCHRF_ACQUISITION_INV = TRM_ACQUISITION_INV,
    /* Acquisition inversion  */
  HDRSRCHRF_SYNC = TRM_CHANNEL_MAINTENANCE,
    /* Decode broadcast and sync to AN with sync priority */

  HDRSRCHRF_SYNC_INV = TRM_CHANNEL_MAINTENANCE_INV,
    /* Decode broadcast and sync to AN with sync inversion priority */

#else
  HDRSRCHRF_SYNC = TRM_DEMOD_PAGE,
    /* Decode broadcast and sync to AN with sync priority */
#endif /* FEATURE_HDR_TRM_UNIFICATION */

#ifdef FEATURE_HDR_TRM_UNIFICATION
  HDRSRCHRF_OFS = TRM_CHANNEL_MAINTENANCE,
    /* Off Frequency Search */
#else
  HDRSRCHRF_OFS = TRM_OFS,
    /* Off Frequency Search */
#endif

  HDRSRCHRF_DIVERSITY = TRM_DIVERSITY,
    /* Diversity */

#ifdef FEATURE_HDR_TRM_UNIFICATION
  HDRSRCHRF_IDLE_DIVERSITY = TRM_DIVERSITY,
    /* Idle diversity. */
#else
  HDRSRCHRF_IDLE_DIVERSITY = TRM_IDLE_DIVERSITY,
    /* Idle diversity. */
#endif

#ifdef FEATURE_HDR_TRM_UNIFICATION
  HDRSRCHRF_ACCESS_DIVERSITY = TRM_DIVERSITY,
    /* Access diversity. */
#else
  HDRSRCHRF_ACCESS_DIVERSITY = TRM_ACCESS_DIVERSITY,
    /* Access diversity. */
#endif

#ifdef FEATURE_HDR_BCMCS
#ifdef FEATURE_HDR_TRM_UNIFICATION
  HDRSRCHRF_BROADCAST_TRAFFIC = TRM_BG_TRAFFIC,
    /* Monitoring Broadcast while in a traffic connection, mapped to TRM BackGround traffic */
#else
  HDRSRCHRF_BROADCAST_TRAFFIC = TRM_BROADCAST_TRAFFIC,
    /* Monitoring Broadcast while in a traffic connection */
#endif /* FEATURE_HDR_TRM_UNIFICATION */
#endif /* FEATURE_HDR_BCMCS */

#ifdef FEATURE_HDR_TRM_UNIFICATION
  HDRSRCHRF_LOW_LAT_TRAFFIC = TRM_ACCESS_URGENT,
    /* Traffic which requires low latency such as IP based video telephony */
#else
  HDRSRCHRF_LOW_LAT_TRAFFIC = TRM_LOW_LAT_TRAFFIC,
    /* Traffic which requires low latency such as IP based video telephony */
#endif

  HDRSRCHRF_SMALL_SCI_PAGE = TRM_SMALL_SCI_PAGE,
    /* Small SCI paging. Priority is lower since we wakeup so often one
       missed page is not really adverse. This gives 1x acquisition a 
       chance to grab the chain. */

#ifdef FEATURE_HDR_BCMCS 
#ifdef FEATURE_HDR_TRM_UNIFICATION
  HDRSRCHRF_HP_BROADCAST = TRM_ACCESS_URGENT,
#else
  HDRSRCHRF_HP_BROADCAST = TRM_HP_BROADCAST,
    /* High Priority BCMCS  */
#endif /* FEATURE_HDR_TRM_UNIFICATION */
#endif /* FEATURE_HDR_BCMCS */

#ifdef FEATURE_HDR_TRM_UNIFICATION
  HDRSRCHRF_DEMOD_PAGE_CONTINUATION = TRM_CHANNEL_MAINTENANCE_INV,
    /* Intra-SCC wakeup for continuation of demod paging.  Priority is higher
       than small sci page, and higher than 1x page to preserve old
       non-intra-SCC sleep behavior as close as possible. */
#else
  HDRSRCHRF_DEMOD_PAGE_CONTINUATION = TRM_DEMOD_PAGE_CONTINUATION,
    /* Intra-SCC wakeup for continuation of demod paging.  Priority is higher
       than small sci page, and higher than 1x page to preserve old
       non-intra-SCC sleep behavior as close as possible. */
#endif

#ifdef FEATURE_HDR_TRM_UNIFICATION
  HDRSRCHRF_SMALL_SCI_PAGE_CONTINUATION = TRM_CHANNEL_MAINTENANCE_INV,
    /* Intra-SCC wakeup with small SCI demod page continuation */
#else
  HDRSRCHRF_SMALL_SCI_PAGE_CONTINUATION = TRM_SMALL_SCI_PAGE_CONTINUATION,
    /* Intra-SCC wakeup with small SCI demod page continuation */
#endif

  HDRSRCHRF_IRAT_MEASUREMENT = TRM_IRAT_MEASUREMENT,
    /* IRAT measurement reason */

  HDRSRCHRF_BG_TRAFFIC = TRM_BG_TRAFFIC,
    /* In a DR-DSDS traffic call */

#ifdef FEATURE_HDR_PAGE_BLOCKING_MGMT
  HDRSRCHRF_OVHD = TRM_CHANNEL_MAINTENANCE,

  HDRSRCHRF_OVHD_HIGH = TRM_CHANNEL_MAINTENANCE_INV,
#endif /* FEATURE_HDR_PAGE_BLOCKING_MGMT */

#ifdef FEATURE_HDR_TRM_UNIFICATION
  HDRSRCHRF_ACQ_DIVERSITY = TRM_DIVERSITY,
#else
  HDRSRCHRF_ACQ_DIVERSITY = TRM_ACQ_DIVERSITY,
#endif

  HDRSRCHRF_LAST_REASON = TRM_NUM_REASONS
    /* For internal bound-checking and array-sizing */

}
hdrsrchrf_reason_enum_t;

#ifdef FEATURE_HDR_TRM_UNIFICATION
/*----------------------------------------------------------------------------
  The sub-reason an RF chain is requested.
----------------------------------------------------------------------------*/
typedef sys_proc_type_e_type       hdrsrchrf_sub_reason_enum_t;
#endif /* FEATURE_HDR_TRM_UNIFICATION*/
/*----------------------------------------------------------------------------
  Unlock request events
    A higher priority caller needs the resource you hold ...
----------------------------------------------------------------------------*/

typedef enum
{
  HDRSRCHRF_UNLOCK_CANCELLED = TRM_UNLOCK_CANCELLED,
    /* Unlock request cancelled.  You no-longer need to unlock. */

  HDRSRCHRF_UNLOCK_BY = TRM_UNLOCK_BY,
    /* ... at the indicated time.  You can hold the lock until then. */

  HDRSRCHRF_UNLOCK_REQUIRED = TRM_UNLOCK_REQUIRED,
    /* ... as soon as possible.  Release the lock when convenient, but soon. */

  HDRSRCHRF_UNLOCK_IMMEDIATELY = TRM_UNLOCK_IMMEDIATELY,
    /* ... immediately for a page response!  Give it up ASAP! */

  HDRSRCHRF_UNLOCK_BAND_INCOMPATIBLE = TRM_UNLOCK_BAND_INCOMPATIBLE
    /* ... unlock due to band incomp. treat the same as IMMEDIATE */
}
hdrsrchrf_unlock_event_enum_t;



/*----------------------------------------------------------------------------
  The TRM mode settings available.
  ----------------------------------------------------------------------------*/

typedef enum
{
  /* Simultaneous 1x idle with HDR traffic mode */
  HDRSRCHRF_MODE_SIMUL_1XIDLE_HDRTC
}
hdrsrchrf_mode_enum_t;


/*----------------------------------------------------------------------------
  The retain lock state
  ----------------------------------------------------------------------------*/

typedef struct
{
  hdrsrchrf_unlock_event_enum_t  event;
    /* The event being sent to the client */

  uint32                         unlock_by_sclk;
    /* Sclk timestamp for HDRSRCHRF_UNLOCK_BY */

  trm_client_info_t              winning_client;
    /* Who did we lose the chain to */
  
#ifdef FEATURE_HDR_QTA
  trm_client_enum_t              qta_winning_client_id;
    /* The winning client during the QTA  */
#endif /* FEATURE_HDR_QTA */
}
hdrsrchrf_retain_lock_state_t;


/*----------------------------------------------------------------------------
  AGC Algorithm Definition
  ---------------------------------------------------------------------------*/

/* The HDRSRCHRF AGC acqusition mode */
#ifndef FEATURE_HDR_REVB
#ifdef FEATURE_RF_SVDO_API
typedef enum
{
  HDRSRCHRF_NORMAL_AGC_ACQ_MODE = RFM_HDR_AGC_MODE__NORMAL_ACQUISITION,
    /* Normal AGC acquistion mode */

  HDRSRCHRF_FAST_AGC_ACQ_MODE = RFM_HDR_AGC_MODE__FAST_ACQUISITION,
    /* Fast AGC acquistion mode */

  HDRSRCHRF_RESUME_AGC_ACQ_MODE = RFM_HDR_AGC_MODE__RESUME_ACQUISITION
    /* Fast AGC acquistion mode */

} hdrsrchrf_agc_acq_mode_enum_type;
#else
typedef enum
{
  HDRSRCHRF_NORMAL_AGC_ACQ_MODE = RF_HDR_NORMAL_AGC_ACQ_MODE,
    /* Normal AGC acquistion mode */

  HDRSRCHRF_FAST_AGC_ACQ_MODE = RF_HDR_FAST_AGC_ACQ_MODE,
    /* Fast AGC acquistion mode */

  HDRSRCHRF_RESUME_AGC_ACQ_MODE = RF_HDR_RESUME_AGC_ACQ_MODE
    /* Fast AGC acquistion mode */

} hdrsrchrf_agc_acq_mode_enum_type;
#endif /* FEATURE_RF_SVDO_API */
#else
#ifdef FEATURE_RF_SVDO_API
typedef enum
{
  HDRSRCHRF_NORMAL_AGC_ACQ_MODE = RFM_HDR_AGC_MODE__NORMAL_ACQUISITION,
    /* Normal AGC acquistion mode */

  HDRSRCHRF_FAST_AGC_ACQ_MODE   = RFM_HDR_AGC_MODE__FAST_ACQUISITION,
    /* Fast AGC acquistion mode */

  HDRSRCHRF_RESUME_AGC_ACQ_MODE = RFM_HDR_AGC_MODE__RESUME_ACQUISITION,
    /* Fast AGC acquistion mode */

  } hdrsrchrf_agc_acq_mode_enum_type;
#else
typedef enum
{
  HDRSRCHRF_NORMAL_AGC_ACQ_MODE = RFCOM_AGC_MODE__NORMAL_ACQUISITION,
    /* Normal AGC acquistion mode */

  HDRSRCHRF_FAST_AGC_ACQ_MODE   = RFCOM_AGC_MODE__FAST_ACQUISITION,
    /* Fast AGC acquistion mode */

  HDRSRCHRF_RESUME_AGC_ACQ_MODE = RFCOM_AGC_MODE__RESUME_ACQUISITION,
    /* Fast AGC acquistion mode */

  HDRSRCHRF_TRACK_AGC_MODE      = RFCOM_AGC_MODE__TRACKING,
    /* AGC tracking mode */

  HDRSRCHRF_STOP_AGC_MODE       = RFCOM_AGC_MODE__STOPPED,
    /* AGC stopped mode */
} hdrsrchrf_agc_acq_mode_enum_type;
#endif /* FEATURE_RF_SVDO_API */
#endif /* FEATURE_HDR_REVB */

typedef enum
{
  HDRSRCHRF_TUNE_DENIED,
  HDRSRCHRF_TUNE_TIMEOUT,
  HDRSRCHRF_TUNE_SUCCESS
} hdrsrchrf_tune_status_enum_type;

#ifdef FEATURE_HDR_QTA
typedef enum
{
  HDRSRCHRF_QTA_START = TRM_START_STATE,
  HDRSRCHRF_QTA_END = TRM_END_STATE
}hdrsrchrf_client_state_enum_type;
#endif /* FEATURE_HDR_QTA */

/*----------------------------------------------------------------------------
  RF device corresponding to RF chain owned by HDRSRCHRF lock
----------------------------------------------------------------------------*/

#define HDRSRCHRF_NO_DEVICE             RFCOM_MAX_DEVICES
  /* Identifer representing "no RF chain is owned" */

#define HDRSRCHRF_MAX_DURATION          0x7fffffff
  /* Maximum duration a lock can be held. */

/*----------------------------------------------------------------------------
  Definitions for antenna tuner support
  ---------------------------------------------------------------------------*/

#define HDRSRCHRF_ANTENNA_TUNER_ACTION_INTERVAL  6
  /* Interval, in number of search dumps, between consecutive calls to RF 
     for antenna tuner action. RF API for antenna tuner action is called 
     in access and traffic states. */

/* Structure type to provide assigned carrier frequency 
   information to TRM for resource reservation. */
typedef struct 
{
  trm_frequency_type_t               freq_info;
    /* frequency info to be provided to TRM. */

  trm_channel_t                      channel_list[HDRSRCH_MAX_NUM_SUBASETS];
    /* Array of assigned channel numbers. */

} hdrsrchrf_trm_freq_info_type;

/*============================================================================

                            FUNCTION DECLARATIONS

============================================================================*/


/*============================================================================

                       RF Resources Allocation Functions

============================================================================*/


/*===========================================================================

FUNCTION HDRSRCHRF_INIT_TUNE_TIMER

DESCRIPTION
   Define/Initialize the RF tune timer.

DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void hdrsrchrf_init_tune_timer( void );


/*============================================================================

FUNCTION HDRSRCHRF_INIT

DESCRIPTION
  Initialize the HDRSRCHRF module.
  
DEPENDENCIES
  Must be called before any other HDRSRCHRF function.

RETURN VALUE
  None

SIDE EFFECTS
  None

============================================================================*/

void hdrsrchrf_init( void );


/*============================================================================

FUNCTION HDRSRCHRF_RESERVE_AT

DESCRIPTION
  Specifies that the caller needs the given RF resource at the given
  time, for the given duration, for the supplied reason.

  This would be used with the "hdrsrchrf_request()" function, below.
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  If the caller currently holds an RF chain, the chain is released.

============================================================================*/

void hdrsrchrf_reserve_at
(
  hdrsrchrf_logical_chain_enum_t        chain_id,
    /* The logical chain ID associated with the request */

  hdrsrchrf_resource_enum_t             resource,
    /* The RF resource which is being requested */

  hdrsrchrf_time_t                      when,
    /* When the resource will be needed (sclks timestamp) */

  hdrsrchrf_duration_t                  duration,
    /* How long the resource will be needed for (in sclks) */

  hdrsrchrf_reason_enum_t               reason
    /* Why the resource is needed (used for priority decisions) */
);


/*============================================================================

CALLBACK HDRSRCHRF_GRANT_CALLBACK_T

DESCRIPTION
  The prototype for event callback functions, used by the Transceiver manager
  to inform the callers of transceiver management events.
  
DEPENDENCIES
  The callback will be called by the Transceiver Manager.  It may be called
  from a task context of another client, or from interrupt context.

RETURN VALUE
  None

SIDE EFFECTS
  None

============================================================================*/

typedef void (*hdrsrchrf_grant_callback_t)
(
  hdrsrchrf_logical_chain_enum_t        chain_id,
    /* The logical chain ID associated with the request */

  boolean                               granted
    /* Whether the resources where granted */
);


/*============================================================================

FUNCTION HDRSRCHRF_REQUEST_AND_NOTIFY

DESCRIPTION
  Specifies the given caller needs the given RF chain resource, for the
  given duration, for the supplied reason.

  When the resource can be granted to the caller, the event callback for the
  caller will be called with the result of the lock request.
  
DEPENDENCIES
  HDR must not call ANY RF functions related to the chain requested until the
  RF resources are granted after this call is made.

RETURN VALUE
  None

SIDE EFFECTS
  If the caller currently holds an RF chain, the chain is released.

============================================================================*/

void hdrsrchrf_request_and_notify
(
  hdrsrchrf_logical_chain_enum_t        chain_id,
    /* The logical chain ID associated with the request */

  hdrsrchrf_resource_enum_t             resource,
    /* The RF resource which is being requested */

  hdrsrchrf_duration_t                  duration,
    /* How long the resource will be needed for (in sclks) */

  hdrsrchrf_reason_enum_t               reason,
    /* Why the resource is needed (used for priority decisions) */

  hdrsrchrf_grant_callback_t            grant_callback
    /* Callback to notify caller when resource is granted */
);


/*============================================================================

FUNCTION HDRSRCHRF_REQUEST

DESCRIPTION
  Specifies that the RF resource is needed, for how long, and for the supplied
  reason.

  The resource request is immediately evaluated, and the result returned.

  This may be used in conjunction with hdrsrchrf_reserve_at().
  
DEPENDENCIES
  None

RETURN VALUE
  Whether the caller was granted the appropriate resources.

SIDE EFFECTS
  If the caller currently holds an RF chain, that chain is released before
  the request is evaluated.

============================================================================*/

boolean hdrsrchrf_request
(
  hdrsrchrf_logical_chain_enum_t        chain_id,
    /* The logical chain ID associated with the request */

  hdrsrchrf_resource_enum_t             resource,
    /* The RF chain which is being requested */

  hdrsrchrf_duration_t                  duration,
    /* How long the resource will be needed for (in sclks) */

  hdrsrchrf_reason_enum_t               reason
    /* Why the resource is needed (used for priority decisions) */
);

/*============================================================================

FUNCTION HDRSRCHRF_EXCHANGE

DESCRIPTION
  Exchanges the RF resource with home RAT during resel/redirection/HO
  scenarios in IRAT. If the TRM API succeeds HDR owns the chain
  immediately. Otherwise HDR will attempt legacy request_&_notify as
  this may be single RAT scenario where exchange mechanism is not used.
  The exchange mechanism is used only when more than 2 RATs are active
  like in 1xSRLTE, 1xSRLTE+G etc
  
DEPENDENCIES
  Home RAT LTE to have enabled exchange for multiRAT scenario

RETURN VALUE
  Whether the caller was granted the appropriate resources. TRUE if
  the exchange succeeded. FALSE otherwise.

SIDE EFFECTS
  None
============================================================================*/

boolean hdrsrchrf_exchange
(
  hdrsrchrf_duration_t                  duration,
    /* How long the resource will be needed for (in sclks) */

  hdrsrchrf_reason_enum_t               reason
    /* Why the resource is needed (used for priority decisions) */
);

/*============================================================================

FUNCTION HDRSRCHRF_REQUEST_OTHER_RAT

DESCRIPTION
  Request TRM lock in the name of the given client
  
DEPENDENCIES
  None

RETURN VALUE
  Whether the caller was granted the appropriate resources.

SIDE EFFECTS
  If the caller currently holds an RF chain, that chain is released before
  the request is evaluated.

============================================================================*/

boolean hdrsrchrf_request_other_rat
(
  trm_client_enum_t                     client_id,
    /* TRM client ID used to request */

  hdrsrchrf_resource_enum_t             resource,
    /* The RF chain which is being requested */

  hdrsrchrf_duration_t                  duration,
    /* How long the resource will be needed for (in sclks) */

  hdrsrchrf_reason_enum_t               reason
    /* Why the resource is needed (used for priority decisions) */
);

#ifndef FEATURE_TRM_API_V2
/*============================================================================

FUNCTION HDRSRCHRF_CANCEL_RESERVE_NOTIFY

DESCRIPTION
  Cancels any outstanding resource reservation or request_and_notify.

  NOTE: This will NOT release a lock already held by the caller.
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

============================================================================*/

void hdrsrchrf_cancel_reserve_notify
(
  hdrsrchrf_logical_chain_enum_t        chain_id
    /* The logical chain ID associated with the request */
);
#endif


/*============================================================================

FUNCTION HDRSRCHRF_RELEASE

DESCRIPTION
  Release the RF chain resource currently held by a caller.
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  If no client is waiting for the resource, the RF chain will be turned off.

============================================================================*/

void hdrsrchrf_release
(
  hdrsrchrf_logical_chain_enum_t        chain_id
    /* The logical chain ID associated with the request */
);

/*============================================================================

FUNCTION HDRSRCHRF_UPDATE_PRIORITY

DESCRIPTION
  When a chain is used for a different reason, it should change the advertised
  reason for holding the RF chain resource, so its priority will change.

  Eg) A client request the RF chain resource for listening for a PAGE.  If
  it receives one, it would change its priority to PAGE_RESPONSE and attempt
  to respond to the page, and eventually change its priority to TRAFFIC.
  If HDR was using IRAT inherited priority, now it will switch back to
  its native priority. Hence this function should be called only for IRAT
  scenarios.
  
DEPENDENCIES
  The client must be holding a RF chain resource lock

RETURN VALUE
  None

SIDE EFFECTS
  None

============================================================================*/

void hdrsrchrf_update_priority
(
   hdrsrchrf_reason_enum_t               reason
    /* Why the resource is needed (used for priority decisions) */
);

/*============================================================================

CALLBACK HDRSRCHRF_UNLOCK_CALLBACK_T

DESCRIPTION
  The prototype for unlock event callback functions, used by the HDRSRCHRF
  module to inform the callers of when it should unlock a resource
  it holds.
  
DEPENDENCIES
  The callback will be called from a task context of another task, or from
  interrupt context.

RETURN VALUE
  None

SIDE EFFECTS
  None

============================================================================*/

typedef void (*hdrsrchrf_unlock_callback_t)
(
  hdrsrchrf_logical_chain_enum_t        chain_id,
    /* The logical chain ID associated with the request */

  hdrsrchrf_unlock_event_enum_t         event,
    /* The event being sent to the client */

  uint32                                unlock_by_sclk
    /* Sclk timestamp for HDRSRCHRF_UNLOCK_BY */
);


/*============================================================================

FUNCTION HDRSRCHRF_RETAIN_LOCK

DESCRIPTION
  Informs the HDR Search RF Manager that the caller wants to hold
  the resource indefinitely.  The HDRSRCHRF may inform the caller that it must
  give up the lock through the supplied unlock callback.
  
DEPENDENCIES
  The client must be holding a RF chain resource lock

RETURN VALUE
  None

SIDE EFFECTS
  None

============================================================================*/

void hdrsrchrf_retain_lock
(
  hdrsrchrf_logical_chain_enum_t        chain_id,
    /* The logical chain ID associated with the request */

  hdrsrchrf_unlock_callback_t           unlock_callback
    /* The required sclk extension, from "now" */
);


/*============================================================================

FUNCTION HDRSRCHRF_GET_RETAIN_LOCK_STATE

DESCRIPTION
  Retrieves the information related to the current retain lock instance. This
  information is related to when the lock is supposed to be released.
    
DEPENDENCIES
  The client must be holding a RF chain resource lock and have called
  retain lock.

RETURN VALUE
  None

SIDE EFFECTS
  None

============================================================================*/

void hdrsrchrf_get_retain_lock_state
(
  hdrsrchrf_logical_chain_enum_t        chain_id,
    /* The logical chain ID associated with the request */

  hdrsrchrf_retain_lock_state_t*        state
    /* How much time is left on the retain lock state. */
);


/*============================================================================

FUNCTION HDRSRCHRF_UNREGISTER_UNLOCK_CB

DESCRIPTION
  Informs the HDRSRCHRF that the client no longer wants to be notified of 
  unlock requests. Although information in unlock requests will be saved and
  accessible via hdrsrchrf_get_retain_lock_state, the unlock callback will
  no longer be called. A call to hdrsrchrf_retain_lock will cause the unlock
  callback to be called with the information that was saved in the last
  unlock call by TRM..
      
DEPENDENCIES
  The client must have called trm_retain_lock.

RETURN VALUE
  None

SIDE EFFECTS
  None

============================================================================*/

void hdrsrchrf_unregister_unlock_cb
(
  hdrsrchrf_logical_chain_enum_t        chain_id
    /* The logical chain ID associated with the request */
);
 

/*============================================================================

FUNCTION HDRSRCHRF_EXTEND_DURATION

DESCRIPTION
  Attempts to extend the duration an RF chain lock is held for.
  If the entire extension can be granted, it will be granted.
  If the entire extension cannot be granted, the lock duration remains
  unchanged, and the caller should release the lock at the original lock
  expiry point.

  The extension is all or nothing.  If a partial extension is desired, the
  hdrsrchrf_change_duration( ) function should be used.
  
DEPENDENCIES
  The caller must be holding an RF chain lock

RETURN VALUE
 Duration extended for.

SIDE EFFECTS
  None

============================================================================*/

trm_grant_return_enum_type hdrsrchrf_extend_duration
(
  hdrsrchrf_logical_chain_enum_t        chain_id,
    /* The logical chain ID associated with the request */

  hdrsrchrf_duration_t                  duration
    /* The required sclk extension, from "now" */
);


/*============================================================================

FUNCTION HDRSRCHRF_CHANGE_DURATION

DESCRIPTION
  Attempts to extend the duration an RF chain resource lock is held for.
  The lock will be extended as long as possible, but not greater than the
  given maximum.  If minimum is not HDRSRCHRF_DONT_SHORTEN, the lock duration
  may actually be shortened if a higher priority client is waiting for
  the resource.
  
DEPENDENCIES
  The client must be holding an RF chain resource lock

RETURN VALUE
  The new lock duration, from "now", in sclks.

SIDE EFFECTS
  None

============================================================================*/

#define HDRSRCHRF_DONT_SHORTEN          0xFFFFFFFFuL
  /* Don't shorten the lock duration - only extend it, if possible */

hdrsrchrf_duration_t hdrsrchrf_change_duration
(
  hdrsrchrf_logical_chain_enum_t        chain_id,
    /* The logical chain ID associated with the request */

  trm_duration_t                        duration
    /* The new lock duration requested. */
);


/*============================================================================

FUNCTION HDRSRCHRF_CHANGE_PRIORITY

DESCRIPTION
  When a chain is used for a different reason, it should change the advertised
  reason for holding the RF chain resource, so its priority will change.

  Eg) A client request the RF chain resource for listening for a PAGE.  If
  it receives one, it would change its priority to PAGE_RESPONSE and attempt
  to respond to the page, and eventually change its priority to TRAFFIC.
  
DEPENDENCIES
  The client must be holding a RF chain resource lock

RETURN VALUE
  None

SIDE EFFECTS
  None

============================================================================*/

void hdrsrchrf_change_priority
(
  hdrsrchrf_logical_chain_enum_t        chain_id,
    /* The logical chain ID associated with the request */

  hdrsrchrf_reason_enum_t               reason
    /* The new resource why the RF lock is held (used for priority decisions) */
);


/*============================================================================

FUNCTION HDRSRCHRF_GET_RF_DEV

DESCRIPTION
  Get the physical RF device identifier mapped to the given logical chain.
  
DEPENDENCIES
  None
  
RETURN VALUE
  The RF device for the given chain or RF_DEVICE_INVALID if HDR does not
  own the chain

SIDE EFFECTS
  None

=========================================================================*/
rfm_device_enum_type hdrsrchrf_get_rf_dev
( 
  hdrsrchrf_logical_chain_enum_t chain_id
    /* The logical chain ID associated with the request */
);

/*============================================================================

FUNCTION HDRSRCHRF_GET_LAST_VALID_RF_DEV

DESCRIPTION
  Get the physical RF device identifier mapped to the PRI logical chain.
  This function always return a valid RF device. When HDR owns RF, it
  returns the owned RF device, otherwise, it turns the last owned RF, if
  HDR never owned any chain, it returns a default based on RF capability.
  
DEPENDENCIES
  None
  
RETURN VALUE
  When HDR owns RF:             PRI RF chain owned
  When HDR does not own RF:     Last PRI RF chain owned
  When HDR never owned RF:      Default PRI chain based on RF capability
  
SIDE EFFECTS
  None

=========================================================================*/
rfm_device_enum_type hdrsrchrf_get_last_valid_rf_dev( void );

/*============================================================================

FUNCTION HDRSRCHRF_HAS_LOCK

DESCRIPTION
  Whether the given chain is locked and thus controlled by HDR.
  
DEPENDENCIES
  None
  
RETURN VALUE
  True if it is locked by HDR, False otherwise.

SIDE EFFECTS
  None

============================================================================*/

boolean hdrsrchrf_has_lock
(
  hdrsrchrf_logical_chain_enum_t        chain_id
    /* The logical chain ID associated with the request */
);


/*============================================================================

FUNCTION HDRSRCHRF_ENABLE_MODE

DESCRIPTION
  Enable the given TRM config mode if possible.
  
DEPENDENCIES
  None
  
RETURN VALUE
  None

SIDE EFFECTS
  None

============================================================================*/

void hdrsrchrf_enable_mode
( 
  hdrsrchrf_mode_enum_t mode
);


/*============================================================================

FUNCTION HDRSRCHRF_DISABLE_MODE

DESCRIPTION
  Disable the given TRM config mode if possible.
  
DEPENDENCIES
  None
  
RETURN VALUE
  None

SIDE EFFECTS
  None

============================================================================*/

void hdrsrchrf_disable_mode
( 
  hdrsrchrf_mode_enum_t mode
);


/*=========================================================================

FUNCTION HDRSRCHRF_HAS_MULTI_RX

DESCRIPTION 
  This function queries TRM if mulitple RX chains are supported.

DEPENDENCIES
  None

RETURN VALUE

  TRUE  - support multiple receive chains
  FALSE - support single receive chain

SIDE EFFECTS 
  None

=========================================================================*/

boolean hdrsrchrf_has_multi_rx( void );


/*============================================================================

FUNCTION HDRSRCHRF_IS_SHDR_SUPPORTED

DESCRIPTION
  Returns whether the hardware supports SHDR operation.
  
DEPENDENCIES
  None
  
RETURN VALUE
  Whether the hardware supports SHDR operation.

SIDE EFFECTS
  None

============================================================================*/

boolean hdrsrchrf_is_shdr_supported( void );

/*============================================================================

                       External RF Control Functions

============================================================================*/


/*============================================================================

CALLBACK HDRSRCHRF_TUNE_DONE_CB_T

DESCRIPTION
  The prototype for event callback functions, used to report tune status
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

============================================================================*/

typedef void (*hdrsrchrf_tune_done_cb_t)
( 
 hdrsrchrf_tune_status_enum_type status 
);

/*===========================================================================

FUNCTION HDRSRCHRF_TUNE_TO_CHAN

DESCRIPTION
  This function tunes the RF to the specified band and channel and then
  calls the given callback when done. If band tune is not allowed by MCS
  return false.

DEPENDENCIES
  None

RETURN VALUE 
  None for non DR
  For DR:
  TRUE: if tune is allowed
  FALSE if tune fails


SIDE EFFECTS
  None

===========================================================================*/

#ifdef FEATURE_HDR_DR_DSDS
boolean
#else
void
#endif /* FEATURE_HDR_DR_DSDS */
hdrsrchrf_tune_to_chan
(
  hdrsrchrf_logical_chain_enum_t        chain_id,
    /* The logical chain ID associated with the request */
 
  const sys_channel_type                *channel,
    /* Channel to tune the RF to */

#ifdef FEATURE_HDR_REVB
  const uint8                           *demod_idx,
    /* Demod index assignment matching to the channel assignment*/

  uint8                                 channel_cnt,
#endif /* FEATURE_HDR_REVB */

  hdrsrchrf_tune_done_cb_t              tune_done,
    /* Callback when tuning is complete */

  timetick_type                         timeout_ms
    /* timeout in mS, only used for band req API */
);  
  

/*===========================================================================

FUNCTION HDRSRCHRF_PREP_TO_SLEEP

DESCRIPTION
  This function gives the RF module a chance to do any activities
  it needs to do to prepare for sleep.  For now that means stopping
  the AGC and switching the trk_lo mux away from HDR.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void hdrsrchrf_prep_to_sleep( void );


/*===========================================================================

FUNCTION HDRSRCHRF_PREP_TO_WAKEUP

DESCRIPTION
  Does any RF init unique to a wakeup.  If RTC is off, set a timer for 
  1 half-slot in order to delay AGC enabling after the RTC wakeup
  boundary.  

DEPENDENCIES
  None 

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void hdrsrchrf_prep_to_wakeup
( 
  boolean                               rtc_off 
    /* Whether the RTC is still off (which is the desired behavior) */
);


/*===========================================================================

FUNCTION HDRSRCHRF_REPORT_RX_POWER

DESCRIPTION
  This function reports the RX power through diag.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void hdrsrchrf_report_rx_power( void );


/*===========================================================================

FUNCTION HDRSRCHRF_PDM_CAPTURE_VALUES

DESCRIPTION

DEPENDENCIES

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void hdrsrchrf_pdm_capture_values( void );


/*===========================================================================

FUNCTION HDRSRCHRF_PDM_RESTORE_VALUES

DESCRIPTION

DEPENDENCIES

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void hdrsrchrf_pdm_restore_values( void );


/*===========================================================================

FUNCTION HDRSRCHRF_CANCEL_IN_PROGRESS_TUNES

DESCRIPTION
  If any RF chains are in the process of being tuned, cancel the tune so 
  we can power down earlier.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void hdrsrchrf_cancel_in_progress_tunes( void );


/*===========================================================================

FUNCTION HDRSRCHRF_GET_RX_POWER

DESCRIPTION
  This function stores rx power estimate in dBm read from the rx AGC

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void hdrsrchrf_get_rx_power
(
  int *rx0_dBm, 
    /* TXRX0's RX power in dBm */

  int *rx1_dBm, 
    /* RX1's RX power in dBm */

  boolean *diversity  
    /* Whether diversity is on and thux RX1's value is relevant. */
);


/*===========================================================================

FUNCTION HDRSRCHRF_GET_CHANNEL

DESCRIPTION
  This function returns the channel that the given RF chain is tuned to.

DEPENDENCIES
  None

RETURN VALUE
  A pointer to the sys_channel_type* channel structure that defines the channel.  This 
  structure shall not be modified by the caller.

SIDE EFFECTS
  None

===========================================================================*/

const sys_channel_type* hdrsrchrf_get_channel
(
  hdrsrchrf_logical_chain_enum_t        chain_id
    /* The logical chain ID associated with the request */

#ifdef FEATURE_HDR_REVB
  ,uint8                                *chan_cnt,
    /* The number of channel coverred by the current RF config */

  const hdrsrch_demod_idx_type          **demod_idx
    /* A pointor to the demod index assignment table. The structure
       shall not be modified by the caller. */
#endif /* FEATURE_HDR_REVB */
);


/*============================================================================

                       Regional RF Functions

============================================================================*/

/*===========================================================================

FUNCTION HDRSRCHRF_TUNE_SIG

DESCRIPTION
  This function handles the TUNE signal from the Tune Timer.

DEPENDENCIES
  To be called from hdrsrchstate.c mainloop.

RETURN VALUE
  None

SIDE EFFECTS
  TBD

===========================================================================*/

void hdrsrchrf_tune_sig
( 
  hdrsrchrf_logical_chain_enum_t chain_id
    /* Chain ID of the RF chain tuned. */
);


/*============================================================================

                       External Functions

============================================================================*/


/*=========================================================================

FUNCTION HDRSRCHRF_SET_AGC_ACQ_PARAMS

DESCRIPTION
  This function sets the RF AGC acquisition parameters. Note that the AGC
  settings chosen are only persistent for the next tune.

DEPENDENCIES 
  None

RETURN VALUE 
  None
  
SIDE EFFECTS
  None

=========================================================================*/

void hdrsrchrf_set_agc_acq_params
( 
  hdrsrchrf_agc_acq_mode_enum_type  agc_acq_mode,
    /* Mode for AGC acquisition: Normal, Fast, or Resume */

  uint32                            agc_rtc_offset,
    /* RTC offset to use for fast and resume AGC acquisition mode */

  int16                             resume_agc
    /* AGC used to start from when using the resume acquisition algorithm */
);


/*============================================================================

                       External RF Control Functions
                            in Support of DAST
                       (Dynamic AGC Settling Time)

============================================================================*/


/*=========================================================================

FUNCTION HDRSRCHRF_RESET_AGC_EST

DESCRIPTION 
  This function resets the AGC stability channel estimator.

DEPENDENCIES
  None

RETURN VALUE
  None
  
SIDE EFFECTS
  None

=========================================================================*/

void hdrsrchrf_reset_agc_est( void );


/*=========================================================================

FUNCTION HDRSRCHRF_EST_AGC_STABILITY

DESCRIPTION 
  This function estimates the stability of the AGC. It should
  be invoked at the end of every tracking session if the
  AGC resume acquisition algorithm is desired for the next
  time the RF is tuned to the same channel.

DEPENDENCIES
  The radio must be tuned with AGC already settled when called.

RETURN VALUE
  Whether the AGC estimate determines the AGC resume acquisition algorithm
  is appropriate for this channel.
  
SIDE EFFECTS
  Alters the agc_est structure with the information related to this past
  AGC tracking interval.

=========================================================================*/

boolean hdrsrchrf_est_agc_stability
( 
  hdrsrchrf_logical_chain_enum_t    chain_id
    /* The logical chain ID associated with the request */
);


/*=========================================================================

FUNCTION HDRSRCHRF_FORCE_RESUME_AGC_OFF

DESCRIPTION 
  This function indicates that for reasons other than the AGC estimator
  can determine, the resume AGC acquisition algorithm shouldn't be used
  for the next wakeup.

DEPENDENCIES
  This function is latched during hdrsrchrf_est_agc_stability and thus
  must be called prior to take affect.

RETURN VALUE
  None
  
SIDE EFFECTS
  None

=========================================================================*/

void hdrsrchrf_force_resume_agc_off( void );


/*=========================================================================

FUNCTION HDRSRCHRF_GET_PREV_AGC

DESCRIPTION 
  This function returns the last good AGC value seen while awake on 
  the same channel.

DEPENDENCIES
  hdrsrchrf_est_agc_stability must be called before going to sleep
  so that the hdrsrchrf can capture the last AGC.

RETURN VALUE
  The AGC value that was last captured by hdrsrchrf_est_agc_stability.

SIDE EFFECTS
  None

=========================================================================*/

int16 hdrsrchrf_get_prev_agc( void );


/*=========================================================================

FUNCTION HDRSRCHRF_AGC_IS_STABLE

DESCRIPTION 
  This function returns whether the AGC is stable enough to use the resume
  AGC acquisition algorithm for DAST.

DEPENDENCIES
  hdrsrchrf_est_agc_stability must be called before going to sleep
  so that the hdrsrchrf can measure the stability.

RETURN VALUE
  Whether the AGC estimate determined that the AGC is stable enough
  to use the resume acquisition algorithm for DAST.
  
SIDE EFFECTS
  None

=========================================================================*/

boolean hdrsrchrf_agc_is_stable( void );


#ifdef FEATURE_HDR_REVB
/*=========================================================================

FUNCTION HDRSRCHRF_CHANNEL_RECONFIG_IS_NEEDED

DESCRIPTION 
  The function will check if rf reconfiguration is needed given a channel
  list and its corresponding demod assignment list.

DEPENDENCIES
  None

RETURN VALUE
  boolean - if RF reconfiguration ( tune ) is needed given the new channel
            list and its demod assignments.
  
SIDE EFFECTS
  None

=========================================================================*/

boolean hdrsrchrf_channel_reconfig_is_needed
( 
  uint8                         chan_cnt, 
    /* channel count */

  const sys_channel_type*       chan_list,
    /* Channel list */

  const hdrsrch_demod_idx_type* demod_list
    /* Demod assignment list */
);

/*=========================================================================

FUNCTION HDRSRCHRF_GET_CARRIER_IDX

DESCRIPTION 
  This function return the carrier (CSR/SRAM) index assigned to the given
  channel.

DEPENDENCIES
  None

RETURN VALUE
  None
  
SIDE EFFECTS
  None

=========================================================================*/

boolean hdrsrchrf_get_carrier_idx
( 
  sys_channel_type channel,
    /* Channel to query*/

  uint8            *csr_idx
    /* The carrier index to return  */
);


/*=========================================================================

FUNCTION HDRSRCHRF_ENTER_MCDO

DESCRIPTION 
  This function makes RF module enter the MC DO mode. The function should
  be called everytime the HDR enters DO mode from other technology like
  hybrid tune away, wakeup from sleep, etc.

DEPENDENCIES
  None

RETURN VALUE
  None
  
SIDE EFFECTS
  RF module enter the MC DO mode.

=========================================================================*/

void hdrsrchrf_enter_mcdo( void );


/*===========================================================================

FUNCTION HDRSRCHRF_TC_ENTER_IDLE

DESCRIPTION
  The function update the tunning infomation, channel_list, channel_cnt,
  csr configuration, sample servers during the state transition from traffic
  to idle.

DEPENDENCIES
  The function should be called after the set management change from traffic
  to idle.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void hdrsrchrf_tc_enter_idle
(
  sys_channel_type * channel
    /* The channel to idle on */
);

#endif /* FEATURE_HDR_REVB */

/*=========================================================================

FUNCTION HDRSRCHRF_RF_CHAIN_TUNE_DONE

DESCRIPTION 
  This function checks if the RF tuning is done on the given chain.

DEPENDENCIES
  None

RETURN VALUE
  TRUE - If tuning is done on the given chain.
  FALSE - otherwise.
  
SIDE EFFECTS
  None.

=========================================================================*/

boolean hdrsrchrf_rf_chain_tune_done (hdrsrchrf_logical_chain_enum_t chain_id);

/*============================================================================

FUNCTION HDRSRCHRF_RESET_RF_CHAIN_STATE

DESCRIPTION
  Reset the state of the RF chain resource currently held by a caller.  This 
  API should not be called with the HDRSRCHRF_DIV_CHAIN parameter.  Instead
  hdrsrchrf_release_diversity should be called. 
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  The diversity chain will be disabled and released if the primary chain
  was given.

============================================================================*/

void hdrsrchrf_reset_rf_chain_state
(
  hdrsrchrf_logical_chain_enum_t        chain_id
    /* The logical chain ID associated with the request */
);

/*=========================================================================

FUNCTION HDRSRCHRF_IS_BAND_CHAN_SUPPORTED

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
boolean hdrsrchrf_is_band_chan_supported 
( 
  sys_band_class_e_type   band, 
    /* Band */
  word                    chan
    /* Channel number */
);


/*============================================================================

FUNCTION HDRSRCHRF_GET_BAND_SUBCLASS

DESCRIPTION
  This function gets the currently supported band subclasses of the given band.
  
DEPENDENCIES
  None
  
RETURN VALUE
  The mask of the current subclasses of the given band.

SIDE EFFECTS
  None

============================================================================*/
uint32 hdrsrchrf_get_band_subclass
(
  uint8    band_class
    /* Band Class */
);


/*=========================================================================

FUNCTION HDRSRCHRF_IS_TUNED

DESCRIPTION 
  This function returns if the primary chain state is HDRSRCHRF_RF_TUNED.

DEPENDENCIES
  None

RETURN VALUE
  True  - If the primary chain is HDRSRCHRF_RF_TUNED.
  False - Otherwise. 
  
SIDE EFFECTS
  None

=========================================================================*/

boolean hdrsrchrf_is_tuned( void );

#ifdef FEATURE_HDR_LTE_TO_EHRPD_IRAT
/*=========================================================================

FUNCTION HDRSRCHRF_MEAS_SETUP_AND_BUILD_SCRIPT

DESCRIPTION 
  This function calls rf APIs to set up and build scripts for IRAT pilot
  measurement.

DEPENDENCIES
  LTE has to call rfm_meas_common_enter() first

RETURN VALUE
  True   :  the operation succeeds
  False  :  the opreation fails
  
SIDE EFFECTS
  None

=========================================================================*/
rfm_meas_result_type hdrsrchrf_meas_setup_and_build_script
(
  rfm_meas_lte_setup_and_build_scripts_param_type   *p_source_param,
    /* source tech parameters */
  rfm_meas_header_type                              *p_rf_header,
    /* RF header information for source parameters */
  rfm_meas_hdr_setup_and_build_scripts_param_type   *p_target_param,
    /* target tech parameters */
  boolean                                           is_idle_mode
    /* whether it is connected mode measurement */
);                           
#endif /* FEATURE_HDR_LTE_TO_EHRPD_IRAT */

/*===========================================================================

FUNCTION HDRSRCHRF_FORCE_TUNE_TO_CHAN

DESCRIPTION
  This function set up internal data structure and sample server as if RF
  is tuned. It is used for idle mode RF MEAS API work around.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void hdrsrchrf_force_tune_to_chan
(
  hdrsrchrf_logical_chain_enum_t        chain_id,
    /* The logical chain ID associated with the request */
 
  const sys_channel_type                *channel,
    /* Channel to tune the RF to */

#ifdef FEATURE_HDR_REVB
  const hdrsrch_demod_idx_type          *demod_idx,
    /* Demod index assignment matching to the channel assignment*/

  uint8                                 channel_cnt,
#endif /* FEATURE_HDR_REVB */

  hdrsrchrf_tune_done_cb_t              tune_done
    /* Callback when tuning is complete */
);

/*=========================================================================

FUNCTION HDRSRCHRF_FORCE_CHAIN_LOCK

DESCRIPTION 
  This function force hdrsrchrf having rf chain lock. For SVLTE+SHDR
  config, the function will request for TRM lock, and returns FALSE
  if denied

DEPENDENCIES
  None

RETURN VALUE
  TRUE   - if lock is granted
  FALSE  - otherwise
  
SIDE EFFECTS
  None

=========================================================================*/

boolean hdrsrchrf_force_chain_lock
(
  hdrsrchrf_reason_enum_t            reason,
    /* The reason the lock is being held */
  rfm_device_enum_type               rf_chain
    /* The RF physical chain mapped into the logical chain. */
);

/*=========================================================================

FUNCTION HDRSRCHRF_FORCE_CHAIN_UNLOCK

DESCRIPTION 
  This function force hdrsrchrf not having rf chain lock

DEPENDENCIES
  None

RETURN VALUE
  None
  
SIDE EFFECTS
  None

=========================================================================*/

void hdrsrchrf_force_chain_unlock( void );

/*=========================================================================

FUNCTION HDRSRCHRF_DISABLE_RX

DESCRIPTION 
  This function disables rx.

DEPENDENCIES
  None

RETURN VALUE
  None
  
SIDE EFFECTS
  None

=========================================================================*/

void hdrsrchrf_disable_rx
(
  hdrsrchrf_logical_chain_enum_t        chain_id
    /* The logical chain ID associated with the request */
);

/*=========================================================================

FUNCTION HDRSRCHRF_FORCE_DEMOD_CARRIER_ZERO

DESCRIPTION 
  This function forces primary chain to be demod carrier 0.

DEPENDENCIES
  This function is only called when AT changes state from 4a->3a.

RETURN VALUE
  None
  
SIDE EFFECTS
  It will change the hdrsrchrf structure only but no actual tuning.

=========================================================================*/
void hdrsrchrf_force_demod_carrier_zero ( void );

/*=========================================================================

FUNCTION HDRSRCHRF_GET_RXLM_BUFFER_HANDLE

DESCRIPTION 
  This function returns the primary chain RXLM buffer DO is using if the
  buffer has been allocated.

DEPENDENCIES
  None

RETURN VALUE
  Allocation status and buffer handle
  
SIDE EFFECTS
  None

=========================================================================*/

boolean hdrsrchrf_get_rxlm_buffer_handle
( 
  hdrsrchrf_logical_chain_enum_t chain_id,
  lm_handle_type            *rxlm_buf
    /* The RXLM buffer handle */
);

/*===========================================================================

FUNCTION HDRSRCHRF_GET_RXLM_CHAIN

DESCRIPTION
  This function returns the RXLM chain number needed to initialize
  rxlm buffer. 

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
rxlm_chain_type hdrsrchrf_get_rxlm_chain
(
  hdrsrchrf_logical_chain_enum_t hdrsrch_chain
);


/*===========================================================================

FUNCTION HDRSRCHRF_GET_TXLM_CHAIN

DESCRIPTION
  This function returns the RXLM chain number needed to initialize
  rxlm buffer. 

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
txlm_chain_type hdrsrchrf_get_txlm_chain( void );

/*==========================================================================

FUNCTION        HDRSRCHRF_GET_TX_POWER_INFO

DESCRIPTION
  Returns HDR tx power to the caller only in traffic state.
  It returns tx power in 1/10 dBm unit.
  
    
DEPENDENCIES
  

INPUTS
  None

RETURN VALUE 
 hdrsrch_tx_pwr_info_struct_type 
 

SIDE EFFECTS
  none

==========================================================================*/

void hdrsrchrf_get_tx_power_info
(
    hdrsrch_tx_pwr_info_struct_type *tx_pwr_info
);

/*==========================================================================

FUNCTION        HDRSRCHRF_GET_RX_POWER_INFO

DESCRIPTION
  Returns HDR RX power info to the caller. It returns the rx agc per rx chain
  in 1/10 dBm unit, and ecio (nstaneous primary in traffic and combined in idle state)
  in 1/10 dBm unit.
    
DEPENDENCIES
  

INPUTS
  None

RETURN VALUE 
 hdrsrch_rx_pwr_info_struct_type 
 

SIDE EFFECTS
  none

==========================================================================*/

void hdrsrchrf_get_rx_power_info
( 
    hdrsrch_rx_pwr_info_struct_type *rx_pwr_info
);

/*===========================================================================

FUNCTION HDRSRCHRF_REQUEST_TO_SLEEP

DESCRIPTION
  This function gives the RF module a chance to do any activities
  it needs to do to prepare for sleep.  For now that means stopping
  the AGC.
  
DEPENDENCIES
  This function must be called with FPD disabled

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void hdrsrchrf_request_to_sleep( void );


/*==========================================================================

FUNCTION        HDRSRCHRF_IS_BAND_CHAN_SUPPORTED_V2

DESCRIPTION
  Returns RF supports the band/chan with good RF Cal.
    
DEPENDENCIES
  

INPUTS
  band, chan

RETURN VALUE 
 TRUE: good RF cal for band/chan.
 FALSE: Bad RF cal.
 

SIDE EFFECTS
  none

==========================================================================*/

boolean hdrsrchrf_is_band_chan_supported_v2
(
  sys_band_class_e_type   band, 
    /* Band */
  word                    chan
    /* Channel number */
);


/*============================================================================

FUNCTION HDRSRCHRF_CHANGE_REASON

DESCRIPTION 
  This function is only used in DO to LTE reselection.  It is called to determine
  if TRM will allow the chain to be used for IRAT measurements.
 
DEPENDENCIES
  The client must be holding a RF chain resource lock

RETURN VALUE
  None

SIDE EFFECTS
  None

============================================================================*/

trm_grant_return_enum_type hdrsrchrf_change_reason
(
  hdrsrchrf_logical_chain_enum_t        chain_id,
    /* The logical chain ID associated with the request */

  hdrsrchrf_reason_enum_t               reason
    /* The new resource why the RF lock is held (used for priority decisions) */
);

/*============================================================================

FUNCTION HDRSRCHRF_PREP_FOR_IRAT_MEAS

DESCRIPTION 
  This function is only used in DO to LTE reselection.  It is called as part of
  the TRM reservation sequence for IRAT measurements.
 
DEPENDENCIES
  The client must be holding a RF chain resource lock

RETURN VALUE
  None

SIDE EFFECTS
  None

============================================================================*/

hdrsrchrf_duration_t hdrsrchrf_prep_for_irat_meas
(
  hdrsrchrf_logical_chain_enum_t        chain_id,
    /* The logical chain ID associated with the request */

  hdrsrchrf_duration_t                  duration
    /* The new lock duration granted. */
);

/*==========================================================================

FUNCTION        HDRSRCHRF_SET_FUTURE_BAND

DESCRIPTION
  This function sets the future band HDR will be on. This function
  must be called before requesting a TRM lock if the previous band
  HDR was on (for PRI chain) was different. so that TRM can have 
  the updated bandclass info to unlock 1x if needed.
    
DEPENDENCIES
  It must be called when HDR does not own the chain.

INPUTS
  RF chain id.
  Pointer to populated band/channel information.

RETURN VALUE 
 TRUE    if HDR does not own the chain
 FALSE   Otherwise
 

SIDE EFFECTS
  none

==========================================================================*/
boolean hdrsrchrf_set_future_band
(
  hdrsrchrf_logical_chain_enum_t  chain_id,
  hdrsrchrf_trm_freq_info_type    *freq_info_ptr
);

/*==========================================================================

FUNCTION        HDRSRCHRF_ANTENNA_TUNER_ACTION

DESCRIPTION
  This function calls RF API for antenna tuner action.
  The function is expected to be called approximately 
  every 200 ms.
    
DEPENDENCIES
  None.

INPUTS
  None

RETURN VALUE 
  None

SIDE EFFECTS
  none

==========================================================================*/
void hdrsrchrf_antenna_tuner_action( void );


/*==========================================================================

FUNCTION        HDRSRCHRF_SET_TRM_FREQ_INFO

DESCRIPTION
  This function sets band/channel information to be passed to TRM for 
  resource reservation.
    
DEPENDENCIES
  None

INPUTS
  Pointer to populated band/channel information.

RETURN VALUE 
  None

SIDE EFFECTS
  none

==========================================================================*/
void hdrsrchrf_set_trm_freq_info
(
  hdrsrchrf_trm_freq_info_type *freq_info_ptr
);

/*==========================================================================

FUNCTION        HDRSRCHRF_GET_TRM_FREQ_INFO

DESCRIPTION
  This function returns pointer to saved band/channel information to be 
  passed to TRM for resource reservation.
    
DEPENDENCIES
  hdrsrchrf_set_trm_freq_info() must be called before calling this function.

INPUTS
  None.

RETURN VALUE 
  Pointer to populated band/channel information.

SIDE EFFECTS
  none

==========================================================================*/
hdrsrchrf_trm_freq_info_type * hdrsrchrf_get_trm_freq_info( void );

/*==========================================================================

FUNCTION        HDRSRCHRF_GET_CHAIN_HELD_REASON

DESCRIPTION
  This function returns the reason for which the chain is held
  currently by HDR
    
DEPENDENCIES
  None

INPUTS
  chain id

RETURN VALUE 
  Reason for taking the chain 

SIDE EFFECTS
  none

==========================================================================*/
hdrsrchrf_reason_enum_t hdrsrchrf_get_chain_held_reason
(
  hdrsrchrf_logical_chain_enum_t  chain_id
);

/*============================================================================

FUNCTION HDRSRCHRF_REENABLE_REQUEST_NOTIFY

DESCRIPTION
  Resets the content of the req_notify_pending flag

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

============================================================================*/

void hdrsrchrf_reenable_request_notify(void);


#ifdef FEATURE_HDR_MODEM_COEXISTENCE_SW
/*==========================================================================

FUNCTION        HDRSRCHRF_SEND_TX_FREQ_TO_COEX

DESCRIPTION
  This function sends frequency change info to coex manager.
    
DEPENDENCIES
  None

INPUTS
  chain id

RETURN VALUE 
  None

SIDE EFFECTS
  none

==========================================================================*/
void hdrsrchrf_send_tx_freq_to_coex
(
   rfm_device_enum_type                  rf_device
     /* The logical chain ID */
);
#endif /* FEATURE_HDR_MODEM_COEXISTENCE_SW */

#ifdef FEATURE_HDR_PAGE_BLOCKING_MGMT
/*==========================================================================

FUNCTION        HDRSRCHRF_REGISTER_SCI_WITH_TRM 
 
DESCRIPTION
  This function registers the SCI with TRM. TRM uses it for its page block
  management algorithms.
    
DEPENDENCIES
  None

INPUTS
  Sleep cycle in milli seconds

RETURN VALUE 
  None

SIDE EFFECTS
  None

==========================================================================*/
void hdrsrchrf_register_sci_with_trm
(
   uint16                               sci_ms
     /* sleep cycle in milli secs */
);

/*==========================================================================

FUNCTION        HDRSRCHRF_REGISTER_PBR_MODE_WITH_TRM 
 
DESCRIPTION
  This function registers the PBR mode with TRM. 
    
DEPENDENCIES
  None

INPUTS
  None

RETURN VALUE 
  None

SIDE EFFECTS
  None

==========================================================================*/
void hdrsrchrf_register_pbr_mode_with_trm(void);
#endif /* FEATURE_HDR_PAGE_MANAGEMENT */

#ifdef FEATURE_HDR_QTA

/*===========================================================================

FUNCTION HDRSRCHRF_GET_QTA_WINNING_CLIENT_ID

DESCRIPTION
  This function returns the winning client ID information
  
DEPENDENCIES
  None

RETURN VALUE
  Winning Client ID of given QTA

SIDE EFFECTS
  None

===========================================================================*/

trm_client_enum_t hdrsrchrf_get_qta_winning_client_id(  void );

/*===========================================================================

FUNCTION HDRSRCHRF_SET_QTA_WINNING_CLIENT_ID

DESCRIPTION
  This function stores the winning client ID information.
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void hdrsrchrf_set_qta_winning_client_id( void );

#ifdef FEATURE_HDR_QTA_THRU_TRM
/*===========================================================================

FUNCTION HDRSRCHRF_SET_QTA_CLIENT_STATE

DESCRIPTION
  This function calls TRM API to update the QTA gap start and end.
  
DEPENDENCIES
  Expects non-NULL RF params for start QTA
  Return value is valid only for end QTA

RETURN VALUE
  TRUE if the previous gap was an empty gap
  FALSE otherwise

SIDE EFFECTS
  TRM initiates QTA with target RAT. But target RAT can start QTA
  only after source tech HDR here releases the primary chain

===========================================================================*/

boolean hdrsrchrf_set_qta_client_state
(
  hdrsrchrf_client_state_enum_type state, /* START_QTA or END_QTA */
  rfm_meas_common_param_type *rf_params /* Params needed for build scripts */
);

#else /* FEATURE_HDR_QTA_THRU_TRM */

/*===========================================================================

FUNCTION HDRSRCHRF_SET_QTA_CLIENT_STATE

DESCRIPTION
  This function calls TRM API to update the QTA gap start and end.
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void hdrsrchrf_set_qta_client_state( hdrsrchrf_client_state_enum_type state );

#endif /* FEATURE_HDR_QTA_THRU_TRM */
#endif /* FEATURE_HDR_QTA */

/*============================================================================

FUNCTION HDRSRCHRF_TRM_ASYNC_CB

DESCRIPTION 
  This is the callback called by TRM in response to any TRM API's invoked. 
  
DEPENDENCIES
  In response to TRM API
  
RETURN VALUE
  None
  
SIDE EFFECTS
  None

=========================================================================*/
void hdrsrchrf_trm_async_cb
( 
   trm_async_callback_information_type  *trm_cb_info
     /* Callback information passed back */
);

/*============================================================================

FUNCTION HDRSRCHRF_GET_RF_DEVICE

DESCRIPTION 
  This function returns the RF dev for the client. 
  
DEPENDENCIES
  None

RETURN VALUE
  RF Dev

SIDE EFFECTS
  None

============================================================================*/
rfm_device_enum_type hdrsrchrf_get_rf_device
( 
   trm_client_enum_t               client
    /* The client which is being informed of an event */
);

/*============================================================================

FUNCTION HDRSRCHRF_GET_REASON

DESCRIPTION 
  This function returns the trm reason for the client being queried. 
  
DEPENDENCIES
  None

RETURN VALUE
  Client TRM reason

SIDE EFFECTS
  None

============================================================================*/
trm_reason_enum_t  hdrsrchrf_get_reason
( 
   trm_client_enum_t               client
    /* Which client we need the TRM reason for */
);

/*============================================================================

FUNCTION HDRSRCHRF_REGISTER_WITH_TRM

DESCRIPTION 
  Register for primary  and secondary TRM async callback
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

============================================================================*/
void hdrsrchrf_register_with_trm ( void );

#ifdef FEATURE_HDR_DR_DSDS
/*============================================================================

FUNCTION HDRSRCHRF_UPDATE_DR_MODE

DESCRIPTION 
  Function to check with TRM if we are in DR mode
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

============================================================================*/
void hdrsrchrf_update_dr_mode ( void );
#endif /* FEATURE_HDR_DR_DSDS */

/*===========================================================================

FUNCTION HDRSRCHRF_BAND_GRANT_CB

DESCRIPTION
  This function is called by TRM when it grants the band tune.

DEPENDENCIES
  This function executes in arbiturary task and should take great care
  when modifying HDRSRCH data structure.

RETURN VALUE
  TRUE:  If band is granted
  FALSE: If band if pending/denied

SIDE EFFECTS
  None

===========================================================================*/
boolean hdrsrchrf_band_grant_cb
(
  trm_client_enum_t client,    /* Client requested for the band change */
  trm_band_type band,          /* Band class */
  trm_modify_return_enum_type granted,   /* Status of band grant */
  trm_request_tag_t tag
);

/*===========================================================================

FUNCTION HDRSRCHRF_GET_PRI_CHAIN_UNLOCK_STATUS

DESCRIPTION
  This function is called to get primary chain unlock status

DEPENDENCIES
  none

RETURN VALUE 
                  TRUE --> UNLOCK is ignored by HDR primary chain as GSM is winning client
                  FALSE --> UNLOCK is honoured by HDR
SIDE EFFECTS
  None

===========================================================================*/
extern boolean hdrsrchrf_get_pri_chain_unlock_status( void );

/*==========================================================================

FUNCTION        HDRSRCHRF_GET_CLIENT_ASID_MAPPING 

DESCRIPTION
  Gets the GSM client ASID mapping.
  TRM_GSM1 will always map to Multi-mode subscription 
    
DEPENDENCIES
  None

INPUTS
  Client id

RETURN VALUE 
  ASID corresponding to a client, currently this is being handled
  only for GSM client

SIDE EFFECTS
  None

==========================================================================*/
sys_modem_as_id_e_type hdrsrchrf_get_client_asid_mapping
(
   trm_client_enum_t                client_id
     /* client_id */
);

/*==========================================================================

FUNCTION        HDRSRCHRF_GET_SIMUL_CAP 

DESCRIPTION
  TRM get simultaneous capability 
    
DEPENDENCIES
  None

INPUTS
  None

RETURN VALUE 
  TRM Simultaneous Capability Information

SIDE EFFECTS
  None

==========================================================================*/
uint32 hdrsrchrf_get_simul_cap( void );

/*===========================================================================

FUNCTION HDRSRCHRF_CANCEL_BAND_GRANT_REQUEST

DESCRIPTION
  This function cancels any pending band grant request if we are waiting on
  one.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  none

===========================================================================*/

void hdrsrchrf_cancel_band_grant_request
( 
  hdrsrchrf_logical_chain_enum_t chain_id
    /* Chain ID of the RF chain tuned. */
);

#ifdef FEATURE_HDR_TRM_UNIFICATION
/*===========================================================================

FUNCTION HDRSRCHRF_GET_SUB_REASON

DESCRIPTION
  This function returns the appropriate subreason based on the reason that is being passed 
  to TRM.

DEPENDENCIES
  None

RETURN VALUE
  sub_reason to be passed to TRM

SIDE EFFECTS
  none

===========================================================================*/

hdrsrchrf_sub_reason_enum_t hdrsrchrf_get_sub_reason
(
  hdrsrchrf_reason_enum_t reason
);
#endif /* FEATURE_HDR_TRM_UNIFICATION*/
#endif /* HDRSRCHRF_H */
