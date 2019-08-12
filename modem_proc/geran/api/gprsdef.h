#ifndef INC_GPRSDEF_H
#define INC_GPRSDEF_H
/*****************************************************************************
***
*** TITLE
***
***  GPRS COMMON DEFINITIONS
***
***
*** DESCRIPTION
***
***  Provides a common point of reference for all GPRS file specific data
***  data types, defines, scaling factors and wrapping functions. All GPRS
***  modules should include this file.
***
***
*** Copyright (c) 2001-2013 Qualcomm Technologies, Inc.
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
*** $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/geran/api/gprsdef.h#1 $
***
*** when       who     what, where, why
*** --------   ---     ------------------------------------------------------
*** 14-01-12   jj      CR336812 New feature Reset recovery mechanism
*** 11-06-06   tjw     Split API content from original file
*** 
*****************************************************************************/

#include "geran_grr_api.h"

/* Macro ARFCNS_EQUAL invokes a function for GERAN clients because the macro 
   has a conditional compilation which is not permitted in the API */
#define ARFCNS_EQUAL(a,b) arfcns_equal(a,b)

/*****************************************************************************
***
***     Public Type Definitions
***
*****************************************************************************/

/* Defines enum to define the setup of either a GPRS
 * or EGPRS TBF
 */

typedef enum
{
  TBF_MODE_INVALID,
  TBF_MODE_GPRS,
  TBF_MODE_EGPRS

} tbf_mode_T;

/* Defines enum to define the setup of various test modes */

typedef enum
{
  TEST_MODE_OFF,
  TEST_MODE_B,
  TEST_MODE_SRB,
  TEST_MODE_A,
  TEST_MODE_INVALID = 0xff
} test_mode_T;

typedef enum
{
  GPDU_LOW_PRIORITY  = 0,
  GPDU_HIGH_PRIORITY = 1,
  GPDU_MAX_PRIORITY  = 2
} geran_pdu_priority_t;
// ===========================================================================
//  GPRS MAC Primary Definitions
// ===========================================================================

/* Defines MAX size of PCCCH DL control blocks from L1 */
#define MAX_SIZE_OF_CONTROL_BLOCK_DATA  22   /*size of control data within RLC/MAC radio block in octets*/
#define SIZE_OF_CONTROL_RADIO_BLOCK     23   /*size of RLC/MAC radio block in octets*/

/* Defines maximum number of timeslots per frame */
#define MAX_TN 8

#endif /* !INC_GPRSDEF_H */
