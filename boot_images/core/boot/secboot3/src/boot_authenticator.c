/*=============================================================================

                             Boot Authenticator

GENERAL DESCRIPTION
  This module performs SBL binary image authenticating, utilizing the
  functions provided by the PBL fucntion pointers.

Copyright 2014 by Qualcomm Technologies, Inc.  All Rights Reserved.
=============================================================================*/


/*=============================================================================

                            EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/boot.bf/3.1.2.c3/boot_images/core/boot/secboot3/src/boot_authenticator.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
11/26/14   lm      Added error check in boot_store_tpm_hash_block API
11/17/14   sk      Added to verify condition check for integer overflow and underflow.
11/12/14   mohan   Added check code to verify integer overflow
08/21/14   jz      Cleaned up obsoleted code
06/30/14   tj	   Added Additonal Conditional Check for Integer overflow in boot_auth_load_header
06/25/14   tj      Make sure that signature & cert ptr falls in the valid range
				   and should not overlapping.
06/10/14   jz      Check auth_enabled befor calling PBL secboot driver,
                   fixed boot_is_auth_enabled to check return status in the caller
04/29/14   ck      Added SAFE 1.0 logic and fixed secboot_authenticate function pointer assignment
04/29/14   ck      Renamed boot_sbl_auth_image to boot_auth_image as all images are verified
04/01/14   ck      Updated pbl_secboot_verified_info_type to secboot_verified_info_type
                   as Bear PBL uses common secboot lib now.
12/10/13   ck      Replaced return value of boot_is_auth_enabled to void since secboot
                   status is checked inside of function
12/06/13   ck      Removed bl_shared_data_type parameter from boot_is_auth_enabled
08/06/13   aus     Added support for new secboot_verified_info structure
05/07/13   dh      In boot_secboot_ftbl_init, Copy the pbl secboot function 
                   pointers to sbl local function tables individually to avoid
                   mismatch between pbl secboot function table structure and
                   sbl secboot function table structure
04/29/12   kedara  Update boot_auth_load_mba_header to check secure boot status
04/15/12   kedara  Added boot_auth_load_mba_header ,boot_is_msa_enabled_in_hw.
02/13/13   dh      SBL keeps its own copy of secboot function table. Based on 
                   FEATURE_BOOT_EXTERN_SECBOOT_COMPLETED flag we copy over 
                   PBL secboot function table or local secboot lib function table.
12/06/12   dh      Change SHARED_IMEM_TPM_HASH_REGION_BASE to SHARED_IMEM_TPM_HASH_REGION_OFFSET
11/28/12   dh      Add roll back version protection logic 
11/19/12   dh      add boot_store_tpm_hash_block to store tpm hash to shared imem
11/08/12   kedara  Support loading hash segment to seperate buffer.
10/17/12   dh      Check return value of secboot_init
09/26/12   kedara  Added boot_get_sbl_auth_verified_info
09/21/12   kedara  Modified auth function to return a value instead of
                   entering error handler
04/12/12   dh      Rewrote boot_pbl_auth_image to use latest pbl secboot API
12/01/11   dh      ported Rollback prevention version check.
10/27/11   dh      Added boot_auth_init_qd_certs
10/18/11   dh      Added boot_get_ou_field_info
09/01/11   dh      remove boot_pbl_auth_sbl2_image
08/10/11   kpa     Moved api supporting QDST to boot_auth_qd_cert.c
06/28/11   kpa     Added BOOT_MSM_HW_ID define to be used in QDST.
02/11/11   dh      Added a size check for signature_size in boot_pbl_auth_image
01/28/11   dxiang  Define boot_auth_image() utilizing PBL Function Pointers
11/28/10   plc     Added support for Secure-Boot with Qualcomm Development 
                   certificates
10/01/10   plc     Initial revision.
===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include BOOT_PBL_H
#include "boot_authenticator.h"
#include "boot_sbl_if.h"
#include "boot_util.h"
#include "miheader.h"
#include "boot_logger.h"
#include "boot_target.h"
#include "boot_rollback_version.h"
#include "secboot_hw.h"
#include "secboot.h"
#include "secboot_util.h"
#include "boot_error_if.h"
#include "boot_shared_imem_cookie.h"
#include "boot_extern_secboot_interface.h"

/*=========================================================================== 
 
                                    GLOBALS
 
===========================================================================*/

static __align(32)  secboot_image_info_type     sbl_auth_image_info;
static __align(32)  secboot_verified_info_type  sbl_auth_verified_info;


/* SBL's local copy of secboot function tables */
static secboot_hw_ftbl_type sbl_secboot_hw_ftbl;
static secboot_ftbl_type sbl_secboot_ftbl;

/*===========================================================================

                              FUNCTION DEFINITIONS

===========================================================================*/

/*===========================================================================

**  Function : boot_secboot_ftbl_init

** ==========================================================================
*/
/*!
* 
* @brief
*    This function initializes SBL's own copy of secboot function tables
*
* @param[in] bl_shared_data Pointer to shared data passed between functions
*
* @par Dependencies
*    None
* 
* @retval
*    bl_error_type Returns error code in case of error
*                  Returns BL_ERR_NONE in case of no error
* 
* 
*/
bl_error_type boot_secboot_ftbl_init(bl_shared_data_type *bl_shared_data)
{
  bl_error_type status = BL_ERR_IMG_SECURITY_FAIL;

#ifndef FEATURE_BOOT_EXTERN_SECBOOT_COMPLETED

  secboot_ftbl_type *pbl_secboot_ftbl_ptr = NULL;
  secboot_hw_ftbl_type *pbl_secboot_hw_ftbl_ptr = NULL;
  
  /* If we don't have a local secboot lib compiled in, use pbl's secboot lib */
  if( bl_shared_data != NULL &&
      bl_shared_data->sbl_shared_data != NULL &&
      bl_shared_data->sbl_shared_data->pbl_shared_data != NULL &&
      bl_shared_data->sbl_shared_data->pbl_shared_data->secboot_shared_data != NULL)
  {
    pbl_secboot_hw_ftbl_ptr = &bl_shared_data->sbl_shared_data->
                              pbl_shared_data->secboot_shared_data->pbl_secboot_hw_ftbl;
    pbl_secboot_ftbl_ptr = &bl_shared_data->sbl_shared_data->
                              pbl_shared_data->secboot_shared_data->pbl_secboot_ftbl;
                              
    /*Copy the pbl secboot function pointer to sbl local function tables*/                          
    sbl_secboot_hw_ftbl.secboot_hw_is_auth_enabled = pbl_secboot_hw_ftbl_ptr->secboot_hw_is_auth_enabled;

    sbl_secboot_ftbl.secboot_init = pbl_secboot_ftbl_ptr->secboot_init;
    sbl_secboot_ftbl.secboot_deinit = pbl_secboot_ftbl_ptr->secboot_deinit;
    sbl_secboot_ftbl.secboot_authenticate = pbl_secboot_ftbl_ptr->secboot_authenticate;
                       
    status = BL_ERR_NONE;
  }

#else

  /* If we have a local secboot lib compiled in, use sbl's secboot lib */
  if(E_SECBOOT_SUCCESS == boot_secboot_get_ftbl(&sbl_secboot_ftbl) &&
     E_SECBOOT_HW_SUCCESS == boot_secboot_hw_get_ftbl(&sbl_secboot_hw_ftbl))
  {
    status = BL_ERR_NONE;
  }
  
#endif

  return status;
}

/*===========================================================================

**  Function : boot_is_auth_enabled

** ==========================================================================
*/
/*!
* 
* @brief
*    This function determins if secboot is enabled 
*
* @param[out] is_auth_enabled Pointer to a boolean which will be set 
*                             to true if secboot is enabled, false if not
*
* @par Dependencies
*    None
* 
* @retval
*    bl_error_type Returns error code in case of error
*                  Returns BL_ERR_NONE in case of no error
* 
* 
*/
bl_error_type boot_is_auth_enabled(boot_boolean *is_auth_enabled)
{
  bl_error_type status = BL_ERR_IMG_SECURITY_FAIL;
  secboot_hw_etype sec_hw_err = E_SECBOOT_HW_SUCCESS;
  secboot_hw_ftbl_type *sbl_secboot_hw_ftbl_ptr = &sbl_secboot_hw_ftbl;


  if(sbl_secboot_hw_ftbl_ptr->secboot_hw_is_auth_enabled != NULL)
  {    
    sec_hw_err = sbl_secboot_hw_ftbl_ptr->secboot_hw_is_auth_enabled(SECBOOT_HW_APPS_CODE_SEGMENT,
                                                                     (uint32 *)is_auth_enabled);
   
    if (sec_hw_err == E_SECBOOT_HW_SUCCESS)
    {
      status = BL_ERR_NONE;
    }
  }

  return status;
}

/*===========================================================================

**  Function : boot_auth_image

** ==========================================================================
*/
/*!
* 
* @brief
*   This function authenticates a specific image with a specific image_type
*   ID. 
*
* @param[in] bl_shared_data - Pointer to shared data passed between functions
* @param[in] image_type - Secure image type ID
* 
* @par Dependencies
*   Image loaded successfully and boot_auth_load_header() has been called 
* 
* @retval
*    bl_error_type Returns error code in case of authentication error
*                  Returns 0 in case of no error
* 
* @par Side Effects
*    None
* 
*/
bl_error_type boot_auth_image(bl_shared_data_type * bl_shared_data,
                              secboot_sw_type image_sw_id)
{
  bl_error_type status = BL_ERR_NONE;            
       
  do
  {
    secboot_handle_type  sbl_sec_handle = {0};
    secboot_error_type   sec_err         = E_SECBOOT_SUCCESS;
    
    secboot_ftbl_type    *sbl_secboot_ftbl_ptr = &sbl_secboot_ftbl;
    uint32               fuse_img_version = 0; 
    uint32               cert_img_version = 0;

#ifndef FEATURE_BOOT_EXTERN_SECBOOT_COMPLETED
    boot_boolean         is_auth_enabled = FALSE;
#endif

    /* Check function pointers for validity */
    if (sbl_secboot_ftbl_ptr->secboot_init == NULL ||
        sbl_secboot_ftbl_ptr->secboot_deinit == NULL ||
        sbl_secboot_ftbl_ptr->secboot_authenticate == NULL)
    {
      status = BL_ERR_NULL_PTR_PASSED;
      break;
    }


    /* Ensure verified info structure is clean. */
    qmemset((void *)&sbl_auth_verified_info,
            0,
            sizeof(sbl_auth_verified_info));  
  

    /* Get the current image version from roll back protection fuses
       and assign to the auth image info structure. */
    status = boot_rollback_get_fuse_version(image_sw_id,
                                            &fuse_img_version);
    if(status != BL_ERR_NONE)
    {
      break;
    }
    sbl_auth_image_info.sw_type        = image_sw_id;
    sbl_auth_image_info.sw_version     = fuse_img_version;    
     
          
    /* Initializes the secboot handle and Crypto HW engine */
    sec_err = sbl_secboot_ftbl_ptr->secboot_init(NULL,
                                                 &sbl_sec_handle);
    if (sec_err != E_SECBOOT_SUCCESS)
    {
      status = BL_ERR_IMG_SECURITY_FAIL;
      break;
    }


/* If FEATURE_BOOT_EXTERN_SECBOOT_COMPLETED is not defined then PBL
   secboot driver is being used.  SAFE 1.0 is not in PBL so call using
   older style secboot_authenticate */
#ifndef FEATURE_BOOT_EXTERN_SECBOOT_COMPLETED
    /* Check if secboot is enabled */
    status = boot_is_auth_enabled(&is_auth_enabled);  
    if ((status != BL_ERR_NONE) || (is_auth_enabled == FALSE))
    {
      break;
    }
    sec_err =
        sbl_secboot_ftbl_ptr->secboot_authenticate(&sbl_sec_handle,
                                                   SECBOOT_HW_APPS_CODE_SEGMENT,
                                                   &sbl_auth_image_info,
                                                   &sbl_auth_verified_info);
#else
    /* Use secboot driver to authenticate the image.  The following are valid
       scenarios via SAFE 1.0:
       secboot disabled - unsigned image
       secboot disabled - signed image, trusted or untrusted root
       secboot enabled - signed image, trusted root only */
    sec_err = secboot_authenticate_image(SECBOOT_HW_APPS_CODE_SEGMENT,
                                         &sbl_auth_image_info,
                                         &sbl_auth_verified_info,
                                         sbl_secboot_ftbl_ptr,
                                         &sbl_secboot_hw_ftbl,
                                         NULL);
#endif

    /* All scenarios are evaluated by secboot authenticate function.
       E_SECBOOT_SUCCESS is the only valid result in all scenarios. */
    if (sec_err != E_SECBOOT_SUCCESS)
    {
      status = BL_ERR_IMG_SECURITY_FAIL;
      break;
    }
    

    /* Deinitializes the secboot handle and Crypto HW engine*/
    sec_err = sbl_secboot_ftbl_ptr->secboot_deinit(&sbl_sec_handle);    
    if(E_SECBOOT_SUCCESS != sec_err)
    {
      status = BL_ERR_IMG_SECURITY_FAIL;
      break;
    }
    
    /* At this point authentication is successful, store the hash info
       to shared imem. */
	   boot_store_tpm_hash_block(bl_shared_data,
                              &sbl_auth_verified_info);
  
    /* We need to update the current image version 
       to match the image version in the cert */
    cert_img_version = sbl_auth_verified_info.sw_id >> 32;
    status =  boot_rollback_update_img_version(image_sw_id,
                                               cert_img_version);
    if(status != BL_ERR_NONE)
    {
      break;
    }      

  }while(0);
  
  return status;  
}


/*===========================================================================

**  Function : boot_auth_load_header

** ==========================================================================
*/
/*!
* 
* @brief
*    This function loads the header information to be accessed when
*    performing authentication routines.  
*
* @param[in] mi_boot_image_header_type* Pointer to the image header
* 
* @par Dependencies
*    mi_boot_image_header_type* is not NULL
* 
* @retval
*    None
* 
* @par Side Effects
*    None
* 
*/
void boot_auth_load_header( mi_boot_image_header_type* image_header )
{
  /* Verify the image header structure */
  BL_VERIFY( ( image_header != NULL ),
               BL_ERR_NULL_PTR_PASSED );
			   
  /* Validate certificate chain ptr falls in the valid range */
  BL_VERIFY((((uint32)image_header->cert_chain_ptr >= (uint32)image_header->image_dest_ptr) &&
		((uint32)image_header->cert_chain_ptr <= ((uint32)image_header->cert_chain_ptr+image_header->cert_chain_size))&&
		((uint32)image_header->cert_chain_ptr+image_header->cert_chain_size) <= ((uint32)image_header->image_dest_ptr+image_header->image_size)),
		BL_ERR_ADDR_INVALID_RANGE);
		
  /* Validate Signature ptr falls in the valid range */
  BL_VERIFY((((uint32)image_header->signature_ptr >= (uint32)image_header->image_dest_ptr) &&
		((uint32)image_header->signature_ptr <= ((uint32)image_header->signature_ptr+image_header->signature_size))&&
		((uint32)image_header->signature_ptr+image_header->signature_size) <= ((uint32)image_header->image_dest_ptr+image_header->image_size)),
		BL_ERR_ADDR_INVALID_RANGE);
  
  /* Check if the code ,signature and cert chain images are non overlapping */
  BL_VERIFY(((uint32)(image_header->image_dest_ptr) <= ((uint32)image_header->image_dest_ptr+image_header->code_size)),
               BL_ERR_IMG_SIZE_TOO_BIG );
  BL_VERIFY((((uint32)image_header->image_dest_ptr+image_header->code_size)
			<= (uint32)image_header->signature_ptr),
               BL_ERR_ADDR_OVER_LAPPING );
			   
  BL_VERIFY(((uint32)(image_header->signature_ptr) <= ((uint32)image_header->signature_ptr+image_header->signature_size)),
               BL_ERR_IMG_SIZE_TOO_BIG );
  BL_VERIFY((((uint32)image_header->signature_ptr+image_header->signature_size)
			<= (uint32)image_header->cert_chain_ptr),
               BL_ERR_ADDR_OVER_LAPPING );
 
  qmemset((void *)&sbl_auth_image_info, 0, sizeof(sbl_auth_image_info));

  sbl_auth_image_info.header_ptr_1   = (uint8*)image_header;
  sbl_auth_image_info.header_len_1   = sizeof(*image_header);  
  sbl_auth_image_info.code_ptr_1     = image_header->image_dest_ptr;
  sbl_auth_image_info.code_len_1     = image_header->code_size;
  sbl_auth_image_info.x509_chain_ptr = image_header->cert_chain_ptr;
  sbl_auth_image_info.x509_chain_len = image_header->cert_chain_size;
  sbl_auth_image_info.signature_ptr  = image_header->signature_ptr;
  sbl_auth_image_info.signature_len  = image_header->signature_size;
  
}


/*===========================================================================
**  Function : boot_get_sbl_auth_verified_info
** ==========================================================================
*/
/*!
* 
* @brief
*   This function returns the address of the sbl_auth_verified_info data 
*   structure. This field is populated during authentication routines, and 
*   may be required by other modules. The secboot_verified_info_type corresponds
*   to the last authenticated image.
*
* @param[in] bl_shared_data - Pointer to shared data passed between functions
* 
* @par Dependencies
*   sbl_auth_verified_info is populated post authentication
* 
* @retval
*   None
* 
* @par Side Effects
*   None
* 
*/
secboot_verified_info_type * boot_get_sbl_auth_verified_info
(
  bl_shared_data_type *bl_shared_data
)
{
  return &sbl_auth_verified_info;
}

/*===========================================================================

**  Function : boot_update_auth_image_info

** ==========================================================================
*/
/*!
* 
* @brief
*    This function updates the authentication structs to point to correct
*    addresses inside hash segment buffer. This is needed as hash segment now
*    is loaded to a fixed address buffer. But the hash header from compiled
*    image continues to point to incorrect compile time addresses.
*
* @param[in] 
*  uint8* hash_buffer_base_addr:  Pointer to hash buffer base addr
* 
* @par Dependencies
*  uint8* hash_buffer_base_addr* is not NULL.
*  Must be called *after* loading hash segment header and before calling
*  authentication routines. (Assumes sbl_auth_image_info is populated with
*  correct data)
* 
* @retval
*  None
* 
* @par Side Effects
*  None
* 
*/
void boot_update_auth_image_info( uint8* hash_buffer_base_addr )
{
  /* Verify the buffer pointer */
  BL_VERIFY( ( hash_buffer_base_addr != NULL ),
               BL_ERR_NULL_PTR_PASSED );
  BL_VERIFY(((uint32)hash_buffer_base_addr + sizeof(mi_boot_image_header_type)) >= (uint32)(hash_buffer_base_addr),
						  BL_ERR_SBL);
  BL_VERIFY(((uint32)sbl_auth_image_info.code_ptr_1 + sbl_auth_image_info.code_len_1) 
						  >= (uint32)(sbl_auth_image_info.code_ptr_1), BL_ERR_SBL);
  BL_VERIFY(((uint32)sbl_auth_image_info.signature_ptr + sbl_auth_image_info.signature_len) >= 
						  (uint32)(sbl_auth_image_info.signature_ptr), BL_ERR_SBL);
  
  sbl_auth_image_info.code_ptr_1     = hash_buffer_base_addr + 
                                           sizeof(mi_boot_image_header_type);
  
  sbl_auth_image_info.signature_ptr  = sbl_auth_image_info.code_ptr_1 +
                                       sbl_auth_image_info.code_len_1 ;
                                       
  sbl_auth_image_info.x509_chain_ptr = sbl_auth_image_info.signature_ptr + 
                                       sbl_auth_image_info.signature_len ;
    
}


/*===========================================================================
**  Function :  boot_store_tpm_hash_block
** ==========================================================================
*/
/*!
* 
* @brief
*   Store hash of loaded image into shared imem location if authentication is enabled
* 
* @param[in] bl_shared_data - Pointer to shared data passed between functions
*
* @param[in] sbl1_auth_verified_info image hash info
*
* @par Dependencies
*   None
*   
* @retval
*   Updated pointer value if ok.
*   NULL if error.
* 
* @par Side Effects
*   None
* 
*/
void boot_store_tpm_hash_block
(
  bl_shared_data_type *bl_shared_data, 
  secboot_verified_info_type *sbl1_auth_verified_info
)
{
#if (defined(SHARED_IMEM_TPM_HASH_REGION_SIZE) && defined(SHARED_IMEM_TPM_HASH_REGION_OFFSET))

  /* 
     Store the hash of the image last authenticated on to shared imem.
     Hash block pointer is updated to next available
     free location. Along with the hash, hash block also contains other info
     about image such as image address.
  */
   
  /* First uint32 represents how many blocks there are */
  uint32 tpm_hash_base = SHARED_IMEM_TPM_HASH_REGION_OFFSET + (uint32)boot_shared_imem_cookie_ptr;
  uint32 *num_of_hashes = (uint32 *)tpm_hash_base;
  uint8 remaining_block_length = 0;
  uint32 *tpm_hash_block_ptr = bl_shared_data->
                               sbl_shared_data->
                               next_avail_tpm_hash_blk_ptr;
  boot_boolean is_auth_enabled = FALSE;
  
  /* First determine if secboot is enabled */
  BL_VERIFY(boot_is_auth_enabled(&is_auth_enabled) == BL_ERR_NONE,
				BL_ERR_IMG_SECURITY_FAIL); 
				
  if(is_auth_enabled == TRUE)
  {
    /* Validate hash pointer and available space */
    BL_VERIFY(( tpm_hash_block_ptr != NULL ), BL_ERR_NULL_PTR );
    BL_VERIFY(( tpm_hash_base < (uint32)tpm_hash_block_ptr ),
                BL_ERR_TPM_HASH_BLK_STR_FAIL );
				
	/* Integer Underflow Check */
    BL_VERIFY(( (uint32)tpm_hash_block_ptr < ( tpm_hash_base + SHARED_IMEM_TPM_HASH_REGION_SIZE )),
                BL_ERR_TPM_HASH_BLK_STR_FAIL );
				
	/* Integer overflow Check */
	BL_VERIFY(( tpm_hash_base < ( tpm_hash_base + SHARED_IMEM_TPM_HASH_REGION_SIZE )),
                BL_ERR_TPM_HASH_BLK_STR_FAIL );
		
    /* Verify there is enough space remain to store one hash block */
    remaining_block_length = tpm_hash_base + 
                             SHARED_IMEM_TPM_HASH_REGION_SIZE - 
                             (uint32)tpm_hash_block_ptr;
    
    BL_VERIFY(remaining_block_length >= sizeof(secboot_image_hash_info_type),
              BL_ERR_TPM_HASH_BLK_STR_FAIL);
    
    /* Store hash to shared imem */
    qmemcpy(tpm_hash_block_ptr, 
            &sbl1_auth_verified_info->image_hash_info, 
            sizeof(secboot_image_hash_info_type));
    
    /* Update pointer to point to next available location */
    bl_shared_data->sbl_shared_data->next_avail_tpm_hash_blk_ptr = 
              (uint32 *)((uint32)tpm_hash_block_ptr + sizeof(secboot_image_hash_info_type));
    *num_of_hashes += 1;
  }

#endif  
  
}/*  boot_store_tpm_hash_block */


/*===========================================================================

**  Function : boot_is_msa_enabled_in_hw

** ==========================================================================
*/
/*!
* 
*  @brief
*    This function Checks if MSA [/subsystem self authentication (SSA)]
*    feature is enabled in hardware.
*
*  @param[in] 
*    None
*
*  @par Dependencies
*    Should be called only after boot_secboot_ftbl_init
* 
*  @retval
*    TRUE if MSA/SSA Feature is enabled in hw / qfprom fuses else
*    FALSE
* 
*  @par Side Effects
*    None
* 
*/
boot_boolean boot_is_msa_enabled_in_hw ( void )
{
  boot_boolean is_auth_enabled = FALSE;

  secboot_hw_etype sec_hw_err = E_SECBOOT_HW_SUCCESS;
  
  secboot_hw_ftbl_type *sbl_secboot_hw_ftbl_ptr = &sbl_secboot_hw_ftbl;
  
  BL_VERIFY((sbl_secboot_hw_ftbl_ptr->secboot_hw_is_auth_enabled != NULL),
             BL_ERR_NULL_PTR_PASSED );

  sec_hw_err = sbl_secboot_hw_ftbl_ptr->secboot_hw_is_auth_enabled(SECBOOT_HW_MBA_CODE_SEGMENT,
                                                                 (uint32 *)&is_auth_enabled);
  BL_VERIFY((E_SECBOOT_HW_SUCCESS == sec_hw_err),
             BL_ERR_SUBSYS_BOOT_FAIL );

  return is_auth_enabled;
}
