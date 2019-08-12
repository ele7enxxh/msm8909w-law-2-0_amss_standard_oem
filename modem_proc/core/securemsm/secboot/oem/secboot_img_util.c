/*===========================================================================

                     S E C U R I T Y    S E R V I C E S

                S E C U R E  B O O T   X.5 0 9  P A R S E R

                               M O D U L E

FILE:  secboot_util.c

DESCRIPTION:
  Secure Boot wraper function

EXTERNALIZED FUNCTIONS


Copyright (c) 2013-2014 by Qualcomm Technologies, Inc. All Rights Reserved.
===========================================================================*/

/*=========================================================================

                          EDIT HISTORY FOR FILE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/core.mpss/3.5.c12.3/securemsm/secboot/oem/secboot_img_util.c#1 $
$DateTime: 2016/12/13 07:59:23 $
$Author: mplcsds1 $

when         who                what, where, why
--------   ----               ------------------------------------------- 
05/05/14   hw                  add engg cert support
01/21/14   hw                  enable mrc signed image (with root 0) on non-secure device
01/20/14   hw                  add parm define to enable unsigned image on 8092
01/10/14   hw                  disallow unsigned image to be loaded
11/06/13   hw                  Init version.
===========================================================================*/


/*==========================================================================

           Include Files for Module

==========================================================================*/
#include "secboot_img_util.h"
#include "secboot_i.h"
#include "secboot_x509.h"

// check data + len is always smaller than the bound and no overflow
#define DATA_IN_BOUND(data, len, bound) ((data + len > data) && (data + len < bound))

#ifdef ENG_CERT_SECBOOT_BUILD
extern e_auth_img_error_type auth_image_with_eng_cert( 
    secboot_auth_image_context*  context,
    secboot_image_info_type *image_info,
    secboot_verified_info_type *verified_info
);
#endif

/**
 * @brief checks if the image is signed or not
 *
 * @param[in] image_info image information
 *
 * @return TRUE for signed image, return FALSE for unsigned image
 */
static boolean secboot_is_signed_image(secboot_image_info_type *image_info)
{
  if (image_info != NULL)
  {
    if ( (image_info->signature_ptr != NULL) &&
         (image_info->signature_len != 0) && 
         (image_info->x509_chain_ptr != NULL) && 
         (image_info->x509_chain_len != 0))
    {
      return TRUE;
    }
  }
  return FALSE;
}

/*!
* 
* @brief
*    This function searches fuse info from the image
*
* @param[in]  image_info      image information
*             
* @param[in/out]  fuse_info   fuse information from the image
* 
* @retval
*    e_auth_img_error_type Returns error code in case of authentication error
*                          Returns 0 in case of no error
* 
*/
static const uint8 hw_id_string[]="HW_ID";
static e_auth_img_error_type secboot_get_fuse_info_from_image(
  secboot_fuse_info_type *fuse_info, 
  secboot_image_info_type* image_info)
{
  //e_auth_img_error_type status = E_AUTH_IMG_FAILURE;
  secx509_errno_enum_type x509_status = E_X509_FAILURE;
  uint32 total_cert_num = 0;
  
  /* return failure, if the image pointer is NULL */
  if ( (image_info == NULL) || 
     (image_info->x509_chain_ptr == NULL) ||
     (image_info->signature_ptr == NULL) )
  {
    return E_AUTH_IMG_INVALID_PRAM;
  }
  
  /* default specific fuse information here: */
  MEMSET(fuse_info->root_of_trust, 0, sizeof(fuse_info->root_of_trust));
  
  fuse_info->use_root_of_trust_only = FALSE;
  fuse_info->auth_use_serial_num = 0;
  fuse_info->serial_num = 0;

  /* init multiple root cert fuse value  for default one root qpsa signed image */
  fuse_info->root_sel_info.is_root_sel_enabled = SECBOOT_ROOT_CERT_SEL_DISABLED;
  fuse_info->root_sel_info.root_cert_sel = 0;
  fuse_info->root_sel_info.num_root_certs = 0;

  /* MRC signed image needs to setup additional fuses. 
     MRC cert has more than one root, plus attest and subca cert. The total cert number is larger than 3.
     (This only works for the case whose cert chain has the attest-subca-root(s) 3-level structure)*/
  x509_status = secboot_cal_cert_num( image_info->x509_chain_ptr, image_info->x509_chain_len, 
                                      &total_cert_num);
  if (E_X509_SUCCESS != x509_status) 
  {
    return E_AUTH_IMG_FAILURE;
  }

  if ( total_cert_num > 3) 
  {
    fuse_info->root_sel_info.is_root_sel_enabled = SECBOOT_ROOT_CERT_SEL_ENABLED;
    /* assign the total number of root cert */
    fuse_info->root_sel_info.num_root_certs = total_cert_num - 2; 
  }

  fuse_info->msm_hw_id = 0;

  return E_AUTH_IMG_SUCCESS;
}

/*!
* 
* @brief
*    This function checks if image authentication is required or not.
*    The auth is required for both secboot-enabled device and secboot-disabled device.
*    The unsigned image is not allowed, if FEATURE_ALLOW_UNSIGNED_IMAGE is not defined.
*
* @param[in]  context              context that stores function pointers and related info.
*             image_info           image information
*
* @param[in/out]  is_auth_required   flag to indicate if authentication is required
* 
* @retval
*    e_auth_img_error_type Returns error code in case of authentication error
*                          Returns 0 in case of no error
* 
*/
static e_auth_img_error_type auth_img_is_auth_required(
  secboot_auth_image_context*  context,
  secboot_image_info_type*     image_info,
  uint32*                      is_auth_required
)
{
  secboot_hw_etype   hw_status = E_SECBOOT_HW_FAILURE;
  uint32             is_auth_enabled = FALSE;

  if ((NULL == context) || (NULL == context->secboot_hw_ftbl_ptr))
    return E_AUTH_IMG_INVALID_PRAM;

  hw_status = context->secboot_hw_ftbl_ptr->secboot_hw_is_auth_enabled(context->code_segment, &is_auth_enabled);  
  if(E_SECBOOT_HW_SUCCESS != hw_status)
  {
    return E_AUTH_IMG_HW_FAILURE;
  }

  /* secure device, auth is required */
  if (TRUE == is_auth_enabled)
  {
    *is_auth_required = TRUE;
  }
  /* non-secure device: auth signed image */
  else if ((FALSE == is_auth_enabled) && 
           (TRUE == secboot_is_signed_image(image_info)))
  {
    *is_auth_required = TRUE;
  }
  else
  {
#ifdef FEATURE_ALLOW_UNSIGNED_IMAGE
    *is_auth_required = FALSE;
    status = E_SECBOOT_SUCCESS;
#else
    /* unsigned image in non-secure device is not allowed.*/
    return E_AUTH_IMG_IMAGE_NOT_SUPPORTED;
#endif
  }

  return E_AUTH_IMG_SUCCESS;
}

/*!
* 
* @brief
*    This function authenticates the image using the prod cert fuse info. It accepts fuse
*    info provided outside the function.
*
* @param[in] code_segment         code segment.
*            image_info           secboot_image_info_type info for image
*            verified_info        secboot_verified_info_type verification info
*            secboot_ftbl_ptr     the pointer to secboot functions
*            secboot_hw_ftbl_ptr  the pointer to secboot hardware dependent functions
*            crypto_hash_ftbl_ptr the pointer to hash functions
* 
* @retval
*    e_auth_img_error_type Returns error code in case of authentication error
*                          Returns 0 in case of no error
* 
*/
static e_auth_img_error_type auth_image(
  secboot_auth_image_context*     context,
  secboot_image_info_type*        image_info,
  secboot_verified_info_type*     verified_info,
  secboot_fuse_info_type*         image_fuse_info
)
{
  e_auth_img_error_type status = E_AUTH_IMG_FAILURE;
  secboot_error_type deinit_status = E_SECBOOT_FAILURE;
       
  secboot_handle_type    sec_handle = {{0}};
   
  if ( (NULL == context)
       || (context->secboot_ftbl_ptr == NULL) 
       || (context->secboot_hw_ftbl_ptr == NULL))
  {
    return E_AUTH_IMG_INVALID_PRAM;
  }

  if ( (context->secboot_ftbl_ptr->secboot_init == NULL)
       || (context->secboot_ftbl_ptr->secboot_init_fuses == NULL)
       || (context->secboot_ftbl_ptr->secboot_authenticate == NULL)
       || (context->secboot_ftbl_ptr->secboot_deinit == NULL))
  {
    return E_AUTH_IMG_INVALID_PRAM;
  }

  /* Initializes the secboot handle and Crypto HW engine */
  status = (e_auth_img_error_type)context->secboot_ftbl_ptr->secboot_init( context->crypto_hash_ftbl_ptr, &sec_handle);
  if( E_AUTH_IMG_SUCCESS == status )
  {
    do
    {
      /* initialize fuse info if that is provided. */
      if ( NULL != image_fuse_info )
      {
        status = (e_auth_img_error_type)context->secboot_ftbl_ptr->secboot_init_fuses(&sec_handle, *image_fuse_info);
        if (E_AUTH_IMG_SUCCESS != status)
        {
          break;
        }
      }

      status = (e_auth_img_error_type)context->secboot_ftbl_ptr->secboot_authenticate( 
                                                       &sec_handle,
                                                       context->code_segment,
                                                       image_info,
                                                       verified_info);
      /* Check the result of authentication */ 
      if(E_AUTH_IMG_SUCCESS != status)
      {
        break;
      }
    }
    while(0);
      
    /* Deinitializes the secboot handle and Crypto HW engine*/
    /* deinit always be run, and don't report the status if already an error from previous function */
    deinit_status = context->secboot_ftbl_ptr->secboot_deinit(&sec_handle);
    if (E_AUTH_IMG_SUCCESS == status)
    {
      status = (e_auth_img_error_type)deinit_status;
    }
  }
    
  return status;  
}

/*===========================================================================
**  Public functions
** ==========================================================================
*/
/**
 * @brief See documentation in the header
 *
 */
e_auth_img_error_type secboot_authenticate_image_context(
  secboot_auth_image_context*  context,
  secboot_image_info_type* image_info,
  secboot_verified_info_type* verified_info
)
{
  e_auth_img_error_type  status = E_AUTH_IMG_FAILURE;
  secboot_hw_etype    hw_status = E_SECBOOT_HW_FAILURE;
  uint32       is_auth_required = FALSE;
  uint32        is_auth_enabled = FALSE;

  secboot_fuse_info_type  image_fuse_info;
  secboot_fuse_info_type*  image_fuse_info_ptr = NULL;

  MEMSET(&image_fuse_info, 0, sizeof(image_fuse_info));

  if ((NULL == context) || (NULL == image_info) || (NULL == verified_info))
  {
    return E_AUTH_IMG_INVALID_PRAM;
  }

  /* If FEATURE_ALLOW_UNSIGNED_IMAGE is not defined, unsigned image is not accepted and
     this function returns failure. If FEATURE_ALLOW_UNSIGNED_IMAGE is defined, unsigned 
     image is accepted, the call passes and is_auth_required is set to FALSE. */
  status = auth_img_is_auth_required(context, image_info, &is_auth_required);
  if (E_AUTH_IMG_SUCCESS != status)
  {
    return status;
  }

  if (is_auth_required)
  {
      /* fuse info is extracted from the image for secboot disabled device. */
      /* for secboot enabled device, fuse info is always from the qfprom hardware. */
      hw_status = context->secboot_hw_ftbl_ptr->secboot_hw_is_auth_enabled(context->code_segment, &is_auth_enabled); 
      if(E_SECBOOT_HW_SUCCESS != hw_status)
      {
        return E_AUTH_IMG_HW_FAILURE;
      }

      if ( FALSE == is_auth_enabled )
      {
        status = secboot_get_fuse_info_from_image(&image_fuse_info, image_info);
        if(E_AUTH_IMG_SUCCESS != status)
        {
          return status;
        }
        image_fuse_info_ptr = &image_fuse_info;
      }

      status = auth_image(
                context,
                image_info,
                verified_info,
                image_fuse_info_ptr
               );
      if (E_AUTH_IMG_SUCCESS == status)
      {
        /* return if auth passes with production cert*/
        return status;
      }

      /* for non-sec device: ignore the untrusted root, as oem might use oem-signed image */
      if ( ( FALSE == is_auth_enabled ) && (E_SECBOOT_UNTRUSTED_ROOT == (secboot_error_type)status) )
      {
        status = E_AUTH_IMG_SUCCESS;
        return status;
      }

#ifdef ENG_CERT_SECBOOT_BUILD
      /* try engg cert, return success if auth passes with engg cert. 
         auth_image_with_eng_cert return failure if eng cert is not accepted.
         this will use context->image_sw_id and context->pbl_reenable_debug 
         to enable image signed with engg cert
      */
      status = auth_image_with_eng_cert(
                 context,
                 image_info,
                 verified_info
               );
#endif
  }
  else /* when unsigned image is accepted and auth is not required, just return pass*/
  {
    status = E_AUTH_IMG_SUCCESS;
  }

  return status;
}

/**
 * @brief See documentation in the header
 *
 */
secboot_error_type secboot_authenticate_image(
  uint32 code_segment,
  secboot_image_info_type* image_info,
  secboot_verified_info_type* verified_info,
  secboot_ftbl_type* secboot_ftbl_ptr,
  secboot_hw_ftbl_type* secboot_hw_ftbl_ptr,
  secboot_crypto_hash_ftbl_type* crypto_hash_ftbl_ptr
)
{
  secboot_error_type status = E_SECBOOT_FAILURE;
  secboot_auth_image_context  context;
  MEMSET(&context, 0, sizeof(context));

  context.code_segment = code_segment;
  context.secboot_ftbl_ptr = secboot_ftbl_ptr;
  context.secboot_hw_ftbl_ptr = secboot_hw_ftbl_ptr;
  context.crypto_hash_ftbl_ptr = crypto_hash_ftbl_ptr;

  status = (secboot_error_type)secboot_authenticate_image_context(
                                &context,
                                image_info,
                                verified_info
                                );

  return status;
}
