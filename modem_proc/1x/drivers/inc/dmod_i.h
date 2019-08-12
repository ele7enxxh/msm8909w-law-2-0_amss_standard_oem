#ifndef DMOD_I_H
#define DMOD_I_H
/*===========================================================================

              D E M O D U L A T O R    C H I P    S E R V I C E S

                            H E A D E R    F I L E

DESCRIPTION
  This file contains declarations and definitions to support interaction
  with the QUALCOMM CDMA Demodulator Chip.

  Copyright (c) 1990 - 2013 Qualcomm Technologies, Inc. 
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

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/1x/drivers/inc/dmod_i.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
04/22/11   jtm     CMI Phase 4 - Final SU API changes.
04/01/09   jtm     Removed includes for comdef.h and target.h
07/03/06   awj     Moved pilot_rec_type to sect parms, removed parm translation
06/09/05   ejv     Add back in changes and comments deleted from last checkin.
05/26/05   sst     Re-included include files, other files which include demod.h
                    need the includes
05/26/05   ejv     Removed include file.  Moved some constants to srch_comb.c
                   and srch_fing_driver.h.
04/01/05   sfm     Removed DMOD_LOCK_Fx_M macros
03/28/05   ejv     Removed unsed values.
09/29/04   ejv     Added SPR_256_CHIP_V
03/12/04   sst     Added DMOD_PILOT_GAIN_218_HZ_V
01/28/04   ejv     Mainline T_MSM6500.
04/28/03   ejv     Remove old/unused #defines.
02/18/03   ejv     Added T_MSM6500.
11/08/02   rtp     Corrected the bit masks for DP_RX_DATA interrupt
                   for 6100
08/06/02   gs      Added msm_help.h include to support shadow change.
07/30/02   gs      Initial merge into perforce for cougar.
05/30/02   ejv     Mainlined T_MSM5000.
02/11/02   rm      Merged MSM6050 support.
02/08/02   ajn     Merged MSM5500 support (From 5500 archieve: 04/04/01 sq)
01/11/02   ejv     Added DMOD_INW.
05/11/01   ejv     Removed T_MSM31.
01/18/01   ejv     Added MSM5100 support.
12/11/00   jcw     Added DMOD_RX_DATA(2)_INT_POL values for MSM5105
12/11/00   rmd     Added bit definitions for INT_STATUS_0 and INT_STATUS_1
10/16/00   ejv     Removed T_SURF.
06/29/00   aks     Added Bluetooth, MMC Controller and QDSP2 INT1 definitions
                   for MSM3300.
02/11/00   dgy     Removed FEATURE_PA_RANGE_DIG_COMP.
01/14/00   ejv     Merged FEATURE_PA_RANGE_DIG_COMP for TigerII.
10/27/99   ajn     Added TigerII interrupt for QPCH.
10/11/99   ejv     Removed T_MSM2P checking.
09/24/99   rm      Added in macros for MAC Engine.
08/06/99   ejv     Mainlined T_MSM3 support.  Changed DMOD_LOCK_RES_V to
                   DMOD_F_UNLOCK_V.  Added preliminary Pink Panther support
                   and initial Tiger2 support.
04/16/99   ejv     Added parentheses around expressions in outp commands.
01/13/99   ejv     Removed #ifndef T_MSM3 code.
12/02/98   ejv     Replaced INT 26 value and mask with DP RX2 DATA int.
10/29/98   ejv     jkl: Added word size mask for Interrupt Control Registers
10/16/98   ejv     Merged changes to support Panther.
03/04/98   jjn     Added DMOD_GP_I_POL2_INV_V and DMOD_GP_I_POL1_INV_V for
                   invereted polarity.
02/02/98   smo     Changed erroneous values for time-tracking gain constants,
                   TT_K1 and TT_K2, MSM2300 only.
10/07/97   smo     Added MSM2300 specific definitions.
06/20/97   thh     Added DMOD_GP_I_POL3_INV_V value for inverted polarity.
01/02/97   fkm     Added DMOD_GP_I_POL0_INV_V value for Inverted Polarity
                   of this interrupt.  Needed for TGP Dial Shuttle.
06/07/96   smo     Added Pilot filter bandwidth values for PCS/800 settings.
10/10/95   fkm     Minor Macro Changes to save ROM.
09/12/95   tdw/jah Added DMOD_Q_C_* bits in DMOD_AGC_CTL register.
08/29/95   jah     Fixed swapped DMOD_VOC_ENC/DMOD_VOC_DEC values.
07/17/95   rdh     Added I invert and pctl PUNC sym siz choices for MSM 2p.
04/26/95   rdh     Updated comments on finger time tracking settings.
02/17/95   jah     Added PDM1, PDM2, and new interrupt choices for MSM 2p.
12/22/93   jah     Added macro DMOD_STROBEM() for strobing a value
09/07/93   ptw     Added PCC register bit DMOD_PC_PMODE.
06/09/93   jca     Added DMOD_INT_STATUS_2.
05/02/93   ptw     Changed to new MSM conditionals.
03/15/93   ptw     Added target.h, Demod vs. MSM conditionals
02/18/93   ptw     Corrected PCC register TX_GAIN polarity and enable bits.
02/04/93   jai     Changed comments for DMOD_GAIN values.
12/30/92    gb     Added Tristate enables and polarity for TRK_LO.
                   Added port definitions for PA_VGG_VTL and TX_GAIN_COMP.
10/21/92   jah     Added M/N counter control for FM clock and Baud clock.
                   Added TXCO block tri-state control values.
07/27/92   jah     Changed to DMOD_KEY_READ_IN for BIO, added includes
07/16/92   ptw     Various changes for DMSS integration
07/02/91   rdb     Corrected DMOD_GAIN constants from 1,2,4,8 to 2,4,8,32
06/25/91   rdb     Added definition for DMOD_POS_LATCH port.
06/02/91   rdb     Minor revs.  Added bit to port 0x38 to enable/disable
                   puncturing.
05/06/91   rdb     Fixed a few comments.
03/28/91   rdb     Initial release.
11/05/90   rdb     First cut of file.

===========================================================================*/


/*===========================================================================

                        DATA DECLARATIONS

===========================================================================*/

/*---------------------------------------------------------------------------
                          Write Registers
---------------------------------------------------------------------------*/

/*-   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -*/
/* Searcher Control Register */

#define DMOD_GAIN_M     0x03  /* Mask for Gain factor */
#define DMOD_GAIN_2_V   0x00  /* Value for Gain of 2/32 =  .0625 */
#define DMOD_GAIN_4_V   0x01  /* Value for Gain of 4/32 =  .125  */
#define DMOD_GAIN_8_V   0x02  /* Value for Gain of 8/32 =  .25   */
#define DMOD_GAIN_16_V  0x03  /* Value for Gain of 16/32 = .5    */

/*-   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -*/
/* Finger Pilot Filter Gain Register */

#define DMOD_PILOT_GAIN_382_HZ_V    0x10   /* 382Hz BW for 800MHz */
#define DMOD_PILOT_GAIN_218_HZ_V    0x09   /* 218Hz BW for 450MHz */
#define DMOD_PILOT_GAIN_764_HZ_V    0x20   /* 764Hz BW for PCS */

/*-   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -*/
/* Demod Control Register */

#define DMOD_REP_FACTOR_1  0x00  /* Symbol repetition factor of 1 */
#define DMOD_REP_FACTOR_2  0x01  /* Symbol repetition factor of 2 */
#define DMOD_REP_FACTOR_4  0x02  /* Symbol repetition factor of 4 */

/*---------------------------------------------------------------------------
                          Read Registers
---------------------------------------------------------------------------*/

/*-   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -*/
/* Finger Lock RSSI Register */

#define DMOD_RSSI_GAIN_3MS_V   0x08            /* 3.3ms time constant */
#define DMOD_RSSI_GAIN_6MS_V   0x04            /* 6.6ms time constant */
#define DMOD_RSSI_GAIN_13MS_V  0x02            /* 13.2ms time constant */

/*-   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -*/

/* spreading rates for DEM1X_CHANNELx_IMMED/FRAME:RATE_SEL */
#define  DMOD_SPR_64             0x0 /* 64 chips per symbol */
#define  DMOD_SPR_128            0x1 /* 128 chips per symbol */
#define  DMOD_SPR_256            0x2 /* 256 chips per symbol */
#define  DMOD_SPR_512            0x3 /* 512 chips per symbol */

/*  MAC Engine Registers */

/* Rounding Factor for SCH Soft Decision */
#define DMOD_COMB_RND_CONVOLUTIONAL     0x0 /* Convolutional Coding */
#define DMOD_COMB_RND_TURBO             0x1 /* Turbo Coding */

/* Control for the Spreading Rate for Demodulator Channels */
#define SPR_256_CHIP_V   0x3f     /* Spreading rate = 256 chips */
#define SPR_128_CHIP_V   0x1f     /* Spreading rate = 128 chips */
#define SPR_64_CHIP_V    0x0f     /* Spreading rate = 64  chips */
#define SPR_32_CHIP_V    0x07     /* Spreading rate = 32  chips */
#define SPR_16_CHIP_V    0x03     /* Spreading rate = 16  chips */
#define SPR_8_CHIP_V     0x01     /* Spreading rate = 8   chips */
#define SPR_4_CHIP_V     0x00     /* Spreading rate = 4   chips */

/*-   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -*/

#endif /* DMOD_I_H */

