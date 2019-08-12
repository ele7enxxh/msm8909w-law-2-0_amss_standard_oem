#ifndef TZ_SSD_S_H
#define TZ_SSD_S_H

/**
@file tz_ssd_s.h
@brief Secure Software Download
*/

/*===========================================================================
   Copyright (c) 2013 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                            EDIT HISTORY FOR FILE

  $Header: //components/rel/tz.bf/2.5.c10/trustzone_images/core/securemsm/trustzone/qsee/services/ssd/inc/tz_ssd_s.h#1 $
  $DateTime: 2016/06/17 14:31:11 $
  $Author: pwbldsvc $

when       who      what, where, why
--------   ---      ------------------------------------
01/22/13   pre      Initial version.

===========================================================================*/

/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/
#include "tzbsp_pil.h"
#include "tz_ssd.h"


/*----------------------------------------------------------------------------
 * Function Declarations and Documentation
 * -------------------------------------------------------------------------*/

/**
   Parse the XML in the encrypted image & decrypt the data in-place
   Caller needs to allocate the memory in non secure memory region

   @param[in,out] img_ptr         Pointer to bootlader image
   @param[in,out] img_len_ptr     Pointer to length of image

   @pre Since this API is only exposed to the secure side and this API
        may be used to decrypt images in both secure and non-secure
        memory, the checking of the memory area for the images must be
        done by the caller.

   @return 0 on success, negative on failure
*/
int tzbsp_ssd_decrypt_img
(
  uint32** img_ptr,
  uint32* img_len_ptr
);

/**
   Parses SSD metadata and saves the contents for future calls to
   the secure decrypt fragment API

   @param[in]     mem_ptr         pointer to memory that may contain metadata
   @param[in]     mem_len         length of the given memory in bytes
   @param[in/out] ssd_ctx_ptr_ptr SSD context information.

   @pre Since this API is only exposed to the secure side and this API
        may be used to parse data in both secure and non-secure
        memory, the checking of the memory area for the data must be
        done by the caller.

   @return 0 on success, negative on failure
*/
int tzbsp_ssd_parse_md_s
(
  uint32*            mem_ptr,
  uint32             mem_len,
  ssd_context_type** ssd_ctx_ptr_ptr
);

/**
   Decrypt ELF segments.

   @param ssd_ctx_ptr_ptr  A pointer to pointer to a filled SSD context
                           struct.
   @param segment_num      The ELF segment number.
   @param last_seg         Is this the last segment in the ELF image?
   @param seg_len          The length of the segment.
   @param seg_ptr          The pointer to the PA start address of the loaded
                           segment.

   @pre tzbsp_ssd_parse_md_pil must be called before calling this
        function.  There should be no way to update the ssd_fs between
        the two calls.
   @pre ssd_ctx_ptr_ptr and *ssd_ctx_ptr_ptr are valid
   @pre seg_len must be non-zero
   @pre Since this API is only exposed to the secure side and this API
        may be used to decrypt data in both secure and non-secure
        memory, the checking of the memory area for the segments must
        be done by the caller.

   @return E_SUCCESS on success, negative on failure
*/
int tzbsp_ssd_decrypt_elf_seg_s
(
  ssd_context_type** ssd_ctx_ptr_ptr,
  uint32             segment_num,
  uint32             last_seg,
  uint32             seg_len,
  uint32*            seg_ptr
);

#endif /* TZ_SSD_S_H */
