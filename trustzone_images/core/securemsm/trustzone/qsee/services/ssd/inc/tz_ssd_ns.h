#ifndef TZ_SSD_NS_H
#define TZ_SSD_NS_H

/**
@file tz_ssd_ns.h
@brief Secure Software Download
*/

/*===========================================================================
   Copyright (c) 2013 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                            EDIT HISTORY FOR FILE

  $Header: //components/rel/tz.bf/2.5.c10/trustzone_images/core/securemsm/trustzone/qsee/services/ssd/inc/tz_ssd_ns.h#1 $
  $DateTime: 2016/06/17 14:31:11 $
  $Author: pwbldsvc $

when       who      what, where, why
--------   ---      ------------------------------------
10/20/10    rv      Initial version.

===========================================================================*/

/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/
#include <comdef.h>
#include "tz_ssd.h"
#include "tz_syscall_pub.h"


/*----------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * -------------------------------------------------------------------------*/


/*----------------------------------------------------------------------------
 * Type Declarations
 * -------------------------------------------------------------------------*/


/*----------------------------------------------------------------------------
 * Function Declarations and Documentation
 * -------------------------------------------------------------------------*/
/**
   Parses SSD metadata and saves the contents for future calls to
   decrypt fragment

   @param[in]   mem_len    length of the given memory in bytes
   @param[in]   mem_ptr    pointer to memory that may contain metadata
   @param[out]  ret_ptr    Return structure for the parse md request.
   @param[in]   ret_len    Length of the return structure.

   @return 0 on success, negative on failure
*/
int tzbsp_ssd_parse_md_ns
(
  uint32                         mem_len,
  uint32*                        mem_ptr,
  tz_ssd_parse_md_rsp_info_t* ret_ptr,
  uint32                         ret_len
);

/**
   Decrypt elf segment fragments

   @param[in]  md_ctx_id    Context token from parsing the metadata.
   @param[in]  segment_num  Current segment number being decrypted
   @param[in]  last_seg     Specifies the last segment or not
   @param[in]  last_frag    Specifies last fragment or not
   @param[in]  frag_len     Length of the given fragment.
   @param[in]  frag_ptr     Pointer to encrypted image fragment.
   @param[out] status_ptr   Return value of function.
   @param[in]  status_len   Length of the status pointer.

   @return 0 on success, negative on failure
*/
int tzbsp_ssd_decrypt_elf_seg_frag_ns
(
  uint32  md_ctx_id,
  uint32  segment_num,
  uint32  last_seg,
  uint32  last_frag,
  uint32  frag_len,
  uint32* frag_ptr,
  uint32* status_ptr,
  uint32  status_len
);

/**
   Decrypt image fragments

   @param[in]  md_ctx_id    Context token from parsing the metadata.
   @param[in]  frag_ptr     Pointer to encrypted image fragment.
   @param[in]  frag_len_ptr Pointer to length of fragment.
   @param[in]  last_frag    Specifies last fragment or not [1 = last fragment].
   @param[out] status_ptr   Return value of function.
   @param[in]  status_len   Length of the status pointer.

   @return 0 on success, negative on failure
*/
int tzbsp_ssd_decrypt_img_frag_ns
(
  uint32  md_ctx_id,
  uint32  last_frag,
  uint32  frag_len,
  uint32* frag_ptr,
  uint32* status_ptr,
  uint32  status_len
);

/**
   Parse the XML in the encrypted image & decrypt the data in-place
   Caller needs to allocate the memory in non secure memory region

   @param[in] img_ptr         Pointer to bootlader image
   @param[in] img_len_ptr     Pointer to length of image

   @return 0 on success, negative on failure
*/
/* TODO: remove API */
int tzbsp_ssd_decrypt_img_ns
(
  uint32** img_ptr,
  uint32*  img_len_ptr
);

/**
   Loads a keystore given from the HLOS into protected memory and
   decrypts it.

   @param[in]  keystore_ptr Pointer to the keystore
   @param[in]  keystore_len Length of the keystore in bytes
   @param[out] retval       Return value of function
   @param[in]  retval_len   Length of the return value pointer
 */
int tzbsp_ssd_protect_keystore_ns
(
  uint32*  keystore_ptr,
  uint32   keystore_len,
  uint32*  retval_ptr,
  uint32   retval_len
);

/**
   Encrypt the Keystore with a key derived from the CE hardware key

   @param[in] keystore_ptr        Pointer to length of keystore
   @param[in] keystore_len        Length of keystore

   @return 0 on success, negative on failure
*/
int tzbsp_ssd_encrypt_keystore_ns
(
  uint32** ks_ptr,
  uint32*  ks_len
);

#endif /* TZ_SSD_NS_H */
