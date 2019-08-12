#ifndef CU_OEM_KEY_H
#define CU_OEM_KEY_H

/** @file sec_oem_key.h
 * @brief
 * This file contains the definitions of the constants, data structures and
 * and interfaces for reading/writing private data.
 * 
 */

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  $Header: //components/rel/core.mpss/3.5.c12.3/api/securemsm/cryptoutil/cu_oem_key.h#1 $ 
  $DateTime: 2016/12/13 07:59:23 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
09/13/10   qxu     init version. moved the key feature here from cu_oem_util.h

Copyright (c) 2008-2010 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

#include "secerrno.h"

#define CU_OEM_HW_KEY_ENABLED 1
#define CU_OEM_HW_KEY_DISABLED 0

/*!
   @addtogroup cu_oemapi 
   This section explains the Cryptographic OEM Library provided by sec_oem_key.h
   @{
*/
/*!
*   @brief: 
*   This function is used to Provides an API for OEM implementation of unique 
*   device ID, Hardware Key.
*
*   @details:
*   Provides an API for OEM implementation of unique device ID, Hardware Key.
*
*   @return:
*   Returns E_SUCCESS if get hardware key successfully.
*
*   @sideeffects
*   None.
*/
secerrno_enum_type cu_oem_hw_key_get (void *key, int len);


/*!
*   @brief: 
*   This function is used for user to indicate whether OEM Hardware key needs 
*   to be enabled or not.
*
*   @details:
*   Provides an API for OEM implementation of unique device Hardware Key.
*
*   @return:
*   Returns CU_OEM_HW_KEY_ENABLED then the oem cutomerized hardware key will be used.
*   Returns CU_OEM_HW_KEY_DISABLED then MSM hardware key will be used.
*
*   @sideeffects
*   None.
*/
uint8 cu_oem_hw_key_enabled(void);

/**
* @}
 */

#endif /* CU_OEM_KEY_H */
