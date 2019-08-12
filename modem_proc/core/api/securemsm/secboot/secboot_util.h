#ifndef SECBOOT_UTIL_H
#define SECBOOT_UTIL_H

/*****************************************************************************
*
* @file  secboot_util.h (Wrapper for Secboot Authentication API)
*
* @brief This header files contains the types and API to authenticate an
*        image based on certificates and signature. 
* 
* @details
* 
* @note  
*     Example usecase for secure EFS tar feature:
*     //Here it is declared as static.Alternatively caller can allocate memory for this handle
*     //and after the function call can deallocate this memory 
*     static secboot_util_handle_ptr   secboot_handle_ptr;
*     secboot_util_fuse_info_type      fuse_info;
*     secboot_util_verified_info_type  verified_info_ptr;
*     
*     memset(&fuse_info, 0, sizeof(fuse_info));
*     fuse_info.auth_use_serial_num = 0;
*     fuse_info.msm_hw_id = 0x0000000000000000;
*     fuse_info.root_of_trust = root_of_trust_buf;//supply the root of trust here
*     fuse_info.root_of_trust_len = 32;
*     //if we are signing the image with debug enable option then serial number 
*     //has to be supplied.For Secure EFS tar feature this is not applicable.So they can 
*     //give serial number as zero.
*     fuse_info.serial_num = 0x0; 
*     fuse_info.use_root_of_trust_only = FALSE;
*     fuse_info.use_supplied_fuses = TRUE;
*     
*     
*     ret_value = secboot_util_auth_img_verify
*                 (
*                   &secboot_handle_ptr,
*                   signed_img_ptr,
*                   signed_img_len,  
*                   SECBOOT_UTIL_HW_MSS_CODE_SEGMENT,
*                   0x0000000000000000, //This needs to be the value with which image is signed.
*                   &fuse_info,
*                   &verified_info_ptr
*                 );
*     
*     if(ret_value == FALSE)
*     {
*       //authentication failure.Dont execute/use the supplied image.
*     }
*     
*     Example use case for binary modem feature:
*     uint32 auth_enabled;
*     
*     ret_value = secboot_util_hw_is_auth_enabled(SECBOOT_UTIL_HW_MSS_CODE_SEGMENT, &auth_enabled);
*     if(ret_value != E_SUCCESS)
*      return E_FAILURE;
*     
*     //If authentication is enbaled then call secboot_util_auth_img_verify() to authenticate the image.
*     if(auth_enabled)
*     {
*       memset(&fuse_info, 0, sizeof(fuse_info));
*       fuse_info.use_supplied_fuses = FALSE;  
*       
*       ret_value = secboot_util_auth_img_verify
*                   (
*                     &secboot_handle_ptr,
*                     signed_img_ptr,
*                     signed_img_len,  
*                     SECBOOT_UTIL_HW_MSS_CODE_SEGMENT,
*                     0x0000000000000002, //Upper 32 bits contains version of the image 
*                                         // which has to be read from corresponding fuses if allocated
*                                         // for the image.Lower 32 bit contains sw type.
*                                         // For modem subsytem 0x2 is allocated.
*                     NULL,
*                     &verified_info_ptr
*                  );
*       
*       if(ret_value == FALSE)
*       {
*          //authentication failure.Dont execute/use the supplied image
*       }
*     }
* 
*    Copyright (c) 2012 Qualcomm Technologies, Inc.  All Rights Reserved.
*    Qualcomm Technologies Proprietary and Confidential.
*
*****************************************************************************/


/*===========================================================================

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$PVCSPath:$
$Header: //components/rel/core.mpss/3.5.c12.3/api/securemsm/secboot/secboot_util.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
10/19/2012 vg       Initial API - based on old api with some changes
============================================================================*/

/** @ingroup SecureMSM
 *  @{
 */

/** @defgroup SecbootAuthentication Secboot Authentication Library */

/**
 * @addtogroup SecbootAuthentication
 *
 * @{
 */


/*===========================================================================
 
                           INCLUDE FILES

===========================================================================*/

#include "comdef.h"

/*===========================================================================
                      PUBLIC DATA DECLARATIONS
===========================================================================*/


/**
 * @brief Secboot handle
 */
typedef struct secboot_util_handle_type
{
    uint32 handle[128]; /**< 512 byte buffer needed by secboot for it's operations */
}secboot_util_handle_type;

/* store the root cert selection information */
typedef struct secboot_util_root_cert_fuse_info_type
{
  uint32   is_root_sel_enabled;  /* Is Root Cert Selection enabled */
  uint32   root_cert_sel;        /* valid only if root cert selection is enabled */
  uint32   num_root_certs;       /* valid only if root cert selection is enabled */
} secboot_util_root_cert_fuse_info_type;

/**
 * @brief Structure to store fuses related information.
 */
typedef struct
{
  uint8        *root_of_trust;
  uint32       root_of_trust_len;
  uint64       msm_hw_id;
  uint32       auth_use_serial_num;
  uint32       serial_num; 
  boolean      use_root_of_trust_only;
  secboot_util_root_cert_fuse_info_type    root_sel_info;
} secboot_util_fuse_info_type;

/**
 * @brief Data returned from a successful authentication.
 */
typedef struct secboot_util_verified_info_type
{
  uint64       sw_id;        /**< The software id (upper 32 bits:version, lower 32 bits:type)
                                  the image was signed with */
  uint32       img_offset;
  uint32       img_len;
} secboot_util_verified_info_type;

/**
 * @brief Below comments are applicable for header of EFS tar file format or else
 *        this strcuture will be treated as mbn header.
 */
typedef struct
{
  uint32 image_id;           /* This will be IMG_GEN(value 27) */
  uint32 header_vsn_num;     /* version.Security code does not use this */
  uint32 image_src;          /* Offset to beginning of image starting from end 
                                of header. */
  uint8* image_dest_ptr;     /* This will be set to NULL */
  uint32 image_size;         /* Size of complete signed image(img+sig+certs) 
                                in bytes excluding the header */
  uint32 code_size;          /* Size of image in bytes */
  uint8* signature_ptr;      /* Offset to images attestation signature starting 
                                from end of header */
  uint32 signature_size;     /* Size of the attestation signature in bytes */
  uint8* cert_chain_ptr;     /* Offset to chain of attestation certs starting 
                                from end of header */
  uint32 cert_chain_size;    /* Size of the attestation chain in bytes */
} secboot_util_auth_img_hdr_t;

/**
 * @brief Length of a SHA256 hash.
 */
#define SHA256_HASH_LENGTH 32

/**
 Identifies the secure boot fuses which represent the
 authentication information for the code.
 */
#define SECBOOT_UTIL_HW_APPS_CODE_SEGMENT   1 /**< Code segment in SECURE_BOOTn register */
                                              /**< representing authentication information. */
                                              /**< for the application processors images */     
#define SECBOOT_UTIL_HW_MBA_CODE_SEGMENT    2 /**< Code segment in SECURE_BOOTn register */
                                              /**< representing authentication information. */
                                              /**< for the Modem Boot Authentication (MBA) image */     
#define SECBOOT_UTIL_HW_MSS_CODE_SEGMENT    3 /**< Code segment in SECURE_BOOTn register */
                                              /**< representing authentication information. */
                                              /**< for the modem image */     


/**
 * @brief This function authenticates an image.
 *
 * @param[in,out] secboot_handle_ptr Pointer to the secboot handle. The handle
 *                                   must be allocated in protected memory
 * @param[in]     signed_img_ptr     Pointer to signed image.
 * @param[in]     signed_img_len     Length of the signed image.
 * @param[in]     code_segment       Code segment in SECURE_BOOTn fuse register
 *                                   containing authentication settings for
 *                                   the image.Length of the signed image
 * @param[in]     sw_id              The software id (upper 32 bits:version, lower 32 bits:type)
 *                                   the image was signed with. 
 * @param[in]     fuse_info_ptr      Contains fuse information.Caller can pass as NULL
 *                                   if they dont need to pass fuse info.
 * @param[in,out] verified_info_ptr  Pointer to data that was processed during
 *                                   authentication. Caller can use this data
 *                                   if the function returned success.Caller can pass
 *                                   NULL if they dont need this info.
 * 
 * @return TRUE on success and FALSE on Failure.
 *
 * @dependencies Call this function if the secboot fuse is blown.
 *
 * @sideeffects  none  
 *
 * @see none
 *
 */
boolean secboot_util_auth_img_verify
(
  secboot_util_handle_type            *secboot_handle_ptr,
  const uint8                         *signed_img_ptr,
  uint32                               signed_img_len,  
  uint32                               code_segment,
  uint64                               sw_id,  
  const secboot_util_fuse_info_type   *fuse_info_ptr,
  secboot_util_verified_info_type     *verified_info_ptr
);


/**
 * @brief This function checks if the image associated with the code segment
 *        needs to be authenticated. If authentication is required, callers
 *        MUST authenticate the image successfully before allowing it to execute.
 *
 * @param[in]     code_segment       Code segment in SECURE_BOOTn register
 *                                   containing authentication information
 *                                   of the image.
 * @param[in,out] auth_enabled_ptr   Pointer to a uint32 indicating whether
 *                                   authentication is required. Will be
 *                                   populated to 0 if authentication
 *                                   is not required, 1 if authentication
 *                                   is required.
 *         
 * @return E_SECBOOT_HW_SUCCESS on success. Appropriate error code on failure. 
 *         Caller's must NOT allow execution to continue if a failure is returned.
 *
 * @dependencies None
 *
 * @sideeffects  None
 *
 * @see Security Control HDD and SWI for SECURE_BOOT fuses
 *
 */
uint32 secboot_util_hw_is_auth_enabled
(
  uint32    code_segment,
  uint32   *auth_enabled_ptr
);


/// @}
//
/// @}
#endif /* SECBOOT_UTIL_H */

