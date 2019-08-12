#ifndef SEC_PIL_H
#define SEC_PIL_H

/**
@file sec_pil.h
@brief Secure Peripheral Image Loader
*/

/*===========================================================================
   Copyright (c) 2012-2014 by QUALCOMM Technologies, Inc.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                            EDIT HISTORY FOR FILE

  $Header: //components/rel/core.mpss/3.5.c12.3/securemsm/secpil/inc/sec_pil.h#1 $
  $DateTime: 2016/12/13 07:59:23 $
  $Author: mplcsds1 $

when       who      what, where, why
--------   ---      ------------------------------------
02/26/12   vg       Included sec_pil_env.h to resolve compilation errors
                    for MBA. 
02/20/12   vg       Ported from TZ PIL.

===========================================================================*/

/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/
#include <comdef.h>
#include "miprogressive.h"
#include "miheader.h"

/*----------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * -------------------------------------------------------------------------*/

#define ELF_PT_NULL    0
#define ELF_PT_LOAD    1


/*----------------------------------------------------------------------------
 * Type Declarations
 * -------------------------------------------------------------------------*/

typedef enum
{
  SEC_PIL_AUTH_MODEM_PROC,           /**<-- Modem Processor */
  SEC_PIL_AUTH_QDSP6_PROC,           /**<-- QDSP6 Processor */
  SEC_PIL_AUTH_SPS_PROC,             /**<-- SPS Processor */
  SEC_PIL_AUTH_EXEC,                 /**<-- EXEC Image */
  SEC_PIL_AUTH_MODEM_SW_PROC,        /**<-- Modem SW Processor */
  SEC_PIL_AUTH_MODEM_FW_PROC,        /**<-- Modem FW Processor */
  SEC_PIL_AUTH_RIVA_PROC,            /**<-- Riva Processor */
  SEC_PIL_AUTH_SEC_APP,              /**<-- TZ secure appliactions */
  SEC_PIL_AUTH_GNSS_PROC,            /**<-- GSS Processor */
  SEC_PIL_AUTH_VIDEO_PROC,           /**<-- Video Processor */
  SEC_PIL_AUTH_DBG_POLICY,
  SEC_PIL_AUTH_UNSUPPORTED_PROC,     /**<-- Processor not Supported */
    /**<-- No of Processors supported */
  SEC_PIL_AUTH_NUM_SUPPORTED_PROCS = SEC_PIL_AUTH_UNSUPPORTED_PROC 
}sec_pil_proc_et;
typedef struct sec_pil_verified_info_s
{
   uint64  sw_id;        /**< The software id (upper 32 bits:version, lower 32 bits:type)
                         the image was signed with */
   uint32  enable_debug; /**< Value of the debug settings from the attestation cert i.e
                         SECBOOT_DEBUG_NOP, SECBOOT_DEBUG_DISABLE, SECBOOT_DEBUG_ENABLE */
} sec_pil_verified_info_t;


/*----------------------------------------------------------------------------
 * Function Declarations and Documentation
 * -------------------------------------------------------------------------*/
 
/**
 * @brief Initilize all static/global variables used for PIL
 *
 * @return 0 on success, negative on failure
 */
int sec_pil_init(void);

/**
 * @brief Authenticates hash table segment against its signature.
 *
 * @param[in] proc        The peripheral image
 * @param[in] elf_hdr     Pointer to the ELF header 
 *
 * @return 0 on success, negative on failure.
 */
int sec_pil_init_image(sec_pil_proc_et proc,
                       const Elf32_Ehdr * elf_hdr,
                       sec_pil_verified_info_t *v_info,
                       const uint8 *key,
                       uint32 key_len);
/**
 * @brief Clean up any memory associated with initialization
 *
 * @param[in] proc        The peripheral image
 */
void sec_pil_deinit_image(sec_pil_proc_et proc);

/**
 * @brief Validates ELF segments against hash table and reset peripheral 
 *
 * @param[in] proc  The peripheral image
 * @param[in] reset Brings up the subsytem if the reset is TRUE.
 *
 * @return 0 on success, negative on failure
 */
int sec_pil_auth_reset(sec_pil_proc_et proc,boolean reset);

/**
 * @brief Authenticates hash table segment against its signature.
 *
 * @param[in] proc              The peripheral image
 * @param[in] image_start_addr  The physical start address of the ELF Program Segments
 * @param[in] length_loaded     Length of the image loaded so far.
 *
 * @return 0 on success, negative on failure.
 */
int sec_pil_verify_segments(sec_pil_proc_et proc,
                            uint32 image_start_addr,
                            uint32 length_loaded,
                            uint32 total_image_length);


/**
 * @brief Gets the image entry's point.
 *
 * @param[in]  proc         The peripheral image
 * @param[out] image_entry  Holds the image entry point.
 *
 * @return 0 on success, negative on failure.
 */
int sec_pil_get_entry (sec_pil_proc_et proc,uint32 *image_entry);

/**
 * @brief Gets the image entry's point.
 *
 * @param[in]  proc          The peripheral image
 * @param[out] image_length  Holds the image length.
 *
 * @return 0 on success, negative on failure.
 */
int sec_pil_get_image_length(sec_pil_proc_et proc, uint32 *image_length);

/**
 * @brief Gets the meta data length from ELF and program headers.
 *        Meta data = size of ELF header+size of proram header+size of hash segment.
 *
 * @param[in]      proc               The peripheral image
 * @param[in]      elf_hdr            Pointer to the ELF header
 * @param[in,out]  meta_data_length   Holds the meta data length loaded for input
 *                                    and holds the actual meta data length required. 
 *
 * @return 0 on success, negative on failure.
 */
int sec_pil_get_meta_data_length(sec_pil_proc_et proc,
                                 const Elf32_Ehdr * elf_hdr,
                                 uint32 *meta_data_length);

/**
 * @brief Gets the image length in memory.
 *
 * @param[in]  proc          The peripheral image
 * @param[out] image_length  Holds the image length.
 *
 * @return 0 on success, negative on failure.
 */
int sec_pil_get_image_length_in_memory(sec_pil_proc_et proc, uint32 *image_length);

/**
 * @brief Check for loadable, non paged segments
 *
 * @param[in] Program hdr entry for the segment
 *
 * @result \c TRUE if the segment is valid, \c FALSE otherwise.
 */
boolean sec_pil_is_valid_segment(const Elf32_Phdr* entry);

/**
 * @brief Find the elf segment that matches seg_flags
 *
 * @param[in] phdr The program header entry start ptr
 * @param[in] end The last program header entry ptr
 * @param[in] seg_flags The type of the segment
 *
 * @return - NULL if no match if found
 */
Elf32_Phdr *sec_pil_elf_find_seg(const Elf32_Phdr * phdr, const Elf32_Phdr * end,
                                 uint32 seg_flags);

/**
 * @brief Find the segment of the given p_flags
 *
 * @param[in] phdr The program header entry start ptr
 * @param[in] end The program header entry end ptr
 * @param[in] seg_flags The p_flags of a given segment
 *
 * @return Valid program header pointer if found, \c NULL otherwise.
 */
int sec_pil_auth_validate_hash_segment_entries(const Elf32_Phdr * phdr, const Elf32_Phdr * end,
                                               uint32 seg_flags);

/**
 * @brief Given an image that contains a Unified Image Encryption (UIE) header,
 *          Attempt to iterate through all ELF segments, and perform any
 *          decryption necessary, based on the header parameters.
 *        NOTE: This performs IN-PLACE decryption on the image, and will change
 *              the segment data upon successful completion.
 *
 * @param[in] proc        The peripheral image
 *
 * @return 0 on success, Error codes otherwise (defined in uie.h)
 */
int sec_pil_decrypt_segments(sec_pil_proc_et proc, void* logger);

/**
 *  @brief Returns whether or not image contained a proper UIE header
 *
 *  @param[in] proc       The peripheral image
 *
 *  @return TRUE if image is encrypted, FALSE otherwise.
 */
boolean sec_pil_is_image_encrypted(sec_pil_proc_et proc);

#endif /* SEC_PIL_H */
