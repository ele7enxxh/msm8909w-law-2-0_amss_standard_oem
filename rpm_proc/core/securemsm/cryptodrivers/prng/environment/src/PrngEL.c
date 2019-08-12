/**
@file PrngEL.c 
@brief PRNG Environment Library source file 
*/

/*===========================================================================

                     Crypto Engine Environment Library 

DESCRIPTION

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None
  
Copyright (c) 2010 - 2011 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
============================================================================*/


/*===========================================================================

                           EDIT HISTORY FOR FILE

  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

 $Header: 
 $DateTime: 2015/02/25 02:53:43 $
 $Author: pwbldsvc $ 

when         who     what, where, why
--------     ---     ----------------------------------------------------------
09/08/11      nk     Added clock disable routine.     
8/21/10       YK     Initial version
============================================================================*/

#include "PrngEL.h"
#include "PrngEL_Env.h"
#include "PrngCL.h"

//PRNGEL_MUTEX_TYPE prngel_mutex;
uint32 prngel_mutex_init = 0;

/**
 * @brief    This function enable PRNG Engine.  
 *
 * @param None
 *
 * @return None
 *
 * @see PrngEL_ClkDisable
 *
 */

PrngEL_Result_Type PrngEL_ClkEnable( void )
{
	PrngEL_Result_Type stat = PRNGEL_ERROR_NONE;
    stat = (PrngEL_Result_Type)PRNGCL_ENABLE_CLK();
	return stat;
}

/**
 * @brief    This function disable PRNG Engine.  
 *
 * @param None
 *
 * @return None
 *
 * @see PrngEL_ClkEnable
 *
 */

PrngEL_Result_Type PrngEL_ClkDisable( void )
{
	PrngEL_Result_Type stat = PRNGEL_ERROR_NONE;
    stat = (PrngEL_Result_Type) PRNGCL_DISABLE_CLK();
	return stat;
}

/**
 * @brief Mutex Enter 
 *        
 *
 * @return None
 *
 * @see 
 *
 */
void PrngELMutexEnter( void )
{
    PRNGEL_MUTEX_ENTER();
}

/**
 * @brief  Mutex Exit
 *        
 *
 * @return None
 *
 * @see 
 *
 */
void PrngELMutexExit( void )
{
   PRNGEL_MUTEX_EXIT();
}
