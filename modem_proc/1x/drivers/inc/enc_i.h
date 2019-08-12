#ifndef ENC_I_H
#define ENC_I_H
/*===========================================================================

      E N C O D E R   ( I N T E R L E A V E R )    H E A D E R    F I L E

DESCRIPTION
  All the declarations and definitions necessary to support interaction
  with the QUALCOMM CDMA Interleaver / Encoder chip set

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

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/1x/drivers/inc/enc_i.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
04/14/16   srk     Optimized TX Trigger fixes and code cleanup.
10/27/14   srk     Remove the obsolete enc_set_btf function.
02/13/14   dkb     Add support for optimized Tx trigger.
11/14/13   dkb     Change ovsf value for 2X SCH to correct value.
11/14/13   dkb     Clean up enum issues and correct erroneous comments.
10/07/13   dkb     Update 1x Tx waveform for new FW interface.
09/30/13   dkb     Initial RL changes for new FW interface.
04/23/13   trc     Add return code to enc_is95c_enable()
03/11/13   trc     API cleanup for Triton/Dime
02/15/13   srk     Replace mcs hwio with m1x hwio
12/11/12   trc     Layering/Cleanup for Triton
06/15/12   trc     Add immediate TxDAC triggering capability
02/23/12   srk     Mainlined FEATURE_IS2000_SCH
02/22/12   jtm     Change Tx clock voting to use additional states.
11/30/11   srk     Feature clean up: Mainlined and Internalized Features
09/02/11   sk      Rebased the code to the NikeL modem package by mainlining
                   FEATURE_MODEM_1X_NIKEL.
05/09/11   trc     Add NikeL TxDAC trigger API
04/28/11   trc     Removed enc_hw_reset
12/08/10   jtm     Removed enc_set_tx_polarity and enc_set_rx_polarity.
11/01/10   jtm     Private file clean up.
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
#include "1x_variation.h"
#include "comdef.h"     /* Definitions for byte, word, etc.     */
#include "customer.h"
#include "modem_1x_defs.h"

//drivers
#include "enci.h"
#include "enci_i.h"             /* Encoder register & macro definitions */
#include "enc.h"
#include "enc_v.h"
#include "m1x_hwio_mpss.h"

//other
#include "qw.h"         /* definition of qword                  */
#include "rex.h"        /* definition of REX data types         */
#include "misc.h"

/*===========================================================================

                        DATA DECLARATIONS

===========================================================================*/
/* Encoder channel enum type */
typedef enum
{
  ENC_CHAN_1 = 0,
  ENC_CHAN_2 = 1,
  ENC_CHAN_3 = 2,
  ENC_TOTAL_CHAN
} enc_chan_number_type;

/* Encoder Code rates
*/
typedef enum
{
  ENC_CODE_1_BY_2_RATE = 0x00,
  ENC_CODE_1_BY_3_RATE = 0x01,
  ENC_CODE_1_BY_4_RATE = 0x02
} enc_code_rate_type;

/* TX_EARLY_FRAME_ADV periods
*/
typedef enum
{
  ENC_TX_EARLY_FRAME_DISABLE = 0x00,
  ENC_TX_EARLY_FRAME_5_MS    = 0x01,
  ENC_TX_EARLY_FRAME_10_MS   = 0x02,
  ENC_TX_EARLY_FRAME_20_MS   = 0x03
} enc_tx_early_frame_period_type;

/* FCH mode
*/
typedef enum {
  ENC_IS95_A_MODE = 0x0000,
  ENC_IS95_C_MODE = 0x0001
} enc_is95_mode_type;

typedef enum
{
  ENC_NO_GATING_PATTERN = 0xFFFF,
  ENC_HALF_RATE_GATING_PATTERN = 0xAAAA,
  ENC_QTR_RATE_GATING_PATTERN = 0x8888,
  ENC_FCH_8TH_RATE_GATING_PATTERN = 0xCCCC // Pilot duty cycle is 50%
} enc_gating_pattern_type;

/* Masks for looking at circuit status returned by enc_status()
*/
typedef enum {
  ENC_STATUS_WRITE_ERROR_M = 0x0055,    /* Encoder write error */
  ENC_STATUS_ENCODER_BUSY_M = 0x002A,    /* Encoder busy */
  ENC_STATUS_POSITION_ERROR_M = 0x0040,  /* Start encoding at the same time */
  ENC_STATUS_TX_FR_INT_SLOT_M = 0x0180,  /* 5 ms frame slot */
  ENC_STATUS_TX_FR_INT_SLOT_POS = 0x07  /* interrupt slot bit position */
} enc_status_mask_type;

/* Definitions of TX clock states */
typedef enum {
  ENC_TX_CLOCKS_ENABLE,
  ENC_TX_CLOCKS_DISABLE
} enc_tx_clock_states_type;

typedef enum {
  MULTI_BPSK_ON_I = 31,         /* CH1 */
  MULTI_BPSK_ON_Q = 32,         /* CH2 IS95c, CH3 */
  MULTI_BPSK_ON_IQ = 33         /* CH2 IS95a */
} enc_modulation_type;

typedef enum {
  ENC_OVSF_0 = 0,                   /* IS95a FCH, IS95a ACH */
  ENC_OVSF_1 = 1,                   /* CH1, CH3_SF2_SCH, CH3_SF4_SCH */
  ENC_OVSF_2 = 2,                   /* CH2_IS95C_FCH, CH3_SF8_SCH, CH3_SF8_EACH */
  ENC_OVSF_4 = 4                    /* CH3_SF16_SCH */
} enc_ovsf_type;

/* Spreading factor values for channel configs */
typedef enum {
  ENC_SF_8 = 1,
  ENC_SF_16,
  ENC_SF_32,
  ENC_SF_64,
  ENC_SF_128,
  ENC_SF_256,
  ENC_SF_RESERVED,
  ENC_SF_1,
  ENC_SF_2,
  ENC_SF_4
} enc_sprd_factor_type;

/* Structure for short code pn state values */
typedef struct
{
  uint16 spn_i;
  uint16 spn_q;
} enc_spn_state_type;

/*===========================================================================

                      PUBLIC FUNCTION DECLARATIONS

===========================================================================*/

/*===========================================================================

FUNCTION ENC_SET_FRAME_OFF

DESCRIPTION
  Specify Frame stagger offset for forward/reverse traffic frames
  This is the offset from the system frame in power control groups.
  This offset has no effect while in Sync Channel mode.  The value
  is reset to 0 offset by enc_init().

  Takes effect at the next SYNC80M.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void enc_set_frame_off
(
  byte fwd_off,         /* Forward channel frame stagger */
  byte rev_off          /* Reverse channel frame stagger */
);


/*===========================================================================

FUNCTION ENC_BOZO_MODE

DESCRIPTION
  Set the "bozo mode" bit, so that the User PN long code state is reloaded
  on each 80 ms Transmit Frame boundary.  Useful when debugging time sync
  problems between the Cel and the mobile.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void enc_bozo_mode
(
  boolean enable_bozo_mode      /* Whether to enable/disable "bozo" mode */
);


/*===========================================================================

FUNCTION ENC_FCH_RC

DESCRIPTION
  Set the reverse fundamental channel radio configuration

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
extern void enc_fch_rc
(
   enc_fch_radio_config_type fch_rc
);



/*===========================================================================

FUNCTION ENC_DCCH_RC

DESCRIPTION
  Set the reverse DCCH radio configuration

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
extern void enc_dcch_rc
(
   enc_fch_radio_config_type dcch_rc
);


/*===========================================================================

FUNCTION ENC_STATUS_CHECK

DESCRIPTION
  Read the status of the encoder circuit, and tries to correct any faults.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void enc_status_check( void );



/*===========================================================================

FUNCTION ENC_SCH_RC

DESCRIPTION
  Set the reverse supplemental channel radio configuration

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
extern void enc_sch_rc
(
   enc_sch_radio_config_type sch_rc
);

/*===========================================================================

FUNCTION      ENC_CHANGE_TX_CLK_RESOURCES

DESCRIPTION
              This function is used to enable/disable the clock resources
              required by 1X for TX via the m1xrm driver. This
              function requests the necessary clock resources by voting
              for a particular clock state. Conversely, this function
              can be used to vote off the resources.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  If state = ENC_TX_CLOCKS_ENABLE:  TX clock resources will be voted on.
  If state = ENC_TX_CLOCKS_DISABLE:  TX will vote for INACTIVE clocks.
===========================================================================*/
void enc_change_tx_clk_resources
(
  enc_tx_clock_states_type state
);


/*===========================================================================

FUNCTION ENC_PN_RAND_DELAY

DESCRIPTION
  Delay the transmit time by a specified number of chips.  This value is
  loaded internally by the Interleaver at the next SYNC80M, and takes effect
  at the successive internal 80 millisecond boundary (the BTF roll-over).

  For CDMA transmit PN randomization, call this routine with the number of
  chips of delay desired.  Call it again with 0 to return to non-delayed
  transmit frame timing.

DEPENDENCIES
  None

RETURN VALUE
  Number of chips from the previous call to enc_pn_rand_delay.

SIDE EFFECTS
  None

===========================================================================*/
extern word enc_pn_rand_delay
(
  word chips        /* Chip delay for transmit frame timing */
);


/*===========================================================================

FUNCTION ENC_SCH_FTM_DATA_OUT

DESCRIPTION
  Send data to the encoder, generate a CRC (or not), and encode it at
  a specified rate.  For FTM only.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void enc_sch_ftm_data_out
(
  byte *data_ptr,                /* Buffer to send to Encoder               */
  boolean generate_crc,          /* Have the Encoder generate a CRC, or not */
  enc_sch_rate_type rate,        /* Rate to encode the data as              */
  boolean turbo_code,            /* TRUE= Turbo, FALSE=Convolutional        */
  word ltu_size                  /* LTU block size with CRC in bits         */
);



#ifdef FEATURE_IS2000_REL_A_CC
//enc_reach_on
/*===========================================================================

FUNCTION ENC_REACH_DATA_OUT

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
void enc_reach_data_out
(
  byte *data_ptr,                /* Buffer to send to Encoder               */
  boolean generate_crc,          /* Have the Encoder generate a CRC, or not */
  enc_sch_rate_type rate,        /* Rate to encode the data as              */
  boolean turbo_code,            /* TRUE= Turbo, FALSE=Convolutional        */
  word ltu_size,                 /* LTU block size with CRC in bits         */
  word frame_size                /* Frame size                              */
);
//enc_reach_on
#endif


/*===========================================================================

FUNCTION ENC_SCH_LTU_SIZE

DESCRIPTION
  Set supplemental channel LTU size. 32 <= ltu_size < 512 bits

DEPENDENCIES
  None

RETURN VALUE
   None

SIDE EFFECTS
  None

===========================================================================*/
extern void enc_sch_ltu_size
(
  word sch_ltu_size        /* 32 <= ltu_size < 512 bits */
);


/*===========================================================================

FUNCTION ENC_IS95C_ENABLE

DESCRIPTION
  Turns the IS95C bit of FCH_CTL register on/off

DEPENDENCIES

RETURN VALUE
   0  ==  success
  -1  ==  failure to switch reliably

SIDE EFFECTS

===========================================================================*/
extern int enc_is95c_enable
(
  boolean is95c
);


/*===========================================================================

FUNCTION ENC_SCH_ENABLE

DESCRIPTION
  Turns the SCH_ENABLE bit of SCH_CTL register on/off

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
extern void enc_sch_enable
(
   boolean enable_sch
);


/*===========================================================================

FUNCTION ENC_PCH_ENABLE

DESCRIPTION
   Turns the PCH_ENABLE bit of MISC_CTL register on/off

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
extern void enc_pch_enable
(
   boolean enable_pch
);

/*===========================================================================

FUNCTION ENC_PCBIT_TEST_MASK

DESCRIPTION
   Sets the mask in MOD_PCBIT_TEST_MASK register. When a bit in this register
   is set to 1 the corresponding bit of MOD_PCBIT_TEST_DATA register is sent
   as power control bit on the reverse pilot channel.

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
extern void enc_pcbit_test_mask
(
  word pcbit_test_mask
);


/*===========================================================================

FUNCTION ENC_PCBIT_TEST_DATA

DESCRIPTION
   Sets the contents of MOD_PCBIT_TEST_DATA

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
extern void enc_pcbit_test_data
(
   word pcbit_test_data
);


/*===========================================================================

FUNCTION ENC_SCH_MUXPDU_DATA_OUT

DESCRIPTION
  Send data to the encoder for MUXPDUs 5 and 3

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void enc_sch_muxpdu_data_out
(
  byte *data_ptr,                /* Buffer to send to Encoder    */
  word total_numberofbyte        /* total number of sending byte */
);

/*===========================================================================

FUNCTION ENC_SCH_DATA_OUT_CFG

DESCRIPTION
   Program the MOD_SCH_CTL register

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void enc_sch_data_out_cfg
(
  enc_sch_rate_type rate,        /* Rate to encode the data as              */
  boolean turbo_code,            /* TRUE= Turbo, FALSE=Convolutional        */
  word ltu_size,                 /* LTU block size with CRC in bits         */
  boolean generate_crc           /* Have the Encoder generate a CRC, or not */
);


/*===========================================================================

FUNCTION ENC_IS95C_TX_PATN

DESCRIPTION
   Program the IS95C_TX_PATN register

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void enc_is95c_tx_patn( enc_gating_pattern_type pattern );

/*===========================================================================

FUNCTION ENC_GATING_MODE

DESCRIPTION
   Program the MOD_MISC_CTL register with one of 3 mutually exclusive pilot
   gating modes: PILOT_GATING_MODE, FCH_8TH_RATE_GATING_MODE or
   HHO_PREAMBLE_MODE, NO_GATING_MODE.

DEPENDENCIES
   The gating pattern must be specified using enc_is95c_tx_patn();

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void enc_gating_mode( enc_gating_mode_type gating_mode );

/*===========================================================================

FUNCTION ENC_DCCH_ENABLE

DESCRIPTION
  Turns the DCCH_ENABLE bit of MOD_MISC_CTL register on/off

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void enc_dcch_enable( boolean dcch_en );

/*===========================================================================

FUNCTION ENC_DCCH_DATA_OUT

DESCRIPTION
  Send DCCH data to the encoder, generate a CRC (or not), and encode it at
  a specified rate.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void enc_dcch_data_out
(
  byte *data_ptr,               /* Buffer to send to Encoder               */
  boolean generate_crc,         /* Have the Encoder generate a CRC, or not */
  enc_rate_type rate            /* Rate to encode the data as              */
);

/*===========================================================================

FUNCTION enc_write_sch_data_done

DESCRIPTION
  This function is used to indicate the HW writing to ENCODER is done.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
 None.

===========================================================================*/
void enc_sch_write_data_done( void );

/*===========================================================================

FUNCTION enc_sync_tx_timing

DESCRIPTION
  This function prepares the Tx prefill and TxDAC timing setup.  It can
  either trigger immediately (abort case), or at the Sync20/Sync80 (normal)

DEPENDENCIES
  Long code slam values should have already been loaded into HW.
  This needs to happen within 13ms of next SYNC80 boundary

RETURN VALUE
  None

SIDE EFFECTS
 Disables any active encoder channels!!  Caller must re-enable if needed,
 but re-enable must happen AFTER the trigger point.

===========================================================================*/
void enc_sync_tx_timing
(
  boolean trigger_now           /* Trigger now, or on next Sync20/Sync80 */
);
#endif  /* ENC_I_H */
