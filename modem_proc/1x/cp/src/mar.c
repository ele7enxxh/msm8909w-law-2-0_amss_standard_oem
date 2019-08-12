/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                   M A R    S E R V I C E S    F I L E

GENERAL DESCRIPTION
  This module contains functions and variables to support Frame & Bit Error
  Rate Services, using a Markov process to model rate changes.
  This includes generating tx packets, checking received
  packets and keeping statistics on the bit error types. The generation
  of the pseudo-random sequence of numbers is in accordance with the algo-
  rithm described in section 2.6.9 of the CDMA Digital CAI Standard. This
  algorithm is also presented, in great detail, in an article entitled
  "Random Number Generators: Good Ones Are Hard To Find", by Stephen K. Park
  and Keith W. Miller, Communications of the ACM, October 1988, Volume 31,
  Number 10.

  The algorithm uses the following iteration:

    z(n) = a x z(n-1) mod m

  where z(n) are integers, a = 7**5 = 16807, m = 2**31 - 1 = 2147483647.

  A generator based on this algorithm is known formally as a prime
  modulus multiplicative linear congruential generator.

EXTERNALIZED FUNCTIONS
  mar_init - for setting up the rate and mask variables.
  mar_tx - for generating the next pseudo-random packet.
  mar_rx - for checking the received packet against a reference.

INITIALIZATION AND SEQUENCING REQUIREMENTS
  Prior to using the other bit error rate services, function mar_init
  should be called to initialize the rate dependant variables.

  Note: these services are re-entrant, i.e., they may be called concurrently
  from multiple tasks and/or interrupt handlers.

  Copyright (c) 1990 - 2014 Qualcomm Technologies, Inc.
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

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$PVCSPath: L:/src/asw/MSM5100/CP_REL_A/vcs/mar.c_v   1.3   03 Oct 2002 10:51:08   phosabet  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/1x/cp/src/mar.c#1 $ $DateTime: 2016/12/13 08:00:08 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/06/14   gga     1X_CP F3 redistribution
01/03/13   nee     Replacement of memcpy() to memscpy()
01/23/12   srk     Feature Cleanup
06/14/11   ag      Fix to initialize MSO according to the actual SO connected
                   in traffic.
04/22/11   jtm     CMI Phase 4 - Final SU API changes.
04/12/11   ag      Added information logs for RDA test support.
04/04/11   jtm     Changed use of mar_tx_data in mar_rx to be more readable.
03/31/11   jtm     Added capability for MSO RDA test to be enabled dynamically.
03/31/11   ag      Added fix to not do RX/TX processing if MSO has not been
                   initialized at least once.
02/10/11   ag      Increased size of Markov statistics variables for eMSO to
                   32 bits from 16 bits to support long duration testing.
01/04/11   ag      Fixed compiler and lint warnings.
12/06/10   ag      Fixed the issue of ignoring frame based on PCG Index.
11/15/10   jtm     Initial SU API split for CP.
10/08/10   ag      Renamed Markov, Loopback related counters and functions.
09/07/10   ag      Moved MAR_IS_EMSO to mar.h.
07/27/10   ag      Removed support for DTX mode for SMS SO with RC8.
06/15/10   ag      Support to set Markov in DTX mode when SMS SO is with RC8.
06/15/10   ag      Added support for RDA testing
05/12/10   ssh     Cleaned up the lint issues.
04/21/10   jtm     Added include for modem_1x_utils.h.
04/19/10   jj      Replaced PACKED dword* with PACKED_UINT32_ACCESS, to be
                   able to work with Q6.
04/19/10   ag      Added support for displaying frames with bit errors.
03/02/10   ag      Added support for checking if FW guarantees a frame or not
                   when forward RC is RC11 and rate is 1/8 non-critical.
02/08/10   ag      Excluded D&B, B&B frames from total frame count which is
                   used for error rate calculation.
12/29/09   ag      Fixed processing of Markov model directive
12/15/09   ag      Fixed issue of Markov reset state not getting reset.
11/09/09   jtm     Fixed Klocwork errors.
11/06/09   ag      1X Advanced - Enhanced Markov Service Option Support. Also
                   fixed issues in legacy MSO operation.
11/02/09   jtm     Fixed compiler warnings.
05/14/09   ag      CMI phase 2 changes
04/26/06   an      Fixed Lint Errors
02/03/05   fc      Fixed lint error.
01/09/03   ph      removed FEATURE_NO_VBB
12/12/02   dlb     Typecast data_ptr as PACKED so that compiler won't assume
                   that it's word/dword aligned.
10/03/02   ph      Lint Cleanup
05/31/02   va      Fixed case 2 of mar_get_vect to retrieve values from mso_tx_save.
04/18/01   lcc     Removed mar_log_tx from IRAM.
07/27/00   rmd     Added MSM5000_IRAM_REV #ifndef around the functions that need to
                   be placed inside IRAM.
03/17/99   ms      Added FEATURE_NO_VBB.
10/02/98   pms     Neared more variables to save ROM space.
09/15/98   pms     Backed out _near pointers
09/04/98   pms     Neared the variables to save ROM space.
09/01/98   lcc     Merged in ARM support.
04/08/96   gb      Fixed Markov tx to send 8th rate frames before sync.
04/08/96   dna     Changed external array names to follow convention.
02/28/96   dna     Modified references to muli-dimentional markov tables, now
                   accessed through an indirect pointer, to dereference
                   the pointer.
01/15/96   gb      Fixed Markov reinitialization by adding mar_init_synch.
12/11/95   gb      Fixed mar_rx to not count signalling frames as errors
                   and fixed variable rate initialization.
11/20/95   gb      Added Markov retrievable parameter support.
10/06/95   gb      Fixed new vector counters to be spec compliant.
09/29/95   gb      Changed rate indexing to be raw VOC_RATE to handle blank.
09/07/95   gb      Fixed mar_get_stat reporting inverted rate indexing.
08/16/95   gb      Changed mar_get_stat reporting to support rate set 2.
06/14/95   gb      Added support for Rate set 2 service options.
05/12/94   gb      Added service to update markov probability table.
07/21/92   gb      Created DMSS port.

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "1x_variation.h"
#include <memory.h>
#include "comdef.h"
#include "modem_1x_defs.h"
#include "modem_1x_utils.h"

#include "cai_v.h"
#include "mccdma_v.h"
#include "mccdma_i.h"
#include "snm.h"
#include "mar_i.h"
#include "martable.h"
#include "rxccommon_i.h"

/*===========================================================================

                DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains definitions for constants, macros, types, variables
and other items needed by this module.

===========================================================================*/
/* SEEDS need to be reversed on opposite end of link - MTSO */
  /* random number generator seed for TX data and rates */
#define DEF_TX_SEED    0x55555555
  /* random number generator seed for RX data and rates */
#define DEF_RX_SEED    0x2aaaaaaa
//#define DEF_RX_SEED    0x55555555  /* For OFFLINE TEST only */

enum  mar_index_type {
  TOTAL_FRAMES,
  BAD_FRAMES,
  BIT_ERRS
} ;                /* indices into totals array */

/* indicates MSO has been initialized or not */
static boolean mar_init_done = FALSE;

/* indicates Test Control Directive or not */
static boolean mar_proc_test_ctl_dir = FALSE;

/* Counters for Rx statistics MSO_Ex_Rx and MSO_Ex_Ry.
 * The first row is not used. Remaining five rows are for frames of expected
 * rates eighth, quarter, half, full and zero respectively. This is in line
 * with mar_rate_type.
 * The six columns are for received frames of type - signaling, eighth rate,
 * quarter rate, half rate, full rate and erasure/zero rate. This is in line
 * with cai_data_rate_type. */
static dword mar_test [6] [6];

#ifdef FEATURE_IS2000_1X_ADV
static dword mar_pcg_ack[4] [14];  /* counters for ACK received in PCG (1-14)
                                     for all types of frames */
#endif /* FEATURE_IS2000_1X_ADV */

static dword mar_totals [3];           /* a few pertinent totals */
static dword good_errs [5];            /* frames detected correct rate but bit errs */
static word  rx_M_state;               /* current state of rx Markov model */
static word  tx_M_state;               /* current state of tx Markov model */
static dword rx_M_seed = DEF_RX_SEED;  /* current seed for rx Markov model */
static dword tx_M_seed = DEF_TX_SEED;  /* current seed for tx Markov model */
static word  mar_reset_state;          /* reset state of this rate Markov model */
#ifdef FEATURE_IS2000_1X_ADV
static word  mar_pending_reset_state;  /* pending reset state */
#endif /* FEATURE_IS2000_1X_ADV */
static word  M_test_mode;              /* index into tables of probabilities */
static word  M_rate_set;               /* index into tables of bitmasks */
static dword mar_esn;                  /* Serial num of phone used to sync Markov */
static word  mar_range;                /* Range of data rates for MSO */
static word  model_width;              /* width of MSO model */

#ifdef FEATURE_IS2000_1X_ADV
/* Instances of pending and in-use SCSTP tables */
static mar_prob_dtx_type mar_pending_scstp_dtx_prob;
static mar_prob_dtx_type mar_inuse_scstp_dtx_prob;
/* Boolean to indiacte Model Directive values are pending */
static boolean mar_model_dir_pending = FALSE;
#endif /* FEATURE_IS2000_1X_ADV */

static boolean mar_limit_state = FALSE;     /* limit state enabled flag */
static word    mar_limit_frames = 3;        /* limit state frame count */
static boolean mar_limit_flag = TRUE;       /* limit state invert flag */
static word    mar_limit_count;             /* limit state counter */
static word    new_limit_count;
static byte    new_rate;

static boolean mar_copy_rx_counters;
static boolean mar_copy_tx_counters;
static boolean mar_clear_rx_counters;
static boolean mar_clear_tx_counters;

/* This defines the 'a' value used by the generator. */
#define  RAN_A_VAL  16807

/* This defines the 'm' value used by the generator. */
#define  RAN_M_VAL  2147483647

/* indicator of whether markov service is in sync with base station */
static boolean mar_rx_sync;
static boolean mar_tx_sync;

/* Formal statistics types to store copied counters */
static mso_e_tx_type mso_tx, mso_tx_save;
static mso_e_rx_type mso_rx;

#ifdef FEATURE_IS2000_1X_ADV
static mso_tx_ackch1_type mso_tx_achch1, mso_tx_achch1_save;
static mso_rx_pcg_ack_type mso_pcg_ack;
static mso_rx_ackch_type mso_rx_achch, mso_rx_achch_save;
#endif /* FEATURE_IS2000_1X_ADV */

#ifdef FEATURE_MODEM_1X_MSO_RDA_TEST
/* Counters for received good frames (containing no bit errors).
 * The five rows are for frames of expected rates eighth, quarter,
 * half, full and zero respectively. This is in line with mar_rate_type.
 * The five columns are for marked frames of type - eighth rate,
 * quarter rate, half rate, full rate and erasure. This is in line
 * with cai_data_rate_type.
 */
static dword mar_good_frames[5][5];

/* Counters for bad frames (having bit errors).
 * The five rows are for frames of expected rates eighth, quarter,
 * half, full and zero respectively. This is in line with mar_rate_type.
 * The five columns are for marked frames of type - eighth rate,
 * quarter rate, half rate, full rate and erasure. This is in line with
 * cai_data_rate_type.
 */
static dword mar_bad_frames[5][5];
#endif /* FEATURE_MODEM_1X_MSO_RDA_TEST */

static void mar_copy_rx_stats (void);
static void mar_copy_tx_stats (void);

/* The connected Markov service option */
word mar_so;

/*===========================================================================

FUNCTION MAR_RAN_NEXT

DESCRIPTION
  This function seeds the pseudo-random number generator with a specified
  value, performs the next iteration for the pseudo-random
  number sequence and returns the resulting value.

DEPENDENCIES
  None.

RETURN VALUE
  This function a returns a left justified, 31-bit psuedo-random dword
  representing a binary fraction in the range: 0 < binary fraction < 1.
  Notice that bit zero of the return value will always be zero.
  This value can be used to re-seed the generator.

SIDE EFFECTS
  None.

===========================================================================*/

dword  mar_ran_next
(
  dword  seed
    /* This value is used to seed the pseudo-random number generator.
       It MUST be in the following range: 0 < seed < 2**31 - 1. Notice
       that the seed must NOT be zero and must NOT be 2**31 - 1. */
)
{
  /*------------------------------------------------------------------------
    This function calculates the next value of ran_reg using the formula:

          z(n) = a * z(n-1) mod( 2^31 - 1 )

    The value of z(n) is calculated as follows using a 32-bit wordsize:

    1. Shift z(n-1) left by one bit so it is right justified.

    2. Multiply a*z(n-1) and place the result into two 32-bit words called
       az_hi and az_lo.

       At this point, we have:   a*z(n-1) = az_hi*(2^32) + az_lo

    3. Keep the lower 31 bits of the product in the 32-bit word az_lo, and
       place the remaining upper 15 bits in the 32-bit word az_hi.

       At this point, we have:   a*z(n-1) = az_hi*(2^31) + az_lo
       which can be rewritten:   a*z(n-1) = az_hi*(2^31-1) + (az_lo+az_hi)

    4. To get a*z(n-1)mod(2^31-1), add az_hi to az_lo and check to see if
       the result is equal to or greater than 2^31-1.  If it is, subtract
       off 2^31-1. (This is why z(n-1) was right justified in step 1)

    5. Finally, shift the result left by one bit to make z(n) a left
       justified binary fraction again.
  ------------------------------------------------------------------------*/

  dword az_hi,az_lo;

  /* Shift z(n-1) left one bit so we can use MSB as an overflow bit */

  seed >>= 1;

  /* Multiply a*z(n-1), putting the lower 31 bits into az_lo, and the
  ** remaining upper bits into az_hi
  */

  az_lo = RAN_A_VAL * (seed & 0xFFFF);
  az_hi = RAN_A_VAL * (seed >> 16) + (az_lo >> 16);

  az_lo = ( (az_hi << 16) | (az_lo & 0xFFFF) ) & RAN_M_VAL;
  az_hi >>= 15;

  /* Add the upper bits to the lower bits to get a*z(n-1)mod(2^31-1), if the
  ** result is not less than the modulus, subtract the modulus from the result
  */

  az_lo += az_hi;

  if ( az_lo >= RAN_M_VAL ){
    az_lo -= RAN_M_VAL;
  }

  /* Place the result into z(n) and shift it back to a left justified
  ** binary fraction
  */

  seed = az_lo << 1;

  return seed;

} /* mar_ran_next */

/*===========================================================================

FUNCTION MAR_TX

DESCRIPTION
  This function seeds the pseudo-random number generator with the frame
  value, performs the next iterations for the pseudo-random
  number sequence and stores the resulting packet in the buffer pointed
  to by data_ptr.

DEPENDENCIES
  The rate dependant variables should have been initialized by a call to
  mar_init before calling this function.

RETURN VALUE
  Returns the test rate value of the frame produced.

SIDE EFFECTS
  The buffer pointed to by data_ptr will be filled with up to 171 bits.

===========================================================================*/

byte  mar_tx (
byte *data_ptr,                 /* buffer to be filled */
dword frame32,                  /* ls 32 bits of frame number */
boolean is_rx                   /* indicator whether rx frame or tx frame */
)
{
  word i;
  dword *seed_ptr;              /* pointer to previous value */
  word cur_state;               /* temp variable for state */
  byte mar_frame_rate;                /* rate of this frame */
  word derivative;              /* Markov state modulo 4data rate range */
  word rand15;
  word *state_prob;             /* pointer to array of probabilities for a
                                   data rate and state combination */
#ifdef FEATURE_IS2000_1X_ADV
    size_t copy_size;
    /* Return value of memscpy function */
#endif /* FEATURE_IS2000_1X_ADV */


/*-------------------------------------------------------------------------*/

  if (!mar_init_done)
  {
    M1X_MSG( DCP, LEGACY_ERROR,
      "MSO not initialized. Return 1/8 rate");
    return (MAR_8_RATE);
  }

  if ((cdma.so_cur_pri != CAI_SO_NULL) && (mar_so != cdma.so_cur_pri))
  {
    mar_init(M_test_mode, TRUE, FALSE, mar_esn, M_rate_set);
  }

  if (is_rx)                    /* is it an RX frame? */
  {
    seed_ptr = &rx_M_seed;      /* point to RX seed */
    cur_state = rx_M_state;     /* point to RX state */
    frame32 ^= DEF_RX_SEED;     /* XOR frame with RX SEED */
  }
  else
  {
    seed_ptr = &tx_M_seed;      /* point to TX seed */
    cur_state = tx_M_state;     /* point to TX state */
    frame32 ^= DEF_TX_SEED;     /* XOR frame with TX SEED */
  }

  if ((frame32 & 0x1FF) == (mar_esn & 0x1FF ))  /* 512 frame Markov sync */
  {

#ifdef FEATURE_IS2000_1X_ADV
    /* Copy pending values to in-use variables */
    if (MAR_IS_EMSO && is_rx && mar_model_dir_pending)
    {
      mar_model_dir_pending = FALSE;
      mar_reset_state = mar_pending_reset_state;          /* Reset Markov state */
      copy_size = memscpy(mar_inuse_scstp_dtx_prob,sizeof(mar_prob_dtx_type),
                          mar_pending_scstp_dtx_prob,sizeof(mar_prob_dtx_type)
                          );

      if( copy_size != sizeof(mar_prob_dtx_type) )
      {
        M1X_MSG( DCP, LEGACY_ERROR,
          "Truncated memscpy, bytes_copied:%d, src_size:%d, dest_size:%d",
          copy_size,
          sizeof(mar_prob_dtx_type),
          sizeof(mar_prob_dtx_type));
      }
    }
#endif /* FEATURE_IS2000_1X_ADV */

    cur_state = mar_reset_state;                /* Reset Markov state */
    *seed_ptr = frame32 << 1;                   /* Reset seed to current */
    *seed_ptr = mar_ran_next (*seed_ptr);       /*    frame and crank a */
    *seed_ptr = mar_ran_next (*seed_ptr);       /*    few times */
    *seed_ptr = mar_ran_next (*seed_ptr);
    if (is_rx)                                  /* is it an RX frame? */
    {
      if (mar_copy_rx_counters)
      {
        mar_copy_rx_counters = FALSE;
        mar_copy_rx_stats ();
      }
      if (mar_clear_rx_counters)
      {
        mar_clear_rx_counters = FALSE;
        (void) memset ( mar_test, 0, sizeof(mar_test) );
#ifdef FEATURE_IS2000_1X_ADV
        (void) memset ( mar_pcg_ack, 0, sizeof(mar_pcg_ack) );
        (void) memset ( &mso_rx_achch, 0, sizeof(mso_rx_achch_save) );
#endif /* FEATURE_IS2000_1X_ADV */
        (void) memset ( mar_totals, 0, sizeof(mar_totals) );
        (void) memset ( good_errs, 0, sizeof(good_errs) );

#ifdef FEATURE_MODEM_1X_MSO_RDA_TEST
        (void) memset ( mar_good_frames, 0, sizeof(mar_good_frames) );
        (void) memset ( mar_bad_frames, 0, sizeof(mar_bad_frames) );
#endif
      }

      mar_rx_sync = TRUE;                       /* flag RX now in sync */
    }
    else                                        /* is it a TX frame? */
    {
      if (mar_copy_tx_counters)
      {
        mar_copy_tx_counters = FALSE;
        mar_copy_tx_stats ();
      }
      if (mar_clear_tx_counters)
      {
        mar_clear_tx_counters = FALSE;
        (void) memset ( &mso_tx, 0, sizeof(mso_tx) );
#ifdef FEATURE_IS2000_1X_ADV
        (void) memset ( &mso_tx_achch1, 0, sizeof(mso_tx_achch1) );
#endif /* FEATURE_IS2000_1X_ADV */
      }

      mar_tx_sync = TRUE;                     /* flag TX now in sync */
    }
  }

  state_prob = (
#ifdef FEATURE_IS2000_1X_ADV
    MAR_IS_EMSO ?
     (word*)mar_inuse_scstp_dtx_prob[M_test_mode][cur_state] :
#endif /* FEATURE_IS2000_1X_ADV */
     (word*)mar_scstp_non_dtx_prob[M_test_mode][cur_state]);

  derivative = cur_state % (word) mar_range;           /* cur_state mod mar_range */

  *seed_ptr = mar_ran_next (*seed_ptr);         /* get next ran number */
  /* store 24 bits in frame */
  PACKED_UINT32_ACCESS( data_ptr ) = *seed_ptr >> 8;  /*lint !e826 */
  rand15 = PACKED_UINT16_ACCESS(data_ptr) & 0x7FFF;

  for (i = 0; i < model_width; i++)
  {                         /* test ran # against Markov probabilities */
    if ( rand15 < state_prob[i] )
    {
      /* set next state in model */
      cur_state = (word) (mar_range * derivative + ( model_width - i ));
      /* no need to test more */
      break;
    }
  }
    /* if tested model_width times must be in last probability range - set new state */
  if (i == model_width) cur_state = mar_range * derivative;
  mar_frame_rate = 4 - (byte) (cur_state % mar_range);          /* get rate from state */

    /* look up the number of 24 bit fields we need to generate */
  for (i=1; i < mar_tri_bytes[M_rate_set] [mar_frame_rate]; i++)
  {
    *seed_ptr = mar_ran_next (*seed_ptr);       /* use prev to seed next */
    /* store next tri-byte */
    PACKED_UINT32_ACCESS( (data_ptr + i * 3) ) = *seed_ptr >> 8; /*lint !e826 */
  }
    /* mask unused bits in last tri-byte according to rate of frame */
  PACKED_UINT32_ACCESS( (data_ptr + (i-1) * 3) )&= mar_last_mask [M_rate_set] [mar_frame_rate]; /*lint !e826 */

  if (is_rx)
    rx_M_state = cur_state;                     /* update rx Markov */
  else
    tx_M_state = cur_state;                     /* update tx Markov */

  if (( mar_limit_state ) && ( !is_rx ))
  {
    new_rate = mar_frame_rate;
    new_limit_count = mar_limit_count;
    if ( ((!mar_limit_flag) &&
          (mar_frame_rate == MAR_1_RATE) && /*lint !e641 */
          (mar_limit_count <  mar_limit_frames)) ||
         (( mar_limit_flag) &&
          (mar_frame_rate == MAR_1_RATE) &&  /*lint !e641 */
          (mar_limit_count >= mar_limit_frames)) )
    {
      new_rate = MAR_2_RATE;  /*lint !e641 */
    }
    if (((mar_frame_rate == MAR_1_RATE) && (mar_limit_count >= mar_limit_frames)) || /*lint !e641 */
        (( mar_limit_flag) && (mar_frame_rate != MAR_1_RATE)) ) /*lint !e641 */
    {
      new_limit_count = 0;
    }
    if ( ((!mar_limit_flag ) && (mar_frame_rate != MAR_1_RATE)) ||  /*lint !e641 */
         ((mar_frame_rate == MAR_1_RATE) && (mar_limit_count <  mar_limit_frames)) )  /*lint !e641 */
    {
        new_limit_count = mar_limit_count + 1;
    }
    mar_frame_rate = new_rate;
    mar_limit_count = new_limit_count;
  }

  if ((!mar_tx_sync) && (!is_rx))
    return (MAR_8_RATE); /*lint !e641 */   /* return eighth rate until sync */

  return ( mar_frame_rate );                          /* return frame rate */

}

/*===========================================================================

FUNCTION MAR_RX

DESCRIPTION
  This function calls the mar_tx function to generate a reference packet
  and seeds the pseudo-random number generator with the received frame
  number, checks the rx packet against the reference and increments the
  appropriate error counters according to received rate decision rx_rate.

DEPENDENCIES
  The rate dependant variables should have been initialized by a call to
  mar_init before calling this function.

RETURN VALUE
  If Markov services are in sync this procedure returns a word being Markov
  rate in lower byte and frame bit errors in upper byte.  If Markov services
  are not in sync or if any input value to this function is invalid, then this
  procedure returns 0xFFFF, indicating that the value should not be logged.

SIDE EFFECTS
  Some of the error counters in the structure mar_test may be incremented.

===========================================================================*/

word  mar_rx (
byte *mar_rx_data,                       /* pointer to rx dec buffer */
byte rx_rate,                            /* rate decision from rx */
dword frame32,                           /* ls 32 bits of frame # */
byte *mar_mask_ptr                       /* mask of bit errors */
#ifdef FEATURE_MODEM_1X_MSO_RDA_TEST
,byte *ref_tx_data,                      /* Reference TX frame */
byte ref_frame_exp_rate                  /* Rate of expected frame */
#endif /* FEATURE_MODEM_1X_MSO_RDA_TEST */
#ifdef FEATURE_IS2000_1X_ADV
,int ack_pcg_index,                      /* PCG index of ACK */
boolean valid_frame                      /* TRUE if frame is guaranteed, FALSE otherwise */
#endif /* FEATURE_IS2000_1X_ADV */
)
{
  word i;
  byte err_mask;
  byte bers;                                 /* number of bit errs in pkt */
  int err = FALSE;                           /* bit error flag */
  byte exp_rate;                             /* Markov rate and mux */
  static byte mar_tx_data_buf [ 265 / 8 + 1 ];   /* tx reference pkt storage */
  byte* mar_tx_data = NULL;

#ifdef FEATURE_IS2000_1X_ADV
  const caii_srv_cfg_type  *srv_config;      /* Service configuration */
#endif /* FEATURE_IS2000_1X_ADV */

/*-------------------------------------------------------------------------*/

  if (!mar_init_done)
  {
    M1X_MSG( DCP, LEGACY_ERROR,
      "MSO not initialized.");
    return( 0xFFFF );                  /* return nosync flag */
  }

  #ifdef FEATURE_MODEM_1X_MSO_RDA_TEST
  if( rxc_is_mso_rda_test_enabled() )
  {
    /* point to rate and tx frame provided by Mux. */
    exp_rate = ref_frame_exp_rate;
    mar_tx_data = ref_tx_data;

    if (mar_tx_data == NULL)
    {
      M1X_MSG( DCP, LEGACY_ERROR,
        "mar_tx_data pointer NULL!");
      return (0xFFFF);
    }
  }
  else
  #endif /* FEATURE_MODEM_1X_MSO_RDA_TEST */
  {
    /* Point to rate and tx frame generated here. */
    exp_rate = mar_tx ( mar_tx_data_buf, frame32, TRUE ); /* create ref packet */
    mar_tx_data = mar_tx_data_buf;

    #ifdef FEATURE_IS2000_1X_ADV
    srv_config = snm_get_current_config();
    if (exp_rate == (byte)MAR_NC_8_RATE)
    {
      if ( (srv_config->for_fch_rc == (byte)CAI_RC_11) && !valid_frame )
      {
        exp_rate = (byte)MAR_0_RATE;
      }
      else
      {
        exp_rate = (byte)MAR_8_RATE;
      }
    }
    #endif /* FEATURE_IS2000_1X_ADV */
  }

  /* check each byte for errors and lookup # of bit errors in mar_ones table */

  bers = 0;

  /* Sanity check to make sure the rate is in the correct bounds. */
  if ((exp_rate < (byte) MAR_8_RATE) || (exp_rate > (byte) MAR_0_RATE))
  {
    M1X_MSG( DCP, LEGACY_HIGH,
      "Invalid expected Markov rate (%d)",
      exp_rate);
    return (0xFFFF);
  }

  M1X_MSG( DCP, LEGACY_LOW,
    "[MSO] Rate of expected frame : %d, received frame : %d",
    exp_rate,
    rx_rate);

  /* Compare the bits. Do not compare for Rate 0 frame */
  if ( (exp_rate != (byte)MAR_0_RATE) && (exp_rate == rx_rate) )
  {
    for (i=0; i < mar_bytes[M_rate_set] [exp_rate-1]; i++)
    {
      if ( (err_mask = (mar_rx_data [i] ^ mar_tx_data [i])) != 0)
      {
        if (mar_mask_ptr != NULL)
          mar_mask_ptr [i] = err_mask;
        bers += mar_ones [ err_mask ];
        err = TRUE;
      }
    }

    /* check last bits for errors and
       lookup # of bit errors in mar_ones table */
    err_mask =  (mar_rx_data [i] ^ mar_tx_data [i]) &
                           mar_last_byte [M_rate_set] [exp_rate-1];
    if (err_mask)
    {
      if (mar_mask_ptr != NULL)
        mar_mask_ptr [i] = err_mask;
      bers += mar_ones [ err_mask ];
      err = TRUE;
    }
  }

  if( mar_rx_sync )
  {
    /* markov service is in sync - tally statistics */

    /* Update RX counters (MSO_Ex_Rx, MSO_Ex_Ry) */
    if ((exp_rate != rx_rate) || ((exp_rate == rx_rate) && !err) )
    {
      mar_test [exp_rate] [rx_rate]++;    /* incr error counters in array */
    }

#ifdef FEATURE_IS2000_1X_ADV
    if (ack_pcg_index > 14)
    {
      M1X_MSG( DCP, LEGACY_HIGH,
        "Invalid PCG index %d of ACK",
        ack_pcg_index);
    }
    /* Increment counters for ACK in PCG */
    else if (!err && (ack_pcg_index > 0) && (exp_rate <= (byte)MAR_1_RATE) &&
             (exp_rate == rx_rate))
    {
      mar_pcg_ack [exp_rate - 1] [ack_pcg_index - 1]++;
    }
#endif /* FEATURE_IS2000_1X_ADV */

    /* Total frames count. Ignore signalling frames in total frame count
     * for the purpose of FER calculation. */
    if (rx_rate != (byte)CAI_BLANK_RATE)
    {
      mar_totals [TOTAL_FRAMES]++;
    }
    /* Bad frame count (MSO_Ex_Ry + MSO_Ex_RERR). Ignore signalling frames
     * for the purpose of FER calculation. */
    if (( err ) || ( rx_rate != exp_rate ))       /* bad rate decision */
    {
      if (rx_rate != (byte)CAI_BLANK_RATE)
        mar_totals [BAD_FRAMES]++;
    }
    /* Frames with detected correct rate but having bit errors (MSO_Ex_RERR) */
    if (( err ) && ( exp_rate == rx_rate )              /* good rate decision */
        && (exp_rate != (byte)MAR_0_RATE))  /* skip expected zero rate frames */
      good_errs [exp_rate]++;

    mar_totals [BIT_ERRS] += bers;                /* update total bit errors */

#ifdef FEATURE_MODEM_1X_MSO_RDA_TEST
    if( rxc_is_mso_rda_test_enabled() )
    {
      /* Update counters for number of received good frames and bad frames */
      err = FALSE;
      if ((exp_rate != (byte)MAR_0_RATE) && (rx_rate != (byte)CAI_BLANK_RATE))
      {
        for (i=0; i < mar_bytes[M_rate_set] [exp_rate-1]; i++)
        {
          if (mar_rx_data [i] ^ mar_tx_data [i])
          {
            err = TRUE;
          }
        }

        /* check last bits for errors */
        if ((mar_rx_data [i] ^ mar_tx_data [i]) &
                 mar_last_byte [M_rate_set] [exp_rate-1])
        {
          err = TRUE;
        }
      }

      if ( (rx_rate != (byte)CAI_BLANK_RATE) &&
           ((!err) ||
            ((exp_rate == (byte)MAR_0_RATE) && (rx_rate == (byte)CAI_NULL_RATE)) ) )
      {
        /* Update good frame counter */
        mar_good_frames[exp_rate-1][rx_rate-1]++;
        M1X_MSG( DCP, LEGACY_MED,
          "[MSO RDA Test] Expected rate %d ; RDA Marked rate %d. Genie good [ 1 ]",
          exp_rate,
          rx_rate);
      }

      if ( (rx_rate != (byte)CAI_BLANK_RATE) &&
           ( err ||
             ((exp_rate == (byte)MAR_0_RATE) && (rx_rate != (byte)CAI_NULL_RATE)) ) )
      {
        /* Update bad frame counter */
        mar_bad_frames[exp_rate-1][rx_rate-1]++;
        M1X_MSG( DCP, LEGACY_MED,
          "[MSO RDA Test] Expected rate %d ; RDA Marked rate %d. Genie good [ 0 ]",
          exp_rate,
          rx_rate);
      }
    }
#endif /* FEATURE_MODEM_1X_MSO_RDA_TEST */

    return (exp_rate + (bers << 8) );             /* return stats */
  }
  else
  {
    return( 0xFFFF );                             /* return nosync flag */
  }

}  /*lint !e818*/

/*===========================================================================

FUNCTION MAR_GET_STAT

DESCRIPTION
  This function accepts a pointer to a Markov statistic buffer.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

void mar_get_stat
(
  mar_stat_type *mar_ptr
    /* pointer to structure defined in mar.h */
)
{
  word i;

  mar_ptr->m_rate = (mar_test_mode_type) M_test_mode;    /* mode of Markov model */
  mar_ptr->bit_errs = mar_totals [BIT_ERRS];        /* bit error total */
  mar_ptr->bad_frames = mar_totals[BAD_FRAMES];     /* frame error total */
  mar_ptr->total_frames = mar_totals[TOTAL_FRAMES]; /* total frames rxed */

#ifdef FEATURE_IS2000_1X_ADV
  for( i=0; i<4; i++ )
  {
    mar_ptr->good_errs[i] = good_errs[i+1]; /* bit errs in good rate frames */
  }

  for( i=0; i<4; i++ )
  {
    mar_ptr->mar_test[i][9] = mar_test[4-i][5];       /* Erasures */
    mar_ptr->mar_test[i][0] = mar_test[4-i][4];       /* Got full errs */
    mar_ptr->mar_test[i][5] = mar_test[4-i][3];       /* Got 1/2 errs */
    mar_ptr->mar_test[i][6] = mar_test[4-i][2];       /* Got 1/4 errs */
    mar_ptr->mar_test[i][7] = mar_test[4-i][1];       /* Got 1/8 errs */
    mar_ptr->mar_test[i][3] = mar_test[4-i][0];       /* Signalling */
    mar_ptr->mar_test[i][8] = good_errs[4-i];         /* Frames with bit errors */
  }
  /* zero rate frames */
  mar_ptr->mar_test[4][9] = mar_test[5][5];            /* Erasures */
  mar_ptr->mar_test[4][0] = mar_test[5][4];            /* Got full errs */
  mar_ptr->mar_test[4][5] = mar_test[5][3];            /* Got 1/2 errs */
  mar_ptr->mar_test[4][6] = mar_test[5][2];            /* Got 1/4 errs */
  mar_ptr->mar_test[4][7] = mar_test[5][1];            /* Got 1/8 errs */
  mar_ptr->mar_test[4][3] = mar_test[5][0];            /* Signalling */
#else
  for( i=0; i<4; i++ )
  {
    mar_ptr->good_errs[i] = (word) good_errs[i+1]; /* bit errs in good rate frames */
  }

  for( i=0; i<4; i++ )
  {
    mar_ptr->mar_test[i][9] = (word) mar_test[4-i][5];       /* Erasures */
    mar_ptr->mar_test[i][0] = (word) mar_test[4-i][4];       /* Got full errs */
    mar_ptr->mar_test[i][5] = (word) mar_test[4-i][3];       /* Got 1/2 errs */
    mar_ptr->mar_test[i][6] = (word) mar_test[4-i][2];       /* Got 1/4 errs */
    mar_ptr->mar_test[i][7] = (word) mar_test[4-i][1];       /* Got 1/8 errs */
    mar_ptr->mar_test[i][3] = (word) mar_test[4-i][0];       /* Signalling */
    mar_ptr->mar_test[i][8] = (word) good_errs[4-i];         /* Frames with bit errors */
  }
#endif /* FEATURE_IS2000_1X_ADV */

} /* mar_get_stat */

#ifdef FEATURE_MODEM_1X_MSO_RDA_TEST
/*===========================================================================

FUNCTION MAR_GET_RDA_TEST_STAT

DESCRIPTION
  This function accepts a pointer to a buffer for Markov statistic for
  RDA test.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

void mar_get_rda_test_stat
(
  mar_rda_test_stat_type *stat_ptr
    /* pointer to structure defined in mar.h */
)
{
  word i;

  stat_ptr->m_rate = (mar_test_mode_type) M_test_mode;    /* mode of Markov model */

  /* loop for expected full rate to eighth rate frames */
  for ( i=0; i<4; i++)
  {
    stat_ptr->mar_good_frames[i][0] = mar_good_frames[3-i][3];    /* full rate */
    stat_ptr->mar_good_frames[i][1] = mar_good_frames[3-i][2];    /* 1/2 rate  */
    stat_ptr->mar_good_frames[i][2] = mar_good_frames[3-i][1];    /* 1/4 rate  */
    stat_ptr->mar_good_frames[i][3] = mar_good_frames[3-i][0];    /* 1/8 rate  */
    stat_ptr->mar_good_frames[i][4] = mar_good_frames[3-i][4];    /* erasure   */
  }

  /* Expected zero rate */
  stat_ptr->mar_good_frames[4][0] = mar_good_frames[4][3];        /* full rate */
  stat_ptr->mar_good_frames[4][1] = mar_good_frames[4][2];        /* 1/2 rate  */
  stat_ptr->mar_good_frames[4][2] = mar_good_frames[4][1];        /* 1/4 rate  */
  stat_ptr->mar_good_frames[4][3] = mar_good_frames[4][0];        /* 1/8 rate  */
  stat_ptr->mar_good_frames[4][4] = mar_good_frames[4][4];        /* erasure   */

  /* loop for expected full rate to eighth rate frames */
  for ( i=0; i<4; i++)
  {
    stat_ptr->mar_bad_frames[i][0] = mar_bad_frames[3-i][3];      /* full rate */
    stat_ptr->mar_bad_frames[i][1] = mar_bad_frames[3-i][2];      /* 1/2 rate  */
    stat_ptr->mar_bad_frames[i][2] = mar_bad_frames[3-i][1];      /* 1/4 rate  */
    stat_ptr->mar_bad_frames[i][3] = mar_bad_frames[3-i][0];      /* 1/8 rate  */
    stat_ptr->mar_bad_frames[i][4] = mar_bad_frames[3-i][4];      /* erasure   */
  }

  /* Expected zero rate */
  stat_ptr->mar_bad_frames[4][0] = mar_bad_frames[4][3];          /* full rate */
  stat_ptr->mar_bad_frames[4][1] = mar_bad_frames[4][2];          /* 1/2 rate  */
  stat_ptr->mar_bad_frames[4][2] = mar_bad_frames[4][1];          /* 1/4 rate  */
  stat_ptr->mar_bad_frames[4][3] = mar_bad_frames[4][0];          /* 1/8 rate  */
  stat_ptr->mar_bad_frames[4][4] = mar_bad_frames[4][4];          /* erasure   */
} /* mar_get_rda_test_stat */
#endif /* FEATURE_MODEM_1X_MSO_RDA_TEST */

/*===========================================================================

FUNCTION MAR_COPY_RX_STAT

DESCRIPTION
  This function takes a snapshot of the Markov statistics.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

static void mar_copy_rx_stats (void)
{
  size_t copy_size;
  /* Return value of memscpy function */

  copy_size = memscpy ( &mso_rx, sizeof(mso_e_rx_type),
                       mar_test, sizeof(mar_test) );
  if( copy_size != sizeof(mar_test) )
  {
    M1X_MSG( DCP, LEGACY_ERROR,
      "Truncated memscpy, bytes_copied:%d, src_size:%d, dest_size:%d",
      copy_size,
      sizeof(mar_test),
      sizeof(mso_e_rx_type));
  }

#ifdef FEATURE_IS2000_1X_ADV

  copy_size = memscpy ( &mso_pcg_ack, sizeof(mso_rx_pcg_ack_type),
                       mar_pcg_ack, sizeof(mar_pcg_ack) );

  if( copy_size != sizeof(mar_pcg_ack) )
  {
    M1X_MSG( DCP, LEGACY_ERROR,
      "Truncated memscpy, bytes_copied:%d, src_size:%d, dest_size:%d",
      copy_size,
      sizeof(mar_pcg_ack),
      sizeof(mso_rx_pcg_ack_type));
  }
 mso_rx_achch_save = mso_rx_achch;
#endif /* FEATURE_IS2000_1X_ADV */

} /* mar_copy_rx_stats */

/*===========================================================================

FUNCTION MAR_COPY_TX_STATS

DESCRIPTION
  This function takes a snapshot of the Markov statistics.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

static void mar_copy_tx_stats (void)
{

 mso_tx_save = mso_tx;
#ifdef FEATURE_IS2000_1X_ADV
 mso_tx_achch1_save = mso_tx_achch1;
#endif /* FEATURE_IS2000_1X_ADV */

} /* mar_copy_tx_stats */

/*===========================================================================

FUNCTION MAR_LOG_TX

DESCRIPTION
  This function counts up the Markov TX statistics.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

void mar_log_tx (word mux)
{

  if (!mar_tx_sync)
    return;

  /* determine counter based on frame catetory */
  switch( mux )
  {

    case CAI_MUX1_REV_1_ID:
    case CAI_MUX2_REV_1_ID:
    {
      /* full rate */
      mso_tx.mso_e1_t1++;
      break;
    }

    case CAI_MUX1_REV_6_ID:
    case CAI_MUX2_REV_11_ID:
    {
      /* half rate frame */
      mso_tx.mso_e2_t2++;
      break;
    }

    case CAI_MUX1_REV_7_ID:
    case CAI_MUX2_REV_19_ID:
    {
      /* quarter rate frame */
      mso_tx.mso_e4_t4++;
      break;
    }

    case CAI_MUX1_REV_8_ID:
    case CAI_MUX2_REV_24_ID:
    {
      /* eigth rate frame */
      mso_tx.mso_e8_t8++;
      break;
    }

#ifdef FEATURE_IS2000_1X_ADV
    case CAI_REV_NC_TX_ID:
    {
      /* non-critical eight rate transmitted frame */
      mso_tx.mso_enc_t8++;
      break;
    }

    case CAI_REV_NC_BLANK_ID:
    {
      /* non-critical eight rate DTX-ed frame */
      mso_tx.mso_enc_t0++;
      break;
    }
#endif /* FEATURE_IS2000_1X_ADV */

    case CAI_MUX1_REV_2_ID:
    case CAI_MUX1_REV_3_ID:
    case CAI_MUX1_REV_4_ID:
    case CAI_MUX1_REV_11_ID:
    case CAI_MUX1_REV_12_ID:
    case CAI_MUX1_REV_13_ID:

    case CAI_MUX2_REV_2_ID:
    case CAI_MUX2_REV_3_ID:
    case CAI_MUX2_REV_4_ID:
    case CAI_MUX2_REV_6_ID:
    case CAI_MUX2_REV_7_ID:
    case CAI_MUX2_REV_8_ID:
    case CAI_MUX2_REV_10_ID:
    {
      /* full rate d&b frame */
      mso_tx.mso_e1_td++;
      break;
    }

    case CAI_MUX1_REV_5_ID:
    case CAI_MUX1_REV_14_ID:

    case CAI_MUX2_REV_5_ID:
    case CAI_MUX2_REV_9_ID:
    {
      /* full rate b&b frame */
      mso_tx.mso_e1_tb++;
      break;
    }

    case CAI_MUX2_REV_12_ID:
    case CAI_MUX2_REV_13_ID:
    case CAI_MUX2_REV_15_ID:
    case CAI_MUX2_REV_16_ID:
    case CAI_MUX2_REV_18_ID:
    {
      /* half rate d&b frame */
      mso_tx.mso_e2_td++;
      break;
    }

    case CAI_MUX2_REV_14_ID:
    case CAI_MUX2_REV_17_ID:
    {
      /* half rate b&b frame */
      mso_tx.mso_e2_tb++;
      break;
    }

    case CAI_MUX2_REV_20_ID:
    case CAI_MUX2_REV_22_ID:
    {
      /* quarter rate d&b frame */
      mso_tx.mso_e4_td++;
      break;
    }

#ifdef FEATURE_IS2000_1X_ADV
    case CAI_REV_NC_PRI_SEC_ID:
    case CAI_REV_NC_PRI_SIG_ID:
    {
      /* non-critical eight rate d&b frame */
      mso_tx.mso_enc_td++;
      break;
    }
#endif /* FEATURE_IS2000_1X_ADV */

    case CAI_MUX2_REV_21_ID:
    case CAI_MUX2_REV_23_ID:
    {
      /* quarter rate b&b frame */
      mso_tx.mso_e4_tb++;
      break;
    }

    case CAI_MUX2_REV_25_ID:
    {
      /* eighth rate b&b frame */
      mso_tx.mso_e8_tb++;
      break;
    }

#ifdef FEATURE_IS2000_1X_ADV
    case CAI_RACK1_ACK_ID:
    {
      /* ACK on RACKCH1 */
      mso_tx_achch1.mso_rackch1_tack++;
      break;
    }

    case CAI_RACK1_NAK_ID:
    {
      /* NAK on RACKCH1 */
      mso_tx_achch1.mso_rackch1_tnak++;
      break;
    }
#endif /* FEATURE_IS2000_1X_ADV */

    default:
      /* appease lint */
      break;
  } /* switch */

} /* mar_log_tx */

/*===========================================================================

FUNCTION MAR_LOG_RX

DESCRIPTION
  This function counts up the Markov RX statistics.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void mar_log_rx(word mux)
{
  if (!mar_rx_sync)
    return;

#ifdef FEATURE_IS2000_1X_ADV
  /* determine counter based on frame catetory */
  switch( mux )
  {
    case CAI_FACK_EACK_RACK_ID:
    {
      /* F-ACKCH expected ACK received ACK */
      mso_rx_achch.mso_fackch_eack_rack++;
      break;
    }

    case CAI_FACK_EACK_RNAK_ID:
    {
      /* F-ACKCH expected ACK received NAK */
      mso_rx_achch.mso_fackch_eack_rnak++;
      break;
    }

    case CAI_FACK_ENAK_RACK_ID:
    {
      /* F-ACKCH expected NAK received ACK */
      mso_rx_achch.mso_fackch_enak_rack++;
      break;
    }

    case CAI_FACK_ENAK_RNAK_ID:
    {
      /* F-ACKCH expected NAK received NAK */
      mso_rx_achch.mso_fackch_enak_rnak++;
      break;
    }

    default:
      /* appease lint */
      break;
  }
#endif /* FEATURE_IS2000_1X_ADV */

} /* mar_log_rx */

/*===========================================================================

FUNCTION MAR_INIT

DESCRIPTION
  This function initializes the rate dependant variable global to this
  module and if requested zeroes the bit error accumulators.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  The local global variables mar_tri_bytes, mar_last_mask, mar_bytes,
  mar_last_byte are initialized and the structure test may be zeroed.

===========================================================================*/

void mar_init
(
word test_mode,              /* data rate to generate and test data at */
boolean clear_count,         /* flag TRUE = zero frame error counters */
boolean copy_count,          /* flag TRUE = copy frame error counters */
dword esn,                   /* Serial number of phone   */
boolean rate_set             /* rate set 1 or 2 (0 or 1) */
)
{
#ifdef FEATURE_IS2000_1X_ADV
  size_t copy_size;
  /* Return value of memscpy function */
#endif /* FEATURE_IS2000_1X_ADV */
  if (cdma.so_cur_pri == CAI_SO_NULL)
  {
    mar_so = cdma.so_req;
    M1X_MSG( DCP, LEGACY_HIGH,
      "Markov SO (set to so_req) %d",
      mar_so);
  }
  else
  {
    mar_so = cdma.so_cur_pri;
    M1X_MSG( DCP, LEGACY_HIGH,
      "Markov SO (set to so_cur_pri) %d",
      mar_so);
  }

  M_rate_set = rate_set;  /* which frame sizes to generate and check */

  mar_esn = esn;          /* When to sync markov model for this phone */

#ifdef FEATURE_IS2000_1X_ADV
  if(MAR_IS_EMSO)
    mar_range = (int)MAR_0_RATE;
  else
#endif /* FEATURE_IS2000_1X_ADV */
    mar_range = (int)MAR_1_RATE;

  model_width = mar_range - 1;

  M_test_mode = test_mode;   /* which Markov table of probabilities to use */
  switch (test_mode)
  {
    case MAR_VAR_RATE:
#ifdef FEATURE_IS2000_1X_ADV
    case MAR_NC_8_RATE_TEST:
#endif /* FEATURE_IS2000_1X_ADV */
    /* fall through */
      mar_reset_state = (word) (mar_range * mar_range - 1);
      break;
    default:
      mar_reset_state = (word) ((mar_range + 1) * ((int)MAR_1_RATE_TEST - (int)M_test_mode));
      break;
  }
  rx_M_state = mar_reset_state;  /* default state of RX Markov */
  tx_M_state = mar_reset_state;  /* default state of TX Markov */

  if (!mar_proc_test_ctl_dir)
  {
    /* Do not clear all counters while processing Test Control Directive */
    if (clear_count)
    {
      M1X_MSG( DCP, LEGACY_MED,
        "Clear all counters and buffers");
      (void) memset ( mar_test, 0, sizeof(mar_test) );
#ifdef FEATURE_IS2000_1X_ADV
      (void) memset ( mar_pcg_ack, 0, sizeof(mar_pcg_ack) );
#endif
      (void) memset ( mar_totals, 0, sizeof(mar_totals) );
      (void) memset ( good_errs, 0, sizeof(good_errs) );
      (void) memset ( &mso_rx, 0, sizeof(mso_rx) );
#ifdef FEATURE_IS2000_1X_ADV
      (void) memset ( &mso_rx_achch, 0, sizeof(mso_rx_achch) );
      (void) memset ( &mso_rx_achch_save, 0, sizeof(mso_rx_achch_save) );
#endif
      (void) memset ( &mso_tx, 0, sizeof(mso_tx) );
      (void) memset ( &mso_tx_save, 0, sizeof(mso_tx_save) );
#ifdef FEATURE_IS2000_1X_ADV
      (void) memset ( &mso_tx_achch1, 0, sizeof(mso_tx_achch1) );
      (void) memset ( &mso_tx_achch1_save, 0, sizeof(mso_tx_achch1_save) );
#endif /* FEATURE_IS2000_1X_ADV */

#ifdef FEATURE_MODEM_1X_MSO_RDA_TEST
      (void) memset ( mar_good_frames, 0, sizeof(mar_good_frames) );
      (void) memset ( mar_bad_frames, 0, sizeof(mar_bad_frames) );
#endif /* FEATURE_MODEM_1X_MSO_RDA_TEST */
    }

#ifdef FEATURE_IS2000_1X_ADV
    /* Do not reset SCSTP while processing test control directive */
    if (MAR_IS_EMSO)
    {
      /* Copy the default Scaled Cumulative State Transition Probability (DTX)
         values to the In-Use instance of the SCSTP Table. */
      copy_size = memscpy(mar_inuse_scstp_dtx_prob,sizeof(mar_prob_dtx_type),
                          mar_scstp_dtx_prob, sizeof(mar_prob_dtx_type));
      if( copy_size != sizeof(mar_prob_dtx_type) )
      {
        M1X_MSG( DCP, LEGACY_ERROR,
          "Truncated memscpy, bytes_copied:%d, src_size:%d, dest_size:%d",
          copy_size,
          sizeof(mar_prob_dtx_type),
          sizeof(mar_prob_dtx_type));
      }

      copy_size = memscpy(mar_pending_scstp_dtx_prob, sizeof(mar_prob_dtx_type),
                          mar_scstp_dtx_prob, sizeof(mar_prob_dtx_type));

      if( copy_size != sizeof(mar_prob_dtx_type) )
      {
        M1X_MSG( DCP, LEGACY_ERROR,
          "Truncated memscpy, bytes_copied:%d, src_size:%d, dest_size:%d",
          copy_size,
          sizeof(mar_prob_dtx_type),
          sizeof(mar_prob_dtx_type));
      }
    }
#endif /* FEATURE_IS2000_1X_ADV */

  }
  else
  {
    /* Set the indicator back to FALSE */
    mar_set_test_ctl_dir_ind(FALSE);
    mar_clear_rx_counters = clear_count;
    mar_clear_tx_counters = clear_count;
  }

  mar_copy_rx_counters = copy_count;
  mar_copy_tx_counters = copy_count;

  mar_init_done = TRUE;
}

/*===========================================================================

FUNCTION MAR_INIT_SYNCH

DESCRIPTION
 Initialize the state of the Markov model to unsynched.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  The local global boolean variable mar_rx_sync is cleared.

===========================================================================*/
void mar_init_synch(void)
{
 /* initialize markov services to not in sync */
 mar_rx_sync = FALSE;
 mar_tx_sync = FALSE;
}

/*===========================================================================

FUNCTION MAR_SET_TEST_CTL_DIR_IND

DESCRIPTION
  This function sets the Markov Test Control Directive indication.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

void mar_set_test_ctl_dir_ind
(
boolean ind
)
{
  mar_proc_test_ctl_dir = ind;
}

#ifdef FEATURE_IS2000_1X_ADV
/*===========================================================================

FUNCTION MAR_PROC_MODEL_DIR

DESCRIPTION
  This function does the Markov Model Directive processing.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

void mar_proc_model_dir
(
const byte tsf[]
)
{
  int tsf_index, i, j;
  mar_pending_reset_state = tsf[MAR_SYNC];
  tsf_index = MAR_SCSTP;
  for(i = 0; i < 25; i++)
  {
    for(j = 0; j < 4; j++)
    {
      mar_pending_scstp_dtx_prob[0][i][j] = tsf[tsf_index++];
      mar_pending_scstp_dtx_prob[0][i][j] <<= 8;
      mar_pending_scstp_dtx_prob[0][i][j] |= tsf[tsf_index++];
    }
  }
  mar_model_dir_pending = TRUE;
} /* mar_proc_model_dir */
#endif /* FEATURE_IS2000_1X_ADV */

/*===========================================================================

FUNCTION MAR_RATE_LIMIT

DESCRIPTION
  This function initializes the rate limiting algorithm to the specified rate.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

void mar_rate_limit
(
  word rate_factor        /* rate limit factor to use in sysparm format */
)
{
  switch (rate_factor)
  {
    case 0:
      mar_limit_state = FALSE;     /* limit state for normal operation */
      break;
    case 2:
      mar_limit_state = TRUE;     /* limit state for 7/8 rate limit */
      mar_limit_frames = 3;
      mar_limit_flag = TRUE;
      break;
    case 4:
      mar_limit_state = TRUE;     /* limit state for 3/4 rate limit */
      mar_limit_frames = 1;
      mar_limit_flag = TRUE;
      break;
    case 6:
      mar_limit_state = TRUE;     /* limit state for 5/8 rate limit */
      mar_limit_frames = 3;
      mar_limit_flag = FALSE;
      break;
    case 8:
      mar_limit_state = TRUE;     /* limit state for 1/2 rate limit */
      mar_limit_frames = 0;
      mar_limit_flag = TRUE;
      break;
    default:
      break;
  }
}

/*===========================================================================

FUNCTION MAR_GET_VECT

DESCRIPTION
  This function updates a given table with the appropriate Mar stats.

DEPENDENCIES
  None.

RETURN VALUE
  Length of vector (vector elements are dwords here).

SIDE EFFECTS
  None.

===========================================================================*/

byte mar_get_vect
(
byte vect_counter_id,   /* id of vector of counters */
dword *tsf               /* address of storage for vector */
)
{
  byte len;
  int i;

  switch (vect_counter_id)
  {
    case 0: /* FER Counters */
      tsf[0] = mso_rx.mso_e1_r1;
      tsf[1] = mso_rx.mso_e1_r1 + mso_rx.mso_e2_r2 +
               mso_rx.mso_e4_r4 + mso_rx.mso_e8_r8;
      tsf[2] = mso_rx.mso_e1_r2 + mso_rx.mso_e1_r4 +
               mso_rx.mso_e1_r8 + mso_rx.mso_e1_re +
               good_errs [MAR_1_RATE];
      tsf[3] = mso_rx.mso_e2_r1 + mso_rx.mso_e2_r4 +
               mso_rx.mso_e2_r8 + mso_rx.mso_e2_re +
               good_errs [MAR_2_RATE] +
               mso_rx.mso_e4_r1 + mso_rx.mso_e4_r2 +
               mso_rx.mso_e4_r8 + mso_rx.mso_e4_re +
               good_errs [MAR_4_RATE] +
               mso_rx.mso_e8_r1 + mso_rx.mso_e8_r2 +
               mso_rx.mso_e8_r4 + mso_rx.mso_e8_re +
               good_errs [MAR_8_RATE] + tsf [2];
      len = 4;

#ifdef FEATURE_IS2000_1X_ADV
      if (MAR_IS_EMSO)
      {
        tsf[4] = mso_pcg_ack.mso_e1_r1_1;
        tsf[5] = mso_pcg_ack.mso_e1_r1_2;
        tsf[6] = mso_pcg_ack.mso_e1_r1_3;
        tsf[7] = mso_pcg_ack.mso_e1_r1_4;
        tsf[8] = mso_pcg_ack.mso_e1_r1_5;
        tsf[9] = mso_pcg_ack.mso_e1_r1_6;
        tsf[10] = mso_pcg_ack.mso_e1_r1_7;
        tsf[11] = mso_pcg_ack.mso_e1_r1_8;
        tsf[12] = mso_pcg_ack.mso_e1_r1_9;
        tsf[13] = mso_pcg_ack.mso_e1_r1_10;
        tsf[14] = mso_pcg_ack.mso_e1_r1_11;
        tsf[15] = mso_pcg_ack.mso_e1_r1_12;
        tsf[16] = mso_pcg_ack.mso_e1_r1_13;
        tsf[17] = mso_pcg_ack.mso_e1_r1_14;
        len = 18;
      }
#endif /* FEATURE_IS2000_1X_ADV */

      break;

    case 1: /* Same rate received counters */
      tsf[0] = mso_rx.mso_e1_r1;
      tsf[1] = mso_rx.mso_e2_r2;
      tsf[2] = mso_rx.mso_e4_r4;
      tsf[3] = mso_rx.mso_e8_r8;
      len = 4;

#ifdef FEATURE_IS2000_1X_ADV
      if (MAR_IS_EMSO)
      {
        tsf[4] = mso_pcg_ack.mso_e1_r1_1;
        tsf[5] = mso_pcg_ack.mso_e1_r1_2;
        tsf[6] = mso_pcg_ack.mso_e1_r1_3;
        tsf[7] = mso_pcg_ack.mso_e1_r1_4;
        tsf[8] = mso_pcg_ack.mso_e1_r1_5;
        tsf[9] = mso_pcg_ack.mso_e1_r1_6;
        tsf[10] = mso_pcg_ack.mso_e1_r1_7;
        tsf[11] = mso_pcg_ack.mso_e1_r1_8;
        tsf[12] = mso_pcg_ack.mso_e1_r1_9;
        tsf[13] = mso_pcg_ack.mso_e1_r1_10;
        tsf[14] = mso_pcg_ack.mso_e1_r1_11;
        tsf[15] = mso_pcg_ack.mso_e1_r1_12;
        tsf[16] = mso_pcg_ack.mso_e1_r1_13;
        tsf[17] = mso_pcg_ack.mso_e1_r1_14;

        tsf[18] = mso_pcg_ack.mso_e2_r2_1;
        tsf[19] = mso_pcg_ack.mso_e2_r2_2;
        tsf[20] = mso_pcg_ack.mso_e2_r2_3;
        tsf[21] = mso_pcg_ack.mso_e2_r2_4;
        tsf[22] = mso_pcg_ack.mso_e2_r2_5;
        tsf[23] = mso_pcg_ack.mso_e2_r2_6;
        tsf[24] = mso_pcg_ack.mso_e2_r2_7;
        tsf[25] = mso_pcg_ack.mso_e2_r2_8;
        tsf[26] = mso_pcg_ack.mso_e2_r2_9;
        tsf[27] = mso_pcg_ack.mso_e2_r2_10;
        tsf[28] = mso_pcg_ack.mso_e2_r2_11;
        tsf[29] = mso_pcg_ack.mso_e2_r2_12;
        tsf[30] = mso_pcg_ack.mso_e2_r2_13;
        tsf[31] = mso_pcg_ack.mso_e2_r2_14;

        tsf[32] = mso_pcg_ack.mso_e4_r4_1;
        tsf[33] = mso_pcg_ack.mso_e4_r4_2;
        tsf[34] = mso_pcg_ack.mso_e4_r4_3;
        tsf[35] = mso_pcg_ack.mso_e4_r4_4;
        tsf[36] = mso_pcg_ack.mso_e4_r4_5;
        tsf[37] = mso_pcg_ack.mso_e4_r4_6;
        tsf[38] = mso_pcg_ack.mso_e4_r4_7;
        tsf[39] = mso_pcg_ack.mso_e4_r4_8;
        tsf[40] = mso_pcg_ack.mso_e4_r4_9;
        tsf[41] = mso_pcg_ack.mso_e4_r4_10;
        tsf[42] = mso_pcg_ack.mso_e4_r4_11;
        tsf[43] = mso_pcg_ack.mso_e4_r4_12;
        tsf[44] = mso_pcg_ack.mso_e4_r4_13;
        tsf[45] = mso_pcg_ack.mso_e4_r4_14;

        tsf[46] = mso_pcg_ack.mso_e8_r8_1;
        tsf[47] = mso_pcg_ack.mso_e8_r8_2;
        tsf[48] = mso_pcg_ack.mso_e8_r8_3;
        tsf[49] = mso_pcg_ack.mso_e8_r8_4;
        tsf[50] = mso_pcg_ack.mso_e8_r8_5;
        tsf[51] = mso_pcg_ack.mso_e8_r8_6;
        tsf[52] = mso_pcg_ack.mso_e8_r8_7;
        tsf[53] = mso_pcg_ack.mso_e8_r8_8;
        tsf[54] = mso_pcg_ack.mso_e8_r8_9;
        tsf[55] = mso_pcg_ack.mso_e8_r8_10;
        tsf[56] = mso_pcg_ack.mso_e8_r8_11;
        tsf[57] = mso_pcg_ack.mso_e8_r8_12;
        tsf[58] = mso_pcg_ack.mso_e8_r8_13;
        tsf[59] = mso_pcg_ack.mso_e8_r8_14;
        len = 60;
      }
#endif /* FEATURE_IS2000_1X_ADV */

      break;

    case 2: /* Same Rate Transmitted Counters */
      tsf[0] = mso_tx_save.mso_e1_t1;
      tsf[1] = mso_tx_save.mso_e2_t2;
      tsf[2] = mso_tx_save.mso_e4_t4;
      tsf[3] = mso_tx_save.mso_e8_t8;
      len = 4;

#ifdef FEATURE_IS2000_1X_ADV
      if (MAR_IS_EMSO)
      {
        tsf[4] = mso_tx_save.mso_enc_t8;
        tsf[5] = mso_tx_save.mso_enc_t0;
        len = 6;
      }
#endif /* FEATURE_IS2000_1X_ADV */

      break;

    case 3: /* Transmitted Counters */
      i = 0;
      tsf[i++] = mso_tx_save.mso_e1_t1;
      tsf[i++] = mso_tx_save.mso_e2_t2;
      tsf[i++] = mso_tx_save.mso_e4_t4;
      tsf[i++] = mso_tx_save.mso_e8_t8;

#ifdef FEATURE_IS2000_1X_ADV
      if (MAR_IS_EMSO)
      {
        tsf[i++] = mso_tx_save.mso_enc_t8;
        tsf[i++] = mso_tx_save.mso_enc_t0;
      }
#endif /* FEATURE_IS2000_1X_ADV */

      tsf[i++] = mso_tx_save.mso_e1_td;
      tsf[i++] = mso_tx_save.mso_e2_td;
      tsf[i++] = mso_tx_save.mso_e4_td;
      tsf[i++] = mso_tx_save.mso_e8_td;

#ifdef FEATURE_IS2000_1X_ADV
      if (MAR_IS_EMSO)
      {
        tsf[i++] = mso_tx_save.mso_enc_td;
      }
#endif /* FEATURE_IS2000_1X_ADV */

      tsf[i++] = mso_tx_save.mso_e1_tb;
      tsf[i++] = mso_tx_save.mso_e2_tb;
      tsf[i++] = mso_tx_save.mso_e4_tb;
      tsf[i++] = mso_tx_save.mso_e8_tb;

#ifdef FEATURE_IS2000_1X_ADV
      if (MAR_IS_EMSO)
      {
        tsf[i++] = mso_tx_save.mso_enc_tb;
      }
#endif /* FEATURE_IS2000_1X_ADV */

      len = (byte) i;
      break;

    case 4: /* Rate 1 Expected Counters */
      tsf[0] = mso_rx.mso_e1_r1;
      tsf[1] = mso_rx.mso_e1_rd;
      tsf[2] = 0L;
      tsf[3] = mso_rx.mso_e1_r2;
      tsf[4] = mso_rx.mso_e1_r4;
      tsf[5] = mso_rx.mso_e1_r8;
      tsf[6] = 0L;
      tsf[7] = mso_rx.mso_e1_re;
      tsf[8] = good_errs [MAR_1_RATE];
      len = 9;

#ifdef FEATURE_IS2000_1X_ADV
      if (MAR_IS_EMSO)
      {
        tsf[9] = mso_pcg_ack.mso_e1_r1_1;
        tsf[10] = mso_pcg_ack.mso_e1_r1_2;
        tsf[11] = mso_pcg_ack.mso_e1_r1_3;
        tsf[12] = mso_pcg_ack.mso_e1_r1_4;
        tsf[13] = mso_pcg_ack.mso_e1_r1_5;
        tsf[14] = mso_pcg_ack.mso_e1_r1_6;
        tsf[15] = mso_pcg_ack.mso_e1_r1_7;
        tsf[16] = mso_pcg_ack.mso_e1_r1_8;
        tsf[17] = mso_pcg_ack.mso_e1_r1_9;
        tsf[18] = mso_pcg_ack.mso_e1_r1_10;
        tsf[19] = mso_pcg_ack.mso_e1_r1_11;
        tsf[20] = mso_pcg_ack.mso_e1_r1_12;
        tsf[21] = mso_pcg_ack.mso_e1_r1_13;
        tsf[22] = mso_pcg_ack.mso_e1_r1_14;
        len = 23;
      }
#endif /* FEATURE_IS2000_1X_ADV */

      break;

    case 5: /* Rate 1/2 Expected Counters */
      tsf[0] = mso_rx.mso_e2_r1;
      tsf[1] = mso_rx.mso_e2_rd;
      tsf[2] = 0L;
      tsf[3] = mso_rx.mso_e2_r2;
      tsf[4] = mso_rx.mso_e2_r4;
      tsf[5] = mso_rx.mso_e2_r8;
      tsf[6] = 0L;
      tsf[7] = mso_rx.mso_e2_re;
      tsf[8] = good_errs [MAR_2_RATE];
      len = 9;

#ifdef FEATURE_IS2000_1X_ADV
      if (MAR_IS_EMSO)
      {
        tsf[9] = mso_pcg_ack.mso_e2_r2_1;
        tsf[10] = mso_pcg_ack.mso_e2_r2_2;
        tsf[11] = mso_pcg_ack.mso_e2_r2_3;
        tsf[12] = mso_pcg_ack.mso_e2_r2_4;
        tsf[13] = mso_pcg_ack.mso_e2_r2_5;
        tsf[14] = mso_pcg_ack.mso_e2_r2_6;
        tsf[15] = mso_pcg_ack.mso_e2_r2_7;
        tsf[16] = mso_pcg_ack.mso_e2_r2_8;
        tsf[17] = mso_pcg_ack.mso_e2_r2_9;
        tsf[18] = mso_pcg_ack.mso_e2_r2_10;
        tsf[19] = mso_pcg_ack.mso_e2_r2_11;
        tsf[20] = mso_pcg_ack.mso_e2_r2_12;
        tsf[21] = mso_pcg_ack.mso_e2_r2_13;
        tsf[22] = mso_pcg_ack.mso_e2_r2_14;
        len = 23;
      }
#endif /* FEATURE_IS2000_1X_ADV */

      break;

    case 6: /* Rate 1/4 Expected Counters */
      tsf[0] = mso_rx.mso_e4_r1;
      tsf[1] = mso_rx.mso_e4_rd;
      tsf[2] = 0L;
      tsf[3] = mso_rx.mso_e4_r2;
      tsf[4] = mso_rx.mso_e4_r4;
      tsf[5] = mso_rx.mso_e4_r8;
      tsf[6] = 0L;
      tsf[7] = mso_rx.mso_e4_re;
      tsf[8] = good_errs [MAR_4_RATE];
      len = 9;

#ifdef FEATURE_IS2000_1X_ADV
      if (MAR_IS_EMSO)
      {
        tsf[9] = mso_pcg_ack.mso_e4_r4_1;
        tsf[10] = mso_pcg_ack.mso_e4_r4_2;
        tsf[11] = mso_pcg_ack.mso_e4_r4_3;
        tsf[12] = mso_pcg_ack.mso_e4_r4_4;
        tsf[13] = mso_pcg_ack.mso_e4_r4_5;
        tsf[14] = mso_pcg_ack.mso_e4_r4_6;
        tsf[15] = mso_pcg_ack.mso_e4_r4_7;
        tsf[16] = mso_pcg_ack.mso_e4_r4_8;
        tsf[17] = mso_pcg_ack.mso_e4_r4_9;
        tsf[18] = mso_pcg_ack.mso_e4_r4_10;
        tsf[19] = mso_pcg_ack.mso_e4_r4_11;
        tsf[20] = mso_pcg_ack.mso_e4_r4_12;
        tsf[21] = mso_pcg_ack.mso_e4_r4_13;
        tsf[22] = mso_pcg_ack.mso_e4_r4_14;
        len = 23;
      }
#endif /* FEATURE_IS2000_1X_ADV */

      break;

    case 7: /* Rate 1/8 Expected Counters */
      tsf[0] = mso_rx.mso_e8_r1;
      tsf[1] = mso_rx.mso_e8_rd;
      tsf[2] = 0L;
      tsf[3] = mso_rx.mso_e8_r2;
      tsf[4] = mso_rx.mso_e8_r4;
      tsf[5] = mso_rx.mso_e8_r8;
      tsf[6] = 0L;
      tsf[7] = mso_rx.mso_e8_re;
      tsf[8] = good_errs [MAR_8_RATE];
      len = 9;

#ifdef FEATURE_IS2000_1X_ADV
      if (MAR_IS_EMSO)
      {
        tsf[9] = mso_pcg_ack.mso_e8_r8_1;
        tsf[10] = mso_pcg_ack.mso_e8_r8_2;
        tsf[11] = mso_pcg_ack.mso_e8_r8_3;
        tsf[12] = mso_pcg_ack.mso_e8_r8_4;
        tsf[13] = mso_pcg_ack.mso_e8_r8_5;
        tsf[14] = mso_pcg_ack.mso_e8_r8_6;
        tsf[15] = mso_pcg_ack.mso_e8_r8_7;
        tsf[16] = mso_pcg_ack.mso_e8_r8_8;
        tsf[17] = mso_pcg_ack.mso_e8_r8_9;
        tsf[18] = mso_pcg_ack.mso_e8_r8_10;
        tsf[19] = mso_pcg_ack.mso_e8_r8_11;
        tsf[20] = mso_pcg_ack.mso_e8_r8_12;
        tsf[21] = mso_pcg_ack.mso_e8_r8_13;
        tsf[22] = mso_pcg_ack.mso_e8_r8_14;
        len = 23;
      }
#endif /* FEATURE_IS2000_1X_ADV */

      break;

#ifdef FEATURE_IS2000_1X_ADV
    case 8: /* Rate 0 Expected Counters */
      tsf[0] = mso_rx.mso_e0_r1;
      tsf[1] = mso_rx.mso_e0_rd;
      tsf[2] = mso_rx.mso_e0_r2;
      tsf[3] = mso_rx.mso_e0_r4;
      tsf[4] = mso_rx.mso_e0_r8;
      tsf[5] = 0L;
      tsf[6] = mso_rx.mso_e0_re;
      len = 7;
      break;

    case 9: /* R-ACKCH1 Transmitted Counters */
      tsf[0] = mso_tx_achch1_save.mso_rackch1_tack;
      tsf[1] = mso_tx_achch1_save.mso_rackch1_tnak;
      len = 2;
      break;

    case 10: /* F-ACKCH Transmitted Counters */
      tsf[0] = mso_rx_achch.mso_fackch_eack_rack;
      tsf[1] = mso_rx_achch.mso_fackch_eack_rnak;
      tsf[2] = mso_rx_achch.mso_fackch_enak_rnak;
      tsf[3] = mso_rx_achch.mso_fackch_enak_rack;
      len = 4;
      break;
#endif /* FEATURE_IS2000_1X_ADV */

    default:
      len = 0;
   }
   return (len);

}
