#ifndef ENCI_I_H
#define ENCI_I_H
/*===========================================================================

      E N C O D E R   I N T E R N A L   H E A D E R    F I L E

DESCRIPTION
   All the declarations and definitions necessary for the Encoder driver to
   interact with the QUALCOMM CDMA Interleaver / Encoder chip set

REFERENCES
  CDMA Mobile Station ASIC Specification Sheet

  Copyright (c) 1992 - 2016 Qualcomm Technologies, Inc.
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

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/1x/drivers/inc/enci_i.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
04/14/16   srk     Optimized TX Trigger fixes and code cleanup.
10/08/14   srk     Set the correct PN state for UTX targets.
03/04/14   dkb     Remove RxLM and TxLM hardcoded BTF delays.
02/13/14   dkb     Add support for optimized Tx trigger.
02/03/14   dkb     Feature guard Triton UTX code under FEATURE_MODEM_1X_UTX
01/09/14   dkb     Account for RxFE and TxFE digital delays to BTF.
11/25/13   cjb     Mainlined FEATURE_MODEM_1X_TRITON.
11/08/13   cjb     Updated the max limit for RF BTF to 300 cx8
10/24/13   dkb     Add changes to address FL/RL discrepencies.
09/30/13   dkb     Initial RL changes for new FW interface.
03/20/13   trc     Update Triton SCH select and interleaver settings
02/15/13   srk     Replace mcs hwio with m1x hwio
03/11/12   trc     Clean up default BTF value
09/02/11   sk      Rebased the code to the NikeL modem package by mainlining
                   FEATURE_MODEM_1X_NIKEL.
03/30/11   adw     Register setting updates for NikeL
08/04/10   vks     Remove FEATURE_MODEM_1X_GENESIS_1_0 support. Replace
                   FEATURE_MODEM_1X_GENESIS_1_1 with FEATURE_MODEM_1X_GENESIS.
06/04/10   trc     Genesis 1.1 Modulator CH3 support.
08/05/09   jtm     Moved ENC_NUM_FRAC_RATES to enc.h.
07/06/09   jtm     Mainlined FEATURE_IS2000_1X_ADV in this file for RC index
                   calculations.
04/08/09   jtm     Initial 1X Advanced feature support.
04/01/09   jtm     Recombined enci.h and enci_v.h since this file is not
                   exported outside of modem.
03/26/09   jtm     Mainlined T_MSM7600. Replaced #ifndef T_MSM7800 with
                   !FEATURE_MODEM_1X_ARTEMIS && !FEATURE_MODEM_1X_POSEIDON.
03/26/09   jtm     Added include for customer.h
03/04/09   adw     Removed the temporary _v.h include.
02/03/09   jtm     Added support for T_POSEIDON2
12/01/08   adw     Temporarily include the internal _v.h file.
11/21/08   adw     Merge from CMI sandbox, based on VU_MODEM_1X_DRIVERS.00.00.06
05/21/08   vlc     Added changes for 7800 modem to mainline.
10/16/06   trc     Add MSM7600 support
04/21/05   vlc     Main lined code in current targets, cleaned out obsolete targets,
                   cleaned out obsolete features.
03/14/05   vlc     Merged in BTF value changes for MSM7500.  (dlb')
03/05/04   bn      Checked in encoder driver changes for MSM6700
09/03/03   dna     Ported for MSM6500
02/06/02   ems     Merged in changes for MSM6050.  Added register mappings for
                   MSM6050 compatability
05/18/01   kk      RS2 TDSO support. (LTU size increased to 560)
04/26/01   vlc     Added ENC_SCH_RC_INX macro.
01/10/01   tc      Added ENC_INIT_BTF value for QRF3300
11/10/00   tc      Added MSM5100 modulator driver support
10/22/00   rmd     Cleaned up code, merged with MSM5000 code and moved bit definitions
                   to msm5XXXbits.h.
09/07/00   jcw     Added masks for VERY_EARLY_FRAME values
07/14/00   sr      Removed ENC_NO_GATING_V. This is replaced by
                   ENC_NO_GATING_PATTERN in enc.h
05/18/00   sr      Changed ENC_NO_GATING_V to 0xFFFF from 0xFF
05/08/00   sr      Added support for MSM5000B
02/08/00   bgc     Added FEATURE_PLT_RF to control the BTF.
11/19/99   sr      Changes to support Supplemental Channels
11/04/99   lcc     Changed BTF values to the optimized value of 0xc80.
10/07/99   lcc     Merged from PLT 2.0.
09/23/99   lcc     Merged in MSM5000and PLT support from PLT archive.
11/05/98   dlb     Added support for MSM3000.
08/14/98   aks     Adjusted PA warmup time for QCP-1960 to 156uSec.
07/13/98   aks     Adjusted the PA warmup time for 5GP.  QCP-860 is 150uSec
                   and QCP-1960 is 299uSec.
06/30/98   aks     Shifted BTF for QCP-1960 by 0.6 uSec.
03/19/98   aks     Configured for Odie (T_O).
02/05/98   kss     Adjusted ENC_INIT_BTF_* for Module.
12/10/97   aks     Adjusted Q-800 BTF value, brought closer to 0.0us.
12/03/97   mk      Removed ENC_TEST_POINT_SEL, defined in deci.h.
11/24/97   aks     Adjusted TGP BTF value for cellular band.  Brought MSM22
                   closer to zero so that MSM23 does not exceed standards.
06/17/97   dhh     Added target T_Q for Q Phone.
06/16/97   aks     Added CRC disable.  Added ENC_TEST_POINT_SEL.  Changes are
                   related to MSM2.3.
03/14/97   jjn     Configured for Module (T_MD)
01/23/97   jah     BTF looks good, changed TGP PA_WARMUP to 200us.
11/02/96   jah     Configured for TGP (T_T), BTF still needs tuning.
02/07/96   jah     Added ENC_INIT_BTF_* for QCP-1000
01/10/96   jah     Changed ENC_INIT_BTF_PCS for QCP-1900 to 4,224 CHIPX8's
12/22/95   jah     Added ENC_INIT_BTF_PCS, for PCS BTF configuration.
10/15/95   jah     Updated Gemini PA_WARMUP to 638us to track GDAC change.
10/10/95   fkm     Minor Macro Changes to save ROM.
09/28/95   jah     Set Gemini PA_WARMUP to 600us to track GDAC change.
08/25/95   jah     Set Gemini & Beta-2 BTF values per measurement
08/24/95   jah     Set Gemini & ISS-2 PA_WARMUP to 100us
03/20/95   jah     Added support for 14400 rate set and MSM2p encoder mods.
06/28/94   jah     Added 1st guess PA_WARMUP values for ISS-2 and CSS-1
06/28/94   jah     Added PA_WARMUP values for Gemini
05/31/94   jah     Added PA_WARMUP values for ISS1.
05/05/93   jah     Added ENC_MASK_DATA_M
05/03/93   jah     Added ENC_MASK_DATA for T_MSM.
04/26/93   jah     Added changes for Beta II mobile.
04/17/93   jah     Set the mobile PA_WARMUP to 150us after Clarence/Todd/Matt's
                   measurements.  Portable set to 100us, to only allow for
                   tolerances spec'd by Mike/Ana.
03/09/93   jah     Changed ENC_PA_WARMUP_V and ENC_PCN_PA_WARMUP_V for the
                   portable/mobile/PCN RAS PA warm-up capability.
07/27/92   jah     Made enc_image[] private
01/30/92   jah     Ported module from brassboard enc.h

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

//common
#include "customer.h"
#include "comdef.h"     /* Definitions for byte, word, etc.     */
#include "modem_1x_defs.h"

//drivers
#include "enci.h"
#include "m1x_hwio_mpss.h"

//other
#include "processor.h"  /* Definition of chip selects           */

/* These definitions used to live in the file msmbits.h, which is currently
   deprecated and will be extinct in MSM7600. */

#define ENC_U_PN_M                  HWIO_FMSK(U_PN_STATE_5,U_PN_STATE)
  /*    Mask to get/set bozo bit ENC_U_PN_S5 */
#define ENC_BOZO_M                  HWIO_FMSK(U_PN_STATE_5,BOZO)

#define ENC_BOZO_V                  (0x01 << \
        HWIO_SHFT(U_PN_STATE_5,BOZO))

#define ENC_NO_BOZO_V               (0x00 << \
        HWIO_SHFT(U_PN_STATE_5,BOZO))
#define ENC_FCH_FRAME_SIZE_V        0x0002        /* frame size value */

#define ENC_FCH_ENCODE_TIME_V       0x0003        /* encoding time value */
#define ENC_DCCH_ENCODE_TIME_V      0x0002

#define ENC_FCH_PCG_COUNT_V         0x0000        /* pcg count value */

#define ENC_TX_VERY_EARLY_FRAME_PRIOD_M  HWIO_FMSK(TX_VERY_EARLY_FRAME_CTL,VERY_EARLY_FRAME_PER)

#define ENC_IS95_MODE_M             HWIO_FMSK(MOD_CH2_ENC_CTL_0,IS_95_C)
#define ENC_IS95_C_V                (0x01 << HWIO_SHFT(MOD_CH2_ENC_CTL_0,IS_95_C))

#define ENC_IS95_A_V                (0x00 << HWIO_SHFT(MOD_CH2_ENC_CTL_0,IS_95_C))

#define ENC_IS95_FCH_ON_M           HWIO_FMSK(MOD_CH2_ENC_CTL_0,FCH_EN)
#define ENC_IS95C_FCH_ON_V          (0x01 << HWIO_SHFT(MOD_CH2_ENC_CTL_0,FCH_EN))

#define ENC_IS95C_FCH_OFF_V         (0x00 << HWIO_SHFT(MOD_CH2_ENC_CTL_0,FCH_EN))

#define ENC_IS95_SCH_ON_M           HWIO_FMSK(MOD_CH3_ENC_CTL_0,SCH_EN)
#define ENC_IS95C_SCH_ON_V          (0x01 << HWIO_SHFT(MOD_CH3_ENC_CTL_0,SCH_EN))

#define ENC_IS95C_SCH_OFF_V         (0x00 << HWIO_SHFT(MOD_CH3_ENC_CTL_0,SCH_EN))
#define ENC_WALSHCOVER_SEL_SCH_M    0x000F
#define ENC_WALSHCOVER_SEL_SCH_1_V  0x0001
#define ENC_WALSHCOVER_SEL_SCH_2_V  0x0002
#define ENC_WALSHCOVER_SEL_FCH_M    0x00F0
#define ENC_WALSHCOVER_SEL_FCH_V    0x0040
#define ENC_WALSHCOVER_SEL_DCCH_M   0x0F00
#define ENC_WALSHCOVER_SEL_DCCH_V   0x0800

#define ENC_SCH_ENCODE_TIME_V       0x0000

#define ENC_SCH_PCG_COUNT_V         0x0000

#define ENC_SCH_DATA_RATE_M         0x001E
#define ENC_SCH_RATE_SET_M          0x0020

#define ENC_SCH_FRAME_SIZE_20_V     0x0002

#define ENC_SCH_SELECT_SCH_V        0x0000
#define ENC_SCH_INTLV_SIZE_1X_V     0x0000
#define ENC_SCH_INTLV_SIZE_2X_V     0x0000
#define ENC_SCH_INTLV_SIZE_4X_V     0x0020
#define ENC_SCH_INTLV_SIZE_8X_V     0x0040
#define ENC_SCH_INTLV_SIZE_16X_V    0x0060

#define ENC_SCH_LOW_REPEAT_1_V      0x0000
#define ENC_SCH_LOW_REPEAT_2_V      0x0200
#define ENC_SCH_LOW_REPEAT_4_V      0x0400
#define ENC_SCH_LOW_REPEAT_8_V      0x0600
#define ENC_SCH_LOW_REPEAT_16_V     0x0800
#define ENC_SCH_MULTIFRAME_1_V      0x0000

#define ENC_SCH_CODE_RATE_M         HWIO_FMSK(MOD_CH3_ENC_CTL_0,SCH_CODE_RATE)
#define ENC_PICH_ON_V                (0x01 << \
        HWIO_SHFT(MOD_MISC_CTL,CDMA2000_PICH_EN))

#define ENC_PICH_OFF_V               (0x00 << \
        HWIO_SHFT(MOD_MISC_CTL,CDMA2000_PICH_EN))

#define ENC_NO_MOD_ROTATION_V       0x0000
#define ENC_FCH_CTL_DIS_V           0x0000
#define ENC_SCH_CTL_DIS_V           0x0000
#define ENC_CLEAR_CRC_POLY_V        0x0000
#define ENC_SCH_LTU_SIZE_ZERO_V     0x0000

#define ENC_PUNC_PATTERN_NONE_V     0xFFFF

#define ENC_TURBO_CODE_M            HWIO_FMSK(MOD_CH3_ENC_CTL_0,TURBO_ENCODE)
#define ENC_TURBO_CODE_V            (0x01 << HWIO_SHFT(MOD_CH3_ENC_CTL_0,TURBO_ENCODE))

#define ENC_CONV_CODE_V             (0x00 << HWIO_SHFT(MOD_CH3_ENC_CTL_0,TURBO_ENCODE))

#define ENC_LTU_ENA_M               HWIO_FMSK(MOD_CH3_ENC_CTL_0,SCH_LTU_EN)
#define ENC_LTU_ENA_V               (0x01 << HWIO_SHFT(MOD_CH3_ENC_CTL_0,SCH_LTU_EN))

#define ENC_LTU_DIS_V               (0x00 << HWIO_SHFT(MOD_CH3_ENC_CTL_0,SCH_LTU_EN))

#define ENC_SCH_RC4_TURBO_PUNCT_0_PATT    0xA000
#define ENC_SCH_RC4_TURBO_PUNCT_1_PATT    0x00DD

/*      CRC Length; 0 means disable */
#define ENC_DCCH_CRCLEN_M         0x3E00  /* CRC Length; 0 means disable */

#define ENC_FCH_CRCLEN_M          0x1F00  /* CRC Length; 0 means disable */

/*      CRC Length; 0 means disable */
#define ENC_SCH_CRCLEN_M            HWIO_FMSK(MOD_CH3_ENC_CTL_0,SCH_CRC_LENGTH)

#define ENC_CRCDIS_V                0x00    /* Don't Generate and insert CRC */

/*      Transmit clocks disable     */
#define ENC_TX_CLKS_DIS_M           HWIO_FMSK(MOD_CLK_CTL,TX_CLKS_DISABLE)

/*      Transmit clocks disable on  */
#define ENC_TX_CLKS_DIS_V           (0x01 << \
        HWIO_SHFT(MOD_CLK_CTL,TX_CLKS_DISABLE))

/*      Transmit clocks disable off */
#define ENC_TX_CLKS_ENA_V           (0x00 << \
        HWIO_SHFT(MOD_CLK_CTL,TX_CLKS_DISABLE))

/*      TX_PUNCT follows MASK_DATA */
#define ENC_TXP_MASK_V              (0x01 << \
        HWIO_SHFT(MOD_MISC_CTL,PA_CTL))

/*      TX_PUNCT on (high)         */
#define ENC_TXP_ON_V                (0x02 << \
        HWIO_SHFT(MOD_MISC_CTL,PA_CTL))

/*      yes */
#define ENC_ACCESS_V                (0x01 << \
        HWIO_SHFT(MOD_MISC_CTL,IS95A_ACCESS_CH))

/*      no  */
#define ENC_NO_ACCESS_V             (0x00 << \
        HWIO_SHFT(MOD_MISC_CTL,IS95A_ACCESS_CH))

/*      IS95 C Pilot transmitted */
#define ENC_IS95C_PILOT_M           HWIO_FMSK(MOD_MISC_CTL,CDMA2000_PICH_EN)

/*      no  */
#define ENC_NO_IS95C_PILOT_V        (0x00 << \
        HWIO_SHFT(MOD_MISC_CTL,CDMA2000_PICH_EN))

/*      TX in offset-bin or 2'c comp */
#define ENC_TX_FMT_M                HWIO_FMSK(TX_GENERAL_CTL,TX_DATA_FORMAT)

/*      TX offset-binary             */
#define ENC_TX_OFF_V                (0x00 << \
        HWIO_SHFT(TX_GENERAL_CTL,TX_DATA_FORMAT))

/* latch the gating rate bits every 20ms */
#define ENC_PCH_GRATE_20MS_LAT_V                (0x00 << \
        HWIO_SHFT(MOD_TEST_CTL,PCH_GATE_5MS_LAT))

/*      Set MASK low                  */
#define ENC_MASK_LO_V               (0x00 << \
        HWIO_SHFT(MOD_TEST_CTL,MASK_CTL))

/*      source = FIR filter           */
#define ENC_TX_FIR_V                (0x00 << \
        HWIO_SHFT(MOD_TEST_CTL,TX_DATA_CTL))

/*      Rx spectral inversion         */
#define ENC_RX_POL_M                HWIO_FMSK(CDMA_RXFc_CTL,SPECTRAL_INVERSION)

#define ENC_STMR_CMD_LC_STATE_80MS_V  (0x01 << \
        HWIO_SHFT(MOD_STMR_CMD, TX_LC_STATE_LD))
#define ENC_STMR_CMD_LC_MASK_20MS_V   (0x02 << \
        HWIO_SHFT(MOD_STMR_CMD, TX_MASK_LD))

#define ENC_PICH_GATING_RATE_M         0x0600
#define ENC_PICH_GATING_RATE_ALL_V     0x0000
#define ENC_PICH_GATING_RATE_HALF_V    0x0200
#define ENC_PICH_GATING_RATE_QUARTER_V 0x0400

#define ENC_FCH_EIGHTH_GATE_M         0x0800
#define ENC_FCH_EIGHTH_GATE_ENA_V     0x0800
#define ENC_FCH_EIGHTH_GATE_DIS_V     0x0000

#define ENC_RETRANS_M                 0x1000
#define ENC_RETRANS_DIS_V             0x0000


#define ENC_DCCH_EN_M                 0x0001
#define ENC_DCCH_EN_V                 0x0001
#define ENC_DCCH_DIS_V                0x0000


/*===========================================================================

                        DATA DECLARATIONS

===========================================================================*/

/* Variables for handling the time it takes to boot the emulator */
#define ENC_CHK_BOOT           5 /* Check if emulator booted every N ticks    */

#define ENC_BOOT_TOUT   (3000 / ENC_CHK_BOOT)   /* Timeout for emulator boot */

/* Initial I & Q PN state (set MSB of 15 bit register) */
#define ENC_PN_INIT_STATE      0x4000

/* Maintain 3 PCG trigger margin: 3pcg * 1536cx1/pcg * 8cx8/cx1 = 36864 cx8 */
#define ENC_TRIG_MARGIN_CX8    36864

/*---------------------------------------------------------------------------
                          Write Registers
----------------------------------------------------------------------------*/

/*-   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -*/

  /* Warmup delay for the chip, time turned on before data is transmitted */

/* Time the PA needs to "warm up" before transmitting a power control
** groups worth of bits.  The equation is:
**
** PA_WARMUP_V = (153.6 * DELAY) + 64
**
** for a "warm up" time of (.15 + 10%) .15 ms = 0t87 = 0x57
**                                     .10 ms = 0t80 = 0x50
*/
#define ENC_PA_WARMUP_V         0xA2    /* .638 ms */
#define ENC_TX_WARMUP_V         0xA2
#define ENC_PCN_PA_WARMUP_V     0xA2    /* .638 ms */

#define ENC_LTU_MIN                 32
#define ENC_LTU_MAX                 1144

#define ENC_FCH_RATE_POS      2
#define ENC_SCH_RATE_POS      1
#define ENC_RATE_SET_POS      4
#define ENC_FCH_CODE_RATE_POS 5
#define ENC_FCH_CRCLEN_POS    8
#define ENC_SCH_CODE_RATE_POS 6
#define ENC_SCH_CRCLEN_POS    8
#define ENC_SCH_GAIN_POS      8
#define ENC_GATING_MODE_POS   10

#define ENC_FCH_ENCODE_TIME_POS       0
#define ENC_FCH_PCG_COUNT_POS         2
#define ENC_FCH_RADIO_CONFIG_POS      2
#define ENC_FCH_FRAME_SIZE_POS        0
#define ENC_SCH_ENCODE_TIME_POS       0
#define ENC_SCH_PCG_COUNT_POS         2
#define ENC_SCH_RATE_SET_POS          5
#define ENC_SCH_SELECT_POS            0
#define ENC_SCH_FRAME_SIZE_POS        2
#define ENC_SCH_VARIABLE_RATE_POS     4
#define ENC_SCH_INTLV_SIZE_POS        5
#define ENC_SCH_LOW_REPEAT_POS        9
#define ENC_SCH_MULTIFRAME_POS        12
#define ENC_SCH_WALSHCOVER_POS        14

  /* Rel C DV hardware allows encoder interrupt to happen every PCG in a 20ms
  ** frame.  Concept of 5ms bin is replaced by specifying the PCG, ranging
  ** in value from 0 to 15.  Need these defines to be backwards compatible
  ** with non Rel C code.
  */
#define ENC_INT_BIN_MASK_VALUE_BIN_0 0x000f      /* 20 ms bin 0    */
#define ENC_INT_BIN_MASK_VALUE_BIN_1 0x00f0      /* 20 ms bin 1    */
#define ENC_INT_BIN_MASK_VALUE_BIN_2 0x0f00      /* 20 ms bin 2    */
#define ENC_INT_BIN_MASK_VALUE_BIN_3 0xf000      /* 20 ms bin 3    */

#define ENC_INT_BIN_0 0x0001                     /* 20 ms bin 0    */
#define ENC_INT_BIN_1 0x0002                     /* 20 ms bin 1    */
#define ENC_INT_BIN_2 0x0004                     /* 20 ms bin 2    */
#define ENC_INT_BIN_3 0x0008                     /* 20 ms bin 3    */

/*-   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -*/

/*      data rate mask */
#define ENC_FCH_DATA_RATE_M         HWIO_FMSK(MOD_CH2_ENC_CTL_0,FCH_ENC_RATE)

#define ENC_FCH_CODE_RATE_M         HWIO_FMSK(MOD_CH2_ENC_CTL_0,FCH_CODE_RATE)

#define ENC_FCH_RADIO_CONFIG_M      0x000C        /* radio config mask */

#define ENC_FCH_FRAME_SIZE_M        0x0003        /* frame size mask */
#define ENC_FCH_ENCODE_TIME_M       0x0003        /* encoding time mask */
#define ENC_FCH_PCG_COUNT_M         0x0004        /* pcg count mask */
#define ENC_DCCH_GAIN_M             0xFF00

#define ENC_PCH_GAIN_M              HWIO_FMSK(MOD_PCH_GAIN,PCH_GAIN)
#define ENC_SCH_GAIN_M              HWIO_FMSK(MOD_SCH_FCH_GAIN,SCH_GAIN)
#define ENC_FCH_GAIN_M              HWIO_FMSK(MOD_SCH_FCH_GAIN,FCH_GAIN)

#define ENC_TX_VERY_EARLY_FRAME_ADV_M    HWIO_FMSK(TX_VERY_EARLY_FRAME_CTL,VERY_EARLY_FRAME_ADV)



#define ENC_WALSHCOVER_SEL_M        0x0FFF
#define ENC_SCH_ENCODE_TIME_M       0x0003
#define ENC_SCH_PCG_COUNT_M         0x0004
#define ENC_SCH_FRAME_SIZE_M        0x0003
#define ENC_SCH_FRAME_SIZE_5_V      0x0000
#define ENC_SCH_FRAME_SIZE_10_V     0x0001
#define ENC_SCH_SELECT_M            0x000C
#define ENC_SCH_SELECT_NONE_V       0x0000
#define ENC_SCH_SELECT_CCCH_V       0x0008
#define ENC_SCH_SELECT_EACH_V       0x000C

#define ENC_SCH_INTLV_SIZE_M        0x00E0
#define ENC_SCH_LOW_REPEAT_M        0x0E00
#define ENC_SCH_LOW_REPEAT_32_V     0x0A00

#define ENC_SCH_MULTIFRAME_M        0x3000
#define ENC_SCH_MULTIFRAME_2_V      0x1000
#define ENC_SCH_MULTIFRAME_4_V      0x2000

#define ENC_PICH_M                   HWIO_FMSK(MOD_MISC_CTL,CDMA2000_PICH_EN)

#define ENC_ZERO_GAIN_V             0x0000

/*-   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -*/
/*      Codec Control */
#define ENC_CODEC_CTL_M             HWIO_FMSK(MOD_CLK_CTL,CODEC_CTL)
/*      CODEC_CLK and CODEC_SYNC low    */

#define ENC_CODEC_LO_V              (0x00 << \
        HWIO_SHFT(MOD_CLK_CTL,CODEC_CTL))

/*      CODEC_CLK low, CODEC_SYNC hi    */
#define ENC_CODEC_LOHI_V            (0x01 << \
        HWIO_SHFT(MOD_CLK_CTL,CODEC_CTL))

/*      CODEC_CLK hi, CODEC_SYNC low    */
#define ENC_CODEC_HILO_V            (0x02 << \
        HWIO_SHFT(MOD_CLK_CTL,CODEC_CTL))

/*      CODEC_CLK and CODEC_SYNC high   */
#define ENC_CODEC_HI_V              (0x03 << \
        HWIO_SHFT(MOD_CLK_CTL,CODEC_CTL))

/*      CODEC_CLK and CODEC_SYNC normal */
#define ENC_CODEC_NORM_V            (0x04 << \
        HWIO_SHFT(MOD_CLK_CTL,CODEC_CTL))

/*      Vocoder frame reference disable - if set, SET VOC_FR_REF, VOC_CS_B,
        VOC_WR_B, VOC_OE_B low */
#define ENC_VOC_REF_DIS_M           HWIO_FMSK(MOD_CLK_CTL,VOC_REF_DISABLE)

#define ENC_VOC_REF_DIS_V           (0x01 << \
        HWIO_SHFT(MOD_CLK_CTL,VOC_REF_DISABLE))

#define ENC_VOC_REF_ENA_V           (0x00 << \
        HWIO_SHFT(MOD_CLK_CTL,VOC_REF_DISABLE))

/*-   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -*/
/*      Power amplifier control    */
#define ENC_TXP_CTL_M               HWIO_FMSK(MOD_MISC_CTL,PA_CTL)

/*      TX_PUNCT off (low)         */
#define ENC_TXP_OFF_V               (0x00 << \
        HWIO_SHFT(MOD_MISC_CTL,PA_CTL))


/*      Access channel transmitted */
#define ENC_ACCESS_M                HWIO_FMSK(MOD_MISC_CTL,IS95A_ACCESS_CH)

/*      yes */
#define ENC_IS95C_PILOT_V           (0x01 << \
        HWIO_SHFT(MOD_MISC_CTL,CDMA2000_PICH_EN))

/*      TX in 2's comp               */
#define ENC_TX_2S_V                 (0x01 << \
        HWIO_SHFT(TX_GENERAL_CTL,TX_DATA_FORMAT))


/*-   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -*/
/*                             CORDIC ROTATOR                              */
/*-   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -*/

/* Enable the Cordic Rotator          */
#define ENC_SET_CORDIC_EN_V                (0x01 << \
        HWIO_SHFT(MOD_TEST_CTL,CORDIC_EN))

/* Disable the Cordic Rotator         */
#define ENC_NO_SET_CORDIC_EN_V             (0x00 << \
        HWIO_SHFT(MOD_TEST_CTL,CORDIC_EN))

/*-   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -*/
/*                             GATING RATE                                 */
/*-   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -*/

/* latch the gating rate bits every 5ms */
#define ENC_PCH_GRATE_5MS_LAT_V                (0x01 << \
        HWIO_SHFT(MOD_TEST_CTL,PCH_GATE_5MS_LAT))



/*-   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -*/
/*                             TEST REGISTERS                              */
/*-   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -*/

/*      Set IQ Phase Waveform         */
#define ENC_SET_IQ_M                HWIO_FMSK(MOD_TEST_CTL,SET_IQ_HIGH)

/*      Internal Master Mask Control  */
#define ENC_MASK_CTL_M              HWIO_FMSK(MOD_TEST_CTL,MASK_CTL)

/*      MASK functions normally       */
#define ENC_MASK_V                  (0x01 << \
        HWIO_SHFT(MOD_TEST_CTL,MASK_CTL))

/*      Set MASK high                 */
#define ENC_MASK_HI_V               (0x03 << \
        HWIO_SHFT(MOD_TEST_CTL,MASK_CTL))

/*      Transmit data control         */
#define ENC_TX_DAT_CTL_M            HWIO_FMSK(MOD_TEST_CTL,TX_DATA_CTL)

/*      source = TX_DATA_TEST         */
#define ENC_TX_TEST_V               (0x01 << \
        HWIO_SHFT(MOD_TEST_CTL,TX_DATA_CTL))
/*      Tx spectral inversion         */
#define ENC_TX_POL_M                HWIO_FMSK(MOD_TEST_CTL,TX_SPECTRAL_INVERSION)

/*-   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -*/

#define ENC_STMR_CMD_M                (HWIO_RMSK(MOD_STMR_CMD))
#define ENC_STMR_CMD_LC_STATE_NOP_V   (0x00 << \
        HWIO_SHFT(MOD_STMR_CMD, TX_LC_STATE_LD))
#define ENC_STMR_CMD_LC_MASK_NOP_V    (0x00 << \
        HWIO_SHFT(MOD_STMR_CMD, TX_MASK_LD))
#define ENC_STMR_CMD_LC_MASK_IMMED_V  (0x01 << \
        HWIO_SHFT(MOD_STMR_CMD, TX_MASK_LD))
#define ENC_RETRANS_ENA_V             0x1000
#define ENC_IS95A_MODE_M              0x2000
#define ENC_IS95A_MODE_ENA_V          0x2000
#define ENC_IS95A_MODE_DIS_V          0x0000

/*===========================================================================

                      TYPE DECLARATIONS

===========================================================================*/
typedef struct
{
   byte length;
   word poly;
} enc_crc_info_type;

/*-   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -*/

/*-------------------------------------------------------------------------*/

/*===========================================================================

                      MACRO FUNCTION DECLARATIONS

===========================================================================*/


/*===========================================================================

MACRO ENC_OUT

DESCRIPTION
  This macro outputs a specified value to a specified port. All 8 bits of the
  port are written (i.e., the port is not buffered by software).

PARAMETERS
  io    Encoder register defined by this header via ENC_IO()
  val   Value to write the Encoder register

DEPENDENCIES
  Address activated registers will not activate if CHIPX8 is off.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

#define  ENC_OUT( io, val )  MSM_OUT( io, val )


/*===========================================================================

MACRO ENC_OUTM

DESCRIPTION
  This macro outputs a specified value, qualified by a specified mask, to
  a specified port address. Only the bits corresponding to the mask are
  actually affected; other bits retain their previous values. To do this,
  an image is maintained of the previous value written to the port which
  is then combined with the new value.

PARAMETERS
  io    Encoder register defined by this header via ENC_IO()
  mask  Mask of bits in Encoder register to write
  val   Value to write the Encoder register

DEPENDENCIES
  Address activated registers will not activate if CHIPX8 is off.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

#define ENC_OUTM(io, mask, val)  MSM_OUTM(io, mask, val)

//

/*===========================================================================

MACRO ENC_OUTH

DESCRIPTION
  This macro outputs a specified value to a specified port. All 16 bits of the
  port are written (i.e., the port is not buffered by software).

PARAMETERS
  io    Encoder register defined by this header via ENC_IO()
  val   Value to write the Encoder register

DEPENDENCIES
  Address activated registers will not activate if CHIPX8 is off.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

#define  ENC_OUTH( io, val )  MSM_OUTH( io, val )


/*===========================================================================

MACRO ENC_OUTHM

DESCRIPTION
  This macro outputs a specified value, qualified by a specified mask, to
  a specified port address. Only the bits corresponding to the mask are
  actually affected; other bits retain their previous values. To do this,
  an image is maintained of the previous value written to the port which
  is then combined with the new value.

PARAMETERS
  io    Encoder register defined by this header via ENC_IO()
  mask  Mask of bits in Encoder register to write
  val   Value to write the Encoder register

DEPENDENCIES
  Address activated registers will not activate if CHIPX8 is off.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

#define ENC_OUTHM(io, mask, val)  MSM_OUTHM(io, mask, val)


/*===========================================================================

MACRO ENC_IN

DESCRIPTION
  This macro reads and returns the value from a specified port. No mask is
  applied to the read value.

PARAMETERS
  io    Encoder register defined by this header via ENC_IO()

DEPENDENCIES
  Address activated registers will not activate if CHIPX8 is off.

RETURN VALUE
  byte read from the Encoder register.

SIDE EFFECTS
  None

===========================================================================*/

#define  ENC_IN( io )  MSM_IN( io )


/*===========================================================================

MACRO ENC_INM

DESCRIPTION
  This macro reads the value from a specified I/O port address and then
  applies a specified mask to the value. Bits not corresponding to the
  mask are returned as zero.

PARAMETERS
  io    Encoder register defined by this header via ENC_IO()
  mask  Mask of bits in Encoder register to read

DEPENDENCIES
  Address activated registers will not activate if CHIPX8 is off.

RETURN VALUE
  byte read from the Encoder register.

SIDE EFFECTS
  None

===========================================================================*/

#define  ENC_INM( io, mask )  MSM_INM( io, mask )


/*===========================================================================

MACRO ENC_FCH_RC_INX

DESCRIPTION
   This converts the 1-based enum to a 0 based index for array
   subscripting.  If the RC is ENC_FCH_RC_8 (new for 1x Advanced) then use
   ENC_NUM_FCH_RCS-1.

PARAMETERS

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS
  None

===========================================================================*/
#define  ENC_FCH_RC_INX( rc ) \
(((uint16) (rc) == (uint16) ENC_FCH_RC_8) ? \
                        ((uint16) ENC_NUM_FCH_RCS - 1):((uint16) ( rc ) - 1))


/*===========================================================================

MACRO ENC_FCH_RATE_INX

DESCRIPTION
   This converts the old defintions of fractional rates to what is
   understood by MSM5000.

PARAMETERS

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS
  None

===========================================================================*/

#define  ENC_FCH_RATE_INX( rate )  ( 3 - (uint16) ( rate ) )


/*===========================================================================

MACRO ENC_SCH_RC_INX

DESCRIPTION
   This converts the 3-based enum to a 0 based index for array
   subscripting. If the RC is ENC_SCH_RC_8 (new for 1x Advanced) then use
   ENC_NUM_SCH_RCS-1.

PARAMETERS

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS
  None

===========================================================================*/
#define  ENC_SCH_RC_INX( rc ) \
(((uint16) (rc) == (uint16) ENC_SCH_RC_8) ? \
                        ((uint16) ENC_NUM_SCH_RCS - 1):((uint16) ( rc ) - 3))

#endif  /* ENCI_I_H */
