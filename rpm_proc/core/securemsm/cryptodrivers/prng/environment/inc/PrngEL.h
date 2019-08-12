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

  $Header: //components/rel/rpm.bf/2.1.1/core/securemsm/cryptodrivers/prng/environment/inc/PrngEL.h#1 $
  $DateTime: 2015/02/25 02:53:43 $
  $Author: pwbldsvc $ 

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

#endif /*__PRNGEL_H__ */
