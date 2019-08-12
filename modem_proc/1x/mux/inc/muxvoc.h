
#ifndef MUXVOC_H
#define MUXVOC_H

/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

       M U X  V O C O D E R  I N T E R F A C E  H E A D E R  F I L E


GENERAL DESCRIPTION
  Contains logic surrounding Layer1 Mux's interaction with the vocoder.

EXPORTED FUNCTIONS:
  muxvoc_clear_inhibit_mvs_enable_flag
  muxvoc_config_vocoder
  muxvoc_print_voc_frame_info
  muxvoc_reset_rx_frame_process_count
  muxvoc_reset_tx_frame_process_count
  muxvoc_reset_tx_voice_to_silence
  muxvoc_rx_tc_init
  muxvoc_rxc_pri_voice
  muxvoc_set_voc_activity
  muxvoc_tx_tc_init
  muxvoc_tx_voc_traffic
  rxc_reset_voc_init
  muxvoc_disable_rx
  muxvoc_disable_tx
  muxvoc_mvs_standby
 - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

  Copyright (c) 2011 - 2014 Qualcomm Technologies, Inc. 
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

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*==========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/1x/mux/inc/muxvoc.h#1 $ $DateTime: 2016/12/13 08:00:08 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
08/21/14   eye     Added MVS_STANDBY during HHO with frame_offset change.
04/22/11   jtm     CMI Phase 4 - Final SU API changes.
01/25/11   jtm     Initial implementation.

==========================================================================*/

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/

/* Common */
#include "comdef.h"
#include "customer.h"
#include "modem_1x_defs.h"

//cp
#include "cai.h"

//mux
#include "txcmc_i.h"

//drivers
#include "enc_v.h"

/*===========================================================================

                   EXTERNAL DEFINITIONS AND TYPES

===========================================================================*/

/*===========================================================================

                    EXTERNAL FUNCTION PROTOTYPES

===========================================================================*/
/*===========================================================================

FUNCTION MUXVOC_DISABLE_RX

DESCRIPTION
  This routine disables the Rx voice processing once the primary voice SO has
  been released.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

void muxvoc_disable_rx(void);

/*===========================================================================

FUNCTION MUXVOC_DISABLE_TX

DESCRIPTION
  This routine disables the tx voice processing once the primary voice SO has
  been released.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

void muxvoc_disable_tx(void);

/*===========================================================================

FUNCTION MUXVOC_RX_TC_INIT

DESCRIPTION
  This routine initializes the muxvocoder module using the TCI command
  provided by MC.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void muxvoc_rx_tc_init ( boolean inhibit_voc_enable );

/*===========================================================================

FUNCTION MUXVOC_CLEAR_INHIBIT_VOC_ENABLE_FLAG

DESCRIPTION
  This routine sets the inhibit mvs enable flag to FALSE which will then allow
  subsequent calls to mvs_enable() to be performed.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void muxvoc_clear_inhibit_voc_enable_flag ( void );

/*===========================================================================

FUNCTION RXC_RESET_VOC_INIT

DESCRIPTION
  ****This function is deprecated***

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void rxc_reset_voc_init ( void );

/*===========================================================================

FUNCTION RXC_VOC_CONFIG

DESCRIPTION
  If the vocoder has not been configured, then configure it to work
  with the current voice service option.  Note that the service option
  may be changed in a middle of a call as well.

DEPENDENCIES
  Called if voc config is not done and SO is voice.

RETURN VALUE
  None.

SIDE EFFECTS
  Queue request to Vocoder driver.
  Service option CAI_SO_VOICE_IS96A is not supported by MVS.
  Service option SMV is not supported by MVS.
  For any service option not supported (by lack of a defined feature or
  otherwise) the vocoder api will not be called.

===========================================================================*/
boolean muxvoc_config_vocoder (uint16 srv_opt);

/*===========================================================================
FUNCTION      RXC_PRI_VOICE

DESCRIPTION   Procedure does the procssing for voice SO's.

DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
void muxvoc_rxc_pri_voice
(
  byte bits[],                  /* Address of first byte of primary data   */
  word bit_pos,                 /* Position of first bit of data in buffer */
  word bit_len,                 /* Number of bits of data in buffer        */
  cai_data_rate_type frame_rate /* Rate of the current voice frame         */
);

/*===========================================================================
FUNCTION      MUXVOC_TX_TC_INIT

DESCRIPTION   Initialize voice frames to silence.

              Note that txc_voc_eighth_rate_null_frame is an eighth rate frame,
              therefore it is smaller than the voice buffer.

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  TX voice buffers will be set to 1/8th rate null.

===========================================================================*/
void muxvoc_tx_tc_init( void );

/*===========================================================================
FUNCTION      MUXVOC_RESET_TX_VOICE_TO_SILENCE

DESCRIPTION   Reset voice frames to silence.

              Note that txc_voc_eighth_rate_null_frame is an eighth rate frame,
              therefore it is smaller than the voice buffer.

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  The current voice frame buffer will be set to 1/8th rate null.

===========================================================================*/
void muxvoc_reset_tx_voice_to_silence( void );

/*===========================================================================
FUNCTION      MUXVOC_PRINT_VOC_FRAME_INFO

DESCRIPTION   Print debug information about the transmitted vocoder frame
              if debuging is enabled.

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None

===========================================================================*/
void muxvoc_print_voc_frame_info
(
  enc_rate_type rate,
  byte *data
);

/*===========================================================================

FUNCTION MUXVOC_TX_VOC_TRAFFIC

DESCRIPTION
  Get traffic data (primary or secondary) from the vocoder.

DEPENDENCIES
  The vocoder should be initialized already.

RETURN VALUE
  Returns the address of the vocoder data in a pointer passed to the
  procedure.

  Returns the rate of the vocoder data

SIDE EFFECTS
  None

===========================================================================*/
cai_data_rate_type muxvoc_tx_voc_traffic
(
  byte **data_ptr,               /* Returns pointer to data        */
  txc_sig_frame_type signext     /* Signaling mode for next packet */
);

/*===========================================================================

FUNCTION muxvoc_enable_tx_sync_check

DESCRIPTION
    Enable the logic used to determine if TX and MVS voice frame processing
    is in sync. This is meant to catch cases where the MVS callback occurs
    too late or too early, resulting in degraded speech.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void muxvoc_enable_tx_sync_check(void);

/*===========================================================================

FUNCTION muxvoc_enable_rx_sync_check

DESCRIPTION
    Enable the logic used to determine if RX and MVS voice frame processing
    is in sync. This is meant to catch cases where the MVS callback occurs
    too late or too early, resulting in degraded speech.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void muxvoc_enable_rx_sync_check(void);

/*===========================================================================

FUNCTION muxvoc_set_voc_activity

DESCRIPTION
  Enable or disable the voice activity test.

DEPENDENCIES
  Only applicable to JCDMA.

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

void muxvoc_set_voc_activity
(
  boolean enable,   /* enable/disable voice activity test */
  word voc_1,       /* # of full rate */
  word voc_2,       /* # of 1/2 rate */
  word voc_8        /* # of 1/8 rate */
);

/*===========================================================================

FUNCTION muxvoc_mvs_standby

DESCRIPTION
  Calls MVS_STANDBY with MVS mode.

DEPENDENCIES
  Only for voice SO's.

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

void muxvoc_mvs_standby( void );

#ifdef FEATURE_VOICE_AGENT_ARCHITECTURE
/*===========================================================================

FUNCTION MUXVOC_GET_VOC_FRAME

DESCRIPTION
  This function will be called when txc wants to get the vocoder frame from
  vocoder and send it the decoder.

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void muxvoc_get_voc_frame( void );

#endif

#endif /* MUXVOC_H */
