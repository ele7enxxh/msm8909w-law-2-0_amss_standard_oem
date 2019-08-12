#ifndef ENC_V_H
#define ENC_V_H
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

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/1x/api/enc_v.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
10/21/13   dkb     Add 1x waveform API changes to init fw smem pointer.
04/19/13   trc     Add ENC_SLEEP_MODE for cleaner enc/mod/dac teardown API
12/10/12   trc     Layering/Cleanup for Triton
08/17/11   trc     Mainline RF SVDO API
05/09/11   trc     Add enc_tx_1x_test_waveform() API
11/15/10   jtm     Initial SU API split for CP.
11/09/10   jtm     Initial SU API split.
10/07/10   jtm     Added enc_update_tx_clk_resources().
01/22/10   trc     Mainlined FEATURE_ENC_BTF_IN_NV.
11/07/09   vks     Delete enc_<channel_type>_gain() functions. Use the data
                   structure channel_gains and api provided by muxmdsp.h.
09/01/09   jtm     Changed macro ENC_RC_TO_RATE_SET to become a function.
08/25/09   jtm     Modified ENC_RC_TO_RATE_SET to return the correct rate set
                   for RC8.
08/21/09   jtm     Lint fixes.
07/06/09   jtm     Mainlined RC8 to remove FEATURE_IS2000_1X_ADV for CMI.
04/08/09   jtm     Initial 1X Advanced feature support.
03/26/09   jtm     Replaced #ifndef T_MSM7800 with
                   !FEATURE_MODEM_1X_ARTEMIS && !FEATURE_MODEM_1X_POSEIDON.
03/26/09   jtm     Added include for customer.h and modem_1x_defs.h
03/20/09   jtm     Moved enc_gating_mode_type and enc_rate_type from private
                   header to public header.
03/10/09   vlc     Added function enc_hw_reset() which resets encoder HW block.
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

//drivers
#include "enci.h"
#include "enc.h"

//other
#include "qw.h"         /* definition of qword                  */
#include "rex.h"        /* definition of REX data types         */
#include "misc.h"

#include "cdmafw_msg.h"

/*===========================================================================

                        DATA DECLARATIONS

===========================================================================*/

/* Channel states for the Encoder
*/
typedef enum {
  ENC_AC_MODE,          /* Access Channel mode                  */
  ENC_TC_MODE,          /* Traffic Channel/Acquisition mode     */
  ENC_SLEEP_MODE        /* Disabled                             */
} enc_mode_type;


/* Radio Configuration
*/

typedef enum
{
  ENC_FCH_RC_1 = 1, /* Defined to be same as CAI_RC_?.  Don't change */
  ENC_FCH_RC_2 = 2,
  ENC_FCH_RC_3 = 3,
  ENC_FCH_RC_4 = 4,
  ENC_FCH_RC_8 = 8,  /* New RC for 1x Advanced */
  ENC_NUM_FCH_RCS = 5
} enc_fch_radio_config_type;

typedef enum {
  ENC_STATUS_INTERRUPT_SLOT_0  = 0x00,  /* 5 ms slot bin 0 */
  ENC_STATUS_INTERRUPT_SLOT_1  = 0x01,  /* 5 ms slot bin 1 */
  ENC_STATUS_INTERRUPT_SLOT_2  = 0x02,  /* 5 ms slot bin 2 */
  ENC_STATUS_INTERRUPT_SLOT_3  = 0x03  /* 5 ms slot bin 3 */
} enc_status_interrupt_slot_type;

/* Size (in bytes) of full rate packet packet at 9600 and 14400 bps
*/
#define ENC_9600_FULL_RATE_SIZE   24
#define ENC_14400_FULL_RATE_SIZE  36

/*
    There are 3072 bits in 20 ms frame for data rate = 153.6K;
   6144 bits for rate = 307.2k as specified in the standard.
   3072/8 = 384  6144/8=768.
*/
#define ENC_SCH_RC3_16X_SIZE  384
#define ENC_SCH_RC3_32X_SIZE  768
#define ENC_SCH_RC4_16X_SIZE  576


/*===========================================================================

                      PUBLIC FUNCTION DECLARATIONS

===========================================================================*/

/*===========================================================================

FUNCTION ENC_SET_MODE

DESCRIPTION
  Sets the operating mode of the driver: sleep, cdma, sync, paging, traffic.

DEPENDENCIES
  CDMA mode  - must not be entered before 20ms after enc_init().
  Sleep mode - must not be entered before 20ms after enc_tx_enable(FALSE).

RETURN VALUE
  None

SIDE EFFECTS
  Changes the operating mode of the circuit.
  Sleep mode - masks the transmit path, turns off the PA, and disables
               the CHIPX8 clocks to the circuit.  PN state is lost.
  CDMA mode  - masks the transmit path, turns ON the PA, and enables the
               CHIPX8 clocks to the circuit.  If previously in sleep mode,
               the circuit will not re-synchronize until the next SYNC80M.
  SC mode    - initialize the circuit for the Sync Channel
  PC mode    - initialize the circuit for the Paging Channel
  TC mode    - initialize the circuit for the Traffic Channel

===========================================================================*/
void enc_set_mode
(
  enc_mode_type channel_mode    /* Mode in which the circuit is to run */
);


/*===========================================================================

FUNCTION ENC_TX_ENABLE

DESCRIPTION
  Enable/disable the transmit path by masking and unmasking it.
  The TX_PUNCT change takes effect on the next transmit frame boundary.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void enc_tx_enable
(
  boolean enable_tx     /* Enable/disable (mask/unmask) the transmitter */
);


/*===========================================================================

FUNCTION ENC_USER_PN_STATE

DESCRIPTION
  Set the User PN long code state.  The User PN state takes effect at the next
  80 ms Transmit Frame Boundary (which proceeds SYNC80M by the count in the
  Back-to-the-Future counter).  Loading 0's into the user PN state inhibits
  spreading from the User PN Generator.  This operation is non-atomic.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void enc_user_pn_state
(
  qword user_pn_state   /* User PN state to load into User PN Generator */
);


/*===========================================================================

FUNCTION ENC_USER_PN_MASK

DESCRIPTION
  Set the User PN long code mask.  The User PN mask takes effect at the next
  20 ms Transmit Frame Boundary.  Loading 0's into the user PN mask inhibits
  spreading from the User PN Generator.  This operation is non-atomic.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void enc_user_pn_mask
(
  qword user_pn_mask    /* User PN mask to load into User PN Generator */
);


/*===========================================================================

FUNCTION ENC_IQ_PN_STATE

DESCRIPTION
  Set the I & Q PN states into the I & Q PN Generator.  Load 0's to inhibit
  spreading from the I & Q PN Generator.  This takes effect at the next
  80 ms Transmit Frame Boundary (which proceeds SYNC80M by the count in the
  Back-to-the-Future counter).  This operation is non-atomic.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void enc_iq_pn_state
(
  word i_pn_state,      /* I PN state */
  word q_pn_state       /* Q PN state */
);


/*===========================================================================

FUNCTION ENC_RATE_SET

DESCRIPTION
  Set the rate set to use for encoding data on the Traffic Channel.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void enc_rate_set
(
  enc_rate_set_type rate_set    /* Rate set to use for encoding data */
);


/*===========================================================================

FUNCTION ENC_FCH_DATA_OUT

DESCRIPTION
  Send data to the encoder, generate a CRC (or not), and encode it at
  a specified rate.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void enc_fch_data_out
(
  byte *data_ptr,               /* Buffer to send to Encoder                 */
  boolean generate_crc,         /* Have the Encoder generate a CRC, or not   */
  enc_rate_type rate        /* Rate to encode the data as                */
);



/*===========================================================================

FUNCTION ENC_FCH_ENABLE

DESCRIPTION
  Turns the FCH_ENABLE bit of FCH_CTL register on/off

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
void enc_fch_enable
(
  boolean enable_fch
);

/*===========================================================================

FUNCTION enc_tx_1x_test_waveform

DESCRIPTION
  This function starts/stops a 1x Tx test waveform

DEPENDENCIES
  RF and FW should already be enabled and in Tx-capable mode

RETURN VALUE
  None

SIDE EFFECTS
 None.

===========================================================================*/
void enc_tx_1x_test_waveform
(
  boolean enable
);

/*===========================================================================

FUNCTION ENC_FTM_TX_1X_INIT

DESCRIPTION
  This function initializes the necessary 1x structures for FTM testing

DEPENDENCIES
  RF and FW should be enabled and pointer to FW smem should not be NULL.

RETURN VALUE
  None

SIDE EFFECTS
 None.

===========================================================================*/
void enc_ftm_tx_1x_init
(
  cdmafw_shared_memory_interface_t* smem_addr  /* FW Shared memory address */
);

/*===========================================================================

FUNCTION ENC_FTM_TX_1X_DEINIT

DESCRIPTION
  This function cleans up any 1x structures when FTM testing is complete

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
 None.

===========================================================================*/
void enc_ftm_tx_1x_deinit( void );

#endif  /* ENC_V_H */

