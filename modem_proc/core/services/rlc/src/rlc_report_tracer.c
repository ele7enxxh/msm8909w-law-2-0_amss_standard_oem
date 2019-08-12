/**
@file rlc_report_tracer.c
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

#include "rlc_os.h"
#include "rlc.h"
#include "rlc_v.h"
#include "rlc_report_tracer.h"
#include "tracer.h"
#include "comdef.h"
#include <memory.h>
#include <string.h>
#include <stdio.h>

const char* rlc_generic_tracer_report_string = "The value of variable 1 is 0x%llx and variable 2 is 0x%llx\n";

/**
Functions
*/
rlc_error_type rlc_report_tracer_init(rlc_attribute_type *pAttribute)
{

  tracer_client_handle_t hTracerHandle = NULL;
  tracer_return_enum_t rTracerReturn = TRACER_RETURN_ERROR;

  if(pAttribute == NULL)
  {
    return RLC_FAILED;
  }

  if(pAttribute->report_ptr == NULL)
  {
    return RLC_FAILED;
  }

  rTracerReturn = tracer_register( &hTracerHandle, 
                                  (tracer_ost_entity_id_enum_t)TRACER_ENTITY_DEFAULT, 
                                  (tracer_ost_protocol_id_t)TRACER_PROTOCOL_DEFAULT);
  if(rTracerReturn == TRACER_RETURN_ERROR || hTracerHandle == NULL)
  {
    return RLC_FAILED;
  }

  pAttribute->report_ptr->report_handle = (rlc_report_output_handle)hTracerHandle;

  return RLC_SUCCESS;
}


rlc_error_type rlc_report_tracer(rlc_attribute_type *pAttribute, char* string)
{
  if(pAttribute == NULL ||
    pAttribute->report_ptr == NULL ||
    pAttribute->report_ptr->report_handle == NULL ||
    string == NULL)
  {
    return RLC_INVALID_PARAMETER;
  }
  
  if(TRACER_RETURN_SUCCESS != tracer_data( (tracer_client_handle_t)pAttribute->report_ptr->report_handle, 
                               (tracer_options_t)(TRACER_OPT_TIMESTAMP|TRACER_OPT_GUARANTEE),
                               rlc_strnlen(string, RLC_REPORT_BUF_SIZE), /* safer than strlen */
                               string ))
  {
    return RLC_FAILED;
  }
  return RLC_SUCCESS;
}
