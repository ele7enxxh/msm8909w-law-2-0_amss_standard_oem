#ifndef RLC_REPORT_QXDM_H
#define RLC_REPORT_QXDM_H
/**
@file rlc_report_qxdm.h
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
11/28/12   cp      Initial revision.

=============================================================================*/


/*=============================================================================

                           INCLUDE FILES

=============================================================================*/

#include "rlc.h"

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

rlc_error_type rlc_report_qxdm(rlc_attribute_type *pAttribute, char* string);

#endif /*RLC_REPORT_QXDM_H*/
