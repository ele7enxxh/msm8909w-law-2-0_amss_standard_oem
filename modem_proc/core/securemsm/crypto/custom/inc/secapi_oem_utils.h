#ifndef SECAPI_OEM_UTILS_H
#define SECAPI_OEM_UTILS_H

/** @file secapi_oem_utils.h
 * @brief
 * This file contains the definitions of the constants, data structures and
 * and interfaces for reading/writing private data.
 * 
 */

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  $Header: //components/rel/core.mpss/3.5.c12.3/securemsm/crypto/custom/inc/secapi_oem_utils.h#1 $ 
  $DateTime: 2016/12/13 07:59:23 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
04/20/11   sp      Fixed compilation warnings.
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
Copyright (c) 2007-2010 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

#ifdef __cplusplus
extern "C"
{
#endif

#include "secerrno.h"

/** This enum identifies the seed type to be used with CSPRNG */
typedef enum
{
  E_CSPRNG_OEM_SEED       = 0,  /**< --  Use OEM specified seed mechanism, currently
                                 * defaulted to NV */
  E_CSPRNG_NV_SEED        = 1,  /**< --  Use NV to store seed data, presently routed
                                 * through oem layer. */
  E_CSPRNG_NO_SEED        = 2
}secapi_util_selected_csprng_seed_type;

/*!
*   @brief: secapi_oem_iv_get ()
*   This function provides an API to allow an OEM to get the IV for decryption.

*   @details:
*   Allows an OEM to get the IV for decryption.
*
*   @return:
*   E_SUCCESS if successful, another error code if not
*
*   @sideeffects
*   None.
*/
secerrno_enum_type secapi_oem_iv_get (void *iv, int len);

/*!
*
*   @brief: secapi_oem_csprng_seed_get ()
*   This function provides an API to allow OEM to retrieve the seed. 
*
*   @details:
*   this API is called by CSPRNG to retrieve the seed. this API Allows an OEM
*   to customize the seed retrieval mechanism, currently defaulted to use the
*   seed value for CSPRNG to be stored in and retrived from the NV memory.
*
*   @return:
*   returns E_SUCCESS once the seed to used as the initial entropy for CSPRNG is
*   read correctly from the storage mechanism, presently as NV item. if 
*   successful also returns the populated seed_ptr.
*
*   @sideeffects
*   None.
*/
secerrno_enum_type secapi_oem_csprng_seed_get ( uint8* seed_ptr, uint16 seed_len);

/*!
*
*   @brief: secapi_oem_csprng_seed_put ()
*   This function provides an API to allow an OEM defined mechanism to store 
*   the updated seed value for CSPRNG.
*
*   @details:
*   Allows an OEM defined mechanism to store the updated seed value for CSPRNG.
*
*   @return:
*   Returns E_SUCCESS if seed is stored in its correct place. presently as an NV
*
*   @sideeffects
*   None.
*/
secerrno_enum_type secapi_oem_csprng_seed_put (uint8* seed_ptr, uint16 seed_len);

/*!
*
*   @brief: 
*   This function provides an API to allow OEM to specify the seed type to 
*   be used with CSPRNG.
*   
*   @details:
*   Allows OEM to specify the seed type to be used with CSPRNG.
*
*   @return:
*   returns the selected seed mechanism as an enum.
*
*   @sideeffects
*   None.
*/
secapi_util_selected_csprng_seed_type secapi_oem_csprng_seed_type_get(void);
/**
 @}
*/
/* end_addtogroup_crypto_secoemapi  */


#ifdef __cplusplus
}
#endif


#endif /* SEC_OEM_UTILS_H */
