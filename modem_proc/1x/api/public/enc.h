#ifndef ENC_H
#define ENC_H
/*===========================================================================

      E N C O D E R   ( I N T E R L E A V E R )    H E A D E R    F I L E

DESCRIPTION
  All the declarations and definitions necessary to support interaction
  with the QUALCOMM CDMA Interleaver / Encoder chip set

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

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/1x/api/public/enc.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
11/26/14   jh      remove qw.h include
11/13/12   trc     Layering/Cleanup for Triton
04/05/12   jtm     Removed msm.h include.
08/31/09   adw     Removed unnecessary customer.h include.
08/13/09   jtm     Removed FEATURE_1X_DISABLE_CMI.
08/05/09   jtm     Lint fixes. Added ENC_NUM_FRAC_RATES to enc_rate_type.
07/06/09   jtm     Removed featurization for CMI phase2 compliance.
04/28/09   jtm     Initial 1X Advanced feature support.
04/08/09   adw     Added temporary CMI-lite featurization.
04/06/09   jtm     Added ENC_RATE_INVALID to enc_rate_set_type.
04/02/09   jtm     Added customer.h. Removed target.h
03/20/09   jtm     Moved enc_gating_mode_type and enc_rate_type from private
                   header to public header.
03/04/09   adw     Removed the temporary _v.h include.
12/01/08   adw     Temporarily include the internal _v.h file.
11/21/08   adw     Merge from CMI sandbox, based on VU_MODEM_1X_DRIVERS.00.00.06
07/16/08   jtm     Removed unused functions as part of the ULC memory reduction
                   effort.
05/21/08   vlc     Added changes for 7800 modem to mainline.
04/21/05   vlc     Main lined code in current targets, cleand out obsolete targets,
                   cleaned out obsolete features.
10/12/04   ljl     Updated encoder channel for R-SCH.
06/18/04   bn      Renamed the encoder channel enum type
03/05/04   bn      Checked in encoder driver changes for MSM6700
01/06/03   bn      Added new function to handle encoder's writing for MUXPDU5
03/14/02   ems     Added enci.h include
02/22/02   tc      Supported FEATURE_IS2000_REL_A_CC.
02/11/02   sr      Removed enc_version()
02/08/02   tc      Supported Reverse Enhance Access Channel.
02/06/02   ems     Merged in changes for MSM6050.  Featurized enc_voc_ctl and
                   enc_codec_clk_ctl.  Note MOD_CLK_CTL is test use only.
                   Removed ENC_CHIPX8_CLK_DISABLE ENC_CHIPX8_CLK_ENABLE.
11/08/01   hrk     Added function prototype for enc_pcbit_test_mask().
07/12/01   vlc     Changed FEATURE_RC4_SCH to FEATURE_DS_IS2000_RS2 in order
                   to eliminate FEATURE_RC4_SCH.
05/27/01   sr/snn  Modified ENC_SCH_DATA_OUT to take pdu size as a parameter.
05/03/01   tc      Supported reverse link DCCH.
04/26/01   vlc     Added Reverse Link SCH RC4 support.
04/09/01   sr      Rounded up size of SCH encoder data to nearest 32 bits
03/23/01   sr      Added support for 32X
02/28/01   tn      Added support for FEATURE_ENC_BTF_IN_NV.
02/14/01   tn      Added enc_set_rx_polarity() and enc_set_tx_polarity().
                   Added new definitions for Tx/Rx spectral polarity.
11/10/00   tc      Defined feature T_MSM_5100
11/09/00   tc      Added MSM5100 modulator driver support
10/22/00   rmd     Clean up code and merged with MSM5000 code.
09/07/00   jcw     Changed include to generic msm.h instead of msm50reg.h
07/14/00   sr      Added enc_gating_pattern_type enum defintion
06/22/00   sr      Added boolean generate_crc to enc_sch_data_out_cfg()
05/08/00   sr      Added support for MSM5000B
                   Added enc_sch_data_out_cfg()
                   Added macro ENC_SCH_DATA_OUT()
                   Changed ENC_RDWR_M to include bit for SCH
04/25/00   sr      Added function prototype for enc_pcbit_test_data()
04/11/00   sr      Merged in rel 0.3 changes:
                      Added function descriptions as part of code cleanup
                      for release 0.3
                   Removed unused prototype for enc_dcch_gain()
11/19/99   sr      Changes to support Supplemental Channels
10/07/99   lcc     Merged in from PLT 2.0.
09/23/00   lcc     Merged in MSM5000 support from PLT archive.
12/28/94   jah     Added support for 14400 rate set.
10/20/94   jah     Changed enc_pn_rand_delay() to return previous delay.
10/11/94   jah     Added enc_pn_rand_delay()
03/07/94   jah     Improve some function comments.
05/11/93   jah     Added enc_chipx8_clk_enable/disable.
08/31/92   jah     Added enc_init_pa() to configure PA control for PCN/not
07/31/92   jah     Added enc_acp_pa_ctl() to control the PA for Analog calls.
07/28/92   jah     Added enc_voc_ctl() and enc_codec_clk_ctl()
01/30/92   jah     Ported module from brassboard enc.h

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

//common
#include "comdef.h"     /* Definitions for byte, word, etc.     */

//other
#include "rex.h"        /* definition of REX data types         */
#include "misc.h"

/*===========================================================================

                        DATA DECLARATIONS

===========================================================================*/

typedef enum
{
  ENC_NO_GATING_MODE = 0x0,
  ENC_HHO_PREAMBLE_MODE = 0x1,
  ENC_FCH_8TH_RATE_GATING_MODE = 0x2,
  ENC_PILOT_GATING_MODE = 0x4
} enc_gating_mode_type;

/* Data encoding rates for enc_data_out()
*/
typedef enum {
  ENC_FULL_RATE = 0x00,    /* Encode "full" rate frames    */
  ENC_HALF_RATE = 0x01,    /* Encode "half" rate frames    */
  ENC_QTR_RATE  = 0x02,    /* Encode "quarter" rate frames */
  ENC_8TH_RATE  = 0x03,    /* Encode "eighth" rate frames  */
  ENC_NUM_FRAC_RATES = 0x04
} enc_rate_type;

/* Data encoding rate sets for enc_data_out()
*/
typedef enum {
  ENC_RATE_9600  = 0x00,  /* Select 9600 bps rate set   */
  ENC_RATE_14400 = 0x01,  /* Select 14400 bps rate set  */
  ENC_RATE_INVALID = 0x02 /* Invalid rate set           */
} enc_rate_set_type;

/* Data encoding rates for enc_sch_data_out()
*/
typedef enum
{
  ENC_SCH_8TH_RATE     = 0x00, /* RC3:    1.5 Kbps, RC4:    1.8 Kbps    */
  ENC_SCH_QTR_RATE     = 0x01, /* RC3:    2.7 Kbps, RC4:    3.6 Kbps    */
  ENC_SCH_HALF_RATE    = 0x02, /* RC3:    4.8 Kbps, RC4:    7.2 Kbps    */
  ENC_SCH_FULL_RATE    = 0x03, /* RC3:    9.6 Kbps, RC4:   14.4 Kbps    */
  ENC_SCH_2X_RATE      = 0x04, /* RC3:   19.2 Kbps, RC4:   28.8 Kbps    */
  ENC_SCH_4X_RATE      = 0x05, /* RC3:   38.4 Kbps, RC4:   57.6 Kbps    */
  ENC_SCH_8X_RATE      = 0x06, /* RC3:   76.8 Kbps, RC4:  115.2 Kbps    */
  ENC_SCH_16X_RATE     = 0x07, /* RC3:  153.5 Kbps, RC4:  230.4 Kbps    */
  ENC_SCH_32X_RATE     = 0x08, /* RC3:  307.2 Kbps, RC4:  not supported */
  ENC_NUM_SCH_RATES
} enc_sch_rate_type;

typedef enum
{
  ENC_SCH_RC_3 = 3,
  ENC_SCH_RC_4 = 4,
  ENC_SCH_RC_8 = 8,  /* New RC for 1x Advanced */
  ENC_NUM_SCH_RCS =  3
} enc_sch_radio_config_type;


#endif  /* ENC_H */

