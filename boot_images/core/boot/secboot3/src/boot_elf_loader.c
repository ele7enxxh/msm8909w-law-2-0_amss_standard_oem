/*===========================================================================

                              ELF Image Loader

GENERAL DESCRIPTION
  This module performs generic ELF image loading. 

Copyright 2014 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================
                           EDIT HISTORY FOR FILE
  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

when       who        what, where, why
--------   ---        -------------------------------------------------------
12/23/14   jz         Validate loadable segment filesz/memsz after header is loaded, cleaned up
12/05/14   jz         Added clobber check and protection in boot_elf_load_seg_in_chunks
12/04/14   jz         Added integer overflow check in boot_elf_load_seg_in_chunks
11/17/14   sk         Added condition check for potential out-of-bond hash pointer
10/30/14   aus        Remove added local clobber holes after reading from flash
10/27/14   jz         Added boundary and integer overflow checks in boot_elf_reconstruct_elf_image
10/09/14   jz         Added boot_elf_reconstruct_elf_image, removed modified elf loading logic
08/26/14   jz         Fixed/updated the call to boot_rmb_set_pmi_code_start_addr 
07/31/14   jz         Added support for modified elf loading where the image is loaded as one blob
06/11/14   jz         Fixed boot_is_auth_enabled to check return status in the caller
04/11/14   ck         Added explicit clobber check using ELF mem size when loading a segment.
                      Added loaded ELF segment to clobber protection to prevent another image
                      from overwriting it.
03/05/14   ck         Passing QC header buffer to boot_elf_open_and_parse_elf_header 
                      and boot_elf_verify_image_header so it can be cleared before use
02/12/14   ck         Validate program header before loading the segment
12/10/13   ck         boot_is_auth_enabled no longer has a return value
12/09/13   jz         Added 64bit elf loading support
12/06/13   ck         Replaced calls to boot_pbl_is_auth_enabled with boot_is_auth_enabled
12/02/13   ck         Added boot_get_elf_eident_class function
10/18/13   dhaval     explicit hash-verify for ehdr+phdr after hash-table auth.
                      Changes to do segment hash cheks only if ehdr+phdr hash 
				      check was successful
08/24/13   lm         Fixed bootup failure issue when SDI as elf featue enabled and
				      SDI image not flashed case.
07/29/13   aus        Support for loading ELF SDI image
07/29/13   dh         Fix a bug in hash buffer size check
07/16/13   aus        Update the hash size check
05/31/13   kedara     Add hash segment buffer to local clobber table.
04/17/13   kedara     Support mpss authentication, Non-MSA boot flow.
03/28/13   dhaval     updates to support sha256 
02/19/13   kedara     Added boot_get_elf_header, boot_get_program_header.
01/30/13   kedara     Subsystem self authentication updates.
11/08/12   kedara     Updates to enable hashing and authentication of elf's.
09/28/12   kedara     Relocate boot_rmb_set_pmi_elf_load_ready. Check memsize
                      before validating segment dest addr.
09/21/12   jz         Removed redundant call to boot_elf_load_init_hash_segment
09/20/12   kedara     Load hash segment to fixed buffer in DDR ZI.
09/12/12   kedara     Fix bss segment zero out logic.
06/15/12   kedara     Added subsystem self authentication (ssa) for elf images.
06/11/12   kedara     include boot_flash_dev_if.h. Added crypto auth lib
                      wrapper api boot_secboot_hash*
08/24/11   aus        Fixed the hash verification for case when the segment 
                      ELF size is 0x0
07/06/11   aus        Fixed adding the clobber hole for image header
03/14/11   dxiang     Initial revision.
===========================================================================*/

/*===========================================================================
                     INCLUDE FILES FOR MODULE
===========================================================================*/
#include "boot_loader.h"
#include "boot_error_handler.h"
#include "boot_util.h"
#include "miprogressive.h"
#include "boot_flash_dev_if.h"
#include "boot_rmb.h"
#include "boot_hash_buffer.h"
#include "boot_hash.h"
#include "boot_elf_header.h"
#include "boot_authenticator.h"
#include "boot_sbl_qsee_interface.h"

/*===========================================================================
                    VARIABLE DECLARATIONS FOR MODULE
===========================================================================*/
/* Data Chunk size to be 1MB */
#define MAX_CHUNK_SIZE 0x100000

#define MAX_ADDRESS_32 0xFFFFFFFF
#define MAX_ADDRESS 0xFFFFFFFFFFFFFFFF


/* Secure context for hashing.
  Use zero_init attribute to force variables less than 8 bytes to
  be allocated in ZI otherwise compiler may optmize it into RW
*/
__attribute__((section("BOOT_DDR_ZI_DATA_ZONE"), zero_init)) static boot_hash_ctx_t elf_ctx;

#pragma arm section zidata = "BOOT_DDR_ZI_DATA_ZONE"
/* Variables to be placed in DDR so as to be accessible by drivers (eg Crypto)
    using BAM.
*/

/* Program Header array
   In consideration of limited memory, the maximum number of program header 
   entries may be configurable from the top level Scons file of any image 
   which may use the ELF loader module. */
static Elf32_Phdr phdr_array_32[MI_PBT_MAX_SEGMENTS];
static Elf64_Phdr phdr_array[MI_PBT_MAX_SEGMENTS];

/* ELF Header */
static Elf32_Ehdr ehdr_32;
static Elf64_Ehdr ehdr;

/* Output digest buffer for comparing hashes */
static uint8 verify_digest_buf[PBL_SHA_HASH_MAX_LEN];
extern hash_alg_t sbl_hash_algorithm;
extern uint32 sbl_hash_len;

/* ELF format (object class), default to 64bit */
static uint8 elf_format = ELF_CLASS_64;

static void *ehdr_ptr, *phdr_array_ptr;
static uint32 ehdr_size, phdr_size;
static uint16 phdr_num;

#pragma arm section zidata

/* Flash translation interface */
static boot_flash_trans_if_type * trans_if = NULL;

/* Start of the hash entries in memory */
static uint8 * hash_segment_start = NULL;


/* Subsystem Self authentication (SSA) enable check */
static boolean elf_ssa_enabled = FALSE;

/* Flag to indicate if config table entry enables authentication */
static boolean elf_cfg_table_auth = FALSE;

/* Subsystem Self auth meta info variables */
static uint8* hash_segment_base_addr = 0x0;
static uint32  hash_segment_length = 0x0;
static uint32  total_elf_segments_size = 0x0;

/* flag to track whether ehdr+phdr hashing was successful or not */
static boolean boot_elf_is_ehdr_phdr_hash_success = FALSE;

/* Temporary buffer to use for loading hash segment */
static uint8 *hash_segment_base_ptr = NULL;


/*===========================================================================
                        LOCAL FUNCTION DEFINITIONS
=============================================================================*/

/*===========================================================================
**  Function :  boot_elf_verify_header
** ==========================================================================
*/
/*!
* 
* @brief
*   This function verifies the ELF Header by sanity checking various fields
*   within the ELF Header structure 
*
* @par Dependencies
*   None 
* 
* @retval
*   TRUE - If ELF Header has passed verification
*   FALSE - Otherwise
* 
*/
static boolean boot_elf_verify_header()
{
  /* Sanity check the ELF header,
    * and ensure that there is at least 1 prog-header entry
    * and total number of prog-hdr segments are not more than the allocated prog-hdr buffer
    */
  if (elf_format == ELF_CLASS_32)
    return (  ehdr_32.e_ident[ELFINFO_MAG0_INDEX] == ELFINFO_MAG0 && 
            ehdr_32.e_ident[ELFINFO_MAG1_INDEX] == ELFINFO_MAG1 &&
            ehdr_32.e_ident[ELFINFO_MAG2_INDEX] == ELFINFO_MAG2 &&
            ehdr_32.e_ident[ELFINFO_MAG3_INDEX] == ELFINFO_MAG3 &&
            ehdr_32.e_ident[ELFINFO_CLASS_INDEX] == ELF_CLASS_32 &&
            ehdr_32.e_ident[ELFINFO_VERSION_INDEX] == ELF_VERSION_CURRENT &&
            ehdr_32.e_ehsize == sizeof(Elf32_Ehdr) &&
            ehdr_32.e_phentsize == sizeof(Elf32_Phdr) &&
            ehdr_32.e_phnum != 0 &&
            ehdr_32.e_phnum <= MI_PBT_MAX_SEGMENTS);
            
  else if (elf_format == ELF_CLASS_64)
  return (  ehdr.e_ident[ELFINFO_MAG0_INDEX] == ELFINFO_MAG0 && 
            ehdr.e_ident[ELFINFO_MAG1_INDEX] == ELFINFO_MAG1 &&
            ehdr.e_ident[ELFINFO_MAG2_INDEX] == ELFINFO_MAG2 &&
            ehdr.e_ident[ELFINFO_MAG3_INDEX] == ELFINFO_MAG3 &&
            ehdr.e_ident[ELFINFO_CLASS_INDEX] == ELF_CLASS_64 &&
            ehdr.e_ident[ELFINFO_VERSION_INDEX] == ELF_VERSION_CURRENT &&
            ehdr.e_ehsize == sizeof(Elf64_Ehdr) &&
            ehdr.e_phentsize == sizeof(Elf64_Phdr) &&
            ehdr.e_phnum != 0 &&
            ehdr.e_phnum <= MI_PBT_MAX_SEGMENTS);

  else
    return FALSE; // invalid

} 

/*===========================================================================
**  Function :  boot_elf_phdr_is_loadable
** ==========================================================================
*/
/*!
* 
* @brief
*   This function checks if a specific program header should be loaded
*   into memory or not
*
* @param[in] phdr - Pointer to the Program Header structure 
*
* @par Dependencies
*   None 
* 
* @retval
*   TRUE - If Program Header should be loaded
*   FALSE - Otherwise
* 
*/
static boolean boot_elf_phdr_is_loadable(void * phdr)
{
  uint32 phdr_type;
  uint32 phdr_flags;
  
  BL_VERIFY( phdr != NULL, BL_ERR_NULL_PTR_PASSED );

  phdr_type = (elf_format == ELF_CLASS_64) ? ((Elf64_Phdr *)phdr)->p_type : ((Elf32_Phdr *)phdr)->p_type;
  phdr_flags = (elf_format == ELF_CLASS_64) ? ((Elf64_Phdr *)phdr)->p_flags : ((Elf32_Phdr *)phdr)->p_flags;

  return (  phdr_type == MI_PBT_ELF_PT_LOAD  &&
            MI_PBT_PAGE_MODE_VALUE(phdr_flags) == MI_PBT_NON_PAGED_SEGMENT  &&
            MI_PBT_ACCESS_TYPE_VALUE(phdr_flags) != MI_PBT_NOTUSED_SEGMENT  &&
           /* hash segment to be loaded explicitly to fixed buffer */
            MI_PBT_SEGMENT_TYPE_VALUE(phdr_flags) != MI_PBT_HASH_SEGMENT   &&
            MI_PBT_ACCESS_TYPE_VALUE(phdr_flags) != MI_PBT_SHARED_SEGMENT  );
} 

/*===========================================================================
**  Function :  boot_elf_enable_hash_checking
** ==========================================================================
*/
/*!
* 
* @brief
*   This function determines whether or not ELF hash verification should be
*   skipped. Default behavior is to verify all data segments with their
*   respective hash entries. If secure boot is enabled, hash checking is 
*   forced active.
*
* @par Dependencies
*   Define FEATURE_BOOT_SKIP_ELF_HASH_VERIFICATION to skip verification
* 
* @retval
*   TRUE - If ELF hash verification is enabled
*   FALSE - Otherwise
* 
*/
static boolean boot_elf_enable_hash_checking()
{
  boot_boolean is_auth_enabled = FALSE;

  bl_error_type status = boot_is_auth_enabled(&is_auth_enabled);
  BL_VERIFY((status == BL_ERR_NONE), BL_ERR_IMG_SECURITY_FAIL); 

  if (is_auth_enabled && elf_cfg_table_auth)
  {
    return TRUE;
  }
  else
  {
#ifdef FEATURE_BOOT_SKIP_ELF_HASH_VERIFICATION
    return FALSE;
#else
    return TRUE;
#endif
  }
}

/*===========================================================================
**  Function :  boot_elf_load_seg_generic
** ==========================================================================
*/
/*!
* 
* @brief
*   This function loads the data segment into memory using the information
*   from a specific program header and input destination address.  
*
* @param[in] phdr - Pointer to the Program Header structure 
*            dest_addr - pointer to memory region where seg needs to be
*                        loaded
*
* @par Dependencies
*   None 
* 
* @retval
*   None
* 
*/
static void boot_elf_load_seg_generic(void * phdr, uint8 *dest_addr)
{
  boolean success = FALSE;
  uint64 phdr_offset, phdr_filesz, phdr_memsz;

  BL_VERIFY( (phdr != NULL), BL_ERR_NULL_PTR_PASSED);
   
  phdr_offset = (elf_format == ELF_CLASS_64) ? ((Elf64_Phdr *)phdr)->p_offset : ((Elf32_Phdr *)phdr)->p_offset;
  phdr_filesz = (elf_format == ELF_CLASS_64) ? ((Elf64_Phdr *)phdr)->p_filesz : ((Elf32_Phdr *)phdr)->p_filesz;
  phdr_memsz = (elf_format == ELF_CLASS_64) ? ((Elf64_Phdr *)phdr)->p_memsz : ((Elf32_Phdr *)phdr)->p_memsz;


  /* There is a boot clobber check in the flash read logic but that will only
     check the elf file size length.  Need to ensure that zero init regions
     are also checked.  So make an explicit clobber check using mem size to
     be safe. */
  BL_VERIFY(boot_clobber_check_local_address_range(boot_flash_trans_get_clobber_tbl_ptr(trans_if),
                                                   dest_addr,
                                                   phdr_memsz),
            BL_ERR_CLOBBER_PROTECTION_VIOLATION);


  /* Only process segments with length greater than zero */
  /* 0x0 is a valid address.  Hence no need to validate dest_addr for null pointer */
  if (phdr_filesz)
  {
    /* Copy the data segment flash into memory */
    success = boot_flash_trans_read( trans_if,
                                     dest_addr,
                                     (uint32)phdr_offset,
                                     (uint32)phdr_filesz );
    BL_VERIFY( success, BL_ERR_ELF_FLASH );
  }
   
 
  /* If uninitialized data exists, make sure to zero-init */
  if (phdr_memsz > phdr_filesz)
  {
    qmemset((uint8 *)(uintnt)(dest_addr + phdr_filesz), 
            0,
            (phdr_memsz - phdr_filesz)); 
  }


  /* Mark this memory area with boot clobber protection for another level of
     security.  This ensures the image just loaded cannot be overwritten by
     loading of another image. */
  boot_clobber_add_global_protection_region(dest_addr,
                                            phdr_memsz);
}


/*===========================================================================
**  Function :  boot_elf_load_seg_in_chunks
** ==========================================================================
*/
/*!
* 
* @brief
*   This function loads the data segment into memory using the information
*   from a specific program header.  
*
* @param[in] phdr - Pointer to the Program Header structure 
*
* @par Dependencies
*   None 
* 
* @retval
*   None
* 
*/
static void boot_elf_load_seg_in_chunks(void * phdr)
{
  boolean success = FALSE;  
  uint32 chunk_size = MAX_CHUNK_SIZE;
  uint64 remaining_data = 0, chunk_dest_addr =0, chunk_offset =0;
  uint64 phdr_entry_offset, phdr_entry_paddr, phdr_entry_filesz, phdr_entry_memsz;

  boot_boolean is_auth_enabled = FALSE;
  bl_error_type status = BL_ERR_IMG_SECURITY_FAIL;

  BL_VERIFY( phdr != NULL, BL_ERR_NULL_PTR_PASSED );

  status = boot_is_auth_enabled(&is_auth_enabled);
  BL_VERIFY((status == BL_ERR_NONE), BL_ERR_IMG_SECURITY_FAIL); 

  if (elf_format == ELF_CLASS_64)
  {
    phdr_entry_offset = ((Elf64_Phdr *)phdr)->p_offset;
    phdr_entry_paddr = ((Elf64_Phdr *)phdr)->p_paddr;
    phdr_entry_filesz = ((Elf64_Phdr *)phdr)->p_filesz;
    phdr_entry_memsz = ((Elf64_Phdr *)phdr)->p_memsz;
  }
  else
  {
    phdr_entry_offset = ((Elf32_Phdr *)phdr)->p_offset;
    phdr_entry_paddr = ((Elf32_Phdr *)phdr)->p_paddr;
    phdr_entry_filesz = ((Elf32_Phdr *)phdr)->p_filesz;
    phdr_entry_memsz = ((Elf32_Phdr *)phdr)->p_memsz;
  }
  
  /* Do a clobber check on memsz before zeroing out the memory */
  BL_VERIFY(boot_clobber_check_local_address_range(boot_flash_trans_get_clobber_tbl_ptr(trans_if),
                                                   (uint8 *)(uintnt)phdr_entry_paddr,
                                                   (uint32)phdr_entry_memsz),
            BL_ERR_CLOBBER_PROTECTION_VIOLATION);
	  
  if (boot_elf_phdr_is_loadable(phdr))
  {
    /* Only process segments with valid parameters */
    /* 0x0 is a valid address. Hence no need to validate dest_addr for null pointer 
    */
    if (phdr_entry_filesz)
    {
      remaining_data = phdr_entry_filesz;
      chunk_dest_addr = phdr_entry_paddr;
      chunk_offset = phdr_entry_offset;
      while(remaining_data !=0)
      {
        if(remaining_data < chunk_size )
        {
          chunk_size = remaining_data;
        }
          
        /* Copy the data segment flash into memory */
        success = boot_flash_trans_read( trans_if,
                                         (uint8 *)(uintnt)chunk_dest_addr,
                                         chunk_offset,
                                         chunk_size );
        BL_VERIFY( success, BL_ERR_ELF_FLASH );
          
        chunk_dest_addr += chunk_size;
        chunk_offset += chunk_size;
        remaining_data = remaining_data - chunk_size;
        
        /* Update size indicating how much elf is loaded */
        BL_VERIFY((total_elf_segments_size + chunk_size > total_elf_segments_size),
                  BL_ERR_IMG_SECURITY_FAIL); 
        total_elf_segments_size = total_elf_segments_size + chunk_size;

        if(!is_auth_enabled)
        { 
          /* Set total length of loaded segments as soon as chunk of data is
           loaded ONLY if MSA is enabled and SBL is not doing authentication.
           Else to be done post hash verification. mba will xpu lock segments
           as soon as length is updated 
          */
          boot_rmb_set_pmi_code_length(total_elf_segments_size);
        }
      }
    }
	
    /* If uninitialized data exists, make sure to zero-init */
    if (phdr_entry_memsz > phdr_entry_filesz)
    {
      qmemset((uint8 *)(uintnt)(phdr_entry_paddr + phdr_entry_filesz),
              0,
              (phdr_entry_memsz - phdr_entry_filesz)); 
		
      /* Update size indicating how much elf is loaded */
      BL_VERIFY((total_elf_segments_size + (phdr_entry_memsz - phdr_entry_filesz) > total_elf_segments_size),
                       BL_ERR_IMG_SECURITY_FAIL); 
      total_elf_segments_size = total_elf_segments_size + (phdr_entry_memsz - phdr_entry_filesz);

      if(!is_auth_enabled)
      {
        /* Set total length of loaded segments as soon as chunk of data is
            loaded ONLY if MSA is enabled and SBL is not doing authentication.
            Else to be done post hash verification. mba will xpu lock segments
            as soon as length is updated 
        */
        boot_rmb_set_pmi_code_length(total_elf_segments_size);
      }
    }

    /* Mark this memory area with boot clobber protection for another level of
       security.  This ensures the image just loaded cannot be overwritten by
       loading of another segment/image. */
    boot_clobber_add_global_protection_region((uint8 *)(uintnt)phdr_entry_paddr,
                                              (uint32)phdr_entry_memsz);
  }
}



/*===========================================================================
**  Function :  boot_elf_load_segment
** ==========================================================================
*/
/*!
* 
* @brief
*   This function loads the data segment into memory using the information
*   from a specific program header.  
*
* @param[in] phdr - Pointer to the Program Header structure 
*
* @par Dependencies
*   None 
* 
* @retval
*   None
* 
*/
static void boot_elf_load_seg(void * phdr)
{
  uint64 phdr_entry_paddr;
  
  BL_VERIFY( phdr != NULL, BL_ERR_NULL_PTR_PASSED );

  phdr_entry_paddr = (elf_format == ELF_CLASS_64) ? ((Elf64_Phdr *)phdr)->p_paddr : ((Elf32_Phdr *)phdr)->p_paddr;
  
  if(boot_elf_phdr_is_loadable(phdr))
  {
    boot_elf_load_seg_generic(phdr, (uint8 *)(uintnt)phdr_entry_paddr);
  }
}

/*===========================================================================
**  Function :  boot_elf_verify_segment
** ==========================================================================
*/
/*!
* 
* @brief
*   This function will verify a particular Program Header with an expected
*   hash value. This function returns nothing, but completes upon success.
*   The program headers are verified differently as follows:
*   
*   1. Type MI_PBT_PHDR_SEGMENT - Hash over ELF Header + All Program Headers
*   2. Type MI_PBT_HASH_SEGMENT - Do not verify the hash segment itself
*   3. Any loadable segment - Hash over the full data segment 
*   4. Otherwise - Return
*
* @param[in] phdr - Pointer to the Program Header structure 
* @param[in] expected_hash - Byte Pointer to the expected hash value 
*
* @par Dependencies
*   None 
* 
* @retval
*   None
* 
*/
static void boot_elf_verify_segment(void * phdr, uint8 * expected_hash)
{
  uint32 phdr_entry_flags;
  uint64 phdr_entry_paddr, phdr_entry_filesz;

  if (elf_format == ELF_CLASS_64)
  {
    phdr_entry_flags = ((Elf64_Phdr *)phdr)->p_flags;
    phdr_entry_paddr = ((Elf64_Phdr *)phdr)->p_paddr;
    phdr_entry_filesz = ((Elf64_Phdr *)phdr)->p_filesz;
  }
  else
  {
    phdr_entry_flags = ((Elf32_Phdr *)phdr)->p_flags;
    phdr_entry_paddr = ((Elf32_Phdr *)phdr)->p_paddr;
    phdr_entry_filesz = ((Elf32_Phdr *)phdr)->p_filesz;
  }
    
  /* ensure that hash_table is authenticated before trusting/using 
     hashes from hash table for segment hashing */
  BL_VERIFY( (boot_get_hash_table_auth_result() == TRUE), 
              BL_ERR_IMG_SECURITY_FAIL);
   
  /* proceed to do elf segment hashing only if ehdr+phdr hash check
     was already completed successfully */
  BL_VERIFY( (boot_elf_is_ehdr_phdr_hash_success == TRUE), 
              BL_ERR_IMG_SECURITY_FAIL);

  if( MI_PBT_SEGMENT_TYPE_VALUE(phdr_entry_flags) == MI_PBT_PHDR_SEGMENT)
  {
    /* ehdr+phdr is already verified, continue */ 
    return;
  }  
  /* Skip hash verification for the hash segment */
  else if( MI_PBT_SEGMENT_TYPE_VALUE(phdr_entry_flags) == MI_PBT_HASH_SEGMENT)
  {
    return;
  }
  /* Verify hash for the load segment only if the file size is not 0 */
  else if ((boot_elf_phdr_is_loadable(phdr)) && (phdr_entry_filesz > 0))
  {
    /* Computer hash over full data segment */
    BL_VERIFY(BL_ERR_NONE == 
                boot_hash(sbl_hash_algorithm,
                          (uint8 *)(uintnt)phdr_entry_paddr,
                          (uint32)phdr_entry_filesz,
                          verify_digest_buf,
                          sbl_hash_len),
              BL_ERR_IMG_SECURITY_FAIL);
    
    /* Compare calculated hash value to entry in hash table */
    BL_VERIFY (bByteCompare( verify_digest_buf, 
                                expected_hash, 
                                sbl_hash_len ) == 0,
                  BL_ERR_ELF_HASH_MISMATCH); 
  }
  else
  {
    return;
  }
}

/*===========================================================================
**  Function :  boot_elf_verify_start_addr_32
** ==========================================================================
*/
/*!
* 
* @brief
*   This function verifies elf entry_point (image start address) falls 
*   within loadable elf segments area [loadable segment's (phy/virt address
*   + file_size)].
*   
*
* @par Dependencies
*   None 
* 
* @retval
*   boolean status - TRUE if elf image entry_point/start address is within 
*                    loadable segment's range otherwise false 
* 
*/
static boolean boot_elf_verify_start_addr_32()
{
  uint32 index = 0;
  boolean status = FALSE;
  Elf32_Phdr *phdr_entry_ptr = 0;
  
  /* scan through program header segments */
  for(index = 0; index < ehdr_32.e_phnum; index++)
  {
    phdr_entry_ptr = &phdr_array_32[index];
    if(boot_elf_phdr_is_loadable((void *)phdr_entry_ptr))
    {
      /* Only scan loadable segments with filesz>0 to check against entry_point  */
      if( (phdr_entry_ptr->p_memsz > 0) && (phdr_entry_ptr->p_filesz > 0) )
      {
        /* check again physical address range, most images have entry point
        within physical address range with completely different mapping
        for virtual space */
        if((phdr_entry_ptr->p_filesz < (MAX_ADDRESS_32 - phdr_entry_ptr->p_paddr)) && 
           (ehdr_32.e_entry >= phdr_entry_ptr->p_paddr) && 
           (ehdr_32.e_entry < (phdr_entry_ptr->p_paddr + phdr_entry_ptr->p_filesz)) 
          ) 
        { 
          status = TRUE;
          break;
        }
        /* check again for virtual address range. RPM image entry_point 
          falls within virtual range. */
        if ( (phdr_entry_ptr->p_filesz < (MAX_ADDRESS_32 - phdr_entry_ptr->p_vaddr)) && 
         (ehdr_32.e_entry >= phdr_entry_ptr->p_vaddr) && 
         (ehdr_32.e_entry < (phdr_entry_ptr->p_vaddr + phdr_entry_ptr->p_filesz)) 
        ) 
        { 
          status = TRUE;
          break;
        }
      }    
    }
  }
  return status;
}

/*===========================================================================
**  Function :  boot_elf_verify_start_addr  (64bit version)
** ==========================================================================
*/
/*!
* 
* @brief
*   This function verifies elf entry_point (image start address) falls 
*   within loadable elf segments area [loadable segment's (phy/virt address
*   + file_size)].
*   
*
* @par Dependencies
*   None 
* 
* @retval
*   boolean status - TRUE if elf image entry_point/start address is within 
*                    loadable segment's range otherwise false 
* 
*/
static boolean boot_elf_verify_start_addr()
{
  uint32 index = 0;
  boolean status = FALSE;
  Elf64_Phdr *phdr_entry_ptr = 0;
  
  /* scan through program header segments */
  for(index = 0; index < ehdr.e_phnum; index++)
  {
    phdr_entry_ptr = &phdr_array[index];
    if(boot_elf_phdr_is_loadable((void *)phdr_entry_ptr))
    {
      /* Only scan loadable segments with filesz>0 to check against entry_point  */
      if( (phdr_entry_ptr->p_memsz > 0) && (phdr_entry_ptr->p_filesz > 0) )
      {
        /* check again physical address range, most images have entry point
        within physical address range with completely different mapping
        for virtual space */
        if((phdr_entry_ptr->p_filesz < (MAX_ADDRESS - phdr_entry_ptr->p_paddr)) && 
           (ehdr.e_entry >= phdr_entry_ptr->p_paddr) && 
           (ehdr.e_entry < (phdr_entry_ptr->p_paddr + phdr_entry_ptr->p_filesz)) 
          ) 
        { 
          status = TRUE;
          break;
        }
        /* check again for virtual address range. RPM image entry_point 
          falls within virtual range. */
        if ( (phdr_entry_ptr->p_filesz < (MAX_ADDRESS - phdr_entry_ptr->p_vaddr)) && 
         (ehdr.e_entry >= phdr_entry_ptr->p_vaddr) && 
         (ehdr.e_entry < (phdr_entry_ptr->p_vaddr + phdr_entry_ptr->p_filesz)) 
        ) 
        { 
          status = TRUE;
          break;
        }
      }    
    }
  }
  return status;
}

/*===========================================================================
**  Function :  boot_elf_verify_elf_hdr
** ==========================================================================
*/
/*!
* 
* @brief
*   This function verifies hash of elf_hdr with an expected
*   hash value. If ehdr+phdr hash verification fails then this function  
*   doesn't return and enters boot_error_handler if hash check fails.
*   
*
* @par Dependencies
*   first hash in the hash table always represents hash of ehdr+phdr 
* 
* @retval
*   None
* 
*/
void boot_elf_verify_elf_hdr()
{
  /* reset the flag tracking ehdr+phdr hash check for current elf */
   boot_elf_is_ehdr_phdr_hash_success = FALSE;
   
   /* ensure ehdr buffer exist */
   BL_VERIFY( ehdr_ptr!=NULL, BL_ERR_ELF_NULL_PTR );

   /* ensure that there is atleast 1 prog-header entry and total number 
      of prog-hdr segments are not more than allocated prog-hdr buffer in SBL */
   BL_VERIFY( (phdr_num!=0) && (phdr_num <= MI_PBT_MAX_SEGMENTS), 
              BL_ERR_ELF_FORMAT );

  /* Verifiy the ELF Header */
  BL_VERIFY( boot_elf_verify_header()==TRUE, BL_ERR_ELF_PARSE );
  
  /* elf-header basic checks are ok, continue with elf_hdr+prog_hdr 
     hash verification */
  /* Initialize hash context */
  BL_VERIFY(BL_ERR_NONE == 
              boot_hashInit(&elf_ctx, sbl_hash_algorithm),
              BL_ERR_IMG_SECURITY_FAIL);

  if(boot_elf_enable_hash_checking())
  {
    /* ensure that hash_table is authenticated before trusting/using 
     hashes from hash table for ehdr+phdr hash verification */
    BL_VERIFY( (boot_get_hash_table_auth_result() == TRUE), 
                BL_ERR_IMG_SECURITY_FAIL);

    /* Hash the ELF header */
    BL_VERIFY(BL_ERR_NONE == 
                boot_hashUpdate(&elf_ctx,
                                    (const uint8 *)ehdr_ptr,
                                    ehdr_size),
                BL_ERR_IMG_SECURITY_FAIL);

    /* Hash the program headers */
    BL_VERIFY(BL_ERR_NONE == 
                boot_hashUpdate(&elf_ctx,
                                    (const uint8 *)phdr_array_ptr,
                                    phdr_num * phdr_size),
                BL_ERR_IMG_SECURITY_FAIL);

    /* Finalize the hash to obtain hash value */
    BL_VERIFY(BL_ERR_NONE == 
                boot_hashFinal(&elf_ctx,
                              verify_digest_buf,
                              sbl_hash_len),
                BL_ERR_IMG_SECURITY_FAIL); 

    /* Compare calculated hash value to first hash_entry in the 
       hash table which is assumed to be ehdr+phdr hash */
    BL_VERIFY (bByteCompare( verify_digest_buf, 
                             hash_segment_start, 
                             sbl_hash_len ) == 0,
                  BL_ERR_ELF_HASH_MISMATCH);
				  
    /* track successful hashing of ehdr+phdr*/
    boot_elf_is_ehdr_phdr_hash_success = TRUE;
  }
  /* check entry_point to ensure that it falls within loadable 
     segment area from verified phdr segments */
  if (elf_format == ELF_CLASS_64)
    BL_VERIFY(TRUE == 
              boot_elf_verify_start_addr(),
              BL_ERR_ELF_FORMAT);
  else
    BL_VERIFY(TRUE == 
              boot_elf_verify_start_addr_32(),
              BL_ERR_ELF_FORMAT);
  	
}

/*===========================================================================
**  Function :  boot_elf_ziinit_structures
** ==========================================================================
*/
/*!
* 
* @brief
*   This function zero-initializes all global variables within the module which
*   are necessary to perform ELF loading.  
*
* @param[out] header_ptr - Pointer to the QC header that is read from the image
*
* @par Dependencies
*   None 
* 
* @retval
*   None
* 
*/
void boot_elf_ziinit_structures(mi_boot_image_header_type *header_ptr)
{
  /* reset the flag tracking hash table authentication result for current elf */
  boot_set_hash_table_auth_result(FALSE);
  
  /* reset the flag tracking ehdr+phdr hash check for current elf */
  boot_elf_is_ehdr_phdr_hash_success = FALSE;
  
  /* zi-init header which holds hash-table info */
  if (header_ptr)
    qmemset((uint8 *)header_ptr, 0, sizeof(mi_boot_image_header_type));

  /* zi-init elf header */
  qmemset((uint8 *)&ehdr_32, 0, sizeof(Elf32_Ehdr));
  qmemset((uint8 *)&ehdr, 0, sizeof(Elf64_Ehdr));

  /* zi-init prog-header */
  qmemset((uint8 *)(phdr_array_32), 0, (sizeof(Elf32_Phdr) * MI_PBT_MAX_SEGMENTS));
  qmemset((uint8 *)(phdr_array), 0, (sizeof(Elf64_Phdr) * MI_PBT_MAX_SEGMENTS));

  /* zi-init hash table segment */
  qmemset((uint8 *)(boot_get_hash_segment_buffer()), 0, HASH_SEGMENT_SIZE);
}


/*===========================================================================
**  Function :  boot_seg_verification
** ==========================================================================
*/
/*!
* 
* @brief
*   This function verifies loaded elf segment and updates hash pointer
*
* @par Dependencies
*   Must be called only after loading elf segment
*
* @param[in] index - Index of current segment in program header table 
* @param[in] curr_hash_ptr_val - Pointer to seg hash entry in hash table
* 
* @retval
*   None
*/
static void boot_seg_verification(uint32 index, uint8 **curr_hash_ptr_val)
{
  void *phdr_entry_ptr;
  uint32 phdr_entry_flags;
  uint64 phdr_entry_filesz;

  if (elf_format == ELF_CLASS_64)
  {
    phdr_entry_ptr = &phdr_array[index];
    phdr_entry_flags = phdr_array[index].p_flags;
    phdr_entry_filesz = phdr_array[index].p_filesz;
  }
  else
  {
    phdr_entry_ptr = &phdr_array_32[index];
    phdr_entry_flags = phdr_array_32[index].p_flags;
    phdr_entry_filesz = phdr_array_32[index].p_filesz;
  }
  	
  /* Verify the segment if necessary */
  if(boot_elf_enable_hash_checking())
  {
       /* Validate that the hash ptr offsets do not overflow beyond the allocated buffer and that there is atleast one 
	   hash_digest_size of data left over in the buffer for the last hash ptr offset */
	BL_VERIFY((( (uint32)*curr_hash_ptr_val <= ((uint32)hash_segment_start + HASH_SEGMENT_SIZE - sbl_hash_len)) && 
	           (((uint32)*curr_hash_ptr_val) >= (uint32)hash_segment_start)), BL_ERR_SBL);
			   
    boot_elf_verify_segment((void *)phdr_entry_ptr, *curr_hash_ptr_val);

    /* Increment the hash pointer accordingly */
    if( MI_PBT_PAGE_MODE_VALUE(phdr_entry_flags) == MI_PBT_PAGED_SEGMENT )
    {
      /* Paged segments have a hash entry for each 4K page of data.
        Thus these hash entries must be skipped to find the hash
        entry for the next segment. */

      /* Compute number of 4K pages by rounding up */
      uint32 num_4k_pages = ( ( (uint32)phdr_entry_filesz - 1 ) / SIZE4K ) + 1;  

      *curr_hash_ptr_val += num_4k_pages * sbl_hash_len;
    }
    else
    {
      *curr_hash_ptr_val += sbl_hash_len;
    } 
  }
}

/*===========================================================================
**  Function :  boot_elf_load_and_verify_image_in_chunks
** ==========================================================================
*/
/*!
* 
* @brief
*   This function loads all the data segments corresponding to the global
*   program header array in chunks and also verifies hashes if applicable. 
*
* @par Dependencies
*   boot_elf_load_init_hash_segment() must be called
* 
* @retval
*   None
*/
static void boot_elf_load_and_verify_image_in_chunks(void)
{
  boot_boolean is_auth_enabled = FALSE;
  uint32 index = 0;
  bl_error_type status = BL_ERR_IMG_SECURITY_FAIL;

  /* Pointer to keep track of expected hash values ~ Initialized to the first
   * entry in the hash segment. */
  uint8 * curr_hash_ptr = hash_segment_start;  
  void *phdr_entry_ptr;
  uint32 phdr_flags;
  
  /* For all program headers */
  for(index = 0; index < phdr_num; index++)
  {
    if (elf_format == ELF_CLASS_64)
    {
      phdr_entry_ptr = &phdr_array[index];
      phdr_flags = phdr_array[index].p_flags;
    }
    else
  {
      phdr_entry_ptr = &phdr_array_32[index];
      phdr_flags = phdr_array_32[index].p_flags;
    }
	
    /* Load the corresponding data segment except hash segment
      It should be already loaded by this time 
    */
    if(MI_PBT_SEGMENT_TYPE_VALUE(phdr_flags) != MI_PBT_HASH_SEGMENT)
    {  
      /*Load segment in chunks and update loaded image size*/
      boot_elf_load_seg_in_chunks(phdr_entry_ptr);
    }

    /* verify segment hash */
    boot_seg_verification(index, &curr_hash_ptr);
    
    status = boot_is_auth_enabled(&is_auth_enabled);
    BL_VERIFY((status == BL_ERR_NONE), BL_ERR_IMG_SECURITY_FAIL); 
	
    if (is_auth_enabled)
    {
     /* Set total length of loaded segments after hash verification when
        MSA is disabled and SBL is authenticating. mba will xpu lock
        segments as soon as length is updated.
      */
      boot_rmb_set_pmi_code_length(total_elf_segments_size);
    }    
    
  }
}

/*===========================================================================
**  Function :  boot_elf_load_and_verify_image
** ==========================================================================
*/
/*!
* 
* @brief
*   This function loads all the data segments corresponding to the global
*   program header array. After each segment is loaded into memory, this 
*   function will also verify the data segments with their respective hashes.
*
* @par Dependencies
*   boot_elf_load_init_hash_segment() must be called
* 
* @retval
*   None
*/
static void boot_elf_load_and_verify_image(void)
{
  uint32 index = 0;
  void *phdr_entry_ptr;

  /* Pointer to keep track of expected hash values ~ Initialized to the first
   * entry in the hash segment. */
  uint8 * curr_hash_ptr = hash_segment_start;

  /* For all program headers */
  for(index = 0; index < phdr_num; index++)
  {
    phdr_entry_ptr = (elf_format == ELF_CLASS_64) ? (void *)&phdr_array[index] : (void *)&phdr_array_32[index];
	
    /* Load the corresponding data segment */
    boot_elf_load_seg(phdr_entry_ptr);

    /* verify segment hash */
    boot_seg_verification(index, &curr_hash_ptr);
  }
}

/*===========================================================================
**  Function :  boot_elf_load_deinit
** ==========================================================================
*/
/*!
* 
* @brief
*   This function deinitializes the necessary variables when ELF loading is 
*   complete. The flash translation interface is closed, and reset to NULL
*   so that it may be re-used again by the module for a new target image. 
*
* @par Dependencies
*   None 
* 
*/
static void boot_elf_load_deinit(void)
{
  BL_VERIFY( trans_if != NULL, BL_ERR_NULL_PTR_PASSED );

  /* reset the flag tracking hash table authentication result for current elf */
  boot_set_hash_table_auth_result(FALSE);

  /* reset the flag tracking ehdr+phdr hash check for current elf */
  boot_elf_is_ehdr_phdr_hash_success = FALSE;

  /* Release the translation layer resource */
  boot_flash_dev_close_image(&trans_if);

  /* Assign trans_if to NULL to re-initialize interface */
  trans_if = NULL;
}

/*===========================================================================
**  Function :  boot_get_elf_entry_address
** ==========================================================================
*/
/*!
* 
* @brief
*   This function returns the value of the elf header entry pointer. 
*
* @par Dependencies
*   None 
* 
*/
uint64 boot_get_elf_entry_address(void)
{
   return (elf_format == ELF_CLASS_64) ? ehdr.e_entry : ehdr_32.e_entry;
}

/*===========================================================================
**  Function :  boot_get_elf_eident_class
** ==========================================================================
*/
/*!
* 
* @brief
*   This function returns the value of the elf e_ident class value.
*   1 = 32 bit elf, 2 = 64 bit elf according to ELF spec.
*
* @par Dependencies
*   None 
* 
*/
uint32 boot_get_elf_eident_class(void)
{
   return elf_format;
}

/*===========================================================================
**  Function :  boot_get_elf_header
** ==========================================================================
*/
/*!
* 
* @brief
*   This function returns the address of elf header. 
*
* @par Dependencies
*   None 
* 
*/
uint64 boot_get_elf_header(void)
{
   return (elf_format == ELF_CLASS_64) ? (uint64)&ehdr : (uint64)&ehdr_32;
}

/*===========================================================================
**  Function :  boot_get_program_header
** ==========================================================================
*/
/*!
* 
* @brief
*   This function returns the address of program header. 
*
* @par Dependencies
*   None 
* 
*/
uint64 boot_get_program_header(void)
{
   return (elf_format == ELF_CLASS_64) ? (uint64)phdr_array : (uint64)phdr_array_32;
}

/*===========================================================================
**  Function :  boot_elf_populate_meta_info
** ==========================================================================
*/
/*!
* 
* @brief
*  This function will populate a buffer with the meta data for the current
*  elf image being processed. Meta data format:
*  elf header followed by program header table followed by
*  hash segment  
*
* @param[in] 
*  None
*
* @param[out] 
*  None
*
* @par Dependencies
*   Must be called only after boot_elf_load_init_hash_segment.
*   If the buffer lies in DDR, api must be called after DDR init.
*/
static void boot_elf_populate_meta_info(void)
{
  uint8 *meta_buff_ptr = NULL;

  if(elf_ssa_enabled == TRUE)
  { 
   /*Proceed only if subsytem expects the meta data */
    /*Get pointer to meta buffer */
    meta_buff_ptr = boot_rmb_get_meta_info_buffer();

    BL_VERIFY((NULL !=  meta_buff_ptr), BL_ERR_NULL_PTR_PASSED);

    /* zi init max possible meta_buff_ptr with ehdr, phdr, 
       hash-segment_buffer max sizes */
    qmemset((uint8*)meta_buff_ptr,
            0,
            (ehdr_size + 
             (phdr_size * MI_PBT_MAX_SEGMENTS) +
             boot_get_hash_buffer_size()));

    /* Copy elf header to buffer */
    qmemcpy(meta_buff_ptr, ehdr_ptr, ehdr_size);

    /* Update buffer to point to next free location */
    meta_buff_ptr = meta_buff_ptr + ehdr_size;

    /* Copy program header table to buffer */
    qmemcpy(meta_buff_ptr, phdr_array_ptr,
                   (phdr_size * phdr_num));

    /* Update buffer to point to next free location */
    meta_buff_ptr = meta_buff_ptr + 
                    (phdr_size * phdr_num);

    /* Copy hash segment to buffer*/
    qmemcpy(meta_buff_ptr, (const void *) hash_segment_base_addr,
            hash_segment_length);

    /*Signal subsystem authenticator image to retrieve meta info buffer */
    boot_rmb_populate_meta_info();
  }
}

/*===========================================================================
**  Function :  boot_elf_ssa_load_image_and_metadata
** ==========================================================================
*/
/*!
* 
* @brief
*  This function will populate a buffer with the meta data for the current
*  elf image being processed. It then loads the elf image and signals mba
*  via rmb registers, to process metadata .
*
* @param[in] 
*  None
*
* @param[out] 
*  None
*
* @par Dependencies
*   Must be called only after boot_elf_load_init_hash_segment.
*   If the buffer lies in DDR, api must be called after DDR init.
*/
static void boot_elf_ssa_load_image_and_metadata(void)
{
  uintnt ehdr_entry = (elf_format == ELF_CLASS_64) ? ehdr.e_entry : ehdr_32.e_entry;

  /* Initialize total segments size of elf to be loaded */
  total_elf_segments_size = 0;

  /* Get meta information for the elf to be loaded. send it to subsystem
    image expecting it (eg MBA)
  */
  boot_elf_populate_meta_info();

  /* Set the image start address. It is assumed the elf segments are in
    increasing order and first segment start address is lowest address
  */
  boot_rmb_set_pmi_code_start_addr(ehdr_entry);
  
  /*Signal MBA image to start processing PMI elf segments and
    jump to elf
  */
  boot_rmb_set_pmi_elf_load_ready();

  /* Load all data segments into memory for segmented authentication
  */ 
  boot_elf_load_and_verify_image_in_chunks();
    
}

/*===========================================================================
**  Function :  boot_elf_open_and_parse_elf_header
** ==========================================================================
*/
/*!
* 
* @brief
*   This function opens the elf header in the partition and parses the header
*
* @param[in] partition_id - Pointer to the partition ID information
* @param[in] header_ptr   - Pointer to the QC header buffer that must be cleared
*
* @par Dependencies
*   None 
* 
* @retval
*   None
* 
*/
static void boot_elf_open_and_parse_elf_header(uint8 * partition_id,
                                               mi_boot_image_header_type * header_ptr)
{
  boolean success = FALSE;

  /* re-initialize all elf-hdr, prog-hdr, hash-table buffers to 0 */
  boot_elf_ziinit_structures(header_ptr);

  /* Configure the target image using custom partition ID information */ 
  boot_flash_configure_target_image(partition_id);

  /* Open the translation interface. 
     For each ELF image to be loaded, the translation interface must be opened once and only 
     used for that particular partition ID/target image. When the loading process is finished, 
     the translation interface will be released and reset to NULL. This allows for subsequent 
     target ELF images to re-open and use the translation interface when they are loaded. */
  if (trans_if == NULL)
  {
    trans_if = boot_flash_dev_open_image(GEN_IMG);
  }
  
  BL_VERIFY( trans_if != NULL, BL_ERR_NULL_PTR );

  /* Allow the address range of the elf header structure to be written to */
  /* Note here we use 64bit ELF header as the container since they have the
   * same fields (same e_ident field) except for 3 fields that are of 64bits long */
  boot_clobber_add_local_hole( boot_flash_trans_get_clobber_tbl_ptr( trans_if ),
                               &ehdr, sizeof(Elf64_Ehdr) );

  /* Copy the ELF header from flash into our local structure */
  success = boot_flash_trans_read( trans_if,
                                   &ehdr,
                                   0,
                                   sizeof(Elf64_Ehdr));

  /* Remove local hole */
  boot_clobber_remove_local_hole( boot_flash_trans_get_clobber_tbl_ptr( trans_if ),
                                  &ehdr, sizeof(Elf64_Ehdr) );


  BL_VERIFY( success, BL_ERR_ELF_FLASH );

  /* Check the EI_CLASS to see if it's 32bit or 64bit ELF 
    * Note the e_ident format is the same in both 32bit and 64bit
    */
  elf_format = ehdr.e_ident[ELFINFO_CLASS_INDEX];

  if (elf_format == ELF_CLASS_64)
  {
    ehdr_ptr =&ehdr;
    ehdr_size = sizeof(ehdr);
	
    phdr_size = ehdr.e_phentsize;	
    phdr_num = ehdr.e_phnum;
    phdr_array_ptr = phdr_array;
  }
  else
  {
    ehdr_ptr =&ehdr_32;
    ehdr_size = sizeof(ehdr_32);
    /* It's 32bit ELF so update ehdr_32 and clear ehdr */
    qmemcpy(&ehdr_32, &ehdr, ehdr_size);
    qmemset(&ehdr, 0, sizeof(Elf64_Ehdr));
	
    phdr_size = ehdr_32.e_phentsize;
    phdr_num = ehdr_32.e_phnum;
    phdr_array_ptr = phdr_array_32;
  }
}
 
/*===========================================================================
                        PUBLIC FUNCTION DEFINITIONS
=============================================================================*/

/*===========================================================================
**  Function :  boot_elf_verify_image_header
** ==========================================================================
*/
/*!
* 
* @brief
*   This function validates the elf header in the partition 
*
* @param[in] partition_id - Pointer to the partition ID information
* @param[in] header_ptr   - Pointer to the QC header buffer that must be cleared
*
* @par Dependencies
*   None 
* 
* @retval
*   Boolean TRUE is valid header, FALSE otherwise
* 
*/
boolean boot_elf_verify_image_header(uint8 * partition_id,
                                     mi_boot_image_header_type * header_ptr)
{
  boolean success = FALSE;
  
  BL_VERIFY( partition_id != NULL, BL_ERR_NULL_PTR_PASSED );

  boot_elf_open_and_parse_elf_header(partition_id,
                                     header_ptr);
	
  /* Verify the ELF Header */
  success = boot_elf_verify_header();
  BL_VERIFY( success, BL_ERR_ELF_PARSE );

  /* Release the translation layer resource */
  boot_flash_dev_close_image( &trans_if );
 
 
  return success;
}

 
/*===========================================================================
**  Function :  boot_elf_load_init_hash_segment
** ==========================================================================
*/
/*!
* 
* @brief
*   This function initializes all global variables within the module which are
*   necessary to perform ELF loading. This function will also load the 
*   hash data segment into memory, read in the QC header, and assign 
*   hash_segment_start to point to the start of the hash entries. 
*
* @param[in] partition_id - Pointer to the partition ID information
* @param[out] header_ptr - Pointer to the QC header that is read from the image
*
* @par Dependencies
*   None 
* 
* @retval
*   None
* 
*/
void boot_elf_load_init_hash_segment(uint8 * partition_id, mi_boot_image_header_type *header_ptr)
{
  boolean success = FALSE;
  uint32 index = 0;  
  void *phdr_entry_ptr;
  uint16 phdr_array_size;
  uint32 phdr_flags;
  uint64 ehdr_phoff, phdr_offset, phdr_memsz, phdr_filesz;

  BL_VERIFY( partition_id != NULL && header_ptr != NULL, BL_ERR_NULL_PTR_PASSED );

  boot_elf_open_and_parse_elf_header(partition_id,
                                    header_ptr);

  /* Verify the ELF Header */
  success = boot_elf_verify_header();
  BL_VERIFY( success, BL_ERR_ELF_PARSE );

  ehdr_phoff = (elf_format == ELF_CLASS_64) ? ehdr.e_phoff : ehdr_32.e_phoff;

  phdr_array_size = phdr_num * phdr_size;

  /* Allow the address range of the program header array to be written to */
  boot_clobber_add_local_hole( boot_flash_trans_get_clobber_tbl_ptr( trans_if ),
                               phdr_array_ptr, phdr_array_size); 

  /* Load the program headers */
  success = boot_flash_trans_read( trans_if,
                                   phdr_array_ptr,
                                   (uint32)ehdr_phoff,
                                   phdr_array_size);

  /* Remove local hole */
  boot_clobber_remove_local_hole( boot_flash_trans_get_clobber_tbl_ptr( trans_if ),
                                  phdr_array_ptr, phdr_array_size); 

  BL_VERIFY( success, BL_ERR_ELF_FLASH );

  /* Locate hash segment */
  for(index = 0; index < phdr_num; index++)
  {
    if (elf_format == ELF_CLASS_64)
    {
      phdr_entry_ptr = &phdr_array[index];
      phdr_flags = phdr_array[index].p_flags;
      phdr_offset = phdr_array[index].p_offset;
      phdr_memsz = phdr_array[index].p_memsz;
      phdr_filesz = phdr_array[index].p_filesz;
    }
    else
    {
      phdr_entry_ptr = &phdr_array_32[index];
      phdr_flags = phdr_array_32[index].p_flags;
      phdr_offset = phdr_array_32[index].p_offset;
      phdr_memsz = phdr_array_32[index].p_memsz;
      phdr_filesz = phdr_array_32[index].p_filesz;
    }

    /* Check to make sure loadable segment has valid size */
    if (boot_elf_phdr_is_loadable(phdr_entry_ptr))
    {
      BL_VERIFY( (phdr_memsz >= phdr_filesz), BL_ERR_IMG_HDR_INVALID );
    }

    /* Check for the hash segment program header to parse QC header */
    if( MI_PBT_SEGMENT_TYPE_VALUE(phdr_flags) == MI_PBT_HASH_SEGMENT)
    {
      /* Allow the address range of the image header structure to be written to */
      boot_clobber_add_local_hole( boot_flash_trans_get_clobber_tbl_ptr( trans_if ),
                                   header_ptr, sizeof(*header_ptr) );  

      /* Read in the QC header once hash segment is found */
      success = boot_flash_trans_read( trans_if,
                                       header_ptr,
                                       (uint32)phdr_offset,
                                       sizeof(*header_ptr) );

      /* Remove local hole */
      boot_clobber_remove_local_hole( boot_flash_trans_get_clobber_tbl_ptr( trans_if ),
                                   header_ptr, sizeof(*header_ptr) );  

      BL_VERIFY( success, BL_ERR_ELF_FLASH );
  
      /* Verify Buffer to load hash has sufficient space */
      BL_VERIFY( (phdr_memsz <= HASH_SEGMENT_SIZE),
                  BL_ERR_ELF_FLASH );
      
      /* Get buffer to load hash segment */
      hash_segment_base_ptr = boot_get_hash_segment_buffer();
      
      /* Allow the address range of the local hash segment buffer to be written to */
      boot_clobber_add_local_hole( boot_flash_trans_get_clobber_tbl_ptr( trans_if ),
                                   hash_segment_base_ptr, HASH_SEGMENT_SIZE); 
      
      /* Load full hash segment into memory */
      boot_elf_load_seg_generic(phdr_entry_ptr, hash_segment_base_ptr);

      /* Remove local hole */
      boot_clobber_remove_local_hole( boot_flash_trans_get_clobber_tbl_ptr( trans_if ),
                                      hash_segment_base_ptr, HASH_SEGMENT_SIZE); 

      /* After hash segment is loaded into memory, assign start address of hash entries */
      hash_segment_start = (uint8 *)(hash_segment_base_ptr + sizeof(*header_ptr));
     
      if(elf_ssa_enabled == TRUE)
      {
        /*Store hash segment base addr and length */
         hash_segment_base_addr = (uint8 *)hash_segment_base_ptr;
         hash_segment_length  = phdr_memsz;
      }
      
      /* Exit loop once hash segment is found and processed */ 
      break;
    } 
  }
}

/*===========================================================================
**  Function :  boot_load_elf_image
** ==========================================================================
*/
/*!
* 
* @brief
*   This function will load an ELF formatted image from the requested 
*   partition_id. The QC header located at the beginning of the hash segment
*   will also be loaded into the location pointed to by header_ptr
*
* @param[in] partition_id - Pointer to the partition ID information
* @param[out] header_ptr - Pointer to the QC header that is read from the image
*
* @par Dependencies
*   BOOT_FEATURE_LOAD_ELF must be defined 
* 
* @retval
*   None
* 
*/
void boot_load_elf_image(void)
{
  if(elf_ssa_enabled == TRUE)
  {
    boot_elf_ssa_load_image_and_metadata();
  }
  else
  {
    /* Load all data segments into memory and verify their hashes with the hash segment entries */ 
    boot_elf_load_and_verify_image();
  }
  /* Close the image partition and release the flash translation interface */
  boot_elf_load_deinit();
}

/*===========================================================================
**  Function :  boot_elf_set_auth_and_ssa_enable
** ==========================================================================
*/
/*!
* 
* @brief
*   This function will set/reset Flag to Turn on/Turn off authentication and
*   Subsystem image loading mode for elf images.
*
* @param[in] 
*  boolean auth_from_cfg_table   - TRUE if config table entry for the image
*                                  enables authentication else FALSE.
*  boolean ssa_value             - TRUE if Subsystem image loading mode to be
*                                  turned on else FALSE
*
* @param[out] 
*  None
*
* @par Dependencies
*  None
* 
* @retval
*   None
* 
*/
void boot_elf_set_auth_and_ssa_enable
( 
  boolean auth_from_cfg_table,
  boolean ssa_value
)
{
  elf_cfg_table_auth = auth_from_cfg_table;
  elf_ssa_enabled = ssa_value;
}

/*===========================================================================
**  Function :  boot_elf_reconstruct_elf_image
** ==========================================================================
*/
/*!
* 
* @brief
*  This function reconstructs the ELF image in one blob as laid out in the flash
*
* @param[in]
*  uint32 image_load_base  - base address of where the image will be reconstructed
*  uint32 image_max_size   - maximum size of the region used for reconstruction
*
*  NOTE: image_load_base and image_max_size are assumed to be set 
*        by caller that uses correct address/size
*
* @par Dependencies
*  Called after ELF image is loaded
* 
*/
void boot_elf_reconstruct_elf_image(uint32 image_load_base, uint32 image_max_size)
{
  uint32 index = 0;  
  uint32 phdr_flags;
  uint64 phdr_offset, phdr_filesz, phdr_entry_paddr;
  void *phdr_entry_ptr;

  /* Sanity check the input params */
  BL_VERIFY(((image_load_base + image_max_size) > image_load_base), BL_ERR_ELF_INVAL_PARAM);
  
  /* Zero out the memory region for image copying */
  qmemset((void *)image_load_base, 0, image_max_size);
  
  /* Copy the ELF header to the image destination */
  qmemcpy((void *)image_load_base, ehdr_ptr, ehdr_size);
	
  /* Append the program header table */
  qmemcpy((void *)(image_load_base + ehdr_size), phdr_array_ptr, (phdr_size * phdr_num));

  for (index = 0; index < phdr_num; index++)
  {
    if (elf_format == ELF_CLASS_64)
    {
      phdr_entry_ptr = &phdr_array[index];
      phdr_flags = phdr_array[index].p_flags;
      phdr_offset = phdr_array[index].p_offset;
      phdr_filesz = phdr_array[index].p_filesz;
      phdr_entry_paddr = phdr_array[index].p_paddr;	  
    }
    else
    {
      phdr_entry_ptr = &phdr_array_32[index];
      phdr_flags = phdr_array_32[index].p_flags;
      phdr_offset = phdr_array_32[index].p_offset;
      phdr_filesz = phdr_array_32[index].p_filesz;
      phdr_entry_paddr = phdr_array_32[index].p_paddr;	  
    }
	
    /* Make sure the image being copied doesn't exceed the region's limit,
      * also check against integer overflow 
      */
    BL_VERIFY(((uint32)(phdr_offset + phdr_filesz) <= image_max_size) && 
              ((phdr_offset + phdr_filesz)  >= phdr_offset) &&
              ((image_load_base + (uint32)phdr_offset) >= image_load_base), 
              BL_ERR_IMG_SIZE_TOO_BIG);
	
    if (MI_PBT_SEGMENT_TYPE_VALUE(phdr_flags) == MI_PBT_HASH_SEGMENT)
    {
      /* Copy the hash segment to the image load destination */
      qmemcpy((uint8 *)(image_load_base + (uint32)phdr_offset),
                       hash_segment_base_ptr, phdr_filesz);
    }
    else if (boot_elf_phdr_is_loadable(phdr_entry_ptr))
    {
      /* Copy the segment to the image load destination */
      qmemcpy((void *)(image_load_base + (uint32)phdr_offset),
                       (void *)(uintnt)phdr_entry_paddr, phdr_filesz);
    }
  }
	
}
