#ifndef TZBSP_PIL_H
#define TZBSP_PIL_H

/**
@file tzbsp_pil.h
@brief Peripheral Image Loader
*/

/*===========================================================================
   Copyright (c) 2010 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                            EDIT HISTORY FOR FILE

  $Header: //components/rel/tz.bf/2.5.c10/trustzone_images/core/securemsm/trustzone/qsee/services/img_auth/inc/tzbsp_pil.h#1 $
  $DateTime: 2016/06/17 14:31:11 $
  $Author: pwbldsvc $

when       who      what, where, why
--------   ---      ------------------------------------
10/20/10   cap      Initial version.

===========================================================================*/

/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/
#include <comdef.h>
#include "miprogressive.h"
#include "miheader.h"

#include "tz_syscall_pub.h"

/*----------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * -------------------------------------------------------------------------*/

#define ELF_PT_NULL    0
#define ELF_PT_LOAD    1

/*----------------------------------------------------------------------------
 * Type Declarations
 * -------------------------------------------------------------------------*/

typedef struct tzbsp_elf_info_s
{
  Elf32_Ehdr  *elf_hdr;       /**< Pointer to the elf header */
  Elf32_Phdr  *prog_hdr;      /**< Pointer to the start of the program header; */
  uint32      prog_hdr_num;   /**< Number of program header entries */
  uint8*      hash_seg_hdr;   /**< Pointer to the hash segment */
  uint32      hash_seg_hdr_sz;/**< Hash segment size in bytes */
  uint8*      hash_seg;       /**< Pointer to the hash segment */
  uint32      hash_seg_sz;    /**< Hash segment size in bytes */
  uint8*      sig_ptr;        /**< Pointer to the signature */
  uint32      sig_sz;         /**< Signature Size */
  uint8*      cert_ptr;       /**< Pointer to the certificate */
  uint32      cert_sz;        /**< Size of the certificate */
} tzbsp_elf_info_t;

/*----------------------------------------------------------------------------
 * Function Declarations and Documentation
 * -------------------------------------------------------------------------*/

/**
 * @brief Verify the signature of an image
 * Provides signature validation of the hash table segment,
 * and the authentication of the elf header and program header table
 * @param[in] proc The peripheral image
 * @param[in] elf The elf header of the loaded image
 * @return 0 on success, negative on failure
 */
int32 tzbsp_pil_verify_sig(tz_pil_proc_e_type proc,
                           tzbsp_elf_info_t *elf);

/**
 * @brief Verify the hash on all loadable program segments. The hash in compared
 *        is compared to the hash in the stored hash segment
 * @param[in] proc The peripheral image
 * @param[in] elf The ELF header of the loaded image
 * @return 0 on success, negative on failure
 */
int32 tzbsp_pil_auth_segments(tz_pil_proc_e_type proc,
                              const tzbsp_elf_info_t *elf);

/**
 * @brief Find the elf segment that matches seg_flags
 * @param[in] phdr The program header entry start ptr
 * @param[in] end The last program header entry ptr
 * @param[in] seg_flags The type of the segment
 * @return - NULL if no match if found
 */
Elf32_Phdr *tzbsp_elf_find_seg(const Elf32_Phdr * phdr, const Elf32_Phdr * end,
                               uint32 seg_flags);

/**
 * @brief PIL Auth Service find hash table segment function
 * @param[in] phdr The program header pointer
 **/
Elf32_Phdr * tzbsp_pil_lookup_hash_seg(const Elf32_Phdr * phdr);

/**
 * @brief Selects segments that contribute to the image area calculation.
 *
 * @param[in] entry  An entry in ELF program header.
 *
 * @return TRUE if the entry should be included in image area, FALSE if the
 * entry should be skipped.
 */
boolean tzbsp_is_valid_segment(const Elf32_Phdr* entry);

/**
 * @brief  Calculate the image area. The resulting address range is 4K page
 * aligned. The start of the range is inclusive in the range. The end of the
 * range is the first page after the range not in the range.
 *
 * @param[in]   phdr   The first entry in program header.
 * @param[in]   count  The number of entries in program header.
 * @param[in]   entry  The entry address to the image.
 * @param[out]  start  Contains image start address (on success).
 * @param[out]  end    Contains image end address (on success).
 *
 * @return Zero on success, an error code otherwise.
 */
int32 tzbsp_calc_image_area(const Elf32_Phdr* hdr, uint32 count,
                            uint32 entry, uint32* start, uint32* end);

/**
 * @brief Locks the image memory area.
 *
 * @param[in] proc  The processor ID of the PIL image.
 * @param[in] hdr   Pointer to the first entry in ELF program header.
 * @param[in] count Number of entries in \c hdr.
 * @param[in] entry The entry address to the image.
 *
 * @return 0 on success, negative on failure
 */
int32 tzbsp_pil_lock_xpu(tz_pil_proc_e_type proc, const Elf32_Phdr* hdr,
                         uint32 count, uint32 entry);

/**
 * @brief Unlocks the image memory area.
 *
 * @param[in] proc  The processor to unlock
 *
 * @return 0 on success, negative on failure
 */
int32 tzbsp_pil_unlock_xpu(tz_pil_proc_e_type proc);

/**
 *  @brief Unlock the memory area and zero out all memory
 *
 * @param[in] proc  The processor ID of the PIL image.
 *
 * @return 0 on success, negative on failure
 */
int32 tzbsp_pil_unlock_xpu_zero(tz_pil_proc_e_type proc);

#endif /* TZBSP_PIL_H */
