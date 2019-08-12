/**
@file rlc_report_smd_protocol.c
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

#include "rlc_report_smd.h"
#include "rlc_report_smd_protocol.h"
#include "comdef.h"
#include "smd_lite.h"

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

void rlc_report_smd_protocol_fill_table(rlc_smd_protocol_table table[], uint32 size)
{
  uint32 smdTableIter = SMD_CHANNEL_TYPE_FIRST;
  
  for(smdTableIter = SMD_CHANNEL_TYPE_FIRST; smdTableIter < SMD_CHANNEL_TYPE_LAST || smdTableIter < size; smdTableIter++)
  {
    switch(smdTableIter)
    {
    case SMD_APPS_MODEM:
      table[smdTableIter].bit_mask = RLC_REPORT_TO_APPS_MASK|RLC_REPORT_TO_MPSS_MASK;
      break;
    case SMD_APPS_QDSP:
      table[smdTableIter].bit_mask = RLC_REPORT_TO_APPS_MASK|RLC_REPORT_TO_ADSP_MASK;
      break;
    case SMD_MODEM_QDSP:
      table[smdTableIter].bit_mask = RLC_REPORT_TO_MPSS_MASK|RLC_REPORT_TO_ADSP_MASK;
      break;
    case SMD_APPS_RIVA:
      table[smdTableIter].bit_mask = RLC_REPORT_TO_APPS_MASK|RLC_REPORT_TO_WCNSS_MASK;
      break;
    case SMD_MODEM_RIVA:
      table[smdTableIter].bit_mask = RLC_REPORT_TO_MPSS_MASK|RLC_REPORT_TO_WCNSS_MASK;
      break;
    case SMD_QDSP_RIVA:
      table[smdTableIter].bit_mask = RLC_REPORT_TO_WCNSS_MASK|RLC_REPORT_TO_ADSP_MASK;
      break;
    default:
      break;
    }

    /* Update the table number */
    table[smdTableIter].smd_channel = smdTableIter;  
  }

  /** Let's try opening ports now for each of these in the SMD table */

}
