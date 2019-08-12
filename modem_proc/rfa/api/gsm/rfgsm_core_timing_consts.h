#ifndef RFGSM_CORE_TIMING_CONSTS_H
#define RFGSM_CORE_TIMING_COSNTS_H
/*!
   @file
   rfgsm_core_timing_consts.h

   @brief
   contains the gsm timing related consts.

   @details

*/
/*===========================================================================
Copyright (c) 2009 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
Copyright (c) 2008 by Qualcomm Technologies, Incorporated.  All Rights Reserved.

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfa/api/gsm/rfgsm_core_timing_consts.h#1 $
$DateTime: 2016/12/13 07:59:52 $
$Author: mplcsds1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
10/25/11   sb      Removed conditional definition of GTOW features 
05/19/11   rsr/sr  Change KV cal sequence and pa ramp length 
05/17/11   rsr/sr  Change KV cal sequence length 
09/23/10   lcl     Pull in R0/R1 timing change form QSC6295
08/25/10   rsr     Pull in fix from QSC6695 - jps Fix for ORFS-SW failure during multislot          
03/19/10   lcl     Pull in changes from QSC6295
02/08/10   sr      Lint fixes 
02/04/10   sr      Lint & kw fixes 
12/04/09   sr     removed rx timing related unused consts/macros
07/04/09   vrb    Changing Start Delta for Rx Burst setup to -360 QS
07/04/09   sr     added the RFGSM_CORE_QTR_SYM_IN_MICRO_SEC macro
03/13/09   sr     code cleanup for SCMM build
11/11/08   sr     Initial version.

============================================================================*/
/*===========================================================================

                                  MACROS

===========================================================================*/
/*  GRFC DEFINITIONS INTERNAL TO THIS TARGET - see in rfgsmlib_rtr8600.h   */
/* ----------------------------------------------------------------------- */

/* LCU Tx SBI = 150uS PLL settling time+ 28us for ramp =178us
   LCU Tx SBI = 150us from PA ramp start                    */
#define RTR8600_TOTAL_TX_SETUP_TIME   (230) 
/* RF_ON should turn on approximately 67 us after the SBI */
//#define RTR8600_RF_ON_TX_ON_LAG_RX    (95) //optimal: 95
#define RTR8600_RF_ON_TX_ON_LAG_RX            (RF_SBI_TRANS_MIN_DELAY_QS + 72)

/* For LCU, DPLL will be settling when we have modulator putting out a tone/leading bits.
 * In earlier chips one of the requirements was that the PLL be settled before we start 
 * putting out leading bits. This is not the case now. However, we do want the PLL to settle 
 * in time for the ramp up. We assume default start of ramp coincides with Modulator leading bits. 
 * Modulator extra leading bits could be to turn on the tone while DPLL settles.
 */
/* LCU Tx SBI = 150uS PLL settling time+ 28us for ramp =178us */
#define RTR8600_GSM_TX_BURST_SETUP_START_DELTA     (-((RTR8600_TOTAL_TX_SETUP_TIME - RF_MODLTR_EXTRA_LEADING_QS) - RF_MODLTR_DELAY_QTR_SYMBS) )

#define RTR8600_TX_SBI_START_DELTA  (RTR8600_GSM_TX_BURST_SETUP_START_DELTA)

/* added it to align with PA_CTL stop time                                   */
#define RTR8600_ADJ_TX_ON_OFFSET             5  

//PLL settling time : 100uS
#define RTR8600_TX_PLL_SETTLING_TIME 108

/* Assuming it takes about 3 QS per SBI write. TX_ON to start immediately
   after the mode_ctl reg write (0xA6)  */
#define RTR8600_RF_ON_TX_ON_START_DELTA_TX   (RTR8600_TX_SBI_START_DELTA + 45 )
                                              
//These nos have been changed for LCU - stop
#define RTR8600_RF_ON_TX_ON_STOP_DELTA_TX    (RF_END_OF_BURST_STOP_TIME + RTR8600_ADJ_TX_ON_OFFSET)


/* Overhead in the mdsp for processing sbi commands */
#define RF_MDSP_OVERHEAD  67

/* RF tune delays: These are used by  modem/geran/gdrivers/src/gl1_hw_cm.c */
#define RFGSM_MON_TUNE_DELAY_QS  250 //340  // 321?
#define RFGSM_RX_TUNE_DELAY_QS 250 // 491

/* these are NOT  used by RF Drivers. These are used by
   "..\..\modem\geran\gdrivers\src\gl1_hw_sched.c. These consts are not valid
   anymore */
#define RF_MDSP_RX_ALPHA_QS (RFGSM_RX_TUNE_DELAY_QS + RF_MDSP_OVERHEAD)
#define RF_MDSP_MON_ALPHA_QS (RFGSM_MON_TUNE_DELAY_QS +  RF_MDSP_OVERHEAD)

/*----------------------------------------------------------------------------*/
/*  RX TIMINGS                                                                */
/*----------------------------------------------------------------------------*/
/*!
  @detail
  The frame tick is slammed at the output of the filter.  So, to really line up 
  with the BS, we need to account for all the Rx chain delay (from the antenna 
  to the output of the Channel Filter. 
*/
#define RF_RX_CHAIN_TOTAL_DELAY   54 //39


/*----------------------------------------------------------------------------*/
/*  RX TIMINGS                                                                */
/*----------------------------------------------------------------------------*/
/*! 
  @details 
  The Modulator delay is from the time we start the Modulator to the time when 
  the first bit is completely out of the Modulator. 
*/
#define RF_MODLTR_DELAY_QTR_SYMBS  (17)

#define RF_MODLTR_LEADING_SYMBS   9
#define RF_MODLTR_LEADING_QS   (RF_MODLTR_LEADING_SYMBS * 4)

#define RF_MODLTR_EXTRA_LEADING_SYMBS  (51) 
#define RF_MODLTR_EXTRA_LEADING_QS  (RF_MODLTR_EXTRA_LEADING_SYMBS * 4)

#define RF_MODLTR_TOTAL_LEADING_SYMBS (RF_MODLTR_LEADING_SYMBS + RF_MODLTR_EXTRA_LEADING_SYMBS)

#define RF_MODLTR_TOTAL_LEADING_QS  (RF_MODLTR_LEADING_QS + RF_MODLTR_EXTRA_LEADING_QS)

#define RF_MODLTR_TRAILING_SYMBS  9
#define RF_MODLTR_TRAILING_QS  (RF_MODLTR_TRAILING_SYMBS * 4)

/* in symbols */
#define  MODLTR_CFG_TRAILING_GP_LEN  (RF_MODLTR_TRAILING_SYMBS << 8)

/* 0 for GSM/GPRS, 1 for EDGE*/
#define EMOD_MODLTR_CFG_USE_JOINTMOD  (0x01 << 2)
/* 0 for GSM/GPRS, 1 for EDGE*/ 
#define EMOD_MODLTR_CFG_USE_GMSKMOD  (0x00 << 2)
#define EMOD_MODLTR_CFG_USE_EER   (0x01 << 1)
#define EMOD_MODLTR_CFG_SPEC_INV  (0x01 << 0)

/* DC CALC TX DC offset. Documented in hw/sw document. 
*/
#define RFGSM_QDSP_I_VAL  256
#define RFGSM_QDSP_Q_VAL  (-256)

#define RFGSMLIB_TX_1ST_LEADING_BIT_START_DELTA RF_MODLTR_DELAY_QTR_SYMBS

#define RFGSMLIB_TX_1ST_USEFUL_BIT_START_DELTA  (RF_MODLTR_DELAY_QTR_SYMBS + RF_MODLTR_TOTAL_LEADING_QS)

#define RFGSMLIB_TX_END_OF_BURST_STOP_DELTA  (RF_MODLTR_DELAY_QTR_SYMBS + RF_MODLTR_TOTAL_LEADING_QS)


/*-----------------------------------------------------------------------------
  Constants defining GSM to WCDMA inter-RAT RF setup and shutdown times
  in GSM Quarter Symbols (QS) (3.69us/4).
-----------------------------------------------------------------------------*/
/* Offset to allow RF tune time adjustments.  Used to adjust the desired
   accuracy of Rx PLL convergence before AGC and DC offset cancellation
   begins.  Longer times allow the Rx PLL to converge more closely to
   its final value.  A value of zero gets it within +/- 1MHz of its
   final value. */
#define RF_TUNE_TIME_ADJ_OFFSET_QS  0

/* Extra overhead required for gtow commands due to large number of SBI
transactions */
#define RF_MDSP_GTOW_OVERHEAD  30

/* WCDMA Inter-RAT Rx RF setup warmup time in QS (DSP time to setup RF +
   RF setup time)  */
#define RF_MDSP_GTOW_START_ALPHA_QS (RF_MDSP_OVERHEAD + RF_MDSP_GTOW_OVERHEAD + 964 + RF_TUNE_TIME_ADJ_OFFSET_QS)

/* WCDMA Inter-RAT Rx RF shutdown time in QS */
#define RF_MDSP_GTOW_STOP_ALPHA_QS  (100)



#define RF_RAMP_WINDOW_QTR_SYMBOLS  16  /* ~16 quarter symbols */

/* RFGSM NON SBI DEFINITIONS */
/* Modltr delay + extra leading bits time - PA ramp window duration */
#define RFLLGSM_PA_START_TIME_ADJ   5  /* This adjustment to satisfy ORFS due to switching */

#define RF_PA_START_TIME            ((RFGSMLIB_TX_1ST_USEFUL_BIT_START_DELTA - \
                                     RF_RAMP_WINDOW_QTR_SYMBOLS)             + \
                                     RFLLGSM_PA_START_TIME_ADJ)

/* Modltr delay + leading bits time */
//#define RF_PA_STOP_TIME  RFGSMLIB_TX_END_OF_BURST_STOP_DELTA  //@vrb: redefined in next line

/* End of burst is last bit out of PA */
#define RFLLGSM_PA_STOP_TIME_ADJ    -3  /* This adjustment to satisfy ORFS due to switching */
#define RF_PA_STOP_TIME             (RFGSMLIB_TX_END_OF_BURST_STOP_DELTA + RFLLGSM_PA_STOP_TIME_ADJ)

/* End of burst is last bit out of PA */
#define RF_END_OF_BURST_STOP_TIME (RF_PA_STOP_TIME + RF_RAMP_WINDOW_QTR_SYMBOLS)

/* PA RAMP up/down timing definitions */
#define RFGSM_TXPWR_PROFILE_HIGHSAT_VAL  0x1fff
#define RF_TX_BURST_OFFSET (RF_MODLTR_DELAY_QTR_SYMBS + RF_MODLTR_TOTAL_LEADING_QS + RF_RX_CHAIN_TOTAL_DELAY)


#endif  /* RFGSM_CORE_TIMING_COSNTS_H */
