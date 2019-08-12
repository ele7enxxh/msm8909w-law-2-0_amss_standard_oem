/**
* @file sec_pil_auth.c
* @brief Secure PIL Auth Security implementation
*
* This file implements the Secure Authentication Routines,
* including signature validation and hash validation for the
* hash table segment, elf header and program header table
* segment
*
*/
/*===========================================================================
   Copyright (c) 2012 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                            EDIT HISTORY FOR FILE

  $Header: //components/rel/core.mpss/3.5.c12.3/securemsm/secpil/src/sec_pil_oem_auth.c#1 $
  $DateTime: 2016/12/13 07:59:23 $
  $Author: mplcsds1 $


when       who      what, where, why
--------   ---      ------------------------------------
10/04/13   dm       Initial Version.
===========================================================================*/

/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/
#include <string.h> 
#include <comdef.h>
#include <IxErrno.h>
#include "mba_rmb.h"
#include "HALhwio.h"
#include "mba_hwio.h"
#include "sec_pil.h"
#include "sec_pil_env.h"
#include "sec_pil_priv.h"
#include "secboot.h"
#include "secboot_hw.h"
#include "CeML.h"
#include <stringl.h>


#define PIL_SECBOOT_ALLOW_ENG_CERT_FAILED 0xFD
#define PIL_SECBOOT_INIT_FUSES_FAILED     0xFE 
#define PIL_SECBOOT_GET_CERT_FAILED       0xFF 
#define SEC_BOOT_MSS_SW_TYPE 2

/*----------------------------------------------------------------------------
 *  Function Declarations & Documentation 
 * -------------------------------------------------------------------------*/
/**
 * Returns an alternative root key to be used for authentication
 * of subsystem/secure app images
 *
 * @param[out]  fuse_info   Struct to define the OEM specific fuse
 *                          information
 *
 * @return E_SUCCESS - for no error
 *             E_FAILURE - for error
 */
static uint32 sec_pil_oem_set_eng_cert(secboot_fuse_info_type *fuse_info)
{
  /* define Root of Trust for Eng Cert */
  /* Engineering Root of Trust 
   * a256e4a3af72a36ec51e13d5de8f455ffb2101ec12c4e43d169e24eff35aff63
   */
  const uint8 root_of_trust_eng[CEML_HASH_DIGEST_SIZE_SHA256]=
  { 
    0xa2, 0x56, 0xe4, 0xa3, 0xaf, 0x72, 0xa3, 0x6e, 
    0xc5, 0x1e, 0x13, 0xd5, 0xde, 0x8f, 0x45, 0x5f, 
    0xfb, 0x21, 0x01, 0xec, 0x12, 0xc4, 0xe4, 0x3d,
    0x16, 0x9e, 0x24, 0xef, 0xf3, 0x5a, 0xff, 0x63
  };
  
  /* Set Engineering Certificate Root of trust */
  memscpy(fuse_info->root_of_trust,CEML_HASH_DIGEST_SIZE_SHA256, (void*)root_of_trust_eng, CEML_HASH_DIGEST_SIZE_SHA256);
  fuse_info->use_root_of_trust_only = TRUE;

  /* Optional overrides, if use_root_of_trust_only == FALSE: */
  fuse_info->msm_hw_id = 0;
  fuse_info->auth_use_serial_num = 0;
  fuse_info->serial_num = 0;

  return E_SUCCESS;
}

/**
 * get the Debug_Flag from MSS PBL.
 *
 * @return  TRUE     Debug Enable in the Certificate 
 *      \c  FALSE    Debug disable in the Certifiacte
 */
static inline uint32 sec_pil_oem_get_debug_state(void)
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

/**
 * Check if eng certificate based authentication of image is allowed.
 *
 * @param[out]  eng_cert_allowed  TRUE      Allow authentication with eng cert 
 *                \c  FALSE    Eng Cert not allowed
 *
 * @return E_SUCCESS - for no error
 *             E_FAILURE - for error
 */
static inline uint32 sec_pil_oem_allow_eng_cert(uint32 *eng_cert_allowed)
{
  uint32 is_auth_enable = TRUE; /* Safe side auth is enabled */
  uint32 debug_fuse = 0;

  /* Get the S-GPIO 92 value */
  uint32 s_gpio_pull_up = HWIO_INFI(TLMM_GPIO_IN_OUTn, 92, GPIO_IN); 

  *eng_cert_allowed = FALSE; /* Eng Cert not allowed */

  /* Error can be ignored since default value set to AUTH Enable */
  if (E_SECBOOT_HW_SUCCESS != secboot_hw_is_auth_enabled(
                             SECBOOT_HW_MSS_CODE_SEGMENT,&is_auth_enable))
  {
    return -E_FAILURE;
  }
  /* Read the debug fuses */                             
  debug_fuse = (HWIO_INF(OEM_CONFIG0, ALL_DEBUG_DISABLE) |
                HWIO_INF(OEM_CONFIG0, MSS_NIDEN_DISABLE) |
                HWIO_INF(OEM_CONFIG0, MSS_DBGEN_DISABLE) |
                HWIO_INF(FEATURE_CONFIG2, QC_MSS_NIDEN_DISABLE) |
                HWIO_INF(FEATURE_CONFIG2, QC_MSS_DBGEN_DISABLE));                         

  if(!is_auth_enable || !debug_fuse || !s_gpio_pull_up 
      || sec_pil_oem_get_debug_state())
  {
    *eng_cert_allowed = TRUE; /*Eng Cert Allowed */
  }

  return E_SUCCESS;
                               
}

int sec_pil_auth_with_eng_cert(secboot_crypto_hash_ftbl_type *crypto_ftbl_ptr,
                               secboot_handle_type  *sec_handle_ptr,
                               secboot_image_info_type *image_info_ptr,
                               secboot_verified_info_type *verified_info_ptr,
                               uint32 sw_type, uint32 code_seg)
{
  secboot_fuse_info_type oem_fuse_info;
  uint32 is_eng_cert_allowed = 0;
  secboot_error_type sec_err;
  
  if(E_SUCCESS != sec_pil_oem_allow_eng_cert(&is_eng_cert_allowed))
  {
    SEC_PIL_ERROR_CODE(PIL_SECBOOT_ALLOW_ENG_CERT_FAILED);
    return -E_FAILURE;
  }
  /* Root Cert Verification failed. Falling back to Eng Cert Verification. 
   *  Eng Cert is allowed for Modem Image and for both S/J -Type Device
   */	
  if(SEC_BOOT_MSS_SW_TYPE == sw_type && is_eng_cert_allowed)
  {
    /* Init again for initiating Eng-Cert authentication */
    sec_err = secboot_init( crypto_ftbl_ptr, sec_handle_ptr);
    if ( E_SECBOOT_SUCCESS != sec_err )
    {
      SEC_PIL_ERROR_CODE(PIL_SECBOOT_INIT_FAILED);
      return -E_FAILURE;
    }

    /* Intialize with OEM given Root of Trust */
    if(E_SUCCESS != sec_pil_oem_set_eng_cert(&oem_fuse_info))
    {
      SEC_PIL_ERROR_CODE(PIL_SECBOOT_INIT_FUSES_FAILED);
      return -E_FAILURE;        
    }
    sec_err = secboot_init_fuses( sec_handle_ptr, oem_fuse_info);
    if ( E_SECBOOT_SUCCESS != sec_err )
    {
      SEC_PIL_ERROR_CODE(PIL_SECBOOT_INIT_FUSES_FAILED);
      return -E_FAILURE;
    }

    /* Authenticate using Eng Cert */
    sec_err = secboot_authenticate(sec_handle_ptr,
                       code_seg, image_info_ptr, verified_info_ptr);
    if ( E_SECBOOT_SUCCESS != sec_err )
    {    
      SEC_PIL_ERROR_CODE(PIL_SECBOOT_AUTHENTICATE_FAILED);
      return -E_FAILURE;
    }
    /* De-Init Sec handle / Crypto driver */
    sec_err = secboot_deinit(sec_handle_ptr);
    if ( E_SECBOOT_SUCCESS != sec_err )
    {
      SEC_PIL_ERROR_CODE(PIL_SECBOOT_DEINIT_FAILED);
      return -E_FAILURE;
    }
  }
  else /* Eng Cert not allowed */
  {
    SEC_PIL_ERROR_CODE(PIL_SECBOOT_AUTHENTICATE_FAILED);
    return -E_FAILURE;
  }
  
  return E_SUCCESS;
}


