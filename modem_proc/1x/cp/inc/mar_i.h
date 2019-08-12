#ifndef MAR_I_H
#define MAR_I_H
/*===========================================================================

                   M A R    H E A D E R    F I L E

DESCRIPTION
  This file contains declarations associated with Markov Service Option
  services.

  Copyright (c) 1991 - 2013 Qualcomm Technologies, Inc. 
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

$PVCSPath: L:/src/asw/MSM5100/CP_REL_A/vcs/mar.h_v   1.0.2.0   30 Nov 2001 16:58:06   fchan  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/1x/cp/inc/mar_i.h#1 $ $DateTime: 2016/12/13 08:00:08 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
01/23/12   srk     Feature Cleanup
06/14/11   ag      Fix to initialize MSO according to the actual SO connected
                   in traffic.
04/22/11   jtm     CMI Phase 4 - Final SU API changes.
02/10/11   ag      Increased size of Markov statistics variables for eMSO to
                   32 bits from 16 bits to support long duration testing.
11/15/10   jtm     Initial SU API split for CP.
10/08/10   ag      Renamed Markov, Loopback related counters.
09/07/10   ag      Moved MAR_IS_EMSO to mar.h.
07/27/10   ag      Removed support for DTX mode for SMS SO with RC8.
06/15/10   ag      Support to set Markov in DTX mode when SMS SO is with RC8.
06/15/10   ag      Added support for F-R/ACKCH control directives for Markov.
06/15/10   ag      Added support for RDA testing.
03/02/10   ag      Added support for checking if FW guarantees a frame or not
                   when forward RC is RC11 and rate is 1/8 non-critical.
11/06/09   ag      1X Advanced - Enhanced Markov Service Option Support
01/15/96   gb      Fixed Markov reinitialization by adding mar_init_synch.
11/20/95   gb      Added Markov retrievable parameter support.
10/12/95   gb      Added constants for service option control.
06/14/95   gb      Added support for Rate set 2 service options.
05/12/94   gb      Added new update markov probability table call.
06/14/91   gb      Created file.

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "1x_variation.h"
#include "comdef.h"
#include "cai.h"
#include "modem_1x_defs.h"

/* <EJECT> */

/*===========================================================================

                        DATA DECLARATIONS

===========================================================================*/


/*---------------------------------------------------------------------------
                        TEST MODE CODE CONSTANTS

The following constants are used with the 'test mode' field of Test Control
directive.
---------------------------------------------------------------------------*/

typedef enum
{
  MAR_VAR_RATE = 0,         /* Indicates variable rate test mode */
  MAR_8_RATE_TEST,          /* Indicates rate 1/8 */
  MAR_4_RATE_TEST,          /* Indicates rate 1/4 */
  MAR_2_RATE_TEST,          /* Indicates rate 1/2 */
  MAR_1_RATE_TEST,          /* Indicates rate 1   */
  MAR_NC_8_RATE_TEST        /* Indicates rate 1/8 non critical */
}mar_test_mode_type;

/*
 * CTL_REC_TYPE Message Field definitions
 */
#define MAR_CTL_TYP       0     /* byte index for ctl type field */
#define MAR_CTL_DIR       0     /* Control Directive in the CTL_REC_TYPE */
#define MAR_RET_DIR       1     /* Retrieve counters Directive */
#define MAR_VECT          1     /* byte index for retrieve vector id */
#ifdef FEATURE_IS2000_1X_ADV
#define MAR_MDL_DIR       2     /* Markov Model Directive */
#define MAR_SYNC          1     /* byte index for sync state field */
#define MAR_SCSTP         2     /* byte index for SCSTP table beginning */
#define MAR_CTL_RACK1_DIR 3     /* Test Control Directive for R-ACKCH1 */
#define MAR_CTL_FACK_DIR  4     /* Test Control Directive for F-ACKCH */
#define MAR_ACK_OR_NAK    1     /* byte index for ACK_OR_NAK field */
#endif /* FEATURE_IS2000_1X_ADV */

/*
 * Control Field definitions
 */
#define MAR_CTL 1               /* byte index for ctl field */
#define MAR_TST 0xE0            /* type of Markov test field */
#define MAR_TST_SHFT 5          /* # of bits to shift to use field */
#define MAR_COPY 0x10           /* copy counters bit field */
#define MAR_ZERO 0x08           /* zero counters bit field */


/*---------------------------------------------------------------------------
                        FER DATA RATE CONSTANTS

The following constants are used with the 'rate' fields of FER transmit
and receive routine parameters.
---------------------------------------------------------------------------*/

typedef enum
{
  MAR_NC_8_RATE = 0,   /* Indicates rate 1/8 non-critical data. */
  MAR_8_RATE,          /* Indicates rate 1/8 data. */
  MAR_4_RATE,          /* Indicates rate 1/4 data. */
  MAR_2_RATE,          /* Indicates rate 1/2 data. */
  MAR_1_RATE,          /* Indicates rate 1   data. */
  MAR_0_RATE           /* Indicates rate 0 (DTX) data. */
}mar_rate_type;

/*---------------------------------------------------------------------------
                        MAR ACUMULATOR STRUCTURE

The following structure is used to accumulate the different frame errors.
---------------------------------------------------------------------------*/

typedef struct
{
  dword no_err;     /* data was rx no error, correct rate.       */
  dword err;        /* data was rx in error, correct rate.       */
  dword rate1;      /* data was rx in error, incorrect rate hi.  */
  dword rate2;      /* data was rx in error, incorrect rate mid. */
  dword rate3;      /* data was rx in error, incorrect rate lo.  */
  dword erase;      /* erasures */
  dword frames;     /* total frames received */
  dword berrs;      /* total bits in error */
} mar_type;


/*---------------------------------------------------------------------------
                        MAR STATISTICS TYPE

The following structure is used to display Markov statistics.
---------------------------------------------------------------------------*/

typedef struct
{
  mar_test_mode_type m_rate;  /* current Markov test mode */
  dword bit_errs;        /* current mar_bit_errs */
  dword bad_frames;      /* number of frames with errors */
  dword total_frames;    /* total number of frames */
  #ifdef FEATURE_IS2000_1X_ADV
  dword mar_test[ 5 ] [ 10 ];
    /* full, half, quarter, eighth and zero rate entries from mar_test array */
  dword good_errs [4];    /* good rate frames with bit errs for 4 rates */
  #else
  word mar_test[ 4 ] [ 10 ];
    /* full, half, quarter and eighth rate entries from mar_test array */
  word good_errs [4];    /* good rate frames with bit errs for 4 rates */
  #endif /* FEATURE_IS2000_1X_ADV */
} mar_stat_type;

#ifdef FEATURE_MODEM_1X_MSO_RDA_TEST
/*---------------------------------------------------------------------------
The following structure is used to display rate of expected frames versus
rate marked by RDA with respect to the presence or absence of bit errors.
---------------------------------------------------------------------------*/
typedef struct
{
  mar_test_mode_type m_rate;  /* current Markov test mode */

  dword mar_good_frames[ 5 ] [ 5 ];
    /* number of frames without any bit errors.
     * five rows represent expected full, half, quarter, eighth and zero rate frames
     * and five columns represent RDA-marked full rate, half rate, quarter rate,
     * eighth rate and erasure frames */

  dword mar_bad_frames[ 5] [ 5 ];
    /* number frames with bit errors.
     * five rows represent expected full, half, quarter, eighth and zero rate frames
     * and five columns represent RDA-marked full rate, half rate, quarter rate,
     * eighth rate and erasure frames */

} mar_rda_test_stat_type;
#endif /* FEATURE_MODEM_1X_MSO_RDA_TEST */

#define  MAR_192_BITS  192  /* Bits per rate 1   frame */
#define  MAR_96_BITS   96   /* Bits per rate 1/2 frame */
#define  MAR_48_BITS   48   /* Bits per rate 1/4 frame */
#define  MAR_24_BITS   24   /* Bits per rate 1/8 frame */

typedef struct
{
  byte rate1[ MAR_192_BITS / 8 ];  /* Rate 1   data bits */
  byte rate2[ MAR_96_BITS  / 8 ];  /* Rate 1/2 data bits */
  byte rate4[ MAR_48_BITS  / 8 ];  /* Rate 1/4 data bits */
  byte rate8[ MAR_24_BITS  / 8 ];  /* Rate 1/8 data bits */
}
mar_bits_type;
  /* Format of byte data read from decoder. */

typedef struct
{
  dword mso_e1_t1;
  dword mso_e2_t2;
  dword mso_e4_t4;
  dword mso_e8_t8;
  dword mso_enc_t8;
  dword mso_enc_t0;
  dword mso_e1_td;
  dword mso_e2_td;
  dword mso_e4_td;
  dword mso_e8_td;
  dword mso_enc_td;
  dword mso_e1_tb;
  dword mso_e2_tb;
  dword mso_e4_tb;
  dword mso_e8_tb;
  dword mso_enc_tb;
} mso_e_tx_type;

#ifdef FEATURE_IS2000_1X_ADV
typedef struct
{
   dword mso_rackch1_tack;
   /* Counter for the number of ACK’s transmitted on the Reverse ACK CH 1 */
   dword mso_rackch1_tnak;
   /* Counter for the number of NAK’s transmitted on the Reverse ACK CH 1 */
} mso_tx_ackch1_type;
#endif /* FEATURE_IS2000_1X_ADV */

typedef struct
{
  dword mso_eb_rd;
  dword mso_eb_r8;
  dword mso_eb_r4;
  dword mso_eb_r2;
  dword mso_eb_r1;
  dword mso_eb_re;

  dword mso_e8_rd;
  dword mso_e8_r8;
  dword mso_e8_r4;
  dword mso_e8_r2;
  dword mso_e8_r1;
  dword mso_e8_re;

  dword mso_e4_rd;
  dword mso_e4_r8;
  dword mso_e4_r4;
  dword mso_e4_r2;
  dword mso_e4_r1;
  dword mso_e4_re;

  dword mso_e2_rd;
  dword mso_e2_r8;
  dword mso_e2_r4;
  dword mso_e2_r2;
  dword mso_e2_r1;
  dword mso_e2_re;

  dword mso_e1_rd;
  dword mso_e1_r8;
  dword mso_e1_r4;
  dword mso_e1_r2;
  dword mso_e1_r1;
  dword mso_e1_re;

  dword mso_e0_rd;
  dword mso_e0_r8;
  dword mso_e0_r4;
  dword mso_e0_r2;
  dword mso_e0_r1;
  dword mso_e0_re;
} mso_e_rx_type;

#ifdef FEATURE_IS2000_1X_ADV
typedef struct
{
  dword mso_e8_r8_1;
  dword mso_e8_r8_2;
  dword mso_e8_r8_3;
  dword mso_e8_r8_4;
  dword mso_e8_r8_5;
  dword mso_e8_r8_6;
  dword mso_e8_r8_7;
  dword mso_e8_r8_8;
  dword mso_e8_r8_9;
  dword mso_e8_r8_10;
  dword mso_e8_r8_11;
  dword mso_e8_r8_12;
  dword mso_e8_r8_13;
  dword mso_e8_r8_14;

  dword mso_e4_r4_1;
  dword mso_e4_r4_2;
  dword mso_e4_r4_3;
  dword mso_e4_r4_4;
  dword mso_e4_r4_5;
  dword mso_e4_r4_6;
  dword mso_e4_r4_7;
  dword mso_e4_r4_8;
  dword mso_e4_r4_9;
  dword mso_e4_r4_10;
  dword mso_e4_r4_11;
  dword mso_e4_r4_12;
  dword mso_e4_r4_13;
  dword mso_e4_r4_14;

  dword mso_e2_r2_1;
  dword mso_e2_r2_2;
  dword mso_e2_r2_3;
  dword mso_e2_r2_4;
  dword mso_e2_r2_5;
  dword mso_e2_r2_6;
  dword mso_e2_r2_7;
  dword mso_e2_r2_8;
  dword mso_e2_r2_9;
  dword mso_e2_r2_10;
  dword mso_e2_r2_11;
  dword mso_e2_r2_12;
  dword mso_e2_r2_13;
  dword mso_e2_r2_14;

  dword mso_e1_r1_1;
  dword mso_e1_r1_2;
  dword mso_e1_r1_3;
  dword mso_e1_r1_4;
  dword mso_e1_r1_5;
  dword mso_e1_r1_6;
  dword mso_e1_r1_7;
  dword mso_e1_r1_8;
  dword mso_e1_r1_9;
  dword mso_e1_r1_10;
  dword mso_e1_r1_11;
  dword mso_e1_r1_12;
  dword mso_e1_r1_13;
  dword mso_e1_r1_14;
} mso_rx_pcg_ack_type;

typedef struct
{
  dword mso_fackch_eack_rack; /* F-ACKCH expected ACK received ACK */
  dword mso_fackch_eack_rnak; /* F-ACKCH expected ACK received NAK */
  dword mso_fackch_enak_rack; /* F-ACKCH expected NAK received ACK */
  dword mso_fackch_enak_rnak; /* F-ACKCH expected NAK received NAK */
} mso_rx_ackch_type;
#endif /* FEATURE_IS2000_1X_ADV */

/*===========================================================================

                      FUNCTION DECLARATIONS

===========================================================================*/

/* The connected Markov service option */
extern word mar_so;

/* Macro to check if the connected SO is enhanced Markov Service Option */
#define MAR_IS_EMSO ( mar_so == CAI_SO_MARKOV_SO74 )

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
);

/* <EJECT> */
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
);

/* <EJECT> */
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
  are not in sync this procedure returns 0xFFFF,  indicating that the value
  should not be logged.

SIDE EFFECTS
  Some of the error counters in the structure mar_test may be incremented.

===========================================================================*/

word  mar_rx (
byte *mar_rx_data,                       /* pointer to rx dec buffer */
byte rx_rate,                            /* rate decision from rx */
dword frame32,                           /* ls 32 bits of frame # */
byte *mar_mask_ptr                       /* mask of bit errors */
#ifdef FEATURE_MODEM_1X_MSO_RDA_TEST
,byte *mar_tx_data,                      /* Reference TX frame */
byte exp_rate                            /* Rate of expected frame */
#endif /* FEATURE_MODEM_1X_MSO_RDA_TEST */
#ifdef FEATURE_IS2000_1X_ADV
,int ack_pcg_index,                      /* PCG index of ACK */
boolean valid_frame                      /* TRUE if frame is guaranteed, FALSE otherwise */
#endif /* FEATURE_IS2000_1X_ADV */
);

/* <EJECT> */
/*===========================================================================

FUNCTION MAR_GET

DESCRIPTION
  This function accepts a pointer to the bit error rate accumulator
  structure to be filled from the current test statistics.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  The structure pointed to by mar_ptr willed be filled with the current
    error statistics.

===========================================================================*/

void mar_get
(
mar_type *mar_ptr             /* pointer to structure defined in mar.h */
);

/* <EJECT> */
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
);

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
  mar_rda_test_stat_type *mar_ptr
    /* pointer to structure defined in mar.h */
);
#endif /* FEATURE_MODEM_1X_MSO_RDA_TEST */

/* <EJECT> */
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
  The local global variables tri_bytes, last_mask, bytes, last_byte
  are initialized and the structure test may be zeroed.

===========================================================================*/

void mar_init
(
word test_rate ,             /* data rate to generate and test data at */
boolean clear_count,         /* flag TRUE = clear bit error counters */
boolean copy_count,          /* flag TRUE = copy bit error counters */
dword esn,                   /* Serial number of phone   */
boolean rate_set   /* rate set 1 or 2 */
);

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
);

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
);
#endif /* FEATURE_IS2000_1X_ADV */

/* <EJECT> */
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
);

/* <EJECT> */
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

void mar_log_tx (word mux);

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
void mar_log_rx(word mux);

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
);

/* <EJECT> */
/*===========================================================================

FUNCTION MAR_INIT_SYNCH

DESCRIPTION
 Initialize the state of the Markov model to unsynched.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  The local global boolean variable mar_in_sync is cleared.

===========================================================================*/
void mar_init_synch(void);

#endif /* MAR_I_H */

