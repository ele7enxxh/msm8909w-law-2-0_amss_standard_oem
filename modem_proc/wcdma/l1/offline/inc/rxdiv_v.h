#ifndef RXDIV_V_H
#define RXDIV_V_H

/*============================================================================
                                  R X D I V _ V . H
DESCRIPTION

Copyright (c) 2006 - 2015 Qualcomm Technologies, Inc.  All Rights Reserved.
QUALCOMM Proprietary.  Export of this technology or software is regulated
by the U.S. Government. Diversion contrary to U.S. law prohibited.
============================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/MSM5200/l1/vcs/rxdiv.h_v   $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/wcdma/l1/offline/inc/rxdiv_v.h#1 $
$DateTime: 2016/12/13 08:00:12 $
$Author: mplcsds1 $

when        who     what, where, why
--------    ---     --------------------------------------------------------
02/24/15    ap      Initial check-in for WL1 QSH framework 
16/02/16    svh     Change request time for diversity in idle mode.
07/06/15    svh     New API to check diversity band support.
01/23/15    qj      Fix FET pre procssing operation prior to Rxd transition
01/12/14    sm      Add debug array to keep track of RxD reason bmsk
12/22/14    kr      do combined wakeup for dsds
12/10/14    gm      Mods to reduce min req time to 28ms in non conn states.
12/01/14    br	    Featurised third carrier for KW errors.
11/25/14    rsr     Added changes to ensure that Qual trigger is modified 
                    in task context by OLPC.
11/24/14    rsr     Added Fix to Ensure that on RxD FOFF, we check with FMO
                    state machine, prior to turning on, in case of immediate
                    grant.
11/05/14    rsr     Fixed Compiler Warning.
11/05/14    rsr     Added Fix to bring RF out of LPM prior to Sending AGC cmd 
                    for RF Turn on in CPC-DRX.
10/27/14    ac      Moved wl1_rxd_opts_nv_struct_type definition to WNV
10/24/14    qj      Added API to check if rxd in full diveristy mode in PCH_SLEEP
10/22/14    sm      Remove break in context for tune done when transitioning ON
10/03/14    rsr     Added Changes to ensure that RxD avoids collision with FMO
                    in FACH State.
09/09/14    rsr     Added Changes to wait for AGC CFG done after calling RFM 
                    sleep.
09/03/14    sm      RxD fix to prevent infiniate loop during FOFF to FOFF transition
08/22/14    rsr     Added changes to ensure that if we are denied grant, RxD 
                    enters FOFF state.
08/20/14    sd      Fixed DSDS Medium and Low compiler warnings in WCDMA.
08/12/14    sd      Fixed DSDS Medium and Low compiler warnings in WCDMA.
07/29/14    sm      Fix KW for RxD callback array boundry
07/28/14    sm      Remove additional local commands for unlock and grant
07/18/14    sad     RF api changes and inter freq tune changes
07/15/14    ac      NV reorganization
07/08/14    rsr     Added Fix to Ensure that RxD Is Forced Off Prior to Entering 
                    EDRX Full Sleep.
07/07/14    sm      Request and release diversity rxlm resources in ds mode 
                    along with primary 
07/02/14    sm      Fix compilation due to featurization on TH 1.0
06/23/14    rsr     Added Debug changes to Track State Update Local Commands.
06/19/14    sm      RxD changes to use WRM apis for TRM interaction
06/17/14    sm      RxD changes for fast teardown
05/28/14    sm      API to support wait for agc tune done when RxD transitions on
05/29/14    sks     Combined RF wakeup and tune optimizations on BO.2.0
05/21/14    sm      API to query if the FOFF trigger is set
05/16/14    qj      API to return if UE is in FULL DIVERSITY mode
05/16/14    rsr     Added RxDPM/ARD Multi-Carrier Support.
05/16/14    sm      Move setting foff callback for OMRD to task context
05/14/14    qj      Change FOFF client to uint16
05/12/14    qj      Add change to support multi-carr for 0x430c
05/12/14    qj      Change to not release diveristy chain in div only mode
05/12/14    qj      Disable ant sharing feature through NV 
05/07/14    sm      Changes for CQI adjustement to improve performance of OMRD
04/28/14    sm      FOFF client for Frequency Scan
04/01/14    sm      OMRD changes to make RxD trm compliant
04/01/14    qj      Change for RxD in ACQ/BCH
03/13/14    ac      Deallocate RxLM buffers for diversity after AGC_CFG_DONE
02/24/14    rsr     Added Fix for PS 0/0 case.
02/20/14    qj      NV support for ant sharing
02/14/14    qj      Changes for W diversity antenna sharing during PCH_SLEEP
03/27/14    sm      OMRD changes to make RxD trm compliant
03/03/14    rsr     Added Capability to account for Multi-Carrier SCCH Stats.
01/15/14    qj      Fixed rxd_proc_client_pending_bmsk set and clear issue
01/20/13    qj      Added Fix to take care of lock mode in discontinuous reception
01/08/13    sm      Update RxD Idle algorithm to use RSCP based criteria & new hysteresis timer
                    Added a max api for RxD
01/02/13    qj      Added static trig at clear hs trig/state rsn 
11/20/13    rsr     Added Fix to take care of Race condition where SRCH cb 
                    to RxD fires in task context and Tri cb fires in ISR 
                    context 
09/30/13    sm      RxD local command support 
08/23/13    sm      RxD support for DSDS  
07/17/15    sm      CPC-DRX with RxD and proc mode redesign support added
10/01/13    sm      Check for RxD support before requesting DIV antenna
10/11/13    rsr     0x430C Log packet Update to support filter ready indicators.
10/01/13    sm      Enable NV read when coming Online in FTM Mode
09/29/13    as      Continuing port of Dual Sim from Triton to Dime.
09/23/13    as      Porting Dual Sim from Triton to Dime.
08/09/13    rsr     DBDC FR Changes
07/23/13    vs      Added support for antenna switch diversity feature. 
07/11/13    sm      Added RxD opts NV support
06/19/13    sm      Updated API to enable and Disable RxD to use FOFF and 
                    allow FON in IDLE mode using QXDM commands 
05/23/13    geg     WSW decoupling (except for component of QCHAT)
04/09/13    vs      added support for wcdma periodic statictics log pkt.
03/14/13    sj      Corrected calculation of TPR for capacity based RDDS
02/11/12    sj      Added sliding window for scch scheduling evaluation
                    and changed the threshold for RxDPM v2
09/14/12    sj      Enable RxD in HS + R99 voice MRAB calls
09/14/12    sj      Enabled RxD for FACH signalling by default
08/19/12    sj      Added RxD support for HS-FACH with HS_RACH Active
07/24/12    geg     Upmerge WCDMA.MPSS.1.0 to WCDMA.MPSS.2.0
05/02/12    sj      Added ARD v2 params K and L configurable through NVs
04/04/12    sj      Added RxDPM/ARD v2 support
04/04/12    sj      RxD NV items to use a single EFS cfg file
03/23/12    dp      CM act/deact notification interface unification
03/15/12    sj      Klocwork error fix
03/14/12    gsk     Modified for RxD Idle feature and CR fixes.
03/06/12    vsr     New RxD CTRL log packet (0x430D)
02/03/12    vsr     RxDPM threshold decreased to 20%
01/24/12    vsr     ARD updates
01/27/12    vs      Nikel feature cleanup.
01/20/12    zr      Mainlined FEATURE_WCDMA_GENESIS_SW, FEATURE_RF 
01/19/12    vsr     Externed ARD functions
01/18/12    vsr     ARD framework Fixes & run-length logic
01/17/12    vsr     Added ARD Algorithm Support
01/10/12    sj      Added support for RxD in FACH signaling
12/27/11    mc      Added support for DSR and RxD/CM transitions
12/16/11    stk     Setting the threshold to 10 dB, so the EcIo imbalance 
                    check satisfies for > 5db difference.
12/13/11    vsr     Updated BC11 and BC19 band info
12/01/11    rc      Added support for FEATURE_WCDMA_RXD_PCH.
11/15/11    vsr     Changes for RxDPM
09/06/11    vsr     Changes for Div-Only mode
04/28/11    sj      upmerge from offline/main
04/24/11    geg     Created the file W SW decoupling
04/20/11    vsr     Added support to call RxD APIs through QXDM command 
04/06/11    vsr     Added SCCH decode stats based RxD trigger
03/30/11    vsr     Disable RxD for data calls if SF > 16
===========================================================================*/

#include "wcdma_variation.h"
#include "comdef.h"
#include "wl1trm.h"
#include "rxdiv.h"
#include "l1sapcommon.h"

#ifdef FEATURE_MDSP_MULTI_RF_CHAIN_INTERFACE
#include "rfi.h"

#include "rfm.h"

#include "log_codes.h"
#include "log.h"
#include "log_codes_wcdma_int.h"

#ifdef FEATURE_WPLT
#include "wplt.h"
#endif /* FEATURE_WPLT */

#include "fs_fcntl.h"
#include "fs_public.h"
#include "fs_sys_types.h"
#include "fs_lib.h"

#include "wfw_qice_swi.h"
#include "l1def.h"

/* ==========================================================================
** Includes and Public Data Declarations
** ========================================================================*/

/* -----------------------------------------------------------------------
** Include Files
** ----------------------------------------------------------------------- */

/* -----------------------------------------------------------------------
** Constant / Define Declarations
** ----------------------------------------------------------------------- */
/* Identifiers for each of the hardware antennas, these are useful in naming
    of these antennas. Most importantly they are used for identifying the antenna
    for sending the commands to mDSP for various commands, e.g. start/stop AGC */
#ifdef FEATURE_MULTI_RF_CHAIN_API
#define RFCOM_PATH_0 RF_PATH_0
#define RFCOM_PATH_1 RF_PATH_1
#else
#define RFCOM_PATH_0 0
#define RFCOM_PATH_1 1
#endif

#ifdef FEATURE_WCDMA_RXD_DEBUG_CHAIN_STUCK
#error code not present
#endif 

#define RXD_UNUSED_VAL 0xFF

/* Number of WCDMA bands to be checked for potential RxD support */
#define RXD_NUM_RF_WCDMA_BANDS 9


/* NV bit masks for various RxD functionalities are listed here. The NV allows two kinds 
   control:
   1) Broadly enable Forced or Dynamic RxD for different call types
   2) Configures individual algorithms under the Dynamic RxD option
   
   +-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+
     NV    0      1     2     3     4     5     6     7     8     9     10   11    12    13    14    15    16 
   +-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+
    VAL  |DIV  |CFG  | CFG | CFG | FON | FON | FON | FON | FON | PCH |FACH |FOFF | QUAL|RSCP |CAP  |RXDPM|ARD  |
         |ANT0 |VOICE| PS  | HS  |VOICE| PS  | HS  | SGNL| DCH | CTRL|ACCS |CTRL | CTRL|CTRL |CTRL |CTRL |CTRL |
   +-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+

   */

/* NV mask for neither of antenna assignment or RxD operation to be enforced through NV */
#define RXD_NV_ZERO 0x00

/* NV item mask specifiying which antenna to be used as the primary antenna
    Path 0 to be used as diversity path when TRUE and path 1 as diversity when set to FALSE */
#define RXD_NV_DIV_ANT0 0x01
#define RXD_NV_DEFAULT_DIV_ANT0 RXD_NV_ZERO

/* NV item mask for the config mask to be set for voice calls or not */
#define RXD_NV_CFG_VOICE 0x02
#define RXD_NV_DEFAULT_CFG_VOICE RXD_NV_CFG_VOICE

/* NV item mask for the config mask to be set for voice calls or not */
#define RXD_NV_CFG_PS 0x04
#define RXD_NV_DEFAULT_CFG_PS RXD_NV_CFG_PS

/* NV item mask for the config mask to be set for HS calls or not */
#define RXD_NV_CFG_HS 0x08
#define RXD_NV_DEFAULT_CFG_HS RXD_NV_CFG_HS

/* NV item mask for forced ON RxD operation during a voice call */
#define RXD_NV_FON_VOICE 0x10
#define RXD_NV_DEFAULT_FON_VOICE RXD_NV_ZERO

/* NV item mask for forced ON RxD operation during a release 99 PS data call */
#define RXD_NV_FON_PS 0x20
#define RXD_NV_DEFAULT_FON_PS RXD_NV_ZERO

/* NV item mask for forced ON RxD operation during a HS call */
#define RXD_NV_FON_HS 0x40
#define RXD_NV_DEFAULT_FON_HS RXD_NV_ZERO

/* NV item mask for Quality metric based RxD operation for signalling traffic in DCH */
#define RXD_NV_CFG_DCH_SIGNL 0x80
#define RXD_NV_DEFAULT_CFG_DCH_SIGNL RXD_NV_CFG_DCH_SIGNL

/* NV item mask for forced ON RxD operation while L1 is in DCH state */
#define RXD_NV_FON_DCH 0x100
#define RXD_NV_DEFAULT_FON_DCH RXD_NV_ZERO

/* NV item mask for RxD operation while L1 is in PCH_SLEEP state */
#define RXD_NV_PCH_CTRL 0x200
#define RXD_NV_DEFAULT_PCH_CTRL RXD_NV_ZERO

/* NV item mask for forced ON RxD operation while L1 is in FACH state */
#define RXD_NV_FON_FACH_ACC 0x400
#define RXD_NV_DEFAULT_FON_FACH_ACC RXD_NV_FON_FACH_ACC

/* NV item mask for FOFF control */
#define RXD_NV_FOFF_CTRL 0x800
#define RXD_NV_DEFAULT_FOFF_CTRL RXD_NV_ZERO

/* NV item mask for QUAL control (RDDS) */
#define RXD_NV_QUAL_CTRL 0x1000
#define RXD_NV_DEFAULT_QUAL_CTRL RXD_NV_ZERO

/* NV item mask for RSCP control (RDDS) */
#define RXD_NV_RSCP_CTRL 0x2000
#define RXD_NV_DEFAULT_RSCP_CTRL RXD_NV_ZERO

/* NV item mask for CAP control (RDDS) */
#define RXD_NV_CAP_CTRL 0x4000
#define RXD_NV_DEFAULT_CAP_CTRL RXD_NV_ZERO

/* NV item mask for RXDPM control */
#define RXD_NV_RXDPM_CTRL 0x8000
#define RXD_NV_DEFAULT_RXDPM_CTRL RXD_NV_ZERO

/* NV item mask for ARD control */
#define RXD_NV_ARD_CTRL 0x10000
#define RXD_NV_DEFAULT_ARD_CTRL RXD_NV_ZERO

/* NV item mask for RxD in FACH (SIGNL) */
#define RXD_NV_FON_FACH_SIGNL 0x20000
#define RXD_NV_DEFAULT_FON_FACH_SIGNL RXD_NV_FON_FACH_SIGNL

/* NV item mask for RXDPM-v2 control */
#define RXD_NV_RXDPM_V2_CTRL 0x40000
#define RXD_NV_DEFAULT_RXDPM_V2_CTRL RXD_NV_ZERO

/* NV item mask for ARD-v2 control */
#define RXD_NV_ARD_V2_CTRL 0x80000
#define RXD_NV_DEFAULT_ARD_V2_CTRL RXD_NV_ZERO

/* NV item mask for RxD in FACH (HS-RACH) */
#define RXD_NV_FON_FACH_HS_RACH 0x100000
#ifdef  FEATURE_WCDMA_HS_RACH
#define RXD_NV_DEFAULT_FON_FACH_HS_RACH RXD_NV_FON_FACH_HS_RACH
#else
#define RXD_NV_DEFAULT_FON_FACH_HS_RACH RXD_NV_ZERO
#endif

/* NV item mask for RxD in HS + R99 voice Mutli-RAB */
#define RXD_NV_FON_HS_MRAB 0x200000
#define RXD_NV_DEFAULT_FON_HS_MRAB RXD_NV_FON_HS_MRAB

#define RXD_NV_ASD_CTRL 0x400000
#ifdef FEATURE_WCDMA_ANT_SWITCH_SUPPORT
/* New NV for antenna switch client */
#define RXD_NV_DEFAULT_ASD_CTRL RXD_NV_ASD_CTRL
#else
#define RXD_NV_DEFAULT_ASD_CTRL RXD_NV_ZERO 
#endif

/* NV for antenna sharing */
#define RXD_NV_ANT_SHARING 0x800000
#define RXD_NV_DEFAULT_ANT_SHARING RXD_NV_ZERO

/* NV item mask for Rxd in ACQ/BCH_FAIL*/
#define RXD_NV_FON_ACQ_BCH_FAIL 0x1000000
#define RXD_NV_DEFAULT_FON_ACQ_BCH_FAIL RXD_NV_FON_ACQ_BCH_FAIL

/* Bitmasks for various triggers, cfg masks and ops conditions */
/* -------------------------*/
/* These are the bitmasks for various triggers.The idea behind a trigger is
    a condition that can potentially alter the state of RxD controller. These triggers
    are generated by both external sources such as (CPHY_SETUP_REQ from RRC for
    HS/PS99/voice calls, a GPS request or one of the internal sources such as when
    quality or capacity constraints necessitate RxD mode of operation. In general,
    the external sources have greater priority over internal triggers. These masks
    together generate a complete trigger where individual triggers are put in an
    ordered manner of priority.*/

/* --------------------------------------*/
/* Bit masks for various triggers */

/* Triggers associated with timers come first ----------------------------*/

/* This trigger is set when RRC cfg and NV setting agree that RxD must be enabled 
     for 2sec to support signalling */
#define RXD_SIGNL_TRIGGER_BMSK 0x01

/* Internally generated trigger based on Network capacity criteria requiring RxD
    mode operation.*/
#define RXD_CAP_TRIGGER_BMSK 0x02

/* Trigger that is generated internally when quality criteria necessitate RxD mode */
#define RXD_QUAL_TRIGGER_BMSK 0x04

/* Trigger that is generated internally when RSCP criteria necessitate RxD mode */
#define RXD_RSCP_TRIGGER_BMSK 0x08

/* Trigger that is generated internally when RxDPM criteria necessitate RxD mode */
#define RXD_RXDPM_TRIGGER_BMSK 0x10

/* Trigger that is generated internally when ARD criteria necessitate RxD mode */
#define RXD_ARD_TRIGGER_BMSK 0x20

/**/
#define RXD_PS_SF_TRIGGER_BMSK 0x40

/* Triggers that dont have associated timers start here ---------------------*/

/* This trigger is set when RRC cfg and NV setting agree that RxD must be enabled 
     based on RDDS criteria */
#define RXD_CFG_VOICE_TRIGGER_BMSK 0x80

/* This trigger is set when RRC cfg and NV setting agree that RxD must be enabled 
     based on PS call criteria */
#define RXD_CFG_PS_TRIGGER_BMSK 0x100

/* This trigger is set when RRC cfg and NV setting agree that RxD must be enabled 
     based on HS call criteria (ARD and RxDPM) */
#define RXD_CFG_HS_TRIGGER_BMSK 0x200

/* This trigger almost always results in turning the RxD forcibly ON. Mostly used when
    RxD is an abolute requirement, viz HSDPA call or R99 PS data call */
#define RXD_FON_TRIGGER_BMSK 0x400

/* This trigger, when set in an ON state will always result in turning the RxD feature
     completely OFF. GPS can create such requests when the diversity chain needs to
     be relinquished and given to GPS. This flag will be cleared when the requirement
     for diversity RF chain is no longer needed, eg end of GPS session */
#define RXD_FOFF_TRIGGER_BMSK 0x800

/* Trigger that is set when some static conditions met to turn on RxD */
#define RXD_STATIC_RXD_TRIGGER_BMSK 0x1000

/* Trigger that is generated internally when RxDPM-v2 criteria necessitate RxD mode */
#define RXD_RXDPM_V2_TRIGGER_BMSK 0x2000

/* Trigger that is generated internally when ARD-v2 criteria necessitate RxD mode */
#define RXD_ARD_V2_TRIGGER_BMSK 0x4000

/* This trigger is of more fundamental nature and should be set for kicking of any
     dynamics in the RxD controller. This indicates that the sequencer is enabling the
     RxD state update event every frame tick and the state machinery keeps on ticking */
#define RXD_CFG_TRIGGER_BMSK 0x8000

#ifdef FEATURE_WCDMA_ANT_SWITCH_SUPPORT
/* New trigger bitmask for the antenna switch client */
#define RXD_ASD_TRIGGER_BMSK 0x10000
#endif

/* RxD Trigger timer values (in frames) */

#define RXD_SIGNL_TIMER_VALUE 200
#define RXD_CAP_TIMER_VALUE 8
#define RXD_QUAL_TIMER_VALUE 1000
#define RXD_RSCP_TIMER_VALUE 8
#define RXD_RXDPM_TIMER_VALUE 1
#define RXD_ARD_TIMER_VALUE 1
#define RXD_ARD_TIMER_MMT_VALUE 6
#define RXD_ARD_TIMER_DWELL_VALUE 100
#define RXD_PS_SF_TIMER_VALUE 1
#define RXD_STATIC_RXD_TIMER_VALUE 1
#define RXD_RXDPM_V2_TIMER_VALUE   1
#define RXD_ARD_V2_TIMER_VALUE     1
#define RXD_ASD_TIMER_VALUE     1
#define RXD_INFINITE_TIMER 0xFFFF

/* ARD v2 filters settling time - L */
#define RXD_FILTER_SETTLE_TIME 10
/* ARD v2 minimum dwell time with RxD off - K */
#define RXD_ARD_V2_DWELL_TIME  200

/* --------------------------------------*/
/* Bit masks for various cfg */

/* This config mask is indicative of if the DCH state is entered or not. In conjuction
     with NV item for DCH state, this is used to alter the state machine based on DCH/
     non-DCH state only. Ignored if the DCH state NV item is not enabled */
#define RXD_CFG_L1_DCH_BMSK 0x01
#define RXD_CFG_L1_DCH_SHFT 1

/* This configuration mask set by RRC indicates whether a voice call is currently in
    progress or not. The update in this mask happens through the CPHY_SETUP_REQ
    coming from the RRC for voice call */
#define RXD_CFG_VOICE_BMSK 0x02

/* This cfg mask is also set by RRC and is indicative of whether a R99 PS data call
     is active or not. This will be used differentlt with different NV items settings */
#define RXD_CFG_PSDATA_BMSK 0x04

/* This configuration mask set by RRC indicates whether a HS call is currently in
    progress or not. The update in this mask happens through the CPHY_SETUP_REQ
    coming from the RRC for HS call */
#define RXD_CFG_HS_BMSK 0x08

/* This configuration mask set by RRC indicates whether a MBMS channel is currently up
    or not. The update in this mask happens through the CPHY_SETUP_REQ
    coming from the RRC for MBMS channels */
#define RXD_CFG_MBMS_BMSK 0x10

/* This configuration mask set by RRC indicates whether a PS 0/0 call is currently up
    or not. The update in this mask happens through the CPHY_SETUP_REQ
    coming from the RRC forPS 0/0 */
#define RXD_CFG_PS_0x0_BMSK 0x20

/* This configuration mask set in L1m FACH state with data indicator from MAC is treated as signalling trigger. 
    The update in this mask happens through MAC API */
#define RXD_CFG_FACH_SIGNL_BMSK 0x40

/* This configuration mask set L1m FACH state with no call type is treated as access state trigger. 
    The update in this mask happens through the CPHY_SETUP_REQ */
#define RXD_CFG_FACH_ACC_BMSK 0x80

/* This configuration mask set in L1m FACH state with HS-RACH indicator from HS is 
    indicative of whether a data call is active or not. 
    The update in this mask happens through HS API */
#define RXD_CFG_FACH_HS_RACH_BMSK 0x100

/* This Configuration mask is set before HLSYNC to enable RxD only for
   Signalling (and not enable RDDS) 
   The update in this mask happens from l1msetup */
#define RXD_CFG_DCH_INIT_BMSK 0x200

/* This configuration mask is set in ACQ/BCH when 
   BCH demod has failed; or
   Squal calculation has resulted in failure; or
   CRC errors in SIB decoding exceeded threshold */
#define RXD_CFG_ACQ_BCH_FAIL_BMSK 0x400
/* --------------------------------------*/
/* Bit masks and shifts for various ops variables */

/* The total number of operations that RxD controller might be
     performing in any of the states */
#define RXD_NUM_OPS 6

/* Bit mask for the condition when none of the valid operations is being performed
    Indicative of the NOOP coondition for a particular state */
#define RXD_OPS_ZERO_BMSK 0x0000

/* Bit mask for indicating whether the diversity RF chain is ON or not. Turning ON involves
    waking the chain up and tuning to the serving cell frequency and turning OFF refers to
    putting the diversity RF chain to sleep mode. This operation involves callback*/
#define RXD_OPS_DIV_BMSK 0x0001

/* Bit mask for whether the AGC is turned on the diversity RF chain or not.
    Diversity mode RF is dependent on AGC and turning AGC on/off involves
    sending suitable mDSP commands to the mDSP. This operation involves callback*/
#define RXD_OPS_AGC_BMSK 0x0002

/* This operation involves configuring the searcher for diversity mode operation
     ON or OFF. This initiates altering the alternative search mode on two antennas
     every 20 ms and other related stuff. This operation does not involve callback */
#define RXD_OPS_SRCH_BMSK 0x0004

/* This bitmask is for enabling/disabling the triage software module for diversity. The
     resultant operations are merging the search results from the two antennas, assigning
     fingers in paired mode, paired mode finger management in triage etc. This operation
     does not involve callback */
#define RXD_OPS_TRI_BMSK 0x0008

/* This operation mask is indicative of whether we need to alter the power control setpoint
    in preparation of turning the RxD off. When this mask is set, it leads to the power control
    target being raised by a determined value to enable the turning OFF of the diversity chain.
    Such an approach leads to a smooth transition from an RxD ON state to and OFF state
    without encountering significant BLER and transient errors */
#define RXD_OPS_PCTL_BMSK 0x010

/* Operations related to this mask are the firing of the RxD state update event on the sequencer's
    timeline. When enabled, the event fires every frame and calls the rxd_state_update() function
    every time. Based on whether the triggers, the state of the RxD controller can be altered.*/
#define RXD_OPS_SEQ_BMSK 0x020

/*Clients For RxD Proc Next CmD that will signal completion without a callback*/
#define WL1_RXD_PROC_CLIENT_SEARCH_BMSK 0x1
#define WL1_RXD_PROC_CLIENT_TRIAGE_BMSK 0x2
#define WL1_RXD_PROC_CLIENT_ALL (WL1_RXD_PROC_CLIENT_SEARCH_BMSK|WL1_RXD_PROC_CLIENT_TRIAGE_BMSK)

/* Macros for RxD state machine */
/* -------------------------*/

/* Generating the trigger based on trigger's bit mask */
#define RXD_TRIG_BMSK(str) \
  (RXD_##str##_TRIGGER_BMSK)

/* Check on whether a particular type of trigger is set or not */
#define RXD_TRIG(str) \
  ((RXD_TRIG_BMSK(str) & rxd_trig) ? TRUE:FALSE)

/* Macro for setting a particular trigger, only need to
    supply the relevant string as an argument */
#define RXD_SET_TRIG(str) \
  (rxd_trig |= RXD_TRIG_BMSK(str))

/* Defintion for setting two triggers simultaneously,
    only need to supply relevant strings */
#define RXD_SET_TRIG2(str1, str2) \
  (rxd_trig |= (RXD_TRIG_BMSK(str1) | RXD_TRIG_BMSK(str2)))

/* Macro for clearing up a particular trigger from the complete mask.
     Only need to supply the relevant string as an argument to the macro */
#define RXD_CLEAR_TRIG(str) \
  (rxd_trig &= (uint32)((uint32)~(RXD_TRIG_BMSK(str))))

/* Macro for clearing up two particular triggers from the complete mask.
     Only need to supply the relevant strings 
     as an argument to the macro */
#define RXD_CLEAR_TRIG2(str1, str2) \
  (rxd_trig &= (uint32)((uint32)~(RXD_TRIG_BMSK(str1)|RXD_TRIG_BMSK(str2))))

#define RXD_CLEAR_RDDS_TRIGS() \
  (rxd_trig &= (uint32) ((uint32)~(RXD_TRIG_BMSK(QUAL)|RXD_TRIG_BMSK(CAP)|RXD_TRIG_BMSK(RSCP))))

#define RXD_CLEAR_HS_TRIGS() \
  (rxd_trig &= (uint32) ((uint32)~(RXD_TRIG_BMSK(ARD)|RXD_TRIG_BMSK(RXDPM)| \
                                   RXD_TRIG_BMSK(ARD_V2)|RXD_TRIG_BMSK(RXDPM_V2)|RXD_TRIG_BMSK(STATIC_RXD))))

#define RXD_CLEAR_PS_TRIGS() \
  (rxd_trig &= (uint32) ((uint32)~(RXD_TRIG_BMSK(PS_SF))))

/* Check on whether a particular type of trigger is set as reason for state or not */
#define RXD_STATE_RSN(str) \
  ((RXD_TRIG_BMSK(str) & rxd_state_reason_bmsk) ? TRUE:FALSE)

/* Macro for setting a particular trigger as reason for state, only need to
    supply the relevant string as an argument */
#define RXD_SET_STATE_RSN(str) \
  (rxd_state_reason_bmsk |= RXD_TRIG_BMSK(str))

/* Macro for clearing up a particular trigger from the complete mask.
     Only need to supply the relevant string as an argument to the macro */
#define RXD_CLEAR_STATE_RSN(str) \
  (rxd_state_reason_bmsk &= (uint32)((uint32)~(RXD_TRIG_BMSK(str))))

/* Macro for clearing the reason for state mask */
#define RXD_CLEAR_ALL_STATE_RSN() (rxd_state_reason_bmsk = 0x00)

#define RXD_CLEAR_RDDS_STATE_RSN() \
  (rxd_state_reason_bmsk &= (uint32) ((uint32)~(RXD_TRIG_BMSK(QUAL)|RXD_TRIG_BMSK(CAP)|RXD_TRIG_BMSK(RSCP))))

#define RXD_CLEAR_HS_STATE_RSN() \
  (rxd_state_reason_bmsk &= (uint32) ((uint32)~(RXD_TRIG_BMSK(ARD)|RXD_TRIG_BMSK(RXDPM)| \
                                                RXD_TRIG_BMSK(ARD_V2)|RXD_TRIG_BMSK(RXDPM_V2)|RXD_TRIG_BMSK(STATIC_RXD))))

#define RXD_CLEAR_PS_STATE_RSN() \
  (rxd_state_reason_bmsk &= (uint32) ((uint32)~(RXD_TRIG_BMSK(PS_SF))))

/* Generating the config mask based on cfg type's bit mask */
#define RXD_CFG_BMSK(str) \
  (RXD_CFG_##str##_BMSK)

/* Check on whether a particular type of config mask is set or not */
#define RXD_CFG(str) \
  ((RXD_CFG_BMSK(str) & rxd_cfg) ? TRUE:FALSE)

/* Macro for setting a particular config mask, only need to supply the
     relevant string as an argument (viz L1_DCH, VOICE, PSDATA, HS) only*/
#define RXD_SET_CFG(str) \
  (rxd_cfg |= RXD_CFG_BMSK(str))

/* Defintion for setting two config masks simultaneously,
    only need to supply relevant strings(viz L1_DCH, VOICE, PSDATA, HS) */
#define RXD_SET_CFG2(str1, str2) \
  (rxd_cfg |= (RXD_CFG_BMSK(str1) | RXD_CFG_BMSK(str2)))

/* Macro for clearing up a particular config type from the complete mask.
    Only need to supply the relevant string (viz L1_DCH, VOICE, PSDATA, HS)
    as an argument to the macro */
#define RXD_CLEAR_CFG(str) \
  (rxd_cfg &= (uint16)((uint16)~(RXD_CFG_BMSK(str))))

/* Macro for clearing all the config masks */
#define RXD_CLEAR_ALL_CFG() (rxd_cfg = 0x00)

/* Generating the ops mask based on ops type's bit mask */
#define RXD_OPS_BMSK(str) \
  (RXD_OPS_##str##_BMSK)

/* Check on whether a particular type of ops is set or not */
#define RXD_OPS(str) \
  ((RXD_OPS_BMSK(str) & rxd_ops) ? TRUE:FALSE)

/* Macro for setting a particular ops, only need to
    supply the relevant string as an argument (viz SEQ, PCTL
    TRI, SRCH, AGC, RF etc) */
#define RXD_SET_OPS(str) \
  (rxd_ops |= RXD_OPS_BMSK(str))

/* Defintion for setting two ops masks simultaneously,
    only need to supply relevant strings(viz SEQ, PCTL
    QUAL, CAP, TMR, TRI, SRCH, AGC, RF etc) */
#define RXD_SET_OPS2(str1, str2) \
  (rxd_ops |= (RXD_OPS_BMSK(str1) | RXD_OPS_BMSK(str2)))

/* Macro for clearing up a particular ops type from the complete mask.
    Only need to supply the relevant string (viz EQ, PCTL QUAL, CAP, TMR,
    TRI, SRCH, AGC, RF) as an argument to the macro */
#define RXD_CLEAR_OPS(str) \
  (rxd_ops &= (uint16)((uint16)~(RXD_OPS_BMSK(str))))

/* Generator macro for creating the state integrity constraints, ie the specific
     operations that should be turned ON in each state. The ops mask fields that
     are set to zero are not set and those that are supplied with the relevant string
     (viz SEQ, PCTL, QUAL, CAP, TMR, TRI, SRCH, AGC, RF etc) are considered
     enabled in that state. This generator creates the states integrity operations and
     is of critical importance in creating the entire state ops table correctly */
#define RXD_GEN_OPS(str1, str2, str3, str4, str5, str6) \
  (RXD_OPS_BMSK(str1)|RXD_OPS_BMSK(str2)|RXD_OPS_BMSK(str3)| \
    RXD_OPS_BMSK(str4)|RXD_OPS_BMSK(str5)|RXD_OPS_BMSK(str6))

/* This macro initiates the condition for making a particular state transition by:
     (a) setting the pend_state variable to next state
     (b) proving the operations mode for sequentially enabling the operations in the
     correct order */
#define RXD_STATE_TRANSITION(str1, str2) \
  rxd_pend_state = str1; rxd_cmd_proc_mode = str2

/* Generating the action mask based on ops type's bit mask */
#define RXD_ACT(str) \
  ((RXD_OPS_BMSK(str) & rxd_act) ? TRUE:FALSE)

/* Macro for clearing up a particular ops type from the complete action mask.
    Only need to supply the relevant string (viz EQ, PCTL QUAL, CAP, TMR,
    TRI, SRCH, AGC, RF) as an argument to the macro */
#define RXD_CLEAR_ACT(str) \
  (rxd_act &= (uint16)((uint16)~(RXD_OPS_BMSK(str))))

/* Whether a particular action operation needs to be performed or not,
    only need to supply relevant strings(viz SEQ, PCTL, QUAL, CAP, TMR,
    TRI, SRCH, AGC, RF etc) */
#define RXD_ACT_MASK(str) \
   (((0x001<<str) & rxd_act) ? TRUE:FALSE)

/* Checking macro for whether al the action ops are cleared or not */
#define RXD_ALL_ACT_CLEAR() \
  (rxd_act == 0x000)
  
    
    /* Bit left shifts for RxD measurement related calculations */
#define RXD_MEAS_LSHFT(val, str) (val << RXD_MEAS_##str##_SHFT)
#define RXD_LSHFT(val, str) (val << str)
    
    /* Bit wise right shift for RxD measurement related calculations */
#define RXD_MEAS_RSHFT(val, str) (val >> RXD_MEAS_##str##_SHFT)
#define RXD_RSHFT(val, str) (val >> str)
    
/* Sets mask for all clients: clients will clear them as they finish*/
#define WL1_RXD_PROC_CLIENT_SET_ALL(void) \
  rxd_proc_client_pending_bmsk = WL1_RXD_PROC_CLIENT_ALL

/* Sets mask for a single client */
#define WL1_RXD_PROC_CLIENT_SET(_client) \
  rxd_proc_client_pending_bmsk |= _client

/* Clears mask for a client */
#define WL1_RXD_PROC_CLIENT_CLEAR(_client) \
  rxd_proc_client_pending_bmsk &= ~_client

/* Function to check if a client is valid */
#define WL1_RXD_CLIENT_CHECK(_client) \
  ((_client & rxd_proc_client_pending_bmsk) ? TRUE:FALSE)

/* Sets mask for a single FOFF client */
#define WL1_RXD_FOFF_CLIENT_SET(_client) \
  rxd_div_chain_foff_client_mask |= (uint16)(1 << (uint16)_client)

/* Clears mask for a FOFF client */
#define WL1_RXD_FOFF_CLIENT_CLEAR(_client) \
  rxd_div_chain_foff_client_mask &= (~((uint16)(1 << ((uint16)_client))))

  /* Gets mask for a FOFF client */
#define WL1_RXD_FOFF_CLIENT_GET(_client) \
  (((uint16)(1 << ((uint16)_client))) & rxd_div_chain_foff_client_mask)

/* Checks mask for a FOFF client */
#define WL1_RXD_FOFF_CLIENT(_client) \
  ((((uint16)(1 << ((uint16)_client))) & rxd_div_chain_foff_client_mask) ? TRUE:FALSE)

/* Sets mask for rxd_pre_proc_bmsk*/
#define RXD_PRE_PROC_SET(_client) \
  rxd_pre_proc_bmsk |= (uint8)(1 << (uint8)_client)

/* Clears mask for rxd_pre_proc_bmsk*/
#define RXD_PRE_PROC_CLEAR(_client) \
  rxd_pre_proc_bmsk &= (~((uint8)(1 << ((uint8)_client))))

/* Checks mask for rxd_pre_proc_bmsk*/
#define RXD_PRE_PROC_CHECK(_client) \
  ((((uint8)(1 << ((uint8)_client))) & rxd_pre_proc_bmsk) ? TRUE:FALSE)

/* Macro for checking whether a particular mask for an NV item is set or not */
#define RXD_NV(str) ((RXD_NV_##str & rxd_nv_ctrl) ? TRUE:FALSE)

/* Adhoc macro for checking whether diversity mode operation is
    possible on this frequency band or not */
#define rfm_rxd_capable(str) TRUE // vikas: AI remove this macro when merged with RF tip

/* Generator macro for creating the NV control value for a particular mode of
    antenna assignment and RxD controller operation This will initialize values for a
    particular kind of NV configuration, viz. CFG masks for voice, forced ON (FON)
    mask for voice, HS (rake with or without equalizer support) call, R99 PS data call,
    and antenna 0 or 1 assigned as primary */
#define RXD_GEN_NV(str1, str2, str3, str4, str5, str6, str7, str8, str9, str10, str11, str12, str13, str14, str15, str16, str17, str18, str19, str20, str21, str22, str23, str24, str25) \
    (RXD_NV_##str1|RXD_NV_##str2|RXD_NV_##str3|RXD_NV_##str4| \
    RXD_NV_##str5|RXD_NV_##str6|RXD_NV_##str7|RXD_NV_##str8|\
    RXD_NV_##str9|RXD_NV_##str10|RXD_NV_##str11|RXD_NV_##str12|\
    RXD_NV_##str13|RXD_NV_##str14|RXD_NV_##str15|RXD_NV_##str16|\
    RXD_NV_##str17|RXD_NV_##str18|RXD_NV_##str19|RXD_NV_##str20|\
    RXD_NV_##str21|RXD_NV_##str22|RXD_NV_##str23|RXD_NV_##str24|RXD_NV_##str25)

/* Find the Max Value */
#define RXD_MAX(str1,str2) \
   ((str1 > str2) ? str1:str2)

/* check if the second carrier is deactivated */
#define RXD_SEC_CARR_IS_DEACT() \
   (rxd_sec_carr_deactivated)

   /* activate the second carrier */
#define RXD_SEC_CARR_SET_ACTIVE() \
   (rxd_sec_carr_deactivated = TRUE)

   /* deactivate the second carrier */
#define RXD_SEC_CARR_SET_DEACT() \
   (rxd_sec_carr_deactivated = FALSE)

#define RXD_GET_LOCK_MODE_PARAM() \
  (rxd_div_ant_lock_mode_parm.div_ant_lock_mode) 

#define RXD_SET_LOCK_MODE_PARAM() \
(rxd_div_ant_lock_mode_parm.div_ant_lock_mode = RXD_DIV_ANT_INLOCK)

#define RXD_CLEAR_LOCK_MODE_PARAM() \
(rxd_div_ant_lock_mode_parm.div_ant_lock_mode = RXD_DIV_ANT_OUTOFLOCK)

/* check if clocks must be dropped*/
#define RXD_CHECK_DIV_CLK_DROP()\
(wl1_cme_rxd_check_div_clk_drop)

/* Maximum values for various timers*/
/* ------------------ */

/* The maximum value of capacity timer is set to 100ms for capacity & RSCP trigger */
#define RXD_MAX_CAP_TIMER_IN_FRAMES 10

/* The maximum value of capacity timer is set to 10ms for SCCH trigger */
/* This means the timer will expire once SCCH trigger gets reset (no hysteresis) */
#define RXD_MAX_SCCH_TIMER_IN_FRAMES 1

/* SCCH statistics are checked and trigger status is updated once 
   every window-length number of sub frames */
#define RXD_MAX_SCCH_WINDOW_LENGTH_IN_SUB_FRAMES 100
#define RXD_MAX_SCCH_WINDOW_LENGTH_IN_FRAMES 20

/* SCCH threshold used to set RxDPM trigger */
#define RXD_SCCH_THRESHOLD_IN_SUB_FRAMES 20

/* The maximum value of quality timer is set to 10s */
#define RXD_MAX_QUAL_TIMER_IN_FRAMES 1000

/* The maximum value of off timer is set to 80ms */
#define RXD_MAX_OFF_TIMER_IN_FRAMES 8


/* The maximum number of samples in RxD meas log packet */
#define RXD_MAX_MEAS_LOG_NUM_SAMPLES 50

/* Gain shift for rscp related filtering */
#define RXD_MEAS_RSCP_FILT_GAIN_SHFT 8

/* Fixed point base for rscp metric */
#define RXD_MEAS_RSCP_FIX_PT_SHFT 24

/* Shift to be applied for determining constant multiplier for traffic to pilot 
   amplitude ratio for capacity measurements */
#define RXD_MEAS_CAP_CONST_SHFT 12

/* Constant multiplier in Ect/Ecp calculations */
#define RXD_MEAS_CAP_CONST_MULTIPLIER 15

/* Fixed point base for capacity metric */
#define RXD_MEAS_CAP_FIX_PT_SHFT 24

/* Gain shift for slow capacity related filtering */
#define RXD_MEAS_CAP_SLOW_FILT_GAIN_SHFT 10

/* Gain shift for fast capacity related filtering */
#define RXD_MEAS_CAP_FAST_FILT_GAIN_SHFT 2

/* Weight parameter shift/exponent for cobining slow and fast filtering */
#define RXD_MEAS_CAP_FILT_WEIGHT_SHFT 4

/* Full bit width of capacity measurement fixed point values*/
#define RXD_MEAS_CAP_FULL_WIDTH_SHFT 32

/* Full bit width of capacity measurement fraction width in number of bits */
#define RXD_MEAS_CAP_FRAC_FIX_PT_SHFT 7

/* Bit width for supported logarithm for capacity measurement */
#define RXD_MEAS_CAP_LOG_SHFT 16

/* Gain in dB per bit shift */
#define RXD_MEAS_DB_GAIN_PER_SHIFT 3

/* Floor for RxD capacity threshold in dB */
#define RXD_CAP_MEAS_LOG_FLOOR -17

/* Gain shift for quality related filtering */
#define RXD_MEAS_QUAL_FILT_GAIN_SHFT 6

/* Threshold shift for quality metric */
#define RXD_MEAS_QUAL_THRESH_SHFT 11

/* Shift to be applied to each frame error for quality measurements */
#define RXD_MEAS_QUAL_FER_SHFT 14

/* Shift to be applied for measurement related flags */
#define RXD_MEAS_LOG_CAP_SHFT 0
#define RXD_MEAS_LOG_QUAL_SHFT 1
#define RXD_MEAS_LOG_RXD_SHFT 2 

#define RXD_PCH_TEARDOWN_AGC_BMSK 0x01
#define RXD_PCH_TEARDOWN_RF_BMSK 0x02
#define RXD_PCH_TEARDOWN_CLK_BMSK 0x04

/* Setting the threshold to 10 dB, so the EcIo imbalance check satisfies for > 5db difference  */
#define RXD_PCH_ECIO_IMBALANCE_THRESH 10
/* 5ms */
#define RXD_PCH_HOLD_TIME_FOR_DIV_DISABLE 5
/* 2dB */
#define RXD_PCH_HYST_VAL 2

extern boolean wl1_rxd_pch_debug_f3;

/* defines the largest length(in frames) of a QTA gap before 
   rxdpm needs to be enabled. The value is 12 currently to account for
   2 frames of rxd pre-freeze and 10 frames of the actual QTA gap */
#define RXD_QTA_GAP_THRESHOLD 0x0C

/* defines minimum number of samples required to compute average Agc value in IL mode*/
#define RXD_IL_MIN_NUM_SAMPLE 0x05

/* Minimum time for an RxD WRM Request in SCLK time set for 45ms*/
#define RXD_MIN_REQUEST_TIME_IN_SCLK   ((45 * TIMETICK_NOMINAL_FREQ_HZ)/1000)

/* Minimum time for an RxD WRM Request for PCH and ACQ in SCLK time set for 20 ms.
   Changing this to 20ms because of SR target. 
   If request duration is higher for diversity than primary, lock can get denied.
   Can be a problem especially in X + W idle-idle scenario. */
#define RXD_MIN_REQUEST_TIME_IN_PCH_AND_ACQ_IN_SCLK   ((20 * TIMETICK_NOMINAL_FREQ_HZ)/1000)

/* Number of ms to wait for AGC_CFG_DONE to complete before crashing */
#define RXD_AGC_CFG_DONE_PENDING_TIMEOUT_MS 50

/* Boolean to Track Sec. carrier Status */
extern boolean  rxd_sec_carr_deactivated;

/* -----------------------------------------------------------------------
** Type Declarations
** ----------------------------------------------------------------------- */

/* Callback function definition */
typedef void RXD_HS_RXD_TRANS_IND_CB_FUNC_TYPE(boolean transition);

typedef void RXD_CME_TRANS_CB_FUNC_TYPE(boolean rxd_state);

/* RxD state call back function type */
typedef void RXD_DSR_TRANS_IND_CB_FUNC_TYPE(boolean rxd_state);

/* Type_def for trm function type */
typedef void (*rxd_trm_foff_cb_type)(boolean ca_enabled);

/* Various callbacks that RxD controller processes as posted by the calling
    HW module in a RxD controller (L1) task context
    0: used for calling the RF module to tune to a frequency
    1: return back to RxD controller context and call next function from list of processor functions
    */

typedef enum
{
  RXD_CARR_PRI = 0,
  RXD_CARR_SEC,
#ifdef FEATURE_WCDMA_3C_HSDPA
  RXD_CARR_TER,
#endif
  RXD_NUM_CARRIERS
} rxd_carrier_enum_type;

typedef enum
{
  /* Callback for processing the next command from command processor */
  RXD_CB_FUNC_PROC_NEXT      = 0x00,

  RXD_NUM_CB_FUNCS           = 0x01

} rxd_cb_func_enum_type;

/* States for the RxD Controller*/
typedef enum
{
  /* RXD_INIT: (Single Rx chain) RxD is initialized but disabled.
    State machine not ticking, all state parameters are reset. */
  RXD_INIT,

  /* RXD_FOFF: (Single Rx chain) RxD is forcibly turned OFF.
    State machine ticking, no measurements. */
  RXD_FOFF,

  /* RXD_FON: (Dual Rx chain) RxD is forced on. State
    machine operational, no measurements. */
  RXD_FON,

  /* RXD_OFF: (Single Rx chain) RxD is enabled, but currently off. 
    Measurements are being done. State m/c operational. */
  RXD_OFF,

  /* RXD_ON: (Dual Rx chain) RxD is enabled.
    Measurements are being done. State m/c operational. */
  RXD_ON,

  /*RXD_PCH_FON: (Dual Rx chain) RxD is enabled.
    RxD is enabled in PCH_SLEEP state. State m/c is not operational */
  RXD_PCH_FON,

  RXD_NUM_STATES
}rxd_states_enum_type;

/* The order in which the command processor for RxD controller will
  process the commands.In left mode functionalities related to left most
  bits of action variable are processed first and vice versa for right mode */

typedef enum
{
  /* LEFT mode : Order of operations shall be:
   RF >> AGC >> SRCH >> TRI >> TMR >> CAP >> QUAL >> PCTL >> SEQ */
  RXD_PROC_MODE_LEFT,

  /* RIGHT mode : Order of operations shall be:
   SEQ >> PCTL >> QUAL >> CAP >> TMR >> TRI >> SRCH >> AGC >> RF */
  RXD_PROC_MODE_RIGHT,

/* NOOP mode: Mode in which RxD command processor is idle */
  RXD_PROC_MODE_NOOP

}rxd_cmd_proc_mode_enum_type;

/* If none of the below clients uses second chain WCDMA will make use of it */
typedef enum
{
  RXD_DIV_CHAIN_FOR_WRM,
  RXD_FOFF_FOR_DSDA,
  RXD_FOFF_FOR_CAP_FILT,
  RXD_FOFF_FOR_CURRENT_SAVING,
  RXD_FOFF_FOR_DEBUG,
  RXD_FOFF_FOR_EDRX,
  RXD_FOFF_FOR_CELL_RESEL,
  RXD_FOFF_FOR_FMO,
  RXD_DIV_CHAIN_RESOURCE_NUM_CLIENTS  
} rxd_div_chain_resource_client_type;

/* enum declaration for antenna mode*/
typedef enum
{
  RXD_DIV_ANT_INLOCK = 0,
  RXD_DIV_ANT_OUTOFLOCK
}rxd_div_ant_lock_mode_enum_type;

/* enum declaration for Rx chain cfg requested */
typedef enum
{
  RXD_PRIM_CHAIN0 = 0,
  RXD_PRIM_CHAIN1,
  RXD_NORMAL,
  RXD_NONE
}rxd_chain_cfg_enum_type;

/* enum declaration for ARD status */
typedef enum
{
  RXD_ARD_INIT,
  RXD_ARD_MMT,
  RXD_ARD_DWELL,
  RXD_ARD_DISABLE,
  RXD_ARD_INFINITE
}rxd_ard_status_enum_type;

/* enum for the reasons rxdpm trigger can be set */
typedef enum
{
  RXDPM_INVALID_REASON          = 0x00,
  RXDPM_FOR_INIT                = 0x01,
  RXDPM_FOR_SCCH_RATE_HIGH      = 0x02,
  RXDPM_FOR_CPICH_SNR_LOW_C0    = 0x04,
  RXDPM_FOR_CPICH_SNR_LOW_C1    = 0x08,
  RXDPM_FOR_CPICH_SNR_LOW_C2    = 0x10,
  RXDPM_FOR_CPICH_SNR_LOW_C3    = 0x20,
  RXDPM_FOR_QTA_GAP             = 0x40,
} rxd_rxdpm_reason_type;

/* enum for the reasons rxdpm trigger can be set */
typedef enum
{
  ARD_INVALID_REASON            = 0x00,
  ARD_FOR_INIT                  = 0x01,
  ARD_FOR_DWELL_TIMER_EXPIRED   = 0x02,
  ARD_FOR_CPICH_SNR_LOW_C0      = 0x04,
  ARD_FOR_CPICH_SNR_LOW_C1      = 0x08,
  ARD_FOR_CPICH_SNR_LOW_C2      = 0x10,
  ARD_FOR_CPICH_SNR_LOW_C3      = 0x20,
  ARD_FOR_QTA_GAP               = 0x40,
} rxd_ard_reason_type;

typedef enum
{
  RXD_PCH_NO_DIV = 0,
  RXD_PCH_FAST_DIV,
  RXD_PCH_FULL_DIV,
  RXD_PCH_INACTIVE
}rxd_pch_mode_enum_type;

typedef enum
{
  RXD_AGC_IMBALANCE,
  RXD_AGC_ECIO_IMBALANCE,
  RXD_ECIO_IMBALANCE,
  RXD_NO_IMBALANCE_CHECK
}rxd_pch_imbalance_check_enum_type;

/* struct to hold the state of a qta gap */
typedef enum
{
  RXD_QTA_STATUS_NOT_IN_GAP,
  RXD_QTA_STATUS_IN_GAP,
  RXD_QTA_STATUS_END_GAP
} rxd_qta_state_enum_type;

/* Reasons to setup RxD timers */
typedef enum
{
  RSN_QTA,
  RSN_TRM
} wl1_rxd_tmr_rsn_enum_type;

/* List of RxD local cmd senders */
typedef enum
{
  WL1_RXD_PROC_START,
  WL1_RXD_RXDCTRL_LOG,
  WL1_RXD_ARD_V2_LOG,
  WL1_RXD_PROC_NEXT,
  WL1_RXD_FREEZE_FOR_QTA,
  WL1_RXD_CLIENT_SEARCH,
  WL1_RXD_CLIENT_TRIAGE,
  WL1_RXD_WRM_TIMER_EXPIRED,
  WL1_RXD_WRM_UNLOCK_CANCEL,
  WL1_RXD_TRM_CQI_ADJUST,
  WL1_RXD_QUAL_MEAS_UPDATE, 
  WL1_RXD_FET_LATCHED,
} wl1_rxd_ctrl_cmd_cause_enum_type;

/* List of RxD Oride modes in PCH_SLEEP state */
typedef enum
{
  WL1_RXD_PCH_DEFAULT,
  WL1_RXD_PCH_FOFF,
  WL1_RXD_PCH_FON,
  WL1_PCH_MAX_ORIDE_OPTS
} wl1_pch_oride_enum_type;

/* List of timer values used for DSDS/DSDA */
typedef enum
{
  WL1_RXD_DSDA_FACH_DCH=10,
  WL1_RXD_DSDS_QTA=20,
  WL1_RXD_TRM_CQI_ADJUST_TIME = 20,
} wl1_rxd_timer_threshold_enum;

/* List of DSDA processing states */
/* Will be used when RxD needs to remember DSDA state transition and process it
   later as L1M is not initialised when DSDA mode changed */
typedef enum
{
  Wl1_RXD_DSDA_NO_TRANS,
  WL1_RXD_DSDA_PROCESS_EN,
  WL1_RXD_DSDA_PROCESS_DIS,
  WL1_RXD_DSDA_ENABLED,
  WL1_RXD_DSDA_DISABLED
} wl1_rxd_dsda_decision_enum;

/*list of seq event*/
typedef enum
{
  RXD_START_PROC_EVT,
  RXD_FET_LATCH_EVT, 
  RXD_MAX_EVT_NUM  
}rxd_evt_enum_type;

/*List of pre procession bmsk*/
typedef enum
{ 
  RXD_PRE_PROC_FET_BMSK, 
  RXD_PRE_PROC_MAX_BMSK
}rxd_pre_proc_enum_type;


/*RxD local command structure*/
typedef struct 
{
  /*header*/
  l1_local_cmd_hdr_type hdr;
  /*sender of the local command*/
  wl1_rxd_ctrl_cmd_cause_enum_type cause;
  /* for trm the reason for unlock */
  trm_unlock_event_enum_t trm_unlock_event;
  /* miscellaneous packed data */
  uint16 data;  
  /* unlock by time in trm */
  uint32 unlock_by;
} wl1_rxd_ctrl_cmd_type;

typedef enum
{
  Wl1_RXD_FORCED_OFF_STATE_UPD_LOCAL_CMD,
  WL1_RXD_PROC_NEXT_STATE_UPD_LOCAL_CMD,
  WL1_RXD_STATE_UPD_EVT_HLDR_LOCAL_CMD,
  WL1_RXD_STATE_UPD_SEQ_LOCAL_CMD,
  WL1_RXD_STATE_UPD_CFG_UPDATE_CMD,
  WL1_RXD_STATE_UPD_CM_LOCAL_CMD,
  WL1_RXD_STATE_UPD_DRX_LOCAL_CMD,
  WL1_RXD_STATE_UPD_DBG_LOCAL_CMD,
  WL1_RXD_STATE_UPD_ASD_LOCAL_CMD,
  WL1_RXD_L1M_STATE_UPD_LOCAL_CMD_HANDLED
} wl1_rxd_debug_enum_type;

/* RxD reason debug clients:they indicate where the 
   reason bitmask was modified                     */
typedef enum
{
  Wl1_RXD_STATE_UPD_INIT_TRANSITION,
  Wl1_RXD_STATE_UPD_FOFF_TRANSITION,
  Wl1_RXD_STATE_UPD_FON_TRANSITION,
  Wl1_RXD_STATE_UPD_TRANSITION,
  WL1_RXD_DEC_TRIG_TMRS,
  WL1_RXD_UPD_TRIGS,
  WL1_RXD_PROC_TRIGS,
  WL1_RXD_ARD_INIT,
  WL1_RXD_V2_ARD_INIT,
  WL1_ORIDE_FOR_PS_DATA,
  WL1_REQ_FOR_SEC
} wl1_rxd_rsn_debug_enum_type;

/* Struct declaration for PCH mode RxD */
typedef struct
{
  rxd_pch_mode_enum_type wl1_rxd_pch_mode;
  boolean wl1_rxd_pch_tune_pending;
  uint32 wl1_rxd_pch_waiting_on_sigs;
  uint16 wl1_rxd_pch_full_div_cycle_ctr;
  boolean wl1_rxd_pch_trm_req_pending;
  //boolean wl1_rxd_pch_disable_clocks_pending;
  boolean wl1_rxd_pch_teardown_started;
  boolean wl1_rxd_placed_out_of_lock;
  
  uint16 filtecio_upper_thresh;
  uint16 filtecio_lower_thresh;
  int32  pi_sum_thresh;
  uint16 min_full_div_cycles_ctr;
}rxd_pch_db_type;

/* Struct declaration for PCH mode RxD NV items*/
typedef struct
{
  int16 filtecio_upper_thresh;
  int16 filtecio_lower_thresh;
  int16 pi_sum_thresh;
  int16 min_full_div_cycles;
  int16 oride_val;
}rxd_pch_nv_type;

/* struct declaration for antenna lock mode decision parameters and current mode*/
typedef struct
{
  int32 prim_ant_accum_agc;
  int32 div_ant_accum_agc;
  uint8 frame_counter_for_agc_avg;
  uint8 sample_counter;
  rxd_div_ant_lock_mode_enum_type div_ant_lock_mode;
}rxd_ant_lock_mode_param_struct_type;

/* holds the diversity antenna lock mode state and decision parameters*/
extern rxd_ant_lock_mode_param_struct_type rxd_div_ant_lock_mode_parm;

/* holds a copy of the diversity antenna lock mode state and decision parameters during HHO*/
extern rxd_ant_lock_mode_param_struct_type rxd_div_ant_lock_mode_parm_backup;

/* struct that holds trigger timers */
typedef struct{
/*Following are RxD enable trigger timers */
  uint16 dch_set_up_signl;
  uint16 qual;
  uint16 cap;
  uint16 rscp;
  uint16 rxdpm;
  uint16 ard;
  uint16 ps_sf;
  uint16 static_rxd;
  uint16 rxdpm_v2;
  uint16 ard_v2;
  uint16 asd;
/*Following are RxD disable trigger timers */
  uint16 temp_disable;
}rxd_trig_timer_type;

/* Static database */
typedef struct{
  boolean cm_enabled;
  boolean drx_enabled;
  boolean txd_enabled; /* TxD is enabled for any cell in QSET*/
  boolean mimo_enabled;
  boolean qice_enabled;
}rxd_static_db_struct_type;

/* ARD database */
typedef struct{
  rxd_ard_status_enum_type ard_status;
  uint16 schedule_ctr;
  uint16 mmt_period_sub_frames;
  uint16 mmt_period_frames;
  uint16 dwell_period_frames;
  uint16 dwell_period_frames_min;
  int32 tot_cpich_snr_c0_rxd;
  int32 tot_cpich_snr_c1_rxd;
  int32 tot_cpich_snr_c0_norxd;
  int32 tot_cpich_snr_c1_norxd;
  int16 cpich_snr_delta_thresh;
  uint16 cpich_snr_ctr_rxd;
  uint16 cpich_snr_ctr_norxd;
  boolean ard_decision;
  int16 curr_run_length;
  int16 delta_c0;
  int16 delta_c1;
  uint16 fw_stuck_ctr;
}rxd_ard_db_struct_type;

typedef struct{
  /* Indicate if RxDPM v2 was previously enabled */
  boolean prev_rxdpm_enabled;
  /* Indicate if RxDPM v2 is currently enabled */
  boolean rxdpm_enabled;
  /* Indicate if ARD v2 is currently enabled */
  boolean ard_enabled;
  /* Indicate if the SCCH scheduling rate is higher than the threshold */
  boolean scch_rate_high;
  /* RxDPM/ARD v2 Hysteresis timer L */
  int8 hysteresis_tmr;
  /* RxDPM/ARD v2 algo thresholds */
  uint16 mmse_tap_eng_delta;
  int16 cpich_snr_thresh_1;
  int16 cpich_snr_thresh_2;
  int16 cpich_snr_thresh_3;
  int16 cpich_snr_thresh_4;
  uint16 min_dwell_time_k;
  uint8 filter_settle_time_l;
  /* Indicate if Dual Carrier is enabled */
  boolean dc_enabled;
  /* Indicate if 3C Carrier is enabled */
  boolean tc_enabled;
}rxd_ard_v2_db_struct_type;

/* struct to hold details regarding qta gaps */
typedef struct{
  rxd_qta_state_enum_type qta_gap_status;
  boolean rxdpm_trigger_set;
  uint8  start_cfn;
  uint8  cfn_gap;
}wl1_rxd_qta_struct_type;

typedef struct{
 uint8                     l1_state;
 wl1_rxd_debug_enum_type   module_posted;
 uint16                    curr_cfn; 
 uint16                    client; 
}wl1_rxd_debug_struct_type;

/* Struct to hold RxD reason debug bitmask information*/
typedef PACKED struct PACKED_POST{
 uint16                        curr_cfn;
 wl1_rxd_rsn_debug_enum_type   location_posted;
 uint32                        rsn_bmsk;
}wl1_rxd_rsn_debug_struct_type;

/* Max. Trace Buffer History */
#define WL1_RXD_MAX_TRACE_BUFFER_HISTORY (20)

#define WL1_RXD_INVALID_CLIENT           (0xFFFF)

/* Maximum number of samples per log packet. */
#define RXD_NUM_MAX_ARD_SAMPLES  (50)

/* Number of log packet buffers. */
#define RXD_NUM_ARD_LOG_BUFFERS      (2)

/* ARD log packet. */
LOG_RECORD_DEFINE(RXD_ARD_LOG_PKT)
  uint8                      version_number;
  uint8                      num_samples;
  uint16                     mmt_dwell_time_decision;
  int16                      delta_carr0;
  int16                      delta_carr1;
  WfwQiceArdLogStruct samples[RXD_NUM_MAX_ARD_SAMPLES];
LOG_RECORD_END

#define RXD_NUM_MAX_ARD_V2_SAMPLES  (75)

typedef PACKED struct PACKED_POST
{
  /* QICE cpich SNR 16Q5 */
  int16 snr; /* averated cpich snr over 1 subframe.  */

  /* mmse taps energy on Rx0*/
  uint32 w0;

  /* mmse taps energy on Rx1*/
  uint32 w1;
}snr_tapEnergy_struct_type;

typedef PACKED struct PACKED_POST
{
  uint16 subframe;   /* global subframe number during the Tm */
  
  /* CPICH/MMSE Filter Ready Indicators */
  uint8 filter_indicator;

  snr_tapEnergy_struct_type snr_tapEnergy[RXD_NUM_CARRIERS];
}rxd_ard_v2_log_sample_struct_type;

/* ARD_V2 log packet. */
LOG_RECORD_DEFINE(RXD_ARD_V2_LOG_PKT)
  uint8                      version_number;
  uint8                      num_samples;
  rxd_ard_v2_log_sample_struct_type samples[RXD_NUM_MAX_ARD_V2_SAMPLES];
LOG_RECORD_END

/*Structure definition for the RxD capacity/quality metrics log packet
    Each sample in the log packet has the following format:
      0                   1
      0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5
     +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
     | rxd_meas_mask | rxd_cap_frac  |
     +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
     |       Ect_Ecp_inst(MSB)       |
     +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
     |       Ect_Ecp_inst(LSB)       |
     +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
     |            F_E1(MSB)          |
     +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
     |            F_E1(LSB)          |
     +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
     |            F_E2(MSB)          |
     +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
     |            F_E2(LSB)          |
     +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
     |             F_Q               |
     +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+


  This packet is logged whenever an update in one of the fields
  is detected in any of the state update calls */

/* RxD capacity quality metrics to be logged */
/* ------------------------------------- */
typedef PACKED struct PACKED_POST
{
  /* Flag indicating whether 
   * bit 0 - capacity measurement flag cap_t()
   * bit 1 - quality measurement flag qual_t()
   * bit 2 - whether Rxd is turned on or not */
  uint8 rxd_meas_mask;
  /* Fraction for weighted combining of slow and 
   * fast filtered values */
  uint8 rxd_meas_cap_frac;
  /* Instantaneous value of Ect/Ecp metric */
  uint32 ect_ecp_inst;
  /* Slow filtered value of Ect/Ecp metric */
  uint32 f_e1;
  /* Fast filtered value of Ect/Ecp metric */
  uint32 f_e2;
  /* Filtered value of quality metric */
  uint16 f_q;
}rxd_meas_log_struct_type;

/* Log packet description for RxD measurement log packet. The following macro
   is standard diag template for creating the RxD meas log packets and creates the 
   basic fields by default */
LOG_RECORD_DEFINE(L1_RXD_MEAS_LOG_PKT)
  /* Number of samples in this log packet */
  uint8 num_samples;
  /* Start CFN, at which logging started */
  uint8 s_cfn;
  /* All the samples containing cap/qual related information */
  rxd_meas_log_struct_type meas_sample[RXD_MAX_MEAS_LOG_NUM_SAMPLES];
LOG_RECORD_END


/*Structure definition for the RxD controller log packet
  This packet is logged whenever an update in one of the fields
  is detected in any of the state update calls */

/* Log packet description for RxD controller log packet. The following macro
   is standard diag template for creating the RxD log packets and creates the 
   basic fields by default */
LOG_RECORD_DEFINE(L1_RXD_CTRL_LOG_PKT)
  uint8 version_number;
  /* State for Rxd Controller */
  uint8 rxd_state_log;
  /* New state for RxD Controller(invalid = RXD_NUM_STATES(9),
     when no state transition)*/
  uint8 rxd_pend_state_log;
  /* Operations done by RxD software */
  uint8 rxd_ops_log;
  /* Trigger for RxD state machine */
  uint16 rxd_trig_log;
  /* RxD State Reason */
  uint16 rxd_state_reason_log;
  /* RxD State Timer */
  uint16 rxd_state_tmr_log;
  /* RxD Filter Timer */
  int8 rxd_filter_tmr_log;
LOG_RECORD_END

/* -----------------------------------------------------------------------
** Global Data Declarations
** ----------------------------------------------------------------------- */

/* Ids for the primary and diversity RF chains. For MDSP commands,
    these antenna ids should be used for the purpose of antenna identification */
  extern uint8 rxd_prim_ant_id;
  extern uint8 rxd_div_ant_id;

/* The mask created from NV value based on which RXD operations to perform */
extern uint32 rxd_nv_ctrl;


/* Mask indicating the ops variables for a particular state
     0   1   2   3   4   5   6   7   8   9   10  11  12  13  14  15
     +---+---+----+---+----+---+---+---+---+---+---+---+---+---+---+---+
     |RF |AGC|SRCH|TRI|PCTL|SEQ|R    E    S    E    R    V    E   D|
     +---+---+----+---+----+---+---+---+---+---+---+---+---+---+---+---+
     Each of the bits corresponds to particular ops masks */
extern uint16 rxd_ops;

/* Type of request for RxDiv setup
      0    1     2     3     4     5     6     7
     +-----+-----+-----+-----+-----+-----+-----+-----+
     L1_DCH|VOICE|PS99 |HS   |R  E  S  E  R  V  E  D
     +-----+-----+-----+-----+-----+-----+-----+-----+
     Each of the bits corresponds to particular cfg masks */
extern uint16 rxd_cfg;

/* Flag indicating whether div chain init needs to be performed if it was 
   suspended earlier */
extern boolean rxd_div_init_needed;

/* Type definition for the callback function that will be called in response to a forced OFF
    request, when the RxD controller is ready with releasing the RF resources */
typedef void (*rxd_foff_cb_type)(void);

/* Hack till fwfreeze support is in place */
extern boolean rxd_freeze_flag;
extern boolean wl1_rxd_debug_dont_request_trm;

extern boolean wl1_cme_rxd_check_div_clk_drop;

/* =======================================================================
**                        Function Declarations
** ======================================================================= */


/*===========================================================================
FUNCTION wl1_rxd_handle_olpc_qual_meas_update

DESCRIPTION
This function turn on chain clks

DEPENDENCIES
None

RETURN VALUE
None

SIDE EFFECTS
None
===========================================================================*/
extern void wl1_rxd_handle_olpc_qual_meas_update(uint8 num_tbs, uint8 num_err);

/*===========================================================================
FUNCTION rxd_div_chain_clk_turn_on

DESCRIPTION
This function turn on chain clks

DEPENDENCIES
None

RETURN VALUE
None

SIDE EFFECTS
None
===========================================================================*/
extern void rxd_div_chain_clk_turn_on(void);

/*===========================================================================
FUNCTION rxd_div_chain_clk_turn_off

DESCRIPTION
  This function turns off chain 1 clks

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern void rxd_div_chain_clk_turn_off(void);

/*===========================================================================
FUNCTION rxd_div_ant_lockmode_parameters_init

DESCRIPTION This function initializes or clears the div antenna lock mode 
variables.

DEPENDENCIES

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern void rxd_div_ant_lockmode_parameters_init(void);

/*===========================================================================
FUNCTION rxd_init

DESCRIPTION
  This function initializes the RxD controller by
   (a) Putting the diversity antenna in WCDMA mode
   (b) Puts the divesity RF chain in sleep

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

extern void rxd_init(void);

/*===========================================================================
FUNCTION wl1_rxd_pre_proc_rxd_on_trans_ops()

DESCRIPTION
  This function performs the pre processing operations prior to RxD transition on
  should be called by wrm_request_and_notify_secondary_antenna only
   1. If FMO is ongoing, call back is registered with FMO module. FMO module
      informs RxD with that FMO is suspended by calling the registered callback.
      return FALSE
   2. If FMO is not on going, continue FET pre preocess. 

   return FALSE only when FMO is ongoing. 

DEPENDENCIES
  None

RETURN VALUE
  boolean

SIDE EFFECTS
  None
===========================================================================*/
extern boolean wl1_rxd_pre_proc_rxd_on_trans_ops();

/*===========================================================================
FUNCTION rxd_register_cme_trans_cb

DESCRIPTION
  This function is called by CME module to register callback

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean rxd_register_cme_trans_cb(
  /* Callback function pointer */
  RXD_CME_TRANS_CB_FUNC_TYPE *cm_cme_cb);

/*===========================================================================
FUNCTION rxd_deregister_cme_trans_cb

DESCRIPTION
  This function is called by CME module to de-register callback

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void rxd_deregister_cme_trans_cb(void);

/*===========================================================================
FUNCTION     rxd_register_dsr_callback

DESCRIPTION  
  This function registers DSR callback that will be called when RxD state changes

DEPENDENCIES 
  None 

RETURN VALUE 
  None

SIDE EFFECTS 
  None
===========================================================================*/
extern void rxd_register_dsr_callback(RXD_DSR_TRANS_IND_CB_FUNC_TYPE *dsr_cb);
/*===========================================================================
FUNCTION     rxd_deregister_dsr_callback

DESCRIPTION  
  This function deregisters the RxD DSR callback

DEPENDENCIES 
  None 

RETURN VALUE 

SIDE EFFECTS 
  None
===========================================================================*/
extern void rxd_deregister_dsr_callback(void);
#ifdef FEATURE_QSH_DUMP
/*===========================================================================
FUNCTION wl1_cme_mini_dump_cb

DESCRIPTION
This function Records Mini Dump Information for Debug

DEPENDENCIES
None

RETURN VALUE
None

SIDE EFFECTS
None
===========================================================================*/
void wl1_rxd_mini_dump_cb(void* debug_dump_ptr);
#endif
/*===========================================================================
FUNCTION wl1_rxd_pch_init
         
DESCRIPTION
  Initialise all PCH RxD variables and request TRM for secondary chain

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern void wl1_rxd_pch_init(void);


/*===========================================================================
FUNCTION wl1_rxd_pch_cleanup
         
DESCRIPTION
  Cleanup PCH RxD variables and carry over RxD state if needed

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern void wl1_rxd_pch_cleanup(void);


/*===========================================================================
FUNCTION wl1_rxd_pch_start
         
DESCRIPTION
  This function is to be called when DRX command is received. It enables and 
  disables the div chain thus preparing it for operation in the awake cycles.

DEPENDENCIES
  None

RETURN VALUE
  Boolean: Returns TRUE if RxD can be enabled in PCH state. Else, returns FALSE

SIDE EFFECTS
  None
===========================================================================*/
extern boolean wl1_rxd_pch_start(void);

/*===========================================================================
FUNCTION wl1_rxd_pch_wakeup_state_update
         
DESCRIPTION
  This function is to be called when UE is woken up for PICH if RxD in 
  Idle/PCH has been enabled. It needs to be called before primary RF chain is 
  woken up.

DEPENDENCIES
  None

RETURN VALUE
  rxd_pch_mode_enum_type: Returns the RxD mode for current PICH read

SIDE EFFECTS
  None
===========================================================================*/
extern rxd_pch_mode_enum_type wl1_rxd_pch_wakeup_state_update(void);

/*===========================================================================
FUNCTION wl1_rxd_pch_wakeup_both_chains
         
DESCRIPTION
  This function is to be called when UE wakes up for PICH, after enabling 
  primary clocks, if RxD is to be enabled for this wake cycle. If tune was 
  not done previously, this function makes sure Div chain is enabled and tuned.
  If RxD mode is Fast Diversity, this function initiates the SSBI write to put 
  Div chain in low power mode.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern void wl1_rxd_pch_wakeup_both_chains(void);

/*===========================================================================
FUNCTION wl1_rxd_pch_power_up_diversity
         
DESCRIPTION
  This function is to be called in Fast Diversity Mode when the Dic chain is 
  to be powered up. The Div chain will be powered up if pi_sum threshold is 
  reached and/or oride mode is set.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern boolean wl1_rxd_pch_power_up_diversity(int32 pi_sum);

/*===========================================================================
FUNCTION wl1_rxd_pch_teardown_diversity
         
DESCRIPTION
  This function is to be called once Div chain is no longer needed. It starts 
  off the process of bringing down the div chain by sending stop AGC command 
  to mdsp and sleep command to RF. Once both these are done, div clocks 
  will be disabled.

DEPENDENCIES
  None

RETURN VALUE
  Boolean

SIDE EFFECTS
  None
===========================================================================*/
extern boolean wl1_rxd_pch_teardown_diversity(void);

/*===========================================================================
FUNCTION wl1_rxd_pch_check_status
         
DESCRIPTION
  This function is to be called by drx before primary chain is torn down; If TRM returned
  Div chain and PCH_SLEEP initialisation was not done earlier, it will be done now.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern void wl1_rxd_pch_check_status(void);

/*===========================================================================
FUNCTION wl1_rxd_pch_calculate_next_mode
         
DESCRIPTION
  This function is to be called by drx just before UE goes to sleep in order 
  to caloculate the wakeup time for next PICH read. It uses the input values 
  to decide on the next mode according to systems design.

DEPENDENCIES
  None

RETURN VALUE
  rxd_pch_mode_enum_type: The RxD mode for the next time UE wakes up for PICH read.

SIDE EFFECTS
  None
===========================================================================*/
extern rxd_pch_mode_enum_type wl1_rxd_pch_calculate_next_mode(uint8 pi_val, uint16 filt_ecio, boolean no_resp_to_page, boolean s_crit_check);

/*===========================================================================
FUNCTION wl1_rxd_pch_wait_for_sleep
         
DESCRIPTION
  This function is to be called by drx before calculating the wakeup time for 
  next PICH read. It ensures that Div chain teardown is complete.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern void wl1_rxd_pch_wait_for_sleep(void);

/*===========================================================================
FUNCTION wl1_rxd_pch_teardown_proc_handler
         
DESCRIPTION
  Signal handler for RxD teardown in PCH/IDLE

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern void wl1_rxd_pch_teardown_sig_handler(uint32 signal);

/*===========================================================================
FUNCTION wl1_rxd_is_div_agc_cmd_needed
         
DESCRIPTION
  API to be called by modules other than RxD to determine whether Div chain 
  start AGC command has to be sent

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern boolean wl1_rxd_is_div_agc_cmd_needed(void);

/*===========================================================================
FUNCTION wl1_rxd_pch_get_pisum_thresh
         
DESCRIPTION
  API returns pi_sum threshold read from NV

DEPENDENCIES
  None

RETURN VALUE
  int32: pi_sum threshold for Fast Diversity

SIDE EFFECTS
  None
===========================================================================*/
extern int32 wl1_rxd_pch_get_pisum_thresh(void);

/*===========================================================================
FUNCTION wl1_rxd_pch_update_lockmode
         
DESCRIPTION
  API releases Div fingers if Div chain is seen as Out-of-lock

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern void wl1_rxd_pch_update_lockmode(rxd_pch_imbalance_check_enum_type imbalance_check_type);

/*===========================================================================
FUNCTION wl1_rxd_pch_rxd_out_of_lock
         
DESCRIPTION
  RxD lock status

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern boolean wl1_rxd_pch_rxd_out_of_lock(void);

/*===========================================================================
FUNCTION wl1_rxd_pch_wakeup_div_chain
         
DESCRIPTION
  This function is to be called when UE wakes up for PICH, after enabling 
  primary clocks, if RxD is to be enabled for this wake cycle. If tune was 
  not done previously, this function makes sure Div chain is enabled and tuned.
  If RxD mode is Fast Diversity, this function initiates the SSBI write to put 
  Div chain in low power mode.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern void wl1_rxd_pch_wakeup_div_chain(boolean enable_rf , boolean call_tune_api);

/*===========================================================================
FUNCTION wl1_rxd_pch_notify_new_sleep_cycle

DESCRIPTION
This function is called by wl1sleep when ue goes to sleep in PCH_SLEEP state 
after calculating next wakeup time. Rxd gets sleep_interval_written_sc and 
reserves Diversity chain at wakeup if next idle mode is Full diversity.

DEPENDENCIES
None

RETURN VALUE
None

SIDE EFFECTS
None
===========================================================================*/
extern void wl1_rxd_pch_notify_new_sleep_cycle(void);

/*===========================================================================
FUNCTION wl1_rxd_local_command_trace_func
         
DESCRIPTION
  This function is to be called when any module posts a RxD State Update Local
  Command. This is to maintain a Trace history of Local Commands.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern void wl1_rxd_local_command_trace_func(uint8 l1_state, 
                  wl1_rxd_debug_enum_type module_posted, uint16 curr_cfn, uint16 client);

/*===========================================================================
FUNCTION wl1_rxd_pch_full_div
         
DESCRIPTION
API to check if rxd is in full diverity mode.  

DEPENDENCIES
  None

RETURN VALUE
Return true only when idle rxd mode is full diversity and rxd is not set foff for trm. 

SIDE EFFECTS
  None
===========================================================================*/
extern boolean wl1_rxd_pch_full_div(void);

/*===========================================================================
FUNCTION rxd_is_active

DESCRIPTION

DEPENDENCIES

RETURN VALUE
  Returns boolean indicating whether Diversity Chain is active or not

SIDE EFFECTS
  None
===========================================================================*/
extern boolean rxd_is_active(void);

/*===========================================================================
FUNCTION rxd_swap_rx_chains_at_entry_of_traffic_state

DESCRIPTION
  API called at entry to traffic state. If requested earlier, the
  Rx chains will be swapped.

DEPENDENCIES

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern void rxd_swap_rx_chains_at_entry_of_traffic_state(void);

/*===========================================================================
FUNCTION rxd_save_rxd_ctrl_from_nv

DESCRIPTION
       This reads the RxD value saved in NV.
       
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern void rxd_save_rxd_ctrl_from_nv( void );

/*===========================================================================
FUNCTION rxd_init

DESCRIPTION
       This resets each of the variables for RxD controller, Needs to be called when
       entering idle mode
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern void rxd_idle_init(void);

/*===========================================================================
FUNCTION rxd_state_update

DESCRIPTION
  This function is called every frame to make the necessary state transitions and required changes
  needed to enable/disable the RxD feature

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

extern void rxd_state_update(void);

/*===========================================================================
FUNCTION rxd_config_update

DESCRIPTION
  This will be called by L1M or other modules for RxD state machinery needs to
  be kicked in to operation. It will perform the following:
  (a) If required, enable or disable the event on the sequencer's RX timeline
  (b) Set the necessary triggers

DEPENDENCIES

RETURN VALUE
  Whether able to set the forced off flag or not

SIDE EFFECTS
  None
===========================================================================*/

extern boolean rxd_config_update(void);

/*===========================================================================
FUNCTION rxd_set_signalling_trigger

DESCRIPTION
  This function sets signalling trigger
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern void rxd_set_signalling_trigger(void);

/*===========================================================================
FUNCTION rxd_set_forced_off

DESCRIPTION
  This is called to set the flag for forcibly disabling the RxD in the next state update

DEPENDENCIES
None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

extern void rxd_set_forced_off(rxd_div_chain_resource_client_type client_id, boolean foff, rxd_foff_cb_type cb_func, boolean blind_foff);

/*===========================================================================
FUNCTION wl1_rxd_bring_rf_out_of_lpm_in_cpc_drx

DESCRIPTION
  This function determines if RF out of LPM is reqd. for RxD mode of operation

DEPENDENCIES
None

RETURN VALUE
None

SIDE EFFECTS
  None
===========================================================================*/
extern void wl1_rxd_bring_rf_out_of_lpm_in_cpc_drx(void);


#ifdef FEATURE_WCDMA_ANT_SWITCH_SUPPORT
/*===========================================================================
FUNCTION rxd_set_for_asd

DESCRIPTION
  This is called by switch antenna module to turn ON/OFF RxD

DEPENDENCIES

RETURN VALUE
  Boolean: if the request is accepted or not

SIDE EFFECTS
  None
===========================================================================*/
extern boolean rxd_set_for_asd(boolean rxd_on);

/*===========================================================================
FUNCTION rxd_is_asd_supported

DESCRIPTION
  API used to return true if RxD can be enabled for AS Diversity in current
  band

DEPENDENCIES

RETURN VALUE
  boolean

SIDE EFFECTS
  None
===========================================================================*/
extern boolean rxd_get_rxd_for_as_div(void);
/*===========================================================================
FUNCTION rxd_get_nv_value

DESCRIPTION
  API used to return the value of the RxD nv item
DEPENDENCIES

RETURN VALUE
  uint8

SIDE EFFECTS
  None
===========================================================================*/
extern uint8 rxd_get_ctrl_nv_value(void);
#endif

/*===========================================================================
FUNCTION rxd_local_cb_cmd

DESCRIPTION
  This will process the local L1 command posted by hardware modules (RF/MDSP) and call
  the next suitable function based on index of RxD's hardware callback processor

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern void rxd_local_cb_cmd(void);

/*===========================================================================
FUNCTION rxd_cap_meas_config_update

DESCRIPTION
  This function updates the configuration factors to be utilized for 
  capacity related measurements

DEPENDENCIES
None

RETURN VALUE
None

SIDE EFFECTS
  None
===========================================================================*/
extern void rxd_cap_meas_config_update(l1_sf_enum_type sf_type, uint8 n_pilot, uint8 num_rl);

/*===========================================================================
FUNCTION rxd_cap_meas_update

DESCRIPTION
  This function updates the capacity related measurement 

DEPENDENCIES
None

RETURN VALUE
None

SIDE EFFECTS
  None
===========================================================================*/

extern void rxd_cap_meas_update(uint32 g, uint32 h);

/*===========================================================================
FUNCTION rxd_qual_meas_update

DESCRIPTION
  This function updates the quality related measurement 

DEPENDENCIES
None

RETURN VALUE
None

SIDE EFFECTS
  None
===========================================================================*/

extern void rxd_qual_meas_update(uint8 num_tbs, uint8 num_pass);

/*===========================================================================
FUNCTION rxd_rxdpm_meas_update
         
DESCRIPTION
  To Be Filled

DEPENDENCIES
  None

RETURN VALUE
  TRUE if everything is already OFF, FALSE otherwise

SIDE EFFECTS
  None
===========================================================================*/
extern void rxd_rxdpm_meas_update(boolean* scch_valid, boolean* scch_demod_attempted);

#if defined(FEATURE_WPLT) || defined(FEATURE_WCDMA_PLT_MODE)
/*===========================================================================
FUNCTION rxd_enable

DESCRIPTION
  This is for PLT support to forcibly enable the RxD mode of operation

DEPENDENCIES
None

RETURN VALUE
None

SIDE EFFECTS
  None
===========================================================================*/
extern void rxd_enable(void);

/*===========================================================================
FUNCTION rxd_disable

DESCRIPTION
  This is for PLT support to forcibly disable the RxD mode of operation

DEPENDENCIES
None

RETURN VALUE
None

SIDE EFFECTS
  None
===========================================================================*/

extern void rxd_disable(void);

#endif /* FEATURE_WPLT */

/*===========================================================================
FUNCTION rxd_meas_logging_update

DESCRIPTION
  This function logs the rxd meas info 
DEPENDENCIES

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

void rxd_meas_logging_update(void);

/*===========================================================================
FUNCTION rxd_submit_meas_log_pkt

DESCRIPTION
  This function submits rxd logging 
DEPENDENCIES

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void rxd_submit_meas_log_pkt(void);

/*===========================================================================
FUNCTION rxd_submit_ard_log

DESCRIPTION
  Called when the local command is processed, submits the ARD log packet.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern void rxd_submit_ard_log(void);

/*===========================================================================
FUNCTION rxd_submit_ard_v2_log

DESCRIPTION
  Called when the local command is processed, submits the ARD log packet.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern void rxd_submit_ard_v2_log(void);

/*===========================================================================
FUNCTION rxd_hho_save_imbalance_info

DESCRIPTION
  saves lockmode params during HHO. Will be retrieved if we go back to old config.

DEPENDENCIES

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void rxd_hho_save_imbalance_info(void);

/*===========================================================================
FUNCTION        RXD_UPDATE_EVENT_HANDLER

DESCRIPTION     This function handles the RxD state update event.

DEPENDENCIES    

RETURN VALUE    None

SIDE EFFECTS    
===========================================================================*/
extern void rxd_state_update_event_handler(void);

/*===========================================================================
FUNCTION        RXD_EVALUATE_STATE_UPDATE_EVENT

DESCRIPTION     This function handles the RxD state update event.

DEPENDENCIES    

RETURN VALUE    None

SIDE EFFECTS    
===========================================================================*/
extern void rxd_evaluate_state_update_event(void);

/*===========================================================================
FUNCTION rxd_configure
         
DESCRIPTION
.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern void rxd_configure(uint8 call_type_mask);

/*===========================================================================
FUNCTION rxd_fach_cleanup
         
DESCRIPTION
.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void rxd_fach_cleanup(void);

/*===========================================================================
FUNCTION rxd_dch_cleanup
         
DESCRIPTION
.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern void rxd_dch_cleanup(void);

/*===========================================================================
FUNCTION rxd_deallocate_rxlm_buffer
         
DESCRIPTION
.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void rxd_deallocate_rxlm_buffer(void);

/*===========================================================================
FUNCTION rxd_manage_rxlm_buffer
         
DESCRIPTION
.This function allocates/deallocates RXD rxlm buffers for every wakeup/sleep
when w is in DS mode.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern void rxd_manage_rxlm_buffer(boolean allocate);

/*===========================================================================
FUNCTION rxd_register_hs_callback
         
DESCRIPTION
  This function registers the HS callback function.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

extern void rxd_register_hs_callback(
  /* Callback function pointer */
  RXD_HS_RXD_TRANS_IND_CB_FUNC_TYPE *hs_cb);

/*===========================================================================
FUNCTION rxd_deregister_hs_callback
         
DESCRIPTION
  This function deregisters the HS callback function.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

extern void rxd_deregister_hs_callback(void);

/*===========================================================================
FUNCTION rxd_is_off
         
DESCRIPTION
  This function is used to check if RxD is already OFF or it is pending to 
  go off. Primarily going to be used for CM when checking if TDM table
  can be changed.

DEPENDENCIES
  None

RETURN VALUE
  TRUE if everything is already OFF, FALSE otherwise

SIDE EFFECTS
  None
===========================================================================*/
extern boolean rxd_is_off(void);

/*===========================================================================
FUNCTION wl1_rxd_api_oride_cmd

DESCRIPTION
  API used to call RxD APIs through QXDM command 

DEPENDENCIES
  None

RETURN VALUE
  Return value of API called

SIDE EFFECTS
  None
===========================================================================*/
extern boolean wl1_rxd_api_oride_cmd(uint8 api_identifier, uint32 input_arg_1, uint32 input_arg_2, 
  uint32 input_arg_3, uint32 input_arg_4);

/*===========================================================================
FUNCTION rxd_register_cm_cb

DESCRIPTION
  Registeres the RxD callback with CM

DEPENDENCIES

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern void rxd_register_cm_cb(void);

/*===========================================================================
FUNCTION wl1_rxd_fach_hs_rach_status
         
DESCRIPTION
  API called by HS to inform RxD of HS-RACH status

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern void wl1_rxd_fach_hs_rach_status(boolean status);

/*===========================================================================
FUNCTION rxd_dsds_status_update

DESCRIPTION
  This function is called by L1 DSDS controller before WCDMA gets the 
  START_WCDMA command from RRC. 
  If DSDS status is TRUE, it clears NV related variables to ensure that RxD 
  cannot be enabled in this mode.
  If DSDS status is FALSE, it resets the NV related variables to reflect the
  value read from NV 3851
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern void rxd_dsds_status_update(boolean dsds_state);

/*===========================================================================
FUNCTION wl1_rxd_get_rxd_reason_bitmask
         
DESCRIPTION
  Returns the rxd reason bitmask
 
DEPENDENCIES
  None

RETURN VALUE
  uint32

SIDE EFFECTS
  None
===========================================================================*/
extern uint32 wl1_rxd_get_rxd_reason_bitmask(void);

/*===========================================================================
FUNCTION wl1_rxd_get_rxdpm_reason_bitmask

DESCRIPTION
gives the rxdpm reason bitmask to other modules.

DEPENDENCIES
None

RETURN VALUE
uint8

SIDE EFFECTS
None
===========================================================================*/
extern uint8 wl1_rxd_get_rxdpm_reason_bitmask(void);

/*===========================================================================
FUNCTION wl1_rxd_get_ard_reason_bitmask

DESCRIPTION
gives the ard reason bitmask to other modules.

DEPENDENCIES
None

RETURN VALUE
uint8

SIDE EFFECTS
None
===========================================================================*/
extern uint8 wl1_rxd_get_ard_reason_bitmask(void);

/*===========================================================================
FUNCTION rxd_client_update_done_srch

DESCRIPTION
Checks for search client bitmask and updates the client bitmask
This informs rxd that search has completed it operation

DEPENDENCIES
None

RETURN VALUE
None

SIDE EFFECTS
None
===========================================================================*/
extern void rxd_client_update_done_srch(boolean srch_return);

/*===========================================================================
FUNCTION rxd_client_update_done_tri

DESCRIPTION
Checks for triage client bitmask and updates the client bitmask
This informs rxd that triage has completed it operation

DEPENDENCIES
None

RETURN VALUE
None

SIDE EFFECTS
None
===========================================================================*/
extern void rxd_client_update_done_tri(boolean tri_return);

/*===========================================================================
FUNCTION wl1_rxd_skip_short_cycle_cpc_drx

DESCRIPTION
Indicates to CPC-DRX if a short sleep cycle should be skipped

DEPENDENCIES
None

RETURN VALUE
None

SIDE EFFECTS
None
===========================================================================*/
extern boolean wl1_rxd_skip_short_cycle_cpc_drx(void);

/*===========================================================================
FUNCTION wl1_rxd_skip_cpc_drx

DESCRIPTION
Indicates to CPC-DRX if all cycles should be skipped

DEPENDENCIES
None

RETURN VALUE
None

SIDE EFFECTS
None
===========================================================================*/
extern boolean wl1_rxd_skip_cpc_drx(void);

/*===========================================================================
FUNCTION wl1_rxd_supported

DESCRIPTION
returns true if rxd is supported on the device
DEPENDENCIES
None

RETURN VALUE
boolean

SIDE EFFECTS
None
===========================================================================*/
extern boolean wl1_rxd_supported(void);

/*===========================================================================
FUNCTION wl1_rxd_ctrl_cmd_handler

DESCRIPTION
Handler for local cmd - L1_RXD_CTRL_CMD

DEPENDENCIES
None

RETURN VALUE
None

SIDE EFFECTS
None
===========================================================================*/
void wl1_rxd_ctrl_cmd_handler(wl1_rxd_ctrl_cmd_type *cmd_ptr);
/*===========================================================================
FUNCTION wl1_rxd_qta_state_update

DESCRIPTION
Gets information of whether we are in a QTA gap from wltrm 

DEPENDENCIES
None

RETURN VALUE
uint8

SIDE EFFECTS
None
===========================================================================*/
extern void wl1_rxd_qta_state_update(boolean in_gap);

/*===========================================================================
FUNCTION wl1_rxd_timer_set

DESCRIPTION
sets timers for various dsdx senarios including qta gaps

DEPENDENCIES
None

RETURN VALUE
None

SIDE EFFECTS
None
===========================================================================*/
void wl1_rxd_timer_set(wl1_rxd_tmr_rsn_enum_type state_enum);

/*===========================================================================
FUNCTION wl1_set_pch_oride_val

DESCRIPTION
API to allow update of the pch overide value from dsdx/trm 

DEPENDENCIES
None

RETURN VALUE
None

SIDE EFFECTS
None
===========================================================================*/
extern void wl1_set_pch_oride_val(uint8 set);
/*===========================================================================
FUNCTION rxd_release_div_chain

DESCRIPTION
releases the secondary chain and forces rxd off

DEPENDENCIES
None

RETURN VALUE
None

SIDE EFFECTS
None
=============================================================================*/
extern void rxd_release_div_chain(boolean freeze_filters, rxd_div_chain_resource_client_type client_mask);

/*===========================================================================
FUNCTION wl1_rxd_set_trm_relinquish_cb()

DESCRIPTION
Remembers the TRM callback that is to be called when WCDMA relinquishes the
diversity antenna

DEPENDENCIES
None

RETURN VALUE
None

SIDE EFFECTS
None
===========================================================================*/
extern void wl1_rxd_set_trm_relinquish_cb(rxd_trm_foff_cb_type cb_func);

/*===========================================================================
FUNCTION rxd_client_update_done_srch

DESCRIPTION
Checks for search client bitmask and updates the client bitmask
This informs rxd that search has completed it operation

DEPENDENCIES
None

RETURN VALUE
None

SIDE EFFECTS
None
===========================================================================*/
extern void rxd_client_update_done_srch(boolean srch_return);

/*===========================================================================
FUNCTION rxd_client_update_done_tri

DESCRIPTION
Checks for triage client bitmask and updates the client bitmask
This informs rxd that triage has completed it operation

DEPENDENCIES
None

RETURN VALUE
None

SIDE EFFECTS
None
===========================================================================*/
extern void rxd_client_update_done_tri(boolean tri_return);

/*===========================================================================
FUNCTION wl1_rxd_dsda_omrd_enable

DESCRIPTION
If OMRD is nto enabled for DSDA then do not allow RxD to come ON. 
Set RxD to FOFF and release the secondary antenna 

DEPENDENCIES
None

RETURN VALUE
None

SIDE EFFECTS
None
===========================================================================*/
void wl1_rxd_dsda_omrd_enable(boolean dsda_enabled);

/*===========================================================================
FUNCTION wl1_rxd_dsds_qta_enable

DESCRIPTION
IF we do not want rxD enabled for DSDS mode with QTA, 
then we disable RxD by way of NV. 


DEPENDENCIES
None

RETURN VALUE
None

SIDE EFFECTS
None
===========================================================================*/
void wl1_rxd_dsds_qta_enable(boolean dsds_enabled);

/*===========================================================================
FUNCTION wl1_pch_rxd_check_tune_pending

DESCRIPTION
Informs if a tune is pending for RxD or not in PCH mode


DEPENDENCIES
None

RETURN VALUE
None

SIDE EFFECTS
None
===========================================================================*/
extern boolean wl1_pch_rxd_check_tune_pending(void);

/*===========================================================================
FUNCTION wl1_rxd_is_omrd_disabled_for_dsda

DESCRIPTION
Informs if rxd has been disabled for DSDA mode


DEPENDENCIES
None

RETURN VALUE
None

SIDE EFFECTS
None
===========================================================================*/
extern boolean wl1_rxd_is_omrd_disabled_for_dsda(void);

/*===========================================================================
FUNCTION wl1_rxd_is_omrd_freeze_flag_set

DESCRIPTION
Informs if OMRD has activated the filter freeze flag


DEPENDENCIES
None

RETURN VALUE
Boolean: True if freeze flag is set; Else False

SIDE EFFECTS
None
===========================================================================*/
boolean wl1_rxd_is_omrd_freeze_flag_set(void);

/*===========================================================================
FUNCTION wl1_rxd_supported

DESCRIPTION
Informs if RxD can be enabled in the current build


DEPENDENCIES
None

RETURN VALUE
Boolean: True if nv and band support is present; Else False

SIDE EFFECTS
None
===========================================================================*/
extern boolean wl1_rxd_supported(void);

/*===========================================================================
FUNCTION wl1_rxd_acq_bch_failure

DESCRIPTION
  API called by wl1m amd dlbcchmgr to inform RxD that:
    BCH demod has failed; or
    Squal calculation has resulted in failure; or
    CRC errors in SIB decoding exceeded threshold
  In all these cases Rxd needs to come on

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern void wl1_rxd_acq_bch_failure(void);

/*===========================================================================
FUNCTION rxd_acq_bch_cleanup

DESCRIPTION
  Cleanup RxD variables and carry over RxD state if needed

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern void rxd_acq_bch_cleanup(void);

/*===========================================================================
FUNCTION wl1_rxd_trm_unlock_action

DESCRIPTION
Takes action based on trm unlock request


DEPENDENCIES
None

RETURN VALUE
void

SIDE EFFECTS
None
===========================================================================*/
extern void wl1_rxd_trm_unlock_action(trm_unlock_event_enum_t event);

/*===========================================================================
FUNCTION  wl1_rxd_in_div_only_mode

DESCRIPTION
Returns True if rxd is in diveristy only mode


DEPENDENCIES
None

RETURN VALUE
boolean Returns True if rxd is in diveristy only mode

SIDE EFFECTS
None
===========================================================================*/
extern boolean wl1_rxd_in_div_only_mode(void);

/*===========================================================================
FUNCTION     WL1_RXD_UNLOCK_CALLBACK

DESCRIPTION
  WRM calls this function when TRM requests RxD to release the secondary antenna.

DEPENDENCIES
  None.

PARAMETERS

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
extern boolean wl1_rxd_unlock_callback(wrm_unlock_event_enum_type event);


/*===========================================================================
FUNCTION     WL1_RXD_GRANT_CALLBACK

DESCRIPTION
  WRM calls this function when RxD receives control of the secondary antenna.

DEPENDENCIES
  None.

PARAMETERS
  boolean      grant - Whether ther grant was given or not


RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
extern void wl1_rxd_grant_callback(boolean grant);

/*===========================================================================
FUNCTION  wl1_rxd_is_tune_pending

DESCRIPTION
Returns TRUE if pch tune pending flag is set.


DEPENDENCIES
None

RETURN VALUE
void

SIDE EFFECTS
None
===========================================================================*/
boolean wl1_rxd_is_tune_pending(void);


/*===========================================================================
FUNCTION  wl1_rxd_wait_for_agc_tune_done

DESCRIPTION
wait to prevent pn search from being issued before FW 
returns with AGC done

DEPENDENCIES
None

RETURN VALUE
void

SIDE EFFECTS
None
===========================================================================*/
void wl1_rxd_wait_for_agc_tune_done(void);

/*===========================================================================
FUNCTION  wl1_rxd_wait_for_agc_cfg_done

DESCRIPTION
wait to prevent pn search from being issued before FW 
returns with AGC done

DEPENDENCIES
None

RETURN VALUE
void

SIDE EFFECTS
None
===========================================================================*/
void wl1_rxd_wait_for_agc_cfg_done(void);

/*===========================================================================
FUNCTION  wl1_rxd_handle_mdsp_agc_done

DESCRIPTION
Indication that agc tune is done, signal L1M

DEPENDENCIES
None

RETURN VALUE
void

SIDE EFFECTS
None
===========================================================================*/
void wl1_rxd_handle_mdsp_agc_done(void);

/*===========================================================================
FUNCTION  wl1_rxd_off_for_trm

DESCRIPTION
Returns if RxD is foff for TRM


DEPENDENCIES
None

RETURN VALUE
boolean to query if RxD is foff for TRM

SIDE EFFECTS
None
===========================================================================*/
boolean wl1_rxd_off_for_trm(void);

/*===========================================================================
FUNCTION wl1_rxd_pch_attempt_div
         
DESCRIPTION
API to check if rxd is in full diverity mode and if the secondary chain is 
granted. If the next mode is FULL DIVERSITY and rxd receives the grant upon 
request inform Sleep that RxD will turn ON.

Used in ISR context. 

DEPENDENCIES
  None

RETURN VALUE
Return true only when idle rxd mode is full diversity and rxd has successfully 
aquired the lock for the secondary chain

SIDE EFFECTS
  None
===========================================================================*/
boolean wl1_rxd_pch_attempt_div(void);

/*===========================================================================
FUNCTION rxd_record_buffer_deallocation

DESCRIPTION
  This function records CFN at which RxD buffer was deallocated

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern void rxd_record_buffer_deallocation(uint8 client);

/*===========================================================================
FUNCTION rxd_record_buffer_allocation

DESCRIPTION
  This function records CFN at which RxD buffer was allocated

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern void rxd_record_buffer_allocation(uint8 client);
/*===========================================================================
FUNCTION   rxd_rf_is_asleep

DESCRIPTION
Indication that agc teardown is done for RxD

DEPENDENCIES
None

RETURN VALUE
boolean

SIDE EFFECTS
None
===========================================================================*/
extern boolean rxd_rf_is_asleep(void);
/*===========================================================================
FUNCTION   wl1_rxd_skip_wrm_request_resources_for_secondary

DESCRIPTION
Indication that if request resource for secodary is need to skip

DEPENDENCIES
None

RETURN VALUE
boolean

SIDE EFFECTS
None
===========================================================================*/
extern boolean wl1_rxd_skip_wrm_request_resources_for_secondary(void);

/*===========================================================================
FUNCTION   wl1_rxd_band_supported_on_div

DESCRIPTION
Check if band supported on diversity.

DEPENDENCIES
None

RETURN VALUE
boolean

SIDE EFFECTS
None
===========================================================================*/
extern boolean wl1_rxd_band_supported_on_div(l1_freq_scan_band_enum_type band);

#endif

#endif /* RXDIV_V_H */
