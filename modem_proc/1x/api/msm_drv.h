#ifndef MSM_DRV_H
#define MSM_DRV_H
/*===========================================================================

              M S M   R E G I S T E R   A C C E S S   M A C R O S

                            H E A D E R    F I L E

DESCRIPTION
  This file contains macros to support interaction with the QUALCOMM
  MSM and QSC Register Sets.

Copyright (c) 1999 - 2013 Qualcomm Technologies, Inc. 
  All Rights Reserved Qualcomm Proprietary

  Export of this technology or software is regulated by the U.S. Government.
  Diversion contrary to U.S. law prohibited.

  All ideas, data and information contained in or disclosed by
  this document are confidential and proprietary information of
  Qualcomm Technologies, Inc. and all rights therein are expressly reserved.
  By accepting this material the recipient agrees that this material
  and the information contained therein are held in confidence and in
  trust and will not be used, copied, reproduced in whole or in part,
  nor its contents revealed in any manner to others without the express
  written permission of Qualcomm Technologies, Inc.

===========================================================================*/



/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/1x/api/msm_drv.h#1 $



when       who     what, where, why
--------   ---     ----------------------------------------------------------
04/22/11   jtm     CMI Phase 4 - Final SU API changes.
11/15/10   jtm     Initial SU API split for CP.
11/09/10   jtm     Initial SU API split.
08/27/10   jtm     Removed TX_SET_GAIN_ADJUST. Moved TX_GET_GAIN_ADJUST to
                   muxmdsp.c.
07/30/10   cjb     Include "clk.h" for clk_busy_wait().
07/01/10   jtm     Only include rf_mdsp_tx_agc.h if FEATURE_1X_SUPPORTS_MSGR
                   is *not* defined.
06/09/10   jtm     Fixed merge and compiler errors.
05/21/10   jtm     Removed obsolete TX_SET_GAIN_ADJUST macro definitions.
05/18/10   vks     Mainline FEATURE_MDSP_SUPPORTS_TX_AGC.
05/17/10   vks     Change api to use new RF api.
04/28/10   vks     Featurize mdsp header file includes (these are not required
                   for Q6 interface).
03/25/10   vks     Send FW a message directly instead of using RF macros for
                   tx gain adjust override functionality.
03/11/10   sst     Lint fixes
03/03/10   vks     Call RF-FW update macro evertime a FW register is touched
                   using RF macros.
02/19/10   bb      Added support for "LTE to 1X IRAT core"
                   feature implementation
01/29/10   bb/adw  Featurize proper retarded fix.
01/21/10   adw     Added parenthesis around DEM1X_RTC_GET_COUNT macro.
01/15/10   bb      Modified DEM1X_RTC_GET_COUNT macro to return proper
                   RTC count values for QLIC enabled targets
11/05/09   jtm     Replaced ',' with ';' in various macros.
10/19/09   vks     Moved mdsp read/writes in msm_drv.h to muxmdsp.h and
                   renamed these macros from demod* to muxmdsp*
10/15/09   vks     Move QPCH decision macros from msm_drv.h to
                   srchzz_qpch_ontl_sm.c.
10/06/09   jtm     Temporariliy added RPC disable dipswitch checking back in
                   since RF depends on this checking here.
09/22/09   adw     Added parenthesis around the TX_GET_GAIN_ADJUST and
                   TX_AGC_READ_GAIN_ADJ macros.
08/04/09   jtm     Removed bio function calls from macros. The user of the
                   macro is either already checking software switches or needs
                   to based on the requirements of the new DAL api.
05/11/09   adw     Added power control logging subpacket. Lint fixes.
04/16/09   adw     Refined MSM_DRV_OUTC() macro to use HWIO_FVAL().
04/15/09   adw     Replaced MSM_OUTM with OUT to support SCMM hwio changes.
03/26/09   jtm     Mainlined T_MSM7600, T_MSM6700, and T_MSM6500. Removed
                   T_QSC60X5. Replaced T_MSM7800 with
                   FEATURE_MODEM_1X_ARTEMIS || FEATURE_MODEM_1X_POSEIDON.
                   VD_MODE_WB__INTMODE_MASK was removed. Its definition was
                   implanted directly into source.
03/26/09   jtm     Added include for modem_1x_defs.h
03/16/09   adw     Removed MSM_LOCK check from COMBINER_GET_LONG_CODE_STATE.
03/12/09   vlc     Modified TX_SET_GAIN_ADJUST macro for FW-based TXAGC
                   to a sequence which works better for MDSP FW.
02/03/09   jtm     Added support for T_POSEIDON2
11/06/08   pk/aps  Corrected DEM1X_LATCH_ALL and DEM1X_LATCH_RTC_AND_SC
10/27/08   aps     Corrected QLIC featurizations and macros
10/23/08   jtm     Removed T_QUASAR
09/25/08   jtm     BIO_SW5_M is now checked inside TX_SET_GAIN_ADJUST instead
                   of reading the current value of CDMA1X_cTxGainAdjOverride.
10/04/07   ljl     Supported multi-carriers.
05/17/07   pa      Corrected DEMOD_*_1X_RTC_GP? macros for 7600
04/24/07   sst     Update sample server macros for QSC60x5 == MSM7600
03/22/07   rkc     Rename QLIC_SET|GET_RTF_ADJUST() to SET|GET_RTF_ADJUST()
03/15/07   sst     Added SLEEP_CMD() for QSC60x5
02/23/07   sst     Add QLIC support
01/31/07   sst     Change T_QSC6055 to T_QSC60X5 to catch all tiers
12/19/06   sst     Merge the following change from 11.02.02B branch
 11/22/06  vm       Added the support for Firmware TX AGC
12/07/06   trc     Add SAMPLE_STOP_IMMED() macro for cross-target support
10/27/06   trc     MSM7600 support
10/23/06   sst     Merge from QSC6055 branched files
                   Update macros DEMOD_*_1X_RTC_GP# for QSC6055
10/09/06   trc     MSM7600 support
08/15/06   awj     Disallow inclusion of cagc.c for WINCE
07/27/06   jwob    Moved SLEEP register accessors macros to sclk.c
05/01/06   tjc     Stripped FEATURE_SEARCH1 and mainlined FEATURE_SRCH_PC2
01/16/06   awj     Lint cleanup
07/29/05   grl     Added SVD only gated clocking macros.
05/31/05   rng     Merged from SHDR Sandbox.
03/25/05   bt      Changed DEMOD_1X_SELECT_RTC_ROLLOVER and
                   DEMOD_1X_SELECT_DEMOD_26MS_ROLLOVER() to
                   DEMOD_1X_SELECT_COMBINER_ROLL and
                   DEMOD_1X_SELECT_DELAYED_COMBINER_ROLL.
02/23/05   ejv     Added DEMOD_1X_SELECT_RTC_ROLLOVER() and
                   DEMOD_1X_SELECT_DEMOD_26MS_ROLLOVER() for 6700+.
01/25/05   ejv     Add T_MSM6800.
01/17/05   pj      Added COMBINER_CURRENT_TIMETRACK_FINGER(), returns
                   the finger being tracked by the combiner.
12/02/04   ejv     Added more T_QUASAR checks for non-existing registers.
11/15/04   ejv     Removed NOP macro COMBINER_SET_FREQUENCY_ADJUST.
11/12/04   ejv     Updates for 6700 sleep registers.
11/03/04   ejv     Removed finger _ALL macros.
09/30/04   sfm     Mainlined FEATURE_FINGER_DRIVER
09/28/04   ejv     Removed some unused macros and ported the interface
                   on some others.
09/23/04   kwo     Added demod macros to support logging
09/03/04   sfm     Finger Driver changes
09/03/04   ejv     Add COMBINER_SET_VALID_SECTORS.
08/09/04   ejv     Moved some DEM1X macros to muxmsm.h.
                   Changed macro name.
08/02/04   ejv     Temporarily do not disable accumulators for pre-6700.
                   New macros for power control support.
07/15/04   sfm     Removed FINGER_*, FINGERS_*, and FFE_* macros
07/14/04   ejv     Add T_QUASAR block.
07/08/04   ejv     Update FINGER_GET_LOCK_STATUS to consider both RX chains.
06/29/04   ejv     Update COMBINER_ENABLE_CPCCH for 6700.
06/14/04   ejv     Updated COMBINER_SLEW macros to NOT use shadow.
06/02/04   ejv     Update COMBINER_ENABLE/DISABLE_CH0 macros for 6700.
                   Write TdShift to MDSP for fingx when setting TD mode.
05/07/04   bt      Added DEMOD_1X_RTC_SLEEP_DISABLE and enabled FAST/NORMAL_AGC
                   macros.
04/29/04   ejv     Define FINGER_ACCUM_DISABLE_FRAME for 6700.
04/21/04   ejv     Changed int to uint16 for LINT.
04/20/04   ejv     Updated FINGER_DISABLE_CHx macros.  Add frame aligned
                   macros for accumulator enable/disable.  Added macro
                   FINGER_ENABLED to check if finger is enabled.
03/31/04   ejv     Modified DEM1X_COMBINER_GET_COUNT() and added
                   DEM1X_COMBINER_GET_80MS_COUNT() and
                   DEM1X_COMBINER_GET_SUB80MS_COUNT().
03/23/04   ejv     Changed FINGER_SET_WALSH to FINGER_SET_CHAN_WALSH.
                   Removed FINGER_SET_ACCUM_WALSH and
                   FINGER_SET_SUP_CODE_WALSH.
03/12/04   ejv     Load spreading mask for fingers on CH3.
03/11/04   ejv     Removed NOP macros QPCH_ENABLE/DISABLE.
03/08/04   ejv     Add NULL macros for CH3 enable/disable for 6500.
03/01/04   kwo     Lint Corrections
02/26/04   ejv     Removed unused 6700 macro.
02/25/04   ddh     Modified COMBINER_SLEW_SUB80MS macro to contain () on vars
02/23/04   sfm     Modified COMBINER_SLEW macro to appease lint.
02/23/04   ddh     Cleaned up lint disables/enables added one to SEARCH_SLEW
02/20/04   ejv     Added MSM_NOT_USED for unused params.
02/19/04   ejv     Added new macro COMBINER_SLEW_SUB80MS.  Updated interface
                   to COMBINER_SET_DELAY and COMBINER_SLEW.
02/18/04   ddh     More lint corrections.
                   Removed MSM_LOCK/MSM_UNLOCK from FINGER_RND_CH3_ALL Macro
                   Changed use of f for finger index to _macro_f.
02/12/04   sfm     Changed ul to UL for lint
02/12/04   ejv     Remove FEATURE_GPSONE from around macros.
02/12/04   ddh     Corrected lint errors.
01/28/04   ejv     Mainline T_MSM6500.
01/27/04   ejv     If fingers are not enabled, write to the IMMED register
                   rather then the FRAME register for chan config.
                   Add #define STOP_SAMPLE_MODE 0x0.
01/20/04   sfm     Added 6700 versions of COMBINER_SLEW() and
                   COMBINER_SET_DELAY() macros
01/16/04   ejv     Remove FEATURE_SEARCH2.
01/13/04   bt      Removed unused defines and made sw_beta macros names
                   consistent.
01/12/04   ejv     Updates for 6700.  Added FEATURE_SEARCH1.
12/09/03   ejv     Removed some unused macros.
11/05/03   bt      Cleaned up DEM1X_LATCH macros.
11/04/03   fu/aaj  Support for MMSE Metric and combined RSSI reading
10/24/03   bt      QPCH_ENABLE/DISABLE are not supported in MSM6500, include
                   DEM1X_CHANNEL0_IMMED:QPCH_EN in ARM/DISARM_OOK_DECODE.
10/06/03   ejv     Initial support for 6700.
10/03/03   ejv     Cleanup.  Added FINGER_SET_REP_FACTOR_CH2/ALL for 6500.
09/22/03   ejv     Update FINGER_DISABLE_CH1/2 for 6500.
09/11/03   aar     Fixed bug in macro FINGERS_SET_FREQ_COMB_GAIN().
09/09/03   ejv     Added FINGER_SET_CH1/2_SPR_ALL for 6500.  Changed CH1/2
                   updated from IMMED to FRAME for applicable registers.
08/21/03   ejv     Updates for 6500 firmware revision 0x0029.
08/20/03   fu/aaj  Added macros to pair accumulators for RX diversity
08/18/03   ejv     Modify order of operation in FINGER_DISABLE_CH1/2.
08/04/03   ejv     Update DEM1X_RTC_GET_COUNT for 6100.
07/16/03   ejv     Additional 6500 updates for setting the spreading factor
                   for various walsh lengths in the fingers.
07/20/03   bt      More T_MSM6500 updates.
07/02/03   ejv     Update FINGER_ENABLE_CHx macros for 6500.
06/19/03   ejv     T_MSM6500 updates.
06/17/03   ejv     T_MSM6500 updates.
06/06/03   ejv     Fix TRK_LO_ADJ_MICRO_OVERRIDE_EN( ) for 6100.
04/29/03   bt      Add SEARCH_ENABLE/DISABLE_DEMOD_BYPASS for Cougar.
04/28/03   ejv     Mainline 6100 macros.
03/26/03   bt      Surround writes to VD_MODE with clock gating macros.
02/18/03   ejv     Additional support for T_MSM6500.
02/05/03   gs/kwo  Rel A support for ZZ2.
01/24/03   ejv     Replaced FINGER_SET_BINARY_OFFSET with ..._PN_OFFSET.
01/21/03   ejv     Added T_MSM6500.
12/31/02   ejv     Added RTC section and modified RTC macro names.
11/04/02   rm      Added new macro to check the dsp active status.
10/31/02   bt      add micro processor dump macros
10/02/02   gs      Merge with MSM_SRCH.08.06.42
09/11/02   va      Added macros to read information from MDSP.
07/30/02   gs      Initial merge into perforce for cougar.
07/16/02   ejv     Removed redundant macro.
07/02/02   dlb     Removed unneeded '#error' statement.  Enclosed macros in
                   "do{}while(0)" block to simulate single statement.
06/26/02   dlb     Added macros for turning on/off Dynamic Clock Gating.  Added
                   workaround for MSM6050 DEC1 problem.  Read SLEEP_COUNT if
                   MSM6050_A1.
06/24/02   ejv     Added macro back in which had been inadvertently removed.
06/13/02   abh     Modified macros to read Trk Lo PDMs for MSM6050 Rev2.0
                   compatibility.
05/30/02   ejv     Mainlined T_MSM5000.
03/21/02   ht      Added DMOD_SET_MAC_SEQ_REP_* defs for MSM6050.
03/14/02   rm      Fixed compiler warning.
03/13/02   abh     Fixed the macro TRK_LO_ADJ_SET_FREQ_ERR_8P4.
03/12/02   rm      Work around for MSM6050, where the sleep counter can't
                   be read.
02/11/02   rm      Added changes for MSM6050.
02/11/02   rm      Added macro to read the tx gain adjust.
02/07/02   ejv     Added new macros for MSM6000.
01/25/02   bgc     Updated T_MSM6000.
01/24/02   ejv     Added FEATURE_INVERTED_TRK_LO_POLARITY.
01/11/02   ejv     Added T_MSM6000.
09/26/01   ejv     Added DEMOD_ENABLE_MAC_ENGINE().
09/25/01   ejv     Added SEARCH_SET_WALSH.
08/07/01   aaj     Added QPCH ULPN mask changes for MSM5500
06/13/01   rm      Clear DSP_SUP_OFF when enabling MAC Engine.
05/11/01   ejv     Removed T_MSM31.
03/07/01   ejv     Added clear/set of LOCK_RSSI_EXT_RANGE for chip/symbol macros.
03/01/01   rm      Check for DIP Switch Setting before enable RPC.
02/16/01   dgy     Modified macro COMBINER_SELECT_CH0_512_CHIP_SYMBOL to
                   support CCI decoding.
01/31/01   rm      Added/fixed macros for finger merge detect in hw.
01/22/01   ejv     Added QPCH_SET_ION_QON for MSM5100.
01/18/01   ejv/rm  Added new macros for MSM5100 support.
12/14/00   ejv     Fixed shadowing for QOF enable/disable macros.
12/13/00   hrk     Removed #ifdef FEATURE_GPSONE around Macro CAGC_WRITE_8P2.
10/31/00   rm      Enable Hardware duty cycle fix for MSM5105.
10/25/00   rm      Initialization's for MSM5105. Defined New Macros.
09/21/00   day     Backed out changes to I and Q OFFSET macros to correct
                   argument list error.
09/10/00   ejv     Added FINGER_FCH_QOF_ENABLE, FINGER_FCH_QOF_DISABLE,
                   FINGER_SCH_QOF_ENABLE, and FINGER_SCH_QOF_DISABLE.
07/31/00   ejv     Modified TRK_LO_ADJ_SET_OVERRIDE.
07/25/00   day     Code cleanup on I offset and Q offset macros.
07/18/00   eh      Added macro to initialize viterbi decoder for MSM5000.
07/13/00   yll/day Added support for "Tune Away Without Drop" for FEATURE_GPSONE -
                   Fixed I_OFFSET_GET_ACCUMULATOR and Q_OFFSET_GET_ACCUMULATOR macros.
                   Added macro FING_COMB_OFFSET_READ.
06/29/00   abh/day Added MSM3300 support - FEATURE_GPSONE.  Changed SRCH2_CLK_ENA
                   to enable only Srch2 Block(shadowed writes).
06/12/00   ejv     Added DEMOD_ENABLE/DISABLE_FPC and FINGER_SET_TTL_FAST/SLOW.
06/06/00   bgc     Added revision check for DEMOD_ENABLE_MAC_ENGINE_FOR_SCH.
03/17/00   ajn     Sleep init now assumes DUTY_CYCLE fix for PPanther.
03/02/00   ajn     Added "slow clock" Frequency Error Estimator (FEE) support
02/24/00    eh     Defined SEARCH_ENABLE_DEMOD_BYPASS and
                   SEARCH_DISABLE_DEMOD_BYPASS.
01/27/00   ejv     Define SRCH2_CLK_ENA for MSM3100 only.
01/14/00   ejv     Added macros for adjusting the AGC loop gain constant.
01/13/00   ejv     Added macros for uP int. bypass and sleep controller read.
12/13/99   ejv     Added comments and masks for some macros.
12/06/99   ajn     Added Decoder Zero-Initialize modes
11/29/99   ejv     Mask the bottom 2 bits in SRCH_GET_STATUS_DUMP.
11/10/99   ejv     Added FEATURE_SEARCH2 macros.
10/27/99   ajn     Added QPCH related macros.
10/21/99   rm      Enable DSP for CH2 SCCH demodulation.
10/20/99   dgy     Added new macro to configure QOF.
09/24/99   rm      Added Macros to Configure MAC ENGINE.
09/13/99   rm      Added new macros to control ULPN descramble for RC4.
08/11/99   rm      Added new macros to enable/disable I or Q channel from the
                   Demodulator.
08/11/99   rm      Added new macros for Symbol combiner symbol accumulation.
08/06/99   ejv     Merged jcw's initial MSM5000 support.
07/23/99   ajn     First cut of file.

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

/*===========================================================================

                              END OF INTERFACE

===========================================================================*/

#endif /* MSM_DRV.H */
