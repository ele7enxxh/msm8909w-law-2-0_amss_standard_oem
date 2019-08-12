/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

             L O O P B A C K    S E R V I C E S    F I L E

GENERAL DESCRIPTION
  This module contains functions and variables to support Mobile Station
  Loopback Service Option processing as specified in PN-3291.

EXTERNALIZED FUNCTIONS
  loopback_rx
    Buffers loopback data received on the forward link.

  loopback_tx
    Formats the transmit data according to the receive data rate and
    the rate limit imposed by the MUX1 layer.

  loopback_init
    Sets the loopback receive data buffer to a known state.

INITIALIZATION AND SEQUENCING REQUIREMENTS
  Prior to using the other Loopback service option services, function
  loopback_init should be called to initialize the loopback rx data buffer.

  Copyright (c) 1994 - 2013 Qualcomm Technologies, Inc. 
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

/*===========================================================================

                        EDIT HISTORY FOR MODULE

$PVCSPath: L:/src/asw/MSM5100/CP_REL_A/vcs/loopback.c_v   1.1   03 Oct 2002 10:51:06   phosabet  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/1x/cp/src/loopback.c#1 $ $DateTime: 2016/12/13 08:00:08 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
01/21/14   eye     Increase loopback FIFO depth to account various TX delays.
09/02/11   jtm     Correct loopback FIFO underrun.
07/07/11   trc     Added loopback FIFO
04/22/11   jtm     CMI Phase 4 - Final SU API changes.
02/14/11   ag      Used REX_ISR_LOCK and REX_ISR_UNLOCK for protecting shared
                   data from simultaneous access by multiple tasks/ISRs.
01/27/11   azh     Lint and KW fixes.
11/15/10   jtm     Initial SU API split for CP.
10/08/10   ag      Renamed Markov, Loopback related counters and functions.
07/15/10   ag      Lint fixes.
07/12/10   jj      Replaced CP's QDSP6 feature with an umbrella feature.
06/15/10   ag      Added support for enhanced loopback service option.
08/02/09   jtm     Lint fixes.
05/14/09   ag      CMI phase 2 changes
03/03/08   jj      Featurized Q6 changes.
12/10/08   jj      QDSP6 migration effort.
10/03/02   ph      Lint Cleanup
08/14/95   gb      Added new Loopback_13K service option support.
03/16/94   jca     Created module.

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "1x_variation.h"
#include "comdef.h"
#include "modem_1x_defs.h"
#include "bit.h"
#include "err.h"
#include "msg.h"
#include "m1x_diag.h"

#include "loopback.h"
#include "cai_v.h"
#include "mc_i.h"

#ifdef FEATURE_IS2000_1X_ADV
#include "txcmc_i.h"
#include "rxcmc_i.h"
#endif /* FEATURE_IS2000_1X_ADV */

/*===========================================================================

            LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.

===========================================================================*/

/* -----------------------------------------------------------------------
** Define structure used for buffering Loopback rx packet data
** ----------------------------------------------------------------------- */
typedef struct
{
  word frame_category;
    /* Received packet frame category (see IS-95 6.2.2.2) */
  byte data_bits[36];
    /* Received Loopback Packet data bits (max size of rate-set 2 frame) */
} loopback_data_type;

#define LOOPBACK_FIFO_DEPTH 3

/* The loopback structure is implemented as a 3-deep FIFO to ensure 2
   frame delay between receipt and transmittal of a loopback frame */
typedef struct
{
  uint8              depth;                     /* # of items in FIFO */
  uint8              top_idx;                   /* latest written entry */
  boolean            wait_for_rx;               /* need to wait for Rx frame */
  loopback_data_type buf[LOOPBACK_FIFO_DEPTH];  /* FIFO storage */
} loopback_data_buffer_type;

static loopback_data_buffer_type loopback_data;

static int    lb_full_rate_bits;
static int    lb_half_rate_bits;
static int    lb_quarter_rate_bits;
static int    lb_eighth_rate_bits;

#ifdef FEATURE_IS2000_1X_ADV
static loopback_test_type lb_test_mode;
static lso_rx_ackch_type lso_rx_ackch;
#endif /* FEATURE_IS2000_1X_ADV */

/* <EJECT> */
/*===========================================================================

FUNCTION LOOPBACK_RX

DESCRIPTION
  Buffers Loopback Service Option data received on the forward link
  until the Transmit Task can loop the data back to the base station
  on the reverse link.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void loopback_rx
(
  byte *bit_ptr,
  word bit_position,
  word bit_len,
  word frame_category
)
{
  boolean overrun = FALSE;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  REX_ISR_LOCK(&mc_crit_sect);

  loopback_data.wait_for_rx = FALSE;

  /* If FIFO is less than full, depth increases */
  if ( loopback_data.depth < LOOPBACK_FIFO_DEPTH )
  {
    ++loopback_data.depth;
  }
  else
  {
    overrun = TRUE;
  }

  /* Top of FIFO is always incremented, purging stale element upon overrun */
  loopback_data.top_idx = (loopback_data.top_idx + 1) % LOOPBACK_FIFO_DEPTH;

  /* Copy the mux frame category and the data bits into the new top position */
  loopback_data.buf[loopback_data.top_idx].frame_category = frame_category;
  b_copy( (void *)bit_ptr, bit_position,
          (void *)loopback_data.buf[loopback_data.top_idx].data_bits, 0,
          bit_len );

  REX_ISR_UNLOCK(&mc_crit_sect);

  /* If there was a FIFO overrun, let the user know */
  if ( overrun )
  {
    M1X_MSG( DCP, LEGACY_ERROR,
      "Rx Loopback FIFO overrun, discarding oldest frame");
  }

} /* end loopback_rx() */

/*===========================================================================

FUNCTION LOOPBACK_TX

DESCRIPTION
  Formats the transmit data according to the receive data rate and
  the rate limit imposed by the MUX1 layer.

DEPENDENCIES
  None

RETURN VALUE
  The transmit data rate to use is returned.

SIDE EFFECTS
  None

===========================================================================*/

cai_data_rate_type loopback_tx
(
  cai_data_rate_type max_rate,
    /* Max rate for this frame */
  cai_frame_type    *frame_data
    /* Pointer to loopback data bits */
)
{
  loopback_data_type *rx_data_item;
    /* Buffer for Loopback service option rx packet data */
  cai_data_rate_type tx_data_rate;
    /* To return the transmit data rate to use */
  boolean            underrun = FALSE;
    /* Indicates when the FIFO is dry (bad) */
  boolean            waiting = FALSE;
    /* Indicates when tx is waiting for rx to start */
  uint8 current_depth;
    /* current depth of the FIFO */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Initialize tx_data_rate to avoid compiler warning */
  tx_data_rate = CAI_FULL_RATE;

  REX_ISR_LOCK(&mc_crit_sect);

  /* If RX hasn't processed a frame yet we'll want to wait so we can preserve
     our predefined frame buffering to prevent underruns.*/
  if( !loopback_data.wait_for_rx )
  {
  /* If FIFO isn't empty, grab oldest unread item */
  if ( loopback_data.depth > 0 )
  {
    /* Decrement the depth */
    --loopback_data.depth;
  }
  else
  {
    underrun = TRUE;
  }

    current_depth = loopback_data.depth;
  }
  else
  {
    /* Setting current depth = 0 here so we can continue to pull from
       the bottom of the FIFO, which contains our prefilled frame from
       loopback_init. In loopback_init the depth was set to 1 to account
       for that prefilled frame.
       We'll keep doing this until RX adds to the FIFO. */
    waiting = TRUE;
    current_depth = 0;
  }

  /* Oldest item is (top + size - new depth) % size */
  rx_data_item =
    &loopback_data.buf[ (loopback_data.top_idx +
                         LOOPBACK_FIFO_DEPTH -
                         current_depth) % LOOPBACK_FIFO_DEPTH ];

  REX_ISR_UNLOCK(&mc_crit_sect);

  if( waiting )
  {
    /* Indicate that TX is waiting for RX to recieve its first frame.
       Just transmit the existing pre-filled frame in the buffer. */
    M1X_MSG( DCP, LEGACY_MED,
      "Tx loopback waiting for Rx to start");
  }
  else if ( underrun )
  {
    /* Indicate the underrun condition here, outside locked context, and
       treat it as if it were an erasure frame */
    M1X_MSG( DCP, LEGACY_ERROR,
      "Tx loopback underrun, treating as erasure frame");
    rx_data_item->frame_category = CAI_MUX1_FOR_10_ID;
  }

#ifdef FEATURE_IS2000_1X_ADV
  /* Processing for R-FCH 0bps testing mode */
  if (lb_test_mode == LB_RFCH_BLANKING_TEST)
  {
    if ((max_rate == CAI_FULL_RATE) || (max_rate == CAI_HALF_RATE))
    {
      /* ------------------------------------------------------------------
       ** If Service Option 75 has not been been commanded to generate a
       ** Blank packet, it shall set data bits set to all 1's and CAT_TYPE
       ** '11'. The resulting information bits shall be supplied to the
       ** multiplex sublayer as a Rate 1/8 Non-critial packet.
       ** ------------------------------------------------------------------ */

      /* Ensure that first two bits are set to '11' */
      *frame_data[0] = 0xC0;

      /* Rate 1/8 non critical data block has 16 bits(rate set 1).
       * So set 14 data bits to all 1's */
      *frame_data[0] |= 0x3F;
      *frame_data[1] = 0xFF;

      tx_data_rate = CAI_NON_CRITICAL_RATE;
    }

    else if (max_rate == CAI_BLANK_RATE)
    {
      /* ------------------------------------------------------------------
       ** If Service Option 75 has been commanded to generate a Blank packet,
       ** it shall do so, thus providing zero information bits to the
       ** multiplex sublayer.
       ** ------------------------------------------------------------------ */
      tx_data_rate = CAI_BLANK_RATE;
    }

    else
    {
      ERR_FATAL( "Invalid Rate, %d", (int) max_rate, 0, 0 );
    }
    return (tx_data_rate);
  }
#endif /* FEATURE_IS2000_1X_ADV */

  /* If test mode is normal. This case will reach only if test mode
   * is not R-FCH blanking. */
  if (max_rate == CAI_FULL_RATE)
  {
    /* ----------------------------------------------------------
    ** Service Option 2 has not been commanded to adapt its rates
    ** ---------------------------------------------------------- */

    switch (rx_data_item->frame_category)
    {
      case CAI_MUX2_FOR_1_ID:
      case CAI_MUX1_FOR_1_ID:
        /* ---------------------------------------------------------------
        ** If the received packet is Rate 1, then Service Option 2 shall
        ** set the CAT_TYPE field equal to '00' and the data bits to
        ** the received data bits.  The resulting information bits
        ** shall be supplied to the multiplex sublayer as a Rate 1 packet.
        ** --------------------------------------------------------------- */

        /* Ensure that first two bits are set to '00' */
        *frame_data[0] = 0;

        /* Copy the received data bits into transmit frame */
        b_copy( rx_data_item->data_bits, 2, (byte *) frame_data, 2,
                (lb_full_rate_bits - 2) ); //lint !e734 loss of precision

        tx_data_rate = CAI_FULL_RATE;
        break;

      case CAI_MUX2_FOR_2_ID:
      case CAI_MUX2_FOR_6_ID:
      case CAI_MUX2_FOR_11_ID:

      case CAI_MUX1_FOR_2_ID:
      case CAI_MUX1_FOR_6_ID:
      case CAI_MUX1_FOR_11_ID:
        /* ---------------------------------------------------------------
        ** If the received packet is Rate 1/2, then Service Option 2 shall
        ** set the CAT_TYPE field equal to '00' and the data bits to
        ** the rxed data bits.  The resulting information bits shall
        ** be supplied to the multiplex sublayer as a Rate 1/2 packet.
        ** --------------------------------------------------------------- */

        /* Ensure that first two bits are set to '00' */
        *frame_data[0] = 0;

        /* Copy the received data bits into transmit frame */
        b_copy( rx_data_item->data_bits, 2, (byte *) frame_data, 2,
                (lb_half_rate_bits - 2) ); //lint !e734 loss of precision

        tx_data_rate = CAI_HALF_RATE;
        break;

      case CAI_MUX2_FOR_3_ID:
      case CAI_MUX2_FOR_7_ID:
      case CAI_MUX2_FOR_12_ID:
      case CAI_MUX2_FOR_15_ID:
      case CAI_MUX2_FOR_19_ID:

      case CAI_MUX1_FOR_3_ID:
      case CAI_MUX1_FOR_7_ID:
      case CAI_MUX1_FOR_12_ID:
        /* ---------------------------------------------------------------
        ** If the received packet is Rate 1/4, then Service Option 2 shall
        ** set the CAT_TYPE field equal to '00' and the data bits to
        ** the rxed data bits.  The resulting information bits shall
        ** be supplied to the multiplex sublayer as a Rate 1/4 packet.
        ** --------------------------------------------------------------- */

        /* Ensure that first two bits are set to '00' */
        *frame_data[0] = 0;

        /* Copy the received data bits into transmit frame */
        b_copy( rx_data_item->data_bits, 2, (byte *) frame_data, 2,
                (lb_quarter_rate_bits - 2) ); //lint !e734 loss of precision

        tx_data_rate = CAI_QUARTER_RATE;
        break;

      case CAI_MUX2_FOR_4_ID:
      case CAI_MUX2_FOR_8_ID:
      case CAI_MUX2_FOR_10_ID:
      case CAI_MUX2_FOR_13_ID:
      case CAI_MUX2_FOR_16_ID:
      case CAI_MUX2_FOR_18_ID:
      case CAI_MUX2_FOR_20_ID:
      case CAI_MUX2_FOR_22_ID:
      case CAI_MUX2_FOR_24_ID:

      case CAI_MUX1_FOR_4_ID:
      case CAI_MUX1_FOR_8_ID:
      case CAI_MUX1_FOR_13_ID:
        /* ---------------------------------------------------------------
        ** If the received packet is Rate 1/8, then Service Option 2 shall
        ** set the CAT_TYPE field equal to '00' and the data bits to
        ** the rxed data bits.  The resulting information bits shall
        ** be supplied to the multiplex sublayer as a Rate 1/8 packet.
        ** --------------------------------------------------------------- */

        /* Ensure that first two bits are set to '00' */
        *frame_data[0] = 0;

        /* Copy the received data bits into transmit frame */
        b_copy( rx_data_item->data_bits, 2, (byte *) frame_data, 2,
                (lb_eighth_rate_bits - 2) ); //lint !e734 loss of precision

        tx_data_rate = CAI_EIGHTH_RATE;
        break;

      case CAI_MUX2_FOR_5_ID:
      case CAI_MUX2_FOR_9_ID:
      case CAI_MUX2_FOR_14_ID:
      case CAI_MUX2_FOR_17_ID:
      case CAI_MUX2_FOR_21_ID:
      case CAI_MUX2_FOR_23_ID:
      case CAI_MUX2_FOR_25_ID:

      case CAI_MUX1_FOR_5_ID:
      case CAI_MUX1_FOR_14_ID:
        /* ---------------------------------------------------------------
        ** If the received packet is blank, then Service Option 2 shall
        ** set the CAT_TYPE field equal to '01' and shall set data
        ** bits to any desired value.  The resulting information bits
        ** shall be supplied to the multiplex sublayer as a Rate 1 packet.
        ** --------------------------------------------------------------- */

        /* Ensure that first two bits are set to '01' */
        *frame_data[0] = 0x40;

        tx_data_rate = CAI_FULL_RATE;
        break;

      case CAI_MUX1_FOR_9_ID:
        /* ----------------------------------------------------------------
        ** If the received packet is Rate 1 with bit errors, then Service
        ** Option 2 shall set the CAT_TYPE field equal to '10' and the 169
        ** data bits to the received 169 received data bits.  The resulting
        ** 171 information bits shall be supplied to the multiplex sublayer
        ** as a Rate 1 packet.
        ** ---------------------------------------------------------------- */

        /* Ensure that first two bits are set to '10' */
        *frame_data[0] = 0x80;

        /* Copy the 169 received data bits into transmit frame */
        b_copy( rx_data_item->data_bits, 2, (byte *) frame_data, 2,
                (lb_full_rate_bits - 2) ); //lint !e734 loss of precision

        tx_data_rate = CAI_FULL_RATE;
        break;

      case CAI_MUX2_FOR_26_ID:
      case CAI_MUX1_FOR_10_ID:
        /* ---------------------------------------------------------------
        ** If the received packet is insufficient frame quality (erasure),
        ** then Service Option 2 shall set the CAT_TYPE field equal to
        ** '11' and shall set the 169 data bits to any desired value.
        ** The resulting 171 information bits shall be supplied to the
        ** multiplex sublayer as a Rate 1 packet.
        ** --------------------------------------------------------------- */

        /* Ensure that first two bits are set to '11' */
        *frame_data[0] = 0xC0;

        tx_data_rate = CAI_FULL_RATE;
        break;

      default:
        ERR_FATAL( "Invalid rx frame category",
                    rx_data_item->frame_category, 0, 0 );

    } /* end switch (rx_data_item->frame_category) */
  } /* end if (max_rate == CAI_FULL_RATE) */

  else if (max_rate == CAI_HALF_RATE)
  {
    /* ------------------------------------------------------------------
    ** Service Option 2 has been commanded to generate a non-blank packet
    ** with a maximum rate of 1/2
    ** ------------------------------------------------------------------ */

    switch (rx_data_item->frame_category)
    {
      case CAI_MUX1_FOR_1_ID:
        /* ----------------------------------------------------------------
        ** If the received packet is Rate 1, then Service Option 2 shall
        ** set the CAT_TYPE field equal to '11' and shall set 78 data bits
        ** to the rightmost 78 of the 169 rxed data bits.  The resulting
        ** 80 information bits  shall be supplied to the multiplex sublayer
        ** as a Rate 1/2 packet.
        ** ---------------------------------------------------------------- */

        /* Ensure that first two bits are set to '11' */
        *frame_data[0] = 0xC0;

        /* Copy the rightmost 78 received data bits into transmit frame */
        b_copy( rx_data_item->data_bits, (lb_full_rate_bits - 78), //lint !e734 loss of precision
                (byte *) frame_data, 2, (lb_half_rate_bits - 2) ); //lint !e734 loss of precision

        tx_data_rate = CAI_HALF_RATE;
        break;

      case CAI_MUX2_FOR_1_ID:
        /* ----------------------------------------------------------------
        ** If the received packet is Rate 1, then Service Option 2 shall
        ** set the CAT_TYPE field equal to '11' and shall set 122 data bits
        ** to the rightmost 122 of the 169 rxed data bits.  The resulting
        ** 124 information bits  shall be supplied to the multiplex sublayer
        ** as a Rate 1/2 packet.
        ** ---------------------------------------------------------------- */

        /* Ensure that first two bits are set to '11' */
        *frame_data[0] = 0xC0;

        /* Copy the rightmost 122 received data bits into transmit frame */
        b_copy( rx_data_item->data_bits, (lb_full_rate_bits - 122), //lint !e734 loss of precision
                (byte *) frame_data, 2, (lb_half_rate_bits - 2) ); //lint !e734 loss of precision

        tx_data_rate = CAI_HALF_RATE;
        break;

      case CAI_MUX2_FOR_2_ID:
      case CAI_MUX2_FOR_6_ID:
      case CAI_MUX2_FOR_11_ID:

      case CAI_MUX1_FOR_2_ID:
      case CAI_MUX1_FOR_6_ID:
      case CAI_MUX1_FOR_11_ID:
        /* ---------------------------------------------------------------
        ** If the received packet is Rate 1/2, then Service Option 2 shall
        ** set the CAT_TYPE field equal to '00' and the data bits to
        ** the rxed data bits.  The resulting information bits shall
        ** be supplied to the multiplex sublayer as a Rate 1/2 packet.
        ** --------------------------------------------------------------- */

        /* Ensure that first two bits are set to '00' */
        *frame_data[0] = 0;

        /* Copy the received data bits into transmit frame */
        b_copy( rx_data_item->data_bits, 2, (byte *) frame_data, 2,
                (lb_half_rate_bits - 2) ); //lint !e734 loss of precision

        tx_data_rate = CAI_HALF_RATE;
        break;

      case CAI_MUX2_FOR_3_ID:
      case CAI_MUX2_FOR_7_ID:
      case CAI_MUX2_FOR_12_ID:
      case CAI_MUX2_FOR_15_ID:
      case CAI_MUX2_FOR_19_ID:

      case CAI_MUX1_FOR_3_ID:
      case CAI_MUX1_FOR_7_ID:
      case CAI_MUX1_FOR_12_ID:
        /* ---------------------------------------------------------------
        ** If the received packet is Rate 1/4, then Service Option 2 shall
        ** set the CAT_TYPE field equal to '00' and the data bits to
        ** the rxed data bits.  The resulting information bits shall
        ** be supplied to the multiplex sublayer as a Rate 1/4 packet.
        ** --------------------------------------------------------------- */

        /* Ensure that first two bits are set to '00' */
        *frame_data[0] = 0;

        /* Copy the received data bits into transmit frame */
        b_copy( rx_data_item->data_bits, 2, (byte *) frame_data, 2,
                (lb_quarter_rate_bits - 2) ); //lint !e734 loss of precision

        tx_data_rate = CAI_QUARTER_RATE;
        break;

      case CAI_MUX2_FOR_4_ID:
      case CAI_MUX2_FOR_8_ID:
      case CAI_MUX2_FOR_10_ID:
      case CAI_MUX2_FOR_13_ID:
      case CAI_MUX2_FOR_16_ID:
      case CAI_MUX2_FOR_18_ID:
      case CAI_MUX2_FOR_20_ID:
      case CAI_MUX2_FOR_22_ID:
      case CAI_MUX2_FOR_24_ID:

      case CAI_MUX1_FOR_4_ID:
      case CAI_MUX1_FOR_8_ID:
      case CAI_MUX1_FOR_13_ID:
        /* ---------------------------------------------------------------
        ** If the received packet is Rate 1/8, then Service Option 2 shall
        ** set the CAT_TYPE field equal to '00' and the data bits to
        ** the rxed data bits.  The resulting information bits shall
        ** be supplied to the multiplex sublayer as a Rate 1/8 packet.
        ** --------------------------------------------------------------- */

        /* Ensure that first two bits are set to '00' */
        *frame_data[0] = 0;

        /* Copy the received data bits into transmit frame */
        b_copy( rx_data_item->data_bits, 2, (byte *) frame_data, 2,
                (lb_eighth_rate_bits - 2) ); //lint !e734 loss of precision

        tx_data_rate = CAI_EIGHTH_RATE;
        break;


      case CAI_MUX2_FOR_5_ID:
      case CAI_MUX2_FOR_9_ID:
      case CAI_MUX2_FOR_14_ID:
      case CAI_MUX2_FOR_17_ID:
      case CAI_MUX2_FOR_21_ID:
      case CAI_MUX2_FOR_23_ID:
      case CAI_MUX2_FOR_25_ID:

      case CAI_MUX1_FOR_5_ID:
      case CAI_MUX1_FOR_14_ID:
        /* -------------------------------------------------------------
        ** If the received packet is blank, then Service Option 2 shall
        ** set the CAT_TYPE field equal to '01' and shall set 78 data
        ** bits to any desired value.  The resulting 80 infor bits shall
        ** be supplied to the multiplex sublayer as a Rate 1/2 packet.
        ** ------------------------------------------------------------- */

        /* Ensure that first two bits are set to '01' */
        *frame_data[0] = 0x40;

        tx_data_rate = CAI_HALF_RATE;
        break;

      case CAI_MUX1_FOR_9_ID:
        /* ---------------------------------------------------------------
        ** If the received packet is Rate 1 with bit errors, then Service
        ** Option 2 shall set the CAT_TYPE field equal to '10' and shall
        ** set 78 data bits to the rightmost 78 of the rxed 169 received
        ** data bits.  The resulting 80 information bits shall be supplied
        ** to the multiplex sublayer as a Rate 1/2 packet.
        ** --------------------------------------------------------------- */

        /* Ensure that first two bits are set to '10' */
        *frame_data[0] = 0x80;

        /* Copy the rightmost 78 received data bits into transmit frame */
        b_copy( rx_data_item->data_bits, (lb_full_rate_bits - 78), //lint !e734 loss of precision
                (byte *) frame_data, 2, (lb_half_rate_bits - 2) ); //lint !e734 loss of precision

        tx_data_rate = CAI_FULL_RATE;
        break;

      case CAI_MUX2_FOR_26_ID:
      case CAI_MUX1_FOR_10_ID:
        /* ---------------------------------------------------------------
        ** If the received packet is insufficient frame quality (erasure),
        ** then Service Option 2 shall set the CAT_TYPE field equal to
        ** '11' and shall set 14 data bits to any desired value.  The
        ** resulting 16 info bits shall be supplied to the multiplex
        ** sublayer as a Rate 1/8 packet.
        ** --------------------------------------------------------------- */

        /* Ensure that first two bits are set to '11' */
        *frame_data[0] = 0xC0;

        tx_data_rate = CAI_EIGHTH_RATE;
        break;

      default:
        ERR_FATAL( "Invalid rx frame category",
                    rx_data_item->frame_category, 0, 0 );

    } /* end switch (rx_data_item->frame_category) */
  } /* end else if (max_rate == CAI_HALF_RATE) */

  else if (max_rate == CAI_BLANK_RATE)
  {
    /* ------------------------------------------------------------------
    ** If Service Option 2 has been commanded to generate a Blank packet,
    ** it shall do so, thus providing zero information bits to the
    ** multiplex sublayer.
    ** ------------------------------------------------------------------ */
    tx_data_rate = CAI_BLANK_RATE;
  }

  else
  {
    ERR_FATAL( "Invalid Rate, %d", (int) max_rate, 0, 0 );
  }

  return (tx_data_rate);

} /* end loopback_tx() */

/*===========================================================================

FUNCTION LOOPBACK_INIT

DESCRIPTION
  Sets the loopback receive data buffer to a known state.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void loopback_init
(
  int rate_set   /* current rate set of loopback service option */
)
{
  /* set the rate set information */
  if (rate_set == CAI_MUX1 )
  {
    lb_full_rate_bits = 171;
    lb_half_rate_bits = 80;
    lb_quarter_rate_bits = 40;
    lb_eighth_rate_bits = 16;
  }
  else if ( rate_set == CAI_MUX2 )
  {
    lb_full_rate_bits = 266;
    lb_half_rate_bits = 124;
    lb_quarter_rate_bits = 54 ;
    lb_eighth_rate_bits = 20;
  }

  /* ----------------------------------------------------------------
  ** Initialize the FIFO with one 'erasure' frame delay
  ** ---------------------------------------------------------------- */
  loopback_data.depth = 1;
  loopback_data.top_idx = 0;
  loopback_data.buf[0].frame_category = CAI_MUX1_FOR_10_ID;
  loopback_data.wait_for_rx = TRUE;

#ifdef FEATURE_IS2000_1X_ADV
/* ------------------------------------------------------------
** Set loopback test mode to FCH normal testing
** ------------------------------------------------------------ */
  lb_test_mode = LB_FCH_NORMAL_TEST;
#endif /* FEATURE_IS2000_1X_ADV */

} /* end loopback_init() */

#ifdef FEATURE_IS2000_1X_ADV
/*===========================================================================

FUNCTION LOOPBACK_LOG_RX

DESCRIPTION
  This function counts up the Loopback SO RX statistics.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void loopback_log_rx(word mux)
{
  /* determine counter based on frame catetory */
  switch( mux )
  {
    case CAI_FACK_EACK_RACK_ID:
    {
      /* F-ACKCH expected ACK received ACK */
      lso_rx_ackch.lso_fackch_eack_rack++;
      break;
    }

    case CAI_FACK_EACK_RNAK_ID:
    {
      /* F-ACKCH expected ACK received NAK */
      lso_rx_ackch.lso_fackch_eack_rnak++;
      break;
    }

    case CAI_FACK_ENAK_RACK_ID:
    {
      /* F-ACKCH expected NAK received ACK */
      lso_rx_ackch.lso_fackch_enak_rack++;
      break;
    }

    case CAI_FACK_ENAK_RNAK_ID:
    {
      /* F-ACKCH expected NAK received NAK */
      lso_rx_ackch.lso_fackch_enak_rnak++;
      break;
    }

    default:
    {
      M1X_MSG( DCP, LEGACY_ERROR,
        "Invalid frame category %d",
        mux);
    }

  }
} /* loopback_log_rx */

/*===========================================================================
FUNCTION LOOPBACK_GET_VECT

DESCRIPTION
  This function updates a given storage with the appropriate loopback stats.

DEPENDENCIES
  None.

RETURN VALUE
  Length of vector (vector elements are dwords here).

SIDE EFFECTS
  None.

===========================================================================*/

byte loopback_get_vect
(
  byte vect_counter_id,    /* id of vector of counters */
  dword *tsf               /* address of storage for vector */
)
{
  byte len;

  switch (vect_counter_id)
  {
    case 0: /* F-ACKCH Receive Counters */
    {
      tsf[0] = lso_rx_ackch.lso_fackch_eack_rack;
      tsf[1] = lso_rx_ackch.lso_fackch_eack_rnak;
      tsf[2] = lso_rx_ackch.lso_fackch_enak_rnak;
      tsf[3] = lso_rx_ackch.lso_fackch_enak_rack;
      len = 4;
      break;
    }

    default:
    {
      len = 0;
    }
  }
  return (len);
}

/*===========================================================================

FUNCTION LOOPBACK_PROC_CTL_DIR

DESCRIPTION
  Processes the service option control message directives for
  loopback service option.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void loopback_proc_ctl_dir
(
  const byte tsf[]
)
{
  byte index = LB_CTL_TYP;
  byte dir = tsf[index++];

  switch(dir)
  {
    case LB_FCH_DIR:
    {
      lb_test_mode = LB_FCH_NORMAL_TEST;
      M1X_MSG( DCP, LEGACY_MED,
        "Switching loopback to normal FCH test mode");
      break;
    }

    case LB_R_FCH_0_DIR:
    {
      lb_test_mode = LB_RFCH_BLANKING_TEST;
      M1X_MSG( DCP, LEGACY_MED,
        "Switching loopback to R-FCH 0bps test mode");
      break;
    }

    case LB_RACKCH1_DIR:
    {
      /* Call MUX API to send value of ACK_OR_NAK */
      txc_config_test_ack_or_nak(RACKCH1, tsf[index]);
      break;
    }

    case LB_FACKCH_DIR:
    {
      /* Call MUX API to send value of ACK_OR_NAK */
      rxc_config_test_ack_or_nak(tsf[index++]);

      switch (tsf[index++]) /* Clear Counters */
      {
        case 0: /* clear all F-ACKCH receive counters to zero */
        {
          memset(&lso_rx_ackch, 0, sizeof(lso_rx_ackch));
          break;
        }

        case 1: /* clear LSO_FACKCH_EACK_RACK and LSO_FACKCH_EACK_RNAK */
        {
          lso_rx_ackch.lso_fackch_eack_rack =
            lso_rx_ackch.lso_fackch_eack_rnak = 0;
          break;
        }

        case 2: /* clear LSO_FACKCH_ENAK_RACK and LSO_FACKCH_ENAK_RNAK */
        {
          lso_rx_ackch.lso_fackch_enak_rack =
            lso_rx_ackch.lso_fackch_enak_rnak = 0;
          break;
        }
      }
      break;
    }

    case LB_RACKCH2_DIR:
    {
      /* Call MUX API to send value of ACK_OR_NAK */
      txc_config_test_ack_or_nak(RACKCH2, tsf[index]);
      break;
    }

    default:
      M1X_MSG( DCP, LEGACY_ERROR,
        "Invalid control directive 0x%x",
        dir);
  }

}
#endif /* FEATURE_IS2000_1X_ADV */
