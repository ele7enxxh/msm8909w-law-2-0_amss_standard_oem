/**
* @file tzos_pil_auth_sec.c
* @brief TZBSP PIL Auth Security implementation
*
* This file implements the TZBSP Authentication Routines,
* including signature validation and hash validation for the
* hash table segment, elf header and program header table
* segment
*
*/
/*===========================================================================
   Copyright (c) 2010 - 2013 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                            EDIT HISTORY FOR FILE

  $Header: //components/rel/tz.bf/2.5.c10/trustzone_images/core/securemsm/trustzone/qsee/services/img_auth/src/tzbsp_pil_auth_sec.c#1 $
  $DateTime: 2016/06/17 14:31:11 $
  $Author: pwbldsvc $


when       who      what, where, why
--------   ---      ------------------------------------
10/2013	   hw	    auth image in non-sec device
10/2013	   ygui		put the ZI memory after memory locking
03/2012    yh       software version rollback protection
04/2011    jct      Featurized crypto engine calls
10/2010    Nikita TZBSP AUTH SeC Implementation
===========================================================================*/

/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/
#include <string.h>

#include "tzbsp.h"
#include "tzbsp_mem.h"
#include "tzbsp_xpu.h"
#include "tzbsp_pil.h"
#include "tz_syscall_pub.h"
#include "tzbsp_secboot.h"
#include "crypto/tzbsp_hash.h"
#include "crypto/tzbsp_crypto.h"
#include "miprogressive.h"
#include "tzbsp_config.h"
#include "tzbsp_mmu_cache.h"
#include "tzbsp_rollback_version.h"

extern uint32 tzbsp_subsys_sw_type(tz_pil_proc_e_type proc, uint32* sw_type);

/*----------------------------------------------------------------------------
 * Function Declarations and Documentation
 * -------------------------------------------------------------------------*/

/**
 * @brief Authenticate the ELF and Program Headers by checking
 *        that the hash of the headers matches the hash entry
 *        in the hash table segment.
 *
 * @param[in] elf Pointer to the ELF header.
 */
static int32 tzbsp_auth_elf_headers(const tzbsp_elf_info_t *elf)
{
  int32 ret = 0;
  uint8 *hash = NULL;
  uint8 sha_digest[TZBSP_SHA256_HASH_SZ];

  if (elf == NULL)
  {
    TZBSP_ERROR_CODE(AUTH_ELF_HEADERS_ELF_IS_NULL);
    return -E_FAILURE;
  }

  if (elf->prog_hdr_num == 0)
  {
    TZBSP_ERROR_CODE(AUTH_ELF_HEADERS_PROG_HDR_NUM_ZERO);
    return -E_FAILURE;
  }

  /* There is an assumption that the program headers directly
   * follow the elf header here */
  ret = tzbsp_hash(TZBSP_HASH_SHA256,
                   (uint8*)elf->elf_hdr,
                   sizeof(Elf32_Ehdr) + ((sizeof(Elf32_Phdr) * elf->prog_hdr_num)),
                   sha_digest,
                   sizeof(sha_digest));
  if (ret < 0)
  {
    TZBSP_ERROR_CODE(AUTH_ELF_HEADERS_HASH_FAILED);
    return -E_FAILURE;
  }

  hash = elf->hash_seg;

  if (hash == NULL || elf->hash_seg_sz == 0)
  {
    TZBSP_ERROR_CODE(AUTH_ELF_HEADERS_NULL_HASH);
    return -E_FAILURE;
  }

  /* Check the hash is correct */
  if(memcmp(hash, sha_digest, sizeof(sha_digest)) != 0)
  {
    TZBSP_ERROR_CODE(AUTH_ELF_HEADERS_INVALID_DIGEST);
    if (TRUE == tzbsp_secboot_hw_is_auth_enabled(1))
    {
      return -E_FAILURE;
    }
  }
  return E_SUCCESS;
}

/**
 * @brief Zero out all segments where the memory size differs
 *        from the file size.
 *
 * @note Also verify that the segment size looks correct
 *
 * @param[in] elf Pointer to the ELF segment
 *
 * @return 0 on success, negative on failure
 */
static int32 tzbsp_pil_zero_segments(const tzbsp_elf_info_t *elf)
{
  uint32 i = 0;
  uint32 len = 0;

  for (; i < elf->prog_hdr_num; ++i)
  {
    if (tzbsp_is_valid_segment(&elf->prog_hdr[i]))
    {
      /* This is a corrupted image */
      if (elf->prog_hdr[i].p_filesz > elf->prog_hdr[i].p_memsz)
      {
        TZBSP_ERROR_CODE(PIL_ZERO_SEGMENTS_SIZE_MISMATCH);
        return -E_FAILURE;
      }

      len =  elf->prog_hdr[i].p_memsz - elf->prog_hdr[i].p_filesz;

      /* All segment lengths are rounded up to a multiple of 4K */
      if (len)
      {
        memset((void*)(elf->prog_hdr[i].p_paddr + elf->prog_hdr[i].p_filesz), 0, len);
        tzbsp_dcache_flush_region(
          (void*)(elf->prog_hdr[i].p_paddr + elf->prog_hdr[i].p_filesz), len);
      }
    }
  }
  return E_SUCCESS;
}

/**
 * @brief Perform a hash and verify for a segment.
 *
 * @param[in] hash Pointer to a pre-computed SHA hash
 * @param[in] hash_len The lenght in bytes of the SHA hash
 * @param[in] seg The segment to computer the hash on
 * @param[in] seg_len The length on the segment
 *
 * @return 0 on success, negative on failure
 *
 * @note This is just a simple SHA wrapper
 */
static int32 tzbsp_auth_segment(const uint8 *hash, uint32 hash_len,
                                const uint8 *seg,  uint32 seg_len)
{
  /* We are assuming SHA256 for now */
  uint8 sha_digest[TZBSP_SHA256_HASH_SZ];
  int32 ret = E_SUCCESS;

  if (hash == NULL || seg == NULL || seg_len == 0)
  {
    TZBSP_ERROR_CODE(AUTH_SEGMENT_ZERO_ARGS);
    return -E_INVALID_ARG;
  }

  if (hash_len != TZBSP_SHA256_HASH_SZ)
  {
    TZBSP_ERROR_CODE(AUTH_SEGMENT_INVALID_HASH_LEN);
    return -E_OUT_OF_RANGE;
  }



  ret = tzbsp_hash(TZBSP_HASH_SHA256,
                   seg,
                   seg_len,
                   sha_digest,
                   sizeof(sha_digest));
  if (ret != E_SUCCESS)
  {
    TZBSP_ERROR_CODE(AUTH_SEGMENT_HASH_FAILED);
    return -E_FAILURE;
  }

  /** Check the hash is correct */
   if(memcmp(hash, sha_digest, sizeof(sha_digest)) != 0)
   {
     TZBSP_ERROR_CODE(AUTH_SEGMENT_INVALID_DIGEST);
     return -E_FAILURE;
   }


  return E_SUCCESS;
}

/**
 * @brief Verify the hash on all loadable program segments
 *
 * @param[in] elf ELF header of the loaded image
 *
 * @return 0 on success, negative on failure
 */
int32 tzbsp_pil_auth_segments(tz_pil_proc_e_type proc,
                              const tzbsp_elf_info_t *elf)
{
  uint32 i          = 0;
  uint8 *hash       = NULL;
  uint32 hash_len   = 0;
  int32  ret        = 0;

  if(elf == NULL)
  {
    TZBSP_ERROR_CODE(PIL_AUTH_SEGMENTS_NULL_ELF);
    return -E_FAILURE;
  }

  hash = elf->hash_seg;
  hash_len = elf->hash_seg_sz;

  if (hash == NULL || hash_len == 0)
  {
    TZBSP_ERROR_CODE(PIL_AUTH_SEGMENTS_HASH_NULL);
    return -E_FAILURE;
  }

  /* Skip the first header + first hash.
   * This was already checked in signature validation */
  hash += TZBSP_SHA256_HASH_SZ;

  if (tzbsp_pil_lock_xpu(proc, elf->prog_hdr,
                         elf->prog_hdr_num, elf->elf_hdr->e_entry) < 0)
  {
    TZBSP_ERROR_CODE(PIL_AUTH_SEGMENTS_LOCK_XPU_FAILED);
    return -E_FAILURE;
  }

  if (tzbsp_pil_zero_segments(elf) < 0)
  {
    TZBSP_ERROR_CODE(PIL_AUTH_SEGMENTS_ZERO_SEGMENTS_FAILED);
    return -E_FAILURE;
  }

  for (; i < elf->prog_hdr_num; ++i)
  {
    if (hash_len < TZBSP_SHA256_HASH_SZ)
    {
      TZBSP_ERROR_CODE(PIL_AUTH_SEGMENTS_INVALID_HASH_LEN);
      return -E_FAILURE;
    }

    if (tzbsp_is_valid_segment(&elf->prog_hdr[i]) &&
        elf->prog_hdr[i].p_filesz != 0)
    {
      ret = tzbsp_auth_segment(hash, TZBSP_SHA256_HASH_SZ,
                               (uint8*)elf->prog_hdr[i].p_paddr,
                               elf->prog_hdr[i].p_filesz);

      if (ret < 0)
      {
        TZBSP_ERROR_CODE(PIL_AUTH_SEGMENTS_SEGMENT_AUTH_FAILED);
        tzbsp_pil_unlock_xpu(proc);
        return -E_FAILURE;
      }
    }

    hash_len -= TZBSP_SHA256_HASH_SZ;

    if(MI_PBT_SEGMENT_TYPE_VALUE(elf->prog_hdr[i].p_flags) !=
          MI_PBT_HASH_SEGMENT)
    hash += TZBSP_SHA256_HASH_SZ;
  }

  if (hash_len != 0)
  {
    TZBSP_ERROR_CODE(PIL_AUTH_SEGMENTS_REMAINING_HASH_BYTES);
    tzbsp_pil_unlock_xpu(proc);
    return -E_FAILURE;
  }
  return E_SUCCESS;
}


/**
 * @brief check if the image is signed or not
 *
 * @param[in] elf ELF header of the loaded image
 *
 * @return 0 if the image is not signed, return 1 if the image is signed
 */

int32 tzbsp_pil_is_signed_image(tzbsp_elf_info_t *elf)
{
  if (elf != NULL)
  {
  	if ( (elf->sig_sz != 0) && (elf->cert_ptr != NULL) && (elf->cert_sz != 0))
  	{
  	  return 1;
  	}
  }
  return 0;
}

int32 tzbsp_pil_verify_sig(tz_pil_proc_e_type proc,
                           tzbsp_elf_info_t *elf)
{
  int                         ret = E_SUCCESS;
  uint8                       *tmp = NULL;
  uint32                      start = 0, end = 0;
  uint32                      sw_type;
  secboot_image_info_type     image_info;
  secboot_verified_info_type  verified_info;
  uint64                      sw_id;
  uint32                      cert_sw_version;

  if(elf == NULL)
  {
    TZBSP_ERROR_CODE(PIL_VERIFY_SIG_NULL_ELF);
    return -E_FAILURE;
  }


  // enforce the image authentication in non-secure device
  if (tzbsp_secboot_hw_is_auth_enabled(1) == 0)
  {
    // In non-secure device, image needs not to be signed
    if (tzbsp_pil_is_signed_image(elf) == 0)
    {
    return E_SUCCESS;
  }
  }
  else
  {
    // In secure device, image has to be signed
    if (tzbsp_pil_is_signed_image(elf) == 0)
  {
    TZBSP_ERROR_CODE(PIL_VERIFY_SIG_ZERO_ARGS);
    return -E_FAILURE;
  }
  }


  /* Attempt to determine the SW type for this subsystem */
  if(tzbsp_subsys_sw_type(proc, &sw_type) != E_SUCCESS || 
     sw_type == (uint32)SECBOOT_INVALID_SW_TYPE)
  {
    return -E_FAILURE;
  }

  /* Lock the cert area - this protects against time of use variables */
  start = TZBSP_FLOOR_4K((uint32)elf->cert_ptr);
  end = TZBSP_ROUNDUP(uint32,((uint32)elf->cert_ptr + \
          (uint32)elf->cert_sz),TZBSP_PAGE_4K) ;

  tzbsp_dcache_inval_region((void *) start, (end - start));

  /*The pointer to elf and program headers and hash segment
   *is from non-secure DDR for all proc types except secure application.
   *Hence, lock the region for all proc types. Make sure the pointer
   *is from secure region for secure applications.
   */
  if (proc != TZ_PIL_AUTH_SEC_APP)
  {
    if(tzbsp_mpu_lock_dyn_area(TZBSP_XPU_DYN_AREA_CERT, start, end) < 0)
    {
      TZBSP_ERROR_CODE(PIL_VERIFY_SIG_MPU_LOCK_FAILED);
      return -E_FAILURE;
    }
  }
  else
  {
    if(!tzbsp_is_s_range((void *) start, (end - start)))
    {
      TZBSP_ERROR_CODE(PIL_INIT_SEC_APP_ELF_HDR_NOT_SECURE);
      return -E_FAILURE;
    }
  }

  if (elf->sig_sz > TZBSP_PAGE_4K)
  {
    TZBSP_ERROR_CODE(PIL_VERIFY_SIG_INVALID_SIG_SZ);
    if (proc != TZ_PIL_AUTH_SEC_APP)
    {
      tzbsp_mpu_unlock_dyn_area(TZBSP_XPU_DYN_AREA_CERT);
    }
    return -E_FAILURE;
  }

  if ((tmp = tzbsp_malloc(elf->sig_sz)) == NULL)
  {
    TZBSP_ERROR_CODE(PIL_VERIFY_SIG_MALLOC_FAILED);
    if (proc != TZ_PIL_AUTH_SEC_APP)
    {
      tzbsp_mpu_unlock_dyn_area(TZBSP_XPU_DYN_AREA_CERT);
    }
    return -E_FAILURE;
  }

  memcpy(tmp, elf->sig_ptr, elf->sig_sz);

  image_info.header_ptr_1    = elf->hash_seg_hdr;
  image_info.header_len_1    = elf->hash_seg_hdr_sz;
  image_info.code_ptr_1      = elf->hash_seg;
  image_info.code_len_1      = elf->hash_seg_sz;
  image_info.x509_chain_ptr  = elf->cert_ptr;
  image_info.x509_chain_len  = elf->cert_sz;
  image_info.signature_ptr   = tmp;
  image_info.signature_len   = elf->sig_sz;
  image_info.sw_type         = sw_type;

  tzbsp_rollback_get_version(tzbsp_convert_sw_type_to_rb_data(sw_type),
                             &image_info.sw_version);

  ret = tzbsp_secboot_authenticate_image(&image_info, &verified_info);

  if (proc != TZ_PIL_AUTH_SEC_APP)
  {
    tzbsp_mpu_unlock_dyn_area(TZBSP_XPU_DYN_AREA_CERT);
  }
  tzbsp_free(tmp);

  if(ret != E_SUCCESS)
  {
    TZBSP_ERROR_CODE(PIL_VERIFY_SIG_INVALID_SIGNATURE);
    return -E_FAILURE;
  }

  if (tzbsp_auth_elf_headers(elf) < 0)
  {
    TZBSP_ERROR_CODE(PIL_VERIFY_SIG_INVALID_ELF_HEADERS);
    return -E_FAILURE;
  }

  sw_id = verified_info.sw_id;
  cert_sw_version = (uint32)((sw_id >> 32) & 0xFFFFFFFF);  /* Bits 32-63 */
  if (cert_sw_version > image_info.sw_version )
  {
    ret = tzbsp_rollback_set_version(tzbsp_convert_sw_type_to_rb_data(sw_type),
                                     cert_sw_version);
    if (ret !=E_SUCCESS && ret != -E_OUT_OF_RANGE)
    {
      TZBSP_ERROR_CODE(PIL_ROLLBACK_VERIFY_VERSION);
      return -E_FAILURE;
    }
  }

  return E_SUCCESS;
}
