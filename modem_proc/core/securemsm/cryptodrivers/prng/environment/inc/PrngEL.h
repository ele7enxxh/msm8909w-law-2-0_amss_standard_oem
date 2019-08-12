#ifndef __PRNGEL_H__
#define __PRNGEL_H__

/*===========================================================================

                       P R N G E n g i n e D r i v e r

                       H e a d e r  F i l e (e x t e r n a l)

DESCRIPTION
  This header file contains HW Crypto specific declarations.

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None
  
Copyright (c) 2010 - 2011 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
============================================================================*/

/*===========================================================================

                           EDIT HISTORY FOR FILE

  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/core.mpss/3.5.c12.3/securemsm/cryptodrivers/prng/environment/inc/PrngEL.h#1 $
  $DateTime: 2016/12/13 07:59:23 $
  $Author: mplcsds1 $ 

when       who     what, where, why
--------   ---     ----------------------------------------------------------
09/08/11   nk      Added clock disable routine.
7/25/10   yk     Initial version
============================================================================*/

#include "comdef.h"

typedef enum
{
    PRNGEL_ERROR_NONE,
    PRNGEL_ERROR_BUSY,
    PRNGEL_ERROR_FAILED,
    PRNGEL_ERROR_INVALID_PARAM,
    PRNGEL_ERROR_UNSUPPORTED,
    PRNGEL_ERROR_BAD_STATE
} PrngEL_Result_Type;
/**
 * @brief    This function enable PRNG Engine.  
 *
 * @param None
 *
 * @return PrngEL_Result_Type
 *
 * @see PrngEL_ClkDisable
 *
 */
PrngEL_Result_Type PrngEL_ClkEnable( void );

/**
 * @brief  This function disable PRNG Engine.  
 *
 * @param None
 *
 * @return PrngEL_Result_Type
 *
 * @see PrngEL_ClkEnable
 *
 */
PrngEL_Result_Type PrngEL_ClkDisable( void );

/**
 * @brief Mutex Init
 * This function is called at rc init group zero
 * But this is not enough for the stack check which runs before        
 * Thus this mutex init macro is also called at PrngELMutexEnter. 
 * Incase stack canary check does not call prng during bringup stage then 
 * it will anyhow be called by rcinit level zero which is before all other tasks
 * @return None
 *
 * @see 
 *
 */
void PrngELMutexInit( void );

/**
 * @brief Mutex Enter
 *        
 *
 * @return None
 *
 * @see 
 *
 */
void PrngELMutexEnter( void );

/**
 * @brief Mutex Exit
 *        
 *
 * @return None
 *
 * @see 
 *
 */
void PrngELMutexExit( void );


/**
 * @brief  Memory barrier
 *        
 *
 * @return None
 *
 * @see 
 *
 */
void PrngELMemoryBarrier( void );

#endif /*__PRNGEL_H__ */
