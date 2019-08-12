#ifndef SEC_OEM_UTILS_H
#define SEC_OEM_UTILS_H

/** @file sec_oem_utils.h
 * @brief
 * This file contains the definitions of the constants, data structures and
 * and interfaces for reading/writing private data.
 * 
 */

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  $Header: //components/rel/core.mpss/3.5.c12.3/api/securemsm/crypto/sec_oem_utils.h#1 $ 
  $DateTime: 2016/12/13 07:59:23 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
08/28/10   dm      Crypto API files rearchitecture
06/06/10   qxu     added mainpage, dscriptions for functions, constants and data types
                   to doxygenate API document.
06/06/10   qxu     removed sec_oem_data_set(), sec_oem_data_get(), sec_oem_data_clear()
                   sec_oem_data_dup() functions from public header file.
02/13/09   bm      Removed storing seed into SFS
11/17/09   avm     added API's to enable EM's to provision the seed for CSPRNG
                   OEM's can select the mechanism they need to use for storage 
				   and retrieval of CSPRNG seed.
02/08/08   avm     added API's to enable OEM provisioned Device Key for encrypt
                   Decrypt function.
09/10/07   gr      created
Copyright (c) 2008-2009 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

#ifdef __cplusplus
extern "C"
{
#endif

#include "secerrno.h"


/** This enum identifies the key type to be used with System
 *  SFS */
typedef enum
{
  E_MSM_DEV_KEY       = 0,  /**< --  Use MSM Device Key. Only useful (i.e device unique key) if Secure Boot to be enabled */
  E_OEM_DEV_KEY       = 1,  /**< --  Use OEM Specified key. Protecting the key is OEM's responsibility */
  E_SEC_PART_DEV_KEY  = 2,  /**< --  Use a key from secure partition. Secure partition is a flash partition that is write
                               protected. */
  E_NO_DEV_KEY        = 3
}secapi_util_selected_dev_key_type;

/*!
   @addtogroup crypto_secoemapi 
   This section explains the Cryptographic OEM Library provided by sec_oem_utils.h
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
secerrno_enum_type secapi_oem_hw_key_get (void *key, int len);

/*!
*   @brief: 
*   This function provides an API for OEM to specify the type of device key to use.
*
*   @details:
*   This function provides an API for OEM to specify the type of device key to use.
*
*   @return:
*   E_SUCCESS if successful, another error code if not
*   @sideeffects
*   None.
*/
secapi_util_selected_dev_key_type secapi_oem_dev_key_type_get(void);

#ifdef __cplusplus
}
#endif


#endif /* SEC_OEM_UTILS_H */
