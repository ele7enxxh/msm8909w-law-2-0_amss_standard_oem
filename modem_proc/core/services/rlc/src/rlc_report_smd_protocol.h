#ifndef RLC_REPORT_SMD_PROTOCOL_H
#define RLC_REPORT_SMD_PROTOCOL_H
/**
@file rlc_report_smd_protocol.h
@brief
This module searches for SMD ports that are available. 
*/
/*=============================================================================
  Copyright (c) 2012 QUALCOMM Technologies Incorporated.
  All rights reserved.
  Qualcomm Confidential and Proprietary
=============================================================================*/
/*=============================================================================

                   R L C   S E R V I C E   S U B S Y S T E M

GENERAL DESCRIPTION
  Private RLC functions for setting up SMD ports that are available.


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
#include "rlc_report_smd.h"
#include "comdef.h"

/*=============================================================================

                           FUNCTION DEFINITIONS

=============================================================================*/

/** 
Private function for manually mapping the ports that are available in a 
way that RLC can understand it.

@param[in]  table  Pointer to a table array.
@param[in]  size   Length of the table array.

@return
None.
 
@dependencies  
None.
*/

void rlc_report_smd_protocol_fill_table(rlc_smd_protocol_table table[], uint32 size);

#endif /*RLC_REPORT_SMD_PROTOCOL_H*/
