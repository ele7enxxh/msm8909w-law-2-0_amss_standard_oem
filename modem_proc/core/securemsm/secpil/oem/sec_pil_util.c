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

$Header: //components/rel/core.mpss/3.5.c12.3/securemsm/secpil/oem/sec_pil_util.c#1 $
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
#include "sec_pil_util.h"
#include "secboot_x509.h"
#include "sec_debug_policy.h"
#include "debug_policy_private.h"

// check data + len is always smaller than the bound and no overflow
#define DATA_IN_BOUND(data, len, bound) ((data + len > data) && (data + len < bound))

#ifdef ENG_CERT_SECBOOT_BUILD
extern SEC_IMG_AUTH_ERROR_TYPE auth_image_with_eng_cert( 
    secboot_auth_image_context*  context,
    secboot_image_info_type *image_info,
    secboot_verified_info_type *verified_info
);

extern int32 auth_img_oem_get_debug_state(void);
#endif

/**
 * @brief checks if the image is signed or not
 *
 * @param[in] image_info image information
 *
 * @return TRUE for signed image, return FALSE for unsigned image
 */
static boolean sec_img_auth_is_signed_image(secboot_image_info_type *image_info)
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
*    SEC_IMG_AUTH_ERROR_TYPE Returns error code in case of authentication error
*                          Returns 0 in case of no error
* 
*/
static SEC_IMG_AUTH_ERROR_TYPE sec_img_auth_get_fuse_info_from_image(
  secboot_fuse_info_type *fuse_info, 
  secboot_image_info_type* image_info)
{
  secx509_errno_enum_type x509_status = E_X509_FAILURE;
  uint32 total_cert_num = 0;
  
  /* return failure, if the image pointer is NULL */
  if ( (image_info == NULL) || 
     (image_info->x509_chain_ptr == NULL) ||
     (image_info->signature_ptr == NULL) )
  {
    SEC_PIL_ERROR_CODE(PIL_SEC_IMG_AUTH_INVALID_ARG);
    return SEC_IMG_AUTH_INVALID_ARG;
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
    SEC_PIL_ERROR_CODE(PIL_SEC_IMG_AUTH_INVALID_CAL_CERT_NUM);
    return SEC_IMG_AUTH_INVALID_CAL_CERT_NUM;
  }

  if ( total_cert_num > 3) 
  {
    fuse_info->root_sel_info.is_root_sel_enabled = SECBOOT_ROOT_CERT_SEL_ENABLED;
    /* assign the total number of root cert */
    fuse_info->root_sel_info.num_root_certs = total_cert_num - 2; 
  }

  fuse_info->msm_hw_id = 0x0;

  return SEC_IMG_AUTH_SUCCESS;
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
*    SEC_IMG_AUTH_ERROR_TYPE Returns error code in case of authentication error
*                          Returns 0 in case of no error
* 
*/
static SEC_IMG_AUTH_ERROR_TYPE auth_img_is_auth_required(
  secboot_auth_image_context*  context,
  secboot_image_info_type*     image_info,
  uint32*                      is_auth_required
)
{
  secboot_hw_etype   hw_status = E_SECBOOT_HW_FAILURE;
  uint32             is_auth_enabled = FALSE;

  if ((NULL == context) || (NULL == context->secboot_hw_ftbl_ptr))
  {
    SEC_PIL_ERROR_CODE(PIL_SEC_IMG_AUTH_INVALID_ARG);
    return SEC_IMG_AUTH_INVALID_ARG;
  }

  hw_status = context->secboot_hw_ftbl_ptr->secboot_hw_is_auth_enabled(context->code_segment, &is_auth_enabled);  
  if(E_SECBOOT_HW_SUCCESS != hw_status)
  {
    SEC_PIL_ERROR_CODE(PIL_SEC_IMG_AUTH_HW_FAILURE);
    return SEC_IMG_AUTH_HW_FAILURE;
  }

  /* secure device, auth is required */
  if (TRUE == is_auth_enabled)
  {
    *is_auth_required = TRUE;
  }
  /* non-secure device: auth signed image */
  else if ((FALSE == is_auth_enabled) && 
           (TRUE == sec_img_auth_is_signed_image(image_info)))
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
    SEC_PIL_ERROR_CODE(PIL_SEC_IMG_AUTH_IMAGE_NOT_SUPPORTED);
    return SEC_IMG_AUTH_IMAGE_NOT_SUPPORTED;
#endif
  }

  return SEC_IMG_AUTH_SUCCESS;
}

/*!
* 
* @brief
*    This function authenticates the image using the debug policy.
*
* @param[in] context              context
*            image_info           secboot_image_info_type info for image
*            verified_info        secboot_verified_info_type verification info
* 
* @retval
*    SEC_IMG_AUTH_ERROR_TYPE Returns error code in case of authentication error
*                          Returns 0 in case of no error
* 
*/
SEC_IMG_AUTH_ERROR_TYPE auth_image_against_debug_policy(
  secboot_auth_image_context*     context,
  secboot_image_info_type*        image_info,
  secboot_verified_info_type*     verified_info
)
{
  secboot_error_type status = E_SECBOOT_FAILURE;
  secboot_error_type deinit_status = E_SECBOOT_FAILURE;
  secboot_handle_type    sec_handle;
  secboot_fuse_info_type fuse_info;
  secboot_fuse_info_type dbg_policy_fuse_info;
  dbg_policy_t *dbgp_ptr = get_dbg_policy();
  uint32 rootCount=0;

  if(NULL == dbgp_ptr)
  {
      return SEC_IMG_AUTH_FAILURE;
  }

  MEMSET(&sec_handle, 0, sizeof(sec_handle)); 
  MEMSET(&dbg_policy_fuse_info, 0, sizeof(dbg_policy_fuse_info)); 

  /* Initializes the secboot handle and Crypto HW engine */
  status = context->secboot_ftbl_ptr->secboot_init( context->crypto_hash_ftbl_ptr, &sec_handle);
  if( E_SECBOOT_SUCCESS == status )
  {
   status = context->secboot_ftbl_ptr->secboot_read_fuses(&sec_handle, context->code_segment, &fuse_info);
   if (E_SECBOOT_SUCCESS != status)
   {
 	 return SEC_IMG_AUTH_FAILURE;
   }

   for ( rootCount = 0; rootCount < dbgp_ptr->root_cert_hash_count; rootCount++ )
   {
       memscpy( (uint8 *)&dbg_policy_fuse_info.root_of_trust[0], CEML_HASH_DIGEST_SIZE_SHA256, (uint8 *)&dbgp_ptr->root_cert_hash_array[rootCount][0], CEML_HASH_DIGEST_SIZE_SHA256);

       dbg_policy_fuse_info.msm_hw_id = fuse_info.msm_hw_id;
       dbg_policy_fuse_info.auth_use_serial_num = 0;
       dbg_policy_fuse_info.serial_num = fuse_info.serial_num;

       dbg_policy_fuse_info.root_sel_info.is_root_sel_enabled = SECBOOT_ROOT_CERT_SEL_ENABLED; 
       dbg_policy_fuse_info.root_sel_info.root_cert_sel = 0; 
       dbg_policy_fuse_info.root_sel_info.num_root_certs = 1; 

       status = context->secboot_ftbl_ptr->secboot_init_fuses(&sec_handle, dbg_policy_fuse_info);
       if (E_SECBOOT_SUCCESS != status)
       {
         break;
       }

      status = context->secboot_ftbl_ptr->secboot_authenticate( 
                                                       &sec_handle,
                                                       context->code_segment,
                                                       image_info,
                                                       verified_info);
      /* Check the result of authentication */ 
      if(E_SECBOOT_SUCCESS == status)
      {
        break;
      }
    }

   
    /* Deinitializes the secboot handle and Crypto HW engine*/
    /* deinit always be run, and don't report the status if already an error from previous function */
    deinit_status = context->secboot_ftbl_ptr->secboot_deinit(&sec_handle);
    if (E_SECBOOT_SUCCESS == status)
    {
      status = deinit_status;
    }
  }
    
  return E_SECBOOT_SUCCESS == status
         ? SEC_IMG_AUTH_SUCCESS
         : SEC_IMG_AUTH_FAILURE;
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
*    SEC_IMG_AUTH_ERROR_TYPE Returns error code in case of authentication error
*                          Returns 0 in case of no error
* 
*/
static SEC_IMG_AUTH_ERROR_TYPE auth_image(
  secboot_auth_image_context*     context,
  secboot_image_info_type*        image_info,
  secboot_verified_info_type*     verified_info,
  secboot_fuse_info_type*         image_fuse_info
)
{
  SEC_IMG_AUTH_ERROR_TYPE status = SEC_IMG_AUTH_FAILURE;
  secboot_error_type deinit_status = E_SECBOOT_FAILURE;
       
  secboot_handle_type    sec_handle;
  
  MEMSET(&sec_handle, 0, sizeof(sec_handle)); 

  if ( (NULL == context)
       || (context->secboot_ftbl_ptr == NULL) 
       || (context->secboot_hw_ftbl_ptr == NULL))
  {
    SEC_PIL_ERROR_CODE(PIL_SEC_IMG_AUTH_INVALID_ARG);
    return SEC_IMG_AUTH_INVALID_ARG;
  }

  if ( (context->secboot_ftbl_ptr->secboot_init == NULL)
       || (context->secboot_ftbl_ptr->secboot_init_fuses == NULL)
       || (context->secboot_ftbl_ptr->secboot_authenticate == NULL)
       || (context->secboot_ftbl_ptr->secboot_deinit == NULL))
  {
    SEC_PIL_ERROR_CODE(PIL_SEC_IMG_AUTH_INVALID_ARG);
    return SEC_IMG_AUTH_INVALID_ARG;
  }

  /* validate debug policy inputs and image ID array */
  if ( is_dbg_policy_rot_for_image( image_info->sw_type ) )
  {
      return auth_image_against_debug_policy(context, image_info, verified_info);
  }

  /* Initializes the secboot handle and Crypto HW engine */
  status = (SEC_IMG_AUTH_ERROR_TYPE)context->secboot_ftbl_ptr->secboot_init( context->crypto_hash_ftbl_ptr, &sec_handle);
  if( SEC_IMG_AUTH_SUCCESS == status )
  {
    do
    {
      /* initialize fuse info if that is provided. */
      if ( NULL != image_fuse_info )
      {
        status = (SEC_IMG_AUTH_ERROR_TYPE)context->secboot_ftbl_ptr->secboot_init_fuses(&sec_handle, *image_fuse_info);
        if (SEC_IMG_AUTH_SUCCESS != status)
        {
          break;
        }
      }

      status = (SEC_IMG_AUTH_ERROR_TYPE)context->secboot_ftbl_ptr->secboot_authenticate( 
                                                       &sec_handle,
                                                       context->code_segment,
                                                       image_info,
                                                       verified_info);
      /* Check the result of authentication */ 
      if(SEC_IMG_AUTH_SUCCESS != status)
      {
        break;
      }
    }
    while(0);
      
    /* Deinitializes the secboot handle and Crypto HW engine*/
    /* deinit always be run, and don't report the status if already an error from previous function */
    deinit_status = context->secboot_ftbl_ptr->secboot_deinit(&sec_handle);
    if (SEC_IMG_AUTH_SUCCESS == status)
    {
      status = (SEC_IMG_AUTH_ERROR_TYPE)deinit_status;
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
SEC_IMG_AUTH_ERROR_TYPE sec_img_auth_authenticate_image_context(
  secboot_auth_image_context*  context,
  secboot_image_info_type* image_info,
  secboot_verified_info_type* verified_info
)
{
  SEC_IMG_AUTH_ERROR_TYPE  status = SEC_IMG_AUTH_FAILURE;
  secboot_hw_etype    hw_status = E_SECBOOT_HW_FAILURE;
  uint32       is_auth_required = FALSE;
  uint32        is_auth_enabled = FALSE;

  secboot_fuse_info_type  image_fuse_info;
  secboot_fuse_info_type*  image_fuse_info_ptr = NULL;

  MEMSET(&image_fuse_info, 0, sizeof(image_fuse_info));

  if ((NULL == context) || (NULL == image_info) || (NULL == verified_info))
  {
    SEC_PIL_ERROR_CODE(PIL_SEC_IMG_AUTH_INVALID_ARG);
    return SEC_IMG_AUTH_INVALID_ARG;
  }

  if ((context->secboot_ftbl_ptr == NULL) 
       || (context->secboot_hw_ftbl_ptr == NULL)
       || (context->secboot_hw_ftbl_ptr->secboot_hw_is_auth_enabled == NULL) )
  {
    SEC_PIL_ERROR_CODE(PIL_SEC_IMG_AUTH_INVALID_ARG);
    return SEC_IMG_AUTH_INVALID_ARG;
  }

  /* If FEATURE_ALLOW_UNSIGNED_IMAGE is not defined, unsigned image is not accepted and
     this function returns failure. If FEATURE_ALLOW_UNSIGNED_IMAGE is defined, unsigned 
     image is accepted, the call passes and is_auth_required is set to FALSE. */
  status = auth_img_is_auth_required(context, image_info, &is_auth_required);
  if (SEC_IMG_AUTH_SUCCESS != status)
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
        SEC_PIL_ERROR_CODE(PIL_SEC_IMG_AUTH_HW_FAILURE);
        return SEC_IMG_AUTH_HW_FAILURE;
      }

      if ( FALSE == is_auth_enabled )
      {
        status = sec_img_auth_get_fuse_info_from_image(&image_fuse_info, image_info);
        if(SEC_IMG_AUTH_SUCCESS != status)
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
      if (SEC_IMG_AUTH_SUCCESS == status)
      {
        /* return if auth passes with production cert*/
        return status;
      }

      if ( (is_dbg_policy_rot_for_image(image_info->sw_type)) )
      {
       return status;
      }	  

      /* for non-sec device: ignore the untrusted root, as oem might use oem-signed image */
      if ( ( FALSE == is_auth_enabled ) && (E_SECBOOT_UNTRUSTED_ROOT == (secboot_error_type)status) )
      {
        status = SEC_IMG_AUTH_SUCCESS;
        return status;
      }

#ifdef ENG_CERT_SECBOOT_BUILD
      /* update modem pbl debug re-enable flag, which is turned on for engg device
      */
      if ( TRUE == auth_img_oem_get_debug_state())
      {
        context->pbl_reenable_debug = SECBOOT_DEBUG_ENABLE;
      }
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
    status = SEC_IMG_AUTH_SUCCESS;
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

  status = (secboot_error_type)sec_img_auth_authenticate_image_context(
                                &context,
                                image_info,
                                verified_info
                                );

  return status;
}
