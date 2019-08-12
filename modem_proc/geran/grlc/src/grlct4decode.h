/*****************************************************************************
***
*** TITLE
***
***  EGPRS RLC T.4 Decoder
***
***
*** DESCRIPTION
***
***  EGPRS RLC T.4 Decoder Definition
***
***
*** Copyright (c) 2004-2013 Qualcomm Technologies, Inc.
*** All Rights Reserved. QUALCOMM Proprietary.
***
*** Export of this technology or software is regulated by the U.S.
*** Government. Diversion contrary to U.S. law prohibited.
***
*****************************************************************************/


/*****************************************************************************
***
***                        EDIT HISTORY FOR MODULE
***
***  This section contains comments describing changes made to the module.
***  Notice that changes are listed in reverse chronological order.
***
*** $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/geran/grlc/src/grlct4decode.h#1 $
***
*** when       who     what, where, why
*** --------   ---     ------------------------------------------------------
*** 23/04/04    hv     Created
***
*****************************************************************************/

#ifndef INC_GRLCT4DECODE_H
#define INC_GRLCT4DECODE_H

/*****************************************************************************
***
***     Included Files
***
*****************************************************************************/

#include "geran_variation.h"
#include "customer.h" /* This include must be first before #ifdef FEATURE_  */


#include "gprsdef.h"
#include "gprsdef_g.h"
#include "geran_multi_sim.h"


/*****************************************************************************
***
***     Public Type Definitions
***
*****************************************************************************/



/*****************************************************************************
***
***     Public Defines & Constants
***
*****************************************************************************/


/*****************************************************************************
***
***     Public Data References
***
*****************************************************************************/


/*****************************************************************************
***
***     Public Macros
***
*****************************************************************************/

/*****************************************************************************
***
***     Public Functions
***
*****************************************************************************/

/*===========================================================================
===
===  FUNCTION      GRLC_T4_DECODE
===
===  DESCRIPTION
===
===    Performs T.4 decoding of the EGPRS PUAN.
===    This function returns pass if no errors were found in the compressed
===    bitmap and failed otherwise. This function does not return an expanded
===    bitmap as it takes time to format it here and to decode the bitmap from
===    the calling function. Instead the caller passes a pointer to an action
===    function. This is then invoked to take appropriate actions when a negative
===    or positive acknowledgement is found, from the runs of 1s and 0s.
===
===    The input parameters comprises:
===    - crbb_ptr: compressed bitmap (CRBB) in the form of a pointer to an uint32
===                which represents the 1st element of the CRBB array
===
===                       b31 ..........................................b0
===            crbb[0] :  [MSB-codeword1, MSB-codeword2, MSB-codeword3...]
===            crbb[1] :  [codeword3-LSB,                                ]
===            crbb[2] :  [                                              ]
===            crbb[3] :  [last-codeword-LSB, un-used bits un-used bits  ]
===
===    - cbscc: compressed bitmap starting colour code
==     - Lc: number of bits in CRBB. eg if Lc = 96 then only crbb[0,1,2] are used
===          and crbb[3] is not used
===    - act_p: pointer to action function. As each codeword is found, the resulting
===
===  DEPENDENCIES
===
===    xxx.
===
===  RETURN VALUE
===
===    FALSE if CRBB contains errors, TRUE otherwise
===
===  SIDE EFFECTS
===
===    xxx.
===
===========================================================================*/
boolean grlc_t4_decode
(
  gas_id_t  gas_id,

  /* pointer to uint32 representing the (first uint32 of the ) CRBB array */
  const uint32 *crbb_ptr,

  /* pointer to action function taking a uint16 to specify the position of
  ** the acknowledgement relative to the beginning of the bitmap and an uint8
  ** specifying the actual value of acknowledgement value (0 or 1) and returning
  ** void
  */
  void (*action_func) (gas_id_t gas_id, uint16 position, uint16 ack_value),

  /* compressed bitmap colour code (0 or 1) */
  uint16  cbscc,

  /* number of bits in the CRBB */
  uint16  lc,

  /* number of bits actually decoded */
  uint16  *bits_decoded
);



#endif /* INC_GRLCT4DECODE_H */

/*** EOF: don't remove! ***/
