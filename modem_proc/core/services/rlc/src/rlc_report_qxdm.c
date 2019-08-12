/**
@file rlc_report_qxdm.c
@brief
This module implements reporting through qxdm utilizing ERR/MSG (f3) 
function calls. 
*/
/*=============================================================================
  Copyright (c) 2012 QUALCOMM Technologies Incorporated.
  All rights reserved.
  Qualcomm Confidential and Proprietary
=============================================================================*/
/*=============================================================================

                   R L C   S E R V I C E   S U B S Y S T E M

GENERAL DESCRIPTION
  Private RLC functions for reporting through QXDM.


=============================================================================*/


/*=============================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: 

when       who     what, where, why
--------   ---     ------------------------------------------------------------
04/02/14   psu     Update diag macros using optimized versions.
11/28/12   cp      Initial revision.

=============================================================================*/


/*=============================================================================

                           INCLUDE FILES

=============================================================================*/

#include "rlc.h"
#include "rlc_v.h"
#include "rlc_os.h"
#include "rlc_report_qxdm.h"
//#include "msg.h"
#include "msg_diag_service.h"
#include "stdlib.h"
#include <string.h>
#include <stdio.h>

/*=============================================================================

                           FUNCTION DEFINITIONS

=============================================================================*/

/** 
Private function for reporting through QXDM.

@param[in]  pAttribute  Internal attribute pointer to pass in client info.
@param[in]  string      Buffer of data to report on.

@return
RLC_SUCCESS           --  Successfully reported through QXDM.
RLC_INVALID_PARAMETER --  Attribute is NULL.
 
@dependencies  
None.
*/

rlc_error_type rlc_report_qxdm(rlc_attribute_type *pAttribute, char* string)
{
  if( pAttribute == NULL )
  {
    return RLC_INVALID_PARAMETER;
  }
  
  MSG_1(MSG_SSID_DFLT, MSG_LEGACY_HIGH,"RLC: %s",string);
  return RLC_SUCCESS;
}
