/*=============================================================================

                             Boot Authenticator OEM

GENERAL DESCRIPTION
  This module is an extended version of secboot library Module
  which support eng cert based authentication.

Copyright 2014 by Qualcomm Technologies, Inc.  All Rights Reserved.
=============================================================================*/


/*=============================================================================

                            EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/core.mpss/3.5.c12.3/securemsm/secpil/oem/sec_pil_oem.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
09/09/14   hw      Initial revision.
===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "sec_pil_util.h"
#include "secboot_x509.h"
#include "sec_pil_oem.h"
#include "mba_rmb.h"

#define S_GPIO_INDEX  (10)

/*===========================================================================

                              FUNCTION DEFINITIONS

===========================================================================*/
/**
 * get the Debug_Flag from MSS PBL.
 *
 * @return  TRUE     Debug Enable in the Certificate 
 *      \c  FALSE    Debug disable in the Certifiacte
 */
int32 auth_img_oem_get_debug_state(void)
{
  pbl_modem_mba_shared_data_type *pbl_shared_data;
  pbl_secboot_shared_info_type *pbl_shared_info;
  secboot_verified_info_type *secboot_ver_info;
  uint32 debug_enable = FALSE; /*Default Debug Disable */

  do
  {
    /* Read the RMB for PBL Shared data */
    pbl_shared_data = mba_rmb_get_pbl_shared_data();
    if(pbl_shared_data == NULL)
    {
      break;
    }
    /* Retrieve the debug enable setting on cold boot */
    pbl_shared_info = (pbl_secboot_shared_info_type *)(&(pbl_shared_data-> \
                      pbl_modem_mba_secboot_shared_data));
    if(pbl_shared_info == NULL)
    {
      break;
    }
    secboot_ver_info = (secboot_verified_info_type *)(&(pbl_shared_info-> \
                       pbl_verified_info));
    if (secboot_ver_info == NULL)
    {
      break;
    }

    /* get the Debug_Enable */
    if (SECBOOT_DEBUG_ENABLE == secboot_ver_info->enable_debug)
    {
      debug_enable = TRUE;
    }
  }while(0);

  return debug_enable;
}

/*!
* 
* @brief
* Check if eng certificate based authentication of image is allowed.
*
* @param[in]  context   the context that stored function pointers and information
*
* @param[in/out]  eng_cert_allowed   TRUE -  Allow authentication with eng cert  
*                            \c  FALSE -  Eng Cert not allowed
* @retval
*    SEC_IMG_AUTH_ERROR_TYPE Returns error code in case of eng Cert Setting error
*                  Returns 0 in case of no error
* 
*/
static SEC_IMG_AUTH_ERROR_TYPE auth_img_allow_eng_cert(
  secboot_auth_image_context*  context,
  uint32 *eng_cert_allowed
)
{
  uint32 is_auth_enable = TRUE; /* Safe side auth is enabled */
  uint32 apps_jtag_fuse_enabled = 0;
  uint32 enable_debug_atcert = SECBOOT_DEBUG_DISABLE;

  /* GPIO-10: 
   low: engineering device 
   high: production device
  */
  uint32 s_gpio_pull_up = HWIO_INFI(TLMM_GPIO_IN_OUTn, S_GPIO_INDEX, GPIO_IN);

  /* Set to Not Allowed */
  *eng_cert_allowed = FALSE;
  
  if ((NULL == context) || (NULL == context->secboot_hw_ftbl_ptr))
  {
    SEC_PIL_ERROR_CODE(PIL_SEC_IMG_AUTH_INVALID_ARG);
    return SEC_IMG_AUTH_INVALID_ARG;
  }
  
  /* check if the jtag disable fuse gets blown or not*/
  apps_jtag_fuse_enabled = DEBUG_ENABLED_APPS;

  /* check if the jtage is reenabled from pbl */
  enable_debug_atcert = context->pbl_reenable_debug;
  
  /* if auth is enabled then check whether eng_cert is allowed or not */
  if(E_SECBOOT_HW_SUCCESS == context->secboot_hw_ftbl_ptr->secboot_hw_is_auth_enabled(
                               SECBOOT_HW_MSS_CODE_SEGMENT,
                               &is_auth_enable
                             ) )
  {
    if(!is_auth_enable || apps_jtag_fuse_enabled || !(s_gpio_pull_up) 
       || (SECBOOT_DEBUG_ENABLE == enable_debug_atcert))
    {
      *eng_cert_allowed = TRUE;
    }
  }
  else
  {
    SEC_PIL_ERROR_CODE(PIL_SEC_IMG_AUTH_HW_FAILURE);
    return SEC_IMG_AUTH_HW_FAILURE;
  }

  return SEC_IMG_AUTH_SUCCESS;
}

/*!
* 
* @brief
* Returns an alternative root key to be used for authentication
* of subsystem/secure app images
*
* @param[out]  fuse_info   Struct to define the OEM specific fuse
*                          information & root of trust for Eng Cert 
* 
* @retval
*    SEC_IMG_AUTH_ERROR_TYPE Returns error code in case of eng Cert Setting error
*                  Returns 0 in case of no error
*
*/
static SEC_IMG_AUTH_ERROR_TYPE auth_img_set_eng_cert(secboot_fuse_info_type *fuse_info)
{
  /* define Root of Trust for Eng Cert */
  
  /* Set Engineering  Root Cert Here */
  const uint8 root_of_trust_eng[CEML_HASH_DIGEST_SIZE_SHA256]=
  { 
   0x32, 0x36,0xe6, 0xb5,0x2c, 0xe8,0x11, 0xa8,
   0x03, 0x79,0x49, 0x73,0x94, 0x94,0x5b, 0x55,
   0xeb, 0x09,0x4d, 0x27,0x6c, 0xac,0x7a, 0x23,
   0xb3, 0x75,0x6d, 0x26,0x31, 0x78,0x61, 0x74
  };
  
  /* Set Engineering Certificate Root of trust */
  MEMSCPY(fuse_info->root_of_trust, CEML_HASH_DIGEST_SIZE_SHA256, (void*)root_of_trust_eng, CEML_HASH_DIGEST_SIZE_SHA256);
  fuse_info->use_root_of_trust_only = TRUE;

  /* Optional overrides, if use_root_of_trust_only == FALSE: */
  fuse_info->msm_hw_id = 0;
  fuse_info->auth_use_serial_num = 0;
  fuse_info->serial_num = 0;

  /* init multiple root cert fuse value */
  fuse_info->root_sel_info.is_root_sel_enabled = SECBOOT_ROOT_CERT_SEL_DISABLED;
  fuse_info->root_sel_info.root_cert_sel = 0;
  fuse_info->root_sel_info.num_root_certs = 0; 

  return SEC_IMG_AUTH_SUCCESS;
}

/*!
* 
* @brief
*    This function authenticates the appsbl image using the eng cert 
*
* @param[in] context              context that stores function pointers and related info.
*            image_info           secboot_image_info_type info for image
*            verified_info        secboot_verified_info_type verification info
* 
* @retval
*    SEC_IMG_AUTH_ERROR_TYPE Returns error code in case of authentication error
*                  Returns 0 in case of no error
* 
* @par Side Effects
*    Returns failure if engg cert is not allowed
* 
*/
SEC_IMG_AUTH_ERROR_TYPE auth_image_with_eng_cert( 
    secboot_auth_image_context*  context,
    secboot_image_info_type *image_info,
    secboot_verified_info_type *verified_info
)
{
  SEC_IMG_AUTH_ERROR_TYPE status = SEC_IMG_AUTH_FAILURE;                   
  do
  {
    secboot_fuse_info_type oem_fuse_info;
    secboot_error_type     sec_err       = E_SECBOOT_FAILURE;
    uint32                 is_eng_device = FALSE;
    secboot_handle_type    sbl_sec_handle;

    MEMSET(&oem_fuse_info, 0, sizeof(oem_fuse_info));
    MEMSET(&sbl_sec_handle, 0, sizeof(sbl_sec_handle));

    /* Input Validation */
    if( (NULL == context) ||
        (NULL == context->secboot_ftbl_ptr) ||
        (NULL == image_info) || 
        (NULL == verified_info) )
    {
      SEC_PIL_ERROR_CODE(PIL_SEC_IMG_AUTH_INVALID_ARG);
      status = SEC_IMG_AUTH_INVALID_ARG;
      break;
    }

    /* Eng Cert based authentication is allowed for LK / APPS SBL image and for 
     * both engineering device (S-Type & J-Type)
     */
    status = auth_img_allow_eng_cert(context, &is_eng_device);
    if(SEC_IMG_AUTH_SUCCESS != status)
    {
      break;
    }

    if( (SECBOOT_ENGG_CERT_SW_TYPE == context->image_sw_id) && is_eng_device)
    {
      /* Initializes the secboot handle and Crypto HW engine */
      sec_err = context->secboot_ftbl_ptr->secboot_init( NULL, &sbl_sec_handle);
    
      if( E_SECBOOT_SUCCESS != sec_err )
      {
        status = (SEC_IMG_AUTH_ERROR_TYPE)sec_err;
        break;
      }
      
      /* Get OEM given Root of Trust & initialize with OEM given Root of Trust */
      status = auth_img_set_eng_cert(&oem_fuse_info);
      if( SEC_IMG_AUTH_SUCCESS != status )
      {
        break;
      } 
      /* Direct Call, since changing secboot_ftbl_type structure will have side effects */
      sec_err = context->secboot_ftbl_ptr->secboot_init_fuses(&sbl_sec_handle, oem_fuse_info);
      if ( E_SECBOOT_SUCCESS != sec_err )
      {
        status = (SEC_IMG_AUTH_ERROR_TYPE)sec_err;
        break;
      }
      /* Authenticate using Eng Cert */
      sec_err = context->secboot_ftbl_ptr->secboot_authenticate( &sbl_sec_handle,
                                       SECBOOT_HW_APPS_CODE_SEGMENT,
                                       image_info,
                                       verified_info);
      if ( E_SECBOOT_SUCCESS != sec_err )
      {
        status = (SEC_IMG_AUTH_ERROR_TYPE)sec_err;
        break;
      }
      /* Deinitializes the secboot handle and Crypto HW engine */
      sec_err = context->secboot_ftbl_ptr->secboot_deinit(&sbl_sec_handle);
     
      if(E_SECBOOT_SUCCESS != sec_err)
      {
        status = (SEC_IMG_AUTH_ERROR_TYPE)sec_err;
        break;
      }

      status = SEC_IMG_AUTH_SUCCESS;
    }
    else /* Eng Cert Not allowed */
    {
      SEC_PIL_ERROR_CODE(PIL_SEC_IMG_AUTH_ENGG_CERT_NOT_ALLOWED);
      status = SEC_IMG_AUTH_ENGG_CERT_NOT_ALLOWED;
      break;          
    }
  }while(0);
  
  return status;  
}
