#ifndef MSM_DRV_I_H
#define MSM_DRV_I_H
/*===========================================================================

              M S M   R E G I S T E R   A C C E S S   M A C R O S

                            H E A D E R    F I L E

DESCRIPTION
  This file contains macros to support interaction with the QUALCOMM
  MSM and QSC Register Sets.

  Copyright (c) 1999 - 2015 Qualcomm Technologies, Inc.
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

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/1x/drivers/inc/msm_drv_i.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
01/09/15   srk     Feature cleanup.
05/28/14   as      Jolokia compile clean-up
03/11/14   ssh     BO CGPS changes for TTR.
12/07/13   cjb     Centralized shadow register writes.
                   Removed old/unused RTC register writes.
12/07/13   cjb     Changes to support DEMBACK_1
02/15/13   srk     Replace mcs hwio with m1x hwio
02/13/13   cjb     Fixed MACROS used for setting Combiner long code mask.
09/26/12   jtm     DIME_REVISIT clean up and macro clean up.
08/28/12   cjb     Added 1us delay after writing to LATCH registers.
02/16/12   srk     Replaced FEATURE_LTE_TO_1X by FEATURE_MODEM_1X_IRAT_LTO1X.
01/23/12   srk     Feature Cleanup
09/02/11   sk      Rebased the code to the NikeL modem package by mainlining
                   FEATURE_MODEM_1X_NIKEL.
08/18/11   adw     Mainline 1x message router support.
07/27/11   vks     Fix compiler warnings.
05/05/11   adw     Additional changes required to enable lto1x on NikeL.
04/28/11   cjb     Changes needed to enable FEATURE_LTE_TO_1X for NikeL.
04/22/11   jtm     CMI Phase 4 - Final SU API changes.
03/30/11   adw     Register updates for NikeL.
03/28/11   jtm     Removed deprecated RF includes.
01/28/11   vks     Featurize RF interface with FEATURE_MODEM_1X_SUPPORTS_RF.
12/09/10   jtm     Cleaned up unneeded legacy clock code.
11/01/10   jtm     Modem 1x SU API changes.
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

//common
#include "1x_variation.h"
#include "customer.h"
#include "modem_1x_defs.h"

//diag
#include "cdma2kdiagi.h"

//drivers
#include "dec5000_v.h"
#include "dmod_i.h"
#include "m1x_hwio_mpss.h"
#include "msm_drv.h"

//searcher
#include "srch_fing_driver_i.h"

//other
#include "DALSys.h"

//feature dependent
#include "cdmafw_msg.h"
#include "muxmsgr.h"

#ifdef FEATURE_MODEM_1X_SUPPORTS_RF

#include "rfm_1x.h"

#endif /* FEATURE_MODEM_1X_SUPPORTS_RF */


/*===========================================================================

                            C O N S T A N T S

===========================================================================*/

#define DEM1X_ZERO                                          0x0
#define MSMDRV_SET                                          0xFFFF
#define CLEAR                                               0x0000
#define MSMDRV_ENABLE                                       0xFFFF
#define DISABLE                                             0x0000
#define LOCKED                                              0xFFFF
#define NOT_LOCKED                                          0x0

#define COMBINER_80MS_CX8_MASK                              0x00FFFFFUL
#define COMBINER_80MS_COUNT_MASK                            0x7F00000UL

/* Support 6 cells + NULL cell for power control combining */
#define NUM_PWR_CTRL                                        7


/*===========================================================================

                                 MACROS

===========================================================================*/
#define MSM_DRV_OUTC( io, field, val ) \
    HWIO_OUT( io, HWIO_FVAL( io, field, val ) )

#define MSM_DRV_SET( io, field ) \
    HWIO_OUTM( io, HWIO_FMSK( io, field ), HWIO_FMSK( io, field ) )

#define MSM_DRV_SET_BIT( io, bit ) \
    HWIO_OUTM( io, ( 1 << bit ), ( 1 << bit ) )

#define MSM_DRV_SET_i( io, field, index ) \
    HWIO_OUTMI( io, index, HWIO_FMSK( io, field ), HWIO_FMSK( io, field ) )

#define MSM_DRV_CLEAR( io, field ) \
    HWIO_OUTM( io, HWIO_FMSK( io, field ), 0 )

#define MSM_DRV_CLEAR_BIT( io, bit ) \
    HWIO_OUTM( io, ( 1 << bit ), 0 )

#define MSM_DRV_W( io, val ) \
    HWIO_OUT( io, val )

#define MSM_DRV_W_MASKED( io, field, val ) \
    HWIO_OUTF( io, field, val )

#define MSM_DRV_W_i( io, val, index ) \
    HWIO_OUTI( io, index, val )

#define MSM_DRV_W_MASKED_i( io, field, val, index ) \
    HWIO_OUTFI( io, index, field, val )

#define MSM_DRV_R( io ) \
    HWIO_IN( io )

#define MSM_DRV_R_MASKED( io, field ) \
    HWIO_INF( io, field )

#define MSM_DRV_R_MASKED_i( io, field, index ) \
    HWIO_INFI( io, index, field )

#define MSM_DRV_PULSE( io, field ) \
    HWIO_OUTF( io, field, (uint32)1 ); HWIO_OUTF( io, field, 0 )

/*===========================================================================

                              SHADOW COPY for REGISTERS

===========================================================================*/
/* Structure for shadow copy of the registers */
typedef struct
{
  uint32 RTC_CTL;
  uint32 RTC_SLEEP;

  /* DEMBACK 0 */
  uint32 DINT_CHw_CFG_0;
  uint32 DINT_CHw_CFG_1;
  uint32 DINT_CHw_CFG_2;
  uint32 SVD_MODE;

  /* DEMBACK 1 */
  uint32 DINT_CHw_CFG_DB1_0;
  uint32 DINT_CHw_CFG_DB1_1;
  uint32 DINT_CHw_CFG_DB1_2;
  uint32 SVD_MODE_DB1;

} msm_drv_hwio_shadow_reg_type;

extern volatile msm_drv_hwio_shadow_reg_type msm_drv_hwio_shadow_reg;

/* Macro for shadow field write */
#define MSM_DRV_SHADOW_W( io, field, val )                                    \
  /* Update the shadow first */                                               \
  msm_drv_hwio_shadow_reg.io =                                                \
  ((((uint32)(val) << HWIO_SHFT(io, field)) & (HWIO_FMSK(io, field))) |       \
   (msm_drv_hwio_shadow_reg.io & (uint32)(~(HWIO_FMSK(io, field)))));         \
  /* Overwrite the HW with the shadow copy */                                 \
  HWIO_OUT( io, (msm_drv_hwio_shadow_reg.io) )

/* Macro for shadow masked write */
#define MSM_DRV_SHADOW_M( io, mask, val )                                     \
  /* Update the shadow first */                                               \
  msm_drv_hwio_shadow_reg.io = ( ((uint32)(val) & (mask)) |                   \
    (msm_drv_hwio_shadow_reg.io & (uint32)(~(mask))) );                       \
  /* Overwrite the HW with the shadow copy */                                 \
  HWIO_OUT( io, (msm_drv_hwio_shadow_reg.io) )

/* Macro for shadow indexed/masked write */
#define MSM_DRV_SHADOW_IM( io, index, mask, val )                             \
  /* Update the shadow first */                                               \
  msm_drv_hwio_shadow_reg.io##_##index = ( ((uint32)(val) & (mask)) |         \
    (msm_drv_hwio_shadow_reg.io##_##index & (uint32)(~(mask))) );             \
  /* Overwrite the HW with the shadow copy */                                 \
  HWIO_OUTI( io, index, (msm_drv_hwio_shadow_reg.io##_##index) )

/* Macro for shadow set */
#define MSM_DRV_SHADOW_SET( io, field )                                       \
  /* Update the shadow first */                                               \
  msm_drv_hwio_shadow_reg.io =                                                \
  (((uint32)HWIO_FMSK(io, field)) |                                           \
   (msm_drv_hwio_shadow_reg.io & (uint32)(~(HWIO_FMSK(io, field)))));         \
  /* Overwrite the HW with the shadow copy */                                 \
  HWIO_OUT( io, msm_drv_hwio_shadow_reg.io )

/* Macro for shadow clear */
#define MSM_DRV_SHADOW_CLEAR( io, field )                                     \
  /* Update the shadow first */                                               \
  msm_drv_hwio_shadow_reg.io =                                                \
    msm_drv_hwio_shadow_reg.io & (uint32)(~(HWIO_FMSK(io, field)));           \
  /* Overwrite the HW with the shadow copy */                                 \
  HWIO_OUT( io, msm_drv_hwio_shadow_reg.io )

/* Macro for shadow read */
#define MSM_DRV_SHADOW_R( io, field )                                         \
  (msm_drv_hwio_shadow_reg.io & HWIO_FMSK(io,field)) >> HWIO_SHFT(io,field)

/*===========================================================================

                              DEMBACK REGISTERS

===========================================================================*/

#ifdef FEATURE_MODEM_1X_SINGLE_DEMBACK
/* Normal register update */
#define SET_DEMBACK_REG( reg, val )                                           \
  HWIO_OUT( reg, (val) );

/* Masked register update */
#define SET_DEMBACK_REG_M( reg, mask, val )                                   \
  HWIO_OUTM( reg, (mask), (val) );

/* Shadow register masked update */
#define SET_SHADOW_DEMBACK_REG_M( reg, mask, val )                            \
  MSM_DRV_SHADOW_M( reg, mask, (val) );

/* Shadow register masked update */
#define SET_SHADOW_DEMBACK_REG_IM( reg, index, mask, val )                    \
  MSM_DRV_SHADOW_IM( reg, index, mask, (val) );

/* Shadow register field update */
#define SET_SHADOW_DEMBACK_REG_F( reg, field, val )                           \
  MSM_DRV_SHADOW_W( reg, field, (val) );

/* Field-wise register set/clear */
#define SET_DEMBACK_REG_F( reg, field, set )                                  \
  if( set )                                                                   \
  {                                                                           \
    MSM_DRV_SET( reg, field );                                                \
  }                                                                           \
  else                                                                        \
  {                                                                           \
    MSM_DRV_CLEAR( reg, field );                                              \
  }

/* Get register address */
#define GET_DEMBACK_ADDR( reg )                                               \
  ( HWIO_ADDR( reg ) )

/* Read register */
#define GET_DEMBACK_REG( reg )                                                \
  ( HWIO_IN( reg ) )

/* Read register field */
#define GET_DEMBACK_REG_F( reg, field )                                       \
  ( HWIO_INF( reg, field ) )

/* Dword register read */
#define GET_DEMBACK_REG_DW( reg )                                             \
  ( in_dword( HWIO_ADDR( reg ) ) )

#else /* !FEATURE_MODEM_1X_SINGLE_DEMBACK */

/* Normal register update */
#define SET_DEMBACK_REG( reg, val )                                           \
  if( dec_demback_id == DEC_DEMBACK_0 )                                       \
  {                                                                           \
    HWIO_OUT( reg, (val) );                                                   \
  }                                                                           \
  else if( dec_demback_id == DEC_DEMBACK_1 )                                  \
  {                                                                           \
    HWIO_OUT( reg##_DB1, (val) );                                             \
  }                                                                           \

/* Masked register update */
#define SET_DEMBACK_REG_M( reg, mask, val )                                   \
  if( dec_demback_id == DEC_DEMBACK_0 )                                       \
  {                                                                           \
    HWIO_OUTM( reg, (mask), (val) );                                          \
  }                                                                           \
  else if( dec_demback_id == DEC_DEMBACK_1 )                                  \
  {                                                                           \
    HWIO_OUTM( reg##_DB1, (mask), (val) );                                    \
  }                                                                           \

/* Shadow register masked update */
#define SET_SHADOW_DEMBACK_REG_M( reg, mask, val )                            \
  if( dec_demback_id == DEC_DEMBACK_0 )                                       \
  {                                                                           \
    MSM_DRV_SHADOW_M( reg, mask, (val) );                                     \
  }                                                                           \
  else if( dec_demback_id == DEC_DEMBACK_1 )                                  \
  {                                                                           \
    MSM_DRV_SHADOW_M( reg##_DB1, mask, (val) );                               \
  }                                                                           \

/* Shadow register masked update */
#define SET_SHADOW_DEMBACK_REG_IM( reg, index, mask, val )                    \
  if( dec_demback_id == DEC_DEMBACK_0 )                                       \
  {                                                                           \
    MSM_DRV_SHADOW_IM( reg, index, mask, (val) );                             \
  }                                                                           \
  else if( dec_demback_id == DEC_DEMBACK_1 )                                  \
  {                                                                           \
    MSM_DRV_SHADOW_IM( reg##_DB1, index, mask, (val) );                       \
  }                                                                           \

/* Shadow register field update */
#define SET_SHADOW_DEMBACK_REG_F( reg, field, val )                           \
  if( dec_demback_id == DEC_DEMBACK_0 )                                       \
  {                                                                           \
    MSM_DRV_SHADOW_W( reg, field, (val) );                                    \
  }                                                                           \
  else if( dec_demback_id == DEC_DEMBACK_1 )                                  \
  {                                                                           \
    MSM_DRV_SHADOW_W( reg##_DB1, field, (val) );                              \
  }                                                                           \

/* Field-wise register set/clear */
#define SET_DEMBACK_REG_F( reg, field, set )                                  \
  if( dec_demback_id == DEC_DEMBACK_0 )                                       \
  {                                                                           \
    if( set )                                                                 \
    {                                                                         \
      MSM_DRV_SET( reg, field );                                              \
    }                                                                         \
    else                                                                      \
    {                                                                         \
      MSM_DRV_CLEAR( reg, field );                                            \
    }                                                                         \
  }                                                                           \
  else if( dec_demback_id == DEC_DEMBACK_1 )                                  \
  {                                                                           \
    if( set )                                                                 \
    {                                                                         \
      MSM_DRV_SET( reg##_DB1, field );                                        \
    }                                                                         \
    else                                                                      \
    {                                                                         \
      MSM_DRV_CLEAR( reg##_DB1, field );                                      \
    }                                                                         \
  }                                                                           \

/* Get register address */
#define GET_DEMBACK_ADDR( reg )                                               \
  ( ( dec_demback_id == DEC_DEMBACK_0 ) ?                                     \
          HWIO_ADDR( reg ) :                                                  \
          HWIO_ADDR( reg##_DB1 ) )

/* Read register */
#define GET_DEMBACK_REG( reg )                                                \
  ( ( dec_demback_id == DEC_DEMBACK_0 ) ?                                     \
          HWIO_IN( reg ) :                                                    \
          HWIO_IN( reg##_DB1 ) )

/* Read register field */
#define GET_DEMBACK_REG_F( reg, field )                                       \
  ( ( dec_demback_id == DEC_DEMBACK_0 ) ?                                     \
          HWIO_INF( reg, field ) :                                            \
          HWIO_INF( reg##_DB1, field ) )

/* Dword register read */
#define GET_DEMBACK_REG_DW( reg )                                             \
  ( ( dec_demback_id == DEC_DEMBACK_0 ) ?                                     \
          in_dword( HWIO_ADDR( reg ) ) :                                      \
          in_dword( HWIO_ADDR( reg##_DB1 ) ) )

#endif /* !FEATURE_MODEM_1X_SINGLE_DEMBACK */

/*===========================================================================

                        DEMODULATION REGISTERS

===========================================================================*/



/*===========================================================================

MACRO DEMODULATOR_RESET

DESCRIPTION     This macro resets the demodulator circuitry.

DEPENDENCIES    CHIPX8 must be oscillating.

RETURN VALUE    None.

SIDE EFFECTS    Too numerous to mention.

===========================================================================*/

#define DEMODULATOR_RESET( )    \
    MSM_DRV_PULSE( DEM1X_RESET, RESET )


/*==========================================================================
MACRO DEMOD_REP_FACTOR

DESCRIPTION    This macro sets the repetition factor in the demodulator.

DEPENDECIES    These macros must be called from inside the context of a
               *_LOCK / *_UNLOCK pair to ensure proper operation.


RETURN VALUE   None

SIDE EFFECTS   None

=============================================================================*/

#define DEMOD_REP_FACTOR( factor ) \
    MSM_DRV_W_MASKED( DEM1X_CHANNEL2_FRAME, REP_FACTOR, (factor) )

/*==========================================================================
MACRO DEMOD_SET_MAC_SEQ_REP_x

DESCRIPTION    These macros set the FFE_FINGER_CH2_TD_SIGN value
               for multi-frame interleaving.

DEPENDECIES    These macros must be called from inside the context of a
               *_LOCK / *_UNLOCK pair to ensure proper operation.


RETURN VALUE   None

SIDE EFFECTS   None

=============================================================================*/

#define DEMOD_SET_MAC_SEQ_REP_1( ) \
    DEMOD_REP_FACTOR( 0 )

#define DEMOD_SET_MAC_SEQ_REP_2( ) \
    DEMOD_REP_FACTOR( 1 )

#define DEMOD_SET_MAC_SEQ_REP_4( ) \
    DEMOD_REP_FACTOR( 2 )

/*==========================================================================
MACRO DEMOD_ENABLE_CH0_RC4
MACRO DEMOD_DISABLE_CH0_RC4

DESCRIPTION    These macros enable/disable indicate that channel 0 is in
               RC4.

DEPENDECIES    These macros must be called from inside the context of a
               *_LOCK / *_UNLOCK pair to ensure proper operation.


RETURN VALUE   None

SIDE EFFECTS   None

=============================================================================*/

#define DEMOD_ENABLE_CH0_RC4( ) \
    MSM_DRV_SET( DEM1X_CHANNEL0_IMMED, RC4 )

#define DEMOD_DISABLE_CH0_RC4( ) \
    MSM_DRV_CLEAR( DEM1X_CHANNEL0_IMMED, RC4 )

#define DEMOD_ENABLE_CH0_RC4_FRAME( ) \
    MSM_DRV_SET( DEM1X_CHANNEL0_FRAME, RC4 )

#define DEMOD_DISABLE_CH0_RC4_FRAME( ) \
    MSM_DRV_CLEAR( DEM1X_CHANNEL0_FRAME, RC4 )

/*==========================================================================
MACRO DEMOD_SET_CH0_TD_SIGN

DESCRIPTION    This macro sets the CH0_TD_SIGN value as requested for various
               RCs.

DEPENDECIES    These macros must be called from inside the context of a
               *_LOCK / *_UNLOCK pair to ensure proper operation.


RETURN VALUE   None

SIDE EFFECTS   None

=============================================================================*/

#ifdef HWIO_DEM1X_CHANNEL0_IMMED_TD_SIGN_BMSK
#define DEMOD_SET_CH0_TD_SIGN( value ) \
    MSM_DRV_W_MASKED( DEM1X_CHANNEL0_IMMED, TD_SIGN, (value) )
#else /* HWIO_DEM1X_CHANNEL0_IMMED_TD_SIGN_BMSK */
#define DEMOD_SET_CH0_TD_SIGN( value )
#endif /* !HWIO_DEM1X_CHANNEL0_IMMED_TD_SIGN_BMSK */

#define DEMOD_SET_CH0_TD_SIGN_FRAME( value ) \
    MSM_DRV_W_MASKED( DEM1X_CHANNEL0_FRAME, TD_SIGN, (value) )


/*==========================================================================
MACRO DEMOD_SET_CH1_TD_SIGN

DESCRIPTION    This macro sets the CH1_TD_SIGN value as requested for various
               RCs.

DEPENDECIES    These macros must be called from inside the context of a
               *_LOCK / *_UNLOCK pair to ensure proper operation.


RETURN VALUE   None

SIDE EFFECTS   None

=============================================================================*/

#ifdef HWIO_DEM1X_CHANNEL1_IMMED_TD_SIGN_BMSK
#define DEMOD_SET_CH1_TD_SIGN( value ) \
    MSM_DRV_W_MASKED( DEM1X_CHANNEL1_IMMED, TD_SIGN, (value) )
#else /* HWIO_DEM1X_CHANNEL1_IMMED_TD_SIGN_BMSK */
#define DEMOD_SET_CH1_TD_SIGN( value )
#endif /* !HWIO_DEM1X_CHANNEL1_IMMED_TD_SIGN_BMSK */

/*=========================================================================
MACRO DEMOD_SET_MAC_RND_128_64_CHIPS_CH1

DESCRIPTION  This macro sets MAC Engine CH1 rounding factors for spreading
              rate of 128 or 64 chips.

              For 6500 the rounding factors are moved as follows:

              MAC_RND_CH1 COMB_RND2 -> DEM1X_CHANNEL1 SOFTDEC_ACCUM_RND
              MAC_RND_CH2 COMB_RND2 -> DEM1X_CHANNEL2 SOFTDEC_ACCUM_RND
              MAC_RND_CH1 COMB_RND3 -> DEM1X_CHANNEL1 SOFTDEC_RND
              MAC_RND_CH2 COMB_RND3 -> DEM1X_CHANNEL2 SOFTDEC_RND

              MAC_RND_CH1 COMB_RND1 -> DEM1X_Fn_HW_CH1 RND
              MAC_RND_CH2 COMB_RND1 -> DEM1X_Fn_HW_CH2 RND

              Because the value that was in COMB_RND1 is now per-finger
              for MSM6500, this value is no longer written by these
              macros, but must be written per-finger, through the new
              macros FINGER_RND_CH1 and FINGER_RND_CH2.

              Also, the mapping from walsh length or decoder type to actual
              rounding factor is no longer done in the hardware.  The
              translation is as follows:

              DEM1X_CHANNEL1/DEM1X_CHANNEL2
              walsh length    frame length    SOFTDEC_ACCUM_RND
              128             80ms            1
              128             40,20ms         0
              64              any             0
              32              any             1
              16              any             1
              8               any             2
              4               any             2

              encoder type    truncation      SOFTDEC_RND
              convolutional   10 bits         0
              turbo           8 bits          1

RETURN VALUE  None

SIDE EFFECTS  None
==========================================================================*/

#define DEMOD_SET_MAC_RND_128_CHIPS_80MS_FRAME_CH1(decoder) \
    MSM_DRV_W_MASKED( DEM1X_CHANNEL1_FRAME, SOFTDEC_ACCUM_RND, 1); \
    MSM_DRV_W_MASKED( DEM1X_CHANNEL1_FRAME, SOFTDEC_RND, (decoder) )

#define DEMOD_SET_MAC_RND_128_64_CHIPS_CH1(decoder) \
    MSM_DRV_W_MASKED( DEM1X_CHANNEL1_FRAME, SOFTDEC_ACCUM_RND, 0); \
    MSM_DRV_W_MASKED( DEM1X_CHANNEL1_FRAME, SOFTDEC_RND, (decoder) )

/*=========================================================================
MACRO DEMOD_SET_MAC_RND_32_CHIPS_CH1

DESCRIPTION  This macro sets MAC Engine CH1 rounding factors for spreading
              rate of 32 chips.

RETURN VALUE  None

SIDE EFFECTS  None
==========================================================================*/

#define DEMOD_SET_MAC_RND_32_CHIPS_CH1(decoder ) \
    MSM_DRV_W_MASKED( DEM1X_CHANNEL1_FRAME, SOFTDEC_ACCUM_RND, 1); \
    MSM_DRV_W_MASKED( DEM1X_CHANNEL1_FRAME, SOFTDEC_RND, (decoder) )

/*=========================================================================
MACRO DEMOD_SET_MAC_RND_128_64_CHIPS

DESCRIPTION  This macro sets MAC Engine CH2 rounding factors for spreading
              rate of 128 or 64 chips.

RETURN VALUE  None

SIDE EFFECTS  None
==========================================================================*/


#define DEMOD_SET_MAC_RND_128_CHIPS_80MS_FRAME(decoder) \
    MSM_DRV_W_MASKED( DEM1X_CHANNEL2_FRAME, SOFTDEC_ACCUM_RND, 1); \
    MSM_DRV_W_MASKED( DEM1X_CHANNEL2_FRAME, SOFTDEC_RND, (decoder) )

#define DEMOD_SET_MAC_RND_128_64_CHIPS(decoder) \
    MSM_DRV_W_MASKED( DEM1X_CHANNEL2_FRAME, SOFTDEC_ACCUM_RND, 0); \
    MSM_DRV_W_MASKED( DEM1X_CHANNEL2_FRAME, SOFTDEC_RND, (decoder) )

/*=========================================================================
MACRO DEMOD_SET_MAC_RND_32_16_CHIPS

DESCRIPTION  This macro sets MAC Engine rounding factors for spreading
              rate of 32 or 16 chips.

RETURN VALUE  None

SIDE EFFECTS  None
==========================================================================*/

#define DEMOD_SET_MAC_RND_32_16_CHIPS(decoder ) \
    MSM_DRV_W_MASKED( DEM1X_CHANNEL2_FRAME, SOFTDEC_ACCUM_RND, 1); \
    MSM_DRV_W_MASKED( DEM1X_CHANNEL2_FRAME, SOFTDEC_RND, (decoder) )

/*=========================================================================
MACRO DEMOD_SET_MAC_RND_8_4_CHIPS

DESCRIPTION  This macro sets MAC Engine rounding factors for spreading
             rate of 8 or 4 chips.

RETURN VALUE  None

SIDE EFFECTS  None
==========================================================================*/

#define DEMOD_SET_MAC_RND_8_4_CHIPS(decoder ) \
    MSM_DRV_W_MASKED( DEM1X_CHANNEL2_FRAME, SOFTDEC_ACCUM_RND, 2); \
    MSM_DRV_W_MASKED( DEM1X_CHANNEL2_FRAME, SOFTDEC_RND, (decoder) )

/*=========================================================================
MACRO DEMOD_SET_MAC_RND_64_CHIPS_CH3

DESCRIPTION  This macro sets MAC Engine rounding factors for spreading
             rate of 64 chips.

RETURN VALUE  None

SIDE EFFECTS  None
==========================================================================*/

#define DEMOD_SET_MAC_RND_64_CHIPS_CH3(decoder) \
     MSM_DRV_W_MASKED( DEM1X_CHANNEL3_FRAME, SOFTDEC_ACCUM_RND, 0); \
     MSM_DRV_W_MASKED( DEM1X_CHANNEL3_FRAME, SOFTDEC_RND, (decoder) )

/*=====================================================================
MACRO DEMOD_SET_CHx_SPR

DESCRIPTION These macros control the spreading rate of the demodulator
            channels 1 & 2.

RETURN VALUE None

SIDE EFFECTS None

======================================================================*/

#define DEMOD_SET_CH1_SPR(spr)\
    MSM_DRV_W_MASKED( DEM1X_CHANNEL1_FRAME, SPR_MASK, (spr) )

#define DEMOD_SET_CH2_SPR(spr)\
    MSM_DRV_W_MASKED( DEM1X_CHANNEL2_FRAME, SPR_MASK, (spr) )

#define DEMOD_SET_CH3_SPR(spr)\
    MSM_DRV_W_MASKED( DEM1X_CHANNEL3_FRAME, SPR_MASK, (spr) )

/*=====================================================================
MACRO DEMOD_SET_SW_BETA_CH0

DESCRIPTION  This macro sets the FCH frame scaling factor for demod soft
             decison symbols to the SVD, for Channel 0.

RETURN VALUE  None

SIDE EFFECTS  This scaling factor takes effect at the next frame boundary.
=========================================================================*/

#define DEMOD_SET_SW_BETA_CH0_IMMED( scale ) \
    MSM_DRV_W_MASKED( DEM1X_CHANNEL0_IMMED, SW_BETA, scale  )

#define DEMOD_SET_SW_BETA_CH0( scale ) \
    MSM_DRV_W_MASKED( DEM1X_CHANNEL0_FRAME, SW_BETA, scale  )

/*=====================================================================
MACRO DEMOD_SET_SW_BETA_CH1

DESCRIPTION  This macro sets the FCH frame scaling factor for demod soft
             decison symbols to the SVD, for Channel 1.

RETURN VALUE  None

SIDE EFFECTS  This scaling factor takes effect at the next frame boundary.
=========================================================================*/

#define DEMOD_SET_SW_BETA_CH1_IMMED( scale ) \
    MSM_DRV_W_MASKED( DEM1X_CHANNEL1_IMMED, SW_BETA, scale  )

#define DEMOD_SET_SW_BETA_CH1( scale ) \
    MSM_DRV_W_MASKED( DEM1X_CHANNEL1_FRAME, SW_BETA, scale  )

/*=====================================================================
MACRO DEMOD_SET_SW_BETA_CH2

DESCRIPTION  This macro sets the SCH frame scaling factor for demod soft
             decison symbols to the SVD, for Channel 2.

RETURN VALUE  None

SIDE EFFECTS  This scaling factor takes effect at the next frame boundary.
=========================================================================*/

#define DEMOD_SET_SW_BETA_CH2( scale ) \
    MSM_DRV_W_MASKED( DEM1X_CHANNEL2_FRAME, SW_BETA, (scale)  )

#define DEMOD_SET_SW_BETA_CH3( scale ) \
    MSM_DRV_W_MASKED( DEM1X_CHANNEL3_FRAME, SW_BETA, (scale)  )

/*==========================================================================
MACRO DEMOD_ENABLE_Q_CHANNEL

DESCRIPTION    This macro sets the demodulator to send Q channel soft decisions
               to the Viterbi Decoder. Enable Q Channel when receiving RC3, RC4
               and RC5 Traffic Channel Frames.

DEPENDECIES    These macros must be called from inside the context of a
               *_LOCK / *_UNLOCK pair to ensure proper operation.


RETURN VALUE   None

SIDE EFFECTS   None.

=============================================================================*/

#define DEMOD_ENABLE_Q_CHANNEL( ) \
    MSM_DRV_CLEAR( DEM1X_COMBINER_CTL, Q_CHAN_OFF )

/*==========================================================================
MACRO DEMOD_DISABLE_Q_CHANNEL

DESCRIPTION    This macro stops the demodulator from sending Q channel soft decisions
               to the Viterbi Decoder. Disable Q channel when receiving Sync, Paging,
               RC1 & RC2 traffic channel frames

DEPENDECIES    These macros must be called from inside the context of a
               *_LOCK / *_UNLOCK pair to ensure proper operation.


RETURN VALUE   None

SIDE EFFECTS   None.

=============================================================================*/

#define DEMOD_DISABLE_Q_CHANNEL( ) \
    MSM_DRV_SET( DEM1X_COMBINER_CTL, Q_CHAN_OFF )

/*===========================================================================

                  DEMODULATOR SEARCH ENGINE REGISTERS

===========================================================================*/


/*===========================================================================

MACRO SEARCH_ANT_SELECT

DESCRIPTION     This macro selects the antenna to search from for searcher1.

DEPENDENCIES    These macros must be called from inside the context of a
                *_LOCK / *_UNLOCK pair to ensure proper operation.

                DMA may only be used with the 1x searcher.

RETURN VALUE    None.

SIDE EFFECTS    None.

===========================================================================*/

#define SEARCH_ANT_SELECT( ant )



/*===========================================================================

                        DEMODULATOR SYMBOL COMBINER

===========================================================================*/

/*===========================================================================

MACRO COMBINER_ENABLE_POWER_CONTROL
MACRO COMBINER_DISABLE_POWER_CONTROL

DESCRIPTION     These macros enable and disable closed loop power control

DEPENDENCIES    This function must be called from inside the context of a
                *_LOCK / *_UNLOCK pair to ensure proper operation.

RETURN VALUE    None.

SIDE EFFECTS    None.

===========================================================================*/
#define COMBINER_ENABLE_POWER_CONTROL( )                             \
  msmdrv_enable_rl_power_control()

#define COMBINER_DISABLE_POWER_CONTROL( )                            \
  msmdrv_disable_rl_power_control()

/*===========================================================================

           CDMA AGC, FREQUENCY TRACKING AND BASEBAND OFFSET CONTROL

===========================================================================*/

/*===========================================================================

MACRO TX_AGC_RESET

DESCRIPTION     This macro resets the tx AGC.

DEPENDENCIES    This macro must be called from inside the context of a
                *_LOCK / *_UNLOCK pair to ensure proper operation.

RETURN VALUE    None.

SIDE EFFECTS    None.

===========================================================================*/

#define TX_AGC_RESET() \
    MSM_DRV_OUTC( TX_AGC_RESET, DATA, 0x1 )


/*===========================================================================

FUNCTION MSMDRV_ENABLE_RL_POWER_CONTROL

DESCRIPTION
  Enable reverse link closed loop power control

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void msmdrv_enable_rl_power_control (void);

/*===========================================================================

FUNCTION MSMDRV_ENABLE_RL_POWER_CONTROL

DESCRIPTION
  Enable reverse link closed loop power control

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void msmdrv_disable_rl_power_control (void);

/*===========================================================================

MACRO COMBINER_SET_NUM_SCCH_SOFT_COMBINED

DESCRIPTION     This macro enables/disables the  supplemental code channels
                being soft combined.

DEPENDENCIES    None.

RETURN VALUE    None.

SIDE EFFECTS    None.

===========================================================================*/

#define COMBINER_SET_NUM_SCCH_SOFT_COMBINED( chan_mask ) \
    MSM_DRV_W( DEM1X_FW_CH_ENABLE_IMMED, chan_mask )

#define COMBINER_SET_NUM_SCCH_SOFT_COMBINED_FRAME( chan_mask ) \
    MSM_DRV_W( DEM1X_FW_CH_ENABLE_FRAME, chan_mask )


/*===========================================================================

MACRO COMBINER_CHANNEL_ENABLE_CHx
MACRO COMBINER_CHANNEL_DISABLE_CHx

DESCRIPTION     These macros enable and disable the channels that are
                demodulated in the combiner.

DEPENDENCIES    This function must be called from inside the context of a
                *_LOCK / *_UNLOCK pair to ensure proper operation.

RETURN VALUE    None.

SIDE EFFECTS

===========================================================================*/

#define COMBINER_ENABLE_CH0( )                     \
    MSM_DRV_SET_BIT( DEM1X_FW_CH_ENABLE_IMMED, 0 );    \
    MSM_DRV_CLEAR( DEM1X_CHANNEL0_IMMED, CH0_DISABLE )

#define COMBINER_DISABLE_CH0( )                    \
    MSM_DRV_CLEAR_BIT( DEM1X_FW_CH_ENABLE_IMMED, 0 );  \
    MSM_DRV_SET( DEM1X_CHANNEL0_IMMED, CH0_DISABLE )

/*===========================================================================

MACRO COMBINER_ENABLE_PUNCTURING
MACRO COMBINER_DISABLE_PUNCTURING

DESCRIPTION     These macros enable and disable puncturing (erasures) in the
                combined data streams.

DEPENDENCIES    These macros must be called from inside the context of a
                *_LOCK / *_UNLOCK pair to ensure proper operation.

RETURN VALUE    None.

SIDE EFFECTS    None.

===========================================================================*/

#define COMBINER_ENABLE_PUNCTURING( ) \
    MSM_DRV_SET( DEM1X_TRAFFIC_REV_PWR_CTL, ERASE_EN )

#define COMBINER_DISABLE_PUNCTURING( ) \
    MSM_DRV_CLEAR( DEM1X_TRAFFIC_REV_PWR_CTL, ERASE_EN )

/*===========================================================================

MACRO COMBINER_SET_INIT_PWR_CTL_OFFSET1
MACRO COMBINER_SET_INIT_PWR_CTL_OFFSET2
MACRO COMBINER_SET_INIT_PWR_CTL_OFFSET3
MACRO COMBINER_SET_INIT_PWR_CTL_OFFSET4
MACRO COMBINER_SET_INIT_PWR_CTL_OFFSET5
MACRO COMBINER_SET_INIT_PWR_CTL_OFFSET6

DESCRIPTION     These macros set the initial offset for power control.
                For 6500 it is for Cell A and Cell B.  For 6700 it is
                per sector, from 1-6.

DEPENDENCIES    These macros must be called from inside the context of a
                *_LOCK / *_UNLOCK pair to ensure proper operation.

RETURN VALUE    None.

SIDE EFFECTS    None.

===========================================================================*/

#define COMBINER_SET_INIT_PWR_CTL_OFFSET1( offset ) \
     MSM_DRV_W_MASKED( DEM1X_COMMON_REV_PWR_CTL,  INIT_OFFSET1, (offset) )
#define COMBINER_SET_INIT_PWR_CTL_OFFSET2( offset ) \
     MSM_DRV_W_MASKED( DEM1X_COMMON_REV_PWR_CTL,  INIT_OFFSET2, (offset) )

#define COMBINER_SET_INIT_PWR_CTL_OFFSET3( offset ) \
     MSM_DRV_W_MASKED( DEM1X_COMMON_REV_PWR_CTL,  INIT_OFFSET3, (offset) )
#define COMBINER_SET_INIT_PWR_CTL_OFFSET4( offset ) \
     MSM_DRV_W_MASKED( DEM1X_COMMON_REV_PWR_CTL,  INIT_OFFSET4, (offset) )
#define COMBINER_SET_INIT_PWR_CTL_OFFSET5( offset ) \
     MSM_DRV_W_MASKED( DEM1X_COMMON_REV_PWR_CTL2, INIT_OFFSET5, (offset) )
#define COMBINER_SET_INIT_PWR_CTL_OFFSET6( offset ) \
     MSM_DRV_W_MASKED( DEM1X_COMMON_REV_PWR_CTL2, INIT_OFFSET6, (offset) )

/*===========================================================================

MACRO COMBINER_SET_VALID_SECTORS

DESCRIPTION     The write to the VALID_SECTOR part of this register
                indicates to the hardware which initial offset values
                (as written by COMBINER_SET_INIT_PWR_CTL_OFFSET macros)
                are valid.

DEPENDENCIES    These macros must be called from inside the context of a
                *_LOCK / *_UNLOCK pair to ensure proper operation.

RETURN VALUE    None.

SIDE EFFECTS    None.

===========================================================================*/

#define COMBINER_SET_VALID_SECTORS( valid_sectors ) \
     MSM_DRV_W_MASKED( DEM1X_COMMON_REV_PWR_CTL2, VALID_SECTOR, (valid_sectors) )

/*===========================================================================

MACRO COMBINER_SET_PC_RATE

DESCRIPTION     This macro sets the power control rate.

DEPENDENCIES    These macros must be called from inside the context of a
                *_LOCK / *_UNLOCK pair to ensure proper operation.

RETURN VALUE    None.

SIDE EFFECTS    None.

===========================================================================*/

#define COMBINER_SET_PC_RATE( rate ) \
     MSM_DRV_W_MASKED( DEM1X_COMMON_REV_PWR_CTL, PC_RATE_SEL, (rate) )

/*==========================================================================
MACRO COMBINER_SET_CH_SYMBOL_RATE

DESCRIPTION     This macro sets the combiner to accumulate symbols for a
                specific number of chips, on a specific channel.

DEPENDENCIES    These macros must be called from inside the context of a
                *_LOCK / *_UNLOCK pair to ensure proper operation.

RETURN VALUE    None.

SIDE EFFECTS    None.

===========================================================================*/

#define COMBINER_SET_CH_SYMBOL_RATE( chan, rate ) \
     MSM_DRV_W_MASKED( DEM1X_CHANNEL##chan##_IMMED, RATE_SEL, (rate) )

/*==========================================================================
MACRO COMBINER_SELECT_CH0_128_CHIP_SYMBOL
MACRO COMBINER_SELECT_CH1_128_CHIP_SYMBOL

DESCRIPTION   This macro sets the combiner to accumulate symbols over two PN
              words(128 chips). This should be done when demodulating RC4 frames and
              for 9.6kbps Quick Paging.


DEPENDENCIES    These macros must be called from inside the context of a
                *_LOCK / *_UNLOCK pair to ensure proper operation.

RETURN VALUE    None.

SIDE EFFECTS    None.

===========================================================================*/
#define COMBINER_SELECT_CH0_128_CHIP_SYMBOL( ) \
     COMBINER_SET_CH_SYMBOL_RATE( 0, DMOD_SPR_128 )

#define COMBINER_SELECT_CH1_128_CHIP_SYMBOL( ) \
     COMBINER_SET_CH_SYMBOL_RATE( 1, DMOD_SPR_128 )

/*==========================================================================
MACRO COMBINER_SELECT_CH0_64_CHIP_SYMBOL
MACRO COMBINER_SELECT_CH1_64_CHIP_SYMBOL

DESCRIPTION   This macro sets the combiner to accumulate symbols over 64 chips.
              This should be done when demodulating RC1, RC2, RC3, RC5 and
              Paging Channel frames.


DEPENDENCIES    These macros must be called from inside the context of a
                *_LOCK / *_UNLOCK pair to ensure proper operation.

RETURN VALUE    None.

SIDE EFFECTS    None.

===========================================================================*/
#define COMBINER_SELECT_CH0_64_CHIP_SYMBOL( ) \
     COMBINER_SET_CH_SYMBOL_RATE( 0, DMOD_SPR_64 )

#define COMBINER_SELECT_CH1_64_CHIP_SYMBOL( ) \
     COMBINER_SET_CH_SYMBOL_RATE( 1, DMOD_SPR_64 )

/*==========================================================================
MACRO COMBINER_SELECT_CH0_256_CHIP_SYMBOL
MACRO COMBINER_SELECT_CH1_256_CHIP_SYMBOL

DESCRIPTION   This macro sets the combiner to accumulate symbols over 256 chips.
              This should be done when demodulating SYNC Channel.

DEPENDENCIES    These macros must be called from inside the context of a
                *_LOCK / *_UNLOCK pair to ensure proper operation.

RETURN VALUE    None.

SIDE EFFECTS    None.

===========================================================================*/
#define COMBINER_SELECT_CH0_256_CHIP_SYMBOL( ) \
     COMBINER_SET_CH_SYMBOL_RATE( 0, DMOD_SPR_256 )

#define COMBINER_SELECT_CH1_256_CHIP_SYMBOL( ) \
     COMBINER_SET_CH_SYMBOL_RATE( 1, DMOD_SPR_256 )

/*==========================================================================
MACRO COMBINER_SELECT_CH0_512_CHIP_SYMBOL
MACRO COMBINER_SELECT_CH1_512_CHIP_SYMBOL

DESCRIPTION   This macro sets the combiner to accumulate symbols over 256 chips.
              This should be done when demodulating SYNC Channel.

DEPENDENCIES    These macros must be called from inside the context of a
                *_LOCK / *_UNLOCK pair to ensure proper operation.

RETURN VALUE    None.

SIDE EFFECTS    None.

===========================================================================*/
#define COMBINER_SELECT_CH0_512_CHIP_SYMBOL( ) \
     COMBINER_SET_CH_SYMBOL_RATE( 0, DMOD_SPR_512 )

#define COMBINER_SELECT_CH1_512_CHIP_SYMBOL( ) \
     COMBINER_SET_CH_SYMBOL_RATE( 1, DMOD_SPR_512 )

/*===========================================================================
MACRO COMBINER_PUNCTURE_1_SYM
MACRO COMBINER_PUNCTURE_2_SYM

DESCRIPTION     These macros control whether 1 or 2 symbols are punctured.

DEPENDENCIES    These macros must be called from inside the context of a
                *_LOCK / *_UNLOCK pair to ensure proper operation.

RETURN VALUE    None.

SIDE EFFECTS    None.

===========================================================================*/

#define COMBINER_PUNCTURE_1_SYM( ) \
    MSM_DRV_SET( DEM1X_TRAFFIC_REV_PWR_CTL, POWER_CTL_PCT )

#define COMBINER_PUNCTURE_2_SYM( ) \
    MSM_DRV_CLEAR( DEM1X_TRAFFIC_REV_PWR_CTL, POWER_CTL_PCT )

/*===========================================================================

MACRO COMBINER_TIMETRACK_FINGER

DESCRIPTION     This macro selects the finger for the symbol combiner to
                timetrack.

DEPENDENCIES    This macro must be called from inside the context of a
                *_LOCK / *_UNLOCK pair to ensure proper operation.

RETURN VALUE    None.

SIDE EFFECTS    None.

===========================================================================*/

#define COMBINER_TIMETRACK_FINGER( finger ) \
    MSM_DRV_W_MASKED( DEM1X_TRACKING, FINGER, (finger) ); \
    MSM_DRV_SET( DEM1X_TRACKING, ENABLE )

/*===========================================================================

MACRO COMBINER_TIMETRACK_DISABLE

DESCRIPTION     This macro selects no finger for the symbol combiner to
                timetrack.

DEPENDENCIES    This macro must be called from inside the context of a
                *_LOCK / *_UNLOCK pair to ensure proper operation.

RETURN VALUE    None.

SIDE EFFECTS    None.

===========================================================================*/

#define COMBINER_TIMETRACK_DISABLE(  ) \
    MSM_DRV_CLEAR( DEM1X_TRACKING, ENABLE )

/*===========================================================================

MACRO COMBINER_CURRENT_TIMETRACK_FINGER

DESCRIPTION     This macro returns the finger currently being tracked by the
                combiner.

DEPENDENCIES    Requires the RXCHIPX8 clock regime.

RETURN VALUE    The finger in question.

SIDE EFFECTS    None.

===========================================================================*/

#ifdef FEATURE_MODEM_1X_BOLT_REVISIT
/* REVISIT: Vamsee to check */
#define COMBINER_CURRENT_TIMETRACK_FINGER() 0xBAD
#else /* !FEATURE_MODEM_1X_BOLT_REVISIT */
#define COMBINER_CURRENT_TIMETRACK_FINGER() \
    MSM_DRV_R_MASKED(DEM1X_TRACKING, FINGER)
#endif /* FEATURE_MODEM_1X_BOLT_REVISIT */


/*===========================================================================

MACRO COMBINER_ARM_SLAM

DESCRIPTION     This macro arms a slam to a finger.

DEPENDENCIES    This macro must be called from inside the context of a
                *_LOCK / *_UNLOCK pair to ensure proper operation.

RETURN VALUE    None.

SIDE EFFECTS    None.

===========================================================================*/

#define COMBINER_ARM_SLAM( finger ) \
    COMBINER_TIMETRACK_FINGER( finger ); \
    MSM_DRV_OUTC( DEM1X_ARM_COMBINER_SLAM, SET, 0x1 )

/*===========================================================================

MACRO COMBINER_SET_DELAY

DESCRIPTION     This macro sets the combiner count relative to the finger
                the combiner is tracking.  The value is loaded on the PN
                roll of that finger if a slam is armed, or when
                DEM1X_SYNC:COMB_TIME cmd is issued.

DEPENDENCIES    This macro must be called from inside the context of a
                *_LOCK / *_UNLOCK pair to ensure proper operation.

RETURN VALUE    None.

SIDE EFFECTS    None.

===========================================================================*/

#define COMBINER_SET_DELAY( pos, systime80ms ) \
    MSM_DRV_W_MASKED( DEM1X_COMBINER_TIME_LOAD, SYS_TIME, systime80ms ); \
    MSM_DRV_W_MASKED( DEM1X_COMBINER_TIME_LOAD, TIMECHIPX8, pos )

/*===========================================================================

MACRO QPCH_SET_ION_QON

DESCRIPTION     This macro sets the firmware I_on and Q_on flags to help
                decode the correct PI bit.

DEPENDENCIES    This macro must be called from inside the context of a
                *_LOCK / *_UNLOCK pair to ensure proper operation.

RETURN VALUE    None.

SIDE EFFECTS    None.

===========================================================================*/
#define QPCH_SET_ION_QON( i_on, q_on ) \
    MSM_DRV_W_MASKED( DEM1X_CHANNEL0_IMMED, QP_I_ON, (i_on) ); \
    MSM_DRV_W_MASKED( DEM1X_CHANNEL0_IMMED, QP_Q_ON, (q_on) )

/*===========================================================================

MACRO COMBINER_ENABLE
MACRO COMBINER_DISABLE


DESCRIPTION     These macros enable and disable the symbol combiner.

DEPENDENCIES    This macro must be called from inside the context of a
                *_LOCK / *_UNLOCK pair to ensure proper operation.

RETURN VALUE    None.

SIDE EFFECTS    Combiner is in low-power mode when disabled.

===========================================================================*/
#define COMBINER_ENABLE( ) \
    MSM_DRV_SET( DEM1X_COMBINER_CTL, ENABLE )

#define COMBINER_DISABLE( ) \
    MSM_DRV_CLEAR( DEM1X_COMBINER_CTL, ENABLE )


/*===========================================================================

MACRO COMBINER_FREQ_TRACK_ENABLE
MACRO COMBINER_FREQ_TRACK_DISABLE


DESCRIPTION     These macros set/clear the ft_enable to the DSP.

DEPENDENCIES    This macro must be called from inside the context of a
                *_LOCK / *_UNLOCK pair to ensure proper operation.

RETURN VALUE    None.

SIDE EFFECTS    None.

===========================================================================*/

#define COMBINER_FREQ_TRACK_ENABLE( ) \
        MSM_DRV_SET( DEM1X_COMBINER_CTL, FREQ_TRACK_EN )

#define COMBINER_FREQ_TRACK_DISABLE( ) \
        MSM_DRV_CLEAR( DEM1X_COMBINER_CTL, FREQ_TRACK_EN )

/*===========================================================================

MACRO DEM1X_LATCH_ALL

DESCRIPTION     This macro causes the symbol combiner's position, the RTC
                position and the LC state to be latched.

DEPENDENCIES    This macro must be called from inside the context of a
                *_LOCK / *_UNLOCK pair to ensure proper operation.

RETURN VALUE    None.

SIDE EFFECTS    None.

===========================================================================*/
/* On DIME it was found that reading RTC_CNT immediately after LATCH
  resulted in 0. Hence, a dummy RTC_CNT_1X read was added to introduce
  delay between setting LATCH and reading the latched value. */
#define DEM1X_LATCH_DELAY() (void) HWIO_IN(RTC_CNT_1X)

#define DEM1X_LATCH_ALL( ) \
    HWIO_OUT(DEM1X_LATCH, HWIO_FMSK(DEM1X_LATCH,REF_COUNT) | \
                          HWIO_FMSK(DEM1X_LATCH,LC_STATE)  | \
                          HWIO_FMSK(DEM1X_LATCH,COMB_TIME)); \
    DEM1X_LATCH_DELAY()

/*===========================================================================

MACRO DEM1X_LATCH_RTC_AND_SC

DESCRIPTION     This macro causes the symbol combiner's position and RTC
                to be latched.

DEPENDENCIES    This macro must be called from inside the context of a
                *_LOCK / *_UNLOCK pair to ensure proper operation.

RETURN VALUE    None.

SIDE EFFECTS    None.

===========================================================================*/

#define DEM1X_LATCH_RTC_AND_SC( ) \
    HWIO_OUT(DEM1X_LATCH, HWIO_FMSK(DEM1X_LATCH,REF_COUNT) | \
                          HWIO_FMSK(DEM1X_LATCH,COMB_TIME)); \
    DEM1X_LATCH_DELAY()

/*===========================================================================

MACRO COMBINER_LATCH_POSITION

DESCRIPTION     This macro latches the symbol combiner's position.

DEPENDENCIES    This macro must be called from inside the context of a
                *_LOCK / *_UNLOCK pair to ensure proper operation.

RETURN VALUE    None.

SIDE EFFECTS    None.

===========================================================================*/

#define COMBINER_LATCH_POSITION( ) \
    MSM_DRV_OUTC( DEM1X_LATCH, COMB_TIME, 0x1 ); \
    DEM1X_LATCH_DELAY()

/*===========================================================================

MACRO RTC_LATCH_POSITION

DESCRIPTION     This macro latches the RTC's position.

DEPENDENCIES    This macro must be called from inside the context of a
                *_LOCK / *_UNLOCK pair to ensure proper operation.

RETURN VALUE    None.

SIDE EFFECTS    None.

===========================================================================*/

#define RTC_LATCH_POSITION( ) \
    MSM_DRV_OUTC( DEM1X_LATCH, REF_COUNT, 0x1 ); \
    DEM1X_LATCH_DELAY()

/*===========================================================================

MACRO DEM1X_RTC_GET_COUNT

DESCRIPTION     This macro reads the latest reference count value that
                was latched when DEM1X_LATCH:REF_COUNT was set.
                For QLIC enabled targets, this value gets retarded by
                DEM1X_RTC_ADJUST symbols. So, account for this delay
                and return proper (unretarded) RTC count value.

DEPENDENCIES    This macro must be called from inside the context of a
                *_LOCK / *_UNLOCK pair to ensure proper operation.

RETURN VALUE    The RTC as of the write to DEM1X_LATCH:REF_COUNT.

SIDE EFFECTS    None.

===========================================================================*/

#define DEM1X_RTC_GET_COUNT( ref_cnt )                       \
    ( (ref_cnt) = MSM_DRV_R_MASKED( DEM1X_REF_COUNT, LATCHED ) )

/*===========================================================================

MACRO DEM1X_COMBINER_GET_COUNT

DESCRIPTION     This macro reads the latest combiner count value that
                was latched when DEM1X_LATCH:COMB_TIME was set.

DEPENDENCIES    This macro must be called from inside the context of a
                *_LOCK / *_UNLOCK pair to ensure proper operation.

RETURN VALUE    The combiner count as of the write to DEM1X_LATCH:COMB_TIME.

SIDE EFFECTS    None.

===========================================================================*/

#define DEM1X_COMBINER_GET_COUNT( sc_pos ) \
    ( (sc_pos) = MSM_DRV_R_MASKED( DEM1X_COMBINER_TIME, LATCHED ) )

/*===========================================================================

MACRO DEM1X_COMBINER_GET_80MS_COUNT

DESCRIPTION     This macro reads the latest combiner count value that
                was latched when DEM1X_LATCH:COMB_TIME was set.

DEPENDENCIES    This macro must be called from inside the context of a
                *_LOCK / *_UNLOCK pair to ensure proper operation.

RETURN VALUE    The combiner count as of the write to DEM1X_LATCH:COMB_TIME.

SIDE EFFECTS    None.

===========================================================================*/

#define DEM1X_COMBINER_GET_80MS_COUNT( sc_pos ) \
    ( (sc_pos) = (MSM_DRV_R_MASKED( DEM1X_COMBINER_TIME, LATCHED ) & \
                COMBINER_80MS_COUNT_MASK )>> 20 )

/*===========================================================================

MACRO DEM1X_COMBINER_GET_SUB80MS_COUNT

DESCRIPTION     This macro reads the latest combiner count value that
                was latched when DEM1X_LATCH:COMB_TIME was set.

DEPENDENCIES    This macro must be called from inside the context of a
                *_LOCK / *_UNLOCK pair to ensure proper operation.

RETURN VALUE    The combiner count as of the write to DEM1X_LATCH:COMB_TIME.

SIDE EFFECTS    None.

===========================================================================*/

#define DEM1X_COMBINER_GET_SUB80MS_COUNT() \
    ( MSM_DRV_R_MASKED( DEM1X_COMBINER_TIME, LATCHED ) & \
      COMBINER_80MS_CX8_MASK )


/*===========================================================================

MACRO COMBINER_GET_POSITION

DESCRIPTION     This macro reads the latched symbol combiner's position.

DEPENDENCIES    This macro and the DMOD_SYMB_COMB_LATCH_POS function must
                be called from inside the context of a MSM_LOCK /
                MSM_UNLOCK pair to ensure proper operation.

RETURN VALUE    None.

SIDE EFFECTS    None.

===========================================================================*/

#define COMBINER_GET_POSITION() \
    /*lint -e{611} Suspicious cast. */ \
    ( ( MSM_DRV_R_MASKED( DEM1X_COMBINER_TIME, LATCHED ) &     \
        COMBINER_80MS_CX8_MASK ) >> 4 )
    /* converted to 2-chip and eliminated upper 80ms - that is what we get
       from the old macro */

/*===========================================================================

MACRO COMBINER_SLEW

DESCRIPTION     This macro slews the symbol combiner position to chipx8
                resolution.

DEPENDENCIES    This macro must be called from inside the context of a
                *_LOCK / *_UNLOCK pair to ensure proper operation.

RETURN VALUE    None.

SIDE EFFECTS    The symbol combiner position is changed.

===========================================================================*/

#define COMBINER_SLEW( slew, systime80ms )                          \
    MSM_DRV_W( DEM1X_SLEW_COMBINER_TIME,                                \
           ((slew) & HWIO_DEM1X_SLEW_COMBINER_TIME_OFFSET_BMSK)  |  \
           ( ((systime80ms) <<                                      \
             HWIO_DEM1X_SLEW_COMBINER_TIME_UPP_SYS_TIME_SHFT) &     \
             HWIO_DEM1X_SLEW_COMBINER_TIME_UPP_SYS_TIME_BMSK ) )

/*===========================================================================

MACRO COMBINER_SLEW_SUB80MS

DESCRIPTION     This macro slews the symbol combiner position to chipx8
                resolution, but only adjusts the sub-80ms count.

DEPENDENCIES    This macro must be called from inside the context of a
                *_LOCK / *_UNLOCK pair to ensure proper operation.

RETURN VALUE    None.

SIDE EFFECTS    The symbol combiner position is changed.

===========================================================================*/

#define COMBINER_SLEW_SUB80MS( slew ) \
    MSM_DRV_W( DEM1X_SLEW_COMBINER_TIME, (slew) & COMBINER_80MS_CX8_MASK )

/*===========================================================================

MACRO COMBINER_SET_LONG_CODE_STATE
MACRO COMBINER_ZERO_LONG_CODE_STATE

DESCRIPTION     These macros set or zero the long code state.

DEPENDENCIES    These macros must be called from inside the context of a
                *_LOCK / *_UNLOCK pair to ensure proper operation.

                The lc_state argument must be l-value.

RETURN VALUE    None.

SIDE EFFECTS    The long code is transfered from the load register to the
                long code generator on the next PN roll.

===========================================================================*/

#define COMBINER_SET_LONG_CODE_STATE( lc_state )          \
    MSM_DRV_W( DEM1X_LC_STATE_LOAD_LO, D_PTR(lc_state)[0] );  \
    /*lint -save -e415 Access of out-of-bounds pointer */ \
    MSM_DRV_W( DEM1X_LC_STATE_LOAD_HI, D_PTR(lc_state)[1] );  \
    /*lint -restore */                                    \
    MSM_DRV_OUTC( DEM1X_ARM_LC_STATE_LOAD, SET, 0x1 )

#define COMBINER_ZERO_LONG_CODE_STATE( ) \
    MSM_DRV_W( DEM1X_LC_STATE_LOAD_LO, DEM1X_ZERO ); \
    MSM_DRV_W( DEM1X_LC_STATE_LOAD_HI, DEM1X_ZERO ); \
    MSM_DRV_OUTC( DEM1X_ARM_LC_STATE_LOAD, SET, 0x1 )


/*===========================================================================

MACRO COMBINER_GET_LONG_CODE_STATE

DESCRIPTION     This macro gets the long code state as of the last PN roll.

DEPENDENCIES    This macro must be called from inside the context of a
                *_LOCK / *_UNLOCK pair to ensure proper operation.

                The lc_state argument must be l-value.

RETURN VALUE    None.

SIDE EFFECTS    None.

===========================================================================*/

#define COMBINER_GET_LONG_CODE_STATE( lc_state ) \
      D_PTR(lc_state)[0] = MSM_DRV_R_MASKED( DEM1X_LC_STATE_LO, LATCHED ), \
      D_PTR(lc_state)[1] = MSM_DRV_R_MASKED( DEM1X_LC_STATE_HI, LATCHED )


/*===========================================================================

MACRO COMBINER_SET_LONG_CODE_MASK_CHAN_ROLL
MACRO COMBINER_SET_LONG_CODE_MASK_CHAN7_IMMED
MACRO COMBINER_SET_LONG_CODE_MASK_CHAN8_IMMED
MACRO COMBINER_SET_LONG_CODE_MASK_CHAN7_SPN
MACRO COMBINER_SET_LONG_CODE_MASK_CHAN8_SPN

DESCRIPTION     These macro set the long code mask for a specific channel.
                The IMMED version sets the long code immediately and
                doesn't wait for a roll.

DEPENDENCIES    These macros must be called from inside the context of a
                *_LOCK / *_UNLOCK pair to ensure proper operation.

RETURN VALUE    None.

SIDE EFFECTS    None.

===========================================================================*/
#define COMBINER_SET_LONG_CODE_MASK_CHAN_ROLL( lc_mask, chan )   \
    COMBINER_SET_LONG_CODE_MASK_SPN( chan, lc_mask )

#define COMBINER_SET_LONG_CODE_MASK_CHAN7_IMMED( lc_mask )         \
       MSM_DRV_W( DEM1X_CH7_LC_MASK_LO_IMMED, D_PTR(lc_mask)[0] ); \
       MSM_DRV_W( DEM1X_CH7_LC_MASK_HI_IMMED, D_PTR(lc_mask)[1] )

#define COMBINER_SET_LONG_CODE_MASK_CHAN8_IMMED( lc_mask )         \
       MSM_DRV_W( DEM1X_CH8_LC_MASK_LO_IMMED, D_PTR(lc_mask)[0] ); \
       MSM_DRV_W( DEM1X_CH8_LC_MASK_HI_IMMED, D_PTR(lc_mask)[1] )

#define COMBINER_SET_LONG_CODE_MASK_CHAN7_SPN( lc_mask )              \
       MSM_DRV_W( DEM1X_CH7_LC_MASK_LO_SPN_ROLL, D_PTR(lc_mask)[0] ); \
       MSM_DRV_W( DEM1X_CH7_LC_MASK_HI_SPN_ROLL, D_PTR(lc_mask)[1] )

#define COMBINER_SET_LONG_CODE_MASK_CHAN8_SPN( lc_mask )              \
       MSM_DRV_W( DEM1X_CH8_LC_MASK_LO_SPN_ROLL, D_PTR(lc_mask)[0] ); \
       MSM_DRV_W( DEM1X_CH8_LC_MASK_HI_SPN_ROLL, D_PTR(lc_mask)[1] )

/*===========================================================================

MACRO COMBINER_SET_LONG_CODE_MASK
MACRO COMBINER_ZERO_LONG_CODE_MASK
MACRO COMBINER_SET_LONG_CODE_MASK_SPN
MACRO COMBINER_ZERO_LONG_CODE_MASK_SPN

DESCRIPTION     These macros set or zero the long code mask.

DEPENDENCIES    These macros must be called from inside the context of a
                *_LOCK / *_UNLOCK pair to ensure proper operation.

                The lc_mask argument must be l-value.

RETURN VALUE    None.

SIDE EFFECTS    None.

===========================================================================*/

#define COMBINER_SET_LONG_CODE_MASK( channel, lc_mask ) \
    if( (channel == 0) || (channel == 1) || (channel == 2) )                        \
    {                                                                               \
      MSM_DRV_W_i( DEM1X_CHn_LC_MASK_LO_IMMED, D_PTR(lc_mask)[0], (uint32)channel );\
      MSM_DRV_W_i( DEM1X_CHn_LC_MASK_HI_IMMED, D_PTR(lc_mask)[1], (uint32)channel );\
    }                                                                               \
    else if( channel == 7 )                                                         \
    {                                                                               \
      COMBINER_SET_LONG_CODE_MASK_CHAN7_IMMED( lc_mask );                           \
    }                                                                               \
    else if( channel == 8 )                                                         \
    {                                                                               \
      COMBINER_SET_LONG_CODE_MASK_CHAN8_IMMED( lc_mask );                           \
    }

#define COMBINER_ZERO_LONG_CODE_MASK( ) \
    MSM_DRV_W_i( DEM1X_CHn_LC_MASK_LO_IMMED, 0, 0 ); \
    MSM_DRV_W_i( DEM1X_CHn_LC_MASK_HI_IMMED, 0, 0 ); \
    MSM_DRV_W_i( DEM1X_CHn_LC_MASK_LO_IMMED, 0, 1 ); \
    MSM_DRV_W_i( DEM1X_CHn_LC_MASK_HI_IMMED, 0, 1 ); \
    MSM_DRV_W_i( DEM1X_CHn_LC_MASK_LO_IMMED, 0, 2 ); \
    MSM_DRV_W_i( DEM1X_CHn_LC_MASK_HI_IMMED, 0, 2 ); \
    MSM_DRV_W( DEM1X_CH7_LC_MASK_LO_IMMED, 0 );      \
    MSM_DRV_W( DEM1X_CH7_LC_MASK_HI_IMMED, 0 );      \
    MSM_DRV_W( DEM1X_CH8_LC_MASK_LO_IMMED, 0 );      \
    MSM_DRV_W( DEM1X_CH8_LC_MASK_HI_IMMED, 0 )

#define COMBINER_SET_LONG_CODE_MASK_SPN( channel, lc_mask ) \
    if( (channel == 0) || (channel == 1) || (channel == 2) )                        \
    {                                                                               \
      MSM_DRV_W_i( DEM1X_CHn_LC_MASK_LO_SPN_ROLL, D_PTR(lc_mask)[0], channel );     \
      MSM_DRV_W_i( DEM1X_CHn_LC_MASK_HI_SPN_ROLL, D_PTR(lc_mask)[1], channel );     \
    }                                                                               \
    else if( channel == 7 )                                                         \
    {                                                                               \
      COMBINER_SET_LONG_CODE_MASK_CHAN7_SPN( lc_mask );                             \
    }                                                                               \
    else if( channel == 8 )                                                         \
    {                                                                               \
      COMBINER_SET_LONG_CODE_MASK_CHAN8_SPN( lc_mask );                             \
    }

#define COMBINER_ZERO_LONG_CODE_MASK_SPN( ) \
    MSM_DRV_W_i( DEM1X_CHn_LC_MASK_LO_SPN_ROLL, 0, 0 ); \
    MSM_DRV_W_i( DEM1X_CHn_LC_MASK_HI_SPN_ROLL, 0, 0 ); \
    MSM_DRV_W_i( DEM1X_CHn_LC_MASK_LO_SPN_ROLL, 0, 1 ); \
    MSM_DRV_W_i( DEM1X_CHn_LC_MASK_HI_SPN_ROLL, 0, 1 ); \
    MSM_DRV_W_i( DEM1X_CHn_LC_MASK_LO_SPN_ROLL, 0, 2 ); \
    MSM_DRV_W_i( DEM1X_CHn_LC_MASK_HI_SPN_ROLL, 0, 2 ); \
    MSM_DRV_W( DEM1X_CH7_LC_MASK_LO_SPN_ROLL, 0 );      \
    MSM_DRV_W( DEM1X_CH7_LC_MASK_HI_SPN_ROLL, 0 );      \
    MSM_DRV_W( DEM1X_CH8_LC_MASK_LO_SPN_ROLL, 0 );      \
    MSM_DRV_W( DEM1X_CH8_LC_MASK_HI_SPN_ROLL, 0 )



/*===========================================================================

           DECODER

===========================================================================*/

/*===========================================================================

MACRO DECODER_INITIAL_STATE_4_ZEROS

DESCRIPTION     These macros set the Decoder's State Metrics to values
                corresponding to the preceeding frame ending with 4 zeros.

DEPENDENCIES    Continuous mode only.  DECODER_INITIAL_STATE_NORMAL() must
                be used after the desired frame has been decoded.

RETURN VALUE    None.

SIDE EFFECTS    Decoders state metrics are zero'd or saturated at all
                future decoder interrupts until normal operation is
                restored.

===========================================================================*/
#define DECODER_INITIAL_STATE_4_ZEROS( )                                      \
          SET_SHADOW_DEMBACK_REG_F( SVD_MODE, INITMODE_2_0, 0x04 )

/*===========================================================================

MACRO DECODER_INITIAL_STATE_NORMAL

DESCRIPTION     This macro terminates any continuous mode decoder
                initialization started by:
                    DECODER_INITIAL_STATE_3_ZEROS( )
                    DECODER_INITIAL_STATE_4_ZEROS( )
                    DECODER_INITIAL_STATE_8_ZEROS( )

DEPENDENCIES    Continuous mode only.

RETURN VALUE    None.

SIDE EFFECTS    None.

===========================================================================*/
#define DECODER_INITIAL_STATE_NORMAL( )                                       \
          SET_SHADOW_DEMBACK_REG_F( SVD_MODE, INITMODE_2_0, 0x00 )

/*===========================================================================

MACRO SET_DINT_CH_CFG

DESCRIPTION     This macro does a masked write to DINT_CH_CFG register

DEPENDENCIES    None.

RETURN VALUE    None.

SIDE EFFECTS    None.

===========================================================================*/
#define SET_DINT_CH_CFG( ch, mask, val )                                     \
          SET_SHADOW_DEMBACK_REG_IM( DINT_CHw_CFG, ch, mask, val )


/*===========================================================================

           CDMA AGC, FREQUENCY TRACKING AND BASEBAND OFFSET CONTROL

===========================================================================*/




/*===========================================================================

MACRO SEARCH_FAST_AGC_LOOP
MACRO SEARCH_NORMAL_AGC_LOOP

DESCRIPTION     These macros change the gain constant of the AGC loop, which
                changes the time it will take to settle and the stability of
                the loop.

DEPENDENCIES    Must include "cagc.h".

RETURN VALUE    None.

SIDE EFFECTS    None.

===========================================================================*/

#ifndef FEATURE_MODEM_1X_SUPPORTS_RF

/* Need to update the new RXF registers */
#define SEARCH_FAST_AGC_LOOP( rx_chain )

#define SEARCH_NORMAL_AGC_LOOP( rx_chain )

#else

#define SEARCH_FAST_AGC_LOOP( rx_chain ) \
    MSM_DRV_W_MASKED_i( CDMA_RX_AGCc_GAIN_CTL, GAIN_CONSTANT, \
                    CAGC_RF_TC_0147_US, rx_chain )

#define SEARCH_NORMAL_AGC_LOOP( rx_chain ) \
    MSM_DRV_W_MASKED_i( CDMA_RX_AGCc_GAIN_CTL, GAIN_CONSTANT, \
                    CAGC_RF_TC_0392_US, rx_chain )

#endif /* FEATURE_MODEM_1X_SUPPORTS_RF */

/*===========================================================================

MACRO CAGC_WRITE_8P1
MACRO CAGC_READ_8P1

DESCRIPTION     Helper macro for 8+1 register writes.

DEPENDENCIES    This function must be called from inside the context of a
                *_LOCK / *_UNLOCK pair to ensure proper operation.

RETURN VALUE    None.

SIDE EFFECTS    CAGC_LOW_HIGH/ control is left in HIGH/ state.

===========================================================================*/

#define CAGC_SELECT_LOW( ) \
    MSM_WH_SET( AGC_RDWR_CTL, AGC_LOW_HIGH_N)

#define CAGC_SELECT_HIGH( ) \
    MSM_WH_CLEAR( AGC_RDWR_CTL, AGC_LOW_HIGH_N)

#define CAGC_WRITE_8P1( address, value) \
    CAGC_SELECT_LOW( ), \
    MSM_DRV_W( address, (byte) ((value) << 7)), \
    CAGC_SELECT_HIGH( ), \
    MSM_DRV_W( address, (byte) ((value) >> 1))

#define CAGC_READ_8P1( address, var) \
    CAGC_SELECT_LOW( ), \
    B_PTR(var)[ 0 ] = MSM_RB( address ), \
    CAGC_SELECT_HIGH( ), \
    B_PTR(var)[ 1 ] = MSM_RB( address ), \
    var >>= 7


/*===========================================================================

MACRO CAGC_WRITE_8P2
MACRO CAGC_READ_8P2

DESCRIPTION     Helper macro for 8+2 register writes.

DEPENDENCIES    This function must be called from inside the context of a
                *_LOCK / *_UNLOCK pair to ensure proper operation.

RETURN VALUE    None.

SIDE EFFECTS    CAGC_LOW_HIGH/ control is left in HIGH/ state.

===========================================================================*/

#define CAGC_WRITE_8P2( address, value) \
    CAGC_SELECT_LOW( ), \
    MSM_DRV_W( address, (byte) ((value) << 6) ), \
    CAGC_SELECT_HIGH( ), \
    MSM_DRV_W( address, (byte) ((value) >> 2) )

#define CAGC_READ_8P2( address, var) \
    CAGC_SELECT_LOW( ), \
    B_PTR(var)[ 0 ] = MSM_RB( address ), \
    CAGC_SELECT_HIGH( ), \
    B_PTR(var)[ 1 ] = MSM_RB( address ), \
    var >>= 6

/*===========================================================================

MACRO CAGC_VALUE_WRITE
MACRO CAGC_VALUE_READ

DESCRIPTION     This macro writes to or retrieves from the contents of the
                Adjusted Gain Control

DEPENDENCIES    This function must be called from inside the context of a
                *_LOCK / *_UNLOCK pair to ensure proper operation.

RETURN VALUE    None.

SIDE EFFECTS

===========================================================================*/
#define CAGC_VALUE_WRITE(value) \
    CAGC_WRITE_8P2( AGC_VALUE_WR, value )

#define CAGC_VALUE_READ(var) \
    CAGC_READ_8P2( AGC_VALUE_RD, var )

/*===========================================================================

MACRO MSM_PA_TX_IS_ON
MACRO MSM_PA_TX_ON
MACRO MSM_PA_TX_OFF

DESCRIPTION     These macros do 3 things:

                1)  Returns whether or not the TX_ON function is enabled.
                2)  Enables TX_ON function.
                3)  Disables TX_ON function.

DEPENDENCIES    This macro must be called from inside the context of a
                *_LOCK / *_UNLOCK pair to ensure proper operation.

RETURN VALUE    MSM_PA_TX_IS_ON returns whether or not TX_ON function is
                enabled.

SIDE EFFECTS    None.

===========================================================================*/

#define MSM_PA_TX_IS_ON() \
    ( (HWIO_PA_ON_CTL_shadow & HWIO_FMSK(PA_ON_CTL, TX_ON_EN)) >> \
       HWIO_SHFT(PA_ON_CTL,TX_ON_EN) )

#define MSM_PA_TX_ON() \
    MSM_DRV_SET( PA_ON_CTL, TX_ON_EN )
#define MSM_PA_TX_OFF() \
    MSM_DRV_CLEAR( PA_ON_CTL, TX_ON_EN )



/*===========================================================================

                        CDMA SLEEP CONTROL

===========================================================================*/

/*===========================================================================

MACRO SLEEP_CMD

DESCRIPTION     This macro triggers the 1X sleep controller to sleep
                immediately.

DEPENDENCIES    This macro must be called from inside the context of a
                *_LOCK / *_UNLOCK pair to ensure proper operation.

                SLEEP/WAKEUP timers should be set before triggering sleep.

RETURN VALUE    None.

SIDE EFFECTS    None.

===========================================================================*/

/* Use "1" below to indicate 1x sleep controller */
#define SLEEP_CMD() \
    MSM_DRV_SET_i( SLEEPn_MICRO_START, START, 1 );


/*===========================================================================

                        Interrupt Controller

===========================================================================*/

/*===========================================================================

MACRO INTERRUPT_CLEAR

DESCRIPTION     These macros reset the interrupt status register for the
                given interrupt line.

DEPENDENCIES    None.

RETURN VALUE    None.

SIDE EFFECTS    None.

===========================================================================*/

#define INTERRUPT_CLEAR( interrupt_0_mask, interrupt_1_mask ) \
    MSM_WW( INT_CLEAR_0, (interrupt_0_mask) ); \
    MSM_WW( INT_CLEAR_1, (interrupt_1_mask) )


/*===========================================================================

MACRO INTERRUPT_STATUS

DESCRIPTION     These macros check the interrupt status for the
                given interrupt line.

DEPENDENCIES    None.

RETURN VALUE    0 if the specified interrupt in not pending
                Non-zero if the interrupt is pending.

SIDE EFFECTS    None.

===========================================================================*/
#ifdef HWIO_RTC_INT_STATUS_ADDR
#define INTERRUPT_STATUS_26MS( )   ( HWIO_IN(RTC_INT_STATUS) & \
                                     HWIO_FMSK(RTC_INT_STATUS,PN_ROLLOVER_1X) )
#else /* !HWIO_RTC_INT_STATUS_ADDR */
#define INTERRUPT_STATUS_26MS( ) 0
#endif /* HWIO_RTC_INT_STATUS_ADDR */



/*===========================================================================

                        TCXO PDM#1 and TCXO PDM#2

===========================================================================*/

/*===========================================================================

MACRO PDM1_SET_NORMAL_POLARITY
MACRO PDM1_SET_INVERTED_POLARITY


DESCRIPTION     These macros alter the polarity of PDM #1

DEPENDENCIES    These macros must be called within a *_LOCK / *_UNLOCK
                pair to ensure proper operation.

RETURN VALUE    None.

SIDE EFFECTS    None.

===========================================================================*/

#define PDM1_SET_NORMAL_POLARITY( ) \
    MSM_DRV_CLEAR( TCXO_PDM_CTL, PDM1_POLARITY)

#define PDM1_SET_INVERTED_POLARITY( ) \
    MSM_DRV_SET( TCXO_PDM_CTL, PDM1_POLARITY)


/*===========================================================================

MACRO PDM2_SET_NORMAL_POLARITY
MACRO PDM2_SET_INVERTED_POLARITY


DESCRIPTION     These macros alter the polarity of PDM #2

DEPENDENCIES    These macros must be called within a *_LOCK / *_UNLOCK
                pair to ensure proper operation.

RETURN VALUE    None.

SIDE EFFECTS    None.

===========================================================================*/

#define PDM2_SET_NORMAL_POLARITY( ) \
    MSM_DRV_CLEAR( TCXO_PDM_CTL, PDM2_POLARITY)

#define PDM2_SET_INVERTED_POLARITY( ) \
    MSM_DRV_SET( TCXO_PDM_CTL, PDM2_POLARITY)


/*===========================================================================

MACRO PDM1_ENABLE
MACRO PDM1_DISABLE


DESCRIPTION     These macros enable and disable PDM #1

DEPENDENCIES    These macros must be called within a *_LOCK / *_UNLOCK
                pair to ensure proper operation.

RETURN VALUE    None.

SIDE EFFECTS    None.

===========================================================================*/

#define PDM1_ENABLE( ) \
    MSM_DRV_SET( TCXO_PDM_CTL, PDM1_EN)

#define PDM1_DISABLE( ) \
    MSM_DRV_CLEAR( TCXO_PDM_CTL, PDM1_EN)


/*===========================================================================

MACRO PDM2_ENABLE
MACRO PDM2_DISABLE


DESCRIPTION     These macros enable and disable PDM #2

DEPENDENCIES    These macros must be called within a *_LOCK / *_UNLOCK
                pair to ensure proper operation.

RETURN VALUE    None.

SIDE EFFECTS    None.

===========================================================================*/

#define PDM2_ENABLE( ) \
    MSM_DRV_SET( TCXO_PDM_CTL, PDM2_EN)

#define PDM2_DISABLE( ) \
    MSM_DRV_CLEAR( TCXO_PDM_CTL, PDM2_EN)


/*===========================================================================

MACRO PDM1_SET_PULSE_DENSITY

DESCRIPTION     This macro sets the density of pulses from PDM #1.  Assuming
                normal polarity:
                    0x80 - Low for 255/256 TCXO/4 pulses
                    0x00 - Low for 127/256 TCXO/4 pulses
                    0x7f - Low for   0/256 TCXO/4 pulses

DEPENDENCIES    None.

RETURN VALUE    None.

SIDE EFFECTS    None.

===========================================================================*/

#define PDM1_SET_PULSE_DENSITY( value ) \
    MSM_DRV_W( PDM1_CTL, (value) )


/*===========================================================================

MACRO PDM2_SET_PULSE_DENSITY

DESCRIPTION     This macro sets the density of pulses from PDM #2.  Assuming
                normal polarity:
                    0x80 - Low for 255/256 TCXO/4 pulses
                    0x00 - Low for 127/256 TCXO/4 pulses
                    0x7f - Low for   0/256 TCXO/4 pulses

DEPENDENCIES    None.

RETURN VALUE    None.

SIDE EFFECTS    None.

===========================================================================*/

#define PDM2_SET_PULSE_DENSITY( value ) \
    MSM_DRV_W( PDM2_CTL, (value) )


/*===========================================================================

                        MSM Clocks Control

===========================================================================*/


/*===========================================================================

                        Version Register

===========================================================================*/




/*===========================================================================

MACRO VERSION_GET

DESCRIPTION     This macro the MSM version number

DEPENDENCIES    None.

RETURN VALUE    The MSM version number.

SIDE EFFECTS    None.

===========================================================================*/

#define VERSION_GET( ) (0)


/*===========================================================================

MACRO VERSION_GET_PROCESS_ID

DESCRIPTION     This macro returns the MSM process version number

DEPENDENCIES    None.

RETURN VALUE    The MSM process version number.

SIDE EFFECTS    None.

===========================================================================*/

#define VERSION_GET_PROCESS_ID( ) \
     MSM_DRV_R_MASKED( HW_REVISION_NUMBER, QUALCOMM_MFG_ID )


/*===========================================================================

MACRO VERSION_GET_MSM_ID

DESCRIPTION     This macro returns the MSM id number

DEPENDENCIES    None.

RETURN VALUE    The MSM id number.

SIDE EFFECTS    None.

===========================================================================*/

#define VERSION_GET_MSM_ID( ) \
    MSM_DRV_R_MASKED( HW_REVISION_NUMBER, PARTNUM )


/*===========================================================================

MACRO VERSION_GET_VERSION_ID

DESCRIPTION     This macro returns the MSM version id number

DEPENDENCIES    None.

RETURN VALUE    The MSM version id number.

SIDE EFFECTS    None.

===========================================================================*/

#define VERSION_GET_VERSION_ID( ) \
    MSM_DRV_R_MASKED( HW_REVISION_NUMBER, VERSION_ID )


/*===========================================================================

                              Quick Paging Channel

===========================================================================*/


/*===========================================================================

MACRO QPCH_ARM_OOK_DECODE
MACRO QPCH_DISARM_OOK_DECODE

DESCRIPTION     These macros arm the QPCH OOK hardware to decode a bit at a
                given position, or disarm the OOK hardware.

                Position (9 bits) is specified in PN words (64 chips), for
                a range of upto 1 PN roll, and indicates the last PN word of
                the OOK bit.  Position is relative to the symbol combiner
                26.67ms roll position.

DEPENDENCIES    I/Q Channel configuration, symbol size, etc must be setup
                first for the QPCH channel.

                SYMB_COMB_CTL1:DESKEW_OFFSET needs correct offset.

RETURN VALUE    None.

SIDE EFFECTS    An "RX_QP_INT" interrupt will be generated within 26.7ms.
                QP_I_RD, QP_Q_RD, QP_I_TH1, and QP_I_TH2 will be modified.

===========================================================================*/

#define QPCH_ARM_OOK_DECODE( pos ) \
    MSM_DRV_W_MASKED( DEM1X_CHANNEL0_IMMED, OOK_POSITION, (pos) ); \
    MSM_DRV_SET( DEM1X_CHANNEL0_IMMED, QPCH_EN )

#define QPCH_DISARM_OOK_DECODE( ) \
    MSM_DRV_CLEAR( DEM1X_CHANNEL0_IMMED, QPCH_EN )

/*===========================================================================

                              QLIC Interface

===========================================================================*/

/*===========================================================================

MACRO QLIC_SET_RTF_CTL_SEL
      QLIC_CLR_RTF_CTL_SEL
      QLIC_GET_RTF_CTL_SEL

DESCRIPTION     These macros sets/gets the RTF control select register

DEPENDENCIES    None.

RETURN VALUE    The "get" macro returns the RTF control select register value.

SIDE EFFECTS    None.

===========================================================================*/
#define QLIC_SET_RTF_CTL_SEL() \
    MSM_DRV_SET( DEM1X_RTF_CTL_SEL, CTL_SEL )

#define QLIC_CLR_RTF_CTL_SEL() \
    MSM_DRV_CLEAR( DEM1X_RTF_CTL_SEL, CTL_SEL )

#define QLIC_GET_RTF_CTL_SEL() \
    MSM_DRV_R( DEM1X_RTF_CTL_SEL )

#define QLIC_SET_LPN_ADV( syms ) \
    MSM_DRV_W_MASKED( DEM1X_LPN_ADV, SYM_ADV, (syms) )

#define SET_RTF_ADJUST( lat )                           \
    MSM_DRV_W_MASKED( DEM1X_RTC_ADJUST, SYM_DLY, (lat) )


/*===========================================================================

                              END OF INTERFACE

===========================================================================*/

#endif /* MSM_DRV_I_H */
