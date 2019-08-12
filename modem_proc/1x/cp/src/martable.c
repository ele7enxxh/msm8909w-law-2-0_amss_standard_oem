/*===========================================================================

                    M A R K O V    T A B L E S

DESCRIPTION
  This file contains tables needed for the markov services

  Copyright (c) 1996 - 2013 Qualcomm Technologies, Inc. 
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

$PVCSPath: L:/src/asw/MSM5100/CP_REL_A/vcs/martable.c_v   1.1   03 Oct 2002 10:51:08   phosabet  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/1x/cp/src/martable.c#1 $ $DateTime: 2016/12/13 08:00:08 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
10/19/10   ag      Featurized 1xAdv related code and removed obsolete code.
07/15/10   ssh     Fixed Compiler Warnings.
11/06/09   ag      1X Advanced - Enhanced Markov Service Option Support
01/09/03   ph      removed FEATURE_NO_VBB
10/03/02   ph      Lint Cleanup
03/17/99    ms     Added FEATURE_NO_VBB
08/31/98   pms     Added Tan's Pascal calling convention code for ROM savings
07/29/98   lcc     Merged in ARM support.
04/08/96   dna     Changed external array names to follow convention.
02/28/96   dna     Created module from tables originally in martable.h.
                   The tables now reside in ROM.  I also made types for
                   the multi-dimentional arrays to make their refrences
                   more readable when moved into the Boot Block.

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "1x_variation.h"
#include "comdef.h"
#include "martable.h"

/*===========================================================================

                DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains definitions for constants, macros, types, variables
and other items needed by this module.

===========================================================================*/


/* Enums for indexing frame and bit error counter test structure */
/* which is declared as an array internally but accessed as a    */
/* structure from externally */

/* lookup tables for masks, use (fer_rate - 1) as index */


/* no. of 24 bits in packets
 * 16,40,80,171 rate set 1
 * 20,54,124,266 rate set 2
 * Data block of type rate 1/8 non-critical is generated at rate 1/8.
*/
mar_2x5_type const mar_tri_bytes = { {1, 1, 2, 4, 7},
                                     {1, 1, 3, 6, 11} };

/* mask last dword to fill packet */
mar_2x5dw_type const mar_last_mask =
{ {0xFFFF, 0xFFFF, 0xFFFF, 0xFF, 0xE0FFFFFF},     //lint !e570 loss of sign
  {0xF0FFFF, 0xF0FFFF, 0x0FC, 0xF0, 0xC0FFFFFF} }; //lint !e570 loss of sign

/* no. of bytes in packet - 1 */
mar_2x4_type const mar_bytes = { {16/8-1, 40/8-1, 80/8-1, 171/8},
                                 {20/8, 54/8, 124/8, 266/8} };

/* mask last byte to compare bits */
mar_2x4_type const mar_last_byte =  { {0xFF, 0xFF, 0xFF, 0xE0},
                                      {0xF0, 0xFC, 0xF0, 0xC0} };

/* lookup table for bits set in a byte used for bit error counting */
byte const mar_ones [256] = {
 0,  1,  1,  2,  1,  2,  2,  3,
 1,  2,  2,  3,  2,  3,  3,  4,
 1,  2,  2,  3,  2,  3,  3,  4,
 2,  3,  3,  4,  3,  4,  4,  5,
 1,  2,  2,  3,  2,  3,  3,  4,
 2,  3,  3,  4,  3,  4,  4,  5,
 2,  3,  3,  4,  3,  4,  4,  5,
 3,  4,  4,  5,  4,  5,  5,  6,
 1,  2,  2,  3,  2,  3,  3,  4,
 2,  3,  3,  4,  3,  4,  4,  5,
 2,  3,  3,  4,  3,  4,  4,  5,
 3,  4,  4,  5,  4,  5,  5,  6,
 2,  3,  3,  4,  3,  4,  4,  5,
 3,  4,  4,  5,  4,  5,  5,  6,
 3,  4,  4,  5,  4,  5,  5,  6,
 4,  5,  5,  6,  5,  6,  6,  7,
 1,  2,  2,  3,  2,  3,  3,  4,
 2,  3,  3,  4,  3,  4,  4,  5,
 2,  3,  3,  4,  3,  4,  4,  5,
 3,  4,  4,  5,  4,  5,  5,  6,
 2,  3,  3,  4,  3,  4,  4,  5,
 3,  4,  4,  5,  4,  5,  5,  6,
 3,  4,  4,  5,  4,  5,  5,  6,
 4,  5,  5,  6,  5,  6,  6,  7,
 2,  3,  3,  4,  3,  4,  4,  5,
 3,  4,  4,  5,  4,  5,  5,  6,
 3,  4,  4,  5,  4,  5,  5,  6,
 4,  5,  5,  6,  5,  6,  6,  7,
 3,  4,  4,  5,  4,  5,  5,  6,
 4,  5,  5,  6,  5,  6,  6,  7,
 4,  5,  5,  6,  5,  6,  6,  7,
 5,  6,  6,  7,  6,  7,  7,  8   };

/*===========================================================================

           MARKOV TABLE BINARY PROBABILITIES FOR NON DTX-able MSO

Probabilities are listed as values which ran# must be less than to be
valid. Probs are listed eighth, eighth+quarter, eighth+quarter+half.
If ran# is greater or equal than last value then transition to full.

Sn = State at frame n.  Rn = Rate at frame n.
State is table 1st dim index.     Transition is index to table 2nd dim.

Sn+1 = 4 * ( Sn mod 4 ) + ( 3 - Trans ).
Rn+1 = 4 - ( Sn+1 mod 4 ).

===========================================================================*/

mar_prob_non_dtx_type const mar_scstp_non_dtx_prob = {

{
  {0,     0,     2916},                 /* Variable Rate Probs */
  {0,     20906, 25264},
  {0,     0,     0},
  {0,     0,     0},
  {0,     0,     4915},
  {0,     17170, 24969},
  {21856, 25887, 27099},
  {0,     0,     0},
  {0,     0,     4522},
  {0,     5472,  16384},
  {21856, 21856, 24576},
  {28246, 29622, 30802},
  {0,     0,     5472},
  {0,     6554,  6554},
  {28377, 28934, 29491},
  {29753, 32473, 32571},
},

{
  {32768,32768,32768},                  /* Fixed Eighth Rate Probs */
  {32768,32768,32768},
  {32768,32768,32768},
  {32768,32768,32768},
  {32768,32768,32768},
  {32768,32768,32768},
  {32768,32768,32768},
  {32768,32768,32768},
  {32768,32768,32768},
  {32768,32768,32768},
  {32768,32768,32768},
  {32768,32768,32768},
  {32768,32768,32768},
  {32768,32768,32768},
  {32768,32768,32768},
  {32768,32768,32768},
},

{
  {0,32768,32768},                      /* Fixed Quarter Rate Probs */
  {0,32768,32768},
  {0,32768,32768},
  {0,32768,32768},
  {0,32768,32768},
  {0,32768,32768},
  {0,32768,32768},
  {0,32768,32768},
  {0,32768,32768},
  {0,32768,32768},
  {0,32768,32768},
  {0,32768,32768},
  {0,32768,32768},
  {0,32768,32768},
  {0,32768,32768},
  {0,32768,32768},
},

{
  {0,0,32768},                          /* Fixed Half Rate Probs */
  {0,0,32768},
  {0,0,32768},
  {0,0,32768},
  {0,0,32768},
  {0,0,32768},
  {0,0,32768},
  {0,0,32768},
  {0,0,32768},
  {0,0,32768},
  {0,0,32768},
  {0,0,32768},
  {0,0,32768},
  {0,0,32768},
  {0,0,32768},
  {0,0,32768},
},

{
  {0,0,0},                              /* Fixed Full Rate Probs */
  {0,0,0},
  {0,0,0},
  {0,0,0},
  {0,0,0},
  {0,0,0},
  {0,0,0},
  {0,0,0},
  {0,0,0},
  {0,0,0},
  {0,0,0},
  {0,0,0},
  {0,0,0},
  {0,0,0},
  {0,0,0},
  {0,0,0}
}
};


#ifdef FEATURE_IS2000_1X_ADV
/*===========================================================================

           MARKOV TABLE BINARY PROBABILITIES FOR DTX-able MSO

Probabilities are listed as values which ran# must be less than to be
valid. Probs are listed eighth, eighth+quarter, eighth+quarter+half.
If ran# is greater or equal than last value then transition to full.

===========================================================================*/

mar_prob_dtx_type const mar_scstp_dtx_prob = {

{
  {0,     0,     14728, 17120},                   /* Variable Rate Probs */
  {0,     12689, 26493, 31025},
  {0,     849,   6718,  7727},
  {0,     0,     0,     0},
  {0,     0,     0,     0},
  {0,     0,     1074,  2149},
  {0,     3400,  13911, 22876},
  {0,     3848,  26810, 26810},
  {20177, 20177, 31858, 31858},
  {0,     0,     0,     0},
  {0,     0,     2345,  4013},
  {0,     10615, 25384, 30922},
  {0,     2253,  24046, 24072},
  {23235, 23235, 32649, 32649},
  {0,     0,     0,     0},
  {0,     0,     0,     3277},
  {0,     0,     0,     0},
  {0,     1184,  21714, 21714},
  {0,     0,     0,     0},
  {29445, 29445, 32524, 32524},
  {0,     0,     2621,  3932},
  {0,     0,     0,     0},
  {0,     1020,  17121, 17121},
  {32208, 32208, 32639, 32639},
  {31119, 32418, 32739, 32739},
},

{
  {0,32768,32768,32768},                         /* Fixed Eighth Rate Probs */
  {0,32768,32768,32768},
  {0,32768,32768,32768},
  {0,32768,32768,32768},
  {0,32768,32768,32768},
  {0,32768,32768,32768},
  {0,32768,32768,32768},
  {0,32768,32768,32768},
  {0,32768,32768,32768},
  {0,32768,32768,32768},
  {0,32768,32768,32768},
  {0,32768,32768,32768},
  {0,32768,32768,32768},
  {0,32768,32768,32768},
  {0,32768,32768,32768},
  {0,32768,32768,32768},
  {0,32768,32768,32768},
  {0,32768,32768,32768},
  {0,32768,32768,32768},
  {0,32768,32768,32768},
  {0,32768,32768,32768},
  {0,32768,32768,32768},
  {0,32768,32768,32768},
  {0,32768,32768,32768},
  {0,32768,32768,32768},
},

{
  {0,0,32768,32768},                            /* Fixed Quarter Rate Probs */
  {0,0,32768,32768},
  {0,0,32768,32768},
  {0,0,32768,32768},
  {0,0,32768,32768},
  {0,0,32768,32768},
  {0,0,32768,32768},
  {0,0,32768,32768},
  {0,0,32768,32768},
  {0,0,32768,32768},
  {0,0,32768,32768},
  {0,0,32768,32768},
  {0,0,32768,32768},
  {0,0,32768,32768},
  {0,0,32768,32768},
  {0,0,32768,32768},
  {0,0,32768,32768},
  {0,0,32768,32768},
  {0,0,32768,32768},
  {0,0,32768,32768},
  {0,0,32768,32768},
  {0,0,32768,32768},
  {0,0,32768,32768},
  {0,0,32768,32768},
  {0,0,32768,32768},
},

{
  {0,0,0,32768},                                 /* Fixed Half Rate Probs */
  {0,0,0,32768},
  {0,0,0,32768},
  {0,0,0,32768},
  {0,0,0,32768},
  {0,0,0,32768},
  {0,0,0,32768},
  {0,0,0,32768},
  {0,0,0,32768},
  {0,0,0,32768},
  {0,0,0,32768},
  {0,0,0,32768},
  {0,0,0,32768},
  {0,0,0,32768},
  {0,0,0,32768},
  {0,0,0,32768},
  {0,0,0,32768},
  {0,0,0,32768},
  {0,0,0,32768},
  {0,0,0,32768},
  {0,0,0,32768},
  {0,0,0,32768},
  {0,0,0,32768},
  {0,0,0,32768},
  {0,0,0,32768},
},

{
  {0,0,0,0},                                      /* Fixed Full Rate Probs */
  {0,0,0,0},
  {0,0,0,0},
  {0,0,0,0},
  {0,0,0,0},
  {0,0,0,0},
  {0,0,0,0},
  {0,0,0,0},
  {0,0,0,0},
  {0,0,0,0},
  {0,0,0,0},
  {0,0,0,0},
  {0,0,0,0},
  {0,0,0,0},
  {0,0,0,0},
  {0,0,0,0},
  {0,0,0,0},
  {0,0,0,0},
  {0,0,0,0},
  {0,0,0,0},
  {0,0,0,0},
  {0,0,0,0},
  {0,0,0,0},
  {0,0,0,0},
  {0,0,0,0},
},

{
  {32768,32768,32768,32768},                     /* Fixed Non-critical Eighth Rate Probs */
  {32768,32768,32768,32768},
  {32768,32768,32768,32768},
  {32768,32768,32768,32768},
  {32768,32768,32768,32768},
  {32768,32768,32768,32768},
  {32768,32768,32768,32768},
  {32768,32768,32768,32768},
  {32768,32768,32768,32768},
  {32768,32768,32768,32768},
  {32768,32768,32768,32768},
  {32768,32768,32768,32768},
  {32768,32768,32768,32768},
  {32768,32768,32768,32768},
  {32768,32768,32768,32768},
  {32768,32768,32768,32768},
  {32768,32768,32768,32768},
  {32768,32768,32768,32768},
  {32768,32768,32768,32768},
  {32768,32768,32768,32768},
  {32768,32768,32768,32768},
  {32768,32768,32768,32768},
  {32768,32768,32768,32768},
  {32768,32768,32768,32768},
  {32768,32768,32768,32768}
}
};
#endif /* FEATURE_IS2000_1X_ADV */

