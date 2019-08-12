#ifndef PLT_TEST_CMD_H
#define PLT_TEST_CMD_H
/*===========================================================================

                  TEST COMMAND HEADER
DESCRIPTION

  This file contains prototypes, #defines, and enums which constitute
  the public interface for shared test command functionality in the phone.

    Copyright (c) 1999, 2000
                  by Qualcomm Technologies, Inc.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.


$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfa/api/cdma/test_cmd.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
04/01/11   sar     Removed #ifdef's to comply with CMI.
05/18/04   rsr     Removed SWIG support
11/15/00   bgc     Initial revision.

===========================================================================*/

#include "comdef.h"
/*--------------------------------------------------------------------------
                       TEST COMMAND STATUS ENUM TYPE
--------------------------------------------------------------------------*/
typedef enum 
{
  TEST_SUCCESS,
  TEST_INVALID_CMD,
  TEST_INVALID_STATE,
  TEST_NO_SUCCESS,
  test_cmd_status_sizer = 0x7FFFFFFF
  /* enforces the enums to be 32 bits */
} test_cmd_status_type;

typedef test_cmd_status_type ftm_cmd_status_type;

extern test_cmd_status_type test_cmd_status;

#endif
