#ifndef AMRSUPI_H
#define AMRSUPI_H

/*===========================================================================

                  M U L T I M O D E   V O I C E   S E R V I C E S
                    A M R   F R A M I N G   P R O C E S S I N G

DESCRIPTION
  This header file contains definitions used by the AMR framing functions.

REFERENCES
  3G TS 26.093 - "AMR Speech Codec; Source Controlled Rate Operation"
  3G TS 26.101 - "AMR Speech Codec; Frame structure"

Copyright (C) 2003-2009, 2013 QUALCOMM Technologies Incorporated.
All rights reserved.
Qualcomm Confidential and Proprietary

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/avs/vsd/common/mvs/inc/amrsupi.h#1 $
$Author: mplcsds1 $
===========================================================================*/

/* <EJECT> */
/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

when       who     what, where, why
--------   ---     ----------------------------------------------------------
06/26/13   sud     Updated Copyright.
08/18/09    sr     Adapting to CVD.
06/01/09    at     Added AMR_WB support.
04/17/03    sm     Initial revision taken from mvsamri.h

===========================================================================*/

/* <EJECT> */
/*===========================================================================

                      INCLUDE FILES FOR MODULE

===========================================================================*/

#include "comdef.h"

/*===========================================================================

                    D A T A   D E F I N I T I O N S

===========================================================================*/
 //NM_AUDIO_LEGACY_BEGIN

/* Definitions for the number of bits in classes A, B, and C for each AMR
** codec mode.
*/
#define AMR_CLASS_A_BITS_BAD   0

#define AMR_CLASS_A_BITS_SID  39
#define AMR_CLASS_A_BITS_AWB_SID  40

#define AMR_CLASS_A_BITS_475  42
#define AMR_CLASS_B_BITS_475  53
#define AMR_CLASS_C_BITS_475   0

#define AMR_CLASS_A_BITS_515  49
#define AMR_CLASS_B_BITS_515  54
#define AMR_CLASS_C_BITS_515   0

#define AMR_CLASS_A_BITS_590  55
#define AMR_CLASS_B_BITS_590  63
#define AMR_CLASS_C_BITS_590   0

#define AMR_CLASS_A_BITS_670  58
#define AMR_CLASS_B_BITS_670  76
#define AMR_CLASS_C_BITS_670   0

#define AMR_CLASS_A_BITS_740  61
#define AMR_CLASS_B_BITS_740  87
#define AMR_CLASS_C_BITS_740   0

#define AMR_CLASS_A_BITS_795  75
#define AMR_CLASS_B_BITS_795  84
#define AMR_CLASS_C_BITS_795   0

#define AMR_CLASS_A_BITS_102  65
#define AMR_CLASS_B_BITS_102  99
#define AMR_CLASS_C_BITS_102  40

#define AMR_CLASS_A_BITS_122  81
#define AMR_CLASS_B_BITS_122 103
#define AMR_CLASS_C_BITS_122  60

#define AMR_CLASS_A_BITS_660  54
#define AMR_CLASS_B_BITS_660  78
#define AMR_CLASS_C_BITS_660   0

#define AMR_CLASS_A_BITS_885   64
#define AMR_CLASS_B_BITS_885   113
#define AMR_CLASS_C_BITS_885    0

#define AMR_CLASS_A_BITS_1265  72
#define AMR_CLASS_B_BITS_1265  181
#define AMR_CLASS_C_BITS_1265   0

#define AMR_CLASS_A_BITS_1425  72
#define AMR_CLASS_B_BITS_1425  213
#define AMR_CLASS_C_BITS_1425   0

#define AMR_CLASS_A_BITS_1585  72
#define AMR_CLASS_B_BITS_1585  245
#define AMR_CLASS_C_BITS_1585   0

#define AMR_CLASS_A_BITS_1825  72
#define AMR_CLASS_B_BITS_1825  293
#define AMR_CLASS_C_BITS_1825   0

#define AMR_CLASS_A_BITS_1985  72
#define AMR_CLASS_B_BITS_1985  325
#define AMR_CLASS_C_BITS_1985   0 

#define AMR_CLASS_A_BITS_2305  72
#define AMR_CLASS_B_BITS_2305  389
#define AMR_CLASS_C_BITS_2305   0

#define AMR_CLASS_A_BITS_2385  72
#define AMR_CLASS_B_BITS_2385  405
#define AMR_CLASS_C_BITS_2385   0

/* Structure containing bit orders
*/
typedef struct tag_amrsup_frame_order_type{
  int   len_a;
  uint16 *class_a;
  int   len_b;
  uint16 *class_b;
  int   len_c;
  uint16 *class_c;
} amrsup_frame_order_type;

//NM_AUDIO_LEGACY_END
#endif /* AMRSUPI_H */
