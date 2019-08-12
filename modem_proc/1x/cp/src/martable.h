#ifndef MARTABLE_H
#define MARTABLE_H
/*===========================================================================

           B E R    T A B L E   H E A D E R    F I L E

DESCRIPTION
  This file contains declarations associated with the MAR services

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

$PVCSPath: L:/src/asw/MSM5100/CP_REL_A/vcs/martable.h_v   1.0.2.0   30 Nov 2001 16:59:16   fchan  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/1x/cp/src/martable.h#1 $ $DateTime: 2016/12/13 08:00:08 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
10/19/10   ag      Featurized 1xAdv related code and removed obsolete code.
11/06/09   ag      1X Advanced - Enhanced Markov Service Option Support
01/09/03   ph      removed FEATURE_NO_VBB
03/17/99   ms      Added FEATURE_NO_VBB
08/31/98   pms     Added Tan's Pascal calling convention code for ROM savings
07/29/98   lcc     Merged in ARM support.
04/08/96   dna     Changed external array names to follow convention.
02/28/96   dna     Moved tables to a C file and added the proper external
                   declarations here.
06/14/95   gb      Added support for Rate set 2 service options.
06/27/91   gb      Created file.

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "1x_variation.h"
#include "comdef.h"

/* <EJECT> */
/*===========================================================================

                        DATA DECLARATIONS

===========================================================================*/

/* Types for different array sizes */
typedef word mar_2x4_type [2] [4];
typedef word mar_2x5_type [2] [5];
typedef dword mar_2x5dw_type [2] [5];
typedef word mar_prob_non_dtx_type [5] [16] [3];
#ifdef FEATURE_IS2000_1X_ADV
typedef word mar_prob_dtx_type [6] [25] [4];
#endif /* FEATURE_IS2000_1X_ADV */


/* Enums for indexing frame and bit error counter test structure */
/* which is declared as an array internally but accessed as a    */
/* structure from externally */

/* lookup tables for masks, use (fer_rate - 1) as index */


/* no. of 24 bits in packets
 * 16,40,80,171,16 rate set 1
 * 20,54,124,266,20 rate set 2
*/
extern mar_2x5_type const mar_tri_bytes;

/* mask last dword to fill packet */
extern mar_2x5dw_type const mar_last_mask;

/* no. of bytes in packet - 1 */
extern mar_2x4_type const mar_bytes;

/* mask last byte to compare bits */
extern mar_2x4_type const mar_last_byte;


 /* lookup table for bits set in a byte used for bit error counting */

extern byte const mar_ones[];

#define NUM_OF_ONES_IN (byte b) ones [b]

/*===========================================================================

           MARKOV TABLE BINARY PROBABILITIES

Probabilities are listed as values which ran# must be less than to be
valid. Probs are listed eighth, eighth+quarter, eighth+quarter+half.
If ran# is greater or equal than last value then transition to full.

Sn = State at frame n.  Rn = Rate at frame n.
State is table 1st dim index.     Transition is index to table 2nd dim.

Sn+1 = 4 * ( Sn mod 4 ) + ( 3 - Trans ).
Rn+1 = 4 - ( Sn+1 mod 4 ).

===========================================================================*/

extern mar_prob_non_dtx_type const mar_scstp_non_dtx_prob;
#ifdef FEATURE_IS2000_1X_ADV
extern mar_prob_dtx_type const mar_scstp_dtx_prob;
#endif /* FEATURE_IS2000_1X_ADV */

#endif /* MARTABLE_H */
