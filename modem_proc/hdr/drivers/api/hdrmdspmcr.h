#ifndef HDRDSPMCR_H
#define HDRDSPMCR_H

/*===*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                         H D R    D S P    M A C R O S


GENERAL DESCRIPTION

EXTERNALIZED FUNCTIONS

REGIONAL FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS

  Copyright (c) 2000-2014 by Qualcomm Technologies, Incorporated.
  All Rights Reserved.


*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

============================================================================

                        EDIT HISTORY FOR MODULE

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/hdr/drivers/api/hdrmdspmcr.h#1 $ $DateTime: 2016/12/13 07:58:05 $ $Author: mplcsds1 $

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

when         who     what, where, why
----------   ---     ------------------------------------------------------- 
12/16/2014   vlc     Read Power log buffer data from FW 
11/19/2014   wsh     Disallow DTX if TxPwr is too high 
10/22/2013   wsh     Changes to support BOLT bring up
06/26/2013   wsh     Added Selective TxD feature
05/23/2013   rmg     Removed featurization around usage of FW const for decob
                     block size.
05/01/2013   ljl     Saved and restored Dyn CPD info.
04/16/2013   smd     Used FW const for decob block size.
01/30/2013   ljl     Added HDRMDSP_GET_MSTR_RTC_OFFSET_CX8() and 
                     HDRMDSP_GET_TX_RTC_OFFSET_CX8(). 
01/10/2013   vlc     Added HDRMDSP_GET_TRKPILOT_POS_CX2(). 
11/30/2012   wsh     Enable NLB on DIME
11/27/2012   smd     Supported log packets of FlexConn and DTX.
08/29/2012   ljl     Added RTC Offset valid support.
06/06/2012   kss     Updated for Dime.
03/08/2012   vlc     Removed shift operation in HDRLOG_GET_PER_SLOT_G_ASP macro. 
                     Changed demod_decision to eq_status.
03/08/2012   wsh     Merged NLB support
02/11/2012   smd     Added new fields in DTX and FlexConn log packets.
11/11/2011   arm     HDR idle and access state diversity implemetation.
10/17/2011   rmg     Replaced obsolete clk APIs with DAL APIs.
10/12/2011   lyl     Removed the obsoleted header file clk.h.
09/21/2011   vlc/lyl Added MC gRice /cset promotion.
09/07/2011   lyl     Added SNR rank setting macros.
06/13/2011   rmg     Updated RxAGC read macros to reflect changes in FW struct.
04/26/2011   lyl     Added HDRDRC_TUNE_AWAY_PRE_ADJ_STORE for svlte type 2.
03/15/2011   ljl     Added HDRFLL_IS_FLL_CPD_TRACK_MODE().
01/21/2011   rmg     Fixed RX / TX CARRIER_BUFF_VALID macros.
01/06/2011   vlc     Added support for FAC gain calculation under feature 
                     FEATURE_HDR_GRICE.
06/29/2010   kss     Fix for logging gaps and Rev 0 RAB macro.
06/20/2010   kss     Restore min/max power flag support for Power logging.
03/25/2010   pxu     Removed FEATURE_HDR_EQ feature checks and 
                             FEATURE_HDRSRCH_RBT feature checks 
03/17/2010   kss     Removed obsolete macros, support tx log buffer index.
02/20/2010   kss     Removed obsolete logging macros.
01/05/2010   kss     Removed obsolete macros.
12/09/2009   ljl     Removed hdrmdsp_type.h. 
11/06/2009   etv     Read from decode_header as 2d array instead of 1d array.
11/07/2009   kss     Remove test framework change bug.
10/24/2009   kss     Revert removal of some macros to aid integration.
10/07/2009   kss     Even more updates for fw interface.
09/29/2009   kss     More updates for fw interface.
09/21/2009   etv     Updated HDRMDSP_GET_FW_DEC_HEADER_N to use block index
                     and header index.
09/16/2009   etv     Updated fw register access for interface change.
07/29/2009   etv     Removed obsolete macros HDRHSTR_SET_COUNT, 
                     HDRHSTR_SET_SCC_OFFSET, HDRHSTR_SET_CC_START
                     HDRHSTR_GET_CC_START, HDRHSTR_GET_SCC_OFFSET
07/22/2009   ljl     Removed HDRDRC_GET_FRAME_OFFSET.
07/20/2009   etv     Updated HDRMDSP_GET_FW_DEC_HEADER_N to read from shared
                     memory interface.
07/09/2009   lyl     Removed obsolete interface (logCarrierValid)
07/06/2009   lyl     Fixed the compilation issue in RUMI build.
07/06/2009   kss     Use test framework changes in 9x00 build (temp).
06/30/2009   lyl     Updated for RxAGC unlocked mode interface.
06/23/2009   etv     Fixed Lint warnings.
06/12/2009   ljl/lyl Updated for firmware MSGR interface.
05/22/2009   etv     Removed obsolete macros.
03/20/2009   zg      Add interface for TX Beta Gain.
03/16/2009   wsh     Fixed lint warnings
03/08/2009   pmk     Added support for QTF
03/19/2009   etv     Read from MSB instead of LSB for RPC logging.
03/06/2009   ljl     Removed HDRDRC_GET_ASP_INDEX_FOR_DRC_COVER.
12/23/2008   etv     ByPass RCL Adjust to predictor SINR when ASP out of lock.
02/18/2009   rmg     Fixed HDRRAB_RAB_IS_SET macro.
01/27/2009   kss     Add interface for T2P bias.
01/09/2009   etv     Added HDRLOG_SET/CLEAR_DEMOD_CARRIER_VALID.
12/12/2008   ljl     Reset rclPredAdj when overriding basp with trigger mode.
12/04/2008   rmg     Corrected HDRAGC_OVERRIDE_AND_ENABLE_RPC_MC.
12/04/2008   rmg     Added HDRAGC_OVERRIDE_AND_ENABLE_RPC_MC.
12/04/2008   rmg     Added support to enable/disable ACI mitigation.
11/24/2008   rmg     Lint fixes.
11/24/2008   lyl     Added DRC filter support.
11/10/2008   Vish    Added macros for wideband saturation detection.
10/26/2008   rmg     Added HDRAGC_OVERRIDE_TX_OPEN_LOOP_MC.
10/13/2008   kss     Lint fixes.
10/07/2008   lyl     Added interface HDRFING_GET_ANT_PHASE for phase info
09/18/2008   ljl     Fixed the compiler warning.
08/27/2008   etv     Fixed bug with programming decoder interrupt rate.
07/08/2008   rmg     Support to clear closed loop adjust on new carriers.
06/30/2008   rmg     Fixed handling of negative TrafficToPilotPeak values.
06/26/2008   rmg     Added support to reset filterd transmit pilot power.
06/18/2008   etv     Added macros to enable/disable DTXMode
06/12/2008   rmg     Added support to write Tx carriers ordered by ranks.
06/11/2008   cc/etv  Added macros for MaxOptionalPayloadSize, 
                     MaxOptionalDataRate and AuxDRCCover.
05/23/2008   kss     Add/update macros for BetaP logging, tx limiting.
05/16/2008   rmg     Changes to support FW image upgrade to remove 
                     TxLimitingMin/Max from per-carrier structure.
05/16/2008   rkc     Used multicarrier struct for DRC filter output and
                     filtered best Active Set Pilot SNR
05/14/2008   etv     Use right mDSP register for ACK Mode macros.
04/09/2008   ljl     Added macros to activate and deactivate ASP.
04/02/2008   rmg     Unstubbed DRC supervision macro.
03/08/2008   etv     Fixed bug due to operator precedence.
03/20/2008   ljl     Updated the Rev B macros.
03/06/2008   etv     DSP gives Log timestamp in slots insteads of half-slots.
                     Un-Stub DRCBoostLength macro.
03/06/2008   jyw     Added support to the hdr search unit test platform.
03/05/2008   kss     Changed OVERRIDE_RPC to not touch tx closed loop
                     if RPC is already enabled.
02/15/2008   cc      Uncommented HDRFING_FINGER_IS_LOCKED_MC macro.
01/30/2008   etv     Read both words of BestASPSINR.
01/30/2008   ljl     Updated HDRMPC_GET_RXTX_CMP_CNT().
01/22/2008   ljl     Renamed HDR_throttleDisable to HDR_txThrottleDisable in fw.
01/22/2008   rmg     Added GET_TX_PILOT_POWER_MC and SET_TX_TOTAL_MAX_BACKOFF.
01/16/2008   ljl     Fixed HDRFLL_OVERRIDE_ROT().
01/15/2008   kss     Fix typo in HDRAGC_GET_TX_TOTAL_POWER_MC()
01/14/2008   etv     Using SNR and SNRFilt instead of RakeSNR and RakeSNRFilt.
01/08/2008   etv     Read both words from HDRLOG_GET_PER_SLOT_EQ_BEST_C2I.
12/19/2007   kss     Added Tx AGC macros.
12/19/2007   etv     Added HDRDRC_GET_RCL_PRED_ADJ_HIGH and 
                     HDRDRC_GET_RCL_PRED_ADJ_LOW
12/26/2007   jyw     Fixed the typo.
12/21/2007   sb      Added HDREQ_GET_EQ_DEMOD_SELECT() and 
                     HDREQ_GET_EQ_ENABLED() macros.
12/18/2007   rkc     Updated for mdsp fll multi-carrier changes
12/18/2007   jyw     Added revB MSTR tracking info macros.
12/18/2007   ljl     Moved ActualDrc to TxAGC structure.
12/10/2007   Vish    - Reverted back the changes made to HDRAGC_SET_RX[0/1]_AGC_ACC()
                       on 11/15/2007.
                     - Fixed .CarrierMode for HDRAGC_ENABLE_RX[0/1]_AGC_MC_OVERRIDE().
                     - Fixed .AntennaMode for HDRAGC_ENABLE_RX[0/1]_AGC_OVERRIDE().
12/05/2007   etv     Added support for mDSP slot logging
12/03/2007   ljl     Added macros to override DSC and ACK.
12/03/2007   jyw     Added HDRASP_SET_SCHD_ID and fixed SET_LINK_ID.
11/26/2007   ljl     Added HDRASP_SET_PHY_CARRIER_INDEX() and 
                     HDRMDSP_SET_CC_DEMOD_CARRIER().
11/20/2007   rmg     Added support for Tx logging.
11/16/2007   ljl     Added HDRASP_SET_RL_MAC_INDEX() and 
                     HDRASP_SET_RAB_MAC_INDEX().
11/15/2007   Vish    - Added HDRAGC_DISABLE_RX[0/1]_CARRIER(), and
                       HDRAGC_[EN/DIS]ABLE_RX[0/1]_AGC_MC_OVERRIDE()
                     - Fixed HDRAGC_SET_RX[0/1]_AGC_ACC()
                     - Removed auto-enable of carrier 0 from HDRAGC_ENABLE_RX0_AGC()
11/11/2007   kss     7800 integration -- removed stubbed macros.
10/29/2007   ljl     Updated for the firmware release 0016.
10/20/2007   kss     Added HDRAGC_DISABLE_RPC() macros.
10/18/2007   ljl     Changed EQ to equalizers in equalizer macros.
09/27/2007   ljl     Updated predictor and handoff macros.
09/27/2007   rmg     Added carrier enable shift and mask defines.
09/25/2007   rmg     Added channel enable/disable macros.
09/21/2007   etv     Updated RAB macros to mult-carrier version.
08/20/2007   ljl     Updated for multi-carrier.
07/20/2007   ljl     Set ForwardTrafficMACIndex and updated DRC force.
07/18/2007   rmg     AGC macro changes
07/16/2007   jyw     Added FEATURE_HDR_MDSP_TX_LINEARIZERS_USE_1X_MEMORY.
07/07/2007   ljl     Added HDRAGC_ENABLE_RX0_CARRIER() and forced DRC value.
05/07/2007   ljl     Supported firmware that split mode configuration 
                     into multiple registers.
05/07/2007   ljl     Changed stubbed register address.
05/01/2007   jyw     Added AFC macros for QSC60X5.
04/20/2007   ljl     Added HDRFING_GET_RTC_OFFSET_X8_HI() and
                     HDRFING_GET_RTC_OFFSET_X8_LO().
04/18/2007   rmg     Updated MAC channel info macros.
04/18/2007   ljl     Added macros for diversity mode and antenna.
04/17/2007   jyw     Added RevB ASP macros.
04/17/2007   rmg     Added MAC logging interrupt macros,
                     Updated HDRMDSP_SET_TRAFFIC_MODE().
04/13/2007   kss     RevB macro changes - RxAGC.
04/11/2007   ljl     Corrected HDRDRC_PREDICTOR_CTRL_MICRO().
04/11/2007   ljl     Redefined BIT().
04/10/2007   mt      Added macro to get the current RL frame offset.
04/09/2007   rmg     RevB macro changes - MAC
04/02/2007   ljl     Added system mode macros.
04/02/2007   ljl     Stubbed DRC and ASP macros to compile.
04/02/2007   ljl     Added equalizer macros.
04/05/2007   etv     Updated for Rev-B firmware changes to aid compilation.
03/31/2007   rmg     Added RevB encoder macros.
03/28/2007   ljl     Stubbed macros for the undefined firmware registers.
03/19/2007   ljl     Added RevB finger macros.
03/19/2007   ljl     Used mdsp_read() and mdsp_write().
01/20/2007   rmg     Added HDRMDSP_GET_TX_PWR_ macros.
02/02/2007   ljl     Renamed HDRFING_SET_PREAMBLE_THRESH to be 
                     HDRMDSP_SET_PREAMBLE_THRESH.
01/15/2007   jyw     Added XO related macros.
01/08/2007   etv     Added macros to read slice<n>HSTRCount.
01/04/2007   kss     Added macros for setting tx power margin up/down step.
12/22/2006   grl/etv Added macros for improved partial loading performance.
12/07/2006   kss     Added macro for setting the DRCLock backoff low SINR 
                     threshold value.
12/05/2006   kss     Added macros for configuring PA headroom algorithm.
11/30/2006   grl     Added macro for determining the current DRC.
11/13/2006   kss     Added macros for setting txT2Pmin and for clearing RL 
                     interlaces.
10/20/2006   kss     Removed macros for delaying Rx AGC enable; added macro 
                     for setting HSTR; added MPC command status check.
10/17/2006   ljl     Added rotator override.
10/13/2006   kss     Added macro for delaying Rx AGC enable.
10/10/2006   kss     Added macro to send AGC write command.
10/03/2006   grl     Added support for HDRDRC_TUNE_AWAY_DIV_TRANS.
09/14/2006   mt      Deleted macros for forced preamble detection status.
08/31/2006   kss     Added macro for clearing RAB.
08/25/2006   kss     Added code review fixes for last check-in.
08/23/2006   kss     Added macros for DAST AGC algorithm.
08/21/2006   mt      Added macro to tell whether firmware used forced premable det.
08/04/2006   ljl     Added HDRFLL_GET_FLL_ACCUM_LOAD().
07/27/2006   mt      Added macro for stopping forced preamble detection.
06/28/2006   jyw     Added macros for MPC.
06/30/2006   etv     Added macros for setting Handoff related registers.
06/30/2006   etv     Added a macro to toggle MUP Decode Filter
06/28/2006   kss     Added macro for tx log buffer.
06/14/2006   sq      Added macros for setting AGC acq parameters
06/14/2006   grl     Removed obsoleted MDSP HW error code.
06/13/2006   grl     Changed DRC enable macro mask to enable RCL-A+
06/01/2006   sq      Added macro to support forced preamble detection
06/05/2006   kss     Added macro for HDR Tx power backoff (PA backoff).
05/22/2006   ljl     Added macros to set PLL gain and Th5.
05/15/2006   etv     Added macro to set MinContinuationSpan
04/18/2006   ds      Changes to HDR MDSP logging macros.
03/30/2006   etv     Replaced macros drc/dscGainBoostLog0 with rlLog1Buf<n>.
             etv     Added macros for DRC/DRCBoostLength & ChannelGainBoost.
03/07/2006   ds      Added macro to read dynamic cross product mode.
02/22/2006   kss     Added HDRAGC_SET_RF_PN_ROLL_DELAY().
02/07/2006   kss     Added HDRFING_FINGER_IS_LOCKED().
01/30/2006   ljl     Added support of HDR_txAgcHwWriteEnable.
12/08/2005   kss     Added macros for freezing RAB and disabling RL throttle.
12/07/2005   kss     Modified RL early termination macro as per MDSP change.
11/18/2005   grl     Added macros for RCL ramp down during div ta.
11/15/2005   kss     Changed tx adjust log register name.
11/14/2005   kss     Changed value for HARQ Mode 0 (based in systems input).
11/11/2005   etv     Setting Bit 14 as well to enable Revised RCL
11/09/2005   sq      Added HDRMDSP_SET_MMSEINV
11/01/2005   kss     Added RL early termination macros.
10/18/2005   kss     Added macros for Rev A digital gains.
10/18/2005   jyw     Added macros for the dec control.
10/13/2005   ds      Fixed DSC Gain boost macro
10/13/2005   ds      Added macros to log DRC, DSC Gain boosts
10/05/2005   ds      Modified EQ Device select and override macros
10/04/2005   kss     Updated register used in HDRLOG_GET_MAX_RRI_LOG.
10/04/2005   sq      Added read/write of CC_START DSP variable
09/28/2005   ljl     Added macro for RA channel gain
09/28/2005   ljl     Added macros for QRAB and FRAB
09/10/2005   ds      Added equalizer device selection algorithm macros
09/06/2005   ljl     Added two macros for sync state optimization.
09/06/2005   ds      Added macros for rotator based acq and Slew rate limiting
09/06/2005   ds      Cleaned Rotator and RVAFC macros
09/06/2005   ds      Modified macros for FLL Accum capture and restore
08/25/2005   kss     Added ARQ and packet status values for RL macros.
08/17/2005   etv     Fixed bug in HDRMDSP_DRC_TRANS_REG_SIZE macro.
07/28/2005   ljl     Corrected the macro HDRASP_GET_MEAN_RPC
07/25/2005   ds      Modified macros for FLL Accum capture and restore
07/22/2005   etv     Changed HDRDRC_SET_TRAFFIC_MAX_DRC to take a parameter 
                     and removed HDRDRC_SET_IDLE_MAX_DRC.
07/05/2005   ds      Added generic MDSP logging support macros
06/30/2005   kss     Added support for changing H-ARQ mode.
06/27/2005   ds      Updated Equalizer override macro to latest FW
06/23/2005   etv     Added support for NullRateDRC38.4 attribute. 
06/23/2005   sq      Added HDRFING_SET_PREAMBLE_THRESH
06/21/2005   kss     Removed the sign inversion of FRAB.
06/21/2005   etv     Added macros for enabling/disabling CC3/4 Mac indices.
06/21/2005   etv     Added support for DRC Translation Offset.
06/09/2005   ds      Corrected rotator disable macro and eq lms step masks
06/07/2005   sq      Added HDRDRC_GET_DRC_LENGTH
06/06/2005   ds      Updated equalizer register for averaged slot-based C/I
06/03/2005   kss     Added macros to support Auxiliary Pilot.
05/27/2005   ljl     Added macro for QRAB.
05/16/2005   etv     Changed macro names for enabling/disabling revA mode to
                     Set Demod modes.
05/13/2005   etv     Added macros for enabling/diabling RevA mode in mDSP
                     Forward link.
05/04/2005   etv     Added Macros for checking and refreshing the DSC Timing
                     mDSP registers.
04/15/2005   ds      Added Equalizer macros under FEATURE_HDR_EQ.
04/14/2005   etv     Added new Rev-A specific Macros.
04/13/2005   sq      Merged macros for controlling the HDR FLL from 6500.
04/04/2005   kss     Added HDRASP_SET_DSC().
03/31/2005   kss     Added several new macros for decoder, tx settings.
02/22/2005   sq      Mainlined FEATURE_HDR_DRC_FILTERING
02/10/2005   kss     Added macros for logging reverse link channel gains.
01/25/2005   ds      Merged changes for multiuser packet mode enable
11/10/2004   ds      Added multiuser packet mode enable macro
10/19/2004   ds      Added Short packet enable macro
04/13/2004   kss     Added HDRAGC_GET_PA_RANGE_STATE().
02/04/2004   sq      Supported FEATURE_HDR_DRC_FILTERING
01/07/2004   kss     Added AGC override enable/disable macros.
12/03/2003   sq      Added macro to set Tx open loop agc value
11/18/2003   aaj     Added macro to enable Merge Prevent logic. Write BASP FL
10/22/2003   aaj     Corrected FLL PDM Save/Restore. Added force FLL lock
11/03/2003   remi    Removed obsolete MDSP logging interface.
                     Removed obsolete MDSP initialization macros.
10/22/2003   aaj     Added macros to save and restore FLL PDM
10/06/2003   aaj     Mainlined T_MSM6500 code
09/17/2003   kss     Support for version 40 of DSP (some names changed).
08/29/2003   sq      Removed HDRAGC_DISABLE_DEMOD/HDRAGC_ENABLE_DEMOD for
                     T_MSM6500. They are obsolete in MSM6500
07/22/2003   sq      Corrected some MACRO definitions for MSM6500
                     ( system time/DSP timer related )
06/27/2003   kss     Corrected offsets for DSP arrays (now in words to match
                     DSP address file)
06/02/2003   aaj     Added macro to set varDrcPred during tune-back to hdr
03/16/2003   sq      Ported to MSM6500(Jaguar) DSP interface
03/11/2003   aaj     Added macros for getting and setting SCC offset
02/14/2003   kss     Added macros for missed preamble interrupt control.
11/26/2002   aaj     Added macro to set FL ASP
07/24/2002   aaj     Added macros to read and reset ASP handoff count
07/23/2002   aaj     Read tx pilot power from HDR_txPilotPowerBy2
07/01/2002   aaj     Correct GET_ASP_INDEX_FOR_DRC_COVER macro
07/01/2002   aaj     Correct GET_DRC_COVER macro where bit mask was incorrect
04/24/2002   kss     Use new DSP register for Tx pilot power.
06/13/2002   aaj     Add support for reading hw error code on halt
04/24/2002   aaj     Added macro to check demod in progress flag setting
03/03/2002   kss     Added several macros to support diversity LNA settings.
03/01/2002   aaj     Support for diversity combining mode selection in DSP
02/26/2002   ajn     Fixed search abort macro. Added '01,'02 to copyright
02/21/2002   kss     Support for mac logging and Rx1 LNA parameters.
02/19/2002   aaj     Support for new mac logging & ASP rpc index for 0x0050
02/08/2002   sst     Added srch abort macros
01/31/2002   ajn     Added MDSP Sleep support macros.
12/20/2001   aaj     Add Timer support
12/07/2001   aaj     Added diversity finger lock bit reading support
11/29/2001   aaj     Add antenna RSSI macro
11/16/2001   kss     Added channel gain read macros.
11/15/2001   aaj     Added ASP log buffer and drclock macros
11/11/2001   kss     Added more test mode macros.
11/08/2001   om      Added macro to find ASP index for DRC Cover
11/07/2001   kss     Added macros for setting/checking max power.
10/23/2001   aaj     Changed names for LNA registers for 0x0034 f/w build
10/08/2001   aaj     Changed names for two DRC registers
09/25/2001   kss     Added macros for changing ACK modes.
09/13/2001   aaj     Changed finger RSSI to be read from finger PathRSSI
09/13/2001   aaj     Added macros for RABLen and RABOff
09/12/2001   kss     Added macros for maxDRC.
09/06/2001   kss     Added macros for DRC lock interface and modeOverride.
09/05/2001   aaj     Added finger diversity macros
08/02/2001   aaj     Added macros for PN ROM fix control.
07/21/2001   kss     Added sequence macros.
06/22/2001   aaj     Fixed DSP bug in idle handoff by changing SET_DRC macro
06/22/2001   kss     Added yet more macros (byte quota).
06/07/2001   kss     Added DRC enable/disable macros.
04/23/2001   kss     Added several more AGC macros.
04/12/2001   aaj     Fixed HDR Searcher Macros
04/10/2001   kss     Added AGC-related macros.
03/29/2001   aaj     Total change for TAZ3 firmware support, dude.
11/22/2000   ajn     Code review changes
10/31/2000   ajn     Un-corrected off by 1 error in ASP macros.
10/20/2000   ajn     Corrected off by 1 error in ASP macros.
10/02/2000   aaj     Added handoff control registers and ASP handoff regs
09/07/2000   aaj     Corrected one error in ASP macro
07/18/2000   aaj     Corrected comments/Added code review comments
07/15/2000   aaj     Added ASP/FINGER macros. Compiled successfully.
07/14/2000   aaj     Created this file

==========================================================================*/


/*==========================================================================

                     INCLUDE FILES FOR MODULE

==========================================================================*/

#include "hdr_variation.h"
#include "customer.h"
#include "comdef.h"
#include "DALSys.h"
#include "hdrdebug.h"
#include "hdrmdsp.h"
#include "hdrfw_msg.h"

/*==========================================================================

                     MACRO DEFINITIONS FOR MODULE

This section contains local definitions for constants, macros, typesdefs,
and other items needed by this module.

==========================================================================*/

#if defined (TEST_FRAMEWORK)
#error code not present
#elif (!defined HDRSRCH_UT_PLATFORM )
//#ifndef HDRSRCH_UT_PLATFORM
//#define DSP_READ(port)           (* (port) )
//#define DSP_WRITE(port, val)     (* (port) ) = ((word) (val))
//#define MDSP_READ32(port)        (*((volatile dword *) (port) ))
//#define MDSP_WRITE32(port, val)  (*((volatile dword *) (port) ) = ((dword) (val)))

/* For MSM6500, the DSP interface file provides absolute DSP reg addresses */

/* Using the macros defined in mdspext.h */
//#define MDSP_READ( reg )        mdsp_read( reg )
//#define MDSP_WRITE( reg, val )  mdsp_write( (reg), (val) )

/* ***TEMP*** UNDEF FOR BRINGUP */
#define MDSP_READ( reg )        /*lint -save -e506*/0/*lint -restore*/
#define MDSP_WRITE( reg, val )  /*lint -save -e506 -e774*/if((val)){}/*lint -restore*/

#define MDSP_READ32(port)        /*lint -save -e506*/0/*lint -restore*/
#define MDSP_WRITE32(port, val)  /*lint -save -e506 -e774*/if((val)){}/*lint -restore*/

#else
#error code not present
#endif  /* HDRSRCH_UT_FLATFORM */

#ifdef BIT
#undef BIT
#endif
#define BIT( b )                 ( 1 << b )


#define HDR_SLOTS_PER_FRAME     16
#define HDR_HSLOTS_PER_FRAME    32

#define HDRMDSP_DEMOD_CARRIER_0 0

#define HDRMDSP_CARRIER_RANK_MASK         0x00FF
#define HDRMDSP_CARRIER_COMPOSITION_MASK  0x0030
#define HDRMDSP_CARRIER_ENABLE_SHFT       0xF
#define HDRMDSP_CARRIER_ENABLE_MASK       0x8000


/*=======================================================================
 MACRO          : HDRMDSP_STUB

 DESCRIPTION    : This macro prints out the stub message.

 Return Value   : None

 =========================================================================*/
#define HDRMDP_STUB_STR( str )  str " is stubbed"

#define HDRMDSP_STUB( str ) \
                HDR_MSG_SRCH( MSG_LEGACY_HIGH, HDRMDP_STUB_STR( str ) )

extern uint16 hdrmdsp_stub_var;
  /* Variable used as stubbed registers */

#define HDRMDSP_STUB_REG_ADDR  ( &hdrmdsp_stub_var )

/*************************************************************************
 *                                                                       *
 *                        DSP Status Macros                              *
 *                                                                       *
 *************************************************************************/


/*=======================================================================
 MACRO          : HDRMDSP_VERSION

 DESCRIPTION    : This macro returns the firmware version running in the
                  modem DSP.

 Return Value   : F/W version

 =========================================================================*/

#define HDRMDSP_VERSION()           \
        ( hdrmdsp_shared_mem_interface->read.general_status.fwVersion )


/*=======================================================================
 MACRO          : HDRMDSP_HALT_CODE

 DESCRIPTION    : This macro returns the HDR Modem DSP halt status

 INPUTS         : None

 Return Value   : Halt code
 =========================================================================*/

#define HDRMDSP_HALT_CODE()           \
       ( hdrmdsp_shared_mem_interface->read.general_status.dspHalt )


/*************************************************************************
 *                                                                       *
 *                        MDSP Interrupt Macros                          *
 *                                                                       *
 *************************************************************************/

/*=======================================================================
 MACRO          : HDRMDSP_READ_INT_MASK

 DESCRIPTION    : This macro reads the MDSP interrupt mask.

 INPUTS         : None

 Return Value   : Interrupt Mask
 =========================================================================*/
#define HDRMDSP_READ_INT_MASK()  MDSP_READ( HDR_intEnable )

/*=======================================================================
 MACRO          : HDRMDSP_INT_ENABLE

 DESCRIPTION    : This macro sets the MDSP interrupt mask. Note that there
                  is only one HW Interrupt between the MSM and the MDSP.
      It is multiplexed between different originations of the
      interrupt source from the DSP.

 INPUTS         : Interrupt Mask

 Return Value   : None
 =========================================================================*/

#define HDRMDSP_INT_ENABLE( bit )  \
         MDSP_WRITE( HDR_intEnable,  HDRMDSP_READ_INT_MASK() | BIT((bit)) )


/*=======================================================================
 MACRO          : HDRMDSP_INT_DISABLE

 DESCRIPTION    : This macro sets the MDSP interrupt mask. Note that there
                  is only one HW Interrupt between the MSM and the MDSP.
              It is multiplexed between different originations of the
              interrupt source from the DSP.

 INPUTS         : Interrupt Mask

 Return Value   : None
 =========================================================================*/

#define HDRMDSP_INT_DISABLE( bit )  \
        MDSP_WRITE( HDR_intEnable, \
                     HDRMDSP_READ_INT_MASK() & ~(uint16)(BIT((bit))) )


/*************************************************************************
 *                                                                       *
 *                        MDSP system mode config macros                 *
 *                                                                       *
 *************************************************************************/

#define HDRMODE_DEMOD_CARRIERS_MASK               0x0003

#define HDRMODE_HYPERSPACE_MASK                   0x8000
#define HDRMODE_HYPERSPACE_SHIFT                  15

#define HDRMODE_RELEASE_MASK                      0x0003
#define HDRMODE_RELEASE_SHIFT                     0
#define HDRMODE_RELEASE_REV_A                     0x1

#define HDRMODE_MODULATOR_MASK                    0x8000
#define HDRMODE_MODULATOR_SHIFT                   15

#define HDRMODE_DIVERSITY_MASK                    0x8000
#define HDRMODE_DIVERSITY_SHIFT                   15

#define HDRMODE_ANTENNA_MASK                      0x0001
#define HDRMODE_ANTENNA_SHIFT                     0

#define HDRMODE_SW_STATE_MASK                     0x0000000F

#define HDRMDSP_ANT_0  0
  /* Antenna 0 */

#define HDRMDSP_ANT_1  1
  /* Antanna 1 */

#define HDRMDSP_NON_DIV 0
  /* Non-diversity mode */

#define HDRMDSP_DIV     1
  /* Diversity mode */




/*=======================================================================
 *                                                                      *
 *                        Power Collapse Macros                         *
 *                                                                      *
 =======================================================================*/

#define HDRMPC_CMD_ISR              HDR_CMD_INTERRUPT 

/*=======================================================================
 MACRO          : HDRMPC_RD_RTC_CMD

 DESCRIPTION    : The Macro issues the RTC read command to the mdsp.

 INPUTS         : None
 =========================================================================*/

#define HDRMPC_RD_RTC_CMD()         MDSP_WRITE32( HDR_mpcCmd, 0x80000000 )

/*=======================================================================
 MACRO          : HDRMPC_WR_RTC_CMD

 DESCRIPTION    : The command issues the RTC write command to the mdsp.

 INPUTS         : None
 =========================================================================*/

#define HDRMPC_WR_RTC_CMD()         MDSP_WRITE32( HDR_mpcCmd, 0x40000000 )

/*=======================================================================
 MACRO          : HDRMPC_WR_AGCc_CMD

 DESCRIPTION    : This command issues the AGC write command to the mdsp.
                  The mdsp will write the contents of rxAgcOverrideValue0/1
                  to the Rx_AGCc_HDR_VALUE register in the Rx Front.

 INPUTS         : None
 =========================================================================*/

#define HDRMPC_WR_AGC0_CMD()        MDSP_WRITE32( HDR_mpcCmd, 0x20000000 )
#define HDRMPC_WR_AGC1_CMD()        MDSP_WRITE32( HDR_mpcCmd, 0x10000000 )

/*=======================================================================
 MACRO          : HDRMPC_GET_MPC_CMD_STATUS

 DESCRIPTION    : The command read the command status register.

 INPUTS         : None

 Return Value   : The status of the mpc command.
 =========================================================================*/

#define HDRMPC_GET_MPC_CMD_STATUS() MDSP_READ32( HDR_mpcCmd )


/*=============================================================================
 MACRO          : HDRMPC_WAIT_MPC_CMD_CLEAR

 DESCRIPTION    : This will wait for an MPC command to clear, indicating
                  that it has been executed by the MDSP.  Very few iterations
                  are expected.
                  If the max specified iterations have executed without the 
                  command clearing, the macro code will exit.
                  
 INPUTS         : loop_count: An int variable, not a value or expression.
                              Variable is used to return status.

 OUTPUTS        : loop_count != 0 : Success. Command completed.
                  loop_count == 0 : Failure. Command did not complete.
                                    (possible DSP error)
                  
 =========================================================================*/
#define HDRMPC_MAX_MPC_WAIT_LOOP 30

#define HDRMPC_WAIT_MPC_CMD_CLEAR( loop_count )                     \
  loop_count = HDRMPC_MAX_MPC_WAIT_LOOP;                            \
  while(( loop_count > 0) && ((HDRMPC_GET_MPC_CMD_STATUS()) != 0 )) \
  {                                                                 \
    DALSYS_BusyWait(1);                                             \
    loop_count--;                                                   \
  }


/*=======================================================================
 MACRO          : HDRMPC_GET_RTC_CNT

 DESCRIPTION    : The command latches the RTC value.

 INPUTS         : None

 Return Value   : RTC cnt in chipx8.
 =========================================================================*/

#define HDRMPC_GET_RTC_CNT()        MDSP_READ32( HDR_mpcRtcCnt )

/*=======================================================================
 MACRO          : HDRMPC_GET_RXTX_CMP_CNT

 DESCRIPTION    : This macro latches the RXTX compare cnt.

 INPUTS         : None

 Return Value   : RTC cnt in chipx8.
 =========================================================================*/

#define HDRMPC_GET_RXTX_CMP_CNT()   MDSP_READ32( HDR_mpcRxTxCompareCnt )

/*=======================================================================
 MACRO          : HDRMPC_SET_RTC_CNT

 DESCRIPTION    : The macro set the RTC cnt in chipx8.

 INPUTS         : Desired RTC cnt in chipx8.
 =========================================================================*/

#define HDRMPC_SET_RTC_CNT( cnt )   MDSP_WRITE32( HDR_mpcRtcCnt, cnt )

/*=======================================================================
 MACRO          : HDRMPC_SET_RXTX_CMP_CNT

 DESCRIPTION    : The macro set the RXTX cnt

 INPUTS         : Desired RXTX compare count.
 =========================================================================*/

#define HDRMPC_SET_RXTX_CMP_CNT( cnt ) MDSP_WRITE32(HDR_mpcRxTxCompareCnt, cnt)



/*************************************************************************
 *                                                                       *
 *                         Searcher Macros                               *
 *                                                                       *
 *************************************************************************/

/* Bit masks for search control register bit fields */

#define HDRSRCH_CTRL_TA_MASK           BIT( 2 )
#define HDRSRCH_CTRL_CAP_BEFORE_MASK   BIT( 0 )
#define HDRSRCH_CTRL_CAP_AFTER_MASK    BIT( 1 )
#define HDRSRCH_ABORT_MASK             BIT( 15 )
#define HDRSRCH_NC_MASK                0xff00
#define HDRSRCH_NN_MASK                0x0030
#define HDRSRCH_SPLIT_MASK             BIT( 3 )
#define HDRSRCH_DUAL_MASK              BIT( 1 )
#define HDRSRCH_ANT_SEL_MASK           BIT( 0 )

#define HDRSRCH_TASK_BASE              HDR_srchTasks
#define HDRSRCH_TASK_SIZE              8 /* 4 words or 8 bytes */

/* Register offsets in bytes */
#define HDRSRCH_TASK_RTC_OFF           0
#define HDRSRCH_TASK_PN                2
#define HDRSRCH_TASK_WIN_SIZE          4
#define HDRSRCH_TASK_PARAMS            6

#define HDRSRCH_RESULT_BASE            HDR_srchResults
#define HDRSRCH_RESULT_SIZE            16 /* 8 words or 16 bytes */


/*************************************************************************
 *                                                                       *
 *                         ASP Macros                                    *
 *                                                                       *
 *************************************************************************/
/*--------------------------------*
 * ASP Related Mask Definitions   *
 *--------------------------------*/

/* ASP Lock Bit masks */
#define HDRASP_LOCK_SECT_MASK         BIT( 0 )
#define HDRASP_LOCK_CELL_MASK         BIT( 1 )
#define HDRASP_LOCK_DRCLOCK_MASK      BIT( 2 )


/*======================================================================
 MACRO          : HDRASP_ACTIVATE

 DESCRIPTION    : Activate an ASP

 INPUTS         : n  - ASP Index
                  c  - Demod Carrier Index 

 RETURN VALUE   : 
 =========================================================================*/

#define HDRASP_ACTIVATE( c, n ) \
          ( hdrmdsp_shared_mem_interface->write.asp_cfg.aspC[c][n].config = 0x8000 )


/*======================================================================
 MACRO          : HDRASP_DEACTIVATE

 DESCRIPTION    : Deactivate an ASP

 INPUTS         : n  - ASP Index
                  c  - Demod Carrier Index 

 RETURN VALUE   : 
 =========================================================================*/

#define HDRASP_DEACTIVATE( c, n ) \
          ( hdrmdsp_shared_mem_interface->write.asp_cfg.aspC[c][n].config = 0x0 )


/*======================================================================
 MACRO          : HDRASP_SET_RTC_OFFSET_VALID

 DESCRIPTION    : Set RTC Offset valid bit for an ASP

 INPUTS         : n  - ASP Index
                  c  - Demod Carrier Index 

 RETURN VALUE   : 
 =========================================================================*/

#define HDRASP_SET_RTC_OFFSET_VALID( c, n ) \
          ( hdrmdsp_shared_mem_interface->write.asp_cfg.aspC[c][n].config |= (BIT(14)) )


/*======================================================================
 MACRO          : HDRASP_CLEAR_RTC_OFFSET_VALID

 DESCRIPTION    : Clear RTC Offset valid bit for an ASP

 INPUTS         : n  - ASP Index
                  c  - Demod Carrier Index 

 RETURN VALUE   : 
 =========================================================================*/

#define HDRASP_CLEAR_RTC_OFFSET_VALID( c, n ) \
          ( hdrmdsp_shared_mem_interface->write.asp_cfg.aspC[c][n].config &= (~(uint16)(BIT(14))) )

/*======================================================================
 MACRO          : HDRASP_GET_LOCK

 DESCRIPTION    : Reads the lock status of an ASP

 INPUTS         : n  - ASP Index
                  c  - Demod Carrier Index 

 RETURN VALUE   : Lock Pilot State of ASP n of Demod Carrier c.
 =========================================================================*/

#define HDRASP_GET_LOCK( c, n ) \
        ( hdrmdsp_shared_mem_interface->                        \
          read.rmac_subframe_ind.rmacInfo[c].mac.lockFlags[n]      \
          & HDRASP_LOCK_SECT_MASK )

/*=======================================================================
 MACRO          : HDRASP_GET_FILT_SINR

 DESCRIPTION    : Reads Filtered SINR for an ASP

 INPUTS         : n  - ASP index
                  c  - Demod Carrier Index

 RETURN VALUE   : Filtered SINR of ASP n of Demod Carrier c.
 =========================================================================*/

#define HDRASP_GET_FILT_SINR( c, n ) \
        ( hdrmdsp_shared_mem_interface->read.general_status.C[c].snrFilt[n] )


/*======================================================================
 MACRO          : HDRASP_GET_QRAB_SOFT

 DESCRIPTION    : Reads QRAB

 INPUTS         : n   - ASP Index
                  c   - Demod Carrier Index


 RETURN VALUE   : QRAB soft of ASP n of Demod Carrier c.
 =========================================================================*/

#define HDRASP_GET_QRAB_SOFT( c, n ) \
        ( hdrmdsp_shared_mem_interface->   \
          read.rmac_subframe_ind.rmacInfo[c].mac.qrabSoft[n] )


/*======================================================================
 MACRO          : HDRASP_GET_FRAB_SOFT

 DESCRIPTION    : Reads FRAB

 INPUTS         : n   - ASP Index
                  c   - Demod Carrier Index
                  qrab - The setting value 

 RETURN VALUE   : FRAB soft of ASP n of Demod Carrier c.
 =========================================================================*/

#define HDRASP_GET_FRAB_SOFT( c, n ) \
        ( hdrmdsp_shared_mem_interface->   \
          read.rmac_subframe_ind.rmacInfo[c].mac.frabSoft[n] )


/*======================================================================
 MACRO          : HDRASP_SET_INIT_CTRL_Z

 DESCRIPTION    : Controls how FRABSoft and QRABSoft are initialized
                  Z=1: FRABSoft/QRABSoft = 0

 INPUTS         : n  - ASP Index
                  c  - Demod Carrier Index  
               
 RETURN VALUE   : None
 =========================================================================*/

#define HDRASP_SET_INIT_CTRL_Z( c, n )  \
          ( hdrmdsp_shared_mem_interface->write.asp_cfg.aspC[c][n].initCtl.Z = 1 )


/*======================================================================
 MACRO          : HDRASP_CLR_INIT_CTRL_Z

 DESCRIPTION    : Clear FRABSoft and QRABSoft init control Z.
                  Z=1: FRABSoft/QRABSoft = 0

 INPUTS         : n  - ASP Index
                  c  - Demod Carrier Index  
               
 RETURN VALUE   : None
 =========================================================================*/

#define HDRASP_CLR_INIT_CTRL_Z( c, n )  \
          ( hdrmdsp_shared_mem_interface->write.asp_cfg.aspC[c][n].initCtl.Z = 0 )


/*======================================================================
 MACRO          : HDRASP_SET_INIT_CTRL_F

 DESCRIPTION    : Controls how FRABSoft and QRABSoft are initialized
                  F=1: FRABSoft = max of FRAB over all in lock sectors, 
                  QRABSoft = 0

 INPUTS         : n  - ASP Index
                  c  - Demod Carrier Index  
               
 RETURN VALUE   : None
 =========================================================================*/

#define HDRASP_SET_INIT_CTRL_F( c, n )  \
          ( hdrmdsp_shared_mem_interface->write.asp_cfg.aspC[c][n].initCtl.F = 1 )

/*======================================================================
 MACRO          : HDRASP_CLR_INIT_CTRL_F

 DESCRIPTION    : Clear FRABSoft and QRABSoft init control F.
                  F=1: FRABSoft = max of FRAB over all in lock sectors, 
                  QRABSoft = 0

 INPUTS         : n  - ASP Index
                  c  - Demod Carrier Index  
               
 RETURN VALUE   : None
 =========================================================================*/

#define HDRASP_CLR_INIT_CTRL_F( c, n )  \
          ( hdrmdsp_shared_mem_interface->write.asp_cfg.aspC[c][n].initCtl.F = 0 )


/*=======================================================================
 MACRO          : HDRASP_SET_CELL_MAP

 DESCRIPTION    : Writes the CellMap index associated with the ASP

 INPUTS         : n  - ASP index
                  c  - Demod Carrier Index 
                  cellmap  - RPC cell map associated with the ASP

 RETURN VALUE   : None
 =========================================================================*/

#define HDRASP_SET_CELL_MAP( c, n, rpc_map ) \
          ( hdrmdsp_shared_mem_interface->write.asp_cfg.aspC[c][n].cellMap = ( rpc_map ) )


/*======================================================================
 MACRO          : HDRASP_SET_DRC_COVER

 DESCRIPTION    : Write DRC walsh cover for the ASP

 INPUTS         : n  - ASP Index
                  c  - Demod Carrier Index
                  cov - DRC Cover

 NOTE           : DRC Cover is only 3 bits long. We are shifting the ASP
                  Index into the upper 4 bits of the DRC cover so that the
                  handoff algorithm can differentitate between two different
                  ASP DRC covers in the IDLE state. IDLE state has drc cover
                  zero for all ASPs. This is a hack and needs to be fixed.
 =========================================================================*/

#define HDRASP_SET_DRC_COVER( c, n, cov ) \
          ( hdrmdsp_shared_mem_interface->write.asp_cfg.aspC[c][n].drcCover = ( cov ) )

/*======================================================================
 MACRO          : HDRASP_SET_AUX_DRC_COVER

 DESCRIPTION    : Write Aux DRC walsh cover for the ASP

 INPUTS         : n  - ASP Index
                  c  - Demod Carrier Index
                  cov - Aux DRC Cover

 RETURN VALUE   : None
 =========================================================================*/
#ifdef FEATURE_HDR_REVB_ENH_RATES

#define HDRASP_SET_AUX_DRC_COVER( c, n, cov ) \
          ( hdrmdsp_shared_mem_interface->write.asp_cfg.aspC[c][n].auxDrcCover = ( cov ) )

#else

/* AuxDRCCover is always set to zero when the feature is not defined to force
   only Rev-A rates */
#define HDRASP_SET_AUX_DRC_COVER( c, n, cov ) \
          ( hdrmdsp_shared_mem_interface->write.asp_cfg.aspC[c][n].auxDrcCover = 0 )

#endif /* FEATURE_HDR_REVB_ENH_RATES */


/*======================================================================
 MACRO          : HDRASP_SET_DSC

 DESCRIPTION    : Write DSC for the ASP

 INPUTS         : n   - ASP Index
                  c   - Demod Carrier Index
                  val - DSC for ASP

 RETURN VALUE   : None
 =========================================================================*/

#define HDRASP_SET_DSC( c, n, val ) \
          ( hdrmdsp_shared_mem_interface->write.asp_cfg.aspC[c][n].dsc = ( val ) )


/*======================================================================
 MACRO          : HDRASP_SET_RA_CHANNEL_GAIN

 DESCRIPTION    : Write RA channel gain for the ASP

 INPUTS         : n   - ASP Index
                  val - Linear value of RA channel gain for ASP
                  c   - Demod Carrier Index 

RETURN VALUE   : None
 =========================================================================*/

#define HDRASP_SET_RA_CHANNEL_GAIN( c, n, val ) \
          ( hdrmdsp_shared_mem_interface->write.asp_cfg.aspC[c][n].rabChannelGain = ( val ) )


/*======================================================================
 MACRO          : HDRASP_SET_LINK_ID

 DESCRIPTION    : Link Identifier to be copied to the decoder header for
                  packets received from this ASP.

 INPUTS         : n  - ASP Index
                  c  - Demod Carrier Index 

RETURN VALUE    : None
 =========================================================================*/

#define HDRASP_SET_LINK_ID( c, n, linkid )    \
          ( hdrmdsp_shared_mem_interface->write.asp_cfg.aspC[c][n].ID.linkID = (linkid) )

/*======================================================================
 MACRO          : HDRASP_SET_SCHD_ID

 DESCRIPTION    : Scheduler Group Identifier to be copied to the decoder 
                  header for packets received from this ASP.

 INPUTS         : n  - ASP Index
                  c  - Demod Carrier Index 

RETURN VALUE    : None
 =========================================================================*/
#define HDRASP_SET_SCHD_GID( c, n, sched_id ) \
          ( hdrmdsp_shared_mem_interface->write.asp_cfg.aspC[c][n].ID.groupID = (sched_id) )


/*======================================================================
 MACRO          : HDRASP_SET_MAC_INDEX

 DESCRIPTION    : MAC_INDEX is a new field in ASP structure

 INPUTS         : n  - ASP Index
                  c  - Demod Carrier Index 

RETURN VALUE    :  None
 =========================================================================*/

#define HDRASP_SET_MAC_INDEX( c, n, mac_index ) \
       ( hdrmdsp_shared_mem_interface->write.asp_cfg.aspC[c][n].flMACIndex = ( mac_index ) )

/*======================================================================
 MACRO          : HDRASP_SET_PHY_CARRIER_INDEX

 DESCRIPTION    : MAC_INDEX is a new field in ASP structure

 INPUTS         : n  - ASP Index
                  c  - Demod Carrier Index 

RETURN VALUE    :  None
 =========================================================================*/

#define HDRASP_SET_PHY_CARRIER_INDEX( c, n, carrier_index ) \
       ( (hdrmdsp_shared_mem_interface->write.asp_cfg.aspC[c][n]).carrier = ( carrier_index ) )


/*=======================================================================
 MACRO          : HDRASP_GET_DRC_LOCK

 DESCRIPTION    : Current Value of DRC lock bit used by the DSP

 INPUTS         : n  - ASP index
                  c  - Demod Carrier Index

 RETURN VALUE   : DRC lock of ASP n of Demod Carrier c.
 =========================================================================*/

#define HDRASP_GET_DRC_LOCK( c, n ) \
        ( ( (hdrmdsp_shared_mem_interface->                       \
             read.rmac_subframe_ind.rmacInfo[c].mac.lockFlags[n]  \
           & HDRASP_LOCK_DRCLOCK_MASK) != 0 ) ? 1 : 0 )


/*======================================================================
 MACRO          : HDRASP_SET_RAB_LENGTH

 DESCRIPTION    : This sets RAB length in the ASP struct

 INPUTS         : n      - ASP Index
                  rablen - RAB length
                  c      - Demod Carrier Index 

RETURN VALUE    : None
 =========================================================================*/

#define HDRASP_SET_RAB_LENGTH( c, n, rablen )  \
          ( hdrmdsp_shared_mem_interface->write.asp_cfg.aspC[c][n].rabLength = ( rablen ) )


/*======================================================================
 MACRO          : HDRASP_SET_RAB_OFFSET

 DESCRIPTION    : This sets RAB offset in the ASP struct

 INPUTS         : n      - ASP Index
                  raboff - RAB length
                  c      - Demod Carrier Index

RETURN VALUE    : None
 =========================================================================*/

#define HDRASP_SET_RAB_OFFSET( c, n, raboff ) \
          ( hdrmdsp_shared_mem_interface->write.asp_cfg.aspC[c][n].rabOffset = ( raboff ) )



#define HDRASP_SET_RL_MAC_INDEX( c, n, index ) \
          ( hdrmdsp_shared_mem_interface->write.asp_cfg.aspC[c][n].rlMACIndex = ( index ) )

#define HDRASP_SET_RAB_MAC_INDEX( c, n, index ) \
          ( hdrmdsp_shared_mem_interface->write.asp_cfg.aspC[c][n].rabMACIndex = ( index ) )



/*=======================================================================
 MACRO          : HDRASP_GET_MEAN_RPC

 DESCRIPTION    : Get filtered RPC hard decision.

 INPUTS         : c        - Carrier Index
                  cell_idx - Cell Index
 =========================================================================*/

#define HDRASP_GET_MEAN_RPC( c, cell_idx )   \
           (hdrmdsp_shared_mem_interface->read.log_buffer[0].C[c].RPCMean[cell_idx])


/*************************************************************************
 *                                                                       *
 *                         Handoff Macros                                    *
 *                                                                       *
 *************************************************************************/
/*--------------------------------*
 * Handoff Related Mask Definitions   *
 *--------------------------------*/

#define HDRMDSP_HO_OVERRIDE_MASK                BIT(15)
#define HDRMDSP_HO_OVERRIDE_RESET_PRED_ADJ_MASK BIT(13)
#define HDRMDSP_HO_OVERRIDE_TRIGGER_MODE_MASK   BIT(14)
#define HDRMDSP_HO_CHANGE_COUNT_RESET_MASK      BIT(15)
#define HDRMDSP_HO_LOCKED_MODE_MASK             BIT(15)
#define HDRMDSP_HO_REF_CARRIER_MASK             0x3
#define HDRMDSP_HO_BEST_ASP_DICT_MASK           0x7

/*=======================================================================
 MACRO          : HDRASP_GET_BASP_RL

 DESCRIPTION    : Get best ASP RL

 INPUTS         : c        - Carrier Index
 =========================================================================*/

/* TBD */
/* Firmware is going to export another register */
#define HDRASP_GET_BASP_RL( c )    \
        ( hdrmdsp_shared_mem_interface->read.general_status.C[c].bestASP )


/*=======================================================================
 MACRO          : HDRASP_HANDOFF_CTRL_DSP_MC

 DESCRIPTION    : DSP controls handoff.

 INPUTS         : c        - Carrier Index

 =========================================================================*/

#define HDRASP_HANDOFF_CTRL_DSP_MC( c )                              \
                   ( hdrmdsp_handoff_c[c].HandoffOverride = 0 )   \


/*=======================================================================
 MACRO          : HDRASP_OVERRIDE_BASP

 DESCRIPTION    : Set best ASP FL. There are two ways to set BestASP:
                   1. Trigger mode
                   2. Override mode.
                  This macro uses override mode. Override stays until 
                  software clear the override bit or set to trigger mode.

 INPUTS         : c        - Carrier Index
                  asp      - ASP index
 =========================================================================*/

#define HDRASP_OVERRIDE_BASP( c, asp )                              \
                    (hdrmdsp_handoff_c[c].BestASPDict     =  asp);  \
                    (hdrmdsp_handoff_c[c].HandoffOverride =         \
                                     HDRMDSP_HO_OVERRIDE_MASK)



/*=======================================================================
 MACRO          : HDRASP_GET_BASP_FL

 DESCRIPTION    : Get best ASP FL

 INPUTS         : c        - Carrier Index
 =========================================================================*/

#define HDRASP_GET_BASP_FL( c )                          \
        ( hdrmdsp_shared_mem_interface->read.general_status.C[c].bestASP )

/* ASP Variables for Equalizer */

/*=======================================================================
 MACRO          : HDRASP_SET_RTC_OFFSET

 DESCRIPTION    : Write RTC Offset to ASP n

 INPUTS         : n - ASP Index
                  rtc - Equalizer RTC offset
                  c - demod carrier

RETURN VALUE    : None
 =========================================================================*/

#define HDRASP_SET_RTC_OFFSET( c, n, rtc )  \
          ( hdrmdsp_shared_mem_interface->write.asp_cfg.aspC[c][n].eqRTCOfsCx2 =  \
          ( ( rtc ) & 0xffff ) )

/*=======================================================================
 MACRO          : HDRASP_SET_PN

 DESCRIPTION    : Assigns PN offset to ASP n

 INPUTS         : n - ASP Index
                  pn - PN Index of this ASP in 64 chip unit
                  c - Demod Carrier 

RETURN VALUE    : None
 =========================================================================*/

#define HDRASP_SET_PN( c, n, pn )  \
          ( hdrmdsp_shared_mem_interface->write.asp_cfg.aspC[c][n].pnIndex = \
          ( ( pn ) & 0xffff ) )

/*************************************************************************
 *                                                                       *
 *                      EQUALIZER Macros                                 *
 *                                                                       *
 * All the numbers are in QDSP word sizes                                *
 *************************************************************************/

/*-----------------------------------------------*
 * Equalizer Assignment Map Definitions  *
 *-----------------------------------------------*/

/* Equalizer assignment bit masks */
#define HDREQ_ASSIGN_ENABLE_MASK                BIT( 0  )
#define HDREQ_ASSIGN_ASSIGN_MASK                BIT( 15 )

/* The following masks are not used */
#define HDREQ_ASSIGN_FILT_RESET_MASK            BIT( 14 )
#define HDREQ_ASSIGN_ANTSEL_MASK                BIT( 10 )
#define HDREQ_ASSIGN_ANTDIV_MASK                BIT(  9 )

#define HDREQ_ASSIGN_ASP_MAP_MASK               0x3800
#define HDREQ_ASSIGN_LMS_STEP_MASK              0x0070
#define HDREQ_ASSIGN_FILT_LEN_MASK              0x000F

#define HDREQ_ASSIGN_MULTIPASS2_ENABLE_MASK     BIT( 12 )
#define HDREQ_ASSIGN_MULTIPASS2_LMS_STEP_MASK   0xE000
#define HDREQ_ASSIGN_MULTIPASS3_ENABLE_MASK     BIT(  8 )
#define HDREQ_ASSIGN_MULTIPASS3_LMS_STEP_MASK   0x0E00
#define HDREQ_ASSIGN_MULTIPASS_SELECT_MASK      0x0003

#define HDREQ_OVERRIDE_EQ_DEMOD_MASK            BIT( 15 )

#define HDREQ_ASSIGN_SIZE      (HDR_eq1AssignConfig       - HDR_eq0AssignConfig)
#define HDREQ_ASSIGN_BASE       HDR_eq0AssignConfig

#define HDREQ_ASSIGN_CFG       (HDR_eq0AssignConfig         - HDREQ_ASSIGN_BASE)
#define HDREQ_ASSIGN_MULTIPASS (HDR_eq0AssignMultiPassSteps - HDREQ_ASSIGN_BASE)
/*------------------------------*
 * Equalizer Status Definitions *
 *------------------------------*/
#define HDREQ_STATUS_SIZE              (HDR_eq1CI           - HDR_eq0CI)
#define HDREQ_STATUS_BASE               HDR_eq0CI

#define HDREQ_STATUS_CI                (HDR_eq0CI           - HDREQ_STATUS_BASE)
#define HDREQ_STATUS_CIFILT            (HDR_eq0CIFilt       - HDREQ_STATUS_BASE)
#define HDREQ_STATUS_MULTIPASS1_CI     (HDR_eq0CI_1         - HDREQ_STATUS_BASE)
#define HDREQ_STATUS_MULTIPASS1_CIFILT (HDR_eq0CIFilt_1     - HDREQ_STATUS_BASE)
#define HDREQ_STATUS_MULTIPASS2_CI     (HDR_eq0CI_2         - HDREQ_STATUS_BASE)
#define HDREQ_STATUS_MULTIPASS2_CIFILT (HDR_eq0CIFilt_2     - HDREQ_STATUS_BASE)


/* Equalizer status bit mask */
#define HDREQ_STATUS_EQ_ASSIGNED_MASK   0x00000001
#define HDREQ_STATUS_EQ_USED_MASK       0x80000000
#define HDREQ_STATUS_SELECTED_EQ_MASK   0x00070000


/*=======================================================================
 MACRO          : HDREQ_GET_DEMOD_DECISION

 DESCRIPTION    : Read the decision on whether equalizer or Rake was used for
                  demod in the current slot 
 
 INPUTS         : c  - Demod carrier
 
 RETURN VALUE   : Mode decision
 =========================================================================*/

#define HDREQ_GET_DEMOD_DECISION( c ) \
         ( ( ( hdrmdsp_shared_mem_interface->read.srch_dump_ind.srchDumpInfo[c].eq.status ) &   \
              HDREQ_STATUS_EQ_USED_MASK ) >> 31 )

/*=======================================================================
 MACRO          : HDREQ_GET_EQ_SELECT

 DESCRIPTION    : Read which equalizer was used for demod in the current 
                  slot. Invalid if Rake was used. 
  
 INPUTS         : c  - Demod carrier
 
 RETURN VALUE   : Selected equalizer
 =========================================================================*/

#define HDREQ_GET_EQ_SELECT( c ) \
          ( ( hdrmdsp_shared_mem_interface->read.srch_dump_ind.srchDumpInfo[c].eq.status &    \
              HDREQ_STATUS_SELECTED_EQ_MASK ) >> 16 )

/*=======================================================================
 MACRO          : HDREQ_GET_EQ_DEMOD_SELECT

 DESCRIPTION    : Read the decision on whether equalizer or rake was used 
                  for demod in the current slot 
 
 INPUTS         : c  - Demod carrier
 
 RETURN VALUE   : Mode decision
                  0 - Rake used
                  1 - Equalizer used
 =========================================================================*/

#define HDREQ_GET_EQ_DEMOD_SELECT( c ) \
        ( ( hdrmdsp_shared_mem_interface->read.srch_dump_ind.srchDumpInfo[c].eq.demodDecision & \
            HDREQ_STATUS_EQ_USED_MASK) >> 31 )

/*=======================================================================
 MACRO          : HDREQ_GET_EQ_ENABLED

 DESCRIPTION    : Read whether equalizer is enabled for demod. 
                    
 INPUTS         : c  - Demod carrier
 
 RETURN VALUE   : Equalizer on/off flag 
 =========================================================================*/

#define HDREQ_GET_EQ_ENABLED( c ) \
          ( ( hdrmdsp_shared_mem_interface->read.srch_dump_ind.srchDumpInfo[c].eq.status &    \
              HDREQ_STATUS_EQ_ASSIGNED_MASK ) )

/*=======================================================================
 MACRO          : HDREQ_GET_EQ_PASS_CI

 DESCRIPTION    : Reads the instantaneous C/I estimate of the specified 
                  equalizer

 INPUTS         : c  - Carrier
                  e  - Equalizer Index
                  0: EQ0 (31-tap), single pass
                  1: EQ1 (17-tap), first pass
                  2: EQ1 (17-tap), second pass
                  3: EQ1 (17-tap), third pass
                  4: EQ2 (17-tap), single pass

 RETURN VALUE   : C/I of equalizer e of carrier c
 =========================================================================*/

#define HDREQ_GET_EQ_PASS_CI( c, e )   \
        ( hdrmdsp_shared_mem_interface->read.srch_dump_ind.srchDumpInfo[c].eq.CIInfo[e].CI )

/*=======================================================================
 MACRO          : HDREQ_GET_EQ_PASS_CI_FILT

 DESCRIPTION    : Reads the filtered C/I estimate of equalizer e

 INPUTS         : c  - Carrier
                  e  - Equalizer Index
                  0: EQ0 (31-tap), single pass
                  1: EQ1 (17-tap), first pass
                  2: EQ1 (17-tap), second pass
                  3: EQ1 (17-tap), third pass
                  4: EQ2 (17-tap), sigle pass

 RETURN VALUE   : Filtered C/I of equalizer e
 =========================================================================*/

#define HDREQ_GET_EQ_PASS_CI_FILT( c, e )   \
        ( hdrmdsp_shared_mem_interface->read.srch_dump_ind.srchDumpInfo[c].eq.CIInfo[e].CIFilt )  

/*=======================================================================
 MACRO          : HDREQ_GET_EQ_PASS_CI_AVR

 DESCRIPTION    : Reads the averaged C/I estimate of equalizer e 
                  over two half slots.

 INPUTS         : c  - Carrier
                  e  - Equalizer Index
                  0: EQ0 (31-tap), single pass
                  1: EQ1 (17-tap), first pass
                  2: EQ1 (17-tap), second pass
                  3: EQ1 (17-tap), third pass
                  4: EQ2 (17-tap), sigle pass

 RETURN VALUE   : Filtered C/I of equalizer e
 =========================================================================*/

#define HDREQ_GET_EQ_PASS_CI_AVR( c, e )   \
        ( hdrmdsp_shared_mem_interface->read.srch_dump_ind.srchDumpInfo[c].eq.CIInfo[e].CIAvr )      

/*************************************************************************
 *                                                                       *
 *                      FINGER Macros                                    *
 *                                                                       *
 * All the numbers are in QDSP word sizes                                *
 *************************************************************************/

/*--------------------------------*
 * Finger Assignment Definitions  *
 *--------------------------------*/

/* Finger assignment bit masks */
#define HDRFING_ASSIGN_ENABLE_MASK      BIT( 0 )
#define HDRFING_ASSIGN_ASP_MASK         0x0700


/*----------------------------*
 * Finger Status Definitions  *
 *----------------------------*/

/* Finger status bit masks */
#define HDRFING_STATUS_ENABLE_MASK      BIT( 0 )
#define HDRFING_STATUS_LOCK_MASK        BIT( 1 )


/*------------------------------*
 * Finger lock flag Definitions *
 *------------------------------*/

/* Finger lock flag bit mask */
#define HDRFING_LOCKED_FLAG_MASK        0x8000

/*--------------------------*
 * Finger Assignment Macros *
 *--------------------------*/

/*=======================================================================
 MACRO          : HDRFING_ENABLE/DISABLE

 DESCRIPTION    : Enables/Disables finger f of carrier c

 INPUTS         : c   - Carrier
                  f   - Finger Index
 =========================================================================*/

#define HDRFING_ENABLE( c, f ) \
        ( hdrmdsp_shared_mem_interface->write.fing_assign.fingC[c][f].config.enable = 1 )

#define HDRFING_DISABLE( c, f ) \
        ( hdrmdsp_shared_mem_interface->write.fing_assign.fingC[c][f].config.enable = 0 )

/*=======================================================================
 MACRO          : HDRFING_SET_RTCX8

 DESCRIPTION    : Selects antenna for finger f of carrier c.

 INPUTS         : c   - Carrier
                  f   - Finger Index
 =========================================================================*/

#define HDRFING_SET_RTC_X8( c, f, rtcx8  ) \
                 ( hdrmdsp_shared_mem_interface->write.fing_assign.fingC[c][f].rtcOfs = ( rtcx8 ) & 0x3ffff )

/*=======================================================================
 MACRO          : HDRFING_SET_ASP_MAP

 DESCRIPTION    : Sets ASP index for finger f of carrier c

 INPUTS         : c       - Carrier
                  f       - Finger Index
                  asp_idx - ASP index
 =========================================================================*/

#define HDRFING_SET_ASP_MAP( c, f, asp_idx ) \
        ( hdrmdsp_shared_mem_interface->write.fing_assign.fingC[c][f].config.asp = ( asp_idx ) ) 

/*=======================================================================
 MACRO          : HDRFING_SET_INIT_SNR

 DESCRIPTION    : Sets initial finger SNR Estimate

 INPUTS         : c   - Carrier
                  f   - Finger Index
 =========================================================================*/

#define HDRFING_SET_INIT_SNR( c, f, snr ) \
        ( hdrmdsp_shared_mem_interface->write.fing_assign.fingC[c][f].initSnr = (snr) )
        

/*=======================================================================
 MACRO          : HDRFING_GET_RTC_OFFSET_X8

 DESCRIPTION    : Reads finger RTC offset. The register is 32-bit in RevB.

 INPUTS         : c  - Carrier
                  f  - Finger Index

 RETURN VALUE   : Reads RTC offset of the finger
 =========================================================================*/

#define HDRFING_GET_RTC_OFFSET_X8( c, f ) \
          ( hdrmdsp_shared_mem_interface->read.srch_dump_ind.srchDumpInfo[c].fing[f].rtcOfs & 0x3FFFF)

/*=======================================================================
 MACRO          : HDRFING_GET_ANT_RSSI

 DESCRIPTION    : Reads finger RSSI

 INPUTS         : c  - Carrier
                  f  - Finger Index

 RETURN VALUE   : Returns RSSI of the finger
 =========================================================================*/

#define HDRFING_GET_ANT_RSSI( c, f, a)  \
          ( ( hdrmdsp_shared_mem_interface->read.srch_dump_ind.srchDumpInfo[c].fing[f].antRssiFilt[a]  >> 16 ) & 0xFFFF )


/*=======================================================================
 MACRO          : HDRFING_GET_ANT_PHASE

 DESCRIPTION    : Reads finger phase info

 INPUTS         : c  - Carrier
                  f  - Finger Index
                  a  - Antenna
 RETURN VALUE   : Returns phase info of the finger
 =========================================================================*/

#define HDRFING_GET_ANT_PHASE( c, f, a)  \
          ( hdrmdsp_shared_mem_interface->read.srch_dump_ind.srchDumpInfo[c].fing[f].onTime[a] )


/*=======================================================================
 MACRO          : HDRFING_GET_RSSI

 DESCRIPTION    : Reads finger RSSI

 INPUTS         : c  - Carrier
                  f  - Finger Index

 RETURN VALUE   : Returns RSSI of the finger
 =========================================================================*/

#define HDRFING_GET_RSSI( c, f )  \
          ( ( hdrmdsp_shared_mem_interface->read.srch_dump_ind.srchDumpInfo[c].fing[f].pathRSSIFilt >> 16 ) & 0xFFFF )


/*=======================================================================
 MACRO          : HDRFING_GET_LOCK

 DESCRIPTION    : Reads finger Lock status

 INPUTS         : f  - Finger Index

 RETURN VALUE   : 0/non-zero (Reads the boolean variable for lock status)
 =========================================================================*/

#define HDRFING_GET_LOCK( c, f )  \
        ( ( hdrmdsp_shared_mem_interface->read.general_status.C[c].      \
            fingStatus[f] & HDRFING_STATUS_LOCK_MASK ) ?       \
            TRUE : FALSE )



/*=======================================================================
 MACRO          : HDRFING_FINGER_IS_LOCKED_MC()

 DESCRIPTION    : Returns the false if no fingers are locked;
                  true if at least one finger is locked.

 INPUTS         : c  - carrier
 =========================================================================*/
#ifndef HDRSRCH_UT_PLATFORM
#define HDRFING_FINGER_IS_LOCKED_MC( c )       \
          ( ( hdrmdsp_shared_mem_interface->   \
              read.rmac_subframe_ind.rmacInfo[c].fingLockedFlag \
              & HDRFING_LOCKED_FLAG_MASK ) ? \
          TRUE : FALSE )            
#else
#error code not present
#endif /* HDRSRCH_UT_PLATFORM */

/*************************************************************************
 *                                                                       *
 *                         FLL Macros                                    *
 *                                                                       *
 *************************************************************************/

/* Bits of the FLL PDM CTL DSP value */
#define HDRFLL_RX_PDM_INVERT_TRK_LO_M          0x0020
#define HDRFLL_ENABLE_TRK_LO_PDM_M             0x0001   

/* Bits of the FLL RVAFC CTL DSP value */
#define HDRFLL_RVAFC_ENABLE_MASK               BIT(15)
#define HDRFLL_RVAFC_BANDWIDTH_MODE_MASK       BIT( 0)
#ifdef FEATURE_XO
#define HDRFLL_XOAFC_ENABLE_MASK               BIT(14)
  /* XOAFC enable bit mask in HDR_fllRVAfcEnable */
#define HDRFLL_MND_WR_ENABLE_MASK              BIT(15)
  /* MND write enable bit mask in HDR_fllMndWrEnable */
#endif /* FEATURE_XO */

#ifdef FEATURE_HDR_TX_ROTATOR
#define HDRFLL_ROT_ENABLE_TXROT_SHFT           2
  /* Tx rotator enable bit shift in HDR_fllRotatorEnable */
#endif /* FEATURE_HDR_TX_ROTATOR */

/* Bits of FLL Rotator enable CTL DSP value for selected antennas  */                                                     
#define HDRFLL_ROT_ANTENNA_SELECT_MASK         0xFFFC
     
/*========================================================================
 MACRO          : HDRFLL_ACQUIRED()

 DESCRIPTION    : FLL LOCK register shows the lock status of the FLL acq
                  process. This register should be read only after the
                  FLL_MODE register is cleared by the DSP. If it returns
                  1 then the FLL acquisition was successful.

 INPUTS         : None

 RETURN VALUE   : FLL ACQ lock status
 ========================================================================*/

#define HDRFLL_ACQUIRED()                                          \
          ( hdrmdsp_shared_mem_interface->read.general_status.fllLocked )

/*========================================================================
 MACRO          : HDRFLL_CAPTURE_FLL_VCO_ACCUM_VALUE

 DESCRIPTION    : Capture the FLL VCO PDM value before exiting HDR channel.
                  Retrun zero if FEATURE_XO is defined.

 INPUTS         : None

 RETURN VALUE   : PDM Value read from the DSP
 ========================================================================*/

#ifndef FEATURE_XO
#define HDRFLL_CAPTURE_FLL_VCO_ACCUM_VALUE()                     \
          hdrmdsp_shared_mem_interface->read.general_status.fllVCOAccum
#else
#define HDRFLL_CAPTURE_FLL_VCO_ACCUM_VALUE()   0
#endif /* FEATURE_XO */

/*========================================================================
 MACRO          : HDRFLL_CAPTURE_FLL_ROT_ACCUM_VALUE

 DESCRIPTION    : Capture the FLL rotator accumulator value. Use HDR_fllAccum
                  to capture the rotator accumulation if FEATURE_XO is defined.
                  Used also in the XO-AFC mode.
 
 INPUTS         : demod_idx - Index of carrier to read rotator accum value

 RETURN VALUE   : FLL rotator accumulator value read from the DSP
 ========================================================================*/

#define HDRFLL_CAPTURE_FLL_ROT_ACCUM_VALUE( demod_idx )                  \
          hdrmdsp_shared_mem_interface->read.general_status.rotAccum[demod_idx]


/*========================================================================
 MACRO          : HDRFLL_READ_DYN_CPD_MODE

 DESCRIPTION    : Read the dynamic cross-product mode .

 INPUTS         : None.

 RETURN VALUE   : Dynamic cross-product mode. The value returned is
                  0 during Acquisition and 1 during tracking.
 ========================================================================*/

/* TBD */
/* The register is removed */
#define HDRFLL_READ_DYN_CPD_MODE( ) 0 /*( ( MDSP_READ( HDR_dynCPDMode ) & BIT(15) ) >> 15 )*/
          


/*************************************************************************
 *                                                                       *
 *                             HSTR MACROS                               *
 *                                                                       *
 *************************************************************************/

/*========================================================================
 MACRO          : HDRHSTR_GET_COUNT

 DESCRIPTION    : Returns half slot timing reference count which is used
                  to tag the incoming packets.

 INPUTS         : None
 ========================================================================*/

#define HDRHSTR_GET_COUNT()                                        \
          ( hdrmdsp_shared_mem_interface->read.general_status.hstrCount )


/*************************************************************************
 *                                                                       *
 *                             MSTR MACROS                               *
 *                                                                       *
 *************************************************************************/

/*------------------------------------------------------------------------
                     HDR_mstrTracking BIT MASKS AND SHIFTS 
 ------------------------------------------------------------------------*/

#define HDRMSTR_TRACKING_BIT_MASK               0x8000
#define HDRMSTR_TRACKING_BIT_SHIFT              0xf

#define HDRMSTR_DEMOD_CARRIER_BIT_MASK          0xf00
#define HDRMSTR_DEMOD_CARRIER_BIT_SHIFT         0x8

#define HDRMSTR_ASP_BIT_MASK                    0x7
#define HDRMSTR_ASP_BIT_SHIFT                   0x0


/*========================================================================
 MACRO          : HDRMSTR_SET/RESET Hyperspace flag

 DESCRIPTION    : If hyperspace flag is reset (0) then the DSP attempts
                  preamble detection on every half slot and TX is disabled

                  ARM should set (1) the hyperspace flag when synchronization
                  is achived. DSP then attempts preamble only at the begining
                  of full slot and TX is enabled

 INPUTS         : None

 RETURN VALUE   : None
 ========================================================================*/

#define HDRMSTR_SET_HYPERSPACE_FLAG()                       \
          MDSP_WRITE( HDR_hyperspaceConfig, HDRMODE_HYPERSPACE_MASK )

#define HDRMSTR_RESET_HYPERSPACE_FLAG()                     \
           MDSP_WRITE( HDR_hyperspaceConfig,  0 )


/*========================================================================
 MACRO          : HDRMSTR_GET_REF_PILOT_INFO

 DESCRIPTION    : This macro returns the refernce pilot sector info  

 INPUTS         : 

 RETURN VALUE   : Return the reference pilot sector info.
 ========================================================================*/

#define HDRMSTR_GET_REF_PILOT_INFO()                                        \
           ( hdrmdsp_shared_mem_interface->read.general_status.mstrTracking )

/*=======================================================================
 MACRO          : HDRMSTR_GET_MSTR_X8

 DESCRIPTION    : Reads MSTR to RTC offset in chipx8 units

 INPUTS         : None

 RETURN VALUE   : MSTR to RTC offset in chipx8 unsigned units
 =========================================================================*/

#define HDRMSTR_GET_MSTR_X8( )                                                \
           ( hdrmdsp_shared_mem_interface->read.general_status.mstrRTCOfs &   \
             0x3ffff )

/*=======================================================================
 MACRO          : HDRMSTR_GET_MSTR_ERROR_X8

 DESCRIPTION    : Reads MSTR ERROR in chipx8 units

 INPUTS         : None

 RETURN VALUE   : MSTR Error in chipx8
 =========================================================================*/

#define HDRMSTR_GET_MSTR_ERROR_X8( )                                    \
           ( hdrmdsp_shared_mem_interface->read.general_status.mstrRTCError )

/*========================================================================
 MACRO          : HDRMDSP_GET_SYS_TIME

 DESCRIPTION    : Returns the System Slot Count maintained by the DSP

 RETURN VALUE   : Slot count
 ========================================================================*/

#define HDRMDSP_GET_SYS_TIME() HDRHSTR_GET_COUNT()


/*************************************************************************
 *                                                                       *
 *                         DRC Channel Macros                            *
 *                                                                       *
 *************************************************************************/

/*=======================================================================
 MACRO          : HDRDRC_GET_VAR_BEST_ASP_PRED

 DESCRIPTION    : Returns the best ASP recommended by the DSP in the VAR
                  mode.

 INPUTS         : None
 =========================================================================*/

/* TBD */
/* Frmware is going to export a new register */
#define HDRDRC_GET_VAR_BEST_ASP_PRED( c )   \
          ( hdrmdsp_shared_mem_interface->read.general_status.C[c].bestASP )

/*=======================================================================
 MACRO          : HDRDRC_GET_CURRENT_DRC

 DESCRIPTION    : Returns the DRC that is requested from the current
                  serving sector. This is the transmitted DRC and not
                  the tentative DRC.

 INPUTS         : None
 =========================================================================*/

#define HDRDRC_GET_CURRENT_DRC( c )  \
        (hdrmdsp_shared_mem_interface->read.general_status.C[c].actualDRC)



/*=======================================================================
 MACRO          : HDRDRC_DRC_SUPERVISION_TIMEOUT_MC

 DESCRIPTION    : Returns TRUE if the DSP supervision timer has expired.
                  The register value will be 0 if the timer has expired so
                  this macro compares it with 0 and returns the result.

 INPUTS         : c        - Carrier Index
 =========================================================================*/

#define HDRDRC_DRC_SUPERVISION_TIMEOUT_MC( c ) \
                     (hdrmdsp_handoff_c[c].DrcSupervisionTimeOut == 0)


/*=======================================================================
 MACRO          : HDRDRC_GET_DRC_LENGTH_MC

 DESCRIPTION    : Gets the specified carrier drc length.

 OUTPUTS        : c          - Carrier Index
 =========================================================================*/

#define HDRDRC_GET_DRC_LENGTH_MC( c ) \
                       ( hdrmdsp_txagc_c[c].DrcLength )

/*=======================================================================
 MACRO          : HDRDRC_GET_RCL_PRED_ADJ

 DESCRIPTION    : Returns the RCL predictor outer-loop adjust.

 INPUTS         : None

 RETURN VALUE   : None
 =========================================================================*/
#define HDRDRC_GET_RCL_PRED_ADJ( c )  \
    (hdrmdsp_shared_mem_interface->read.general_status.C[c].rclPredAdj)


/*=======================================================================
 MACRO          : HDRDRC_GET_DRC_RL

 DESCRIPTION    : Returns the DRC that is being transmitted on Rev Link

 INPUTS         : None

 RETURN VALUE   : None
 =========================================================================*/

#define HDRDRC_GET_DRC_RL_MC(c)     \
    (hdrmdsp_shared_mem_interface->read.general_status.C[c].actualDRC)

/*=======================================================================
 MACRO          : HDRDRC_GET_DRC_FL

 DESCRIPTION    : Returns the DRC that is being transmitted on Rev Link

 INPUTS         : None

 RETURN VALUE   : None
 =========================================================================*/

/* TBD */
/* Is it the correct value for this macro? */
#define HDRDRC_GET_DRC_FL(c)   \
    (hdrmdsp_shared_mem_interface->read.general_status.C[c].actualDRC)

/* DRC RL Logging format */
#define HDRDRC_RL_LOG_DRC_RATE_M  0x0f00 /* DRC Rate mask & shift info */
#define HDRDRC_RL_LOG_DRC_RATE_S  8

#define HDRDRC_RL_LOG_DRC_COVER_M 0x0007 /* DRC Cover mask & shift info */
#define HDRDRC_RL_LOG_DRC_COVER_S 0

#define HDRDRC_RL_LOG_ACK_EN_M    0x2000 /* Ack enable mask & value info */
#define HDRDRC_RL_LOG_ACK_EN_V    0x2000

#define HDRDRC_RL_LOG_ACK_M       0x1000 /* Ack/Nack mask & value info */
#define HDRDRC_RL_LOG_ACK_V       0x0000
#define HDRDRC_RL_LOG_NACK_V      0x1000


/*=======================================================================
 MACRO          : HDRDRC_ENABLE_FLOW_CONTROL

 DESCRIPTION    : Enables the DSP flow control. DSP will send NULL
                  cover while flow control is enabled.

 INPUTS         : None
 =========================================================================*/

#define HDRDRC_ENABLE_FLOW_CONTROL()    \
            MDSP_WRITE( HDR_flowControlOn, 0x8000)


/*=======================================================================
 MACRO          : HDRDRC_DISABLE_FLOW_CONTROL

 DESCRIPTION    : Disables the DSP flow control.

 INPUTS         : None
 =========================================================================*/

#define HDRDRC_DISABLE_FLOW_CONTROL()    \
            MDSP_WRITE( HDR_flowControlOn, 0x0000)



#define HDRDRC_TUNE_AWAY_BOTH       0x0000   /* Tune away both antennas */
#define HDRDRC_TUNE_AWAY_DIV_TRANS  0x0001   /* Tune away both but feed 
                                                single antenna to predictor */
#define HDRDRC_TUNE_AWAY_DIV        0x8001   /* Tune away only the div path. */

#ifdef FEATURE_8X60_DUAL_RADIO_FUSION
#error code not present
#endif /* FEATURE_8X60_DUAL_RADIO_FUSION */

/*=======================================================================
 MACRO          : HDRDRC_GET_DRC_RAMP_DOWN

 DESCRIPTION    : Reads whether the MDSP is performing DRC ramp down.

 INPUTS         : None

 RETURN VALUE   : Whether the MDSP is performing DRC ramp down.
 =========================================================================*/

#define HDRDRC_GET_DRC_RAMP_DOWN()       \
        ( hdrmdsp_shared_mem_interface->read.general_status.tuneAwayActive )


/*=======================================================================
 MACRO          : HDRDRC_GET_DRC_LOCK_UPDATE_MC

 DESCRIPTION    : Returns the slot number when the DRC lock was updated
                  for the given carrier.

 INPUTS         : c_idx - Carrier Index

 RETURN VALUE   : None
 =========================================================================*/

#define HDRDRC_GET_DRC_LOCK_UPDATE_MC(c_idx)              \
           ( hdrmdsp_shared_mem_interface->                        \
             read.rmac_subframe_ind.rmacInfo[c_idx].mac.drcLockUpdate )


/*=======================================================================
 MACRO          : HDRDRC_GET_FILTER_OUTPUT_MC
 
 DESCRIPTION    : Gets the DRC filter output value for a specified carrier.

 INPUTS         : None

 RETURN VALUE   : Current DRC filter output
 =========================================================================*/
#define HDRDRC_GET_FILTER_OUTPUT_MC( c )    \
        ( hdrmdsp_shared_mem_interface->read.general_status.C[c].drcFilt )


/*************************************************************************
 *                                                                       *
 *                            AGC related MACROS                         *
 *                                                                       *
 *************************************************************************/

/* Common flag values */
#define HDRDSP_AGC_ENABLE    0x8000
#define HDRDSP_AGC_DISABLE   0x0000
#define HDRDSP_AGC_READY     0x0000
#define HDRDSP_AGC_OVERRIDE  0x8001
#define HDRDSP_AGC_TRIGGER   0x4000

/* AGC unit conversion info */
#define HDRDSP_AGC_COUNTS_PER_DB   640       /* 640 AGC counts per dB */
#define HDRDSP_AGC_RX_OFFSET_256    -16333   /* (-63.8 dB * 256 )  */

/*************************************************************************
 *                               RX AGC                                  *
 *************************************************************************/

/*========================================================================
 MACRO          : HDRAGC_GET_RX0_AGC()

 DESCRIPTION    : Gets Rx0 AGC value.

 INPUTS         : None.

 RETURN VALUE   : The Rx agc value for antenna 0
 ========================================================================*/
#define HDRAGC_GET_RX0_AGC()                                        \
          (hdrmdsp_shared_mem_interface->read.rf_rd.A[0].AGCCommonAccum)

/*========================================================================
 MACRO          : HDRAGC_GET_RX1_AGC()

 DESCRIPTION    : Gets Rx1 AGC value.

 INPUTS         : demod carrier

 RETURN VALUE   : The Rx agc value for antenna 1
 ========================================================================*/

#define HDRAGC_GET_RX1_AGC()                                  \
          (hdrmdsp_shared_mem_interface->read.rf_rd.A[1].AGCCommonAccum)

/*************************************************************************
 *                               TX AGC                                  *
 *************************************************************************/

/* Tx AGC constants */
#define HDRAGC_NUM_TX_LINEARIZERS  4
#define HDRAGC_NUM_TX_LIN_ENTRIES  33


/* PA Threshold structure offsets
*/
#define HDRAGC_PA_R1_RISE_OFFSET    (0)
#define HDRAGC_PA_R2_RISE_OFFSET    (1)
#define HDRAGC_PA_R3_RISE_OFFSET    (2)
#define HDRAGC_PA_R1_FALL_OFFSET    (3)
#define HDRAGC_PA_R2_FALL_OFFSET    (4)
#define HDRAGC_PA_R3_FALL_OFFSET    (5)



/*========================================================================
 MACRO          : HDRAGC_GET_TX_OPEN_LOOP_MC()

 DESCRIPTION    : Gets Tx open loop agc value for the carrier 0.

 INPUTS         : None

 RETURN VALUE   : The Tx open loop agc value
 ========================================================================*/
#define HDRAGC_GET_TX_OPEN_LOOP()                                         \
          ( hdrmdsp_shared_mem_interface->read.general_status.C[0].openLoop )


/*========================================================================
 MACRO          : HDRAGC_GET_TX_OPEN_LOOP_MC()

 DESCRIPTION    : Gets Tx open loop agc value for the specified carrier.

 INPUTS         : c - Carrier index

 RETURN VALUE   : The Tx open loop agc value
 ========================================================================*/
#define HDRAGC_GET_TX_OPEN_LOOP_MC( c )                               \
          ( hdrmdsp_shared_mem_interface->read.general_status.C[c].openLoop )


/*========================================================================
 MACRO          : HDRAGC_GET_MAX_RRI()

 DESCRIPTION    : Returns the maximum possible Reverse data rate (rri)
                  based on available TX power.

 INPUTS         : None.

 RETURN VALUE   : Max rri value.
 ========================================================================*/
#define HDRAGC_GET_MAX_RRI()  \
         (hdrmdsp_shared_mem_interface->read.general_status.txMaxDataRateIndex);


/*========================================================================
 MACRO          : HDRAGC_GET_DRC_GAIN()

 DESCRIPTION    : Return the gain for the DRC channel.

 INPUTS         : None.

 RETURN VALUE   : Return the gain for the DRC channel.
 ========================================================================*/
#define HDRAGC_GET_DRC_GAIN()      \
         MDSP_READ(HDR_drcDigiGain)

/*========================================================================
 MACRO          : HDRAGC_GET_DATA_GAIN()

 DESCRIPTION    : Return the gain for the DATA channel.

 INPUTS         : None.

 RETURN VALUE   : Return the gain for the DATA channel.
 ========================================================================*/
#define HDRAGC_GET_DATA_GAIN()      \
         MDSP_READ(HDR_dataDigiGain)


/*========================================================================
 MACRO          : HDRAGC_GET_PILOT_GAIN()

 DESCRIPTION    : Return the gain for the pilot.

 INPUTS         : None.

 RETURN VALUE   : Return the gain for the pilot.
 ========================================================================*/
#define HDRAGC_GET_PILOT_GAIN()      \
         MDSP_READ(HDR_ackDigiGain)

/*========================================================================
 MACRO          : HDRAGC_RAB_IS_SET()

 DESCRIPTION    : Checks whether any RAB bits are set.

 INPUTS         : None.

 RETURN VALUE   : TRUE - at least one RAB bit is set.
                  FALSE - no RAB bits were set.
 ========================================================================*/
#define HDRRAB_RAB_IS_SET( c )              \
         ((boolean)((hdrmdsp_shared_mem_interface->    \
           read.general_status.rabDecision & 0x1) != 0))


/*========================================================================
 MACRO          : HDRRAB_GET_FRAB()

 DESCRIPTION    : Returns FRAB value.

 INPUTS         : c  - Carrier Index

 RETURN VALUE   : FRAB accross sectors; signed Q15 
 
 ========================================================================*/
#define HDRRAB_GET_FRAB( c )              \
           ( hdrmdsp_mac_c[c].FRAB )


/*========================================================================
 MACRO          : HDRTXTR_GET_RTC_OFFSET()

 DESCRIPTION    : Reads the RTC offset

 INPUTS         : None

 RETURN VALUE   : The RTC offset
 ========================================================================*/
#define HDRTXTR_GET_RTC_OFFSET()                                         \
          (MDSP_READ(HDR_txRTCOfsLo) | (MDSP_READ(HDR_txRTCOfsHi) << 16))


/*************************************************************************
 *                                                                       *
 *                        DSP LOGGING Macros                             *
 *                                                                       *
 *************************************************************************/

/*========================================================================
 MACRO          : HDRLOG_GET_VALID_BUF ()

 DESCRIPTION    : Returns the valid log buffer

 RETURN VALUE   : Buffer index
 ========================================================================*/
#define HDRLOG_GET_VALID_RL_LOG_BUF()         \
                   ( MDSP_READ( HDR_rlLogValidBuffer ) & 0x03 )

/*========================================================================
MACRO          : HDRLOG_GET_RL_SLOT

DESCRIPTION    : Returns the slot number associated with 1st entry in RL
                 log buffer

RETURN VALUE   : Buffer index
========================================================================*/

#define HDRLOG_GET_RL_SLOT()   \
     ( MDSP_READ( HDR_rlLogValidBuffer ) >> 8 )

/*========================================================================
 MACRO          : HDRLOG_GET_RL_LOG_BUF_ADDR

 DESCRIPTION    : Returns the log buffer address

 INPUT  VALUE   : Index

 RETURN VALUE   : Buffer address
 ========================================================================*/

#define HDRLOG_GET_RL_LOG_BUF_ADDR( buf_idx )  \
          ( buf_idx == 0 ? (dword)(HDR_rlLogBuf0) : \
            ( buf_idx == 1 ? (dword)(HDR_rlLogBuf1) : \
              ( buf_idx == 2 ? (dword)(HDR_rlLogBuf2) : \
                ( buf_idx == 3 ? (dword)(HDR_rlLogBuf3) : \
                  (dword)(HDR_rlLogBuf0) ) ) ) )


/*------------------------------------------------------------------------
                     rlLog1Buf BIT MASKS AND SHIFTS 
 ------------------------------------------------------------------------*/
/* Predicted DRC */
#define HDRDRC_RL_LOG1_PRED_DRC_M               0x0F00        
#define HDRDRC_RL_LOG1_PRED_DRC_S               8
/* DRC Channel Gain Boost */
#define HDRDRC_RL_LOG1_DRC_GAIN_BOOST_M         0x2000
/* DSC Channel Gain Boost */
#define HDRDRC_RL_LOG1_DSC_GAIN_BOOST_M         0x1000
         
/*========================================================================
 MACRO          : HDRLOG_GET_RL_LOG1_BUF_ADDR

 DESCRIPTION    : Returns the log buffer address

 INPUT  VALUE   : Index

 RETURN VALUE   : Buffer address
 ========================================================================*/

#define HDRLOG_GET_RL_LOG1_BUF_ADDR( buf_idx )  \
          ( (buf_idx == 1)? (dword)(HDR_rlLog1Buf1) : \
            ( (buf_idx == 2)? (dword)(HDR_rlLog1Buf2) : \
              ( (buf_idx == 3)? (dword)(HDR_rlLog1Buf3) : \
                (dword)(HDR_rlLog1Buf0) ) ) )

/*========================================================================
 MACRO          : HDRLOG_GET_BASP_SINR_LOG_BUF_ADDR

 DESCRIPTION    : Returns the log buffer address

 INPUT  VALUE   : Index

 RETURN VALUE   : Buffer address
 ========================================================================*/

#define HDRLOG_GET_BASP_SINR_LOG_BUF_ADDR( buf_idx )  \
         ( buf_idx == 0 ? (dword)(HDR_bestAspSinrLogBuf0) : \
         ( buf_idx == 1 ? (dword)(HDR_bestAspSinrLogBuf1) : \
         ( buf_idx == 2 ? (dword)(HDR_bestAspSinrLogBuf2) : \
         ( buf_idx == 3 ? (dword)(HDR_bestAspSinrLogBuf3) : \
         (dword)(HDR_bestAspSinrLogBuf0) ))))

/*========================================================================
 MACRO          : HDRLOG_GET_ASP_LOG_BUF_ADDR

 DESCRIPTION    : Returns the log buffer address

 INPUT  VALUE   : Index

 RETURN VALUE   : Buffer address
 ========================================================================*/

#define HDRLOG_GET_ASP_LOG_BUF_ADDR( buf_idx )  \
         ( buf_idx == 0 ? (dword)(HDR_aspLogBuf0) : \
         ( buf_idx == 1 ? (dword)(HDR_aspLogBuf1) : \
         ( buf_idx == 2 ? (dword)(HDR_aspLogBuf2) : \
         ( buf_idx == 3 ? (dword)(HDR_aspLogBuf3) : (dword)(HDR_aspLogBuf0) ))))
 


/*========================================================================
 MACRO          : HDRMDSP_GET_PA_MAX_T2P()

 DESCRIPTION    : Returns an estimate of the max T2P the PA can support, 
                  in .25 dB units.

 INPUTS         : None.

 RETURN VALUE   : Max T2P the PA can support, in .25 dB units.
 
 ========================================================================*/

#define HDRMDSP_GET_PA_MAX_T2P()              \
           ((int16)(MDSP_READ(HDR_txTrafficToPilotPeak)))


/*------------------------------------------------------------------------
               Tx Power Margin Values for Headroom 
 ------------------------------------------------------------------------*/

#define HDRMDSP_TX_PWR_MARGIN_ZERO_V    0        /*  0 dB */
#define HDRMDSP_TX_PWR_MARGIN_MINUS_3_V 0xF880   /* -3 dB */

#define HDRMDSP_TX_PWR_MARGIN_0P5_V     0x0140   /*  0.5 dB */
#define HDRMDSP_TX_PWR_MARGIN_0P066_V   0x002A   /*  0.066 dB */


#define HDRMDSP_TX_LIMIT_MAX_V          0x4000
#define HDRMDSP_TX_LIMIT_MIN_V          0x2000

/*========================================================================
 MACRO          : HDRMDSP_TX_IS_MIN_PWR_RATCHETING()

 DESCRIPTION    : Returns the status of Tx min power ratcheting

 INPUTS         : None

 RETURN VALUE   : TRUE  if Tx AGC has ratcheted RPC up/down command
                  FALSE otherwise
 ========================================================================*/
#define HDRMDSP_TX_IS_MIN_PWR_RATCHETING()                              \
  (((hdrmdsp_shared_mem_interface->read.general_status.txLimitingFlags) \
     & HDRMDSP_TX_LIMIT_MIN_V) ? TRUE : FALSE)


/*========================================================================
 MACRO          : HDRMDSP_TX_IS_MAX_PWR_RATCHETING()

 DESCRIPTION    : Returns the status of Tx max power ratcheting

 INPUTS         : None

 RETURN VALUE   : TRUE  if Tx AGC has ratcheted RPC up/down command
                  FALSE otherwise
 ========================================================================*/

#define HDRMDSP_TX_IS_MAX_PWR_RATCHETING()                              \
  (((hdrmdsp_shared_mem_interface->read.general_status.txLimitingFlags) \
     & HDRMDSP_TX_LIMIT_MAX_V) ? TRUE : FALSE)   



/*========================================================================
 MACRO          : HDRMDSP_TX_IS_PWR_RATCHETING()

 DESCRIPTION    : Returns the status of Tx power ratcheting.

 INPUTS         : None.

 RETURN VALUE   : TRUE  if Tx AGC has ratcheted RPC up command
                  FALSE otherwise
 ========================================================================*/

#define HDRMDSP_TX_IS_PWR_RATCHETING()                     \
          (((MDSP_READ(HDR_txLimitingMax)) & 0x8000) ? TRUE : FALSE)


/*========================================================================
 MACRO          : HDRMDSP_GET_TX_PWR_MARGIN

 DESCRIPTION    : Returns Tx power margin for the specified carrier.

 INPUTS         : c - Carrier Index

 RETURN VALUE   : Tx power margin in units X dB * 65536 / dynamic range
 ========================================================================*/

#define HDRMDSP_GET_TX_PWR_MARGIN_MC( c )                  \
          ( hdrmdsp_txagc_c[c].PwrMargin )


/*========================================================================
 MACRO          : HDRMDSP_GET_TX_PWR_MARGIN_MAX

 DESCRIPTION    : Returns max Tx power margin

 INPUTS         : None

 RETURN VALUE   : Min Tx power margin in units X dB * 65536 / dynamic range
 ========================================================================*/

#define HDRMDSP_GET_TX_PWR_MARGIN_MAX()              \
           (MDSP_READ(HDR_txPwrMarginMax))


/*========================================================================
 MACRO          : HDRMDSP_GET_TOTAL_TX_PWR()

 DESCRIPTION    : Reads total transmitted power over all carriers from
                  the DSP.

 INPUTS         : None.

 RETURN VALUE   : Max Tx power, in 65536/DynamicRange dB units;
                  DynamicRange = 102.4 dB
 
 ========================================================================*/
#define HDRMDSP_GET_TOTAL_TX_PWR()              \
           (MDSP_READ(HDR_txTotal))


/*========================================================================
 MACRO          : HDRMDSP_GET_MAX_AVAIL_TX_PWR()

 DESCRIPTION    : Reads max available Tx pwr from the DSP.

 INPUTS         : None.

 RETURN VALUE   : Max Tx power, in ((K + 63.5) - X dBm)*65536/DynamicRange
                  units; DynamicRange = 102.4 dBQ2 dB.
 
 ========================================================================*/
#define HDRMDSP_GET_MAX_AVAIL_TX_PWR()              \
          (MDSP_READ(HDR_txTotalMax))


/*========================================================================
 MACRO          : HDRAGC_GET_TX_PILOT_POWER_MC()

 DESCRIPTION    : Gets instantaneous transmit pilot power of the specified
                  carrier

 INPUTS         : c - Carrier Index

 RETURN VALUE   : Instantaneous transmit pilot power, in 
                  65536/DynamicRange dB units.
 
 ========================================================================*/
#define HDRAGC_GET_TX_PILOT_POWER_MC(c)              \
           (hdrmdsp_txagc_c[c].PilotPower)


/*========================================================================
 MACRO          : HDRMDSP_GET_TX_PILOT_PWR_PEAK_MC()

 DESCRIPTION    : Gets peak transmit pilot power of the specified
                  carrier

 INPUTS         : c - Carrier Index

 RETURN VALUE   : Peak transmit pilot power, in 
                  65536/DynamicRange dB units.
 
 ========================================================================*/
#define HDRMDSP_GET_TX_PILOT_PWR_PEAK_MC(c)              \
           (hdrmdsp_txagc_c[c].PilotPowerPeak)


/*========================================================================
 MACRO          : HDRMDSP_GET_FILT_TX_PILOT_PWR_MC()

 DESCRIPTION    : Gets average transmit pilot power of the specified
                  carrier

 INPUTS         : c - Carrier Index

 RETURN VALUE   : Average transmit pilot power, in 
                  65536/DynamicRange dB units.
 
 ========================================================================*/
#define HDRMDSP_GET_FILT_TX_PILOT_PWR_MC(c)              \
          (hdrmdsp_txagc_c[c].PilotPowerFilt)


/*========================================================================
 MACRO          : HDRLOG_GET_EQ_DEMOD_DECISION_LOG_BUF_ADDR

 DESCRIPTION    : Returns the log buffer address

 INPUT  VALUE   : Buffer index

 RETURN VALUE   : Buffer address
 ========================================================================*/

#define HDRLOG_GET_EQ_DEMOD_DECISION_LOG_BUF_ADDR( buf_idx ) \
         ( buf_idx == 0 ? (dword)(HDR_eqDemodDecisionLog0) : \
         ( buf_idx == 1 ? (dword)(HDR_eqDemodDecisionLog1) : \
         ( buf_idx == 2 ? (dword)(HDR_eqDemodDecisionLog2) : \
         ( buf_idx == 3 ? (dword)(HDR_eqDemodDecisionLog3) : \
         (dword)(HDR_eqDemodDecisionLog0) ))))

/*========================================================================
 MACRO          : HDRLOG_GET_EQ_BEST_CI_LOG_BUF_ADDR

 DESCRIPTION    : Returns the log buffer address

 INPUT  VALUE   : Buffer index

 RETURN VALUE   : Buffer address
 ========================================================================*/

#define HDRLOG_GET_EQ_BEST_CI_LOG_BUF_ADDR( buf_idx )  \
         ( buf_idx == 0 ? (dword)(HDR_eqBestCIAvrLog0) :  \
         ( buf_idx == 1 ? (dword)(HDR_eqBestCIAvrLog1) :  \
         ( buf_idx == 2 ? (dword)(HDR_eqBestCIAvrLog2) :  \
         ( buf_idx == 3 ? (dword)(HDR_eqBestCIAvrLog3) :  \
         (dword)(HDR_eqBestCIAvrLog0) ))))

/*========================================================================
 MACRO          : HDRLOG_GET_ALT_RAKE_CI_LOG_BUF_ADDR

 DESCRIPTION    : Returns the log buffer address

 INPUT  VALUE   : Buffer index

 RETURN VALUE   : Buffer address
 ========================================================================*/

#define HDRLOG_GET_ALT_RAKE_CI_LOG_BUF_ADDR( buf_idx ) \
         ( buf_idx == 0 ? (dword)(HDR_eqRakeCIAvrLog0) :  \
         ( buf_idx == 1 ? (dword)(HDR_eqRakeCIAvrLog1) :  \
         ( buf_idx == 2 ? (dword)(HDR_eqRakeCIAvrLog2) :  \
         ( buf_idx == 3 ? (dword)(HDR_eqRakeCIAvrLog3) :  \
         (dword)(HDR_eqRakeCIAvrLog0) ))))


/*========================================================================
 MACRO          : HDRLOG_GET_VALID_MAC_LOG_BUF ()

 DESCRIPTION    : Returns the valid RPC log buffer

 RETURN VALUE   : Buffer index
 ========================================================================*/

#define HDRLOG_GET_VALID_MAC_LOG_BUF() \
                      ( MDSP_READ( HDR_macLogValidBuffer ) & 0x03 )

/*========================================================================
 MACRO          : HDRLOG_GET_MAC_LOG_SLOT_STAMP

 DESCRIPTION    : Returns the slot timestamp for the given MAC log buffer
 ========================================================================*/

#define HDRLOG_GET_MAC_LOG_SLOT_STAMP( buf_idx ) \
          ( buf_idx == 0 ? MDSP_READ( HDR_macLogBuf0 ) : \
          ( buf_idx == 1 ? MDSP_READ( HDR_macLogBuf1 ) : \
          ( buf_idx == 2 ? MDSP_READ( HDR_macLogBuf2 ) : \
          ( buf_idx == 3 ? MDSP_READ( HDR_macLogBuf3 ) : \
                           MDSP_READ( HDR_macLogBuf0 ) ) ) ) )

/*========================================================================
 MACRO          : HDRLOG_GET_VALID_RAB_LOG_BUF ()

 DESCRIPTION    : Returns the valid RAB log buffer

 RETURN VALUE   : Buffer index
 ========================================================================*/

#define HDRLOG_GET_VALID_RAB_LOG_BUF() HDRLOG_GET_VALID_MAC_LOG_BUF()

/*========================================================================
 MACRO          : HDRLOG_GET_RAB_LOG_BUF_ADDR

 DESCRIPTION    : Returns the log buffer address

 INPUT  VALUE   : Index

 RETURN VALUE   : Buffer address
 ========================================================================*/

#define HDRLOG_GET_RAB_LOG_BUF_ADDR( buf_idx )  \
          ( buf_idx == 0 ? (dword)(HDR_macLogBuf0) + 16 : \
          ( buf_idx == 1 ? (dword)(HDR_macLogBuf1) + 16 : \
          ( buf_idx == 2 ? (dword)(HDR_macLogBuf2) + 16 : \
          ( buf_idx == 3 ? (dword)(HDR_macLogBuf3) + 16 : \
          (dword)(HDR_macLogBuf0) + 16))))

/*========================================================================
 MACRO          : HDRLOG_GET_VALID_RPC_LOG_BUF ()

 DESCRIPTION    : Returns the valid RPC log buffer

 RETURN VALUE   : Buffer index
 ========================================================================*/

#define HDRLOG_GET_VALID_RPC_LOG_BUF()  HDRLOG_GET_VALID_MAC_LOG_BUF()

/*=======================================================================
                            RX LOG BUFFER MACROS
========================================================================*/



/*========================================================================
 MACRO          : HDRLOG_GET_RX_STR

 DESCRIPTION    : Returns STR timestamp corresponding to the first slot
                  of the Rx log buffer.
 
 INPUTS         : idx - logBuf index

 RETURN VALUE   : HSTR

 ========================================================================*/

#define HDRLOG_GET_RX_STR( idx ) (hdrmdsp_shared_mem_interface->read.log_buffer[(idx)].SlotTime)

/*========================================================================
 MACRO          : HDRLOG_GET_RX_CARRIER_BUFF_VALID

 DESCRIPTION    : Returns a bit mask denoting through set bit positions
                  if a carrier n is enabled through out the 4-slot logging
                  period. A carrier is enabled if fingers are assigned 
                  to the carrier.
 
 INPUTS         : idx - logBuf index

 RETURN VALUE   : Carrier Buffer Valid bit mask

 ========================================================================*/

#define HDRLOG_GET_RX_CARRIER_BUFF_VALID( idx ) \
          (hdrmdsp_shared_mem_interface->read.log_buffer[idx].CarrierBufferValid)
           
/*========================================================================
 MACRO          : HDRLOG_GET_PER_SLOT_ASP

 DESCRIPTION    : Returns the per slot ASP information which includes the
                  ASP used for demodulation and the ASP on which we are
                  requesting service.
 
 INPUTS         : idx - log Buffer index
                  demod_idx - Carrier Index
                  slot_idx - Index of slot we are interested in.

 RETURN VALUE   : ASP - Demod and Next.

 ========================================================================*/

#define HDRLOG_GET_PER_SLOT_ASP( log_buff_idx, demod_idx, slot_idx )  \
           (hdrmdsp_shared_mem_interface->read.log_buffer[log_buff_idx].C[demod_idx].ASP[slot_idx] & 0x00FF)

/*========================================================================
 MACRO          : HDRLOG_GET_PER_SLOT_BASP_SINR

 DESCRIPTION    : Returns the Best ASP SINR
 
 INPUTS         : l - log Buffer index
                  d - Carrier Index
                  s - Index of slot we are interested in.

 RETURN VALUE   : Best ASP SINR

 ========================================================================*/
/* Max BestASPSINR for legacy log packets. Maps to 18.5dB */
#define HDRLOG_LEGACY_MAX_BASP_SINR                 0x8FFF

#define HDRLOG_GET_PER_SLOT_BASP_SINR( l, d, s )                        \
  ( hdrmdsp_shared_mem_interface->read.log_buffer[l].C[d].BestASPSINR[s] )

/*========================================================================
 MACRO          : HDRLOG_GET_PER_SLOT_EQ_DEMOD_DECISION

 DESCRIPTION    : Returns the Equalizer Demod decision field for a given 
                  slot and demod carrier
 
 INPUTS         : idx - log Buffer index
                  demod_idx - Carrier Index
                  slot_idx - Index of slot we are interested in.

 RETURN VALUE   : Equalizer Demod decision

 ========================================================================*/

#define HDRLOG_GET_PER_SLOT_EQ_DEMOD_DECISION( log_buff, demod, slot ) \
           (hdrmdsp_shared_mem_interface->read.log_buffer[log_buff].C[demod].EQStatus[slot])

/*========================================================================
 MACRO          : HDRLOG_GET_PER_SLOT_EQ_BEST_C2I

 DESCRIPTION    : Returns the Equalizer Best C2I for a given slot and 
                  demod carrier
 
 INPUTS         : idx - log Buffer index
                  demod_idx - Carrier Index
                  slot_idx - Index of slot we are interested in.

 RETURN VALUE   : Equalizer Best C2I

 ========================================================================*/

#define HDRLOG_GET_PER_SLOT_EQ_BEST_C2I( log_buff, demod, slot ) \
  ( hdrmdsp_shared_mem_interface->read.log_buffer[log_buff].C[demod].EQBestCIAvr[slot] )

/*========================================================================
 MACRO          : HDRLOG_GET_PER_SLOT_ALT_RAKE_C2I

 DESCRIPTION    : Returns the Rake C2I given the slot and demod carrier
 
 INPUTS         : idx - log Buffer index
                  demod_idx - Carrier Index
                  slot_idx - Index of slot we are interested in.

 RETURN VALUE   : Rake C2I

 ========================================================================*/

#define HDRLOG_GET_PER_SLOT_ALT_RAKE_C2I( log_buff, demod, slot ) \
           (hdrmdsp_shared_mem_interface->read.log_buffer[log_buff].C[demod].EQRakeCIAvr[slot])

/*========================================================================
 MACRO          : HDRLOG_GET_MAC_LOG_RPC_VALID ()

 DESCRIPTION    : Returns RPC valid bitmask.

 INPUTS         : log_buff_idx  - logBuf index

 RETURN VALUE   : RPC valid bitmask
 ========================================================================*/

#define HDRLOG_GET_MAC_LOG_RPC_VALID( log_buff_idx )           \
           (hdrmdsp_shared_mem_interface->read.log_buffer[log_buff_idx].C[0].RPCValid >> 12)

/*========================================================================
 MACRO          : HDRLOG_GET_MAC_LOG_RPC_ASP ()

 DESCRIPTION    : Returns RPC data for the given asp.

 INPUTS         : log_buff_idx - logBuf index
                  aspi - ASP index

 RETURN VALUE   : RPC values
 ========================================================================*/

#define HDRLOG_GET_MAC_LOG_ASP_RPC( log_buff_idx, aspi )    \
           (hdrmdsp_shared_mem_interface->read.log_buffer[log_buff_idx].C[0].RPC[aspi] >> 12)


/*=======================================================================
                            TX LOG BUFFER MACROS
========================================================================*/

/*========================================================================
 MACRO          : HDRLOG_CHECK_TX_LOG_INT_PENDING_FLAG()

 DESCRIPTION    : Checks the tx log interrupt pending flag and returns a 
                  TRUE if the log interrupt is pending and
                  FALSE otherwise

RETURN VALUE  : TRUE/FALSE
 ========================================================================*/
/* NEEDS_QDSP6_PORT */
#define HDRLOG_CHECK_TX_LOG_INT_PENDING_FLAG()              1

/*========================================================================
 MACRO          : HDRLOG_CLEAR_TX_LOG_INT_PENDING_FLAG()

 DESCRIPTION    : Clears the log interrupt pending flag

 ========================================================================*/
/* NEEDS_QDSP6_PORT */
#define HDRLOG_CLEAR_TX_LOG_INT_PENDING_FLAG()              


/*========================================================================
 MACRO          : HDRLOG_GET_TX_BUF_INDEX ()

 DESCRIPTION    : Returns the Tx logging buffer index at which to read after
                  receiving the logging interrupt

 RETURN VALUE   : Buffer index
 ========================================================================*/
#define HDRLOG_GET_TX_BUF_INDEX()     \
      (hdrmdsp_shared_mem_interface->read.general_status.txLogBufferIndex)


/*========================================================================
 MACRO          : HDRLOG_GET_TX_HSTR

 DESCRIPTION    : Returns HSTR timestamp corresponding to the first slot
                  of the Tx log buffer.
 
 INPUTS         : idx - logBuf index

 RETURN VALUE   : HSTR

 ========================================================================*/
#define HDRLOG_GET_TX_HSTR( idx ) \
          (hdrmdsp_shared_mem_interface->read.tx_log_buffer[idx].TxHalfSlotIndex)

/*========================================================================
 MACRO          : HDRLOG_GET_TX_CARRIER_BUFF_VALID

 DESCRIPTION    : Returns a bit mask denoting through set bit positions
                  if a RL carrier n is enabled through out the 4-slot logging
                  period. An RL carrier is enabled if txC[n].Modulator Config
                  bit P is set through out the 4-slot logging period.
 
 INPUTS         : idx - logBuf index

 RETURN VALUE   : Carrier buffer valid bit mask

 ========================================================================*/

#define HDRLOG_GET_TX_CARRIER_BUFF_VALID( idx ) \
          (hdrmdsp_shared_mem_interface->read.tx_log_buffer[idx].CarrierBufferValid)

/*========================================================================
 MACRO          : HDRLOG_GET_PER_SLOT_MC_RL_INFO

 DESCRIPTION    : Returns the RL field of Tx buffer which includes the DRC,
                  DSC and ACK information for a given slot and demod carrier.
 
 INPUTS         : idx - log Buffer index
                  demod_idx - Carrier Index
                  slot_idx - Index of slot we are interested in.

 RETURN VALUE   : RL field.

 ========================================================================*/

#define HDRLOG_GET_PER_SLOT_MC_RL_INFO( log_buff_idx, demod_idx, slot_idx ) \
           (hdrmdsp_shared_mem_interface->read.tx_log_buffer[log_buff_idx]. \
            C[demod_idx].RL[slot_idx])

/*========================================================================
 MACRO          : HDRLOG_GET_PER_SLOT_TRANS_DRC

 DESCRIPTION    : Returns the Transmitted DRC given the slot and carrier 
                  ID.
 
 INPUTS         : idx - log Buffer index
                  demod_idx - Carrier Index
                  slot_idx - Index of slot we are interested in.

 RETURN VALUE   : Transmitted DRC.

 ========================================================================*/

#define HDRLOG_GET_PER_SLOT_TRANS_DRC( log_buff_idx, demod_idx, slot_idx )  \
  (hdrmdsp_shared_mem_interface->read.tx_log_buffer[log_buff_idx].C[demod_idx].DRC[slot_idx] & 0x001F)

/*========================================================================
 MACRO          : HDRLOG_GET_PER_SLOT_PRED_DRC

 DESCRIPTION    : Returns the predicted DRC given the slot and carrier
                  ID.
 
 INPUTS         : idx - log Buffer index
                  demod_idx - Carrier Index
                  slot_idx - Index of slot we are interested in.

 RETURN VALUE   : HSTR

 ========================================================================*/

#define HDRLOG_GET_PER_SLOT_PRED_DRC( log_buff_idx, demod_idx, slot_idx )  \
  ((hdrmdsp_shared_mem_interface->read.tx_log_buffer[log_buff_idx].       \
    C[demod_idx].DRC[slot_idx] & 0x1F00) >> 8)


/*========================================================================
 MACRO          : HDRLOG_GET_DRC_DIGITAL_GAIN_MC()

 DESCRIPTION    : Returns the digital gain of the drc channel on the given
                  channel in the last subframe.

 INPUTS         : The index returned by HDRLOG_GET_TX_BUF_INDEX()
                  Carrier ID
                  HalfSlot
                  
 RETURN VALUE   : DRC channel gain.
 ========================================================================*/
 
#define HDRLOG_GET_DRC_DIGITAL_GAIN_LOG_MC( idx, ch, halfslot )     \
  (hdrmdsp_shared_mem_interface->read.tx_log_buffer[idx].C[ch].DigitalGain[halfslot].DRC)


/*========================================================================
 MACRO          : HDRLOG_GET_ACK_DIGITAL_GAIN_MC()

 DESCRIPTION    : Returns the digital gain of the ACK channel on the given
                  carrier in the last subframe.
                  
 INPUTS         : The index returned by HDRLOG_GET_TX_BUF_INDEX()
                  Carrier ID
                  HalfSlot

 RETURN VALUE   : ACK channel gain.
 ========================================================================*/
 
#define HDRLOG_GET_ACK_DIGITAL_GAIN_LOG_MC( idx, ch, halfslot )        \
  (hdrmdsp_shared_mem_interface->read.tx_log_buffer[idx].C[ch].DigitalGain[halfslot].ACK)


/*========================================================================
 MACRO          : HDRLOG_GET_DSC_DIGITAL_GAIN_MC()

 DESCRIPTION    : Returns the digital gain of the DSC channel on the given
                  carrier in the last subframe.
                  
 INPUTS         : The index returned by HDRLOG_GET_TX_BUF_INDEX()
                  Carrier ID
                  HalfSlot

 RETURN VALUE   : DSC channel gain.
 ========================================================================*/
 
#define HDRLOG_GET_DSC_DIGITAL_GAIN_LOG_MC( idx, ch, halfslot )     \
  (hdrmdsp_shared_mem_interface->read.tx_log_buffer[idx].C[ch].DigitalGain[halfslot].DSC)


/*========================================================================
 MACRO          : HDRLOG_GET_RRI_DIGITAL_GAIN_MC()

 DESCRIPTION    : Returns the digital gain of the RRI channel on the given
                  carrier in the last subframe.
                  
 INPUTS         : The index returned by HDRLOG_GET_TX_BUF_INDEX()
                  Carrier ID
                  HalfSlot

 RETURN VALUE   : RRI channel gain.
 ========================================================================*/
 
#define HDRLOG_GET_RRI_DIGITAL_GAIN_LOG_MC( idx, ch, halfslot )        \
  (hdrmdsp_shared_mem_interface->read.tx_log_buffer[idx].C[ch].DigitalGain[halfslot].RRI)
                     

/*========================================================================
 MACRO          : HDRLOG_GET_DATA0_DIGITAL_GAIN_MC()

 DESCRIPTION    : Returns the digital gain of the data0 channel on the given
                  carrier in last subframe.

 INPUTS         : The index returned by HDRLOG_GET_TX_BUF_INDEX()
                  Carrier ID
                  HalfSlot

 RETURN VALUE   : Data channel gain.
 ========================================================================*/
 
#define HDRLOG_GET_DATA0_DIGITAL_GAIN_LOG_MC( idx, ch, halfslot )          \
  (hdrmdsp_shared_mem_interface->read.tx_log_buffer[idx].C[ch].DigitalGain[halfslot].Data0Cos)


/*========================================================================
 MACRO          : HDRLOG_GET_DATA1_DIGITAL_GAIN()

 DESCRIPTION    : Returns the digital gain of the data1 channel on the given
 in the           carrier in the last subframe.

 INPUTS         : The index returned by HDRLOG_GET_TX_BUF_INDEX()
                  Carrier ID
                  HalfSlot

 RETURN VALUE   : Data1 channel gain (rev A/B only).
 ========================================================================*/
 
#define HDRLOG_GET_DATA1_DIGITAL_GAIN_LOG_MC( idx, ch, halfslot )  \
  (hdrmdsp_shared_mem_interface->read.tx_log_buffer[idx].C[ch].DigitalGain[halfslot].Data1Cos)


/*========================================================================
 MACRO          : HDRLOG_GET_PILOT_DIGITAL_GAIN_MC()

 DESCRIPTION    : Returns the digital gain of the reverse link pilot 
                  on the given channel in the last subframe.
                  
 INPUTS         : The index returned by HDRLOG_GET_TX_BUF_INDEX()
                  Carrier ID
                  HalfSlot

 RETURN VALUE   : Pilot channel gain.
 ========================================================================*/
 
#define HDRLOG_GET_PILOT_DIGITAL_GAIN_LOG_MC( idx, ch, halfslot )      \
  (hdrmdsp_shared_mem_interface->read.tx_log_buffer[idx].C[ch].DigitalGain[halfslot].Pilot)


/*========================================================================
 MACRO          : HDRLOG_GET_AUX_PILOT_DIGITAL_GAIN_MC()

 DESCRIPTION    : Returns the digital gain of the Auxiliary Pilot on the 
                  given channel in the last subframe.

 INPUTS         : The index returned by HDRLOG_GET_TX_BUF_INDEX()
                  Carrier ID
                  HalfSlot

 RETURN VALUE   : Auxiliary Pilot gain.
 ========================================================================*/
 
#define HDRLOG_GET_AUX_PILOT_DIGITAL_GAIN_LOG_MC( idx, ch, halfslot )  \
  (hdrmdsp_shared_mem_interface->read.tx_log_buffer[idx].C[ch].DigitalGain[halfslot].AUX)


/*========================================================================
 MACRO          : HDRLOG_GET_BETA_P_DIGITAL_GAIN_MC()

 DESCRIPTION    : Returns the BetaP gain value for the
                  given channel in the last subframe.

 INPUTS         : The index returned by HDRLOG_GET_TX_BUF_INDEX()
                  Carrier ID
                  HalfSlot

 RETURN VALUE   : BetaP gain.
 ========================================================================*/
 
#define HDRLOG_GET_BETA_P_DIGITAL_GAIN_LOG_MC( idx, ch, halfslot )  \
  (hdrmdsp_shared_mem_interface->read.tx_log_buffer[idx].C[ch].DigitalGain[halfslot].BetaP)



/*========================================================================
 MACRO          : HDRLOG_GET_MAX_RRI_LOG_MC()

 DESCRIPTION    : In Rev0 mode, returns maximum RRI allowed in the previous 
                  frame.
                  In RevA, returns RL traffic-to-pilot peak ratio.
                  
 INPUTS         : The index returned by HDRLOG_GET_TX_BUF_INDEX()
                  Carrier ID
                  HalfSlot

 RETURN VALUE   : Maximum RRI allowed in the previous frame.
 ========================================================================*/
 
#define HDRLOG_GET_MAX_RRI_LOG_MC( idx, ch, halfslot )                \
  (hdrmdsp_shared_mem_interface->read.tx_log_buffer[idx].C[ch].DigitalGain[halfslot].MaxRLRatePeakTP)

                                          
/*========================================================================
 MACRO          : HDRLOG_GET_TX_AGC_STATUS_LOG_MC()

 DESCRIPTION    : Returns Tx AGC status bits for a given channel.
                  
 INPUTS         : The index returned by HDRLOG_GET_TX_BUF_INDEX()
                  Carrier id
                  Halfslot

 RETURN VALUE   : Tx AGC status bits.
 ========================================================================*/

/* Tx AGC Status bit masks*/
#define HDRLOG_TX_AGC_STATUS_LOG_RPC_M       0x0003 /* RPC bit info */
#define HDRLOG_TX_AGC_STATUS_LOG_RPC_S       0
 
#define HDRLOG_GET_TX_AGC_STATUS_LOG_MC( idx, ch, halfslot )           \
          (hdrmdsp_shared_mem_interface->read.tx_log_buffer[idx].C[ch].DigitalGain[halfslot].TxAGCStatus)

/*========================================================================
 MACRO          : HDRLOG_GET_TX_LIMITING_LOG()

 DESCRIPTION    : Returns Tx AGC limiting status bits.
                  
 INPUTS         : The index returned by HDRLOG_GET_TX_BUF_INDEX()
                  
 RETURN VALUE   : Tx limiting status bits.
 ========================================================================*/

/* Tx Limiting bit masks*/
#define HDRLOG_TX_LIMITING_LOG_THROTTLE_M  0x8000 /* Tx throttle flag */
#define HDRLOG_TX_LIMITING_LOG_THROTTLE_S  15

#define HDRLOG_TX_LIMITING_LOG_MAX_POWER_M 0x4000 /* Tx max power flag */
#define HDRLOG_TX_LIMITING_LOG_MAX_POWER_S 14

#define HDRLOG_TX_LIMITING_LOG_MIN_POWER_M 0x2000 /* Tx min power flag */
#define HDRLOG_TX_LIMITING_LOG_MIN_POWER_S 13

#define HDRLOG_GET_TX_LIMITING_LOG( idx )                            \
          (int16)(hdrmdsp_shared_mem_interface->read.tx_log_buffer[idx].TxLimiting[1])

         
/*========================================================================
 MACRO          : HDRLOG_GET_TOTAL_POWER_LOG()

 DESCRIPTION    : Returns Tx Total Power from the previous 
                  frame(Rev0) / subframe(RevA/B) over all carriers.
                  
 INPUTS         : The index returned by HDRLOG_GET_TX_LOG_INDEX()

 RETURN VALUE   : Raw total power value.
 ========================================================================*/
 
#define HDRLOG_GET_TOTAL_POWER_LOG( idx )                            \
          (int16)(hdrmdsp_shared_mem_interface->read.tx_log_buffer[idx].TxTotal[1])


/*========================================================================
 MACRO          : HDRLOG_GET_TOTAL_POWER_LOG_MC()

 DESCRIPTION    : Returns Tx Total Power from the previous 
                  frame(Rev0) / subframe(RevA/B) for a specified carrier.
                  
 INPUTS         : The index returned by HDRLOG_GET_TX_LOG_INDEX()

 RETURN VALUE   : Raw total power value.
 ========================================================================*/
 
#define HDRLOG_GET_TOTAL_POWER_LOG_MC( idx, ch )                     \
          (int16)(hdrmdsp_shared_mem_interface->read.tx_log_buffer[idx].C[ch].DigitalGain[1].TxTotal)


/*========================================================================
 MACRO          : HDRLOG_GET_OPEN_LOOP_POWER_LOG_MC()

 DESCRIPTION    : Returns Tx Open Loop Power from 
                  the previous frame(Rev 0) / subframe(Rev A).
                  Note: One is added because the preferred logging point is 
                  the second halfslot of the frame/subframe.
                  
 INPUTS         : The index returned by 
                  HDRLOG_GET_DIGITAL_GAIN_LOG_BUFFER_INDEX()

 RETURN VALUE   : Raw open loop power value.
 ========================================================================*/
                                                                        
#define HDRLOG_GET_OPEN_LOOP_POWER_LOG_MC( idx, ch )                 \
          (int16)(hdrmdsp_shared_mem_interface->read.tx_log_buffer[idx].C[ch].DigitalGain[1].TxOpenLoop)


/*========================================================================
 MACRO          : HDRLOG_GET_CLOSED_LOOP_POWER_LOG_MC()

 DESCRIPTION    : Returns Tx Closed Loop Power from 
                  the previous frame(Rev 0) / subframe(Rev A).
                  Note: One is added because the preferred logging point is 
                  the second halfslot of the frame/subframe.
                  
 INPUTS         : The index returned by 
                  HDRLOG_GET_DIGITAL_GAIN_LOG_BUFFER_INDEX()

 RETURN VALUE   : Raw closed loop power value.
 ========================================================================*/
                                                                        
#define HDRLOG_GET_CLOSED_LOOP_POWER_LOG_MC( idx, ch )               \
          (int16)(hdrmdsp_shared_mem_interface->read.tx_log_buffer[idx].C[ch].DigitalGain[1].TxGainAdjVal)


/*========================================================================
 MACRO          : HDRLOG_GET_PILOT_POWER_LOG_MC()

 DESCRIPTION    : Returns Tx Pilot Power from 
                  the previous frame(Rev 0) / subframe(Rev A).
                  Note: One is added because the preferred logging point is 
                  the second halfslot of the frame/subframe.
                  
 INPUTS         : The index returned by 
                  HDRLOG_GET_DIGITAL_GAIN_LOG_BUFFER_INDEX()

 RETURN VALUE   : Raw pilot power value.
 ========================================================================*/
                                                                        
#define HDRLOG_GET_PILOT_POWER_LOG_MC( idx, ch )                     \
          (int16)(hdrmdsp_shared_mem_interface->read.tx_log_buffer[idx].C[ch].DigitalGain[1].TxPilotPower)

                           


/*************************************************************************
 *                                                                       *
 *                        DSP GP TIMER MACROS                            *
 *                                                                       *
 *************************************************************************/

/*========================================================================
 MACRO          : HDRMDSP TIMER ADDRESS MACROS

 DESCRIPTION    : Return addresses for different timers
 ========================================================================*/

#define  HDRMDSP_TIMER_N_ENABLE_ADDR( n )    (HDR_sysTimerEnable + n)
#define  HDRMDSP_TIMER_N_EXPIRED_ADDR( n )   (HDR_sysTimerExpired + n)
#define  HDRMDSP_TIMER_N_COUNT_ADDR( n )     (HDR_sysTimerCount + n)

/*========================================================================
 MACRO          : HDRMDSP_SET_TIMER_N

 DESCRIPTION    : Sets the System Slot Count maintained by the DSP

 INPUT VALUE   : Slot count
 ========================================================================*/

#define HDRMDSP_SET_TIMER_N( n, slot )  \
        MDSP_WRITE( (word *)(HDRMDSP_TIMER_N_COUNT_ADDR((n) )),(slot) & 0xffff)

/*========================================================================
 MACRO          : HDRMDSP_START_TIMER_N

 DESCRIPTION    : Enables timer N

 INPUT VALUE   : Slot count
 ========================================================================*/

#define HDRMDSP_START_TIMER_N( n )  \
          MDSP_WRITE( (word *)(HDRMDSP_TIMER_N_EXPIRED_ADDR((n))), 0x0000 ); \
          MDSP_WRITE( (word *)(HDRMDSP_TIMER_N_ENABLE_ADDR((n))),  0x8000 )


/*========================================================================
 MACRO          : HDRMDSP_STOP_TIMER_N

 DESCRIPTION    : Disables timer N

 INPUT VALUE   : Slot count
 ========================================================================*/

#define HDRMDSP_STOP_TIMER_N( n )  \
          MDSP_WRITE( (word *)(HDRMDSP_TIMER_N_ENABLE_ADDR((n))),  0x0000 )

/*========================================================================
 MACRO          : HDRMDSP_TIMER_N_ACTIVE

 DESCRIPTION    : Checks if timer N is active

 INPUT VALUE   : Slot count
 ========================================================================*/

#define HDRMDSP_TIMER_N_ACTIVE( n )    \
                          MDSP_READ( (word *)(HDRMDSP_TIMER_N_ENABLE_ADDR((n))) )

/*========================================================================
 MACRO          : HDRMDSP_TIMER_N_EXPIRED

 DESCRIPTION    : Checks if timer N has expired

 INPUT VALUE   : Slot count
 ========================================================================*/

#define HDRMDSP_TIMER_N_EXPIRED( n )  \
                          MDSP_READ( (word *)(HDRMDSP_TIMER_N_EXPIRED_ADDR((n))) )


/*========================================================================
 MACRO          : HDRMDSP_DEMOD_IN_PROGRESS

 DESCRIPTION    : Checks if demodulation of a packet is in progress

 INPUT VALUE   : none

 RETURN VALUE  : TRUE/FALSE
 ========================================================================*/

#define HDRMDSP_DEMOD_IN_PROGRESS()  MDSP_READ( HDR_activeSlices )


/*************************************************************************
 *                                                                       *
 *                           DSP SLEEP MACROS                            *
 *                                                                       *
 *************************************************************************/


/*========================================================================
 MACRO          : HDRMDSP_SLEEP_START()

 DESCRIPTION    : Sets the MDSP to HDR sleep mode

 ========================================================================*/

#define HDRMDSP_SLEEP_START()       MDSP_WRITE( HDR_sleepState, 0x8000 )


/*========================================================================
 MACRO          : HDRMDSP_SLEEP_STOP()

 DESCRIPTION    : Exits the MDSP from HDR sleep mode

 ========================================================================*/

#define HDRMDSP_SLEEP_STOP()        MDSP_WRITE( HDR_sleepState, 0x0000 )


/*========================================================================
 MACRO          : HDRMDSP_SLEEP_STATUS()

 DESCRIPTION    : Returns MDSP sleep status

 ========================================================================*/

#define HDRMDSP_SLEEP_STATUS()      MDSP_READ( HDR_hdrSleeping )


/*========================================================================
 MACRO          : HDRMDSP_SLEEP_ADJUST(slots)

 DESCRIPTION    : Adjusts the MDSP's state for the elapsed sleep duration

 ========================================================================*/

#define HDRMDSP_SLEEP_ADJUST(slots) MDSP_WRITE( HDR_sleepAdjust, (slots) )


/*========================================================================
 MACRO          : HDRMDSP_CHECK_MISSED_SYNC_CC_FLAG()

 DESCRIPTION    : Checks the missed sync CC flag.  A true (non-zero)
                  value indicates that SyncCC preamble may have been
                  missed.

RETURN VALUE  : TRUE/FALSE
 ========================================================================*/

#define HDRMDSP_CHECK_MISSED_SYNC_CC_FLAG()              \
          (MDSP_READ( HDR_missedCtrlPacket ))


/*========================================================================
 MACRO          : HDRMDSP_CLEAR_MISSED_SYNC_CC_FLAG()

 DESCRIPTION    : Clears the missed sync CC flag in DSP, which re-enables
                  missed preamble detection.

 ========================================================================*/

#define HDRMDSP_CLEAR_MISSED_SYNC_CC_FLAG()              \
          MDSP_WRITE( HDR_missedCtrlPacket, 0)


/*========================================================================
 MACRO          : HDRMDSP_ENABLE_MERGE_PREVENT

 DESCRIPTION    : Enables finger merge prevention algorithm in the DSP

 ========================================================================*/
/* This is removed in RevB */
/*
#define HDRMDSP_ENABLE_MERGE_PREVENT() \
          MDSP_WRITE( HDR_fingMergePrevent, 0xffff );\
          MDSP_WRITE( HDR_dllAlpha2, 0 )
*/

/*========================================================================
 MACRO          : HDRMDSP_GET_ACTIVE_SLICES()

 DESCRIPTION    : Returns zero if there are no slices active.

  ========================================================================*/
#define HDRMDSP_GET_ACTIVE_SLICES()                  \
          MDSP_READ(HDR_activeSlices) 


/*========================================================================
 MACRO          : HDRMDSP_SET_BCC_DETECT()

 DESCRIPTION    : Schedules BCC packet at slot i.

  ========================================================================*/
/* NEEDS QDSP6 PORT */
#define HDRMDSP_SET_BCC_DETECT(i, value)                    

         
                    
/*========================================================================
 MACRO          : HDRMDSP_GET_RL_PACKET_STATUS_MC(c)

 DESCRIPTION    : Gets the value of the rlPacketStatus register.
 ========================================================================*/

/* Packet status values  */
typedef enum
{
  HDRMDSP_RL_STATUS_IDLE                = 0x0000,
  HDRMDSP_RL_STATUS_LATE_SW_ERROR       = 0x0018,
  HDRMDSP_RL_STATUS_LATE_FW_ERROR       = 0x0014,
  HDRMDSP_RL_STATUS_PACKET_ACCEPTED     = 0x0002,
  HDRMDSP_RL_STATUS_CONTINUING_PREVIOUS = 0x0001
} hdrmdsp_rl_packet_status_type;

        
/*========================================================================
 MACRO          : HDRMDSP_GET_RL_ARQ_STATUS_MC()

 DESCRIPTION    : Gets the value of the rlArqStatus register.
 ========================================================================*/

/* ARQ status values and masks */
#define HDRMDSP_RL_ARQ_STATUS_M                0xE000
#define HDRMDSP_RL_ARQ_STATUS_S                13

#define HDRMDSP_RL_HARQ_ACK_RECEIVED_V         0x8000
#define HDRMDSP_RL_HARQ_NAK_RECEIVED_V         0x4000
#define HDRMDSP_RL_PARQ_NAK_RECEIVED_V         0x2000


/* Mask for Rel-0 Mode */
#define HDRMDSP_IS_856_DEFAULT_MODE_M         0xFFFCFFFF
/* Mask for Rev-A mode */
#define HDRMDSP_IS_856_ENHANCED_MODE_M        0x00010000

#define HDRMDSP_HARQ_MODE_1_VAL 0
#define HDRMDSP_HARQ_MODE_0_VAL 0x016A

#ifdef FEATURE_MDSPSVC_LOG
/*========================================================================

 MACRO          : HDRMDSP_READ_RX_LOG_LENGTH
                  HDRMDSP_READ_TX_LOG_LENGTH

 DESCRIPTION    : Read the number of DSP registers to be logged
                  in the DSP Rx or Tx log buffer.
  
 INPUTS         : None.

 RETURN VALUE   : Number of registers logged by the MDSP Rx or Tx Log Buffer.

 ========================================================================*/
/* Firmware does not define tehse registers */
#define HDRMDSP_READ_RX_LOG_LENGTH( )  MDSP_READ( HDR_hdrRxLogLength )

#define HDRMDSP_READ_TX_LOG_LENGTH( )  MDSP_READ( HDR_hdrTxLogLength )

#endif /* FEATURE_MDSPSVC_LOG */


/*========================================================================
 MACRO          : HDRMDSP_GET_FILT_BEST_ASP_SNR()

 DESCRIPTION    : Get filtered best Active Set Pilot SNR (Hi and Lo) in
                  Q9 format.

 INPUTS         : c - Carrier Index

 RETURN VALUE   : Filtered best ASP SNR
  ========================================================================*/
#define HDRMDSP_GET_FILT_BEST_ASP_SNR( c )                    \
        ( hdrmdsp_shared_mem_interface->                      \
          read.general_status.C[c].filtBestASPSNR )


/*========================================================================
 MACRO          : HDRMDSP_DISABLE_TX_AGC()

 DESCRIPTION    : Disable HDR App updating TXAGC.

  ========================================================================*/

#define HDRMDSP_DISABLE_TX_AGC( )                       \
          MDSP_WRITE( HDR_txAgcHwWriteEnable, 0 ); 
              
/*========================================================================
 MACRO          : HDRMDSP_ENABLE_TX_AGC()

 DESCRIPTION    : Enable HDR App updating TXAGC.

  ========================================================================*/

#define HDRMDSP_ENABLE_TX_AGC( )                       \
          MDSP_WRITE( HDR_txAgcHwWriteEnable, 0x8000 );


/*========================================================================

 MACRO          : HDRFLL_OVERRIDE_ROT

 DESCRIPTION    : Set rotator override accum value.
  
 INPUTS         : Demod index, Rotator and the override accum value

 RETURN VALUE   : None.

 ========================================================================*/

#ifdef FEATURE_HDR_TX_ROTATOR
#define HDRFLL_OVERRIDE_ROT( override_mode, rxrot, txrot )   \
              MDSP_WRITE32( HDR_fllRotatorForce, rxrot );    \
              MDSP_WRITE32( HDR_fllTxRotatorForce, txrot ); \
              MDSP_WRITE( HDR_fllRotatorOverride, override_mode )
#else
#define HDRFLL_OVERRIDE_ROT( demod_idx, override_mode, rxrot, txrot )   \
              hdrmdsp_fll_rot_c[demod_idx].RotatorForce = \
                rxrot[demod_idx] | 0x80000000
#endif /* FEATURE_HDR_TX_ROTATOR */

/*========================================================================
 MACRO          : HDRFLL_IS_FLL_CPD_TRACK_MODE

 DESCRIPTION    : DynCPDMode == 1, Acq Mode,
                  DynCPDMode == 0, Trk Mode,
 
 INPUTS         : demod_idx - Index of carrier to read CPD mode

 RETURN VALUE   : TRUE if Track Mode
 ========================================================================*/

#define HDRFLL_IS_FLL_CPD_TRACK_MODE( demod_idx )   \
        ( ( hdrmdsp_shared_mem_interface->read.general_status.fllDynCPDMode[demod_idx] & 0x1 ) == 0 )

/*========================================================================
 MACRO          : HDRFLL_GET_FLL_DYN_CPD_MODE

 DESCRIPTION    : DynCPDMode == 1, Acq Mode,
                  DynCPDMode == 0, Trk Mode,
 
 INPUTS         : demod_idx - Index of carrier to read CPD mode

 RETURN VALUE   : TRUE if Track Mode
 ========================================================================*/

#define HDRFLL_GET_FLL_DYN_CPD_MODE( demod_idx )   \
        ( hdrmdsp_shared_mem_interface->read.general_status.fllDynCPDMode[demod_idx] & 0x1 )

/*========================================================================
 MACRO          : HDRFLL_GET_FLL_DYN_FREQ_ACCUM

 DESCRIPTION    : DynCPDMode == 1, Acq Mode,
                  DynCPDMode == 0, Trk Mode,
 
 INPUTS         : demod_idx - Index of carrier to read CPD mode

 RETURN VALUE   : TRUE if Track Mode
 ========================================================================*/

#define HDRFLL_GET_FLL_DYN_FREQ_ACCUM( demod_idx )   \
        ( hdrmdsp_shared_mem_interface->read.general_status.fllDynFreqAccum[demod_idx] )


/*========================================================================
 MACRO          : HDRFLL_GET_FLL_DYN_ACQ2TRK_TIMER_HS

 DESCRIPTION    : Get the remaining time after acq to trk.
                  The timer count down once acq to trk happens.
 
 INPUTS         : demod_idx - Index of carrier to read CPD mode

 RETURN VALUE   : TRUE if Track Mode
 ========================================================================*/

#define HDRFLL_GET_FLL_DYN_ACQ2TRK_TIMER_HS( demod_idx )   \
        ( hdrmdsp_shared_mem_interface->read.general_status.fllAcq2TrkTimer[demod_idx] )

/*************************************************************************
 *                                                                       *
 *                         SLICE Macros                                  *
 *                                                                       *
 *************************************************************************/

/* Slice block size and base */
#define HDRMDSP_SLICE_SIZE  (HDR_slice1HSTRCount - HDR_slice0HSTRCount)
#define HDRMDSP_SLICE_BASE   HDR_slice0HSTRCount

#define HDRMDSP_SLICE_HSTR_COUNT  (HDR_slice0HSTRCount    - HDRMDSP_SLICE_BASE)

/*=======================================================================
 MACRO          : HDRMDSP_SLICE_I_ADDR

 DESCRIPTION    : Returns the address of the Slice block

 INPUTS         : i   - Slice index
                  reg - register in the Slice block
 =========================================================================*/

#define HDRMDSP_SLICE_I_ADDR( i, reg ) \
          (HDRMDSP_SLICE_BASE + (i) * HDRMDSP_SLICE_SIZE + (reg) )

/*=======================================================================
 MACRO          : HDRMDSP_SLICE_I_READ

 DESCRIPTION    : Reads parameters from ASP index n

 INPUTS         : i   - Slice Index
                  reg - Address of the parameter
                  val - Value to be written
 =========================================================================*/

#define HDRMDSP_SLICE_I_READ( i, reg )  \
          MDSP_READ( HDRMDSP_SLICE_I_ADDR( (i), (reg)))


/*========================================================================
 MACRO          : HDRMDSP_GET_SLICE_I_HSTR_COUNT

 DESCRIPTION    : Read HSTRCount for the packet being decoded at 
                  interlace i 

 INPUTS         : None

 RETURN VALUE   : None
 ========================================================================*/
/* TBD fw to export */
#define HDRMDSP_GET_SLICE_I_HSTR_COUNT( i ) \
        ( 0 )
/*         HDRMDSP_STUB( "TBD: HDRMDSP_GET_SLICE_I_HSTR_COUNT" ) */
/*          HDRMDSP_SLICE_I_READ( (i), HDRMDSP_SLICE_HSTR_COUNT ) */


/*************************************************************************
 *                                                                       *
 *                         DECODER Macros                                *
 *                                                                       *
 *************************************************************************/
/* Q6 TODO Use from FW definition
 * Number of words in firmware decoder packet header */
#define HDR_FW_PKT_HEADER                 3

/* Size of decoder block in bytes */
#define HDR_DECOB_BLOCK_SIZE             HDRFW_DECODE_SINGLE_PACKET_SIZE_IN_WORD

/*========================================================================
 MACRO          : HDRMDSP_GET_FW_DEC_HEADER_N

 DESCRIPTION    : Given the block index, this macro reads the Nth header
                  from the decoderHeader table for the corresponding 
                  decoder block. 
 
 INPUTS         : block_index - Decoder block index
                  n - header index from 0 through 2.

 RETURN VALUE   : None.

 ========================================================================*/
#define HDRMDSP_GET_FW_DEC_HEADER_N( block_index, n )               \
          hdrmdsp_shared_mem_interface->read.decode_header[block_index][n]

/*========================================================================
 MACRO          : HDRMDSP_DECOB_WORD_OFFSET_FROM_BLK_INDEX

 DESCRIPTION    : Translates the decoder buffer block index to word offset
                  from start of the decoder buffer to block start.
 
 INPUTS         : blk_index 

 RETURN VALUE   : None.

 ========================================================================*/
#define HDRMDSP_DECOB_WORD_OFFSET_FROM_BLK_INDEX( blk_index) \
          ((blk_index) * (HDR_DECOB_BLOCK_SIZE))


/*************************************************************************
 *                                                                       *
 *                     HDR MDSP CARRIER CONFIG MACROS                    *
 *                                                                       *
 *************************************************************************/

 /* EJECT */
/*========================================================================
 MACRO          : HDRMDSP_WRITE_TX_CARRIER_ORDERED_BY_RANK_MC

 DESCRIPTION    : Writes carriers ordered by ranks to the MDSP.  The DSP
                  uses the list for RL carrier gain throttling.  
  
 INPUTS         : index - Index of array txCarriersOrderedByRank[] where
                          the carrier id is to be written to
                  c     - Carrier id

 RETURN VALUE   : None.

 ========================================================================*/
#define HDRMDSP_WRITE_TX_CARRIER_ORDERED_BY_RANK_MC( index, c ) \
          ( hdrmdsp_tx_ranked_carriers[index] = c )


/*========================================================================
 MACRO          : HDRMDSP_SET_CARRIER_COMPOSITION_MC

 DESCRIPTION    : Writes carrier composition bits in the MDSP carrier
                  rank.  MDSP treats CarrierRank as just one number.  The
                  composition bits being the more significant bits, the 
                  carrier composition takes priority over carrier ranks in 
                  RL gain throttling.
  
 INPUTS         : c    - Carrier id
                  comp - Carrier composition
                         0 - Carrier supports Data + Pilot only
                         1 - Carrier supports Data + Pilot + Overhead
                         2 - Carrier supports Pilot only

 RETURN VALUE   : None.

 ========================================================================*/
#define HDRMDSP_SET_CARRIER_COMPOSITION_MC( c, comp ) \
          (hdrmdsp_txagc_c[c].CarrierRank =           \
          (hdrmdsp_txagc_c[c].CarrierRank &           \
          HDRMDSP_CARRIER_COMPOSITION_MASK) |         \
          (comp & HDRMDSP_CARRIER_RANK_MASK))


/*========================================================================
 MACRO          : HDRMDSP_OVERRIDE_DSC_MC

 DESCRIPTION    : Overrides DSC.
  
 INPUTS         : c    - Carrier id
                  dsc  - DSC value

 RETURN VALUE   : None.

 ========================================================================*/
#define HDRMDSP_OVERRIDE_DSC_MC( c, dsc )               \
  ( hdrmdsp_txagc_c[c].DscOverride = dsc )

#define HDRMDSP_ENABLE_DSC_MC( c )               \
  ( hdrmdsp_txagc_c[c].DscConfig = 0x8000 )



/*========================================================================
 MACRO          : HDRLOG_GET_DRC_DIGITAL_GAIN()

 DESCRIPTION    : Returns the digital gain of the drc channel on 
                  carrier 0.

 INPUTS         : The index returned by HDRLOG_GET_TX_BUF_INDEX()
                  
 RETURN VALUE   : DRC channel gain.
 ========================================================================*/
 
#define HDRLOG_GET_DRC_DIGITAL_GAIN_LOG( idx )     \
  HDRLOG_GET_DRC_DIGITAL_GAIN_LOG_MC( idx, 0, 0 )


/*========================================================================
 MACRO          : HDRLOG_GET_ACK_DIGITAL_GAIN()

 DESCRIPTION    : Returns the digital gain of the ACK channel on 
                  carrier 0.
                  
 INPUTS         : The index returned by HDRLOG_GET_TX_BUF_INDEX()

 RETURN VALUE   : ACK channel gain.
 ========================================================================*/
 
#define HDRLOG_GET_ACK_DIGITAL_GAIN_LOG( idx )        \
  HDRLOG_GET_ACK_DIGITAL_GAIN_LOG_MC( idx, 0, 0 )


/*========================================================================
 MACRO          : HDRLOG_GET_DSC_DIGITAL_GAIN()

 DESCRIPTION    : Returns the digital gain of the DSC channel on
                  carrier 0.
                  
 INPUTS         : The index returned by HDRLOG_GET_TX_BUF_INDEX()

 RETURN VALUE   : DSC channel gain.
 ========================================================================*/
 
#define HDRLOG_GET_DSC_DIGITAL_GAIN_LOG( idx )     \
  HDRLOG_GET_DSC_DIGITAL_GAIN_LOG_MC( idx, 0, 0 )


/*========================================================================
 MACRO          : HDRLOG_GET_PILOT_DIGITAL_GAIN()

 DESCRIPTION    : Returns the digital gain of the reverse link pilot 
                  on carrier 0.
                  
 INPUTS         : The index returned by HDRLOG_GET_TX_BUF_INDEX()

 RETURN VALUE   : Pilot channel gain.
 ========================================================================*/
 
#define HDRLOG_GET_PILOT_DIGITAL_GAIN_LOG( idx )      \
  HDRLOG_GET_PILOT_DIGITAL_GAIN_LOG_MC( idx, 0, 0 )


/*========================================================================
 MACRO          : HDRLOG_GET_MAX_RRI_LOG()

 DESCRIPTION    : In Rev0 mode, returns maximum RRI allowed in the previous 
                  frame.
                  In RevA, returns RL traffic-to-pilot peak ratio.
                  
 INPUTS         : The index returned by HDRLOG_GET_TX_BUF_INDEX()

 RETURN VALUE   : Maximum RRI allowed in the previous frame.
 ========================================================================*/
 
#define HDRLOG_GET_MAX_RRI_LOG( idx )                \
  HDRLOG_GET_MAX_RRI_LOG_MC( idx, 0, 0 )

/*========================================================================
 MACRO          : HDRMDSP_BYPASS_RCL_ADJUST_AT_ASP_OUT_OF_LOCK

 DESCRIPTION    : This macro programs firmware to bypass RCL Adjustment 
                  addition to predictor SINR when the serving ASP is out of 
                  lock.
                  
 INPUTS         : None

 RETURN VALUE   : None
 ========================================================================*/
#define HDRMDSP_BYPASS_RCL_ADJUST_AT_ASP_OUT_OF_LOCK()     \
          MDSP_WRITE( HDR_enableBypassRCLUnlockedAsp, 0x8000)


#ifdef FEATURE_HDR_GRICE
 
/*========================================================================
 MACRO          : HDRMDSP_GET_FAC_GAIN_FABMETRIC

 DESCRIPTION    : Returns the FAB gain metric value for up to 12 elements to
                  calculate FAC gain estimate for gRICE equalizer.

 INPUTS         : Pingpong buffer index, range from 0 to 1
                  Element index, range from 0 to 11
 
 RETURN VALUE   : FAB gain metric
 ========================================================================*/

#define HDRMDSP_GET_FAC_GAIN_FABMETRIC( buff_idx, element_idx )               \
          ( hdrmdsp_shared_mem_interface->read.log_buffer[buff_idx].FABEstBuffer.fabRes[element_idx].FABMetric )

/*========================================================================
 MACRO          : HDRMDSP_GET_FAC_GAIN_FABCP

 DESCRIPTION    : Returns the FAB gain Cp value for up to 12 elements to
                  calculate FAC gain estimate for gRICE equalizer.

 INPUTS         : Pingpong buffer index, range from 0 to 1
                  Element index, range from 0 to 11
 
 RETURN VALUE   : FAB gain Cp 
 ========================================================================*/

#define HDRMDSP_GET_FAC_GAIN_FABCP( buff_idx, element_idx )                   \
          ( hdrmdsp_shared_mem_interface->read.log_buffer[buff_idx].FABEstBuffer.fabRes[element_idx].FABCp )

/*========================================================================
 MACRO          : HDRMDSP_GET_FAC_GAIN_NUM_ELEMENTS

 DESCRIPTION    : Returns the number of slots to calculate FAC gain
                  estimate for gRICE equalizer.

 INPUTS         : Pingpong buffer index, range from 0 to 1
 
 RETURN VALUE   : Number of slots, used as the count for pingpong buffer
 ========================================================================*/

#define HDRMDSP_GET_FAC_GAIN_NUM_ELEMENTS( buff_idx )                      \
          ( hdrmdsp_shared_mem_interface->read.log_buffer[buff_idx].FABEstBuffer.numElements )

/*======================================================================
 MACRO          : HDRASP_SET_SNR_RANK

 DESCRIPTION    : Set SNR rank in ASP config

 INPUTS         : n   - ASP Index
                  c   - Demod Carrier Index 
                  val - SNR rank value

 RETURN VALUE   : 
 =========================================================================*/

#define HDRASP_SET_SNR_RANK( c, n, rank_order ) \
          ( hdrmdsp_shared_mem_interface->write.asp_cfg.snrRank[c][n] = rank_order )

/*========================================================================
 MACRO          : HDRLOG_GET_PER_SLOT_G_ASP

 DESCRIPTION    : Returns gRICE G set info given the slot and demod carrier
 
 INPUTS         : idx - log Buffer index
                  demod_idx - Carrier Index
                  slot_idx - Index of slot we are interested in.

 RETURN VALUE   : gRICE G-set info
                 bit 0-2:   Active set pilot indexes x corresponding to
                            entry 1 in gRICE tracking set (i.e. G-set)
                 bit 3-5:  Active set pilot indexes x corresponding to
                            entry 2 in gRICE tracking set (i.e. G-set)
                 bit 6-8:  Active set pilot indexes x corresponding to
                            entry 3 in gRICE tracking set (i.e. G-set)
 
 ========================================================================*/

#define HDRLOG_GET_PER_SLOT_G_ASP( log_buff, demod, slot ) \
          (hdrmdsp_shared_mem_interface->read.log_buffer[log_buff].C[demod].ASP[slot])

/*======================================================================
 MACRO          : HDRASP_GSET_ACTIVATE

 DESCRIPTION    : Activate an ASP and also turn on the "C" bit to indicate
                  to FW that this is set for gRICE CSET ASP (i.e. CSET
                  pilot included in A-set list only for the purpose of gRICE
                  tracking.)

 INPUTS         : n  - ASP Index
                  c  - Demod Carrier Index 

 RETURN VALUE   : 
 =========================================================================*/

#define HDRASP_GSET_ACTIVATE( c, n ) \
          ( hdrmdsp_shared_mem_interface->write.asp_cfg.aspC[c][n].config = 0x0001 )


/*======================================================================
 MACRO          : HDRASP_IS_ACTIVE

 DESCRIPTION    : This function returns whether asp is active (aset or gcset).

 INPUTS         : n  - ASP Index
                  c  - Demod Carrier Index 

 RETURN VALUE   : TRUE: active
                  FALSE: inactive
 =========================================================================*/

#define HDRASP_IS_ACTIVE( c, n ) \
          ( ( hdrmdsp_shared_mem_interface->write.asp_cfg.aspC[c][n].config & 0x8001 ) != 0 )

#endif /* FEATURE_HDR_GRICE */

 
/*=======================================================================
 MACRO          : HDRLOG_GET_PER_SLOT_PRX_SINR

 DESCRIPTION    : Reads Filtered SINR for Prx

 INPUTS         : n  - ASP index
                  c  - Demod Carrier Index

 RETURN VALUE   : PRX SINR of of Demod Carrier c.
 =========================================================================*/

#define HDRLOG_GET_PER_SLOT_PRX_SINR( buff_idx, demod, slot ) \
           (hdrmdsp_shared_mem_interface->read.log_buffer[buff_idx].C[demod].BestASPRakePrxSINR[slot])

#ifdef FEATURE_HDR_REVC
#define HDRASP_SET_LOAD_ADJ( c, n, l) \
          ( hdrmdsp_shared_mem_interface->write.asp_cfg.aspC[c][n].NlbBackOffSinr =  \
            ( ( uint16 ) ( l << 6 ) ) )

#define HDRASP_SET_DEMOD_ALL_CARRIER(v) \
          ( hdrsrchmsg.handoff_cfg_msg.NLBCfg.ccDemodOnAllCarriers = (v) )

#define HDRASP_SET_NLB_THRESHOLD(th) \
          ( hdrsrchmsg.handoff_cfg_msg.NLBCfg.NlbTh = ( (th) << 7 ) )

#define HDRASP_SET_NLB_K(c, k) \
          ( hdrsrchmsg.handoff_cfg_msg.NLBCfg.NlbK[c] = ( (k) << 6 ) )

#endif /* FEATURE_HDR_REVC */

/*=======================================================================
 MACRO          : HDRMDSP_TRKPILOT_POS_CX2

 DESCRIPTION    : This macro reads the Pilot Position (in cx2) of the 2nd tracked
                  pilot in shared memory for fast reacq (optimized time line) 

 Return Value   : 32bit RTC value
 =========================================================================*/

#define HDRMDSP_GET_TRKPILOT_POS_CX2()           \
        ( hdrmdsp_shared_mem_interface->read.general_status.reAcqTrkPilotPosCx2 )
 
/*=======================================================================
 MACRO          : HDRMDSP_GET_MSTR_RTC_OFFSET_CX8

 DESCRIPTION    : This macro reads MSTR RTC offset (in cx8) 

 Return Value   : 32bit RTC value
 =========================================================================*/

#define HDRMDSP_GET_MSTR_RTC_OFFSET_CX8()           \
        ( hdrmdsp_shared_mem_interface->read.general_status.mstrRTCOfs )
 
/*=======================================================================
 MACRO          : HDRMDSP_GET_TX_RTC_OFFSET_CX8

 DESCRIPTION    : This macro reads TX RTC offset (in cx8) 

 Return Value   : 32bit RTC value
 =========================================================================*/

#define HDRMDSP_GET_TX_RTC_OFFSET_CX8()           \
        ( hdrmdsp_shared_mem_interface->read.general_status.txRTCOfs )
 
/*========================================================================
 MACRO          : HDRLOG_GET_FC_ENABLE()

 DESCRIPTION    : Returns whether flexconn is enabeld in fw.
                  
 INPUTS         : The index returned by HDRLOG_GET_TX_BUF_INDEX()

 RETURN VALUE   : whether flexconn is enabled in fw
 ========================================================================*/

#define HDRLOG_GET_FC_ENABLE( idx )           \
          (hdrmdsp_shared_mem_interface->read.tx_log_buffer[idx].fcLog.fcEnable)


/*========================================================================
 MACRO          : HDRLOG_GET_FC_SUBSTATE()

 DESCRIPTION    : Returns flexconn substate in fw: FC_ON or FC_OFF.
                  
 INPUTS         : The index returned by HDRLOG_GET_TX_BUF_INDEX()

 RETURN VALUE   : flexconn substate in fw
 ========================================================================*/

#define HDRLOG_GET_FC_SUBSTATE( idx )           \
          (hdrmdsp_shared_mem_interface->read.tx_log_buffer[idx].fcLog.fcSubState)

/*========================================================================
 MACRO          : HDRLOG_GET_FC_PA_TX_SATE()

 DESCRIPTION    : Returns PA/TX sate.
                  
 INPUTS         : The index returned by HDRLOG_GET_TX_BUF_INDEX()

 RETURN VALUE   : PA/TX state in fw
 ========================================================================*/

#define HDRLOG_GET_FC_PA_TX_STATE( idx )           \
          (hdrmdsp_shared_mem_interface->read.tx_log_buffer[idx].fcLog.fcPaTxState)


/*========================================================================
 MACRO          : HDRLOG_GET_FC_REPOINTING()

 DESCRIPTION    : Returns whether FW has done a drc/dsc repointing.
                  
 INPUTS         : The index returned by HDRLOG_GET_TX_BUF_INDEX()

 RETURN VALUE   : drc/dsc repointint status
 ========================================================================*/

#define HDRLOG_GET_FC_REPOINTING( idx )           \
          (hdrmdsp_shared_mem_interface->read.tx_log_buffer[idx].fcLog.fcRepoint)

/*========================================================================
 MACRO          : HDRLOG_GET_FC_DRC_LOCK()

 DESCRIPTION    : Returns drc lock
                  
 INPUTS         : The index returned by HDRLOG_GET_TX_BUF_INDEX()
                  carrier id

 RETURN VALUE   : drc lock 
 ========================================================================*/

#define HDRLOG_GET_FC_DRC_LOCK( idx, carrier, asp )           \
          (hdrmdsp_shared_mem_interface->read.tx_log_buffer[idx].fcLog.fcDrcLock[carrier][asp])

/*========================================================================
 MACRO          : HDRLOG_GET_DTX_TX_HALF_SLOT()

 DESCRIPTION    : Returns dtx tx half slot
                  
 INPUTS         : The index returned by HDRLOG_GET_TX_BUF_INDEX()
                  slot index

 RETURN VALUE   : tx half slot
 ========================================================================*/

#define HDRLOG_GET_DTX_TX_HALF_SLOT( idx, slot )           \
          (hdrmdsp_shared_mem_interface->read.tx_log_buffer[idx].dtxLog.txHsIndex[slot])

/*========================================================================
 MACRO          : HDRLOG_GET_DTX_CTRL()

 DESCRIPTION    : Returns dtx state in fw
                  
 INPUTS         : The index returned by HDRLOG_GET_TX_BUF_INDEX()
                  slot index

 RETURN VALUE   : dtx state in fw
 ========================================================================*/

#define HDRLOG_GET_DTX_CTRL( idx, slot )           \
          (hdrmdsp_shared_mem_interface->read.tx_log_buffer[idx].dtxLog.dtxCtl[slot])

/*========================================================================
 MACRO          : HDRLOG_GET_DTX_FRAME_OFFSET()

 DESCRIPTION    : Returns dtx frame offset in fw
                  
 INPUTS         : The index returned by HDRLOG_GET_TX_BUF_INDEX()
                  slot index

 RETURN VALUE   : dtx frame offset
 ========================================================================*/

#define HDRLOG_GET_DTX_FRAME_OFFSET( idx, slot )           \
          (hdrmdsp_shared_mem_interface->read.tx_log_buffer[idx].dtxLog.dtxFrameOffset[slot])


/*========================================================================
 MACRO          : HDRLOG_GET_DTX_SLOT()

 DESCRIPTION    : Returns whether the slot is dtx slot
                  
 INPUTS         : The index returned by HDRLOG_GET_TX_BUF_INDEX()
                  slot index

 RETURN VALUE   : whether the slot is dtx slot
 ========================================================================*/

#define HDRLOG_GET_DTX_SLOT( idx, slot )           \
          (hdrmdsp_shared_mem_interface->read.tx_log_buffer[idx].dtxLog.dtxSlot[slot])

/*========================================================================
 MACRO          : HDRLOG_GET_DTX_PA_TX_STATE()

 DESCRIPTION    : Returns PA/TX state
                  
 INPUTS         : The index returned by HDRLOG_GET_TX_BUF_INDEX()
                  slot index

 RETURN VALUE   : PA/TX state
 ========================================================================*/

#define HDRLOG_GET_DTX_PA_TX_STATE( idx, slot )           \
          (hdrmdsp_shared_mem_interface->read.tx_log_buffer[idx].dtxLog.dtxPaTxState[slot])

/*========================================================================
 MACRO          : HDRLOG_GET_TX_LIMITING_MAX()

 DESCRIPTION    : Returns tx limiting max
                  
 INPUTS         : The index returned by HDRLOG_GET_TX_BUF_INDEX()
                  slot index

 RETURN VALUE   : tx limiting max
 ========================================================================*/
#define HDRLOG_GET_TX_LIMITING_MAX( idx, slot )           \
          (hdrmdsp_shared_mem_interface->read.tx_log_buffer[idx].dtxLog.txLimitingMax[slot])


/*========================================================================
 MACRO          : HDRLOG_GET_TX_LIMITING_MIN()

 DESCRIPTION    : Returns tx limiting min
                  
 INPUTS         : The index returned by HDRLOG_GET_TX_BUF_INDEX()
                  slot index

 RETURN VALUE   : tx limiting min
 ========================================================================*/
#define HDRLOG_GET_TX_LIMITING_MIN( idx, slot )           \
          (hdrmdsp_shared_mem_interface->read.tx_log_buffer[idx].dtxLog.txLimitingMin[slot])

/*========================================================================
 MACRO          : HDRLOG_GET_CLOSED_LOOP_PWR()

 DESCRIPTION    : Returns closed loop power
                  
 INPUTS         : The index returned by HDRLOG_GET_TX_BUF_INDEX()
                  slot index

 RETURN VALUE   : closed loop power
 ========================================================================*/

#define HDRLOG_GET_CLOSED_LOOP_PWR( idx, carrier,slot )           \
          (hdrmdsp_shared_mem_interface->read.tx_log_buffer[idx].dtxLog.closedLoopPwr[carrier][slot])

/*========================================================================
 MACRO          : HDRLOG_GET_OPEN_LOOP_PWR()

 DESCRIPTION    : Returns open loop power
                  
 INPUTS         : The index returned by HDRLOG_GET_TX_BUF_INDEX()
                  slot index

 RETURN VALUE   : open loop power
 ========================================================================*/

#define HDRLOG_GET_OPEN_LOOP_PWR( idx, carrier, slot )           \
          (hdrmdsp_shared_mem_interface->read.tx_log_buffer[idx].dtxLog.openLoopPwr[carrier][slot])


/*========================================================================
 MACRO          : HDRLOG_GET_TOTAL_PWR()

 DESCRIPTION    : Returns total power
                  
 INPUTS         : The index returned by HDRLOG_GET_TX_BUF_INDEX()
                  slot index

 RETURN VALUE   : total power
 ========================================================================*/

#define HDRLOG_GET_TOTAL_PWR( idx,carrier, slot )           \
          (hdrmdsp_shared_mem_interface->read.tx_log_buffer[idx].dtxLog.txTotalPwr[carrier][slot])


/*========================================================================
 MACRO          : HDRLOG_GET_PWR_MARGIN()

 DESCRIPTION    : Returns power margin
                  
 INPUTS         : The index returned by HDRLOG_GET_TX_BUF_INDEX()
                  slot index

 RETURN VALUE   : power margin
 ========================================================================*/

#define HDRLOG_GET_PWR_MARGIN( idx,carrier, slot )           \
          (hdrmdsp_shared_mem_interface->read.tx_log_buffer[idx].dtxLog.pwrMargin[carrier][slot]) 

/*========================================================================
 MACRO          : HDRLOG_GET_PILOT_GAIN()

 DESCRIPTION    : Returns pilot channel gain
                  
 INPUTS         : The index returned by HDRLOG_GET_TX_BUF_INDEX()
                  slot index

 RETURN VALUE   : pliot channel gain
 ========================================================================*/

#define HDRLOG_GET_PILOT_GAIN( idx,carrier, slot )           \
          (hdrmdsp_shared_mem_interface->read.tx_log_buffer[idx].dtxLog.pilotGain[carrier][slot]) 


/*========================================================================
 MACRO          : HDRLOG_GET_DRC_GAIN()

 DESCRIPTION    : Returns drc channel gain
                  
 INPUTS         : The index returned by HDRLOG_GET_TX_BUF_INDEX()
                  slot index

 RETURN VALUE   : drc channel gain
 ========================================================================*/

#define HDRLOG_GET_DRC_GAIN( idx,carrier, slot )           \
          (hdrmdsp_shared_mem_interface->read.tx_log_buffer[idx].dtxLog.drcGain[carrier][slot]) 

#ifdef FEATURE_HDR_SELECTIVE_TXD
/*=======================================================================
 MACRO          : HDRLOG_GET_PER_SLOT_AGC

 DESCRIPTION    : Reads per slot AGC on given antenna

 INPUTS         : buff_idx  - log buffer idx
                  slot      - Slot interested
                  ant       - Antenna interested

 RETURN VALUE   : PRX SINR of of Demod Carrier c.
 =========================================================================*/
#define HDRLOG_GET_PER_SLOT_AGC( buff_idx, slot, ant )       \
          ( hdrmdsp_shared_mem_interface->read.log_buffer[buff_idx].RxAGC[slot].antenna[ant].RxAGCCommon )

/*========================================================================
 MACRO          : HDRLOG_GET_PER_SUBFRAME_RACHET

 DESCRIPTION    : Returns the number of times txTotalPwr exceeds MaxLimit
                  in a subframe (racheting)
 
 INPUTS         : idx - log Buffer index

 RETURN VALUE   : racheting count

 ========================================================================*/
#define HDRLOG_GET_PER_SUBFRAME_RATCHET( log_buff_idx )  \
  (hdrmdsp_shared_mem_interface->read.tx_log_buffer[log_buff_idx].TxMaxLimitingCount)
#endif /* FEATURE_HDR_SELECTIVE_TXD */

#ifdef FEATURE_HDR_BOLT_MODEM
/*========================================================================
 MACRO          : HDRMDSP_GET_DECOB_OFFSET

 DESCRIPTION    : Returns offset in decode buffer
 
 INPUTS         : None

 RETURN VALUE   : offset in decode buffer

 ========================================================================*/
#define HDRMDSP_GET_DECOB_OFFSET( )  \
  (hdrmdsp_shared_mem_interface->read.flInterface.decobOffsetInWord)
  
/*========================================================================
 MACRO          : HDRMDSP_GET_DECOB_OFFSET_VALID

 DESCRIPTION    : Returns the validity of DECOB_OFFSET
 
 INPUTS         : None

 RETURN VALUE   : If DECOB_OFFSET is valid

 ========================================================================*/
#define HDRMDSP_GET_DECOB_OFFSET_VALID( )  \
  (hdrmdsp_shared_mem_interface->read.flInterface.decobOffsetValid)
#endif /* FEATURE_HDR_BOLT_MODEM */

#ifdef FEATURE_POWER_INFO_LOG_PACKET
/*========================================================================
 MACRO          : HDRLOG_GET_PWR_LOG_BUF_INDEX ()

 DESCRIPTION    : Returns the power log buffer index at which to read after
                  receiving the logging interrupt

 RETURN VALUE   : Buffer index
 ========================================================================*/
#define HDRLOG_GET_PWR_LOG_BUF_INDEX()     \
      (hdrmdsp_shared_mem_interface->read.general_status.pwrLogBufferIndex)

/*========================================================================
 MACRO          : HDRLOG_GET_PWRLOG_DTX_SLOT

 DESCRIPTION    : Returns the DTX slot counts.  FW array of 16 slots is
                  used to store this information.
 
 INPUTS         : buf_idx - log Buffer index
                  idx - DTX slot array index

 RETURN VALUE   : 1:  Slot was DTXed
                  0:  Slot was not DTXed

 ========================================================================*/
#define HDRLOG_GET_PWRLOG_DTX_SLOT( buf_idx, idx ) \
          ( hdrmdsp_shared_mem_interface->read.pwr_log_buffer[ buf_idx ].dtxSlot[ idx ] )

/*========================================================================
 MACRO          : HDRLOG_GET_PWRLOG_DRX_SLOT

 DESCRIPTION    : Returns the DRX slot counts.  FW array of 16 slots is
                  used to store this information.
 
 INPUTS         : buf_idx - log Buffer index
                  idx - DRX slot array index

 RETURN VALUE   : 1:  Slot was DRXed
                  0:  Slot was not DRXed

 ========================================================================*/
#define HDRLOG_GET_PWRLOG_DRX_SLOT( buf_idx, idx ) \
          ( hdrmdsp_shared_mem_interface->read.pwr_log_buffer[ buf_idx ].drxSlot[ idx ] )

#endif /* FEATURE_POWER_INFO_LOG_PACKET */


/*========================================================================
 MACRO          : HDRMDSP_GET_MAX_TX_LIMIT

 DESCRIPTION    : Returns the maximium Tx power limit across all carriers
                  in raw FW format
 
 INPUTS         : funnel_mode - Whether RMAC is in funnel mode

 RETURN VALUE   : Max Tx power limit.

 ========================================================================*/
#define HDRMDSP_GET_MAX_TX_LIMIT( funnel_mode ) \
  ( ( funnel_mode ) ? \
    hdrmdsp_shared_mem_interface->read.rmac_subframe_ind.\
      txTotalOverallMaxLimitNeq : \
    hdrmdsp_shared_mem_interface->read.rmac_subframe_ind.\
      txTotalOverallMaxLimit )

#endif /* HDRDSPMCR_H */
