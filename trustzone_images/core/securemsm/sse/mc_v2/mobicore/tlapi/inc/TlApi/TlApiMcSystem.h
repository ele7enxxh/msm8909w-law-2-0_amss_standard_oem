/** @addtogroup TLAPI_SYS
 * @{
 * The MobiCore system API interface provides system information and system functions to Trustlets.
 * 
 * Copyright (c) 2013 TRUSTONIC LIMITED
 * All rights reserved
 * 
 * The present software is the confidential and proprietary information of
 * TRUSTONIC LIMITED. You shall not disclose the present software and shall
 * use it only in accordance with the terms of the license agreement you
 * entered into with TRUSTONIC LIMITED. This software may be subject to
 * export or import laws in certain countries.
 */
#ifndef __TLAPIMCSYSTEM_H__
#define __TLAPIMCSYSTEM_H__

#include "TlApi/TlApiCommon.h"
#include "TlApi/TlApiError.h"

#include "mcSuid.h"
#include "mcVersionInfo.h"
#include "TlApi/version.h"


//------------------------------------------------------------------------------

/** Platform information structure.
 * The platform information structure returns manufacturer specific information about
 * the hardware platform.
 */
typedef struct {
    uint32_t size;                      /**< size of the structure. */
    uint32_t manufacturerdId;           /**< Manufacturer ID provided by G&D. */
    uint32_t platformVersion;           /**< Version of platform. */
    uint32_t platformInfoDataLenght;    /**< Length of manufacturer specific platform information. */
    uint8_t  platformInfoData[];        /**< Manufacturer specific platform information data. */
} tlApiPlatformInfo_t;

/** Get information about the implementation of the MobiCore Trustlet API version.
 *
 * @param tlApiVersion     pointer to tlApi version.
 * @return TLAPI_OK if version has been set
 * @returns E_TLAPI_NULL_POINTER    if one parameter is a null pointer.
 * @returns E_TLAPI_UNMAPPED_BUFFER if one buffer is not entirely mapped in Trustlet address space.
 */
_TLAPI_EXTERN_C tlApiResult_t tlApiGetVersion(
    uint32_t *tlApiVersion);

/** Get information about the underlying MobiCore version.
 * The function tlApiGetMobicoreVersion() provides the MobiCore product id and
 * version information about the various MobiCore interfaces as defined in mcVersionInfo.h
 *
 * @param mcVersionInfo     pointer to version information structure.
 * @returns TLAPI_OK if version has been set
 * @returns E_TLAPI_NULL_POINTER    if one parameter is a null pointer.
 * @returns E_TLAPI_UNMAPPED_BUFFER if one buffer is not entirely mapped in Trustlet address space.
 */
_TLAPI_EXTERN_C tlApiResult_t tlApiGetMobicoreVersion(
    mcVersionInfo_t *mcVersionInfo);


///** Get information about the hardware platform.
// * The function tlApiGetPlatformInformation() provides information about the current hardware platform.
// *
// * @param platformInfo pointer to tlApiPlatformInfo_t structure that receives the platform information.
// *
// * @return TLAPI_OK if character c has successfully been read.
// * @return E_TLAPI_BUFFER_TOO_SMALL if mcPlatformInfo.size is too small. On return mcPlatformInfo.size will be set to the required length.
// */
//_TLAPI_EXTERN_C tlApiResult_t tlApiGetPlatformInfo(
//    tlApiPlatformInfo_t    *platformInfo);

/** Terminate the Trustlet with a exit code.
 * Trustlets can use the tlApiExit() to terminate themselves and return an exit code. This
 * can be used if the initialization fails or an unrecoverable error occurred. The Trustlet
 * will be terminated immediately and this function will not return.
 *
 * @param exitCode exit code
 *
 * @return there is no return code, since the function will not return
 */
_TLAPI_EXTERN_C _TLAPI_NORETURN void tlApiExit(
    uint32_t    exitCode);

/** Get the System on Chip Universal Identifier.
 * @param suid pointer to Suid structure that receives the Suid data
 * @returns TLAPI_OK                if Suid has been successfully read.
 * @returns E_TLAPI_NULL_POINTER    if one parameter is a null pointer.
 * @returns E_TLAPI_UNMAPPED_BUFFER if one buffer is not entirely mapped in Trustlet address space.
 */
_TLAPI_EXTERN_C tlApiResult_t tlApiGetSuid(
    mcSuid_t            *suid);

//------------------------------------------------------------------------------
/* Get the virtual memoty type
*
* @param type pointer to address where type is returned
* @param addr start address of checked memory
* @param size size checked memory
*/
//#define TLAPI_PHYS_MEM_TYPE_HIGH_SECURE   (1U<<0)                  /**< High secure memory. (Typically iRam) */
//#define TLAPI_PHYS_MEM_TYPE_SECURE        (1U<<1)                  /**< Secure memory in Dram */
//#define TLAPI_PHYS_MEM_TYPE_NON_SECURE    (1U<<2)                  /**< NonSecure memory in Dram. Accessible form NonSecure world */
#define TLAPI_VIRT_MEM_TYPE_SECURE        (1U<<3)                  /**< The memory area is mapped as secure */
#define TLAPI_VIRT_MEM_TYPE_NON_SECURE    (1U<<4)                  /**< The memory area is mapped as non-secure */

_TLAPI_EXTERN_C tlApiResult_t tlApiGetVirtMemType(
        uint32_t *type,
        addr_t   addr,
        uint32_t size);


//------------------------------------------------------------------------------
/* Helper to simplify NWd buffer testing
 */
static inline bool tlApiIsNwdBufferValid( addr_t addr, uint32_t size ) 
{
  uint32_t type;
  tlApiResult_t ret = tlApiGetVirtMemType( &type, addr, size );
  return ret==TLAPI_OK && (type&TLAPI_VIRT_MEM_TYPE_NON_SECURE)!=0;
}  

// The isNwdBufferValid function name is deprecated, #define ensures backward compatibility
// Please do not use this function name anymore, use tlApiIsNwdBufferValid instead
#define isNwdBufferValid tlApiIsNwdBufferValid

#endif // __TLAPIMCSYSTEM_H__

/** @} */
