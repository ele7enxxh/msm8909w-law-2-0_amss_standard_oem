/*===========================================================================

                        C A G C   H E A D E R   F I L E

DESCRIPTION
  This module contains all the definitions necessary to interface with the
  CAGC.

Copyright (c) 1997 - 2003 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
Copyright (c) 2007, 2010  by Qualcomm Technologies, Incorporated.  All Rights Reserved.

===========================================================================*/
/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rftech_cdma/1x/rf/inc/cagc.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
01/11/10   sr      Deleted rficap.h
09/02/09   sr      Mainlined RF_HAS_MDSP_TX_AGC
04/13/09   sr      commented-out the TX_AGC_ADJ macro which interfere with tlmn def
08/03/07   bmg     Removed all of the unused CAGC macros, modified the one
                   useful macro do sign extension (CAGC_INMSF) to use the
                   correct HWIO interface and be register width agnostic.
02/27/07   go      Added PWR_CTL_EN value
02/07/07   adk     Deleted duplicate #defines CAGC_RF_RATCHET_[ENA|DIS]_V
                   and CAGC_RF_TX_STEP_[FULL|HALF|QUAR]_DB_V
01/25/07   jfc     Include high level rficap.h instead of rficap_mm_cdma1x.h
11/22/06   vm      Added the support for Firmware TX AGC
10/31/06   ycl     Change to include the correct rficap file.
10/17/06   ycl     Modifications for initial 7600 build.
10/14/05   et      fixed a LINT error
10/20/04   bmg     Fixed incorrect RX_AGCc_LNA_RANGE_DELAY setting for RFR6135
09/28/04   ans     Added CAGC_RF_LNA_RANGE_DELAY_85CHIPX2_RFR6135 for RFR6135.
08/29/04   dyc     Mainline RF_HAS_DECOUPLED_PA_CTL
05/01/04   dyc     Updated CAGC_RF_LNA_RANGE_DELAY_85CHIPX2 value.
03/09/04   dyc     Added TX_PDM_DELAY_VAL_V definition.
10/27/03   dyc     Remove duplicate Tx related def block
08/10/03   dyc     Added CAGC_RF_LNA_RANGE_DELAY_85CHIPX2 value
06/26/03   dyc     updated PA_R_MAP definitions
06/13/03   dyc     added CAGC_RF_PA_R_MAP_WB__00001000
06/11/03   et      added more values
06/06/03   et      cleaned up all the #defines that are not being used
06/05/03   et      added CAGC_RF_ before every #define that we use. 
05/28/03   et      replaced cagc.h with this file which has all RAW VALUES
                   instead of values shifted by the mask
05/13/03   et      initial revision
===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "target.h"
#include "processor.h"
#include "comdef.h"     /* Definitions for byte, word, etc.     */
#include "msm.h"

/*===========================================================================

                        DATA DECLARATIONS

===========================================================================*/


/*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=

                 CDMA AGC (CAGC) I/O PORTS AND DATA

=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*/
/*===========================================================================

MACRO CAGC_INMSF

DESCRIPTION
  CAGC IN, Mask, Sign extend, drop it to the Floor.

  This macro reads a bit field and then shifts the value up to sign extend
  it, then shifts the value down to the LSBs of the variable.

PARAMETERS
  reg    The name of the register to read
  field  The name of the field in reg to extract

DEPENDENCIES
  None

RETURN VALUE
  int32 value of the field sign extended and shifted to LSBs.

SIDE EFFECTS
  None

===========================================================================*/
#define  CAGC_INMSF( reg, field ) \
         ( ( ((int32)(HWIO_INM( reg, HWIO_FMSK(reg, field) ))) \
             << SHIFT_UP_FROM_MASK_W(HWIO_FMSK(reg, field))) \
          >> (SHIFT_UP_FROM_MASK_W(HWIO_FMSK(reg, field)) + SHIFT_DN_FROM_MASK(HWIO_FMSK(reg, field))))



/* ------------------  CAGC Register Definitions ------------------------- */


/* -----------------------------------------------------------------------
** CAGC:  Digital AGC Control Register   
**        AGC_CTL_WB
** ----------------------------------------------------------------------- */
/* TX_AGC_CTL */ 

/* MDSP Units are 1/512th of dB */

#define CAGC_RF_TX_STEP_FULL_DB_V 0x200 /* value for 1.0  dB step size      */
#define CAGC_RF_TX_STEP_HALF_DB_V 0x100 /* value for 0.5  dB step size      */
#define CAGC_RF_TX_STEP_QUAR_DB_V 0x80 /* value for 0.25 dB step size      */

#define CAGC_RF_LIN_RANGE_ENA_V   0x1 /* MSM2.3 only.      102.4 dB range */
#define CAGC_RF_LIN_RANGE_DIS_V   0x0 /* MSM2.2 compatible. 85.3 dB range */

#define CAGC_RF_RATCHET_ENA_V     0x1 /* RATCHET_MODE force               */
#define CAGC_RF_RATCHET_DIS_V     0x0 /* RATCHET_MODE normal              */

/* -----------------------------------------------------------------------
** CAGC:  Polarity and Data Override Control Register
**        AGC_CTL2_WB
**        OUT_CTL 
** ----------------------------------------------------------------------- */
#define CAGC_RF_TX_AGC_POL_NORM_V 0x0 /* Rx AGC normal polarity         */
#define CAGC_RF_TX_AGC_POL_INV_V  0x1 /* Rx AGC inverted polarity       */

#define CAGC_RF_LNA_POL_NORM_V    0x0 /* LNA normal polarity            */
#define CAGC_RF_LNA_POL_INV_V     0x1 /* LNA inverted polarity          */

#define CAGC_RF_PAR_POL_NORM_V    0x0 /* PA range normal polarity       */
#define CAGC_RF_PAR_POL_INV_V     0x1 /* PA range invert polarity       */

#define CAGC_RF_RX_AGC_CTL_INT_V  0x1 /* Rx AGC internal control        */
#define CAGC_RF_RX_AGC_CTL_CPU_V  0x0 /* Rx AGC CPU control             */

#define CAGC_RF_TX_AGC_CTL_INT_V  0x1 /* Tx AGC internal control        */
#define CAGC_RF_TX_AGC_CTL_CPU_V  0x0 /* Tx AGC CPU control             */

#define CAGC_RF_LNA_CTL_INT_V     0x1 /* LNA internal control           */
#define CAGC_RF_LNA_CTL_CPU_V     0x0 /* LNA CPU control                */

#define CAGC_RF_PAR_CTL_NONOVERRIDE_AGC_V     0x1 /* PA range internal control      */
#define CAGC_RF_PAR_CTL_OVERRIDE_AGC_V     0x0 /* PA range CPU control           */

#define CAGC_RF_AGC_VALUE_ACCUM_NORM_V 0x0

#define CAGC_RF_AGC_VALUE_OVERRIDE_EN 0x1

#define CAGC_RF_TX_AGC_ADJ_NON_OVERRIDE_V 1
#define CAGC_RF_TX_AGC_ADJ_OVERRIDE_V 0

#define CAGC_RF_AGC_VALUE_NON_OVERRIDE_V 0
#define CAGC_RF_AGC_VALUE_OVERRIDE_V 1
/* -----------------------------------------------------------------------
** CAGC: CAGC Override bits  
**       AGC_CTL3_WB
** ----------------------------------------------------------------------- */

/* PA_R1 and PA_R0 values.  Note that PA_R1 and PA_R0 are not affected
** by the polarity when these signals are being set by the CPU (software).
*/
#define CAGC_RF_PAR_00_V          0x3 /* PA R1,R0 = 0,0                   */
#define CAGC_RF_PAR_01_V          0x2 /* PA R1,R0 = 0,1                   */
#define CAGC_RF_PAR_10_V          0x1 /* PA R1,R0 = 1,0                   */
#define CAGC_RF_PAR_11_V          0x0 /* PA R1,R0 = 1,1                   */

/* PA_R_MAP_WB reg values. PA_R[1:0] pin mapping to states 00, 01, 10, 11 */
#define CAGC_RF_PA_R_MAP_WB__11100100  0xE4
#define CAGC_RF_PA_R_MAP_WB__01011000  0x58
#define CAGC_RF_PA_R_MAP_WB__10101000  0xA8
#define CAGC_RF_PA_R_MAP_WB__10101010  0xAA

/* -----------------------------------------------------------------------
** CAGC:  CDMA AGC Loop Gains
**        AGC_CTL4_WB
** ----------------------------------------------------------------------- */
#define AGC_CTL4  AGC_CTL4_WB
                                    /* MSM23: loop gain ctl during IM   */
#define CAGC_RF_TCIM_0073_US      0x3 /*   73 us time constant            */
#define CAGC_RF_TCIM_0098_US      0x2 /*   98 us time constant            */
#define CAGC_RF_TCIM_0196_US      0x1 /*  196 us time constant            */
#define CAGC_RF_TCIM_GC_V         0x0 /*  Use GAIN_CONSTANT values        */

#define CAGC_RF_TC_0073_US        0xf /*   73 us time constant            */
#define CAGC_RF_TC_0098_US        0x0 /*   98 us time constant            */
#define CAGC_RF_TC_0147_US        0x1 /*  147 us time constant            */
#define CAGC_RF_TC_0196_US        0x2 /*  196 us time constant            */
#define CAGC_RF_TC_0294_US        0x3 /*  294 us time constant            */
#define CAGC_RF_TC_0392_US        0x4 /*  392 us time constant            */
#define CAGC_RF_TC_0587_US        0x5 /*  587 us time constant            */
#define CAGC_RF_TC_0783_US        0x6 /*  783 us time constant            */
#define CAGC_RF_TC_1175_US        0x7 /* 1175 us time constant            */
#define CAGC_RF_TC_1566_US        0x8 /* 1566 us time constant            */
#define CAGC_RF_TC_2349_US        0x9 /* 2349 us time constant            */

/* -----------------------------------------------------------------------
** CAGC:  Controls structures associated with LN_RANGE and LNA_GAIN pins
**        CTL5_WB 
**        AGC_DATA_5
** ----------------------------------------------------------------------- */

#define AGC_CTL5 AGC_CTL5_WB

#define CAGC_RF_LNA_FILT_FRZ_V    0x1    /* Override LNA filter          */
#define CAGC_RF_LNA_FILT_NRM_V    0x0    /* Normal auto LNA filter ops   */

#define CAGC_RF_LNA_RG_FILT_ENA_V 0x1    /* Input from LPF               */
#define CAGC_RF_LNA_RG_FILT_DIS_V 0x0    /* Input from AGC_VALUE         */

#define CAGC_RF_LNA_RG_OVRD_ENA_V 0x1    /* Override LNA range           */
#define CAGC_RF_LNA_RG_OVRD_DIS_V 0x0    /* Normal LNA range ops         */

/* -----------------------------------------------------------------------
** CAGC:  CDMA AGC Control Register
**        CTL6_WB
**        CTL_6
** ----------------------------------------------------------------------- */
#define AGC_CTL6 AGC_CTL6_WB
#define LIN_SEL_M         0x04    /* Mask: subtract LNA_GAIN output?  */
#define LIN_ENA_V         0x04    /* Subtract output from Rx pwr path */
#define LIN_DIS_V         0x00    /* No subtration of LNA's output    */

#define CAGC_RF_LNA_FILT_DIS_V     0x00    /* Mask: behavior of LPF struct.    */

/* ------------------ new masks for AGC_CTL6_WB for PP ---------------------- */
/* The Units of MDSP are CX1 = 0.8138us */
#define CAGC_RF_TX_BLIP_DELAY_00US_V  0x0    // no blip delay
#define CAGC_RF_TX_BLIP_DELAY_06US_V  0x8    // 6.5 us blip delay
#define CAGC_RF_TX_BLIP_DELAY_13US_V  16    // 13 us blip delay
#define CAGC_RF_TX_BLIP_DELAY_26US_V  32   // 26 us blip delay
/* ---------------------------------------------------------------- */

/* -----------------------------------------------------------------------
** CAGC:  AGC_VALUE_MAX port
**        AGC_VALUE_MAX
**        AGC_VALUE_MAX_WB
** ----------------------------------------------------------------------- */

#define CAGC_RF_AGC_MAX_ENA_V     0x1    /* Limit AGC_VALUE's max output    */
#define CAGC_RF_AGC_MAX_DIS_V     0x0    /* No AGC_VALUE's max limit        */
#define CAGC_RF_AGC_VAL_MAX_V     0xFF

#define AGC_MAX_HIGHEST   (int1)0x7F /* Highest value of AGC max limit */
#define AGC_MAX_LOWEST    (int1)0x00 /* Lowest value of AGC max limit  */

#define CAGC_RF_VREF_DELAY_TIMER_V 0x0
#define CAGC_RF_VREF_V             0x0
#define CAGC_RF_TEST_CTL_V         0x0
#define CAGC_RF_LGLUT_LVAL         0x0
#define CAGC_RF_LGLUT_HVAL         0x0
#define CAGC_RF_SBI_CTL_V          0x1
#define CAGC_RF_LNA_DATA_V         0x23

/* -----------------------------------------------------------------------
** CAGC:  AGC_VALUE_RD port
**        AGC_VALUE
** ----------------------------------------------------------------------- */
#define AGC_VALUE  AGC_VALUE_RD_RH
#define AGC_VALUE_M  HWIO_RMSK(AGC_VALUE_RD)
#define AGC_VALUE_LSBS_TO_TRUNCATE 2


/* -----------------------------------------------------------------------
** CAGC:  AGC_VALUE_MIN port
**        VALUE_MIN
**        AGC_VALUE_MIN_WB
**        AGC_VALUE_1_MIN_WB 
** ----------------------------------------------------------------------- */
#define AGC_VALUE_MIN AGC_VALUE_1_MIN_WB
      /*  The following are common to all AGC_VALUE_MIN registers */
#define CAGC_RF_AGC_MIN_DIS_V     0x00       /* No AGC_VALUE's min limit         */

/* -----------------------------------------------------------------------
** CAGC:  AGC_VALUE_2_MIN, AGC_VALUE_11_MIN port
** ----------------------------------------------------------------------- */
#define AGC_VALUE_11_MIN  AGC_VALUE_2_MIN_WB

/* -----------------------------------------------------------------------
** CAGC:  IM_STATE_CTL controls the two timers which operate the IM state machine
**        IM_STATE_CTL_WB
**        IM_STATE_CTL_WB
** ----------------------------------------------------------------------- */

#define NONBYPASS_TIMER_M 0xE0    /* Mask: duration of nonbypass */
#define NONBYPASS_156US_V 0x60    /* 3 * 52us = 156us            */
#define NONBYPASS_TIMER_POS  5    /* starting bit position of nonbypass timer */

#define BYPASS_TIMER_M    0x1F    /* Mask: duration of bypass    */
#define BYPASS_085MS_V    0x0D    /* 13 * 6.6ms = 85.8 ms        */

#define CDMA_LNA_CTL_FALL_ON_V      0x7f    /* Fall value for LNA on   */
#define CDMA_LNA_CTL_FALL_OFF_V     0x80    /* Fall value for LNA off  */


/* -----------------------------------------------------------------------
** CAGC:  AGC Read/Write Control - Control for RAS RAM  and accumulators
**        AGC_RDWR_CTL_WB
**        RAS_RAM_CNTL
** ----------------------------------------------------------------------- */

#define CAGC_RF_FREEZE_PA_STATE_V   0x1 /* freeze the PA state              */
#define CAGC_RF_UNFREEZE_PA_STATE_V 0x0 /* unfreeze the PA state           */

#define CAGC_RF_AGC_FRZ_V          0x1 /* Freezes AGC accum enable         */
#define CAGC_RF_AGC_NRM_V          0x0 /* Selects Normal AGC accum op      */

#define CAGC_RF_TX_OL_FRZ_V        0x1 /* Freezes TX open-loop accum       */
#define CAGC_RF_TX_OL_NRM_V        0x0 /* TX open-loop accum normal op     */

#define CAGC_RF_RAS_RAM_WRITE_ENA_V 0x1 /* RAS RAM write                    */
#define CAGC_RF_RAS_RAM_WRITE_DIS_V 0x0 /* no RAS RAM write                 */
#define CAGC_RF_RAS_RAM_RESET 0x1

/* -----------------------------------------------------------------------
** TEST_CNTL write port
** ----------------------------------------------------------------------- */

#define CAGC_RF_LI_BYPASS_ENA_V   0x1 /* bypass integrator for RSSI read  */
#define CAGC_RF_LI_BYPASS_DIS_V   0x0 /* normal loop integrator mode      */

/* -----------------------------------------------------------------------
** CAGC:  RATCHET_BIT_DIS port
**        RATCHET_BIT_DIS
** ----------------------------------------------------------------------- */

// RATCHET_BIT_DIS    HWIO_ADDR(RATCHET_BIT_DIS)
#define RATCHET_BIT_DIS_M  HWIO_RMSK(RATCHET_BIT_DIS)

/* -----------------------------------------------------------------------
** CAGC:  TX_AGC_ADJ_RD port
**        TX_AGC_ADJ
** ----------------------------------------------------------------------- */
//#define TX_AGC_ADJ    HWIO_ADDR(TX_AGC_ADJ_RD)
#define TX_AGC_ADJ_M  HWIO_RMSK(TX_AGC_ADJ_RD)

/* -----------------------------------------------------------------------
** CAGC:  CDMA_VGA_GAIN_RD port
**        CDMA_VGA_GAIN
** ----------------------------------------------------------------------- */
#define CDMA_VGA_GAIN    HWIO_ADDR(CDMA_VGA_GAIN_RD)
#define CDMA_VGA_GAIN_M  HWIO_RMSK(CDMA_VGA_GAIN_RD)

/* -----------------------------------------------------------------------
** CAGC:  TX_OPEN_LOOP_RD port
**        TX_OPEN_LOOP
** ----------------------------------------------------------------------- */

#define TX_OPEN_LOOP    TX_OPEN_LOOP_RD
#define TX_OPEN_LOOP_M  HWIO_RMSK(TX_OPEN_LOOP_RD)

/* -----------------------------------------------------------------------
** CAGC:  TX_GAIN_ADJ_RD port
**        TX_GAIN_ADJ
** ----------------------------------------------------------------------- */

#define TX_GAIN_ADJ    TX_GAIN_ADJ_RD
#define TX_GAIN_ADJ_M  HWIO_RMSK(TX_GAIN_ADJ_RD)

/* -----------------------------------------------------------------------
** CAGC:  TX_GAIN_CTL read port  
** ----------------------------------------------------------------------- */

#define TX_GAIN_CTL    TX_GAIN_CTL_RD
#define TX_GAIN_CTL_M  HWIO_RMSK(TX_GAIN_CTL_RD) 

/* -----------------------------------------------------------------------
** CAGC:  MOD_TEST_CTL_WH
** ----------------------------------------------------------------------- */
#define MOD_TEST_PA_DIS_VAL 0x0
#define LNA_OFFSET_MAX_V (1023)
#define LNA_OFFSET_MIN_V (0)

#define CAGC_RF_PA_R_MAP_EN_EN_V     0x1
#define CAGC_RF_PA_R_MAP_EN_DIS_V    0x0

#define CAGC_RF_TEMPORAL_HYST_EN_V   0x1
#define CAGC_RF_TEMPORAL_HYST_DIS_V  0x0


/* -----------------------------------------------------------------------
** CAGC:  AGC_CTL8 contains additional AGC Control bits
** ----------------------------------------------------------------------- */
#define AGC_CTL8 AGC_CTL8_WB          

#define CAGC_RF_MASK_DELAY_2PCG_V    0x1 

#define CAGC_RF_TX_MSM3100_EN_V      0x1  // backward compatible mode enabled
#define CAGC_RF_TX_MSM3100_DIS_V     0x0

#define CAGC_RF_RATE_ADJ_READY_YES_V 0x1
#define CAGC_RF_RATE_ADJ_READY_NO_V  0x0

#define CAGC_RF_PWR_CTL_EN_ON_V      1
#define CAGC_RF_PWR_CTL_EN_OFF_V     0

#define EARLY_2_PCG_CTL_ADV_LSB_POS 2
#define EARLY_2_PCG_CTL_PER_LSB_POS 0
#define EARLY_2_PCG_EN_ADV_64CHIPS         64 << EARLY_2_PCG_CTL_ADV_LSB_POS
#define EARLY_2_PCG_EN_PER_EVERY_PCG       0x3 << EARLY_2_PCG_CTL_PER_LSB_POS

/* -----------------------------------------------------------------------
** CAGC:  TX_VERY_EARLY_FRAME_CTL_WH
** ----------------------------------------------------------------------- */
#define TX_VERY_EARLY_FRAME_ADV_MASK HWIO_FMSK(TX_VERY_EARLY_FRAME_CTL,VERY_EARLY_FRAME_ADV) 
#define TX_VERY_EARLY_FRAME_PER_MASK HWIO_FMSK(TX_VERY_EARLY_FRAME_CTL,VERY_EARLY_FRAME_PER) 

#define CAGC_RF_VERY_EARLY_FRAME_20MS          0
#define CAGC_RF_VERY_EARLY_FRAME_10MS          1
#define CAGC_RF_VERY_EARLY_FRAME_5MS           2
#define CAGC_RF_VERY_EARLY_FRAME_0MS           3

#define CAGC_RF_THW2_36_CHIPS                  36
#define CAGC_RF_100_CHIPS                     100
#define CAGC_RF_784_CHIPS                     784
#define CAGC_RF_1536_CHIPS                    1536

#define CAGC_RF_TX_VERY_EARLY_FRAME_ADV_100CHIPS (CAGC_RF_100_CHIPS << 2)
#define CAGC_RF_TX_VERY_EARLY_FRAME_ADV_784CHIPS CAGC_RF_784_CHIPS << 2

/* -----------------------------------------------------------------------
** CAGC:  TX_ALIGN_DELAY_WB
** ----------------------------------------------------------------------- */
#define CAGC_RF_TX_ALIGN_DELAY_223CX8_V     0xdf         // 223 chipx8 align delay

/* -----------------------------------------------------------------------
** CAGC:  TX_PDM_DELAY_VAL
** ----------------------------------------------------------------------- */
#define TX_PDM_DELAY_VAL_V  0x70

/* ----------------------------------------------------------
** CAGC: SUBPCG_PA_WARMUP_DELAY 
** ---------------------------------------------------------- */
#define MIN_PA_WARMUP_DELAY            8
#define DEFAULT_PA_WARMUP_DELAY      784

/* ----------------------------------------------------------
** CAGC: SUBPCG_PA_WARMUP_DELAY
** ---------------------------------------------------------- */
#define MIN_TX_WARMUP_DELAY            8
#define DEFAULT_TX_WARMUP_DELAY      784


/* ----------------------------------------------------------
** CAGC: PA_ON_CTL_WB 
** ---------------------------------------------------------- */
#define CAGC_RF_PA_ON_CELL_BAND_SEL_V 0x0
#define CAGC_RF_PA_ON_PCS_BAND_SEL_V 1 

#define CAGC_RF_PA_ON_ENA_ON_V  1
#define CAGC_RF_PA_ON_ENA_OFF_V 0x0
#define CAGC_RF_TX_ON_ENA_ON_V  1
#define CAGC_RF_TX_ON_ENA_OFF_V  0x0
#define CAGC_RF_BYPASS_TX_SYNTH_LOCK_VAL 1 
#define CAGC_RF_NON_BYPASS_TX_SYNTH_LOCK_VAL  0x0
#define CAGC_RF_BYPASS_PA_SYNTH_LOCK_VAL  1
#define CAGC_RF_NON_BYPASS_PA_SYNTH_LOCK_VAL  0x0
#define CAGC_RF_BYPASS_TX_PUNCT_VAL  1
#define CAGC_RF_NON_BYPASS_TX_PUNCT_VAL 0x0
#define CAGC_RF_BYPASS_PA_PUNCT_VAL  1
#define CAGC_RF_NON_BYPASS_PA_PUNCT_VAL 0x0

/* -----------------------------------------------------------------------
** CAGC:  AGC_CTL9 
** ----------------------------------------------------------------------- */
#define LNA_RANGE_DATA_LSB_SHIFT 4
#define LNA_RANGE_FORCE_LSB_SHIFT 0
#define LNA_RANGE_DATA_011 0x03 << LNA_RANGE_DATA_LSB_SHIFT
#define LNA_RANGE_DATA_010 0x02 << LNA_RANGE_DATA_LSB_SHIFT
#define LNA_RANGE_DATA_000 0x00 << LNA_RANGE_DATA_LSB_SHIFT
#define LNA_RANGE_DATA_001 0x01 << LNA_RANGE_DATA_LSB_SHIFT
#define LNA_RANGE_DATA_111 0x07 << LNA_RANGE_DATA_LSB_SHIFT

#define LNA_RANGE_DECISION_0000 0x00 << LNA_RANGE_FORCE_LSB_SHIFT
#define LNA_RANGE_DECISION_0001 0x01 << LNA_RANGE_FORCE_LSB_SHIFT
#define LNA_RANGE_DECISION_0011 0x03 << LNA_RANGE_FORCE_LSB_SHIFT
#define LNA_RANGE_DECISION_0111 0x07 << LNA_RANGE_FORCE_LSB_SHIFT
#define LNA_RANGE_DECISION_1111 0x0f << LNA_RANGE_FORCE_LSB_SHIFT

/* -----------------------------------------------------------------------
** CAGC:  DC_GAIN register
** ----------------------------------------------------------------------- */
#define CAGC_RF_CPU_FREEZE_AGC_LOOP_VAL 0x1 
#define CAGC_RF_LOOP_NORM_VAL 0x0
#define CAGC_RF_DC_GAIN_EN_ENABLE_VAL  0x1
#define CAGC_RF_DC_GAIN_EN_DISABLE_VAL  0x0
#define CAGC_RF_DC_GAIN_MAX_VALUE 0x9
#define CAGC_RF_DC_GAIN_MIN_VALUE 0x0
#define CAGC_RF_AGC_VALUE_MIN_EN_VAL 0x1

/* -----------------------------------------------------------------------
** CAGC:  TX_RCCCH_FRAME_DELAY
** ----------------------------------------------------------------------- */
#define CAGC_RF_TX_RCCH_FRAME_DELAY_0PCGS 0x0

/* -----------------------------------------------------------------------
** CAGC:  TX_MASK_DATA_SEL
** ----------------------------------------------------------------------- */
#define CAGC_RF_TX_MASK_DATA_SEL_NORM_VAL 0x0

/* -----------------------------------------------------------------------
** CAGC:  LNA_RANGE_DELAY
** ----------------------------------------------------------------------- */
// ~ 8.65 usec for ZIF
#define CAGC_RF_LNA_RANGE_DELAY_85CHIPX8 0x55
#define CAGC_RF_LNA_RANGE_DELAY_85CHIPX4 0x5
#define CAGC_RF_LNA_RANGE_DELAY_85CHIPX2 0x28   /* 85chipx8 in units of chipx2 */

#define TX_PDM_OE_M       0x01 /* mask for TX PDM OE bit           */
#define TX_PDM_ENA_V      0x01 /* TX PDM enable                    */
#define TX_PDM_DIS_V      0x00 /* TX PDM disable                   */

#define CAGC_RF_TX_OL_FRZ_V        0x1 /* Freezes TX open-loop accum       */
#define CAGC_RF_TX_OL_NRM_V        0x0 /* TX open-loop accum normal op     */

#define CAGC_RF_MODE_SEL_1X_V      0x1  /* selects 1X */

