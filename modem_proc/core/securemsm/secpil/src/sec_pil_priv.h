#ifndef SEC_PIL_PRIV_H
#define SEC_PIL_PRIV_H

/**
* @file sec_pil_priv.h
* @brief Contains internal function declarations for secure pil.
*/

/*===========================================================================
   Copyright (c) 2012 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                            EDIT HISTORY FOR FILE

  $Header: //components/rel/core.mpss/3.5.c12.3/securemsm/secpil/src/sec_pil_priv.h#1 $
  $DateTime: 2016/12/13 07:59:23 $
  $Author: mplcsds1 $

when       who      what, where, why
--------   ---      ------------------------------------
04/05/12   vg       Added segmented authentication changes.
02/20/12   vg       Ported from TZ PIL.

===========================================================================*/

/*----------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * -------------------------------------------------------------------------*/

#define SEC_PIL_ROUNDUP(type, val, multiple) (((val) + (multiple)-1) & \
                                              ~(type)((multiple)-1))

#define SEC_PIL_PAGE_4K           4096
/* Clears the lowest 12 bits on xx. */
#define SEC_PIL_FLOOR_4K(xx)      ((xx) & ~(SEC_PIL_PAGE_4K-1))

#define SEC_PIL_SHA1_HASH_SZ    20
#define SEC_PIL_SHA256_HASH_SZ  32

/* Activating SHA256  */
#define SEC_PIL_HASH_SZ     SEC_PIL_SHA256_HASH_SZ
#define SEC_PIL_HASH_ALG    SEC_PIL_HASH_SHA256

#define UINT32_MAX_VALUE 0xFFFFFFFF
#define UINT16_MAX_VALUE 0xFFFF
#define UINT8_MAX_VALUE  0xFF

/*----------------------------------------------------------------------------
 * Type Declarations
 * -------------------------------------------------------------------------*/

typedef enum
{
  SEC_PIL_STATE_INIT = 1,
  SEC_PIL_STATE_RESET,
  SEC_PIL_STATE_DONE
}sec_pil_state_et;

typedef struct sec_pil_elf_info_s
{
  Elf32_Ehdr  *elf_hdr;       /**< Pointer to the elf header */
  Elf32_Phdr  *prog_hdr;      /**< Pointer to the start of the program header; */
  uint32      prog_hdr_num;   /**< Number of program header entries */
  uint8*      hash_seg_hdr;   /**< Pointer to the hash segment header*/
  uint32      hash_seg_hdr_sz; /**< Hash segment header size in bytes */
  uint8*      hash_seg;       /**< Pointer to the hash segment */
  uint32      hash_seg_sz;    /**< Hash segment size in bytes */
  uint8*      sig_ptr;        /**< Pointer to the signature */
  uint32      sig_sz;         /**< Signature Size */
  uint8*      cert_ptr;       /**< Pointer to the certificate */
  uint32      cert_sz;        /**< Size of the certificate */
  uint8*      uie_hdr_ptr;    /**< Pointer to the UIE header */
} sec_pil_elf_info_t;

typedef struct sec_pil_priv_s
{
  sec_pil_elf_info_t elf;
  sec_pil_proc_et proc;  
  sec_pil_state_et state;
  ssd_metadata_type *ssd_md;
  uie_ctx *uie;
} sec_pil_priv_t;

typedef struct sec_pil_seg_auth_info_s
{
  uint8  cur_prog_hdr;
  sec_pil_proc_et proc;
  boolean init;  
  uint8 *read_ptr;
  uint32 len_read;  
  uint8 *hash;
  uint32 hash_len;
  uint32 cur_elf_len;  
} sec_pil_seg_auth_info_t;


/*----------------------------------------------------------------------------
 * Function Declarations
 * -------------------------------------------------------------------------*/

/**
 * @brief This funnction  frees the memory allocated if any and zeros out data
 *        present in the data structures for pil and seg auth info.
 *
 * @param[in,out]  pil       Pointer to Pil internal context data
 * @param[in,out]  seg_info  Pointer to segmented authentication internal 
 *                           context data. 
 *
 * @return E_SUCCESS on success and E_INVALID_ARG for invalid args.
 */
void sec_pil_clean_pil_priv(sec_pil_priv_t *pil,
                            sec_pil_seg_auth_info_t *seg_info);

/**
 * @brief Elf header parser, to check for the correct format.
 *
 * @param[in] elf_hdr   Pointer to the ELF header.
 *
 * @result \c TRUE if the header is in correct format, \c FALSE otherwise.
 */
boolean sec_pil_is_elf(const Elf32_Ehdr * elf_hdr);

/**
 * @brief Populate the PIL info given the provided ELF image.
 *
 * @param[in] proc Peripheral image type
 * @param[in] elf_hdr Pointer to the elf header
 * @param[in] pil Elf header info struct
 * @postcondition This routine allocates memory within \c sec_pil_elf_info_t. So
 *                the caller must free this memory when it is no longer needed.
 *
 * @return 0 on success, negative on failure.
 */
int sec_pil_populate_elf_info(sec_pil_proc_et proc,
                              const Elf32_Ehdr *elf_hdr,
                              sec_pil_elf_info_t *pil);

/**
 * @brief Computes the actual end address for XPU from the length loaded so far.
 *        length loaded so far might not be equal to the length on DDR as length
 *        loaded does not include alignment gaps and just includes mem size.
 *
 * @param[in]     length_loaded  Length loaded so far.  
 * @param[in]     pil            Pointer to Pil internal data context
 * @param[in,out] end_addr       End address to be configured for XPU.
 *
 * @return 0 on success, negative on failure.
 */
int sec_pil_get_end_addr(uint32 length_loaded, 
                         const sec_pil_priv_t *pil,
                         uint32 *end_addr);

#endif /* SEC_PIL_PRIV_H */

