/*!
  @file
  rfca_mem_helper.cpp

  @brief
  This module contains Control code for RFCA memory helper functions.
 
*/

/*==============================================================================

  Copyright (c) 2011 - 2014 Qualcomm Technologies Incorporated. All Rights Reserved

  Qualcomm Proprietary

  Export of this technology or software is regulated by the U.S. Government.
  Diversion contrary to U.S. law prohibited.

  All ideas, data and information contained in or disclosed by
  this document are confidential and proprietary information of
  Qualcomm Technologies Incorporated and all rights therein are expressly reserved.
  By accepting this material the recipient agrees that this material
  and the information contained therein are held in confidence and in
  trust and will not be used, copied, reproduced in whole or in part,
  nor its contents revealed in any manner to others without the express
  written permission of Qualcomm Technologies Incorporated.

==============================================================================*/

/*==============================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfa/rf/rfca/src/rfca_mem_helper.cpp#1 $
   
when       who     what, where, why
--------   ---     ------------------------------------------------------------- 
01/14/14   aki     Added RFCA_Realloc_int
06/10/13   aki     Mainline RFCA 
04/26/12   aki     Removed F3 messages
10/28/11   aki     Initial revision
==============================================================================*/


#include "rfa_variation.h"
#include "comdef.h"

#include "rfca_mem_helper.h"
#include "modem_mem.h"
#include "msg.h"

/*----------------------------------------------------------------------------*/
void* RFCA_Malloc_int (unsigned int size, const char* caller)
{
  void* newMem = modem_mem_alloc(size, MODEM_MEM_CLIENT_RFA);
  return newMem;
}

/*----------------------------------------------------------------------------*/
void RFCA_Free_int (void* ptr, const char* caller)
{
  modem_mem_free(ptr, MODEM_MEM_CLIENT_RFA);
}

/*----------------------------------------------------------------------------*/
void* RFCA_Realloc_int (void* ptr, unsigned int new_size, const char* caller)
{
  return modem_mem_realloc(ptr, new_size, MODEM_MEM_CLIENT_RFA);
}
