/**
@file PrngML.c 
@brief PRNG Engine source file 
*/

/*===========================================================================

                     P R N G E n g i n e D r i v e r

DESCRIPTION
  This file contains declarations and definitions for the
  interface between application and PRNG engine driver

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None
  
Copyright (c) 2009 - 2014 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
============================================================================*/


/*===========================================================================

                           EDIT HISTORY FOR FILE

  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

 $Header: //components/rel/rpm.bf/2.1.1/core/securemsm/cryptodrivers/prng/shared/src/PrngML.c#1 $
 $DateTime: 2015/02/25 02:53:43 $
 $Author: pwbldsvc $ 

when         who     what, where, why
--------     ---     ----------------------------------------------------------
01/21/14     nk      software prng added for bringup
09/17/12     amen    Added prng lite function
09/12/11     nk      Added prng de init functions     
7/25/10      yk     Initial version
============================================================================*/
#include "comdef.h"
#include "PrngCL.h"
#include "PrngML.h"
#include "PrngEL.h"

uint32 prng_init_done = FALSE;

/**
 * @brief    This function returns random number.  
 *
 * @param random_ptr [in]Random number pointer
 * @param random_len [in] Length of random number 
 *
 * @return PrngML_Resut_Type
 *
 *
 */
PrngML_Result_Type PrngML_getdata(uint8* random_ptr, uint16 random_len)
{
  PrngML_Result_Type ret_val = PRNGML_ERROR_NONE;
  
  if(!random_ptr || !random_len)
  {
     return PRNGML_ERROR_INVALID_PARAM; 
  }

  /* Multi Thread safe implementation */
  /* Mutex Enter */
  PrngELMutexEnter();

  if (TRUE != prng_init_done) 
  {
    ret_val = (PrngML_Result_Type) PrngCL_init();
    if (ret_val != PRNGML_ERROR_NONE) return ret_val;
    prng_init_done = TRUE;
  }

  /* Get random data */
  ret_val = (PrngML_Result_Type)PrngCL_getdata(random_ptr, random_len);

  if (TRUE == prng_init_done) 
  {
    ret_val = (PrngML_Result_Type) PrngCL_deinit();
    if (ret_val != PRNGML_ERROR_NONE) return ret_val;
    prng_init_done = FALSE;
  }
  
  /* Mutex Exit */
  PrngELMutexExit();

 return ret_val;
}


/**
 * @brief This function returns random number and can be used when Mutex,
 *        DAL framework are not yet initialised.
 *
 * @param random_ptr [in/out]Random number pointer
 * @param random_len [in] Length of random number 
 *
 * @return PrngML_Result_Type
 */
PrngML_Result_Type PrngML_getdata_lite
(
  uint8*  random_ptr,
  uint16  random_len
)
{
  PrngML_Result_Type ret_val = PRNGML_ERROR_NONE;
  
  /* Input Sanity check */
  if(!random_ptr || !random_len)
  {
    return PRNGML_ERROR_INVALID_PARAM; 
  }

  if (TRUE != prng_init_done) 
  {
    ret_val = (PrngML_Result_Type) PrngCL_lite_init();
    if (ret_val != PRNGML_ERROR_NONE) return ret_val;
    prng_init_done = TRUE;
  }

  /* Get random data */
  ret_val = (PrngML_Result_Type)PrngCL_getdata(random_ptr, random_len);

  if (TRUE == prng_init_done) 
  {
    ret_val = (PrngML_Result_Type) PrngCL_lite_deinit();
    if (ret_val != PRNGML_ERROR_NONE) return ret_val;
    prng_init_done = FALSE;
  }

  return ret_val;
} /* PrngML_getdata_lite() */
