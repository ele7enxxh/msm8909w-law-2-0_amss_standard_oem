#ifndef SEC_PIL_ENV_H
#define SEC_PIL_ENV_H

/**
* @file sec_pil_env.h
* @brief Contains TZ/MBA specific macros,type declarations and function declarations
*        whose implementation is specifc to MBA/TZ.
*/

/*===========================================================================
   Copyright (c) 2012-2014 by QUALCOMM Technologies, Inc.   All Rights Reserved.
===========================================================================*/

/*===========================================================================

                            EDIT HISTORY FOR FILE

  $Header: //components/rel/core.mpss/3.5.c12.3/securemsm/secpil/src/mba/sec_pil_env.h#1 $
  $DateTime: 2016/12/13 07:59:23 $
  $Author: mplcsds1 $

when       who      what, where, why
--------   ---      ------------------------------------
02/28/12   vg       Fixed review comments
02/20/12   vg       Initial version.

===========================================================================*/
/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/
#include <comdef.h>
#include "secboot.h"
#include "secboot_hw.h"
#include <stdlib.h>
#include "mba_error_handler.h"
#include "uie.h"

/*----------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * -------------------------------------------------------------------------*/
/* Debug Codes */ 
#define PIL_GET_SEBOOT_FTBL_FAILED MBA_PIL_GET_SEBOOT_FTBL_FAILED
#define PIL_INIT_IMAGE_NULL_ELF_HDR MBA_PIL_INIT_IMAGE_NULL_ELF_HDR
#define PIL_INIT_IMAGE_UNSUPPORTED_PROC MBA_PIL_INIT_IMAGE_UNSUPPORTED_PROC
#define PIL_INIT_IMAGE_ELF_HDR_NOT_NONSECURE MBA_PIL_INIT_IMAGE_ELF_HDR_NOT_NONSECURE
#define PIL_INIT_IMAGE_IS_NOT_ELF MBA_PIL_INIT_IMAGE_IS_NOT_ELF
#define PIL_INIT_IMAGE_POPULATE_ELF_INFO_FAILED MBA_PIL_INIT_IMAGE_POPULATE_ELF_INFO_FAILED
#define PIL_INIT_IMAGE_VERIFY_SIG_FAILED MBA_PIL_INIT_IMAGE_VERIFY_SIG_FAILED
#define PIL_INIT_IMAGE_PREPARE_SUBSYS_FAILED MBA_PIL_INIT_IMAGE_PREPARE_SUBSYS_FAILED
#define PIL_AUTH_RESET_UNSUPPORTED_PROC MBA_PIL_AUTH_RESET_UNSUPPORTED_PROC
#define PIL_AUTH_RESET_PROC_NOT_IN_RESET MBA_PIL_AUTH_RESET_PROC_NOT_IN_RESET
#define PIL_AUTH_RESET_AUTH_SEGMENTS_FAILED MBA_PIL_AUTH_RESET_AUTH_SEGMENTS_FAILED
#define PIL_AUTH_RESET_SUBSYS_BRING_UP_FAILED MBA_PIL_AUTH_RESET_SUBSYS_BRING_UP_FAILED
#define PIL_VERIFY_SEGMENTS_UNSUPPORTED_PROC MBA_PIL_VERIFY_SEGMENTS_UNSUPPORTED_PROC
#define PIL_GET_CODE_SEGMENT_FAILED MBA_PIL_GET_CODE_SEGMENT_FAILED
#define PIL_VERIFY_SIG_HW_IS_AUTH_ENABLED_FAILED MBA_PIL_VERIFY_SIG_HW_IS_AUTH_ENABLED_FAILED
#define PIL_VERIFY_SEGMENTS_AUTH_INIT_FAILED MBA_PIL_VERIFY_SEGMENTS_AUTH_INIT_FAILED
#define PIL_VERIFY_SIG_MPU_LOCK_FAILED MBA_PIL_VERIFY_SIG_MPU_LOCK_FAILED
#define PIL_VERIFY_SEGMENTS_AUTH_PARTIAL_SEGMENTS_FAILED MBA_PIL_VERIFY_SEGMENTS_AUTH_PARTIAL_SEGMENTS_FAILED
#define PIL_GET_ENTRY_UNSUPPORTED_PROC MBA_PIL_GET_ENTRY_UNSUPPORTED_PROC
#define PIL_GET_IMAGE_LENGTH_UNSUPPORTED_PROC MBA_PIL_GET_IMAGE_LENGTH_UNSUPPORTED_PROC
#define PIL_AUTH_SEGMENTS_NULL_ELF MBA_PIL_AUTH_SEGMENTS_NULL_ELF
#define PIL_AUTH_SEGMENTS_ZERO_SEGMENTS_FAILED MBA_PIL_AUTH_SEGMENTS_ZERO_SEGMENTS_FAILED
#define MI_BOOT_VALID_UNSUPPORTED_PROC MBA_PIL_MI_BOOT_VALID_UNSUPPORTED_PROC
#define MI_BOOT_VALID_MI_NULL MBA_PIL_MI_BOOT_VALID_MI_NULL
#define MI_BOOT_VALID_ZERO_SIZE MBA_PIL_MI_BOOT_VALID_ZERO_SIZE
#define PIL_IS_ELF_MAGIC_FAILED MBA_PIL_IS_ELF_MAGIC_FAILED
#define PIL_HW_IS_AUTH_ENBALED_NULL_SECBOOT_ROUTINES MBA_PIL_HW_IS_AUTH_ENBALED_NULL_SECBOOT_ROUTINES
#define PIL_VERIFY_SIG_NULL_ELF MBA_PIL_VERIFY_SIG_NULL_ELF
#define PIL_VERIFY_SIG_NULL_SECBOOT_ROUTINES MBA_PIL_VERIFY_SIG_NULL_SECBOOT_ROUTINES
#define PIL_SECBOOT_INIT_FAILED MBA_PIL_SECBOOT_INIT_FAILED
#define PIL_SECBOOT_GET_SW_VERSION_FAILED MBA_PIL_SECBOOT_GET_SW_VERSION_FAILED
#define PIL_SECBOOT_GET_SW_ID_FAILED MBA_PIL_SECBOOT_GET_SW_ID_FAILED
#define PIL_SECBOOT_AUTHENTICATE_FAILED MBA_PIL_SECBOOT_AUTHENTICATE_FAILED
#define POPULATE_ELF_PROG_HDR_NOT_IN_NS_MEMORY MBA_PIL_POPULATE_ELF_PROG_HDR_NOT_IN_NS_MEMORY
#define POPULATE_ELF_HASH_SEG_NOT_IN_NS_MEMORY MBA_POPULATE_ELF_HASH_SEG_NOT_IN_NS_MEMORY
#define POPULATE_ELF_IMAGE_HDR_TOO_BIG MBA_POPULATE_ELF_IMAGE_HDR_TOO_BIG
#define POPULATE_ELF_MALLOC_FAILED MBA_POPULATE_ELF_MALLOC_FAILED
#define POPULATE_ELF_HW_IS_AUTH_ENBALED_FAILED MBA_POPULATE_ELF_HW_IS_AUTH_ENBALED_FAILED
#define POPULATE_ELF_MI_BOOT_INVALID MBA_POPULATE_ELF_MI_BOOT_INVALID
#define AUTH_SEGMENT_ZERO_ARGS MBA_AUTH_SEGMENT_ZERO_ARGS
#define AUTH_SEGMENT_INVALID_HASH_LEN MBA_AUTH_SEGMENT_INVALID_HASH_LEN
#define AUTH_SEGMENT_HASH_FAILED MBA_AUTH_SEGMENT_HASH_FAILED
#define AUTH_SEGMENT_INVALID_DIGEST MBA_AUTH_SEGMENT_INVALID_DIGEST
#define AUTH_ELF_HEADERS_ELF_IS_NULL MBA_AUTH_ELF_HEADERS_ELF_IS_NULL
#define AUTH_ELF_HEADERS_PROG_HDR_NUM_ZERO MBA_AUTH_ELF_HEADERS_PROG_HDR_NUM_ZERO
#define AUTH_ELF_HEADERS_HASH_FAILED MBA_AUTH_ELF_HEADERS_HASH_FAILED
#define AUTH_ELF_HEADERS_NULL_HASH MBA_AUTH_ELF_HEADERS_NULL_HASH
#define AUTH_ELF_HEADERS_INVALID_DIGEST MBA_AUTH_ELF_HEADERS_INVALID_DIGEST
#define SEC_PIL_GET_SECBOOT_FTBL_FAILED MBA_SEC_PIL_GET_SECBOOT_FTBL_FAILED
#define SEC_PIL_GET_SECBOOT_HW_FTBL_FAILED MBA_SEC_PIL_GET_SECBOOT_HW_FTBL_FAILED
#define PIL_AUTH_SEG_AUTH_INIT_NULL MBA_PIL_AUTH_SEG_AUTH_INIT_NULL
#define PIL_AUTH_PARTIAL_SEGMENTS_FAILED MBA_PIL_AUTH_PARTIAL_SEGMENTS_FAILED
#define PIL_AUTH_PARTIAL_SEGMENTS_HASH_INIT_FAILED MBA_PIL_AUTH_PARTIAL_SEGMENTS_HASH_INIT_FAILED
#define PIL_AUTH_PARTIAL_SEGMENTS_HASH_UPDATE_FAILED MBA_PIL_AUTH_PARTIAL_SEGMENTS_HASH_UPDATE_FAILED
#define PIL_AUTH_PARTIAL_SEGMENTS_HASH_FINAL_FAILED MBA_PIL_AUTH_PARTIAL_SEGMENTS_HASH_FINAL_FAILED
#define PIL_AUTH_PARTIAL_SEGMENTS_INVALID_DIGEST MBA_PIL_AUTH_PARTIAL_SEGMENTS_INVALID_DIGEST
#define PIL_AUTH_PARTIAL_SEGMENTS_CORRUPTED_IMAGE MBA_PIL_AUTH_PARTIAL_SEGMENTS_CORRUPTED_IMAGE
 
#define PIL_AUTH_SEGMENTS_HASH_NULL MBA_PIL_AUTH_SEGMENTS_HASH_NULL
#define PIL_AUTH_SEGMENTS_LOCK_XPU_FAILED MBA_PIL_AUTH_SEGMENTS_LOCK_XPU_FAILED
#define PIL_AUTH_SEGMENTS_INVALID_HASH_LEN MBA_PIL_AUTH_SEGMENTS_INVALID_HASH_LEN 
#define PIL_AUTH_SEGMENTS_SEGMENT_AUTH_FAILED MBA_PIL_AUTH_SEGMENTS_SEGMENT_AUTH_FAILED
#define PIL_AUTH_SEGMENTS_REMAINING_HASH_BYTES MBA_PIL_AUTH_SEGMENTS_REMAINING_HASH_BYTES
#define PIL_HW_IS_AUTH_ENBALED_FAILED MBA_PIL_HW_IS_AUTH_ENBALED_FAILED
#define PIL_SECBOOT_DEINIT_FAILED MBA_PIL_SECBOOT_DEINIT_FAILED
#define PIL_VERIFY_SIG_MPU_UNLOCK_FAILED MBA_PIL_VERIFY_SIG_MPU_UNLOCK_FAILED
#define PIL_VERIFY_SIG_INVALID_ELF_HEADERS MBA_PIL_VERIFY_SIG_INVALID_ELF_HEADERS
#define PIL_VERIFY_SIG_INVALID_IMAGE_SIGNING MBA_PIL_VERIFY_SIG_INVALID_IMAGE_SIGNING
#define PIL_ZERO_SEGMENTS_SIZE_MISMATCH MBA_PIL_ZERO_SEGMENTS_SIZE_MISMATCH
#define PIL_ZERO_SEGMENTS_INTEGER_OVERFLOW MBA_PIL_ZERO_SEGMENTS_INTEGER_OVERFLOW
#define MI_BOOT_VALID_LENGTH_MISMATCH MBA_MI_BOOT_VALID_LENGTH_MISMATCH
#define PIL_IS_ELF_INVALID_CLASS MBA_PIL_IS_ELF_INVALID_CLASS
#define PIL_IS_ELF_INVALID_VERSION MBA_PIL_IS_ELF_INVALID_VERSION
#define PIL_IS_ELF_INVALID_EHSIZE MBA_PIL_IS_ELF_INVALID_EHSIZE
#define PIL_IS_ELF_INVALID_PHENTSIZE MBA_PIL_IS_ELF_INVALID_PHENTSIZE
#define POPULATE_ELF_INCORRECT_HASH_TABLE_SEGMENTS MBA_POPULATE_ELF_INCORRECT_HASH_TABLE_SEGMENTS
#define POPULATE_ELF_HASH_SEG_TOO_BIG MBA_POPULATE_ELF_HASH_SEG_TOO_BIG
#define POPULATE_ELF_ELF_HDR_NOT_IN_NS_MEMORY MBA_POPULATE_ELF_ELF_HDR_NOT_IN_NS_MEMORY
#define POPULATE_ELF_GET_CODE_SEGMENT_FAILED MBA_POPULATE_ELF_GET_CODE_SEGMENT_FAILED
#define PIL_GET_EBI_CH_INVALID MBA_GET_EBI_CH_INVALID
#define GET_META_DATA_LENGTH_UNSUPPORTED_PROC MBA_GET_META_DATA_LENGTH_UNSUPPORTED_PROC
#define GET_META_DATA_INSUFFICIENT_MEMORY MBA_GET_META_DATA_INSUFFICIENT_MEMORY
#define GET_META_DATA_NO_PROG_HDR MBA_GET_META_DATA_NO_PROG_HDR
#define GET_IMAGE_LENGTH_IN_MEMORY_UNSUPPORTED_PROC MBA_GET_IMAGE_LENGTH_IN_MEMORY_UNSUPPORTED_PROC
#define GET_IMAGE_LENGTH_IN_MEMORY_ERROR MBA_GET_IMAGE_LENGTH_IN_MEMORY_ERROR
#define PIL_GET_END_ADDRESS_INTEGER_OVERFLOW1 MBA_PIL_GET_END_ADDRESS_INTEGER_OVERFLOW1
#define PIL_VERIFY_SEGMENTS_GET_END_ADDR_FAILED MBA_PIL_VERIFY_SEGMENTS_GET_END_ADDR_FAILED
#define PIL_GET_END_ADDRESS_DATA_CORRUPTION MBA_PIL_GET_END_ADDRESS_DATA_CORRUPTION
#define PIL_GET_END_ADDRESS_INTEGER_OVERFLOW2 MBA_PIL_GET_END_ADDRESS_INTEGER_OVERFLOW2
#define POPULATE_ELF_PROG_TBL_TOO_BIG MBA_POPULATE_ELF_PROG_TBL_TOO_BIG
#define POPULATE_ELF_PREVIOUS_MALLOC_IS_NOT_FREED MBA_POPULATE_ELF_PREVIOUS_MALLOC_IS_NOT_FREED
#define POPULATE_ELF_HDR_TOO_BIG MBA_POPULATE_ELF_HDR_TOO_BIG
#define PIL_GET_CODE_SEGMENT_UNSUPPORTED_PROC MBA_PIL_GET_CODE_SEGMENT_UNSUPPORTED_PROC
#define PIL_GET_CODE_SEGMENT_NULL_POINTER MBA_PIL_GET_CODE_SEGMENT_NULL_POINTER
#define PIL_GET_SW_ID_UNSUPORTED_PROC MBA_PIL_GET_SW_ID_UNSUPORTED_PROC
#define PIL_GET_SW_ID_NULL_POINTER MBA_PIL_GET_SW_ID_NULL_POINTER
#define PIL_GET_SW_VERISON_UNSUPPORTED_PROC MBA_PIL_GET_SW_VERISON_UNSUPPORTED_PROC
#define PIL_GET_SW_VERSION_NULL_POINTER MBA_PIL_GET_SW_VERSION_NULL_POINTER
#define PIL_UPDATE_HASH_HASH_INIT_FAILED MBA_PIL_UPDATE_HASH_HASH_INIT_FAILED
#define PIL_UPDATE_HASH_HASH_UPDATE_FAILED MBA_PIL_UPDATE_HASH_HASH_UPDATE_FAILED
#define PIL_UPDATE_HASH_HASH_FINAL_FAILED MBA_PIL_UPDATE_HASH_HASH_FINAL_FAILED
#define PIL_UPDATE_HASH_INVALID_DIGEST MBA_PIL_UPDATE_HASH_INVALID_DIGEST
#define PIL_UPDATE_HASH_CORRUPTED_IMAGE MBA_PIL_UPDATE_HASH_CORRUPTED_IMAGE
#define PIL_UPDATE_HASH_LENGTH_LOADED_FAILED MBA_PIL_UPDATE_HASH_LENGTH_LOADED_FAILED
#define PIL_VERIFY_SEGMENTS_INVALID_ADDRESS MBA_PIL_VERIFY_SEGMENTS_INVALID_ADDRESS
#define PIL_VERIFY_SEGMENTS_PIL_STATE_NOT_IN_RESET MBA_PIL_VERIFY_SEGMENTS_PIL_STATE_NOT_IN_RESET
#define PIL_VERIFY_SEGMENTS_MPU_LOCK_FAILED MBA_PIL_VERIFY_SEGMENTS_MPU_LOCK_FAILED
#define PIL_VERIFY_SEGMENTS_MPU_UNLOCK_FAILED MBA_PIL_VERIFY_SEGMENTS_MPU_UNLOCK_FAILED
#define PIL_VERIFY_SEGMENTS_HASH_PENDING MBA_PIL_VERIFY_SEGMENTS_HASH_PENDING
#define PIL_UPDATE_HASH_SEGMENTS_NOT_CONTIGUOUS MBA_PIL_UPDATE_HASH_SEGMENTS_NOT_CONTIGUOUS
#define PIL_SECBOOT_INVALID_END_ADDR MBA_PIL_SECBOOT_INVALID_END_ADDR
#define PIL_AUTH_PARTIAL_SEGMENTS_DATA_CORRUPTION MBA_PIL_AUTH_PARTIAL_SEGMENTS_DATA_CORRUPTION
#define PIL_AUTH_PARTIAL_SEGMENTS_NOT_CONTIGUOUS MBA_PIL_AUTH_PARTIAL_SEGMENTS_NOT_CONTIGUOUS
#define PIL_AUTH_PARTIAL_SEGMENTS_INTEGER_OVERFLOW MBA_PIL_AUTH_PARTIAL_SEGMENTS_INTEGER_OVERFLOW
#define PIL_AUTH_PARTIAL_SEGMENTS_INVALID_HASH_LEN MBA_PIL_AUTH_PARTIAL_SEGMENTS_INVALID_HASH_LEN
#define MI_BOOT_VALID_INTEGER_OVERFLOW1 MBA_MI_BOOT_VALID_INTEGER_OVERFLOW1
#define MI_BOOT_VALID_INTEGER_OVERFLOW2 MBA_MI_BOOT_VALID_INTEGER_OVERFLOW2
#define PIL_VERIFY_SEGMENTS_ERROR MBA_PIL_VERIFY_SEGMENTS_ERROR
#define PIL_GET_LOAD_ADDR_UNSUPORTED_PROC MBA_PIL_GET_LOAD_ADDR_UNSUPORTED_PROC
#define PIL_GET_LOAD_ADDR_NULL_POINTER MBA_PIL_GET_LOAD_ADDR_NULL_POINTER
#define AUTH_ELF_HEADERS_PROG_HDR_NUM_OVERFLOW MBA_AUTH_ELF_HEADERS_PROG_HDR_NUM_OVERFLOW
#define PIL_UPDATE_ELF_LEN_EXCEED_FILE_SZ MBA_PIL_UPDATE_ELF_LEN_EXCEED_FILE_SZ
#define PIL_UIE_DECRYPT_FAILED MBA_PIL_UIE_DECRYPT_FAILED
#define PIL_SEC_IMG_AUTH_INVALID_ARG  (MBA_PIL_SEC_IMG_AUTH_INVALID_ARG)
#define PIL_SEC_IMG_AUTH_HW_FAILURE  (MBA_PIL_SEC_IMG_AUTH_HW_FAILURE)
#define PIL_SEC_IMG_AUTH_INVALID_CAL_CERT_NUM  (MBA_PIL_SEC_IMG_AUTH_INVALID_CAL_CERT_NUM)
#define PIL_SEC_IMG_AUTH_IMAGE_NOT_SUPPORTED  (MBA_PIL_SEC_IMG_AUTH_IMAGE_NOT_SUPPORTED)
#define PIL_SEC_IMG_AUTH_ENGG_CERT_NOT_ALLOWED  (MBA_PIL_SEC_IMG_AUTH_ENGG_CERT_NOT_ALLOWED)

/* Debug Macros */
#define SEC_PIL_ERROR_CODE(x) \
       do{                                                             \
           mba_debug_log( __FILE__, __LINE__,                      \
                         (x) );   \
         } while( 0 )

#define SEC_PIL_ERROR_CODE_E(x,y)  \
       do{                                                             \
           mba_debug_log( __FILE__, __LINE__,                      \
                         (y) );   \
         } while( 0 )

#define SEC_PIL_LOG_DBG(x)  

#define SEC_PIL_MALLOC malloc
#define SEC_PIL_FREE   free


/*----------------------------------------------------------------------------
 * Type Declarations
 * -------------------------------------------------------------------------*/

typedef uint32 ssd_metadata_type;

typedef struct sec_pil_hash_ctx_s
{
  CeMLHashAlgoType  alg;
  CeMLCntxHandle    *ctx;
} sec_pil_hash_ctx;


/** Support for SHA1 and SHA256 for hash */
typedef enum
{
  SEC_PIL_HASH_NULL         = 1, ///< Do not perform any hashing
  SEC_PIL_HASH_SHA1         = 2,
  SEC_PIL_HASH_SHA256       = 3
} sec_pil_hash_algo_et;


/*----------------------------------------------------------------------------
 * Function Declarations
 * -------------------------------------------------------------------------*/

/**
 * @brief This function is just a stub for MBA. 
 *
 * @return TRUE  
 */
boolean sec_pil_is_ns_range(const void* start, uint32 len);

/**
 * @brief This function is just a stub for MBA. 
 *
 * @return 0  
 */
int sec_pil_subsys_bring_up(sec_pil_proc_et proc, uint32 start_addr);

/**
 * @brief This function is just a stub for MBA. 
 *
 * @return 0  
 */
int32 sec_pil_prepare_subsys(sec_pil_proc_et proc);

/**
 * @brief Locks the image memory area.
 *
 * @param[in] proc  The processor ID of the PIL image.
 * @param[in] hdr   Pointer to the first entry in ELF program header.
 * @param[in] count Number of program header entries in hdr.
 * @param[in] entry The entry address to the image.
 *
 * @return 0 on success.
 */
int sec_pil_lock_xpu(sec_pil_proc_et proc, const Elf32_Phdr* hdr,
                     uint32 count, uint32 entry);

/**
 * @brief Unlocks the image memory area.
 *
 * @param[in] proc  The processor to unlock
 *
 * @return 0 on success.
 */
int sec_pil_unlock_xpu(sec_pil_proc_et proc);

/**
 * @brief This funnction returns PBL secboot function pointers.
 *
 * @param[in,out]  ftbl_ptr     secboot function pointers.
 * @param[in,out]  ftbl_hw_ptr  secboot hw function pointers.
 *
 * @return 0 on success. 
 */
int32 sec_pil_get_secboot_routines(secboot_ftbl_type *ftbl_ptr,
                                    secboot_hw_ftbl_type *ftbl_hw_ptr);

/**
 * @brief This funnction returns code segment in SECURE_BOOTn register
 *        containing authentication information of the image.
 *
 * @param[in]      proc          The peripheral image.
 * @param[in,out]  code_segment  Code segment in SECURE_BOOTn register
 *                               containing authentication information
 *                               of the image.
 *
 * @return E_SUCCESS on success and E_INVALID_ARG for invalid args.
 */
int32 sec_pil_get_code_segment(sec_pil_proc_et proc, uint32 *code_segment);

/**
 * @brief This funnction returns sw version for the peripheral image. 
 *
 * @param[in]      proc   The peripheral image.
 * @param[in,out]  sw_id  SW version for the peripheral image. 
 *
 * @return E_SUCCESS on success and E_INVALID_ARG for invalid args.
 */
int32 sec_pil_get_sw_version(sec_pil_proc_et proc, uint32 *sw_version);

/**
 * @brief This funnction returns sw type for the peripheral image. 
 *
 * @param[in]      proc   The peripheral image.
 * @param[in,out]  sw_id  SW type for the peripheral image. 
 *
 * @return E_SUCCESS on success and E_INVALID_ARG for invalid args.
 */
int32 sec_pil_get_sw_id(sec_pil_proc_et proc,uint32 *sw_id);

/**
 * Dynamically locks an MPU resource group (memory area). Memory area start and
 * end address alignment is dependent on the actual MPU.
 *
 * @param [in] resource   The resource group id the MPU was assigned to.
 * @param [in] start      Start of the memory area.
 * @param [in] end        End of the memory area.
 *
 * @return Zero on success.
 */
int32 sec_pil_mpu_lock_area(uint32 *resource, uint32 start, uint32 end);

/**
 * Dynamically unlocks an MPU resource group (memory area).
 *
 * @param [in] resource The resource group id the MPU was assigned to.
 *
 * @return Zero on success, otherwise an error code.
 */
int32 sec_pil_mpu_unlock_area(uint32 resource);

/**
 * @brief This function is just a stub for MBA. 
 */
void sec_pil_dcache_inval_region(void *addr, unsigned int length);

/**
 * @brief  This function will create a message digest hash using the
 *         algorithm specified.
 *
 * @param hash[in]         The hash algorithm
 * @param msg[in]          The message to hash
 * @param msg_len[in]      The length of the message
 * @param digest[in,out]   The digest to store
 * @param digest_len[in]   Length of the output message digest hash
 *                         buffer in bytes. Must be 20 bytes for SHA1 and
 *                         32 bytes for SHA256
 *
 * @return 0 on success, negative on failure
 */
int sec_pil_hash(sec_pil_hash_algo_et alg,
                 const uint8 *msg,
                 uint32 msg_len,
                 uint8 *digest,
                 uint32 digest_len);

/**
 * @brief Intialize a hash context for update and final functions.
 *
 * @param alg[in]          The algorithm standard to use
 * @param hash_ctx[in,out] The hash context
 *
 * @return 0 on success, negative on failure
 *
 */
int sec_pil_hash_init(sec_pil_hash_algo_et alg,
                      sec_pil_hash_ctx *hash_ctx);

/**
 * @brief  This function will hash some data into the hash context
 *         structure, which Must have been initialized by
 *         sec_pil_hash_init().
 *
 * @param hash_ctx[in]    The hash context
 * @param msg[in]         Pointer to the msg to hash
 * @param msg_len[in]     Length of the msg to hash
 *
 * @return 0 on success, negative on failure
 *
 * @see sec_pil_hash_init
 */
int sec_pil_hash_update(const sec_pil_hash_ctx  *hash_ctx,
                        const uint8           *msg,
                        uint32                msg_len);

/**
 * @brief  Compute the digest hash value
 *
 * @param hash_ctx[in]     The hash context
 * @param digest[in]       Pointer to output message digest hash
 * @param digest_len[in]   Length of the output message digest hash
 *                         buffer in bytes. Must be 20 bytes for SHA1 and
 *                         32 bytes for SHA256
 *
 * @return 0 on success, negative on failure
 *
 * @see sec_pil_hash_init
 */
int sec_pil_hash_final(sec_pil_hash_ctx  *hash_ctx,
                       uint8             *digest,
                       uint32            digest_len);

/**
 * @brief Release all resources with a given hash context.
 *
 * @param hash_ctx[in] Hash context to be deleted
 *
 * @return 0 on success, negative on failure
 *
 */
int sec_pil_hash_free_ctx(sec_pil_hash_ctx *hash_ctx);

/**
 * @brief  This function returns TRUE if the peripheral image is supported
 *         or else returns FALSE.
 *
 * @param[in] proc  The peripheral image.
 *
 * @return TRUE if the proc is supported and FLASE if the proc is not supported.
 */
boolean sec_pil_is_proc_supported(sec_pil_proc_et proc);

int sec_pil_get_load_addr(sec_pil_proc_et proc, uint32 *new_start);

#endif /* SEC_PIL_ENV_H */
