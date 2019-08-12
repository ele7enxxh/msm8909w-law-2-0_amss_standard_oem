/**
* @file tzbsp_pil_auth.c
* @brief TZBSP PIL Auth Service implementation
*
* This file implements the subsystem authentication for PIL inside Trustzone.
* This service is used within Trustzone to securely authenticate the ELF image
* for the respective subsystem. At the time of Secure PIL loading, the HLOS is
* already up and running to for better user experience. When Secure PIL is
* called, TZ XPU protects the relevant memory, authenticates the ELF image, and
* then brings the respective subsystem out of reset.
*/

/*===========================================================================
   Copyright (c) 2010 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                            EDIT HISTORY FOR FILE

  $Header: //components/rel/tz.bf/2.5.c10/trustzone_images/core/securemsm/trustzone/qsee/services/img_auth/src/tzbsp_pil_auth.c#1 $
  $DateTime: 2016/06/17 14:31:11 $
  $Author: pwbldsvc $


when       who      what, where, why
--------   ---      ------------------------------------
11/11/14   re        Fixing the elf addr adjustment before decryption.
03/14/14   hw       update APP_ID search function
11/19/2012   yh        Parse OU APP_ID & OEM_ID
03/2012    Amen      PIL VIDEO
05/2010    Nagarwal  TZBSP PIL AUTH Service Implementation
===========================================================================*/

/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/
#include <string.h>
#include <limits.h>
#include <HALhwio.h>

#include "tzbsp.h"
#include "tzbsp_chipset.h"
#include "tzbsp_config.h"
#include "tzbsp_errno.h"
#include "tzbsp_log.h"
#include "tzbsp_mem.h"
#include "tzbsp_pil.h"
#include "tzbsp_xpu.h"
#include "tzbsp_sys.h"
#include "tzbsp_exec.h"
#include "tzbsp_syscall.h"
#include "miheader.h"
#include "tz_syscall_pub.h"
#include "crypto/tzbsp_hash.h"
#include "secboot_hw.h"
#include "tzbsp_xpu_config.h"
#include "tz_ssd_s.h"
#include "tzbsp_mmu_cache.h"
#include "tzbsp_target.h"
#include "secboot_util.h"
#include "tzbsp_hwio.h"
#include "tzbsp_macc.h"

/*----------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * -------------------------------------------------------------------------*/
#define TZBSP_PIL_STATE_INIT      1
#define TZBSP_PIL_STATE_RESET     2
#define TZBSP_PIL_STATE_DONE      3

#define TZBSP_PIL_LOCK()    tzbsp_mutex_lock(&s_pil_info[proc].lock)
#define TZBSP_PIL_UNLOCK()  tzbsp_mutex_unlock(&s_pil_info[proc].lock)

#define TZBSP_ROUND_OFF           3

/* Parse x509 subject DN */
#define BLOAD8(x) ((uint32)(*((uint8*)(x))))
#define BSTOR8(x,y) (*((uint8*)(x))) = (uint8)(y)

uint8 global_tz_app_id[TZBSP_APP_ID_LEN];

/*----------------------------------------------------------------------------
 * Type Declarations
 * -------------------------------------------------------------------------*/
typedef struct tzbsp_pil_priv_s
{
  struct tzbsp_elf_info_s elf;
  uint32 proc;
  tzbsp_mutex_t lock;
  uint32 state;
  ssd_context_type *ssd_ctx;
} tzbsp_pil_priv_t;

/*----------------------------------------------------------------------------
 * Static Variable Definitions
 * -------------------------------------------------------------------------*/
static tzbsp_pil_priv_t s_pil_info[TZ_PIL_AUTH_UNSUPPORTED_PROC];

/*----------------------------------------------------------------------------
 * Static Function Declarations and Definitions
 * -------------------------------------------------------------------------*/
static boolean tzbsp_pil_is_elf(const Elf32_Ehdr * elf_hdr);
static int tzbsp_pil_populate_elf_info(tz_pil_proc_e_type proc,
                                       const Elf32_Ehdr *, tzbsp_elf_info_t *);
static boolean tzbsp_pil_is_mi_boot_valid(tz_pil_proc_e_type proc,
                                          const mi_boot_image_header_type *mi,
                                          boolean zs_failure);
extern uint32 tzbsp_secboot_hw_is_auth_enabled(uint32);
int tzbsp_subsys_is_valid_area(tz_pil_proc_e_type proc, uint32 start,
                               uint32 end);
int tzbsp_subsys_image_init_ok(tz_pil_proc_e_type proc,
                               const tzbsp_elf_info_t* elf);
static int tzbsp_pil_adjust_elf_addrs(tz_pil_proc_e_type proc);
extern void tzbsp_enable_ce1_clocks(boolean enable);
extern void tzbsp_dsb(void);
extern void busywait(uint32);

/*----------------------------------------------------------------------------
 * Function Declarations and Documentation
 * -------------------------------------------------------------------------*/
static inline void tzbsp_clean_pil_priv(tzbsp_pil_priv_t *pil)
{
  if (pil->elf.elf_hdr != NULL)
    tzbsp_free(pil->elf.elf_hdr);

  memset(pil, 0, sizeof(tzbsp_pil_priv_t));
  pil->state = TZBSP_PIL_STATE_INIT;
}

#ifdef TZBSP_WITH_SSD
static int tzbsp_pil_decrypt_elf_segments
(
  tzbsp_elf_info_t*  elf,
  ssd_context_type** ssd_ctx_ptr_ptr
)
{
  int ii, enc_seg_num = 0, enc_seg_idx = 0;

  /* Image was not found to be an SSD encrypted image during PIL init
     image */
  if (NULL == *ssd_ctx_ptr_ptr)
  {
    return E_SUCCESS;
  }

  /* find the total number of encrypted segments */
  for (ii = 0; ii < elf->prog_hdr_num; ii++)
  {
    /* Only loadable segments that are not the hash segment are
       encrypted. */
    if ( ELF_PT_LOAD != elf->prog_hdr[ii].p_type ||
         MI_PBT_SEGMENT_TYPE_VALUE(elf->prog_hdr[ii].p_flags) ==
         MI_PBT_HASH_SEGMENT )
    {
      continue;
    }

    enc_seg_num++;
  }

  /* Decrypt each encrypted ELF segment */
  for (ii = 0; ii < elf->prog_hdr_num; ii++)
  {
    /* Only loadable segments that are not the hash segment are
       encrypted. */
    if ( ELF_PT_LOAD != elf->prog_hdr[ii].p_type ||
         MI_PBT_SEGMENT_TYPE_VALUE(elf->prog_hdr[ii].p_flags) ==
         MI_PBT_HASH_SEGMENT )
    {
      continue;
    }

    /* Skip decryption for zero length segments, but count the segment
       as encrypted. This matches the .elf encryption tool's segment
       counter. */
    if (0 != elf->prog_hdr[ii].p_filesz)
    {
      if ( E_SUCCESS !=
           tzbsp_ssd_decrypt_elf_seg_s(ssd_ctx_ptr_ptr,
                                       enc_seg_idx,
                                       enc_seg_idx == (enc_seg_num - 1),
                                       elf->prog_hdr[ii].p_filesz,
                                       (uint32*) elf->prog_hdr[ii].p_paddr))
      {
        TZBSP_ERROR_CODE(PIL_DECRYPT_ELF_SEG_S_FAILED);
        return -E_FAILURE;
      }
    }

    enc_seg_idx++;
  }

  return E_SUCCESS;
}

/**
   Checks if there is a SSD MD region added to the hash segment.
   Returns the size of SSD Metadata.

   @param[in] pil Elf header info struct
*/
static uint32 tzbsp_pil_get_ssd_md_len
(
  tzbsp_elf_info_t* pil
)
{
  uint32      prog_tbl_sz = 0;
  uint32      exp_hash_seg_sz = 0;
  uint32      xml_md_size = 0;
  Elf32_Phdr* hash_prog_hdr = NULL;

  prog_tbl_sz = (uint32)(sizeof(Elf32_Phdr)) * (pil->elf_hdr->e_phnum);

  hash_prog_hdr = tzbsp_elf_find_seg(pil->prog_hdr,
                                     (Elf32_Phdr*)((uint8*) pil->prog_hdr +
                                                   prog_tbl_sz),
                                     MI_PBT_HASH_SEGMENT);

  /* This should be covered by an earlier check */
  if( NULL == hash_prog_hdr )
  {
    return 0;
  }

  /* the expected hash segment size */
  exp_hash_seg_sz = (pil->hash_seg_hdr_sz + pil->hash_seg_sz +
                     pil->sig_sz + pil->cert_sz);

  /* the difference in expected & actual size is the offset where
     SSD MD starts (if present) */
  if (hash_prog_hdr->p_filesz > exp_hash_seg_sz)
  {
    xml_md_size = hash_prog_hdr->p_filesz - exp_hash_seg_sz;
  }


  return xml_md_size;
} /* tzbsp_pil_get_ssd_md_len */

/**
   Parse SSD XML if present

   @param[in]  elf       The PIL structure
   @param[in]  elf_hdr   Pointer to the elf header
   @param[out] ssd_ctx   Pointer to the SSD MD structure to fill

   @return 0 on success, negative on failure

   @pre All pointers must be non-NULL and must point to valid-length
        structs
*/
static int tzbsp_pil_parse_ssd_md
(
  tzbsp_elf_info_t*   elf,
  Elf32_Ehdr*         elf_hdr,
  ssd_context_type**  ssd_ctx_ptr_ptr
)
{
  uint32* ssd_md_ptr = NULL;
  uint32  ssd_md_len = 0;

  /* Check the SSD metadata length */
  ssd_md_len = tzbsp_pil_get_ssd_md_len(elf);

  /* If the SSD metadata length is zero, the image is not encrypted */
  if (0 == ssd_md_len)
  {
    return E_SUCCESS;
  }

  /* The beginning of the metadata is in the hash segment after the
     elf headers, program headers and signature. This information
     wasn't copied into the secure elf struct, so we must grab the
     pointer to non-secure memory. */
  ssd_md_ptr = (uint32*)((uint32) elf_hdr +
                         sizeof(Elf32_Ehdr) +
                         elf->elf_hdr->e_phnum * sizeof(Elf32_Phdr) +
                         elf->hash_seg_hdr_sz +
                         elf->hash_seg_sz +
                         elf->sig_sz +
                         elf->cert_sz);

  if ( E_SUCCESS != tzbsp_ssd_parse_md_s(ssd_md_ptr, ssd_md_len,
                                         ssd_ctx_ptr_ptr) )
  {
    TZBSP_ERROR_CODE(PIL_PARSE_MD_S_FAILED);
    return -E_FAILURE;
  }

  return E_SUCCESS;
} /* tzbsp_pil_parse_ssd_md */
#endif /* TZBSP_WITH_SSD */

/**
 * @brief Confirms the memory usage of the ELF, as provided by the HLOS.
 *
 * @param proc      The peripheral type
 * @param mem       Pointer to reponse tzbsp_pil_mem_resp_t
 * @param mem_len   Length of response
 *
 * @return 0 on success, negative on failure.
 */
int tzbsp_pil_mem_area(tz_pil_proc_e_type proc,
                       uint32 start_addr,
                       uint32 image_len)
{
  if (!tzbsp_subsys_is_supported(proc) || TZ_PIL_AUTH_SEC_APP == proc)
  {
    TZBSP_ERROR_CODE(PIL_MEM_AREA_UNSUPPORTED_PROC);
    return -E_INVALID_ARG;
  }

  if (image_len == 0)
  {
    TZBSP_ERROR_CODE(PIL_MEM_AREA_ZERO_IMAGE_LEN);
    return -E_INVALID_ARG;
  }

  /* Address range can't be modified once subsystem has been verified */
  if(s_pil_info[proc].state == TZBSP_PIL_STATE_DONE)
  {
    TZBSP_ERROR_CODE(PIL_MEM_AREA_PROC_BAD_STATE);
    return -E_NOT_ALLOWED;
  }

  /* Argument and state validation complete, attempt to set the memory */
  return tzbsp_subsys_set_memory_range(proc, start_addr,
                                       start_addr + image_len);
}

/**
 * @brief Zero'ize and unlock MPU protected ELF image
 *
 * @param[in] proc      The peripheral type
 *
 * @return 0 on success, negative on failure.
 */
int tzbsp_pil_unlock_area(tz_pil_proc_e_type proc)
{
  if (!tzbsp_subsys_is_supported(proc))
  {
    TZBSP_ERROR_CODE(PIL_UNLOCK_AREA_USUPPORTED_PROC);
    return -E_INVALID_ARG;
  }
  /* Secure applications do not need to be unloaded using this API */
  else if (proc == TZ_PIL_AUTH_SEC_APP)
  {
    TZBSP_ERROR_CODE(PIL_UNLOCK_AREA_USUPPORTED_PROC);
    return -E_INVALID_ARG;
  }


  /* Nothing ever loaded */
  if (s_pil_info[proc].elf.prog_hdr == NULL)
    return E_SUCCESS;

  /* Put the subsystem in reset */
  if(tzbsp_subsys_tear_down(proc, TZBSP_TEAR_DOWN_FULL) < 0)
  {
    TZBSP_ERROR_CODE(PIL_UNLOCK_AREA_TEAR_DOWN_FAILED);
    return -E_FAILURE;
  }

  if (s_pil_info[proc].state == TZBSP_PIL_STATE_DONE)
  {
    if(tzbsp_allow_unlock_xpu())
    {
      if(tzbsp_pil_unlock_xpu(proc) != E_SUCCESS)
      {
        TZBSP_ERROR_CODE(PIL_UNLOCK_AREA_UNLOCK_XPU_FAILED);
        return -E_FAILURE;
      }
    }
    else
    {
      if(tzbsp_pil_unlock_xpu_zero(proc) != E_SUCCESS)
      {
        TZBSP_ERROR_CODE(PIL_UNLOCK_AREA_UNLOCK_XPU_ZERO_FAILED);
        return -E_FAILURE;
      }
    }
  }
  /* No need to clear out all memory, since the proc never booted */
  else
  {
    if(tzbsp_pil_unlock_xpu(proc) != E_SUCCESS)
    {
      TZBSP_ERROR_CODE(PIL_UNLOCK_AREA_UNLOCK_XPU_FAILED2);
      return -E_FAILURE;
    }
  }

  tzbsp_clean_pil_priv(&s_pil_info[proc]);

  return E_SUCCESS;
}

/**
 * @brief Handle the init image syscall.
 *
 * @param[in] proc        The peripheral image
 * @param[in] elf_hdr     Pointer to the ELF header
 *
 * @return 0 on success, negative on failure.
 */
int tzbsp_pil_init_image(tz_pil_proc_e_type proc,
                         Elf32_Ehdr * elf_hdr)
{
  int ret = E_SUCCESS;

  tzbsp_pil_timing_start(TZ_PIL_INIT_ID, proc);
  if (elf_hdr == NULL)
  {
    TZBSP_ERROR_CODE(PIL_INIT_IMAGE_NULL_ELF_HDR);
    return -E_INVALID_ARG;
  }

  if (!tzbsp_subsys_is_supported(proc))
  {
    TZBSP_ERROR_CODE(PIL_INIT_IMAGE_UNSUPPORTED_PROC);
    return -E_INVALID_ARG;
  }

   //Best Effort - Try to unlock any locked area
  //This will tear down the subsystem.
  tzbsp_pil_unlock_area(proc);
  
  /* ELF header must be fully in non-secure memory. */
  if(!tzbsp_is_ns_range(elf_hdr, sizeof(Elf32_Ehdr)))
  {
    if (TZ_PIL_AUTH_SEC_APP != proc)
    {
      TZBSP_ERROR_CODE(PIL_INIT_IMAGE_ELF_HDR_NOT_NONSECURE);
      return -E_BAD_ADDRESS;
    }
  }

  tzbsp_dcache_inval_region((void *) elf_hdr, sizeof(Elf32_Ehdr));

  do
  {
    TZBSP_PIL_LOCK();
    tzbsp_clean_pil_priv(&s_pil_info[proc]);
    s_pil_info[proc].proc = proc;

    if (tzbsp_pil_is_elf(elf_hdr) == FALSE)
    {
      TZBSP_ERROR_CODE(PIL_INIT_IMAGE_IS_NOT_ELF);
      ret = -E_FAILURE;
      break;
    }

    /* Allocate memory and copy headers and segments necessary for reset */
    if (tzbsp_pil_populate_elf_info(proc, elf_hdr, &s_pil_info[proc].elf) != E_SUCCESS)
    {
      TZBSP_ERROR_CODE(PIL_INIT_IMAGE_POPULATE_ELF_INFO_FAILED);
      ret = -E_FAILURE;
      break;
    }
#if !(defined VIRTIO_9x35)
    if (tzbsp_pil_verify_sig(proc, &s_pil_info[proc].elf) < 0)
    {
      TZBSP_ERROR_CODE(PIL_INIT_IMAGE_VERIFY_SIG_FAILED);
      ret = -E_FAILURE;
      break;
    }

#endif
    if(tzbsp_subsys_image_init_ok(proc, &s_pil_info[proc].elf) < 0)
    {
      TZBSP_ERROR_CODE(PIL_INIT_IMAGE_SYBSYS_IMAGE_INIT_OK_FAILED);
      ret = -E_FAILURE;
      break;
    }

#ifdef TZBSP_WITH_SSD
    tzbsp_enable_ce1_clocks(TRUE);
    ret = tzbsp_pil_parse_ssd_md(&s_pil_info[proc].elf, elf_hdr,
                                 &s_pil_info[proc].ssd_ctx);
    tzbsp_enable_ce1_clocks(FALSE);
    TZBSP_ERROR_CODE_E(ret, PIL_INIT_IMAGE_SSD_PARSE_MD_FAILED);
#endif

  } while (0);

  if(ret == E_SUCCESS)
  {
    s_pil_info[proc].state = TZBSP_PIL_STATE_RESET;
  }
  else
  {
    tzbsp_clean_pil_priv(&s_pil_info[proc]);
  }

  TZBSP_PIL_UNLOCK();

  tzbsp_pil_timing_end(TZ_PIL_INIT_ID, proc);

  return ret;
}

/**
 * @brief Helper function to filter out non-secure requests to load a
 *        secure application
 *
 * @param[in] proc        The peripheral image
 * @param[in] elf_hdr     Pointer to the ELF header
 *
 * @return 0 on success, negative on failure.
 */

int tzbsp_pil_init_image_ns(tz_pil_proc_e_type proc,
                            Elf32_Ehdr * elf_hdr)
{
  if (proc == TZ_PIL_AUTH_SEC_APP)
  {
    TZBSP_ERROR_CODE(PIL_INIT_IMAGE_UNSUPPORTED_PROC);
    return -E_INVALID_ARG;
  }

  return tzbsp_pil_init_image(proc, elf_hdr);
}

/**
 * @brief Populate the PIL info given the provided ELF image.
 *
 * @param[in] elf_hdr Pointer to the elf header
 * @param[in] pil Elf header info struct
 * @postcondition This routine allocates memory within \c tzbsp_elf_info_t. So
 *                the caller must free this memory when it is no longer needed.
 *
 * @return 0 on success, negative on failure.
 */
static const uint8 app_id_string[]="APP_ID";
static int tzbsp_pil_populate_elf_info(tz_pil_proc_e_type proc,
                                       const Elf32_Ehdr * elf_hdr,
                                       tzbsp_elf_info_t *pil)
{
  Elf32_Phdr *prog_hdr = NULL;
  mi_boot_image_header_type   *mi_boot_hdr = NULL;
  uint8* tmp = 0;
  char *temp_cert = NULL;
  byte   *mask_ptr;
  uint64 app_id = 0;
  secboot_error_type status = E_SECBOOT_FAILURE;

  /* Force the compiler to use stack variables. We need to be careful and make
     sure that sizes are read locally so the HLOS cannot interrupt TZ and
     re-write sizes under us.
   */
  volatile uint32 elf_hdr_sz = 0;
  volatile uint32 prog_tbl_sz = 0;
  volatile uint32 hash_seg_hdr_sz = 0;
  volatile uint32 hash_seg_sz =0;
  volatile uint32 image_hdr_size = 0;
  volatile uint32 hash_seg_addr = 0;
  volatile uint32 hash_seg_hdr_addr = 0;
  volatile uint16 elf_hdr_e_phnum = 0;

  elf_hdr_sz = (uint32)(sizeof(Elf32_Ehdr));
   
  if (TZ_PIL_AUTH_SEC_APP != proc)
  {
    /* ELF header must be fully in non-secure memory. */
    if (!tzbsp_is_ns_range((uint8*)elf_hdr,
                       TZBSP_ROUNDUP(uint32, elf_hdr_sz,
                                     KRAIT_CACHE_GRANULE_SZ)))
    {
      TZBSP_ERROR_CODE(POPULATE_ELF_ELF_HDR_NOT_IN_NS_MEMORY);
      return -E_BAD_ADDRESS;
    }
  }
  // Store this value locally since we will be using it in later calculations
  // and do not want to risk it being changed in between
  // Additionally we could memcpy the entire ELF header but then we need to
  // pepper the code with free()s in just the right places
  elf_hdr_e_phnum  = elf_hdr->e_phnum;
  if (elf_hdr_e_phnum > MI_PBT_MAX_SEGMENTS)
  {
    TZBSP_ERROR_CODE(POPULATE_ELF_NUMBER_OF_SEGMENTS_INVALID);
    return -E_FAILURE;
  }

  prog_tbl_sz = (uint32)(sizeof(Elf32_Phdr)) * (elf_hdr_e_phnum);
  /* The ELF header should be immediately followed by program headers and then hash
   * segments always, as this is a strict PIL requirement.
   */
  prog_hdr = (Elf32_Phdr*)((uint8*)elf_hdr + sizeof(Elf32_Ehdr));

  if (TZ_PIL_AUTH_SEC_APP != proc)
  {
    /* Program header must be fully in non-secure memory. */
    if (!tzbsp_is_ns_range((uint8*)prog_hdr,
                         TZBSP_ROUNDUP(uint32, prog_tbl_sz,
                                       KRAIT_CACHE_GRANULE_SZ)))
    {
      TZBSP_ERROR_CODE(POPULATE_ELF_PROG_HDR_NOT_IN_NS_MEMORY);
      return -E_BAD_ADDRESS;
    }
  }
  memset((void *)global_tz_app_id, 0, TZBSP_APP_ID_LEN);

  // For now just assume that there is a valid Hash Segment present. We will
  // check for its existence later, once the entire ELF image header has been
  // memcpy'ed in
  hash_seg_hdr_addr = (uint32)((uint32)prog_hdr+prog_tbl_sz);
  hash_seg_hdr_sz = sizeof(mi_boot_image_header_type);
  
  if (TZ_PIL_AUTH_SEC_APP != proc)
  {
    /* Hash segment must be fully in non-secure memory. */
    if (!tzbsp_is_ns_range((uint8*)hash_seg_hdr_addr,
                         TZBSP_ROUNDUP(uint32, hash_seg_hdr_sz,
                                       KRAIT_CACHE_GRANULE_SZ)))
    {
      TZBSP_ERROR_CODE(POPULATE_ELF_HASH_SEG_NOT_IN_NS_MEMORY);
      return -E_BAD_ADDRESS;
    }
  }
  hash_seg_addr = (uint32)((uint32)hash_seg_hdr_addr + hash_seg_hdr_sz);
  /* The filesize includes the certificate chain. Do not include
     this in the memory copy */
  hash_seg_sz = elf_hdr_e_phnum * TZBSP_SHA256_HASH_SZ;

  if (TZ_PIL_AUTH_SEC_APP != proc)
  {
    if (!tzbsp_is_ns_range((uint8*)hash_seg_addr,
                         TZBSP_ROUNDUP(uint32, hash_seg_sz,
                                       KRAIT_CACHE_GRANULE_SZ)))
    {
      TZBSP_ERROR_CODE(POPULATE_ELF_HASH_SEG_NOT_IN_NS_MEMORY);
      return -E_BAD_ADDRESS;
    }
  }
  /* Total copy size is Elf header + array of program headers + hash
   * segement.
   */
  image_hdr_size = elf_hdr_sz + prog_tbl_sz + hash_seg_hdr_sz + hash_seg_sz;

  if (image_hdr_size > TZBSP_PAGE_4K)
  {
    TZBSP_ERROR_CODE(POPULATE_ELF_IMAGE_HDR_TOO_SMALL);
    return -E_FAILURE;
  }

  /* Allocate memory of the size of image header within TZ */
  if ((tmp  = (uint8 *)(tzbsp_malloc(image_hdr_size))) == NULL)
  {
    TZBSP_ERROR_CODE(POPULATE_ELF_MALLOC_FAILED);
    return -E_FAILURE;
  }

  pil->elf_hdr = (Elf32_Ehdr*)tmp;
  tzbsp_dcache_inval_region((void *) elf_hdr, elf_hdr_sz);
  memcpy(pil->elf_hdr, elf_hdr, elf_hdr_sz);
  
  // Check that the value we had stored locally and based all our calculations
  // on actually matches what we have now memcpy'ed in
  if (pil->elf_hdr->e_phnum != elf_hdr_e_phnum)
  {
    TZBSP_ERROR_CODE(POPULATE_ELF_NUMBER_OF_SEGMENTS_INVALID);
    return -E_FAILURE;
  }

  tmp += elf_hdr_sz;
  pil->prog_hdr = (Elf32_Phdr*)tmp;
  tzbsp_dcache_inval_region((void *) prog_hdr, prog_tbl_sz);
  memcpy(pil->prog_hdr, prog_hdr, prog_tbl_sz);
  
  /* Size of the hash table segment - Find the program header for
  * the segment of type MI_PBT_HASH_SEGMENT, and get the size
  * from that entry
  */

  if (tzbsp_elf_find_seg(pil->prog_hdr,
                         (Elf32_Phdr*)((uint8*)(pil->prog_hdr) + prog_tbl_sz),
                         MI_PBT_HASH_SEGMENT) == NULL)
  {
    TZBSP_ERROR_CODE(POPULATE_ELF_NO_PROG_HDR);
    return -E_FAILURE;
  }

  tmp += prog_tbl_sz;
  pil->hash_seg_hdr = tmp;
  tzbsp_dcache_inval_region((void *) hash_seg_hdr_addr, hash_seg_hdr_sz);
  memcpy(pil->hash_seg_hdr, (uint8*)hash_seg_hdr_addr, hash_seg_hdr_sz);
  pil->hash_seg_hdr_sz = hash_seg_hdr_sz;

  tmp += hash_seg_hdr_sz;
  pil->hash_seg = tmp;
  tzbsp_dcache_inval_region((void *) hash_seg_addr, hash_seg_sz);
  memcpy(pil->hash_seg, (uint8*)hash_seg_addr, hash_seg_sz);
  pil->hash_seg_sz = hash_seg_sz;

  pil->prog_hdr_num = pil->elf_hdr->e_phnum;

  mi_boot_hdr= (mi_boot_image_header_type*) pil->hash_seg_hdr;
#if (defined VIRTIO_9x35)
    return E_SUCCESS;
#endif

  if (!tzbsp_pil_is_mi_boot_valid(proc, mi_boot_hdr,
                                  tzbsp_secboot_hw_is_auth_enabled(1)))
  {
    /* Signature must exist when secboot is enabled */
    if (tzbsp_secboot_hw_is_auth_enabled(1))
    {
      TZBSP_ERROR_CODE(POPULATE_ELF_MI_BOOT_INVALID);
      return -E_FAILURE;
    }
    else
    {
      /* Fuse is not blown, signature not required */
      return E_SUCCESS;
    }
  }

  /* Signature comes after hash table */
  pil->sig_ptr  = (uint8*)((uint8*) hash_seg_addr + hash_seg_sz);
  pil->sig_sz   = mi_boot_hdr->signature_size;
  pil->cert_ptr = (uint8*)(pil->sig_ptr + pil->sig_sz);
  pil->cert_sz  = mi_boot_hdr->cert_chain_size;

  if (proc == TZ_PIL_AUTH_SEC_APP && TRUE == tzbsp_secboot_hw_is_auth_enabled(1))
  {
    /* RFC5280    Certificate  ::=  SEQUENCE  {
        tbsCertificate       TBSCertificate,
        signatureAlgorithm   AlgorithmIdentifier,
        signatureValue       BIT STRING  }

     TBSCertificate  ::=  SEQUENCE  {
        version         [0]  EXPLICIT Version DEFAULT v1,
        serialNumber         CertificateSerialNumber,
        signature            AlgorithmIdentifier,
        issuer               Name, ...
     length of certificate head + version + signature + issuer tag = 32, this is fix for all x509 certificate.
     serial number is a variant. below code read out seriralNumber value from No 10th byte of a x509 binary file.
     serialNumber length + 32 is the beginning of issuer.
     Move ptr to the beginning of issuer to find "APP_ID" string. */
    mask_ptr =  pil->cert_ptr + 9;
    temp_cert =  (char *)pil->cert_ptr;
    /* A normal serical number should be less than 16 bytes */
    if(*mask_ptr <= 16 && *mask_ptr >0)
    {
      temp_cert += (32+*mask_ptr);
    }
    else
    {
      temp_cert += 50;
    }

    /* search APP_ID from the image */
    status = secboot_search_OU_field_data(&app_id, app_id_string, (sizeof(app_id_string)-1), 
                                        pil->cert_ptr,pil->cert_sz);
    if ( E_SECBOOT_SUCCESS != status)
      {
       return status;
    }

    memcpy((void *)global_tz_app_id,(uint8 *)(&app_id),8);

    TZBSP_LOG(TZBSP_MSG_DEBUG,"global_tz_app_id 0x%x, 0x%x \n", *((uint32 *)global_tz_app_id), *((uint32 *)global_tz_app_id + 1));
  }

  return E_SUCCESS;
}

/**
 * @brief Authenticate image and reset peripheral syscall
 *
 * @param[in] proc The peripheral image
 *
 * @return 0 on success, negative on failure
 */
int tzbsp_pil_auth_reset(tz_pil_proc_e_type proc)
{
  int32 ret = E_SUCCESS;
  uint32 flags = 0;

  tzbsp_pil_timing_start(TZ_PIL_AUTH_RESET_ID, proc);

  if (!tzbsp_subsys_is_supported(proc))
  {
    TZBSP_ERROR_CODE(PIL_AUTH_RESET_UNSUPPORTED_PROC);
    return -E_INVALID_ARG;
  }
  TZBSP_PIL_LOCK();

  do
  {
    if (s_pil_info[proc].state != TZBSP_PIL_STATE_RESET)
    {
      TZBSP_ERROR_CODE(PIL_AUTH_RESET_PROC_NOT_IN_RESET);
      ret = -E_FAILURE;
      break;
    }
    if (E_SUCCESS != tzbsp_pil_adjust_elf_addrs(proc))
    {
      TZBSP_ERROR_CODE(PIL_AUTH_RESET_ELF_ADDR_ADJUST_FAILED);
      ret = -E_FAILURE;
      break;
    }

#ifdef TZBSP_WITH_SSD
    if (tzbsp_pil_decrypt_elf_segments(&(s_pil_info[proc].elf),
                                       &s_pil_info[proc].ssd_ctx) < 0)
    {
      TZBSP_ERROR_CODE(PIL_AUTH_RESET_DECRYPT_FAILED);
      return -E_FAILURE;
    }
#endif


#ifndef VIRTIO_8909
/* added becuase there is no crypto engine  on virtio */

    if (tzbsp_pil_auth_segments(proc, &s_pil_info[proc].elf) < 0)
    {
      TZBSP_ERROR_CODE(PIL_AUTH_RESET_AUTH_SEGMENTS_FAILED);
      ret = -E_FAILURE;
      break;
    }
#endif
    TZBSP_LOG(TZBSP_MSG_DEBUG,"UIE:[%d]Completed", proc);

    if (tzbsp_macc_calc_segs_hash(proc, &s_pil_info[proc].elf) < 0)
    {
      TZBSP_ERROR_CODE(AUTH_ELF_HEADERS_HASH_FAILED);
      ret = -E_FAILURE;
      break;
    }
    s_pil_info[proc].state = TZBSP_PIL_STATE_DONE;

    /* Call the processor reset routine */
    flags = tzbsp_int_disable_all();
    ret = tzbsp_subsys_bring_up((tz_pil_proc_e_type)proc,
                          (uint32)((int32)s_pil_info[proc].elf.elf_hdr->e_entry));
    tzbsp_int_restore(flags);
    TZBSP_ERROR_CODE_E(ret, PIL_AUTH_RESET_SUBSYS_BRING_UP_FAILED);

    if (E_SUCCESS != ret)
    {
      tzbsp_pil_unlock_xpu(proc);
    }

    /* Nobody needs the elf info anymore, so free it. */
    tzbsp_free(s_pil_info[proc].elf.elf_hdr);
    s_pil_info[proc].elf.elf_hdr = NULL;
  } while (0);

  TZBSP_PIL_UNLOCK();

  tzbsp_pil_timing_end(TZ_PIL_AUTH_RESET_ID, proc);
  return ret;
}

/**
 * @brief Helper function to return the next loadable program header
 *
 * @param[in]   elf     Pointer to the ELF info structure
 * @param[in]   phdr    Pointer to previously return program header
 *
 * @return Pointer to the next loadable program header, NULL if no more exist
 *
 * @precondition phdr initialized to NULL to start iterating from the beginning
 */
static Elf32_Phdr* tzbsp_next_loadable_segment(const struct tzbsp_elf_info_s* elf,
                                               Elf32_Phdr* iter)
{
  Elf32_Phdr* phdr = NULL;
  iter = (NULL == iter) ? elf->prog_hdr : iter + 1;

  for(; iter <= &elf->prog_hdr[elf->prog_hdr_num - 1]; iter++)
  {
    if(tzbsp_is_valid_segment(iter))
    {
      phdr = iter;
      break;
    }
  }

  return phdr;
}

/**
 * @brief Determines the DDR range of an image from the ELF headers;
 *        addresses will be rounded to the nearest page (4kB) boundaries
 *
 * @param[in]   elf     Pointer to the ELF info structure
 * @param[out]  start   Pointer to memory to store the start address (inclusive)
 * @param[out]  end     Pointer to memory to store the end address (exlcusive)
 * @param[in]   proc    The peripheral image
 */
static void tzbsp_pil_get_ddr_mem_range(const struct tzbsp_elf_info_s* elf,
                                         uint32 *start, uint32 *end, tz_pil_proc_e_type proc)
{
  Elf32_Phdr* phdr = NULL;
  uint32 seg_start, seg_end;
  uint32 low_addr = UINT_MAX;
  uint32 high_addr = 0;

  /* Iterate through the program headers in order to find the bounds on the image
   * memory.  Only loadable segments within DDR are considered */
  while(NULL != (phdr = tzbsp_next_loadable_segment(elf, phdr)))
  {
    seg_start = phdr->p_paddr;
    seg_end = phdr->p_paddr + phdr->p_memsz;

    /* TODO: tzbsp_ddr_contains_area() logic should improved to account for relocateable
       images as well. Actual VPU PIL elf header initially failed at tzbsp_ddr_contains_area()
       checking because physical address in segment header was > 2GB. */
  //  if(proc == TZ_PIL_AUTH_SEC_APP || tzbsp_ddr_contains_area((const void*)seg_start, phdr->p_memsz))
  //  {
      low_addr  = (seg_start < low_addr) ? seg_start : low_addr;
      high_addr = (seg_end > high_addr) ? seg_end : high_addr;
  //  }
  }

  /* Ensure 4kB aligned for page granularity */
  *start = TZBSP_ROUNDDOWN(uint32, low_addr, TZBSP_PAGE_4K);
  *end = TZBSP_ROUNDUP(uint32, high_addr, TZBSP_PAGE_4K);
}


/**
 * @brief If the image was relocated by the HLOS, the addresses of the segments
 *        need to be updated to reflect the new location of the segments
 *
 * @param[in]   proc    The peripheral image
 *
 * @return \c E_SUCCESS on success, negative error otherwise
 */
static int tzbsp_pil_adjust_elf_addrs(tz_pil_proc_e_type proc)
{
  Elf32_Phdr* phdr = NULL;
  uint32 start, end, new_start, new_end;
  int64 diff;
  int err = E_SUCCESS;

  /* Get the image address range, according to the ELF provided */
  tzbsp_pil_get_ddr_mem_range(&s_pil_info[proc].elf, &start, &end, proc);

  /* If the address range for this subsystem hasn't been updated, then
   * no adjustment is required */
  if(tzbsp_subsys_is_memory_range_set(proc))
  {
    do
    {
      /* Address range may not match the ELF; update the paddr fields */
      err = tzbsp_subsys_get_memory_range(proc, &new_start, &new_end);
      if(err)
      {
        TZBSP_ERROR_CODE(PIL_ADJUST_ELF_GET_MEM_RANGE_FAILED);
        break;
      }

      /* Ensure the range read from the ELF fits within the new range */
      if((end - start) > (new_end - new_start))
      {
        err = -E_OUT_OF_RANGE;
        TZBSP_ERROR_CODE(PIL_ADJUST_ELF_SIZE_MISMATCH);
        break;
      }

      /* If this new start addresses matches the one from the ELF, then
       * nothing to be done */
      if(new_start == start)
      {
        break;
      }

      /* Adjustment required; work with a signed 64-bit integer to supported both
       * 32- and 64-bit data types in the ELF.  Cast the start and end as unsigned
       * to prevent sign extension */
      diff = (int64)((uint64)new_start - (uint64)start);

      /* Entry point and each loadable segment must be adjusted */
      s_pil_info[proc].elf.elf_hdr->e_entry += diff;
      while(NULL != (phdr = tzbsp_next_loadable_segment(&s_pil_info[proc].elf, phdr)))
      {
        phdr->p_paddr += diff;
      }
    } while(0);
  }
  else
  {
    /* No adjust necessary, update the address range to match the ELF */
    err = tzbsp_subsys_set_memory_range(proc, start, end);
  }

  return err;
}

/**
 * @brief Helper function to filter out non-secure syscalls with the
 *        secure application proc type
 *
 * @param[in] proc The peripheral image
 *
 * @return 0 on success, negative on failure
 */
int tzbsp_pil_auth_reset_ns(tz_pil_proc_e_type proc)
{
  if (proc == TZ_PIL_AUTH_SEC_APP)
  {
    TZBSP_ERROR_CODE(PIL_AUTH_RESET_UNSUPPORTED_PROC);
    return -E_INVALID_ARG;
  }

  return tzbsp_pil_auth_reset(proc);
}


/**
 * @brief Find the segment of the given p_flags
 *
 * @param[in] phdr The program header entry start ptr
 * @param[in] end The program header entry end ptr
 * @param[in] seg_flags The p_flags of a given segment
 *
 * @return Valid program header pointer if found, \c NULL otherwise.
 */
Elf32_Phdr *tzbsp_elf_find_seg(const Elf32_Phdr * phdr, const Elf32_Phdr * end,
                               uint32 seg_flags)
{
  Elf32_Phdr *cur= (Elf32_Phdr *) phdr;

  if (phdr == NULL || end == NULL)
    return NULL;

  while ((uint32*)cur < (uint32*)end)
  {
    if (MI_PBT_SEGMENT_TYPE_VALUE(cur->p_flags) == seg_flags)
    {
      return cur;
    }
    cur++;
  }
  return NULL;
}

/**
 * @brief Elf header parser, to check for the correct format.
 *
 * @param[in] elf_hdr   Pointer to the ELF header.
 *
 * @result \c TRUE if the header is in correct format, \c FALSE otherwise.
 */
static boolean tzbsp_pil_is_elf(const Elf32_Ehdr * elf_hdr)
{
  if (elf_hdr->e_ident[ELFINFO_MAG0_INDEX] != ELFINFO_MAG0
      || elf_hdr->e_ident[ELFINFO_MAG1_INDEX] != ELFINFO_MAG1
      || elf_hdr->e_ident[ELFINFO_MAG2_INDEX] != ELFINFO_MAG2
      || elf_hdr->e_ident[ELFINFO_MAG3_INDEX] != ELFINFO_MAG3)
  {
    TZBSP_ERROR_CODE(PIL_IS_ELF_MAGIC_FAILED);
    return FALSE;
  }

  if (elf_hdr->e_ident[ELFINFO_CLASS_INDEX] != ELF_CLASS_32)
  {
    TZBSP_ERROR_CODE(PIL_IS_ELF_INVALID_CLASS);
    return FALSE;
  }

  if (elf_hdr->e_ident[ELFINFO_VERSION_INDEX] != ELF_VERSION_CURRENT)
  {
    TZBSP_ERROR_CODE(PIL_IS_ELF_INVALID_VERSION);
    return FALSE;
  }

  if (elf_hdr->e_ehsize != sizeof(Elf32_Ehdr))
  {
    TZBSP_ERROR_CODE(PIL_IS_ELF_INVALID_EHSIZE);
    return FALSE;
  }

  if ((elf_hdr->e_phentsize) != (sizeof(Elf32_Phdr)))
  {
    TZBSP_ERROR_CODE(PIL_IS_ELF_INVALID_PHENTSIZE);
    return FALSE;
  }

  return TRUE;
}

/**
 * @brief Verify if the boot header has the correct lengths
 *
 * @note Also verify that the proc is of the supported type
 *
 * @param[in] proc Peripheral image type
 * @param[in] mi Pointer to the header of the hash segment
 * @param[in] zs_failure Whether zero-length elements in the header
 *                       should indicate a fatal error
 *
 * @return \c TRUE if the boot header lengths are correct, otherwise \c FALSE.
 */
static boolean tzbsp_pil_is_mi_boot_valid(tz_pil_proc_e_type proc,
                                          const mi_boot_image_header_type *mi,
                                          boolean zs_failure)
{
  uint32 total_len = 0;

  if (mi == NULL)
  {
    TZBSP_ERROR_CODE(MI_BOOT_VALID_MI_NULL);
    return FALSE;
  }

  if (mi->code_size == 0 || mi->cert_chain_size == 0 ||
      mi->signature_size == 0)
  {
    if (zs_failure)
    {
      TZBSP_ERROR_CODE(MI_BOOT_VALID_ZERO_SIZE);
    }

    return FALSE;
  }

   /* Integer Overflow checking */
   total_len = mi->code_size;

   total_len += mi->cert_chain_size;
	  
   /* Integer overflow check */
   if(total_len < mi->cert_chain_size)
   {
     TZBSP_ERROR_CODE(MI_BOOT_INVALID_SIZE);
	 return FALSE;
   }
   total_len += mi->signature_size;

   /* Integer overflow check */
   if(total_len < mi->signature_size)
   {
     TZBSP_ERROR_CODE(MI_BOOT_INVALID_SIZE);
     return FALSE;
   }

   total_len += TZBSP_ROUND_OFF;

   /* Integer overflow check 
	* Round off verification */
	
   if(total_len < TZBSP_ROUND_OFF)
   {
     TZBSP_ERROR_CODE(MI_BOOT_INVALID_SIZE);
	 return FALSE;
   }
      
  /* Verify the image_size is equal to total of code_size, cert_chain_size and
   * signature_size. We may have added 1 to 3 bytes to the image if the total
   * image size is not multiple of four. We should account for that also.
   */
  total_len = TZBSP_ROUNDUP(uint32,
              mi->code_size + mi->cert_chain_size + mi->signature_size,
              4);

  if (total_len != mi->image_size)
  {
    TZBSP_ERROR_CODE(MI_BOOT_VALID_LENGTH_MISMATCH);
    return FALSE;
  }

  return TRUE;
}

/**
 * @brief Check for loadable, non paged segments
 *
 * @param[in] Program hdr entry for the segment
 *
 * @result \c TRUE if the segment is valid, \c FALSE otherwise.
 */
boolean tzbsp_is_valid_segment(const Elf32_Phdr* entry)
{
  /* Valid hashed segments are loadable */
  if(entry->p_type == ELF_PT_LOAD)
  {
    if ((MI_PBT_PAGE_MODE_VALUE(entry->p_flags) ==
         MI_PBT_NON_PAGED_SEGMENT) &&
        (MI_PBT_SEGMENT_TYPE_VALUE(entry->p_flags) !=
          MI_PBT_HASH_SEGMENT) &&
        (MI_PBT_ACCESS_TYPE_VALUE(entry->p_flags) !=
          MI_PBT_NOTUSED_SEGMENT) &&
        (MI_PBT_ACCESS_TYPE_VALUE(entry->p_flags) !=
          MI_PBT_SHARED_SEGMENT))
    {
      return TRUE;
    }
  }

  return FALSE;
}

/**
 * @brief Initilize all static/global variables used for PIL
 *
 * @return 0 on success, negative on failure
 */
int tzbsp_pil_init(void)
{
  uint32 i = 0;

  TZBSP_LOG_DBG("Initializing PIL\n");
  memset(s_pil_info, 0, sizeof(struct tzbsp_pil_priv_s) *
         TZ_PIL_AUTH_UNSUPPORTED_PROC);

  for (; i < TZ_PIL_AUTH_UNSUPPORTED_PROC; ++i)
    tzbsp_mutex_init(&s_pil_info[i].lock);

  return E_SUCCESS;
}

/**
 * Queries whether PIL supports a specific subsystem.
 *
 * @param [in] proc       Queried subsystem ID.
 * @param [out] verdict   Memory location to store the query verdict. In
 *                        non-secure memory.
 * @param [in] verdict_sz The size of response (size of verdict).
 *
 * @return \c E_SUCCESS if the request was successful, error code otherwise.
 */
int tzbsp_pil_is_subsystem_supported(uint32 proc, uint32* verdict,
                                     uint32 verdict_sz)
{
  if(verdict_sz < sizeof(uint32))
  {
    TZBSP_ERROR_CODE(IS_SUBSYSTEM_SUPPORTED_VERDICT_SIZE);
    return -E_INVALID_ARG;
  }

  /* Check that the verdict is in non-secure memory. */
  if(!tzbsp_is_ns_range(verdict, sizeof(*verdict)))
  {
    TZBSP_ERROR_CODE(IS_SUBSYSTEM_SUPPORTED_VERDICT_NOT_IN_NS_MEMORY);
    return -E_BAD_ADDRESS;
  }

  *verdict = tzbsp_subsys_is_supported(proc);

  return E_SUCCESS;
}

/**
 * Queries whether PIL requires the use of TZ PIL for a  specific subsystem.
 *
 * @param [in] proc       Queried subsystem ID.
 * @param [out] verdict   Memory location to store the query verdict. In
 *                        non-secure memory.
 * @param [in] verdict_sz The size of response (size of verdict).
 *
 * @return \c E_SUCCESS if the request was successful, error code otherwise.
 */
int tzbsp_pil_is_subsystem_mandated(uint32 proc, uint32* verdict,
                                    uint32 verdict_sz)
{
  if(verdict_sz < sizeof(uint32))
  {
    TZBSP_ERROR_CODE(IS_SUBSYSTEM_MANDATED_VERDICT_SIZE);
    return -E_INVALID_ARG;
  }

  /* Check that the verdict is in non-secure memory. */
  if(!tzbsp_is_ns_range(verdict, sizeof(*verdict)))
  {
    TZBSP_ERROR_CODE(IS_SUBSYSTEM_MANDATED_VERDICT_NOT_IN_NS_MEMORY);
    return -E_BAD_ADDRESS;
  }

  *verdict = tzbsp_subsys_is_mandated(proc);

  return E_SUCCESS;
}

/**
 * Returns the start and end address of the PIL memory area
 *
 * @param[in]   spare   Unused; should be zero
 * @param[out]  ns_buf  HLOS provided buffer for the start and end addresses
 *                      of the PIL region, in that order
 * @param[out]  size    Size of the HLOS provided buffer
 *
 * @return \c E_SUCCESS on success, negative error code otherwise.
 */
int tzbsp_pil_get_mem_area(uint32 spare, uint32 *ns_buf, uint32 size)
{
  typedef struct {
    uint32 start;
    uint32 end;
  } mem_range_t;

  mem_range_t *mem_range = (mem_range_t*)ns_buf;

  if(size < sizeof(mem_range_t))
  {
    return -E_INVALID_ARG;
  }

  if(!tzbsp_is_ns_range(mem_range, sizeof(mem_range_t)))
  {
    return -E_BAD_ADDRESS;
  }

  return tzbsp_subsys_get_reloc_pil_range(&mem_range->start, &mem_range->end);
}

/* Syscall to restart/stop modem  
 * call is specific to 8916 ***
 *
 * @param[in] config  register value
 * @param[in] spare   spare
 *
 * @return  Zero on success.  
 *          Negative error code otherwise.
 */

int tzbsp_pil_modem_reset(uint32 config, uint32 spare)
{
  /* 
   * Enable/disable modem to power up.
   */
  HWIO_GCC_MSS_RESTART_OUT(config);
  tzbsp_dsb();
  busywait(2);
  return E_SUCCESS;
}
