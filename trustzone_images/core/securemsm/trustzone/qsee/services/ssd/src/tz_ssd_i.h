#ifndef TZ_SSD_I_H
#define TZ_SSD_I_H
/**
 * @file tz_ssd_i.h
 * @brief This module only has  API's that can be used to
 *        1. Validate & authenticate
 *        2. Parse the metadata
 *        3. Decrypt the encrypted binary image.
 * This file contains the definitions of the constants, data structures,
 * and interfaces that comprise the Secure Software Download
 * module.
 * Only the interfaces declared shall be used by the client for accessing
 * the Secure Software Download module.
 *
 *
 * ===========================================================================
 *                           Edit History
 * when       who   what, where, why
 * --------   ---   --------------------------------------------
 * 02/02/10   rv     Create Module
 * ===========================================================================
*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "tz_ssd.h"
#include "tz_syscall_pub.h"

/*===========================================================================

                 DEFINITIONS AND TYPE DECLARATIONS

===========================================================================*/

/*===========================================================================

                      PUBLIC FUNCTION DECLARATIONS

===========================================================================*/
/**
   This function release all temporary resources

   @sideeffects Frees memory held in input pointer and releases the
                global SSD CE cipher handle.
*/
void ssd_tz_deinit(ssd_context_type *pctx);


/**
   Parses SSD metadata and saves the contents for future calls to
   decrypt fragment

   @param[in]   mem_len    length of the given memory in bytes.
   @param[in]   mem_ptr    pointer to memory that may contain metadata.
   @param[out]  ret_ptr    Return structure for the parse md request.
   @param[out]  ssd_ctx_ptr_ptr Decryption information taken from metadata.

   @sa Allocates memory for ssd_ctx_ptr_ptr.  Must call
       ssd_tz_deinit after calling this funciton.
   @dep Input is sanitized before calling
*/
void tzbsp_ssd_parse_md_i
(
  uint32                         mem_len,
  uint32*                        mem_ptr,
  tz_ssd_parse_md_rsp_info_t* ret_ptr,
  ssd_context_type**             ssd_ctx_ptr_ptr
);

/**
   Decrypt elf segment fragments

   @param[in]  ssd_ctx_ptr_ptr Decryption information from parse_md
   @param[in]  segment_num  Current segment number being decrypted
   @param[in]  last_seg     Specifies the last segment or not
   @param[in]  last_frag    Specifies last fragment or not
   @param[in]  frag_len     Length of the given fragment.
   @param[in]  frag_ptr     Pointer to encrypted image fragment.

   @warn Does not deinit on error.  Only deinits when last_seg and
         last_frag are both TRUE and decryption happens successfully.
         Caller must deinit if error case is not handled.
   @pre  Must have a valid context from parsing metadata
 */
uint32 tzbsp_ssd_decrypt_elf_seg_frag_i
(
  ssd_context_type** ssd_ctx_ptr_ptr,
  uint32             segment_num,
  uint32             last_seg,
  uint32             last_frag,
  uint32             frag_len,
  uint32*            frag_ptr
);

/**
   Parse the XML in the encrypted bootloader image & decrypt the data
   in-place

   @param[in,out] img_ptr      Pointer to bootlader image
   @param[in,out] img_len_ptr  Pointer to length of image

   @return 0 on success, negative on failure
*/
int tzbsp_ssd_decrypt_img_i
(
  uint32**           img_ptr,
  uint32*            img_len_ptr
);

#endif /* TZ_SSD_I_H */
