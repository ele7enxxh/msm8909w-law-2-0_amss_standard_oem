#ifndef ENCI_H
#define ENCI_H
/*===========================================================================

      E N C O D E R   I N T E R N A L   H E A D E R    F I L E

DESCRIPTION
   All the declarations and definitions necessary for the Encoder driver to
   interact with the QUALCOMM CDMA Interleaver / Encoder chip set

REFERENCES
  CDMA Mobile Station ASIC Specification Sheet

  Copyright (c) 1992 - 2013 Qualcomm Technologies, Inc. 
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

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/1x/api/enci.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
11/15/10   jtm     Initial SU API split for CP.
11/09/10   jtm     Initial SU API split.
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
#include "comdef.h"     /* Definitions for byte, word, etc.     */

//other
#include "processor.h"  /* Definition of chip selects           */

#ifndef FEATURE_MODEM_1X_BOLT_REVISIT
/*      I & Q high after spreading    */
#define ENC_SET_IQ_V                (0x01 << \
        HWIO_SHFT(MOD_TEST_CTL,SET_IQ_HIGH))

/*      Don't force I & Q             */
#define ENC_NO_SET_IQ_V             (0x00 << \
        HWIO_SHFT(MOD_TEST_CTL,SET_IQ_HIGH))
#endif /* FEATURE_MODEM_1X_BOLT_REVISIT */


/*===========================================================================

                        DATA DECLARATIONS

===========================================================================*/


/* Initial Transmit Frame interrupt is offset from the frame boundary by
** this offset in power control groups and walsh symbols
*/
#define ENC_INIT_PCG_T_OFF      15      /* Power control group time offset */
#define ENC_INIT_WS_T_OFF       0       /* Walsh symbol time offset        */


/*---------------------------------------------------------------------------
                          Write Registers
----------------------------------------------------------------------------*/


/*-   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -*/
/*      Power amplifier control    */

#define ENC_ZERO_PCBIT_V            0x0000

#endif  /* ENCI_H */
