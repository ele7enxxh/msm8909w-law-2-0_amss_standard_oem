#ifndef WENCI_H
#define WENCI_H

/*============================================================================
              E N C O D E R   D R I V E R   I N T E R N A L
                          H E A D E R   F I L E

DESCRIPTION
This files contains all necessary macros and definitions for interfacing with
the encoder hardware.

INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright (c) 2000,2012 Qualcomm Technologies, Inc.  All Rights Reserved.
============================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/MSM5200/l1/vcs/enci.h_v   1.11   01 May 2002 09:20:04   yshi  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/wcdma/l1/offline/src/wenci.h#1 $ $DateTime: 2016/12/13 08:00:12 $ $Author: mplcsds1 $

when        who     what, where, why
--------    ---     --------------------------------------------------------
11/10/14    tsk     Mainline code under feature FEATURE_WCDMA_JOLOKIA_MODEM on Jolokia branch.
06/03/14    tsk     Compilation fixes for JOLOKIA 1.0
05/22/14    tsk     Featurization for JOLOKIA 1.0
05/13/14    ymu     Feature name FEATURE_WCDMA_DC_HSUPA_FRAME replacement with FEATURE_WCDMA_DC_HSUPA 
03/31/14    ar      Removed legacy macros which are not applicable to Bolt
02/14/14    ar      Porting Bolt specific uplink changes on to Bolt 2.0
12/20/13    gp      eramb_rd_start_addr should be limited to 10 bits.
05/22/13    oh      Adding DC-HSUPA feature framework support.
05/09/12    ar      BOLT Compilation changes
05/06/13    at      SW Workaround for TXHW-A2 issue and cleanup FEATURE_WCDMA_NIKEL_ERAMA_ACCESS_FIX
04/04/13    at      Cleanup FEATURE_WCDMA_TRITON_MCAL_API featurization
09/11/12    ash     Featurized HW Reg Operations for Triton Compilation
08/10/12    at      Changes for Dime RUMI Tx bring-up
07/24/12    geg     Upmerge WCDMA.MPSS.1.0 to WCDMA.MPSS.2.0
06/23/12    at      Changes for Dime RUMI Tx bring-up
02/08/12    vs      Feature cleanup.
01/31/12    raj     feature cleanup
07/18/11     sp     Added support to use AHB interface for R99 ERAMA WRITE
03/30/11    gnk     Mainlined several features
10/13/09    ka      Code changes to bring up UL DPCCH on 8220.
07/22/09    gnk     Mainlining the feature FEATURE_WCDMA_OFFLINE_DEMOD_SUPPORT
06/01/09    asm     Added a unified Ciphering Algorithm enumeration across Access Stratum
                    including Snow3G ciphering functionality. Also mainlined and cleaned up 
                    now-defunct featurization
10/03/08    hk      Bringing in 7k Mailine fixes
11/01/07    gnk     Mainlining the msm6246 changes
09/13/06    asr     Changed the feature name protecting file inclusion for this file from ENCI_H
                    to WENCI_H.
05/08/06    au      Replaced calls to MSM_IN and MSM_OUT with HWIO_IN and HWIO_OUT.
10/14/05    gs      Ciphering regiter bit width, bit position and other
                    related changes
10/07/07    eav     Changed reference from ADDR to ADDRI
09/14/05    gs      Updates for 6280 register name changes
04/18/05    asm     Merged Raven branch encoder files to mainline
11/04/04    src     Merged in 6275-specific changes.
10/11/04    gs      Added macros for Bc,Bd,Rate adjust update
                    Added macros for writing Tx control and data registers
                    for 6275 interface
01/14/04    gs      Added macro for ciphering mode.
                    Updated macros WRITE_TX_ERAMA_TB_CTRL_WORD to include
                    ciphering_mode.
11/10/03    yus     Replace FEATURE_6250_COMPILE with FEATURE_FAST_ENC_START_TIME.
10/22/03    yus     Fix bugs in WRITE_TX_ERAMA_TB_CIPHERING_CTL_WORD1 and
                    WRITE_TX_ERAMA_TB_CIPHERING_CTL_WORD2 macro when it's used 
                    in Saber.
08/18/03    yus     Added a software workaround for the MSM6250's ERAMA double 
                    writing bug. The software workaround uses the Test memories 
                    instead of ERAMA for modulator writing.
                    This workaround is under flag 
                    FEATURE_MSM6250_ERAMA_HW_BUG_SW_WORKAROUND.
08/07/03    yus     Separate START_ENC_TIME to UL_START_ENC_TIME and
                    UL_PRACH_START_ENC_TIME.
08/07/03    yus     Added #define START_ENC_TIME. It has different valus
                    for MSM6200 and MSM6250 due to hardware changes.
06/10/03    yus     Add HWIO_XXXX style macros support.
04/29/02    sk      Replaced FEATURE_MSM5200C with feature specific names.
03/06/02    sk/sh   Merged MSM5200C related changes.
01/18/02    gs      Added the macro writing for Ciphering Control words.
11/02/01    sk      added TX_ACCUM_FREEZE_EN, TX_ACCUM_FREEZE_DIS
04/17/01    sk      Added memory dump size defines.
09/08/00    sk      file created.
                    
===========================================================================*/

/* <EJECT> */
/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "wcdma_variation.h"
#include "comdef.h"
#include "msm.h"
#include "l1mathutil.h"


/*===========================================================================

                     MACROS

===========================================================================*/
/* Start ENC time */
/* Don't change this value when EUL is present.Changing this value may result 
   in problems when EUL and DCH are present */
#define UL_START_ENC_TIME           0x7800
#define UL_PRACH_START_ENC_TIME     0x80007800

/* for freezing/unfreezing closed loop power control */
#define TX_ACCUM_FREEZE_EN     1
#define TX_ACCUM_FREEZE_DIS    0

/* 
 * Note - TrCH Contrl RAM is implemented using registers. 
 * So, it can be viewed using simple MSM_IN() calls.
 */

/* # active TrCHs, # ERAMa TrCHs */
#define WRITE_TX_ERAM_AB_TRCHS(num_active_trchs, num_erama_trchs) \
  HWIO_OUT (TX_ERAM_AB_TRCHS, ((uint32)num_active_trchs << 4) | (uint32)num_erama_trchs);

#define ENC_SCR_CODE_M           0x81000000L
#define ENC_SCR_CODE_LONG_V      0x81000000L  /* always set bit 24 to '1' */
#define ENC_SCR_CODE_SHORT_V     0x00000000L

#define WRITE_TX_BETA_GAIN(spr_gain_pre, spr_gain_data, spr_gain_ctl)  1
#define WRITE_DPCH_TX_BETA_GAIN(ul_bc, ul_bd) enc_set_bc_bd_pair_val(ul_bc, ul_bd)
#define ENC_SET_RATE_ADJ_IN_DELTA(delta) enc_set_rate_adj_in_cm_delta(delta)

/* ERAMa write data control */
#define UL_CIPHERING_F8_ALG_DIR       0
#define UL_CIPHERING_MODE_NORMAL      0
#define UL_CIPHERING_MODE_CONTINUE    1

#define WRITE_TX_ERAMA_TB_CTRL_WORD(ciphering_en, ciphering_mode, hdr_size, trch_cfg0_idx) \
     HWIO_OUT (TX_UNIFIED_ENC_ERAM_DATA_INCRACC_1, (ciphering_en << 10) | \
        (ciphering_mode << 9) | (hdr_size << 3) | trch_cfg0_idx);
    
#define WRITE_TX_ERAMA_TB_CIPHERING_CTL_WORD1(algo, offset, key_idx, bearer_id, cipher_len) \
        HWIO_OUT (TX_UNIFIED_ENC_ERAM_DATA_INCRACC_2, (((algo) & 0x1) << 28) | (((offset) & 0x1F) << 23) | \
    (((key_idx) & 0x7) << 20) | (((UL_CIPHERING_F8_ALG_DIR) & 0x1) << 19) | \
    (((bearer_id) & 0x1F) << 14) | ((cipher_len) & 0x3FFF))

#define WRITE_TX_ERAMA_TB_CIPHERING_CTL_WORD2(count) HWIO_OUT(TX_UNIFIED_ENC_ERAM_DATA_INCRACC_3, count)
#define WRITE_TX_ERAMA_DATA(erama_data) HWIO_OUT(TX_UNIFIED_ENC_ERAM_DATA_INCRACC_0, erama_data);
#define WRITE_TX_ENC_TIMING_CTL(start_enc_now, start_enc_time) \
     HWIO_OUT(TX_UNIFIED_DCH_RACH_ENC_TRIGGER, ((uint32)start_enc_now) | start_enc_time);

/* Miscellaneous */
#define WRITE_TX_PHASE_ACC_RESET(x)     HWIO_OUT(TX_PHASE_ACC_RESET, x);
#define WRITE_BETA_GAIN_GROUP_DLY(x)    HWIO_OUT(TX_BETA_GAIN_GROUP_DLY, x);

/* Tx Conditioning Block controls */
 #define WRITE_TX_COND_RESET(x)         HWIO_OUT(TX_CONDITIONING_RESET, x);

/*-----------------------------------------------------------------------*/
/* TX General Control bit definitions
 * 07 : PHASE_ACC_RESET - 1 resets tcxo phase accumulator
 * 06 : PHSAE_ACC_BYPASS - 1 bypasses tcxo phase accumulator
 * 05 : CORDIC_BYPASS - 1 bypasses cordic rotator
 * 04 : TX_SPECTRAL_INVERSION - 1 when TX spectral inversion is needed
 * 03 : TX_DATA_FORMAT - selects format for TX_IQ_DATA[7:0] pins
 *      1 - two's complement format
 *      0 - offset binary format (0 -> -127.5, 1-> 127.5)
 * [02:01]: PA_CTL - programs the state of TX_PUNCT
 *      00 - overrides TX_PUNCT and clears it to 0
 *      01 - overrides TX_PUNCT and sets it to 1
 *      1x - normal operation (controled by PA_WARMUP in TX AGC)
 * 00 : TX_OUTPUT_INTERFACE - controls output of Tx chain
 *      1 - enable BB3 interface (bypass IPF, DAC), data at chipx8 rate
 *      0 - enable TX DAC interface, data at chipx16 rate
 */
/*--------------------------------------------------------------------*/

/* TX Test Control definitions */
#define TX_CLK_INVERT_MASK      0x04  /* TX_I_CLK polarity mask */
#define TX_CLK_INVERT_HI        0x04  /* invert TX_I_CLK pin */
#define TX_CLK_INVERT_NORMAL    0x00  /* normal operation */

#define TX_SET_IQ_MASK          0x02  /* FIR input control mask */
#define TX_SEQ_IQ_HI            0x02  /* forces both inputs to FIRs high */
#define TX_IQ_NORMAL            0x00  /* normal operation */

#define TX_TONE_ENA_MASK        0x01  /* internal Tx test tone generator */
#define TX_TONE_ENA             0x01  

/* enable and select Tx internal test tone generator */
#define TX_TONE_ENA_NORMAL      0x00  /* normal operation */

/* TX Filler Polarity definitions */
#define TX_RADIO_FRAME_EQ_FILLER_MASK  0x02

/* filler bit during radio frame equalization */
#define TX_RADIO_FRAME_EQ_FILLER_HI    0x02
#define TX_RADIO_FRAME_EQ_FILLER_LO    0x00
#define TX_CODE_BLK_SEG_FILLER_MASK    0x01

/* filler bit during code block segmentation */
#define TX_CODE_BLK_SEG_FILLER_HI      0x01
#define TX_CODE_BLK_SEG_FILLER_LO      0x00

/* TX Status definitions */
#define TX_PA_STATUS_MASK             0x40
#define TX_PA_ON_V                    0x40
#define TX_PA_OFF_V                   0x00
#define TX_TRCH_NUM_MASK              0x3C
#define TX_ENC_BUSY_MASK              0x02
#define TX_ENC_BUSY_V                 0x02
#define TX_ENC_IDLE_V                 0x00
#define TX_ENC_ERR_MASK               0x01
#define TX_ENC_ERR_V                  0x01
#define TX_ENC_NO_ERR_V               0x00

/* read TX status */
#define READ_TX_STATUS()              HWIO_IN(TX_STATUS_RD)

/* Debug macros */
#define TX_ERAMA_SZ            280 //words
#define TESTMEM_ERAMA          0x14

#define TX_ERAMB_SZ            220  //words
#define TESTMEM_ERAMB          0x15

#define TX_TRCH_CFG0_RAM_SZ    16
#define TESTMEM_TRCH_CFG0_RAM  0x16 

#define TX_TRCH_CFG1_RAM_SZ    56
#define TESTMEM_TRCH_CFG1_RAM  0x17

#define TX_RMRAM_SZ            640
#define TESTMEM_RMRAM          0x18

#endif /* WENCI_H */
