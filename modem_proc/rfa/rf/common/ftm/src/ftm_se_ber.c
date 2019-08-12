/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

GENERAL DESCRIPTION

EXTERNALIZED FUNCTIONS


INITIALIZATION AND SEQUENCING REQUIREMENTS

  Copyright (c) 2007 - 2010 by Qualcomm Technologies INCORPORATED. All Rights Reserved.
  Copyright (c) 2014 by Qualcomm Technologies INCORPORATED. All Rights Reserved.

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/
/*===========================================================================

                           EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfa/rf/common/ftm/src/ftm_se_ber.c#1 $
  $DateTime: 2016/12/13 07:59:52 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
04/10/14   zbz     Fix KW
06/24/13   sar     Replaced memcpy calls with safe memscpy.
05/18/12   hy      re-run once on same data sequence if the first time lost PN synth 
03/02/12   aa      Correct initialization of SE BER Configuration 
01/28/12   sar     Feature reduction changes.
12/14/11   sr      MCS1-4 SE-BER feature support.
03/08/11   ems     WCDMA FTM - Fix for Synchronous diagnostic SE BER/BLER 
11/09/10   lcl     Remove compiler warning
10/21/10   ems     Factory features Rx Lvl, non-signaling events and SE/BER
08/31/10   rm      Reverting KW fix which caused SE BER to fail
04/01/10   adk     Merged branch scmm_mdm9k_merge_final
03/24/10   ka      WPLT compilation fix.
02/12/10   xw      Klocwork fix for potential buffer overflow.
01/06/10   sar     Renamed ftm_task.h to rf_test_task_v.h.
11/11/09   jfc     Lint fix: Check for out of bounds index
10/15/09   ckl     Fixed SEBER measurement failure caused by initial slot_num
09/08/09   pl      Remove FEATURE_FTM_HAS_LTE, featurize dec.h inclusion with 
                   CDMA1x and WCDMA specific features
08/11/09   mkv     Merge MODEM_LTE_DEV into rfa2
07/08/09   dyc     Added dec*.h with FEATURE_CMI
04/10/07   ad      Declare read-only tables as const to save R/W memory
03/06/08   jtn     Fix Klocwork errors.
02/22/08   dw      Fixe high Lint warnings(578)
01/22/08   jfc     Parenthesize param 1 in macro FTM_SE_BER_GET_BITMASK.
                   Fix bound check in se_ber_acquire_run()
10/22/07   ad      Merge SE BER support from 6260: GSM Class Ia,Ib/II bit class
                   rearranging and EDGE SRB support to handle MCS 5-9
04/25/07   dp      Added GSM Loop A/B support, and Loop C formatting (remove
                   stealing bits).
01/23/07   avi     Created
===========================================================================*/


/*===========================================================================

          INCLUDE FILES FOR MODULE

===========================================================================*/

#include "rfa_variation.h"
#include "stringl.h" /* for memscpy */
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "target.h"
#include "customer.h"

#include "ftm.h"
#include "rf_test_task_v.h"
#include "ftm_se_ber.h"

#if(defined(FEATURE_CDMA1X) || defined(FEATURE_WCDMA))
#ifdef FEATURE_CMI
#include "dec5000.h"
#else
#ifndef FEATURE_WPLT
#include "dec.h"
#endif
#endif /* FEATURE_CMI */
#endif /* FEATURE_CDMA1X || FEATURE_WCDMA */

#include "err.h"
#include "msg.h"
#include "ftm_log.h"

#ifdef FEATURE_FACTORY_TESTMODE
#ifdef FTM_HAS_SE_BER
/*===========================================================================

                         EXTERNAL GLOBAL VARIABLES

===========================================================================*/

/*===========================================================================

                               MACROS

===========================================================================*/

#define FTM_SE_BER_MIN_REPORT_SAMPLES     8
#define FTM_SE_BER_MAX_REPORT_SAMPLES     64
#define FTM_SE_BER_NUM_BITS_GSM_LOOP_AB   260   /* 260 bits for full rate speech */
#define FTM_SE_BER_NUM_CLASS_IA_BITS      50    /* FR speech only */
#define FTM_SE_BER_NUM_CLASS_IB_BITS      132   /* FR speech only */
#define FTM_SE_BER_NUM_CLASS_II_BITS      78    /* FR speech only */
#define FTM_SE_BER_NUM_PN_SEQ             8     /* number of indpendent PN sequences that we can track
                                                 * at a time.  We will have 1 for each
                                                 * available timeslot.  It is assumed that
                                                 * the SAME PN generator is used for each
                                                 * timeslot, although each sequence can have its
                                                 * own phase (starting position of the PN sequence).
                                                 */

/* this macro gets the bit mask for setting/clearing bit x in a 32 bit word, left aligned
 * that is, x = 0 returns 0x80000000, x=1 returns 0x40000000, etc.
 */
#define FTM_SE_BER_GET_BITMASK( x)        (( uint32 ) ( 0x1 << (MAX_BIT_COUNT - 1 - (x))))
/*===========================================================================

                    LOCAL GLOBAL VARIABLES AND CONSTANTS

===========================================================================*/

/*
  pn_sequence data type to hold all the data necessary to generate the next state
*/

typedef struct
{
  uint32 gen_polynomial;                        /* Generating polynomial for the PN sequence*/
  uint32 sr_state;                              /* State of the linear shift register       */
  se_ber_pn_length_type gen_polynomial_degree;  /* Enumeration defining the order of the generator polynomial */
  uint32 high_bit_value;                        /* The value to be added to the LSR in case the gen.poly output is 1 */
  uint32 sr_block_output;                       /* The block output value PN seq generator, right aligned */
  boolean invert_bit;                           /* indicate if sequence output should be inverted */
} se_ber_pn_sequence_type;

typedef struct
{
  boolean                   initialized;
  uint16                    acq_thresh;
  boolean                   invert;
  se_ber_mode_type          mode;
  ftm_se_ber_loop_ab_order_type  loop_ab_order;
  ftm_se_ber_mcs_type       expected_mcs;
  se_ber_pn_sequence_type   pn_seq[FTM_SE_BER_NUM_PN_SEQ];
  boolean                   acquired[FTM_SE_BER_NUM_PN_SEQ];
  uint16                    bit_count[3];    /* handle up to 3 bit classes */
  uint16                    error_count[3];
  uint16                    lost_acq_water_mark;
} se_ber_config_type;


typedef struct
{

  boolean                                 log_state;
  uint8                                   report_index;
  uint8                                   report_limit;
  ftm_log_se_ber_sample_type              samples[ FTM_SE_BER_MAX_REPORT_SAMPLES];
  ftm_log_se_ber_gsm_loop_ab_sample_type  samples_loop_ab[FTM_SE_BER_MAX_REPORT_SAMPLES];

} se_ber_log_status_type;

/*
  bit_count_lut contains bit counts in integers from 0 to 16.
  The integer serves as a table index; the array element contains the
  actual bit count
*/

static const uint8  bit_count_lut[] = {0, 1, 1, 2, 1, 2, 2, 3, 1, 2, 2, 3, 2, 3, 3, 4};


static const uint32 pn_generating_polynomial[] =
{
  0x00000001, /* FTM_SE_PN_TYPE_PN1 = 0 */
  0x00000011, /* FTM_SE_PN_TYPE_PN9 */
  0x00000005, /* FTM_SE_PN_TYPE_PN11 */ /* polinomial needs verifciation */
  0x00000003  /* FTM_SE_PN_TYPE_PN15 */
};

static const uint8 gen_poly_degree_lut[] = { 1, 9, 11, 15};

static se_ber_config_type ber_state_machine =
{
  FALSE,
  0,
  FALSE,
  FTM_SE_MODE_MAX,
  FTM_SE_BER_LOOP_AB_ORDER_CH_CODEC_INPUT,
  FTM_SE_BER_MCS5,
  {
    { 0,0,FTM_SE_PN_TYPE_MAX,0,0,FALSE },
    { 0,0,FTM_SE_PN_TYPE_MAX,0,0,FALSE },
    { 0,0,FTM_SE_PN_TYPE_MAX,0,0,FALSE },
    { 0,0,FTM_SE_PN_TYPE_MAX,0,0,FALSE },
    { 0,0,FTM_SE_PN_TYPE_MAX,0,0,FALSE },
    { 0,0,FTM_SE_PN_TYPE_MAX,0,0,FALSE },
    { 0,0,FTM_SE_PN_TYPE_MAX,0,0,FALSE },
    { 0,0,FTM_SE_PN_TYPE_MAX,0,0,FALSE },
  },
  { FALSE,FALSE,FALSE,FALSE,FALSE,FALSE,FALSE,FALSE},
  {0,0,0},
  {0,0,0},
  0
};

static se_ber_log_status_type  ber_log_status = {0};


typedef enum
{
  FTM_CLASS_IA = 0,
  FTM_CLASS_IB,
  FTM_CLASS_II,
  FTM_CLASS_MAX
} ftm_se_ber_bit_class_type;


/* Maps the decoded data delivered by the physical layer to the bit class.  The decoded data is
 * arranged according to vocoder parameters, not according to bit class.  For example, the first
 * 6 bits (LSB to MSB) delivered by the physical layer for fullrate mode is the LAR1 vocoder param.  The first 6 entries
 * of this array define the bit class of each bit of the LAR1 param, from LAR1(bit1) to LAR1(bit6).  See
 * Table 1.1 of 46.010 and Table 2 of 45.003.  This table applies to FR speech only */

static ftm_se_ber_bit_class_type  ftm_se_ber_fr_bit_class_mapping[] = 
{
  /* params 1-8 */
  FTM_CLASS_II, FTM_CLASS_IB, FTM_CLASS_IA, FTM_CLASS_IA, FTM_CLASS_IA, FTM_CLASS_IA, /* LAR1 bits 1,2,3,4,5,6 */
  FTM_CLASS_II, FTM_CLASS_II, FTM_CLASS_IB, FTM_CLASS_IA, FTM_CLASS_IA, FTM_CLASS_IA, /* LAR2 */
  FTM_CLASS_II, FTM_CLASS_II, FTM_CLASS_IB, FTM_CLASS_IA, FTM_CLASS_IA,               /* LAR3 */
  FTM_CLASS_II, FTM_CLASS_II, FTM_CLASS_IB, FTM_CLASS_IA, FTM_CLASS_IA,               /* LAR4 */
  FTM_CLASS_II, FTM_CLASS_IB, FTM_CLASS_IB, FTM_CLASS_IA,                             /* LAR5 */
  FTM_CLASS_II, FTM_CLASS_II, FTM_CLASS_IB, FTM_CLASS_IA,                             /* LAR6 */
  FTM_CLASS_II, FTM_CLASS_IB, FTM_CLASS_IA,                                           /* LAR7 */
  FTM_CLASS_II, FTM_CLASS_II, FTM_CLASS_IB,                                           /* LAR8 */

  /* subframe 1, params 9-25 */
  FTM_CLASS_IB, FTM_CLASS_IA, FTM_CLASS_IA, FTM_CLASS_IA, FTM_CLASS_IA, FTM_CLASS_IA, FTM_CLASS_IA, /* LTP Lag */
  FTM_CLASS_IB, FTM_CLASS_IB,                                                         /* LTP Gain */
  FTM_CLASS_IB, FTM_CLASS_IB,                                                         /* RPE Grid Position */
  FTM_CLASS_II, FTM_CLASS_IB, FTM_CLASS_IB, FTM_CLASS_IA, FTM_CLASS_IA, FTM_CLASS_IA, /* Block Amplitude */
  FTM_CLASS_II, FTM_CLASS_IB, FTM_CLASS_IB,                                           /* RPE Pulse 1, SubFrame 1 */
  FTM_CLASS_II, FTM_CLASS_IB, FTM_CLASS_IB,                                           /* RPE Pulse 2, SubFrame 1 */
  FTM_CLASS_II, FTM_CLASS_IB, FTM_CLASS_IB,                                           /* RPE Pulse 3, SubFrame 1 */
  FTM_CLASS_II, FTM_CLASS_IB, FTM_CLASS_IB,                                           /* RPE Pulse 4, SubFrame 1 */
  FTM_CLASS_II, FTM_CLASS_IB, FTM_CLASS_IB,                                           /* RPE Pulse 5, SubFrame 1 */
  FTM_CLASS_II, FTM_CLASS_IB, FTM_CLASS_IB,                                           /* RPE Pulse 6, SubFrame 1 */
  FTM_CLASS_II, FTM_CLASS_IB, FTM_CLASS_IB,                                           /* RPE Pulse 7, SubFrame 1 */
  FTM_CLASS_II, FTM_CLASS_IB, FTM_CLASS_IB,                                           /* RPE Pulse 8, SubFrame 1 */
  FTM_CLASS_II, FTM_CLASS_IB, FTM_CLASS_IB,                                           /* RPE Pulse 9, SubFrame 1 */
  FTM_CLASS_II, FTM_CLASS_IB, FTM_CLASS_IB,                                           /* RPE Pulse 10, SubFrame 1 */
  FTM_CLASS_II, FTM_CLASS_IB, FTM_CLASS_IB,                                           /* RPE Pulse 11, SubFrame 1 */
  FTM_CLASS_II, FTM_CLASS_IB, FTM_CLASS_IB,                                           /* RPE Pulse 12, SubFrame 1 */
  FTM_CLASS_II, FTM_CLASS_IB, FTM_CLASS_IB,                                           /* RPE Pulse 13, SubFrame 1 */

  /* subframe 2, params 26-42 */
  FTM_CLASS_IB, FTM_CLASS_IA, FTM_CLASS_IA, FTM_CLASS_IA, FTM_CLASS_IA, FTM_CLASS_IA, FTM_CLASS_IA, /* LTP Lag */
  FTM_CLASS_IB, FTM_CLASS_IB,                                                         /* LTP Gain */
  FTM_CLASS_IB, FTM_CLASS_IB,                                                         /* RPE Grid Position */
  FTM_CLASS_II, FTM_CLASS_IB, FTM_CLASS_IB, FTM_CLASS_IA, FTM_CLASS_IA, FTM_CLASS_IA, /* Block Amplitude */
  FTM_CLASS_II, FTM_CLASS_IB, FTM_CLASS_IB,                                           /* RPE Pulse 1, SubFrame 2 */
  FTM_CLASS_II, FTM_CLASS_IB, FTM_CLASS_IB,                                           /* RPE Pulse 2, SubFrame 2 */
  FTM_CLASS_II, FTM_CLASS_IB, FTM_CLASS_IB,                                           /* RPE Pulse 3, SubFrame 2 */
  FTM_CLASS_II, FTM_CLASS_IB, FTM_CLASS_IB,                                           /* RPE Pulse 4, SubFrame 2 */
  FTM_CLASS_II, FTM_CLASS_IB, FTM_CLASS_IB,                                           /* RPE Pulse 5, SubFrame 2 */
  FTM_CLASS_II, FTM_CLASS_IB, FTM_CLASS_IB,                                           /* RPE Pulse 6, SubFrame 2 */
  FTM_CLASS_II, FTM_CLASS_IB, FTM_CLASS_IB,                                           /* RPE Pulse 7, SubFrame 2 */
  FTM_CLASS_II, FTM_CLASS_IB, FTM_CLASS_IB,                                           /* RPE Pulse 8, SubFrame 2 */
  FTM_CLASS_II, FTM_CLASS_IB, FTM_CLASS_IB,                                           /* RPE Pulse 9, SubFrame 2 */
  FTM_CLASS_II, FTM_CLASS_IB, FTM_CLASS_IB,                                           /* RPE Pulse 10, SubFrame 2 */
  FTM_CLASS_II, FTM_CLASS_IB, FTM_CLASS_IB,                                           /* RPE Pulse 11, SubFrame 2 */
  FTM_CLASS_II, FTM_CLASS_IB, FTM_CLASS_IB,                                           /* RPE Pulse 12, SubFrame 2 */
  FTM_CLASS_II, FTM_CLASS_IB, FTM_CLASS_IB,                                           /* RPE Pulse 13, SubFrame 2 */

  /* subframe 3, params 43 - 59 */
  FTM_CLASS_IB, FTM_CLASS_IA, FTM_CLASS_IA, FTM_CLASS_IA, FTM_CLASS_IA, FTM_CLASS_IA, FTM_CLASS_IA, /* LTP Lag */
  FTM_CLASS_IB, FTM_CLASS_IB,                                                         /* LTP Gain */
  FTM_CLASS_IB, FTM_CLASS_IB,                                                         /* RPE Grid Position */
  FTM_CLASS_II, FTM_CLASS_IB, FTM_CLASS_IB, FTM_CLASS_IA, FTM_CLASS_IA, FTM_CLASS_IA, /* Block Amplitude */
  FTM_CLASS_II, FTM_CLASS_IB, FTM_CLASS_IB,                                           /* RPE Pulse 1, SubFrame 3 */
  FTM_CLASS_II, FTM_CLASS_IB, FTM_CLASS_IB,                                           /* RPE Pulse 2, SubFrame 3 */
  FTM_CLASS_II, FTM_CLASS_IB, FTM_CLASS_IB,                                           /* RPE Pulse 3, SubFrame 3 */
  FTM_CLASS_II, FTM_CLASS_IB, FTM_CLASS_IB,                                           /* RPE Pulse 4, SubFrame 3 */
  FTM_CLASS_II, FTM_CLASS_IB, FTM_CLASS_IB,                                           /* RPE Pulse 5, SubFrame 3 */
  FTM_CLASS_II, FTM_CLASS_IB, FTM_CLASS_IB,                                           /* RPE Pulse 6, SubFrame 3 */
  FTM_CLASS_II, FTM_CLASS_IB, FTM_CLASS_IB,                                           /* RPE Pulse 7, SubFrame 3 */
  FTM_CLASS_II, FTM_CLASS_IB, FTM_CLASS_IB,                                           /* RPE Pulse 8, SubFrame 3 */
  FTM_CLASS_II, FTM_CLASS_IB, FTM_CLASS_IB,                                           /* RPE Pulse 9, SubFrame 3 */
  FTM_CLASS_II, FTM_CLASS_IB, FTM_CLASS_IB,                                           /* RPE Pulse 10, SubFrame 3 */
  FTM_CLASS_II, FTM_CLASS_IB, FTM_CLASS_IB,                                           /* RPE Pulse 11, SubFrame 3 */
  FTM_CLASS_II, FTM_CLASS_IB, FTM_CLASS_IB,                                           /* RPE Pulse 12, SubFrame 3 */
  FTM_CLASS_II, FTM_CLASS_IB, FTM_CLASS_IB,                                           /* RPE Pulse 13, SubFrame 3 */

  /* subframe 4, params 60 -76 */
  FTM_CLASS_IB, FTM_CLASS_IA, FTM_CLASS_IA, FTM_CLASS_IA, FTM_CLASS_IA, FTM_CLASS_IA, FTM_CLASS_IA, /* LTP Lag */
  FTM_CLASS_IB, FTM_CLASS_IB,                                                         /* LTP Gain */
  FTM_CLASS_IB, FTM_CLASS_IB,                                                         /* RPE Grid Position */
  FTM_CLASS_II, FTM_CLASS_IB, FTM_CLASS_IB, FTM_CLASS_IA, FTM_CLASS_IA, FTM_CLASS_IA, /* Block Amplitude */
  FTM_CLASS_II, FTM_CLASS_IB, FTM_CLASS_IB,                                           /* RPE Pulse 1, SubFrame 4 */
  FTM_CLASS_II, FTM_CLASS_IB, FTM_CLASS_IB,                                           /* RPE Pulse 2, SubFrame 4 */
  FTM_CLASS_II, FTM_CLASS_IB, FTM_CLASS_IB,                                           /* RPE Pulse 3, SubFrame 4 */
  FTM_CLASS_II, FTM_CLASS_IB, FTM_CLASS_IB,                                           /* RPE Pulse 4, SubFrame 4 */
  FTM_CLASS_II, FTM_CLASS_II, FTM_CLASS_IB,                                           /* RPE Pulse 5, SubFrame 4 */
  FTM_CLASS_II, FTM_CLASS_II, FTM_CLASS_IB,                                           /* RPE Pulse 6, SubFrame 4 */
  FTM_CLASS_II, FTM_CLASS_II, FTM_CLASS_IB,                                           /* RPE Pulse 7, SubFrame 4 */
  FTM_CLASS_II, FTM_CLASS_II, FTM_CLASS_IB,                                           /* RPE Pulse 8, SubFrame 4 */
  FTM_CLASS_II, FTM_CLASS_II, FTM_CLASS_IB,                                           /* RPE Pulse 9, SubFrame 4 */
  FTM_CLASS_II, FTM_CLASS_II, FTM_CLASS_IB,                                           /* RPE Pulse 10, SubFrame 4 */
  FTM_CLASS_II, FTM_CLASS_II, FTM_CLASS_IB,                                           /* RPE Pulse 11, SubFrame 4 */
  FTM_CLASS_II, FTM_CLASS_II, FTM_CLASS_IB,                                           /* RPE Pulse 12, SubFrame 4 */
  FTM_CLASS_II, FTM_CLASS_II, FTM_CLASS_IB,                                           /* RPE Pulse 13, SubFrame 4 */

};

/* This table is used to rearrange bits in vocoder param order -> bit class order (d0...d259) */
static uint16 ftm_se_ber_fr_speech_remap_table[] =
{

  /* bit number of rearranged bits to find the d(k) bits */
  /* class 1a: d0 to d49 */
  5,                               /* d0 is bit 5 of the rearranged bits */
  52,108,164,220,                  /* d1,d2,d3,d4 are bits 52,108,164,220 of the rearranged bits */
  4,
  11,
  16,
  3,
  10,
  15,
  21,
  42,98,154,210,
  51,107,163,219,
  9,25,29,
  41,97,153,209,
  40,96,152,208,
  39,95,151,207,
  38,94,150,206,
  50,106,162,218,
  2,
  20,
  32,
  37,93,149,205,

  /* class 1b: d50 to d181 */
  24,28,                 /* LAR5/6 bit 2 */
  44,100,156,212,        /* LPT gain bit 1 */
  36,92,148,204,         /* LPT lag bit 0 */
  46,102,158,214,        /* grid pos bit 1 */
  1,                     /* LAR1 bit 1 */
  8,14,35,19,            /* LAR2/3/8/4 bit 2 */
  23,31,                 /* LAR5/7 bit 1 */
  43,99,155,211,         /* LPT gain bit 0 */
  49,105,161,217,        /* block amp bit 2 */
  55,58,61,64,67,70,73,76,79,82,85,88,91,   /* RPE pulse 13-25, bit 2 */
  111,114,117,120,123,126,129,132,135,138,141,144,147, /* RPE pulse 30-42, bit 2 */
  167,170,173,176,179,182,185,188,191,194,197,200,203, /* RPE pulse 47-59, bit 2 */
  223,226,229,232,235,238,241,244,247,250,253,256,259, /* RPE pulse 64-76, bit 2 */
  45,101,157,213,        /* grid pos bit 0 */
  48,104,160,216,        /* block amp bit 1 */
  54,57,60,63,66,69,72,75,78,81,84,87,90,   /* RPE pulse 13-25, bit 1 */
  110,113,116,119,122,125,128,131,134,137,140,143,146, /* RPE pulse 30-42, bit 1 */
  166,169,172,175,178,181,184,187,190,193,196,199,202, /* RPE pulse 47-59, bit 1 */
  222,225,228,231,                                     /* RPE pulse 64-67, bit 1 */


  /* class 2: d182 to d259 */
  234,237,240,243,246,249,252,255,258, /* RPE pulse 68-76, bit 1 */
  0,                                   /* LAR1 bit 0 */
  7,13,27,                             /* LAR2/3/6 bit 1 */
  30,                                  /* LAR7 bit 0 */
  34,                                  /* LAR8 bit 1 */
  33,12,                               /* LAR8/3 bit 0 */
  18,                                  /* LAR4 bit 1 */
  17,22,                               /* LAR4/5 bit 0 */
  47,103,159,215,                      /* block amp bit 0 */
  53,56,59,62,65,68,71,74,77,80,83,86,89,   /* RPE pulse 13-25, bit 0 */
  109,112,115,118,121,124,127,130,133,136,139,142,145, /* RPE pulse 30-42, bit 0 */
  165,168,171,174,177,180,183,186,189,192,195,198,201, /* RPE pulse 47-59, bit 0 */
  221,224,227,230,233,236,239,242,245,248,251,254,257, /* RPE pulse 68-76, bit 0 */
  6,26                                                 /* LAR2/6 bit 0 */

};

// Synchronous status of BER/BLER
static ftm_se_ber_sync_status_type sync_status = {0,0};

/*===========================================================================

                     FUNCTION PROTOTYPES & DEFINITIONS

===========================================================================*/

    #define MAX_BIT_COUNT (8 * sizeof( uint32 ))

uint8 get_bit_count_32( uint32 data );
uint32 invert_bit_order( uint32 data, uint32 mask );
boolean seed_pn_sequence( se_ber_pn_sequence_type* sequence, se_ber_pn_length_type gen_poly_deg, uint32 lsr_state, boolean invert );
boolean get_pn_sequence_single_bit( se_ber_pn_sequence_type* sequence );
uint32* get_pn_block_output( se_ber_pn_sequence_type* sequence, uint8 block_length );
boolean acquire_pn_synch( se_ber_pn_sequence_type* sequence, uint32* data, uint8 data_len_dword, uint16* threshold, uint32* current_bit_pos );
void compute_ber( se_ber_pn_sequence_type* sequence, uint32* data, uint16 data_bock_sz_bits, uint16* bit_count, uint16* error_count, uint32 *err_pos );
void convert_bytearray_dword( uint8* data, uint32* dword_data, uint32 mask, boolean invert, boolean reverse_bits );
void se_ber_acquire_run( uint32* dword_data, uint16 data_len_bits, uint8 pn_index );


void ftm_se_ber_log_ber_report( void );
void ftm_se_ber_add_log_sample( uint32 fn, uint8 slot_num );
void ftm_se_ber_format_data_bits( uint32* dword_data, ftm_se_ber_config_run_type *config);

void ftm_se_ber_add_log_sample_gsm_ab( uint32 fn, uint8 slot_num, boolean bfi );
void ftm_se_ber_log_ber_report_gsm_ab( void);

// Function to update the synchronous status BER
// The values passed in add to the existing status
void ftm_se_ber_update_sync_status_ber( uint32 bits, uint32 bit_errors );

// Function to clear the synchronous status BLER
void ftm_se_ber_clear_sync_status_ber( void );

/*===========================================================================
FUNCTION     FTM_SE_BER_CONFIGURE

DESCRIPTION
  ftm_se_ber_configure()

DEPENDENCIES
  None.

PARAMETERS
  uint32 data -- the integer to evaluate bit count on

RETURN VALUE
  uint8

SIDE EFFECTS
  None
===========================================================================*/
void ftm_se_ber_configure( se_ber_pn_length_type pn_len, se_ber_mode_type ber_mode, boolean invert, uint16 acq_thresh, uint16 lost_acq_water_mark )
{

  uint8 i;
  ber_state_machine.initialized = FALSE;
  ber_state_machine.invert = invert;
  ber_state_machine.mode = ber_mode;
  ber_state_machine.acq_thresh = acq_thresh;


  ber_state_machine.bit_count[0] = 0;   /* loop C, or class 1a of loop A/B */
  ber_state_machine.bit_count[1] = 0;   /* class 1b of loop A/B */
  ber_state_machine.bit_count[2] = 0;   /* class 2 of loop A/B */

  ber_state_machine.error_count[0] = 0;
  ber_state_machine.error_count[1] = 0;
  ber_state_machine.error_count[2] = 0;

  ber_state_machine.lost_acq_water_mark = lost_acq_water_mark;

  for( i = 0; i < FTM_SE_BER_NUM_PN_SEQ; i++)
  {
    ber_state_machine.acquired[i] = FALSE;
    (void) seed_pn_sequence( &ber_state_machine.pn_seq[i], pn_len, 0x1, FALSE );
  }
  ber_state_machine.initialized = TRUE;
}

/*===========================================================================
FUNCTION     FTM_SE_BER_START

DESCRIPTION
  ftm_se_ber_start() starts the logging interface for the SE BER. It will
  also configure the SE BER state machine with the default parameters.
  THe SE BER state machine can be reconfigured any time using ftm_se_ber_configure()
  API

DEPENDENCIES
  None.

PARAMETERS
  uint8 data -- the integer to evaluate bit count on

RETURN VALUE
  uint8

SIDE EFFECTS
  None
===========================================================================*/
void ftm_se_ber_start ( se_ber_log_reports_count_type report_count )
{

  if( report_count >= FTM_SE_BER_REPORT_COUNT_MIN &&
      report_count <= FTM_SE_BER_REPORT_COUNT_MAX )
  {

    ber_log_status.report_index = 0;             /* will implicitly flush any current buffered samples */
    ber_log_status.report_limit = (uint8)report_count;
    ber_log_status.log_state    = TRUE;
  }
}

/*===========================================================================
FUNCTION     FTM_SE_BER_STOP

DESCRIPTION
  ftm_se_ber_stop()

DEPENDENCIES
  None.

PARAMETERS

RETURN VALUE
  uint8

SIDE EFFECTS
  None
===========================================================================*/
void ftm_se_ber_stop( void )
{
  ber_log_status.log_state      = FALSE;
  ber_log_status.report_index   = 0;
  ber_state_machine.initialized = FALSE;  /* stop the SE BER process in addition to the logging */
}

/*===========================================================================
FUNCTION     FTM_SE_BER_RUN

DESCRIPTION
  ftm_se_ber_run() executes the motions through the SE BER state machine

DEPENDENCIES
  ftm_se_ber_configure()

PARAMETERS
  uint32 data -- the integer to evaluate bit count on

RETURN VALUE
  uint8

SIDE EFFECTS
  None
===========================================================================*/
void ftm_se_ber_run( uint8* rx_data, ftm_se_ber_config_run_type *config )
{
  uint8 data_len_dwords = 0;
  uint32* dword_data;
  uint32* dword_data_err_positions;
  uint32 cur_bit_pos;
  uint16 error_count, bit_count;
  uint16 loc_acq_thresh = ber_state_machine.acq_thresh, i;
  boolean  reverse_bits = FALSE;

  if( config->slot_num >= FTM_SE_BER_NUM_PN_SEQ)
  {
    return;
  }
  if( config->run_mode == FTM_SE_RUN_MODE_GSM_LOOP_AB )
  {
    data_len_dwords = (uint8) (((config->bit_def.num_bits_gsm_loop_ab.num_class1a_bits +
                                 config->bit_def.num_bits_gsm_loop_ab.num_class1b_bits +
                                 config->bit_def.num_bits_gsm_loop_ab.num_class2_bits) / MAX_BIT_COUNT)+1);
  }
  else
  {
    data_len_dwords = (uint8)(config->bit_def.num_bits / MAX_BIT_COUNT + 1);
  }

  if( ber_state_machine.initialized )
  {
    /* Allocate the local data buffer */
    dword_data = (uint32*) ftm_malloc( data_len_dwords * sizeof( uint32 ) );
    if (dword_data == NULL) 
    { 
      FTM_MSG_ERROR(" can't allocate memory for dword_data",0,0,0);
      return;
    }
    dword_data_err_positions = (uint32*) ftm_malloc( data_len_dwords * sizeof( uint32 ) );
    if (dword_data_err_positions == NULL) 
    { 
      FTM_MSG_ERROR(" can't allocate memory for dword_data_err_positions",0,0,0);
      ftm_free( (void*)dword_data );
      return;
    }

    if( config->bit_ordering == FTM_SE_BER_BIT_ORDERING_RIGHT_ALIGNED )
    {
      reverse_bits = TRUE;
    }
    /* copy the data from the buffer into a local buffer of DWORD type */
    for( i = 0; i < data_len_dwords; i++ )
    {
      convert_bytearray_dword( rx_data + i * sizeof( uint32 ), dword_data + i, 0xFFFFFFFF, ber_state_machine.invert, reverse_bits );
    }

    /* data is now in uint32, left aligned (oldest bit in time {first bit of PN seq}
     * is most sig. bit of dword_data[0])
     */

    /* Preformat bits to remove stealing bits, usf bits, etc. depending on technology and loop type */
    ftm_se_ber_format_data_bits( dword_data, config);

    /* At this point the dword_data buffer should be a left aligned, packed sequence,
     * representing the PN data
     */

    /* switch off of the state machine run instruction and go through the motion */
    switch( ber_state_machine.mode )
    {
      case FTM_SE_MODE_ACQUIRE:

        (void) acquire_pn_synch( &ber_state_machine.pn_seq[config->slot_num], dword_data, data_len_dwords - 1, &loc_acq_thresh, &cur_bit_pos );
        break;

      case FTM_SE_MODE_RUN:

        break;

        /*
          FTM_SE_MODE_ACQUIRE_RUN: the acquisition and BER algortihms will be run on every data block passed in
          Once acq state is TRUE the pn generator is switched to the local version and BER computation is possible with
          the replica of Rx bits produced by the local PN generator. The replica is then compared against the Rx bits
        */

      case FTM_SE_MODE_ACQUIRE_RUN:

        if( config->run_mode == FTM_SE_RUN_MODE_GSM_LOOP_AB )
        {

          /* to keep things simple, just run acq on class1a bits--> temp test run on all bits */
          se_ber_acquire_run(dword_data, config->bit_def.num_bits_gsm_loop_ab.num_class1a_bits +
                             config->bit_def.num_bits_gsm_loop_ab.num_class1b_bits +
                             config->bit_def.num_bits_gsm_loop_ab.num_class2_bits,
                             config->slot_num);

          ftm_se_ber_add_log_sample_gsm_ab( config->fn, config->slot_num, config->bfi);
        }
        else
        {
          se_ber_acquire_run(dword_data, config->bit_def.num_bits, config->slot_num );
          if ( config->slot_num < FTM_SE_BER_NUM_PN_SEQ ) 
          {
            ftm_se_ber_add_log_sample( config->fn, config->slot_num );
          }
          else 
          {
            FTM_MSG_ERROR(" config->slot_num exceeds allowed array size = %d", config->slot_num,0,0);
          }
        }

        break;

      case FTM_SE_MODE_AUTO:

        if ( config->slot_num >= FTM_SE_BER_NUM_PN_SEQ )
        {
          FTM_MSG_ERROR("config->slot_num exceeds array size = %d", config->slot_num,0,0);
          ftm_free( (void*) dword_data_err_positions);
          ftm_free( (void*) dword_data);
          return;
        }
        else if( ber_state_machine.acquired[config->slot_num] )
        {
          /* add processing to separate bit classes in loop A/B */

          if( config->run_mode == FTM_SE_RUN_MODE_GSM_LOOP_AB )
          {

            uint16  loop_ab_bit = 0;
            uint16  loop_ab_bit_total = 0;

            /* initialize */

            for( i = 0; i < FTM_CLASS_MAX; i++)
            {
               ber_state_machine.bit_count[i]   = 0;
               ber_state_machine.error_count[i] = 0;
            }

            /* compute BER on class 1a/1b/2 bits */

            loop_ab_bit_total =  config->bit_def.num_bits_gsm_loop_ab.num_class1a_bits +
                                 config->bit_def.num_bits_gsm_loop_ab.num_class1b_bits +
                                 config->bit_def.num_bits_gsm_loop_ab.num_class2_bits;


            if( loop_ab_bit_total > FTM_SE_BER_NUM_BITS_GSM_LOOP_AB)
            {

              FTM_MSG_ERROR("Invalid Number of LoopA/B bits",0,0,0);
              ftm_free( (void*) dword_data_err_positions);
              ftm_free( (void*) dword_data);
              return;
            }

            compute_ber( &ber_state_machine.pn_seq[config->slot_num], dword_data,
                          loop_ab_bit_total,
                          &bit_count,
                          &error_count,
                          dword_data_err_positions
                        );

            /* loop over all bits, find out which bit positions had errors, then count
             * up the errors according to bit class.  This only works for Full Rate Speech
             */

            for( loop_ab_bit = 0; loop_ab_bit < loop_ab_bit_total; loop_ab_bit++)
            {
              /* offset into which err_position word */
              uint8                      word_index = loop_ab_bit / MAX_BIT_COUNT;

              /* bit index to test.  For example when loop_ab_bit = 33, we want word_offset = 1,
               * bit_index = 1.  However, since we are looking at bits from left to right, we want
               * to test using a mask of (0x1 << (31 - bit_index)) to isolate whether bit number
               * 33 was in error
               */
              uint8                      bit_index = loop_ab_bit % MAX_BIT_COUNT;

              ftm_se_ber_bit_class_type  bit_class  = ftm_se_ber_fr_bit_class_mapping[ loop_ab_bit];

              /* In the case that preprocessing is done to rearrange the bits in
               * terms of bit class order to obtain the actual PN sequence, we do not
               * need to use a lookup table to determine the bit class.  This is because
               * we are already looking at the bits in bit class order.  So we can easily
               * classify the bits by the actual bit position. (50 class Ia, followed by
               * 132 class Ib, followed by 78 class II.
               */
              if( ber_state_machine.loop_ab_order == FTM_SE_BER_LOOP_AB_ORDER_CH_CODEC_INPUT)
              {
                if( loop_ab_bit < FTM_SE_BER_NUM_CLASS_IA_BITS)
                {
                  bit_class = FTM_CLASS_IA;
                }
                else if( loop_ab_bit < (FTM_SE_BER_NUM_CLASS_IA_BITS + FTM_SE_BER_NUM_CLASS_IB_BITS))
                {
                  bit_class = FTM_CLASS_IB;
                }
                else
                {
                  bit_class = FTM_CLASS_II;
                }
              }

              if( bit_class > FTM_CLASS_II)
              {
                FTM_MSG_ERROR("Invalid bit class!",0,0,0);
                continue;
              }

              /* increment bit count for this bit class */
              ber_state_machine.bit_count[ bit_class]++;

              /* Test whether this bit position was in error.  If so, increment the error count
               * for this particular bit class */
              if( dword_data_err_positions[ word_index] & FTM_SE_BER_GET_BITMASK(bit_index))
              {
                ber_state_machine.error_count[ bit_class]++;
              }
            }

            /* check if the total error count for all bit classes is over the watermark */
            if( ber_state_machine.error_count[0]+ber_state_machine.error_count[1]+ber_state_machine.error_count[2] >
                (uint32)ber_state_machine.lost_acq_water_mark )
            {

              ber_state_machine.acquired[config->slot_num] = FALSE;
              FTM_MSG_HIGH("SE BER Mode : AUTO-LOST-LOCK, Bits Counted : %d, Errors Counted : %d, WM : %d",
                           ber_state_machine.bit_count[0]+ber_state_machine.bit_count[1]+ber_state_machine.bit_count[2],
                           ber_state_machine.error_count[0]+ber_state_machine.error_count[1]+ber_state_machine.error_count[2],
                           ber_state_machine.lost_acq_water_mark);
            }

            /* send out log packet */
            ftm_se_ber_add_log_sample_gsm_ab( config->fn, config->slot_num, config->bfi);

          }
          else
          {

            /* acquired.  Not loopA/B type so no need to separate bit classes */
            compute_ber( &ber_state_machine.pn_seq[config->slot_num], dword_data, config->bit_def.num_bits, &bit_count, &error_count, NULL );
            ber_state_machine.bit_count[0]   = bit_count;
            ber_state_machine.error_count[0] = error_count;
            ber_state_machine.bit_count[1]   = 0;
            ber_state_machine.error_count[1] = 0;
            ber_state_machine.bit_count[2]   = 0;
            ber_state_machine.error_count[2] = 0;

            if( error_count > (uint32)ber_state_machine.lost_acq_water_mark )
            {

              ber_state_machine.acquired[config->slot_num] = FALSE;
              FTM_MSG_HIGH("SE BER Mode : AUTO-LOST-LOCK, Bits Counted : %d, Errors Counted : %d, WM : %d", ber_state_machine.bit_count[0], ber_state_machine.error_count[0], ber_state_machine.lost_acq_water_mark);
              
              /*if first time lost synth, then re-run to make sure it's not due to ARB waveform restart */
              FTM_MSG_HIGH("SE BER Mode : AUTO-LOST-LOCK-RETRY-ONCE",0,0,0);
              se_ber_acquire_run(dword_data, config->bit_def.num_bits, config->slot_num);
            }

            ftm_se_ber_add_log_sample( config->fn, config->slot_num );
            FTM_MSG_HIGH( "SE BER Mode : AUTO-BER, Bits Counted : %d, Errors Counted : %d, WM : %d", ber_state_machine.bit_count[0], ber_state_machine.error_count[0], ber_state_machine.lost_acq_water_mark);

          }

        }
        else
        {

          /* not yet acquired */
          if( config->run_mode == FTM_SE_RUN_MODE_GSM_LOOP_AB )
          {

            /* to keep things simple, just run acq on all bits */
            se_ber_acquire_run(dword_data, config->bit_def.num_bits_gsm_loop_ab.num_class1a_bits +
                               config->bit_def.num_bits_gsm_loop_ab.num_class1b_bits +
                               config->bit_def.num_bits_gsm_loop_ab.num_class2_bits,
                               config->slot_num);

            /* this log packet will be invalid, as it will show the results from the acq-ber process
             * as all Class 1a bits.  The user of the log should actually check the reported bit counts
             * to make sure the expected number of Class 1a,1b,and II bits are reported. */

            ftm_se_ber_add_log_sample_gsm_ab( config->fn, config->slot_num, config->bfi);

          }
          else
          {

            /* run over all bits, no need to worry about bit classes */
            se_ber_acquire_run(dword_data, config->bit_def.num_bits, config->slot_num);
            ftm_se_ber_add_log_sample( config->fn, config->slot_num );
          }

          FTM_MSG_HIGH( "SE BER Mode : AUTO-ACQ-BER, Bits Counted : %d, Errors Counted : %d, WM : %d", ber_state_machine.bit_count[0], ber_state_machine.error_count[0], ber_state_machine.lost_acq_water_mark);
        }
        break;

      default:
        break;
    }
    /* Free up the local data buffer */
    ftm_free( (void*)dword_data );
    ftm_free( (void*) dword_data_err_positions);
  }
}

/*===========================================================================
FUNCTION     GET_BIT_COUNT_32

DESCRIPTION
  get_bit_count_32() function returns bit count in a 32-bit integer. The
  bit count is based on the reverse look up into a global 16 byte array
  that contains bit counts for integers 0x0 through 0xFF. The integers
  serve as an index into the array

DEPENDENCIES
  None.

PARAMETERS
  uint32 data -- the integer to evaluate bit count on

RETURN VALUE
  uint8

SIDE EFFECTS
  None
===========================================================================*/

uint8 get_bit_count_32( uint32 data )
{
  return (bit_count_lut[ (data >>  0) & 0x0F ] +
          bit_count_lut[ (data >>  4) & 0x0F ] +
          bit_count_lut[ (data >>  8) & 0x0F ] +
          bit_count_lut[ (data >> 12) & 0x0F ] +
          bit_count_lut[ (data >> 16) & 0x0F ] +
          bit_count_lut[ (data >> 20) & 0x0F ] +
          bit_count_lut[ (data >> 24) & 0x0F ] +
          bit_count_lut[ (data >> 28) & 0x0F ]);
}

/*===========================================================================
FUNCTION     INVERT_BIT_ORDER

DESCRIPTION
  invert_bit_order( ) function returns a 32-bit integer that represent a
  number with the bit order inverted from what has been passed in. The mask
  specifeis how many bits will affected by the bit order inversion. The
  return value will be left aligned.

  Examples:
    data = 0x55555555, mask = 0xFFFFFFFF ==> return value = 0xAAAAAAAA
    data = 0x55552222, mask = 0x0000FFFF ==> return value = 0x00004444

DEPENDENCIES
  None.

PARAMETERS
  uint32 data -- the integer to invert the bit order on
  uint32 mask -- masks out the portion of the data on which to perform the
                 inversion

RETURN VALUE
  uint32

SIDE EFFECTS
  None
===========================================================================*/
uint32 invert_bit_order( uint32 data, uint32 mask )
{

  uint32 ret_val = 0x0;
  uint8 mask_bit_cnt = get_bit_count_32( mask );

  se_ber_pn_sequence_type pn1;
  (void) seed_pn_sequence( &pn1, FTM_SE_PN_TYPE_PN1, data & mask, FALSE );

  ret_val = *get_pn_block_output( &pn1, MAX_BIT_COUNT );

  return ret_val >>= (MAX_BIT_COUNT - mask_bit_cnt);
}


/*===========================================================================
FUNCTION     SEED_PN_SEQUENCE

DESCRIPTION
  seed_pn_sequence(...) function initializes the PN sequence LSR generator, based on the
  initial LSR state and the generator's polynomial degree. The return value is TRUE if
  intialization succeeded, FALSE otherwise

DEPENDENCIES
  None.

PARAMETERS
  uint32 data -- the integer to evaluate bit count on

RETURN VALUE
  uint8

SIDE EFFECTS
  None
===========================================================================*/
boolean seed_pn_sequence( se_ber_pn_sequence_type* sequence, se_ber_pn_length_type gen_poly_deg, uint32 lsr_state, boolean invert )
{

  boolean ret_val = TRUE;

  /* Verify that the length of the deneratng polunomial is supported */
  if( gen_poly_deg <= FTM_SE_PN_TYPE_PN15 )
  {
    /* Assign the degree of the generating polynomial */
    sequence->gen_polynomial_degree = gen_poly_deg;
    sequence->gen_polynomial = pn_generating_polynomial[gen_poly_deg];
    /* Put 1 into a position of the highest bit in LSR (used in LSR state calculation later) */
    if( gen_poly_deg == FTM_SE_PN_TYPE_PN1 )
    {
      sequence->high_bit_value = (uint32)1 << (MAX_BIT_COUNT - 1);
    }
    else
    {
      sequence->high_bit_value = (uint32)1 << (gen_poly_degree_lut[gen_poly_deg] - 1);
    }

    /* LSR state */
    sequence->sr_state = 0x0;
    sequence->sr_state = lsr_state;

    /* Content of the output block */
    sequence->sr_block_output = 0x0;
    sequence->invert_bit = invert;
  }
  else
  {
    ret_val = FALSE;
  }


  return ret_val;
}

/*===========================================================================
FUNCTION     GET_PN_SEQUENCE_SINGLE_BIT

DESCRIPTION
  get_pn_sequence_single_bit(...) function outputs a single bit of PN
  sequence based on the current state of the LSR and the gen polynomial.
  The LSR state will be updated based on the current state and the generating
  polynomial. This function does not update the state of the
  sequene.sr_block_output

DEPENDENCIES
  None.

PARAMETERS
  se_ber_pn_sequencetype* sequence -- the pointer to the PN sequence structure

RETURN VALUE
  boolean

SIDE EFFECTS
  Modifies the state of the LSR of the pn sequence variable passed in as an
  argument
===========================================================================*/
boolean get_pn_sequence_single_bit( se_ber_pn_sequence_type* sequence )
{
  boolean ret_val = sequence->sr_state & 0x1;

  if( get_bit_count_32( sequence->sr_state & sequence->gen_polynomial ) % 2 )
  {
    sequence->sr_state >>= 1;
    sequence->sr_state += sequence->high_bit_value;
  }
  else
  {
    sequence->sr_state >>= 1;
  }

  if( sequence->invert_bit )
  {
    return !ret_val;
  }
  else
  {
    return ret_val;
  }
}

/*===========================================================================
FUNCTION     GET_PN_BLOCK_OUTPUT

DESCRIPTION
  get_pn_block_output(...) function computes the n-bit block of PN sequence
  and repopulates the state of the LSR. The state of the block output member
  of se_ber_pn_sequence_type structure is updated

DEPENDENCIES
  seed_pn_sequence() must be called prior to get_pn_block_output() to initiaize
  the sequence variable

PARAMETERS
  uint32 data -- the integer to evaluate bit count on

RETURN VALUE
  uint32* -- pointer to the PN block output

SIDE EFFECTS
  Modifies the state of the LSR of the pn sequence variable passed in as an
  argument. Modifies the state of the PN block output member of se_ber_pn_sequence_type
===========================================================================*/
uint32* get_pn_block_output( se_ber_pn_sequence_type* sequence, uint8 block_length )
{
  uint32 mask = 0x0;
  uint8 i;

  if( block_length <= MAX_BIT_COUNT )
  {

    mask = (uint32)(( (uint64)1 << block_length ) - 1 );

    for( i = 0; i < block_length; i++ )
    {

      sequence->sr_block_output <<= 1;
      if( get_pn_sequence_single_bit( sequence ) ) sequence->sr_block_output++;
    }

    sequence->sr_block_output &= mask;
  }
  else
  {

    sequence->sr_block_output = 0x0;
  }

  return &sequence->sr_block_output;
}

/*===========================================================================
FUNCTION     ACQUIRE_PN_SYNCH

DESCRIPTION
  acquire_pn_sequence(...) function attempts to synchronize the local PN
  generator with the incoming data stream. Threshold argument specifies the
  number of successive error free   bits. Returns TRUE if lock is found

DEPENDENCIES
  None.

PARAMETERS
  uint32 data -- the integer to evaluate bit count on

RETURN VALUE
  boolean

SIDE EFFECTS
  Modifies the state of the LSR of the pn sequence variable passed in as an
  argument.
===========================================================================*/
boolean acquire_pn_synch( se_ber_pn_sequence_type* sequence, uint32* data, uint8 data_len_dword, uint16* threshold, uint32* current_bit_pos )
{
  boolean ret_val = FALSE;

  uint16 acc_thresh = 0x0;

  uint8 rx_bit_data = 0x0, rx_bit_local_pn_gen = 0x0, data_offset = 0x0, shift = 0x0,
        block_end = 0x0, R = gen_poly_degree_lut[sequence->gen_polynomial_degree];

  uint32 i, rx_data_block = 0x0, rx_data_block_inv = 0x0, rx_data_block_mask = (1 << R) - 1,
         stop_at_bit = MAX_BIT_COUNT;  /* temp, give 32 chances to acquire.  MAX_BIT_COUNT * (data_len_dword)- R;  */

  /*
    The algorithm is to seed the PN genertor with the data and then assume that
    the seed was error free. The algorithm is going to keep an accumulator which
    must reach the prescribed count

    The assumption on the data is that the MSB of the PN sequence is the MSB of data[0].
    PN sequence genertor is assumed to be initialized with the proper polynomial as well
  */

  for( i = 0; i < stop_at_bit && acc_thresh < *threshold; *current_bit_pos = R+(++i) )
  {

    block_end = ( i + R ) % MAX_BIT_COUNT;

    if( i && !(i % MAX_BIT_COUNT) )
      ++data_offset;
    /*
      Take r bits (r is the length of generating polynomial) of the
      incoming data and invert the bit sequence
    */
    if( i % MAX_BIT_COUNT > block_end )
    {
      /*
        R-bit sequence spans two adjacent words
      */
      shift = MAX_BIT_COUNT - block_end;
      rx_data_block = (*(data+data_offset) & (rx_data_block_mask >> block_end)) << block_end;
      rx_data_block += ( (1 << block_end) - 1) & (*(data+data_offset+1) >> shift);
    }
    else
    {
      shift = MAX_BIT_COUNT - block_end;
      rx_data_block = rx_data_block_mask & (*(data+data_offset) >> shift);
    }

    rx_data_block_inv = invert_bit_order( rx_data_block, rx_data_block_mask );

    if( i )
    {

      rx_bit_data = rx_data_block & 0x1;
      ( rx_bit_data ^ rx_bit_local_pn_gen ) ? acc_thresh = 0 : ++acc_thresh;
    }
    if( rx_data_block_inv == 0x0)
    {
      acc_thresh = 0;  /* invalidate number of valid bits if all 0s sequence is detected.  PN shift register
                        * can never have all zeros */
    }

    /*
      Re-seed the incoming pn sequence with the r bits
    */
    (void) seed_pn_sequence( sequence, sequence->gen_polynomial_degree, rx_data_block_inv, sequence->invert_bit );
    rx_bit_local_pn_gen = 0x1 & (*get_pn_block_output( sequence, R + 1 ));
  }

  if( *threshold == acc_thresh )
  {

    ret_val = TRUE;
  }

  *threshold = acc_thresh;

  return ret_val;
}

/*===========================================================================
FUNCTION     COMPUTE_BER

DESCRIPTION

DEPENDENCIES
  None.

PARAMETERS


RETURN VALUE
  None.

SIDE EFFECTS
  Modifies the state of the LSR of the pn sequence variable passed in as an
  argument.
===========================================================================*/
void compute_ber( se_ber_pn_sequence_type* sequence, uint32* data, uint16 data_bock_sz_bits, uint16* bit_count, uint16* error_count, uint32 *err_pos )
{
  uint32 pn_data_block, rx_data_block;
  uint8 counter = 0x0;
  uint32 rx_block_mask = 0xFFFFFFFF;

  *bit_count = *error_count = 0x0;

  do
  {
    if( data_bock_sz_bits > MAX_BIT_COUNT )
    {

      *bit_count += MAX_BIT_COUNT;
      pn_data_block = *get_pn_block_output( sequence, MAX_BIT_COUNT );
    }
    else
    {

      *bit_count += data_bock_sz_bits;
      pn_data_block = *get_pn_block_output( sequence, (uint8) data_bock_sz_bits) << (MAX_BIT_COUNT - data_bock_sz_bits);
      rx_block_mask <<= (MAX_BIT_COUNT - data_bock_sz_bits);
    }

    rx_data_block = *(data+counter++) & rx_block_mask;
    *error_count += get_bit_count_32( pn_data_block ^ rx_data_block );

    /* if requested, save off the bit positions that are in error.  This is needed for LoopA/B, to later
     * collect which error bits belong to which bit class */

    if( err_pos != NULL)
    {
      *err_pos++ = pn_data_block ^ rx_data_block;
    }

  } while( !( (data_bock_sz_bits -= MAX_BIT_COUNT) & 0x8000 ) );
}


/*===========================================================================
FUNCTION     ftm_se_ber_bit_swap

DESCRIPTION  Swaps 2 bits at specified locations, in a packed array of
             uint32.  This is meant for use with GSM/EGPRS, where the number
             of bits is at most 348.

DEPENDENCIES
  None.

PARAMETERS   data -> pointer to packed uint32 buffer.  Data should be valid from
             data[0] to data[x], where x = (MAX(bit_num_a,bit_num_b) / 32)


RETURN VALUE
  None.

SIDE EFFECTS

===========================================================================*/
static void ftm_se_ber_bit_swap( uint32 *data, uint16 bit_num_a, uint16 bit_num_b)
{
  boolean bit_a = FALSE;
  boolean bit_b = FALSE;

  if( bit_num_a > 348 || bit_num_b > 348)
  {
    return;   /* cannot be more than 348 bits for 8PSK */
  }

  /* check if data bit number bit_num_a is a "1" */
  if( data[ bit_num_a / MAX_BIT_COUNT] & FTM_SE_BER_GET_BITMASK( bit_num_a % MAX_BIT_COUNT))
  {
    bit_a = TRUE;
  }
  /* check if data bit number bit_num_b is a "1" */
  if( data[ bit_num_b / MAX_BIT_COUNT] & FTM_SE_BER_GET_BITMASK( bit_num_b % MAX_BIT_COUNT))
  {
    bit_b = TRUE;
  }
  /* if the bits are the same, then swapping won't do anything, so
   * exit.  If bits are different, then swapping will invert the bits
   * at positions bit_num_a and bit_num_b.  Invert by xoring the data word
   * with the appropriate mask.  Other bits will be unchanged.
   */
  if( bit_a != bit_b)
  {
    data[ bit_num_a / MAX_BIT_COUNT] ^= FTM_SE_BER_GET_BITMASK( bit_num_a % MAX_BIT_COUNT);
    data[ bit_num_b / MAX_BIT_COUNT] ^= FTM_SE_BER_GET_BITMASK( bit_num_b % MAX_BIT_COUNT);
  }

}

/*===========================================================================
FUNCTION     FORMAT_DATA_BITS

DESCRIPTION
              Perform pre-processing on received bits, to remove the bits
              that aren't actually part of the PN sequence (non-data bits
              that must be sent for physical layer signaling, such as stealing bits).


DEPENDENCIES
  None.

PARAMETERS
              dword_data - packed buffer of data bits, left aligned.
              bit 0 (earliest bit in time) is in MSB of dword_data[0].  That is,
              dword_data[0] contains data bits 0,1,2,...31, read left to right
              dword_data[1] contains data bits 32,33,34...63, etc.

RETURN VALUE
  boolean

SIDE EFFECTS
  None.
===========================================================================*/
void ftm_se_ber_format_data_bits( uint32* dword_data, ftm_se_ber_config_run_type *config)
{


  if( config->run_mode == FTM_SE_RUN_MODE_GSM_LOOP_AB &&
      ber_state_machine.loop_ab_order == FTM_SE_BER_LOOP_AB_ORDER_CH_CODEC_INPUT)
  {


    /* Loops A/B assume that the PN data has been applied as if it was a speech
     * frame delivered by the vocoder.  At input to the channel coding step, these bits are
     * rearranged in order of bit class.  When FTM receives the decoded bits, it is
     * as if it is a decoded speech frame, in vocoder parameter order.  In these cases
     * the bits FTM receives in vocoder param order is a PN sequence

     * Some test equipment
     * may apply the PN sequence AFTER the channel codec rearranging step.  So FTM
     * needs to undo the GL1/FW operation of "de-rearranging"...therefore FTM is
     * actually performing the rearranging operation to order the vocoder param bits
     * in terms of bit class order.  This code below does the rearranging
     * step to restore bit class order, and obtain a PN sequence.
     */

     /* For this we only support Full Rate speech */
    uint16  j;
    uint8  word_index;
    uint32 bit_mask;
    boolean bit_collection[FTM_SE_BER_NUM_BITS_GSM_LOOP_AB] = {FALSE};

    for( j = 0; j < FTM_SE_BER_NUM_BITS_GSM_LOOP_AB; j++)
    {

      /* figure out where d(0) to d(259) are in the source data */
      /* save values to a temp array */

      /* example: d0 is in bit position ftm_se_ber_fr_speech_remap_table[0] of the rx data */
      /*          d1 is in bit position ftm_se_ber_fr_speech_remap_table[1] of the rx data...*/

      uint16     remap_bitnum = ftm_se_ber_fr_speech_remap_table[j];
      word_index = remap_bitnum / MAX_BIT_COUNT;
      bit_mask   = FTM_SE_BER_GET_BITMASK( ( remap_bitnum % MAX_BIT_COUNT));

      if( dword_data[ word_index] & bit_mask)
      {
        bit_collection[j] = TRUE;
      }
      else
      {
        bit_collection[j] = FALSE;
      }
    }

    /* write d0 to d259 in place */
    for( j = 0; j < FTM_SE_BER_NUM_BITS_GSM_LOOP_AB; j++)
    {

      word_index = j / MAX_BIT_COUNT;
      bit_mask   = FTM_SE_BER_GET_BITMASK((j % MAX_BIT_COUNT));

      if( bit_collection[j] == TRUE)
      {
        dword_data[ word_index] |= bit_mask;
      }
      else
      {
        dword_data[ word_index] &= ~bit_mask;
      }
    }

  }

  /* Handle GSM Loop C and SRB loopback data  */

  else if( (config->run_mode == FTM_SE_RUN_MODE_EGPRS_SRB || 
            config->run_mode == FTM_SE_RUN_MODE_GSM_LOOP_C) &&
      config->bit_def.num_bits == 116) /* 114 data bits + 2 stealing bits */
  {

    config->bit_def.num_bits = 114;  /* "remove" 2 stealing bits from bitcount */

    /* remove stealing bits from buffer.  These are not treated as data bits from the PN
     * sequence by the test box.  For GSM mode, these are in bits 57 and 58 of each burst. After
     * this is complete, the 114 actual data bits should be packed consecutively*/

    /* no change for burst<0:31> */
    /* dword_data[0] = dword_data[0];  */

    /*                 burst<32:56>                    burst<58:63>                  burst<64:65>  */
    dword_data[1] = ((dword_data[1] & 0xFFFFFF80) | ((dword_data[1] & 0x1F) << 2) | (dword_data[2] >> 30));
    /*                 burst<66:95>          burst<97:97> */
    dword_data[2] = ((dword_data[2] << 2) | (dword_data[3] >> 30));
    /*                 burst<98:116>  */
    dword_data[3] = (dword_data[3] << 2);
  }
  else if( config->run_mode == FTM_SE_RUN_MODE_EGPRS_SRB &&
           config->bit_def.num_bits == 348)
  {

    /* undo bit swapping per 45.003, section 5.1.9.6 */

    ftm_se_ber_bit_swap( dword_data, 142, 155);
    ftm_se_ber_bit_swap( dword_data, 144, 158);
    ftm_se_ber_bit_swap( dword_data, 145, 161);
    ftm_se_ber_bit_swap( dword_data, 147, 164);
    ftm_se_ber_bit_swap( dword_data, 148, 167);
    ftm_se_ber_bit_swap( dword_data, 150, 170);
    ftm_se_ber_bit_swap( dword_data, 151, 173);
    ftm_se_ber_bit_swap( dword_data, 176, 195);
    ftm_se_ber_bit_swap( dword_data, 179, 196);
    ftm_se_ber_bit_swap( dword_data, 182, 198);
    ftm_se_ber_bit_swap( dword_data, 185, 199);
    ftm_se_ber_bit_swap( dword_data, 188, 201);
    ftm_se_ber_bit_swap( dword_data, 191, 202);
    ftm_se_ber_bit_swap( dword_data, 194, 204);


    /* handle formatting for SRB with 8PSK modulation, depending on MCS */

    switch( ber_state_machine.expected_mcs)
    {

      case FTM_SE_BER_MCS1:
      case FTM_SE_BER_MCS2:
      case FTM_SE_BER_MCS3:
      case FTM_SE_BER_MCS4:
        break;

      case FTM_SE_BER_MCS5:
      case FTM_SE_BER_MCS6:

        /* MCS 5/6. remove burst<156:191> and pack.  Removes 36 bits from middle of burst
         * according to 3GPP 45.003 for burst formatting  */

        dword_data[4] = (dword_data[4] & 0xFFFFFFF0) | (dword_data[6] >> 28);
        dword_data[5] = (dword_data[6] << 4) | (dword_data[7] >> 28);
        dword_data[6] = (dword_data[7] << 4) | (dword_data[8] >> 28);
        dword_data[7] = (dword_data[8] << 4) | (dword_data[9] >> 28);
        dword_data[8] = (dword_data[9] << 4) | (dword_data[10] >> 28);
        dword_data[9] = (dword_data[10] << 4);

        config->bit_def.num_bits = 312;

        break;

      case FTM_SE_BER_MCS7:
      case FTM_SE_BER_MCS8:
      case FTM_SE_BER_MCS9:

        /* MCS7/8/9. remove burst<153:194> and pack.  Removes 42 bits from middle of burst
         * according to 3GPP 45.003 for burst formatting  */

        dword_data[4] = (dword_data[4] & 0xFFFFFF80) | ((dword_data[6] >> 22) & 0x0000007F);
        dword_data[5] = (dword_data[6] << 10) | (dword_data[7] >> 22);
        dword_data[6] = (dword_data[7] << 10) | (dword_data[8] >> 22);
        dword_data[7] = (dword_data[8] << 10) | (dword_data[9] >> 22);
        dword_data[8] = (dword_data[9] << 10) | (dword_data[10] >> 22);
        dword_data[9] = (dword_data[10] << 10);

        config->bit_def.num_bits = 306;

        break;

      default:

        break;

    }

  }
}




/*===========================================================================
FUNCTION     CONVERT_BYTEARRAY_DWORD

DESCRIPTION
  convert_bytearray_dword(...) function .........

DEPENDENCIES
  None.

PARAMETERS


RETURN VALUE
  boolean

SIDE EFFECTS
  None.
===========================================================================*/
void convert_bytearray_dword( uint8* data, uint32* dword_data, uint32 mask, boolean invert, boolean reverse_bits )
{

  uint8 misc[4];
  uint8 i;

  for( i = 0; i < 4; i++ )
  {

    misc[i]=data[3-i];
    if( reverse_bits )
    {
      misc[i] = (uint8) invert_bit_order( misc[i],0x000000FF);
    }
  }

  memscpy( dword_data, 4, misc, 4 );
  *dword_data &= mask;

  if( invert )
    *dword_data ^= mask;

}

/*===========================================================================
FUNCTION     SE_BER_ACQUIRE_RUN

DESCRIPTION
  se_ber_acquire_run(...) function .........

DEPENDENCIES
  None.

PARAMETERS


RETURN VALUE
  boolean

SIDE EFFECTS
  None.
===========================================================================*/
void se_ber_acquire_run( uint32* dword_data, uint16 data_len_bits, uint8 pn_index )
{
  uint32 cur_bit_pos;
  uint16 loc_acq_thresh = ber_state_machine.acq_thresh, bit_count, error_count;
  uint8 data_len_dwords = (uint8)(data_len_bits / MAX_BIT_COUNT + 1),
        block_end_pos, data_off;

  if( pn_index >= FTM_SE_BER_NUM_PN_SEQ)
  {
    return;
  }

  ber_state_machine.acquired[pn_index] = acquire_pn_synch( &ber_state_machine.pn_seq[pn_index], dword_data, data_len_dwords - 1, &loc_acq_thresh, &cur_bit_pos );

  if( ber_state_machine.acquired[pn_index] )
  {

    block_end_pos = (uint8)(cur_bit_pos % MAX_BIT_COUNT);

    /* if acquisition occurred in the middle of the dword fast forward the pn sequence to the next dword boundary */
    if( block_end_pos )
    {
      /* ignoring return value since we are fast forwarding */
      (void) get_pn_block_output( &ber_state_machine.pn_seq[pn_index], MAX_BIT_COUNT - block_end_pos );
      data_off = (uint8) (cur_bit_pos/MAX_BIT_COUNT + 1);
    }
    else
    {
      data_off = (uint8) (cur_bit_pos/MAX_BIT_COUNT);
    }

    compute_ber( &ber_state_machine.pn_seq[pn_index], dword_data + data_off, data_len_bits - MAX_BIT_COUNT * data_off, &bit_count, &error_count, NULL );

    /* Update the bit and error counts */
    ber_state_machine.bit_count[0]   = bit_count;
    ber_state_machine.error_count[0] = error_count;
    ber_state_machine.bit_count[1]   = 0;      /* force class 1b/class2 bit counts to 0 (don't acquire over these) */
    ber_state_machine.error_count[1] = 0;
    ber_state_machine.bit_count[2]   = 0;
    ber_state_machine.error_count[2] = 0;

    FTM_MSG_HIGH("Acquired PN sequence! BitCount = %d, ErrCount = %d, BitPos = %d", bit_count, error_count, cur_bit_pos);
  }
  else
  {

    ber_state_machine.bit_count[0]   = data_len_bits;
    ber_state_machine.error_count[0] = (data_len_bits/2);
    ber_state_machine.bit_count[1]   = 0;        /* force class 1b/class2 bit counts to 0 (don't acquire over these */
    ber_state_machine.error_count[1] = 0;
    ber_state_machine.bit_count[2]   = 0;
    ber_state_machine.error_count[2] = 0;


    FTM_MSG_HIGH("Unable to acquire PN sequence! Thresh = %d, BitPos = %d", ber_state_machine.acq_thresh, cur_bit_pos, 0);
  }
}

/*===========================================================================
FUNCTION     FTM_SE_BER_DISPATCH

DESCRIPTION  Command handler for Single ended BER subsystem.

DEPENDENCIES
  None.

PARAMETERS

RETURN VALUE
  None.

SIDE EFFECTS
===========================================================================*/
ftm_rsp_pkt_type ftm_se_ber_dispatch( ftm_common_pkt_type *cmd)
{

  ftm_rsp_pkt_type                rsp_pkt  = {FTM_RSP_DO_LEGACY, 0, NULL, FALSE};
  boolean                         cmd_fail = FALSE;
  ftm_se_ber_gen_rsp_pkt_type     *gen_rsp_payload;
  ftm_se_ber_status_rsp_pkt_type  *se_ber_rsp_payload;
  ftm_se_ber_config_type          *se_ber_config;
  ftm_se_ber_start_type           *se_ber_start;
  ftm_se_ber_set_loop_ab_order_type *se_ber_set_loop_ab;
  ftm_se_ber_set_expected_mcs_type  *se_ber_set_mcs;

  switch( cmd->ftm_hdr.cmd_id )
  {

    case FTM_SE_BER_CONFIGURE:

      FTM_MSG_HIGH("SE BER Configure Command",0,0,0);

      se_ber_config = ( ftm_se_ber_config_type *) cmd;

      /* validate params */

      if( se_ber_config->pn_type > FTM_SE_PN_TYPE_PN15 ||
          se_ber_config->se_mode > FTM_SE_MODE_AUTO )
      {
        cmd_fail = TRUE;
        ber_state_machine.initialized = FALSE;   /* invalid params, kill ber state machine */
      }
      else
      {
        ftm_se_ber_configure( se_ber_config->pn_type,
                              se_ber_config->se_mode,
                              se_ber_config->invert,
                              se_ber_config->acq_threshold,
                              se_ber_config->acq_lost_wm);
      }

      break;

    case FTM_SE_BER_START:         /* start logging command */

      se_ber_start = (ftm_se_ber_start_type *) cmd;
      /* validate params */

      if( se_ber_start->report_count >= FTM_SE_BER_REPORT_COUNT_MIN ||
          se_ber_start->report_count <= FTM_SE_BER_REPORT_COUNT_MAX )
      {
        ftm_se_ber_start( se_ber_start->report_count );
      }
      else
      {
        cmd_fail = TRUE;
      }
      break;

    case FTM_SE_BER_STOP:         /* stop logging command */

      ftm_se_ber_stop();
      break;

    case FTM_SE_BER_SET_LOOP_AB_ORDER:

      se_ber_set_loop_ab = (ftm_se_ber_set_loop_ab_order_type *) cmd;

      if( se_ber_set_loop_ab->order < FTM_SE_BER_LOOP_AB_ORDER_MAX)
      {
        ber_state_machine.loop_ab_order = se_ber_set_loop_ab->order;
      }
      break;

    case FTM_SE_BER_SET_EXPECTED_MCS:

      /* FTM does not have access to the MCS used, since it is determined after an
       * entire radio block is rec'd.  FTM gets data indications on a burst-by-burst
       * basis.  The user must therefore indicate which MCS is being used */

      se_ber_set_mcs = (ftm_se_ber_set_expected_mcs_type *) cmd;

      if( se_ber_set_mcs->expected_mcs >= FTM_SE_BER_MCS1 &&
          se_ber_set_mcs->expected_mcs <= FTM_SE_BER_MCS9)
      {
        ber_state_machine.expected_mcs = se_ber_set_mcs->expected_mcs;
      }

      break;

  case FTM_SE_BER_GET_SYNC_STATUS:
  break;

  case FTM_SE_BER_CLEAR_SYNC_STATUS:

    ftm_se_ber_clear_sync_status_ber();
  break;

    default:

      cmd_fail = TRUE;
      FTM_MSG_ERROR("Invalid SE BER command: %d", cmd->ftm_hdr.cmd_id,0,0);

      break;
  }

  if(cmd->ftm_hdr.cmd_id == FTM_SE_BER_GET_SYNC_STATUS)
  {
    rsp_pkt = ftmdiag_create_new_pkt( sizeof( ftm_se_ber_status_rsp_pkt_type));
    if( rsp_pkt.pkt_payload != NULL )
    {

      se_ber_rsp_payload = ( ftm_se_ber_status_rsp_pkt_type *) rsp_pkt.pkt_payload;

      se_ber_rsp_payload->header                  = cmd->diag_hdr;
      se_ber_rsp_payload->ftm_rsp_header.rsp_id   = (uint16) FTM_SE_BER_GEN_RESPONSE;
      se_ber_rsp_payload->ftm_rsp_header.rsp_cid  = cmd->ftm_hdr.cmd_id;
      se_ber_rsp_payload->ftm_rsp_header.reserved = 0;
      se_ber_rsp_payload->status                  = cmd_fail;
      se_ber_rsp_payload->total_bits              = sync_status.total_bits;
      se_ber_rsp_payload->total_bit_errors        = sync_status.total_bit_errors;
    }
  }
  else
  {
    rsp_pkt = ftmdiag_create_new_pkt( sizeof( ftm_se_ber_gen_rsp_pkt_type));

    if( rsp_pkt.pkt_payload != NULL )
    {

      gen_rsp_payload = ( ftm_se_ber_gen_rsp_pkt_type *) rsp_pkt.pkt_payload;

      gen_rsp_payload->header                  = cmd->diag_hdr;
      gen_rsp_payload->ftm_rsp_header.rsp_id   = (uint16) FTM_SE_BER_GEN_RESPONSE;
      gen_rsp_payload->ftm_rsp_header.rsp_cid  = cmd->ftm_hdr.cmd_id;
      gen_rsp_payload->ftm_rsp_header.reserved = 0;
      gen_rsp_payload->status                  = cmd_fail;
    }
  }

  return( rsp_pkt);
}



/*===========================================================================

FUNCTION

DESCRIPTION


DEPENDENCIES
  none

===========================================================================*/
void ftm_se_ber_log_ber_report( void)
{

  #ifdef FTM_HAS_LOGGING
  ftm_log_se_ber_report_type *ber_report;


  if( ber_log_status.log_state == FALSE )
  {
    return;
  }

  ber_report = ( ftm_log_se_ber_report_type *)
               ftm_log_malloc( (uint16)FTM_LOG_SE_BER,
                               sizeof( ftm_log_se_ber_report_type)
                               + (ber_log_status.report_limit-1)*sizeof(ftm_log_se_ber_sample_type)  );

  if( ber_report != NULL )
  {
    ber_report->log_id      = FTM_LOG_SE_BER_GENERIC_REPORT;
    ber_report->num_samples = ber_log_status.report_limit;

    memscpy( (void *) &ber_report->data[0], sizeof(ftm_log_se_ber_sample_type)*ber_log_status.report_limit,
             (void *) &ber_log_status.samples[0], sizeof(ftm_log_se_ber_sample_type)*ber_log_status.report_limit);

    log_commit( ber_report);
  }
  #endif

}



/*===========================================================================

FUNCTION

DESCRIPTION

DEPENDENCIES
  none

===========================================================================*/
void ftm_se_ber_add_log_sample( uint32 fn, uint8 slot_num )
{


  ber_log_status.samples[ber_log_status.report_index].fn         = fn;
  ber_log_status.samples[ber_log_status.report_index].slot_num   = slot_num;

  if ( slot_num < FTM_SE_BER_NUM_PN_SEQ ) 
  {
    if( ber_state_machine.acquired[slot_num] )
    {
      ber_log_status.samples[ber_log_status.report_index].pn_state   = FTM_LOG_SE_BER_PN_STATE_LOCKED;
    }
    else
    {
      ber_log_status.samples[ber_log_status.report_index].pn_state   = FTM_LOG_SE_BER_PN_STATE_UNLOCKED;
    }
  }
  else
  {
    FTM_MSG_ERROR(" slot_num exceeds array size = %d",slot_num,0,0);
  }

  ber_log_status.samples[ber_log_status.report_index].num_bits   = ber_state_machine.bit_count[0];
  ber_log_status.samples[ber_log_status.report_index].num_errors = ber_state_machine.error_count[0];
  if ( ber_log_status.samples[ber_log_status.report_index].pn_state == FTM_LOG_SE_BER_PN_STATE_LOCKED )
  {
  (void) ftm_se_ber_update_sync_status_ber(ber_state_machine.bit_count[0], ber_state_machine.error_count[0]);
  }
  ber_log_status.report_index++;

  if( ber_log_status.report_index >= ber_log_status.report_limit )
  {
    /* debug code */
    ftm_log_se_ber_sample_type   ber_debug = {0};
    uint8                        ber_i;
    /* end debug code */

    ber_log_status.report_index = 0;

    /* debug code */
    for( ber_i = 0; ber_i < ber_log_status.report_limit; ber_i++)
    {

      if( ber_log_status.samples[ ber_i].pn_state == FTM_LOG_SE_BER_PN_STATE_LOCKED)
      {
        ber_debug.num_bits += ber_log_status.samples[ ber_i].num_bits;
        ber_debug.num_errors += ber_log_status.samples[ ber_i].num_errors;
      }
    }

    FTM_MSG_MED("   ber report: num_bits: %d, num_errs: %d", ber_debug.num_bits,
                                                             ber_debug.num_errors,
                                                             0);
    /* end debug code */

    ftm_se_ber_log_ber_report();
  }
}



/*===========================================================================

FUNCTION

DESCRIPTION


DEPENDENCIES
  none

===========================================================================*/
void ftm_se_ber_log_ber_report_gsm_ab( void)
{

  #ifdef FTM_HAS_LOGGING
  ftm_log_se_ber_report_gsm_ab_type  *ber_report;

  FTM_MSG_LOW("Attempting to send Loop A/B packet, log_state: %d", ber_log_status.log_state,0,0);

  if( ber_log_status.log_state == FALSE )
  {
    return;
  }


  ber_report = ( ftm_log_se_ber_report_gsm_ab_type *)
               ftm_log_malloc( (uint16)FTM_LOG_SE_BER,
                               sizeof( ftm_log_se_ber_report_gsm_ab_type)
                               + (ber_log_status.report_limit-1)*sizeof(ftm_log_se_ber_gsm_loop_ab_sample_type)  );

  if( ber_report != NULL )
  {

    ber_report->log_id      = FTM_LOG_SE_BER_GSM_LOOP_AB_REPORT;
    ber_report->num_samples = ber_log_status.report_limit;

    memscpy( (void *) &ber_report->data[0], sizeof(ftm_log_se_ber_gsm_loop_ab_sample_type)*ber_log_status.report_limit,
            (void *) &ber_log_status.samples_loop_ab[0], sizeof(ftm_log_se_ber_gsm_loop_ab_sample_type)*ber_log_status.report_limit);

    log_commit( ber_report);

    FTM_MSG_LOW("==> Loop A/B log committed",0,0,0);
  }
  #endif
}



/*===========================================================================

FUNCTION

DESCRIPTION

DEPENDENCIES
  none

===========================================================================*/
void ftm_se_ber_add_log_sample_gsm_ab( uint32 fn, uint8 slot_num, boolean bfi )
{


  FTM_MSG_LOW("Ia bits: %d, Ib bits: %d, II bits: %d", ber_state_machine.bit_count[0],
                                                    ber_state_machine.bit_count[1],
                                                    ber_state_machine.bit_count[2]);

  FTM_MSG_LOW("Ia err: %d, Ib err: %d, II err: %d", ber_state_machine.error_count[0],
                                                    ber_state_machine.error_count[1],
                                                    ber_state_machine.error_count[2]);
  if ( slot_num < FTM_SE_BER_NUM_PN_SEQ ) 
  {
    FTM_MSG_LOW("BFI: %d, PN State: %d, Loop A/B report count: %d",bfi,
                ber_state_machine.acquired[slot_num],
                ber_log_status.report_index);
  }
  else
  {
    FTM_MSG_ERROR("Invalid slot number (%d >= %d)", slot_num, FTM_SE_BER_NUM_PN_SEQ, 0);
    return;
  }


  ber_log_status.samples_loop_ab[ber_log_status.report_index].fn         = fn;
  ber_log_status.samples_loop_ab[ber_log_status.report_index].slot_num   = slot_num;

  if( ber_state_machine.acquired[slot_num] )
  {
    ber_log_status.samples_loop_ab[ber_log_status.report_index].pn_state   = FTM_LOG_SE_BER_PN_STATE_LOCKED;
  }
  else
  {
    ber_log_status.samples_loop_ab[ber_log_status.report_index].pn_state   = FTM_LOG_SE_BER_PN_STATE_UNLOCKED;
  }

  ber_log_status.samples_loop_ab[ber_log_status.report_index].num_class1a_bits   = ber_state_machine.bit_count[0];
  ber_log_status.samples_loop_ab[ber_log_status.report_index].num_class1a_errors = ber_state_machine.error_count[0];
  (void) ftm_se_ber_update_sync_status_ber(ber_state_machine.bit_count[0], ber_state_machine.error_count[0]);

  ber_log_status.samples_loop_ab[ber_log_status.report_index].num_class1b_bits   = ber_state_machine.bit_count[1];
  ber_log_status.samples_loop_ab[ber_log_status.report_index].num_class1b_errors = ber_state_machine.error_count[1];
  (void) ftm_se_ber_update_sync_status_ber(ber_state_machine.bit_count[1], ber_state_machine.error_count[1]);

  ber_log_status.samples_loop_ab[ber_log_status.report_index].num_class2_bits    = ber_state_machine.bit_count[2];
  ber_log_status.samples_loop_ab[ber_log_status.report_index].num_class2_errors  = ber_state_machine.error_count[2];
  (void) ftm_se_ber_update_sync_status_ber(ber_state_machine.bit_count[2], ber_state_machine.error_count[2]);

  ber_log_status.samples_loop_ab[ber_log_status.report_index].bfi                = bfi;

  ber_log_status.report_index++;

  if( ber_log_status.report_index >= ber_log_status.report_limit )
  {
    /* debug vars */
    ftm_log_se_ber_gsm_loop_ab_sample_type  rber_debug = {0};
    uint8 loop_ab_fcount = 0;
    uint8 loop_ab_bfi_count = 0;
    uint8 rber_debug_i = 0;
    /* end debug vars */

    ber_log_status.report_index = 0;

    /* debug code */
    for( rber_debug_i = 0; rber_debug_i < ber_log_status.report_limit; rber_debug_i++)
    {

      loop_ab_fcount++;

      /* for FER, look at BFI independent of PN state */
      if( ber_log_status.samples_loop_ab[ rber_debug_i].bfi)
      {
        loop_ab_bfi_count++;
      }

      /* For RBER calculations, SE BER bit error stats should only be counted when PN state is locked */
      if( ber_log_status.samples_loop_ab[ rber_debug_i].pn_state == FTM_LOG_SE_BER_PN_STATE_LOCKED)
      {

        if( ! ber_log_status.samples_loop_ab[ rber_debug_i].bfi)
        {
          if( ber_log_status.samples_loop_ab[rber_debug_i].num_class1a_bits == 50 &&
              ber_log_status.samples_loop_ab[rber_debug_i].num_class1b_bits == 132 &&
              ber_log_status.samples_loop_ab[rber_debug_i].num_class2_bits == 78)
          {
            rber_debug.num_class1a_bits     +=  ber_log_status.samples_loop_ab[rber_debug_i].num_class1a_bits;
            rber_debug.num_class1a_errors   +=  ber_log_status.samples_loop_ab[rber_debug_i].num_class1a_errors;
            rber_debug.num_class1b_bits     +=  ber_log_status.samples_loop_ab[rber_debug_i].num_class1b_bits;
            rber_debug.num_class1b_errors   +=  ber_log_status.samples_loop_ab[rber_debug_i].num_class1b_errors;
            rber_debug.num_class2_bits      +=  ber_log_status.samples_loop_ab[rber_debug_i].num_class2_bits;
            rber_debug.num_class2_errors    +=  ber_log_status.samples_loop_ab[rber_debug_i].num_class2_errors;
          }
        }
      }
    }

    FTM_MSG_MED("   ber report: Ia bits: %d, Ib bits: %d, II bits: %d", rber_debug.num_class1a_bits,
                                                                        rber_debug.num_class1b_bits,
                                                                        rber_debug.num_class2_bits);
    FTM_MSG_MED("               Ia errs: %d, Ib errs: %d, II errs: %d", rber_debug.num_class1a_errors,
                                                                        rber_debug.num_class1b_errors,
                                                                        rber_debug.num_class2_errors);
    FTM_MSG_MED("               fcount: %d, bfi_count: %d", loop_ab_fcount, loop_ab_bfi_count, 0);
    /* end debug code */
    ftm_se_ber_log_ber_report_gsm_ab();
  }
}


// Function to update the synchronous status BER
// The values passed in add to the existing status
void ftm_se_ber_update_sync_status_ber( uint32 bits, uint32 bit_errors )
{
   // Update the BER stats
  sync_status.total_bits          = sync_status.total_bits + bits;
  sync_status.total_bit_errors    = sync_status.total_bit_errors + bit_errors;
}

// Function to clear the synchronous status BLER
void ftm_se_ber_clear_sync_status_ber( void  )
{
    // Clear all values in the synchronous status
    sync_status.total_bits          = 0;
    sync_status.total_bit_errors    = 0;

}

#endif /* FTM_HAS_SE_BER */
#endif /* FEATURE_FACTORY_TESTMODE */

