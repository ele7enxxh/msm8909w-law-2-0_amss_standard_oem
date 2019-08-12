#ifndef HDRSRCHI_H
#define HDRSRCHI_H

/*===*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                        H D R    S R C H   I N C L U D E

                          Search types Header File

GENERAL DESCRIPTION

   This file contains regional HDR Search internal parameters.

EXTERNALIZED FUNCTIONS

REGIONAL FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS

  Copyright (c) 2000-2013 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
                
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

============================================================================

                        EDIT HISTORY FOR MODULE

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/hdr/srch/api/hdrsrchi.h#1 $ $DateTime: 2016/12/13 07:58:05 $ $Author: mplcsds1 $

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

when         who     what, where, why
----------   ---     ------------------------------------------------------- 
12/18/2014   wsh     Enabled MMSE by default 
10/01/2014   arm     Changes to support HDR band avoidance.
04/28/2014   wsh     Added NV control to pass MMSE config to FW 
11/29/2013   sat     NV control for Lock Release Enhancement (LRE)
04/22/2013   arm     Added changes for DSDA coexistence manager.
03/19/2012   mbs     Added Dime Time Transfer changes
03/19/2012   ljl     Fixed Dime compiler errors.
01/09/2012   smd     Supported dynamic acquisition diversity.
01/06/2011   smd     Added NV items for acquistion diversity.  
01/25/2012   ljl     Updated RSSI and Ec/Io at search dump.
11/27/2011   grl     Removed obsolete sleep API.
11/11/2011   arm     HDR idle and access state diversity implemetation.
11/07/2011   smd     Add NV items for idle/access diversity.
11/04/2011   ljl/smd Disabled WUTA for 8960. 
10/25/2011   ljl     Handled the failure cases during optimized HO.
10/10/2011   arm     Support second chain test call.
08/19/2011   lyl     Added NV control for FL data adaptive RxD.
03/22/2011   arm     Added support for thermal pa backoff feature. 
09/14/2010   arm     Replace TASKLOCK  with critical section.
07/23/2010   ljl     Enabled access hybrid mode.
06/24/2010   grl     Changed a macro definition.
06/11/2010   smd/vlc Changed bit3 of hdrsrch_dbg_msk to enable full PN circle 
                     search test.
05/20/2010   smd     Fixed bugs for Reverse Time Transfer.
04/26/2010   lyl     Added brute force acq support.
03/27/2010   pxu     Removed feature checks for FEATURE_HDR_IDLE_TIMELINE_V3 
03/26/21010  ljl     Disabled access hybrid mode on mdm9k. 
03/19/2010   ljl     Added FEATURE_HDR_FORCE_NORMAL_AGC_ACQ for MDM9K. 
02/12/2010   ljl     Added div settings in hdrsrch struct.
07/22/2009   ljl     Added frame offset.
11/23/2008   grl     Moved ARDA code into hdrsrchrf module.
11/23/2008   grl     Created feature for defaulting DAST on.
01/23/2008   jyw     Added aset demod index list into the hdrsrch structure.
01/23/2008   jyw     Added revB NVs for the LO offset and the demod override.
11/20/2007   jyw     Made hdrsrch sleep macros to use new sleep API.
11/14/2007   jyw     Added the NV bit for the sleep RTC latch.
11/13/2007   rkc     Added handdown mode in hdrsrch_type.
11/07/2007   jyw     Added NV controls for thermal AFC.
10/17/2007   jyw     Added revB set management support.
10/05/2007   jyw     Added NV controls for eqInt and xoPLL in debug array.
09/20/2007   mt      HDR Adaptive diversity implementation.
06/18/2007   rkc     Added support for hdr debug array.
06/02/2007   grl     Added WUTA enabling bit. Removed variance reduction bit.
04/27/2007   grl     Removed tc_mode from TC assignment for access hybrid ctrl.
04/10/2007   mt      Added NV item bits for ATOM implementation.
01/12/2007   grl     Turned on DAST by default.
12/11/2006   ljl     Removed afc_timer.
12/05/2006   grl     Turned off DAST by default.
11/10/2006   mt      Mainlined most sleep optimization features. 
10/06/2006   jyw     Added NV control for not using interpolated srch results.
09/29/2006   grl     Converted agc acq time to half slots from ms.
09/07/2006   ljl     Enabled hybrid access by default.
08/29/2006   mt      Removed sleep overhead NV item definition.
08/21/2006   grl     Added more NV item macros to support DAST debugging
08/01/2006   mt      Defined new HDRSRCH L1 NV item macro.
08/01/2006   ljl     Added debug bit for access hybrid
08/01/2006   grl     Name change of dec deactivate macro.
07/27/2006   mt      Modified HDRSRCH L1 NV item macros.
07/18/2006   grl     Added ungraceful decoder deactivation capability.
07/10/2006   mt      Added support for sleep timeline optimization.
06/07/2006   ljl     Added SBHO optimization.
09/06/2005   ds      Added AFC timer to hdrsrch_struct_type
03/23/2005   hrk     Added gp_timer2 to hdrsrch_struct_type to do timed wait.
11/03/2004   mpa     Removed dependency on std.h (merged with sys.h)
06/30/2004   dna     Added deactivate_done_cb to hdrsrch struct
06/21/2004   sq      Changed tune_timer to use SCLK timer
11/18/2003   aaj     Added system usability callback
03/11/2003   ajn     Updated Copyright notice
11/25/2002   ajn     Added suspend callback.
01/30/2002   ajn     Status moved into command structure
09/04/2001   ajn     Added command in progress data to hdrsrch struct
02/23/2001   ajn     Removed 400/426.7ms constants - moved to HDRTS.
11/22/2000   ajn     Code review changes
11/08/2000   ajn     Moved SCC/CCC lengths into general search parameters
10/31/2000   ajn     Added General Purpose Signals to State Machine
10/23/2000   ajn     Moved hdrsrch_struct_type from hdrsrchstate.h
10/23/2000   ajn     Moved Sector struct to hdrsrchsect; moved fing, set and
                     srch data into hdrsrch{fing,set,drv}.h
10/09/2000   aaj     Added pilot report enum type
09/27/2000   aaj     Added keep flag to the sector structure
09/11/2000   aaj     Added pn circle definitions for X2
09/06/2000   aaj     Added drop timer and pilot report flag to pilot struct
07/27/2000   ajn     Merged into HDR Search Task codebase
06/27/2000   aaj     Compiled the code successfully
06/24/2000   aaj     Created this file

==========================================================================*/


/*==========================================================================

                     INCLUDE FILES FOR MODULE

==========================================================================*/

#include "hdr_variation.h"
#include "rex.h"
#include "hdrsrchcmd.h"

#include "timer.h"
/*==========================================================================

                     DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, typesdefs,
and other items needed by this module.

==========================================================================*/



/*==========================================================================

                     MACROS USED IN THE MODULE

This section contains local definitions for constants, macros, typesdefs,
and other items needed by this module.

==========================================================================*/

/*------------------------------------------------------------------------

  HDRL1 DEBUG MASK BITS:

  Bit 0 - Enable power probes.
  Bit 1 - Enable the WUTA Algorithm (Warm Up Time Adjustment Algorithm)
          (0: ENABLED ; 1: DISABLED).
  Bit 2 - Enable forced premable detection (0: ENABLED ; 1: DISABLED).
  Bit 3 - Enable full PN circle search for IRAT pilot measurement mode.

  Bit 4         - Enable using the uninterpolated search results.

  Bit 5         - Disable adaptive diversity control algorithm
  
  Bits [6]      - Enable forced idle and TC OFS.
                   
  Bit 7         - To disable access hybrid
  
  Bits [15..8] - The LO offset of the ghost channel in the unit of channel
                 separation step which can be different for different band.
  

  Bit  [17..16] - 11 - Enable only the normal (legacy) AGC acq algorithm.
                - 00 - Enable new fast AGC acquisition algorithm.
                - 10 - Enable 2 half slot resume AGC acquisition algorithm.
                - 01 - Enable DAST which switches between the two algorithms.
                NOTE: If FEATURE_HDR_DEFAULT_DAST_ENABLED is disabled
                then 00 and 01 are swapped.

  Bit  [18]     - Disable the RTC latch when going to sleep.

  Bit  [19]     - Unused.

  Bit 20        - The demod index assignment overriden mode enable bit.

  Bits [22..21] -  The first overriding demod index assignment. 

  Bits [23..20] - Resume AGC algorithm AGC loop settling time in half slots.
                  (If bits are '0000' then default value is used, 
                   otherwise same as the binary value i.e. '1001' is 9 hs)

  Bits [27..24] - Resume AGC algorithm reacquisition time in slots. 
                  (If bits are '0000' then default value is used , 
                   otherwise same as the binary value i.e. '0110' is 6 slots)

  Bits [31..28] - Force the SCI to the given value if bits are non-zero.

------------------------------------------------------------------------*/

#define HDRSRCHSLEEP_POWER_STROBE_NV_ENABLE \
        ((( (uint32) hdrsrch_dbg_msk) & 0x1) == TRUE )
  /* Bits 0 from HDR L1 debug mask to indicate 
     whether strobes for power profiling is enabled */

#define HDRSRCH_WUTA_NV_ENABLE \
        ((( ((uint32) hdrsrch_dbg_msk) >> 1) & 0x1) == TRUE )
  /* Bit 1 from HDR L1 debug mask to indicate whether WUTA is enabled */

#define HDRSRCH_FORCED_PREAMBLE_DETECT_NV_ENABLE \
        ((( ((uint32) hdrsrch_dbg_msk) >> 2) & 0x1) == FALSE )
  /* Bit 2 from HDR L1 debug mask to indicate 
     whether forced preamble detection is enabled */

#define HDRSRCH_PILOT_MEAS_PN_CIRCLE_SEARCH_NV_ENABLE \
        ((( ((uint32) hdrsrch_dbg_msk) >> 3) & 0x1) == TRUE )
  /* Bit 3 from HDR L1 debug mask to indicate 
     whether to turn on full PN circle search for IRAT pilot
     measurement. */

#define HDRSRCH_FORCE_SCI_NV_ENABLE \
        ((( ((uint32) hdrsrch_dbg_msk) >> 28 ) & 0xF) != FALSE  )
  /* Bits [28:31] from HDR L1 debug mask to indicate whether
     the SCI is forced. */

#define HDRSRCH_FORCE_SCI_NV_VALUE \
        (( ((uint32) hdrsrch_dbg_msk) >> 28 ) & 0xF)
  /* Bits [28:31] from HDR L1 debug mask to indicate what the
     SCI is forced to. */

#ifdef FEATURE_HDR_FORCE_NORMAL_AGC_ACQ
#define HDRSRCH_NORMAL_AGC_ACQ_NV_ENABLE (1)
  /* TBD: remove when fast/resume agc work */
#else
#define HDRSRCH_NORMAL_AGC_ACQ_NV_ENABLE \
        ((( ((uint32) hdrsrch_dbg_msk) >> 16) & 0x3) == 0x3 )
  /* Bits [16:17] from HDR L1 debug mask to indicate 
     whether the normal AGC algorithm is forced on for reacq */
#endif /* FEATURE_HDR_FORCE_NORMAL_AGC_ACQ */

#define HDRSRCH_RESUME_AGC_ACQ_NV_ENABLE \
        ((( ((uint32) hdrsrch_dbg_msk) >> 16) & 0x3) == 0x2 )
  /* Bits [16:17] from HDR L1 debug mask to indicate 
     whether to enable fast AGC acquisition algorithm for Reacq */

#ifdef FEATURE_HDR_DEFAULT_DAST_ENABLED
#define HDRSRCH_FAST_AGC_ACQ_NV_ENABLE \
        ((( ((uint32) hdrsrch_dbg_msk) >> 16) & 0x3) == 0x1 )
  /* Bits [16:17] from HDR L1 debug mask to indicate 
     whether to enable fast AGC acquisition algorithm for Reacq */

#define HDRSRCH_DAST_AGC_ACQ_NV_ENABLE \
        ((( ((uint32) hdrsrch_dbg_msk) >> 16) & 0x3) == 0x0 )
  /* Bits [16:17] from HDR L1 debug mask to indicate 
     whether DAST is enabled which dynamically switches between
     the fast and resume AGC acq algorithms. */
#else 
#define HDRSRCH_FAST_AGC_ACQ_NV_ENABLE \
        ((( ((uint32) hdrsrch_dbg_msk) >> 16) & 0x3) == 0x0 )
  /* Bits [16:17] from HDR L1 debug mask to indicate 
     whether to enable fast AGC acquisition algorithm for Reacq */

#define HDRSRCH_DAST_AGC_ACQ_NV_ENABLE \
        ((( ((uint32) hdrsrch_dbg_msk) >> 16) & 0x3) == 0x1 )
  /* Bits [16:17] from HDR L1 debug mask to indicate 
     whether DAST is enabled which dynamically switches between
     the fast and resume AGC acq algorithms. */
#endif /* !FEATURE_HDR_DEFAULT_DAST_ENABLED */

#define HDRSRCH_NV_FORCE_IDLE_OFS_ENABLE \
        ((( ((uint32) hdrsrch_dbg_msk) >> 6 ) & 0x1 ) == TRUE )
  /* Bits 5 from HDR L1 debug mask to force idle OFS */

#define HDRSRCH_NV_FORCE_TC_OFS_ENABLE \
        ((( ((uint32) hdrsrch_dbg_msk) >> 6 ) & 0x1 ) == TRUE )
  /* Bits 6 from HDR L1 debug mask to force idle OFS */

#define HDRSRCH_NV_ENABLE_SLEEP_RTC_LATCH \
        ((( ((uint32) hdrsrch_dbg_msk) >> 18) & 0x1) == 0x0 )
  /* Bit 18 used for the debug purpose to turn off the RTC latch
     when going to sleep */

#ifdef HDRSRCH_1X_SEARCH_TEST

#define HDRSRCH_NV_OFS_1X_FREQUENCY \
        (( hdrsrch_dbg_msk >> 32) & 0x0FFF )
  /* 1x OFS frequency */

#define HDRSRCH_NV_OFS_1X_BAND \
        ((( hdrsrch_dbg_msk >> 32) & 0xF000) >> 12 )
  /* 1x OFS band */

#define HDRSRCH_NV_OFS_1X_PN \
        ( (( hdrsrch_dbg_msk >> 48) & 0x00FF ) << 1 )
  /* 1x OFS PN in increments of 2 */

#define HDRSRCH_ENABLE_IDHO_DYNAMIC_THRESH \
        ((( ((uint32) hdrsrch_dbg_msk) >> 19) & 0x1) == FALSE )

#define HDRSRCH_IDHO_THRESH_LO_CACHED 0

#define HDRSRCH_IDHO_THRESH_LO_NOT_CACHED 0

#define HDRSRCH_IDHO_THRESH_HI_CACHED 0

#define HDRSRCH_IDHO_THRESH_HI_NOT_CACHED 0

#else

#define HDRSRCH_NV_OFS_1X_FREQUENCY 0
#define HDRSRCH_NV_OFS_1X_BAND  0


#define HDRSRCH_ENABLE_IDHO_DYNAMIC_THRESH \
        ((( ((uint32) hdrsrch_dbg_msk) >> 19) & 0x1) == FALSE )

#define HDRSRCH_IDHO_THRESH_LO_CACHED \
        ((( hdrsrch_dbg_msk >> 32) & 0xF000) >> 12 )

#define HDRSRCH_IDHO_THRESH_LO_NOT_CACHED \
        ((( hdrsrch_dbg_msk >> 32) & 0x0F00) >> 8 )

#define HDRSRCH_IDHO_THRESH_HI_CACHED \
        ((( hdrsrch_dbg_msk >> 32) & 0x00F0) >> 4 )

#define HDRSRCH_IDHO_THRESH_HI_NOT_CACHED \
        ((( hdrsrch_dbg_msk >> 32) & 0x000F) )

#endif /* HDRSRCH_1X_SEARCH_TEST */

#define HDRSRCH_NV_OFS_TC_CHAN_REPEAT_COUNT \
        ((( hdrsrch_dbg_msk >> 32) & (0xF000 << 16)) >> 28 )

#define HDRSRCH_NV_FORCE_OPMODE_CONFIGURATION \
        ((( hdrsrch_dbg_msk >> 48) & 0x0300 ) >> 8 )
  /* number of iterations for TC OFS */

#define HDRSRCH_ACCESS_HYBRID_NV_ENABLE \
        ((( ((uint32) hdrsrch_dbg_msk) >> 7) & 0x1) == FALSE )
  /* Bit 7 from HDR L1 debug mask to indicate the type of access */

#define HDRSRCH_INTERPOLATED_SRCH_NV_ENABLE \
        ((( ((uint32) hdrsrch_dbg_msk) >> 4) & 0x1) == 0x0 )
  /* Set bit 4 from HDR L1 debug mask to indicate NOT using interpolated 
   * search results */

#define HDRSRCH_ADAPTIVE_DIVERSITY_NV_ENABLE \
        ((( ((uint32) hdrsrch_dbg_msk) >> 5 ) & 0x1) == 0x0 )
  /* Bit 5 from HDR L1 debug mask to indicate 
     whether to enable the adaptive diversity algorithm */

#ifdef FEATURE_HDR_REVB
#define HDRSRCH_GHOST_CHAN_OFFSET \
        ( ( ( ( uint32 ) hdrsrch_dbg_msk) >> 8 ) & 0xFF ) 
  /* Bit 8 to Bit 15 to indicate the channel number offset of the ghost
     channel. */

#define HDRSRCH_DEMOD_ASSIGN_OVERRIDE_ENABLE \
        ( ( ( ( uint32 ) hdrsrch_dbg_msk) >> 20 ) & 0x1 ) 
  /* If not zero, the first demod index assignment is overriden by 
     HDRSRCH_DEMOD_ASSIGN_OVERRIDE_INDEX  */

#define HDRSRCH_DEMOD_ASSIGN_OVERRIDE_INDEX \
        ( ( ( ( ( uint32 ) hdrsrch_dbg_msk) >> 21 ) & 0x3 ) % 0x3 )
  /* The first overriding demod index assignment. The value will be 
     mod 3 to make the value 0b11 valid */
#endif /* FEATURE_HDR_REVB */

#ifdef FEATURE_HDR_FL_DATA_ADAPTIVE_RXD
#define HDRSRCH_FL_DATA_ADAPTIVE_RXD_NV_ENABLE \
        ((( ((uint32) hdrsrch_dbg_msk) >> 23 ) & 0x1) == 0x0 )
  /* Bit 23 from HDR L1 debug mask to indicate 
     whether to enable the FL data adaptive diversity algorithm */
#endif /* FEATURE_HDR_FL_DATA_ADAPTIVE_RXD */

#define HDRSRCH_MIN_TIME_FOR_ACQ_SYNC_REVERSE_TIME_TRAN_MS    2000
  /* HDR srch needs at least 2000ms to finish brute-force acq, sync and pilot 
     measurement */

#define HDRSRCH_MIN_TIME_FOR_REVERSE_TIME_TRAN_MS   3 
  /* HDR srch needs at least 3ms to do pilot measurement search */

/*------------------------------------------------------------------------
  HDR Search Data Structure
------------------------------------------------------------------------*/

/* Converts a number into a natural number ( greater or equal to zero ). */
#define HDRSRCH_NAT_NUM( num ) ( num < 0 ? 0 : num )

/* Absolute value macro.*/
#define LABS_VAL( x ) ( ((x) < 0) ? -(x) : (x) )

/*------------------------------------------------------------------------
  HDR Search EFS NV Item related defines
------------------------------------------------------------------------*/
#define HDRSRCH_EFS_NV_CONF_FILE_PATH  "/nv/item_files/conf/hdrsrch_config_info.conf"

#define HDRSRCH_EFS_NV_IDLE_DIV_PSINR_THRESH "/nv/item_files/modem/hdr/srch/hdr_idle_div_psinr_thresh"
#define HDRSRCH_EFS_NV_IDLE_DIV_SUBPKT_THRESH "/nv/item_files/modem/hdr/srch/hdr_idle_div_subpkt_thresh"
#define HDRSRCH_EFS_NV_IDLE_DIV_SCI_THRESH "/nv/item_files/modem/hdr/srch/hdr_idle_div_sci_thresh"

#define HDRSRCH_EFS_NV_ACQ_DIV_ENABLE  "/nv/item_files/modem/hdr/srch/hdr_acq_div_enable"
#define HDRSRCH_EFS_NV_ACQ_DIV_THRESH0 "/nv/item_files/modem/hdr/srch/hdr_acq_div_thresh0"
#define HDRSRCH_EFS_NV_ACQ_DIV_THRESH1 "/nv/item_files/modem/hdr/srch/hdr_acq_div_thresh1"

/*------------------------------------------------------------------------
  HDR Search Debug Array
------------------------------------------------------------------------*/

/* Enum values supported for HDRSRCHACQ_BINNING_MODE_IDX field in
   hdrsrch_dbg_array[] */
enum
{
  HDRSRCH_DBG_FROM_TCXOMGR_BIN_MODE,
    /* Keep binning mode determined from tcxomgr */

  HDRSRCH_DBG_RGS_BIN_MODE,
    /* Override to use RGS binning mode */

  HDRSRCH_DBG_FT1_BIN_MODE,
  /* Override to use FT1 binning mode */

  HDRSRCH_DBG_FT2_BIN_MODE
  /* Override to use FT2 binning mode */
};

/* NV Item indices into hdrsrch_dbg_array[]
   Note: Only valid if hdrsrch_dbg_array[HDRSRCHACQ_NV_ENABLED_IDX] is
         enabled */
enum
{
  HDRSRCHACQ_NV_ENABLED_IDX, /* 0 */
    /* Tunable parameters enabled index */

  HDRSRCHACQ_NUM_ITERATION_IDX,
    /* Number of full initial acquisition loops to attempt index */

  /* Start of predetect tunable parameters */

  HDRSRCHACQ_PREDETECT_COH_INT_LEN_IDX,
    /* Predetect coherent integration length index */

  HDRSRCHACQ_PREDETECT_NON_COH_INT_LEN_IDX,
    /* Predetect non-coherent integration length index */

  HDRSRCHACQ_PREDETECT_NUM_PILOT_BURSTS_IDX,
    /* Predetect number of pilot bursts index */

  HDRSRCHACQ_PREDETECT_SRCH_WIN_SIZE_IDX, /* 5 */
    /* Predetect search window size (unit is 256 in dbg array)
       Supported: 0x08=2048, 0x10=4096, 0x20=8192 */

  HDRSRCHACQ_PREDETECT_NUM_DETECT_PEAKS_IDX,
    /* Predetect number of detect peaks passed to detect stage index
       (unit is 16) */

  /* Start of detect tunable parameters */

  HDRSRCHACQ_DETECT_COH_INT_LEN_IDX,
    /* Detect coherent integration length index */

  HDRSRCHACQ_DETECT_NON_COH_INT_LEN_IDX,
    /* Detect non-coherent integration length index */

  HDRSRCHACQ_DETECT_NUM_PILOT_BURSTS_IDX,
    /* Detect number of pilot bursts index */

  HDRSRCHACQ_DETECT_SRCH_WIN_SIZE_IDX, /* 10 */
    /* Detect search window size index */

  HDRSRCHACQ_DETECT_PATH_THRESH_IDX,
    /* Detect path threshold index */

  HDRSRCHACQ_BINNING_MODE_IDX,
    /* Used for selecting binning for XO:
       0 - Default to current binning from tcxomgr
       1 - RGS mode
       2 - FT1 mode
       3 - FT2 mode
     */

  HDRSRCHACQ_DETECT_NUM_ANTI_ALIAS_PEAKS_IDX,
    /* Detect number of peaks passed to anti-alias index */

  HDRSRCHACQ_DETECT_NUM_PULLIN_FINGS_IDX,
    /* Detect number of fingers assigned before pullin index */

  /* Start of afc tunable parameters */

  HDRSRCHAFC_OPEN_LOOP_ERR_HI_IDX, /* 15 */
    /* Open Loop frequency error injection (PPB Hi) index
       for XO and VCTCXO  */

  HDRSRCHAFC_OPEN_LOOP_ERR_LO_IDX,
    /* Initial XO calibration correction frequency (PPB Lo) index
       for XO and VCTCXO  */

  HDRSRCHAFC_CLOSED_LOOP_ERR_HI_IDX,
    /* Closed Loop frequency error injection (PPB Hi) index
       for XO and VCTCXO */

  HDRSRCHAFC_CLOSED_LOOP_ERR_LO_IDX,
    /* Closed Loop frequency error injection (PPB Lo) index
       for XO and VCTCXO */

  /* Start of misc tunable parameters */

  HDRSRCHACQ_PULLIN_INIT_RSSI_IDX,
    /* Pull-in initial RSSI index */

  /* Start of MDSP tunable paramters */
  HDRSRCHACQ_MDSP_FT_GAIN_IDX, /* 20 */
    /* MDSP FT gain index */

  /* More General tunable parameters */
  HDRSRCHACQ_SRCH_INT_IDX,
    /* 0 = Enable searcher interpolation,
       1 = Disable using searcher interpolation */

  HDRSRCHAFC_EQ_INT_IDX,
    /* 0 = Enable the equalizer interpolation
       otherwise = Disable the equalizer interpolation */

  HDRSRCHAFC_XO_PLL_IDX,
    /* 0= Enable XO PLL 
       otherwise = Disable XO PLL */

  HDRSRCHAFC_THERMAL_AFC_IDX,
    /* 0= Enable thermal AFC
       otherwise = Disable thermal AFC */

  HDRSRCHACQ_MMSE_CFG_IDX /* 25 */
    /* Bit 0: Enable/Disable(default) FW MMSE detection
       Bit 1: ARQ setting (reserved) */
};

/* Size of HDR SRCH debug array for NV items */
#define HDRSRCH_DBG_ARRAY_SIZE 64

#define HDRSRCH_DBG_OPEN_LOOP_FREQ_ERR \
  ( hdrsrch_dbg_array[HDRSRCHACQ_NV_ENABLED_IDX] == \
         HDRSRCH_DBG_ARRAY_ACQ_ENABLED ) ? \
  ( (hdrsrch_dbg_array[HDRSRCHAFC_OPEN_LOOP_ERR_HI_IDX] << 8) + \
    hdrsrch_dbg_array[HDRSRCHAFC_OPEN_LOOP_ERR_LO_IDX] ) : 0
  /* Open loop freq error to inject for debug (using shortcut if-then-else).
     0 if hdrsrch_dbg_array is not for init acq*/

#define HDRSRCH_DBG_CLOSED_LOOP_FREQ_ERR \
  ( hdrsrch_dbg_array[HDRSRCHACQ_NV_ENABLED_IDX] == \
         HDRSRCH_DBG_ARRAY_ACQ_ENABLED ) ? \
  ( (hdrsrch_dbg_array[HDRSRCHAFC_CLOSED_LOOP_ERR_HI_IDX] << 8) + \
    hdrsrch_dbg_array[HDRSRCHAFC_CLOSED_LOOP_ERR_LO_IDX] ) : 0
  /* Closed loop freq error to inject for debug (using shortcut if-then-else).
     0 if hdrsrch_dbg_array is not for init acq*/

#define HDRSRCH_DBG_NV_XO_PLL_ENABLE \
  ( hdrsrch_dbg_array[HDRSRCHAFC_XO_PLL_IDX] == 0 )
  /* Macro to indicate if XO PLL is enabled */

#define HDRSRCH_DBG_NV_EQ_INT_ENABLE \
  ( hdrsrch_dbg_array[HDRSRCHAFC_EQ_INT_IDX] == 0 )
  /* Macro to indicate if EQ interpolation is enabled */

#define HDRSRCH_DBG_NV_THERM_AFC_ENABLE \
  ( hdrsrch_dbg_array[HDRSRCHAFC_THERMAL_AFC_IDX] == 0 )
  /* Macro to indicate if thermal AFC is enabled */
#if 0

#define HDRSRCH_DBG_CLR_INIT_FREQ_ERR( ) \
  hdrsrch_dbg_array[HDRSRCHACQ_INIT_FREQ_ERROR_HI_IDX] = 0; \
  hdrsrch_dbg_array[HDRSRCHACQ_INIT_FREQ_ERROR_LO_IDX] = 0;
  /* Clear initial freq error */

#define HDRSRCH_DBG_CLR_PULLIN_FREQ_ERR( ) \
  hdrsrch_dbg_array[HDRSRCHACQ_PULLIN_FREQ_ERROR_HI_IDX] = 0; \
  hdrsrch_dbg_array[HDRSRCHACQ_PULLIN_FREQ_ERROR_LO_IDX] = 0;
  /* Clear pullin freq error */

#endif

#define HDRSRCH_DBG_NV_MMSE_RAB_M     0x1
#define HDRSRCH_DBG_NV_MMSE_RAB_S     0

#define HDRSRCH_DBG_NV_MMSE_ARQ_M     0x2
#define HDRSRCH_DBG_NV_MMSE_ARQ_S     1

#define HDRSRCH_DBG_NV_MMSE_RAB       \
  ( ( ( hdrsrch_dbg_array[HDRSRCHACQ_MMSE_CFG_IDX] & \
        HDRSRCH_DBG_NV_MMSE_RAB_M ) >>  \
      HDRSRCH_DBG_NV_MMSE_RAB_S ) == 0 )

#define HDRSRCH_DBG_NV_MMSE_ARQ       \
  ( ( ( hdrsrch_dbg_array[HDRSRCHACQ_MMSE_CFG_IDX] & \
        HDRSRCH_DBG_NV_MMSE_ARQ_M ) >>  \
      HDRSRCH_DBG_NV_MMSE_ARQ_S ) == 0 )

/*==========================================================================

                        EXTERNAL STRUCTURES, VARIABLES

This section contains the declaration of the external variables, structs
which are used by other files of HDRSRCH module

==========================================================================*/

typedef enum
{
  HDRSRCH_HDR_ONLY_MODE,
  HDRSRCH_DUAL_MODE,
  HDRSRCH_HYBRID_MODE
} 
hdrsrch_mode_enum_type;

/*------------------------------------------------------------------------
  HDR Search Data Structure
------------------------------------------------------------------------*/

typedef struct
{
  uint16                             overall_time_allowed_ms;
    /* Maximum time allowed by LTE to do brute-force acq, sync and measurement */

  boolean                            is_brute_force_acq;
    /* Whether it is a brute-force acq */

  uint32                             time_for_acq_ms;
    /* time used for brute-force acq in unit of ms */

}
hdrsrch_brute_force_acq_params_type;

typedef struct
{
  /*-------------------------------------------
     Parameters common to all search modules
  -------------------------------------------*/

  hdrsrch_search_params_struct_type     srch_params;
    /* Search Parameters (Pilot Inc, Win Sizes) */

  hdrsrch_chan_mgmt_params_struct_type  same_chan_mgmt_params;
    /* ASet channel pilot management parameters */

  hdrsrch_chan_mgmt_params_struct_type  diff_chan_mgmt_params;
    /* Non-Aset channel pilot management parameters */

  hdrsrch_chan_mgmt_1x_params_type      chan_mgmt_1x_params;
    /* Non-Aset channel pilot management parameters */

  hdrsrch_tc_mode_enum_type             tc_mode;
    /* The specific HDR traffic mode selected. */

  uint8                                 frame_offset;
    /* Frame offset */

  /*-------------------------------------------
     Callbacks common for all states
  -------------------------------------------*/

  hdrsrch_system_lost_cb_type           system_lost;
    /* Callback to indicate that the SYSTEM has been LOST */

  hdrsrch_suspend_cb_type               suspend_cb;
    /* Callback to indicate lost/regain of RF resources */

  hdrsrch_system_usable_cb_type         system_usable_cb;
    /* Callback to indicate that the System is usable/unusable */

  hdrsrch_deactivate_done_cb_type       deactivate_done_cb;
    /* Callback to indicate that HDRSRCH is completely deactivated */

#ifdef FEATURE_HDR_REVB
  hdrsrch_schdgrp_rpt_cb_type           schdgrp_rpt_cb;
    /* Scheduler group report call back */
#endif /* FEATURE_HDR_REVB */

  /*-------------------------------------------
     Timers common for all states
  -------------------------------------------*/

  rex_timer_type                        dog_timer;
    /* Watchdog timer */

  timer_type                            tune_timer;
    /* Timer used for RF tuning */

  rex_timer_type                        schedule_timer;
    /* Timer used for search scheduling */

  rex_timer_type                        lostdump_timer;
    /* Timer used to signal that a search request has been lost */

  rex_timer_type                        timeout_timer;
    /* Timer used for timeouts */

  rex_timer_type                        gp_timer0;
    /* General Purpose Timer 0 */

  rex_timer_type                        gp_timer1;
    /* General Purpose Timer 1 */

#ifdef FEATURE_HDRSRCH_TIMED_WAIT
  rex_timer_type                        gp_timer2;
    /* General Purpose Timer 2 */
#endif /* FEATURE_HDRSRCH_TIMED_WAIT */

  /*-------------------------------------------
     Channel data common to all search states
  -------------------------------------------*/
#ifndef FEATURE_HDR_REVB
  sys_channel_type                      aset_channel;
    /* Channel the active set belongs to */
#else
  uint8                                 aset_channel_cnt;
    /* ASET channel count up to 3 */

  sys_channel_type                      aset_channel[HDRSRCH_MAX_NUM_CARRIERS];
    /* Channel the active set belongs to */

  hdrsrch_demod_idx_type                aset_demod[HDRSRCH_MAX_NUM_CARRIERS];
    /* ASET demod index assignment list matching to the aset_channel list */
#endif /* FEATURE_HDR_REVB */

  /*-------------------------------------------
     Command being executed
  -------------------------------------------*/

  hdrsrch_cmd_type                    * cmd;
    /* Command currently being executed */
  
  timetick_type                         ho_time;
    /* When idle handoff occured */

  hdrsrch_tc_handdown_mode_enum_type    handdown_mode;
    /* Handdown mode during HDR traffic connection */

  uint8       div_ctrl;
    /* User diversity control read from NV */

  uint32      div_band_mask;
    /* HDR diversity enable band class mask read from NV */

  hdrsrch_brute_force_acq_params_type   brute_force_params;
    /* Variables shared by all states for brute-force acq,sync and meas */

  boolean     lre_enabled;
    /* Flag to indicate LRE enabled/disabled status */

#ifdef FEATURE_HDR_THERMAL_BACKOFF

  int8 div_ctrl_pref_vote;
    /* Diversity vote from runtime user and thermal backoff
     bit 0 - thermal div ctrl pref vote
     bit 1 - runtime user pref vote
     If the bit is set then user wants diversity disabled.
     bit 2 - second chain test call enabled. */

#endif /* FEATURE_HDR_FL_FLOW_CONTROL */

  hdrsrch_winsize_enum_type       saved_aset_win_size;
    /* Active Set window search size */

#if defined FEATURE_HDR_DYNAMIC_IDLE_DIV || defined FEATURE_HDR_ACCESS_DIV
  hdrsrch_idle_state_div_type      idle_state_div;
    /* Dynamic idle state diversity parameters */
#endif /* FEATURE_HDR_DYNAMIC_IDLE_DIV */

#ifdef FEATURE_HDR_DYNAMIC_ACQ_DIV
  hdrsrch_acq_div_type             acq_div;
    /* Acquisiton diversity parameters */
#endif /* FEATURE_HDR_DYNAMIC_ACQ_DIV */

#ifdef FEATURE_HDR_MODEM_COEXISTENCE_SW
  timer_type                       coex_timer;
    /* Timer for reporting rx tx power to MCS periodically */
#endif /* FEATURE_HDR_MODEM_COEXISTENCE_SW */

#ifdef FEATURE_HDR_BAND_AVOIDANCE
  timer_type                              band_avoid_timer;
    /* Timer for reporting rx tx power to MCS periodically */
#endif /* FEATURE_HDR_BAND_AVOIDANCE  */

  /*-------------------------------------------
     Critical section
  -------------------------------------------*/

  rex_crit_sect_type              crit_sect; 
    /* Critical Section */ 

  uint8                            rssi;
    /* RSSI updated every search dump */

  int16                            max_ecio;
    /* Max ecio */     
}
hdrsrch_struct_type;


typedef struct
{
  int32                           accum;
    /* IIR filter internal memory                                 */

  uint16                          num_samples;
    /* num of filter input samples so far                         */

  int16                           filtered_eng;
    /* Filtered output value                                      */
      
} hdrsrch_opmode_filter_struct_type;

typedef struct
{
  int16                           poor_coverage_ecio_lo;
    /* Lower Ec/Io Threshold to detect poor coverage              */

  int16                           poor_coverage_ecio_hi;
    /* Higher Ec/Io Threshold to detect poor coverage             */
    
  int16                           poor_coverage_ec_lo;
    /* Lower Ec Threshold to detect poor coverage                 */

  int16                           poor_coverage_ec_hi;
    /* Higher Ec Threshold to detect poor coverage                */
        
  int16                           good_coverage_ecio_hi;        
    /* Higher Ec/Io Threshold to detect good coverage             */
      
} hdrsrch_opmode_threshold_struct_type;


typedef struct
{
  boolean                         active;
    /* whether timer is active or not                             */

  uint32                          elapsed;
    /* the elapsed time since timer started running               */

  uint32                          duration;
    /* the duration timer is set for                              */
      
} hdrsrch_opmode_timer_struct_type;

typedef struct
{
  /*-------------------------------------------
     Parameters common to all search modules
  -------------------------------------------*/

  boolean                               active;
    /* whether the handdown algorithm is active or not */
    
  hdrsrch_opmode_config_enum_type       config;
    /* the current AT operational mode configuration */

  hdrsrch_mode_enum_type                current_mode;
    /* the current AT operational mode */

  boolean                               first_run;
    /* whether this is the first run after power up or not */
    
  timetick_type                         run_time;
    /* time at which opmode algorithm executed in sysclks */

  timetick_type                         log_time;
    /* time at which opmode algorithm executed in sysclks */
    
  timetick_type                         indication_time;
    /* time at which last indication was sent to upper layers in sysclks */
    
  int16                                 hdr_ecio;
    /* filtered energy for HDR serving sector ecio */

  int16                                 hdr_ec;
    /* filtered energy for HDR serving sector ec */
    
  hdrsrch_opmode_filter_struct_type     ecio_filter;
    /* filter for serving sector EcIo */
    
  hdrsrch_opmode_filter_struct_type     ec_filter;
    /* filter for serving sector Ec */

  hdrsrch_opmode_threshold_struct_type  threshold;
    /* structure to keep algorithm thresholds */
        
  uint16                                dual_mode_timer_scale;
    /* scale value for dual mode timer */
      
  uint16                                hdr_only_mode_timer_scale;
    /* scale value for hdr only mode timer */
    
  hdrsrch_opmode_timer_struct_type      ecio_timer;
    /* timer for Ec/Io condition to enable dual mode */

  hdrsrch_opmode_timer_struct_type      ec_timer;
    /* timer for Ec/Io condition to enable dual mode */

  hdrsrch_opmode_timer_struct_type      ecio_timer2;
    /* timer for Ec/Io condition to disable dual mode
       i.e enable HDR only mode */

  hdrsrch_opmode_indication_cb_type     poor_coverage_cb;
    /* call back function to indicate poor DO coverage 
       to enable DUAL/HYBRID mode in border cell */
    
  hdrsrch_opmode_indication_cb_type     good_coverage_cb;
    /* call back function to indicate good DO coverage 
       to go back to HDR ONLY mode in border cell */  
}hdrsrch_opmode_struct_type;

/* hdrsrch_dbg_array[0] type used to overload the use of hdrsrch_dbg_array[] */
typedef enum
{
  HDRSRCH_DBG_ARRAY_DISABLED = 0,
    /* Debug array is disabled */

  HDRSRCH_DBG_ARRAY_ACQ_ENABLED = 1
    /* Enable for hdrsrchacq */
}
hdrsrch_dbg_array_0_type;



/*------------------------------------------------------------------------
  External structure declaration.
------------------------------------------------------------------------*/

extern hdrsrch_struct_type  hdrsrch;
  /* HDRSRCH Global Variables */

extern hdrsrch_opmode_struct_type  hdrsrchmode;
  /* HDRSRCH Global Variables */

extern uint64               hdrsrch_dbg_msk;
  /* Data storage for HDR SRCH debug mask item read from NV */

extern uint8                hdrsrch_dbg_array[HDRSRCH_DBG_ARRAY_SIZE];
  /* Data storage for HDR SRCH debug array item read from NV */

/*==========================================================================

                    FUNCTION PROTOTYPES USED IN THE MODULE

This section contains function prototypes for the functions used in this
module.

==========================================================================*/

#endif /* HDRSRCHI_H */

