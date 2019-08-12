#ifndef RLC_REPORT_TRACER_H
#define RLC_REPORT_TRACER_H
/**
@file rlc_report_tracer.h
@brief
This module implements reporting through QDSS tracer. 
*/
/*=============================================================================
  Copyright (c) 2012 QUALCOMM Technologies Incorporated.
  All rights reserved.
  Qualcomm Confidential and Proprietary
=============================================================================*/
/*=============================================================================

                   R L C   S E R V I C E   S U B S Y S T E M

GENERAL DESCRIPTION
  Private RLC functions for reporting through QDSS tracer.


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
Private function for initializing reporting through QDSS for a client.

@param[in]  pAttribute  Internal attribute pointer to fill in.

@return
RLC_SUCCESS --  Successfully setup QDSS attribute for the client.
RLC_FAILED --   Error in setting up QDSS attribute for the client.
 
@dependencies  
None.
*/

rlc_error_type rlc_report_tracer_init(rlc_attribute_type *pAttribute);

/** 
Private function for reporting through QDSS.

@param[in]  pAttribute  Internal attribute pointer to pass in client info.
@param[in]  string      Buffer of data to report on.

@return
RLC_SUCCESS --  Successfully reported through QDSS.
RLC_FAILED  --  Error reporting through QDSS.
 
@dependencies  
pAttribute must be setup properly in private tracer_init function.
*/

rlc_error_type rlc_report_tracer(rlc_attribute_type *pAttribute, char* string);

#endif /*RLC_REPORT_TRACER_H*/
