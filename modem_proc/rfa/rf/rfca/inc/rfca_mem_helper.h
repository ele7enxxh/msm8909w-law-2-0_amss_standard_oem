/*!
  @file
  rfca_mem_helper.h
 
  @brief
  Memory helper routines for RFCA. 

*/

/*==============================================================================

  Copyright (c) 2011-2014 Qualcomm Technologies Incorporated. All Rights Reserved

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

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfa/rf/rfca/inc/rfca_mem_helper.h#1 $
   
when       who     what, where, why
--------   ---     ------------------------------------------------------------- 
01/14/14   aki     Added RFCA_Realloc
06/10/13   aki     Mainline RFCA
01/25/12   aki     Documentation update
10/28/11   aki     Initial revision

==============================================================================*/


#ifndef _RFCA_MEM_HELPER_H_
#define _RFCA_MEM_HELPER_H_

#include "rfa_variation.h"
#include "comdef.h"

#ifdef __cplusplus
extern "C" {
#endif

/*----------------------------------------------------------------------------*/
/*!  
  @brief Allocates memory 

  @param [in] size   Size of memory block to allocate
  @param [in] caller Caller indetifier, i.e. __FUNCTION__ macro 
 
  @return Pointer to memory block or NULL if allocation failed.

*/
void* RFCA_Malloc_int (unsigned int size, const char* caller);

/*----------------------------------------------------------------------------*/
/*!  
  @brief Frees memory previously allocated from modem heap

  @param [in] ptr    Pointer to allocated memory buffer.
  @param [in] caller Caller indetifier, i.e. __FUNCTION__ macro 

*/
void RFCA_Free_int (void* ptr, const char* caller);

/*----------------------------------------------------------------------------*/
/*!  
  @brief Reallocates memory 

  @param [in] ptr        Pointer to allocated memory buffer.
  @param [in] new_size   New size of memory block
  @param [in] caller     Caller indetifier, i.e. __FUNCTION__ macro 

*/
void* RFCA_Realloc_int (void* ptr, unsigned int new_size, const char* caller);

#define RFCA_Malloc(x)  RFCA_Malloc_int(x, __FUNCTION__)

#define RFCA_Free(x)  RFCA_Free_int(x, __FUNCTION__)

#define RFCA_Realloc(x, new_size)  RFCA_Realloc_int(x, new_size, __FUNCTION__)

#ifdef __cplusplus
}
#endif

#endif // _RFCA_MEM_HELPER_H_
