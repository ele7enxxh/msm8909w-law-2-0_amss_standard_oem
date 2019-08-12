/*****************************************************************************
***
*** TITLE
***
***  EGPRS RLC T.4 Encoder
***
***
*** DESCRIPTION
***
***  EGPRS RLC T.4 Encoder Definition
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
*** $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/geran/grlc/src/grlct4encode.h#1 $   
***
*** when       who     what, where, why
*** --------   ---     ------------------------------------------------------
*** 07/23/04    hv     Created
***
*****************************************************************************/

#ifndef INC_GRLCT4ENCODE_H
#define INC_GRLCT4ENCODE_H

/*****************************************************************************
***
***     Included Files
***
*****************************************************************************/

#include "geran_variation.h"
#include "customer.h" /* This include must be first before #ifdef FEATURE_  */


#include "gprsdef.h"
#include "gprsdef_g.h"
#include "msg.h"

/*****************************************************************************
***
***     Public Type Definitions
***
*****************************************************************************/

/*--------------------------------------------------------------------------------
** Structure to hold the resultant code word(s) returned by the encoding function
**--------------------------------------------------------------------------------
*/
typedef struct
{
  /* Make-up code word and its length in bits. If mcode_len is 0 then the make-up code
  ** word does not exist. Maximum mcode_len value is 13
  */
  uint16  mcode;
  uint16  mcode_len;

  /* Terminating code word and its length in bits. A meaningful terminating code word
  ** is always returned idf the unpit params are valid. Minimum tcode_len value is 2
  */
  uint16  tcode;
  uint16  tcode_len;

} t4_encode_res_t;

/*****************************************************************************
***
***     Public Functions
***
*****************************************************************************/

/*===========================================================================
===
===  FUNCTION    GRLC_T4_ENCODE()
===
===  DESCRIPTION
===
===    This function performs T.4 encoding on the given run and run length specified.
===    The function returns TRUE if the given parameters are valid and the result
===    is returned in the passed data structure. Depending on the run length the 
===    result may contain the make-up/terminating code word pair or just the 
===    a terminating code word. If the make-up code word does not exist then its
===    run length is set to 0.
===
===  DEPENDENCIES
===
===    none
===
===  PARAMS
===
===    run: 0 or 1. Considered invalid otherwise
===    run_length: from 0 to 960
===    res_ptr: results stored back in the passed structure. 
===         If run and run_length are valid tcode and tcode_len always contain 
===         meaningful values. mcode_len is set to 0 if make-up code does not exist
===  
===  RETURN VALUE
===
===    Boolean TRUE if given params are valid, FALSE otherwise
===
===  SIDE EFFECTS
===
===========================================================================*/
boolean grlc_t4_encode
(
  t4_encode_res_t   *res_ptr,
  uint16            run,
  uint16            run_length
);


#endif /* INC_GRLCT4ENCODE_H */

/*** EOF: don't remove! ***/
