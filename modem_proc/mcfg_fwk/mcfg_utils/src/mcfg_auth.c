/*==============================================================================

                M O D E M   C O N F I G   A U T H
 
GENERAL DESCRIPTION
  Auth library for MCFG

Copyright (c) 2012-2013 by QUALCOMM Technologies Incorporated.
==============================================================================*/

/*==============================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mcfg_fwk/mcfg_utils/src/mcfg_auth.c#1 $ 
$DateTime: 2016/12/13 08:00:27 $ 
$Author: mplcsds1 $ 
$Change: 11985219 $ 

when     who  what, where, why
-------- ---  ----------------------------------------------------------------
05/08/12 cy   Initial creation

==============================================================================*/

#include "comdef.h"

#include "msg.h"

#include "mcfg_common.h"
#include "mcfg_utils.h"
#include "mcfg_utils_i.h"
#include "mcfg_auth_i.h"
#include "mcfg_load.h"

#include "secapi.h"
#include "secboot_util.h"
#include <IxErrno.h>

/*==============================================================================

                  DEFINITIONS AND DECLARATIONS FOR MODULE

==============================================================================*/
/*==============================================================================
  Constants and Macros
==============================================================================*/
/*---------------------------------------------------------------------------
  MCFG SW TYPE used for CSMS signing, identically to
  SECBOOT_AMSS_HASH_TABLE_SW_TYPE
---------------------------------------------------------------------------*/
#define MCFG_AMSS_HASH_TABLE_SW_TYPE  2

#ifdef FEATURE_QDST_SECBOOT
#define MCFG_SHA256_HASH_LEN  32

static const uint8 qd_certs_root_of_trust[MCFG_SHA256_HASH_LEN] = {
  0xd5, 0x85, 0x22, 0xcd, 0x60, 0x2a, 0x50, 0x1c, 0x3a, 0x79, 0x8d, 0x20,
  0x15, 0x33, 0xae, 0x31, 0xf9, 0x24, 0x27, 0x2b, 0x0b, 0x76, 0xf1, 0x59,
  0xfe, 0x81, 0x75, 0x3b, 0x5f, 0xcf, 0xce, 0x60
};
#endif

/*==============================================================================
  Typedefs
==============================================================================*/
typedef struct
{
  uint32 image_id;           /* This will be IMG_GEN(value 27) */
  uint32 header_vsn_num;     /* Use version 0x4? Security code does not use this */
  uint32 image_src;          /* Offset to beginning of image starting from end of header. So this will always be zero */
  uint8* image_dest_ptr;     /* This will be set to NULL */
  uint32 image_size;         /* Size of complete signed image(img+sig+certs) in bytes excluding the header */
  uint32 code_size;          /* Size of tar image in bytes */
  uint8* signature_ptr;      /* Offset to images attestation signature starting from end of header */
  uint32 signature_size;     /* Size of the attestation signature in bytes */
  uint8* cert_chain_ptr;     /* Offset to chain of attestation certs starting from end of header */
  uint32 cert_chain_size;    /* Size of the attestation chain in bytes */
} mcfg_auth_img_hdr_t;

/*==============================================================================
  Variables
==============================================================================*/

/*============================================================================== 
 
                  PRIVATE FUNCTION DEFINITIONS FOR MODULE
 
==============================================================================*/

/*===========================================================================

  FUNCTION mcfg_auth_verify_hash

  DESCRIPTION
    Verfies the hash against the data segment

  DEPENDENCIES
    None

  PARAMETERS
    config [in] Pointer to signed image
    
  RETURN VALUE
    TRUE if hash matches, FALSE if it doesn't

  SIDE EFFECTS
    None

===========================================================================*/
static boolean mcfg_auth_verify_hash
(
  uint8 *p_img,
  uint32 sig_offset, 
  uint32 data_offset, 
  uint32 data_seg_size
)
{
  secerrno_enum_type          stat;
  secapi_handle_type          sh = NULL;
  secapi_hsh_param_data_type  hash_param;
  boolean                     error;

  secapi_algo_enum_type algo_type = SECAPI_SHA256;
  uint32 algo_digest_size = SECAPI_HSH_SHA256_DIGEST_SIZE;
  unsigned char digest_out[SECAPI_HSH_SHA256_DIGEST_SIZE];
  unsigned char *digest_in;
  boolean retVal = FALSE;

  if (MCFG_INTERGER_OVERFLOWED(p_img, sig_offset))
  {
    MCFG_MSG_ERROR_2("mcfg_auth_verify_hash() invalid start %p offset %lx",
                     p_img, sig_offset);
    return FALSE;
  }

  MCFG_MSG_LOW_3("mcfg_auth_verify_hash() sig_offset 0x%4x, data_offset 0x%4x"\
                 " size %d", sig_offset, data_offset, data_seg_size);

  memset((void *)digest_out, 0, sizeof(digest_out));
  digest_in = p_img + sig_offset + sizeof(mcfg_auth_img_hdr_t) + 
              2*algo_digest_size;

  stat = secapi_new(&sh, algo_type);
  if ( stat )
  {
      MSG_ERROR("HASH SHA: failed secapi handle", 0, 0, 0);
      return FALSE;
  }

  error = FALSE;

  do
  {
      hash_param.mode = SECAPI_HSH_INPUT_MODE_ATOMIC;
      stat = secapi_hsh_set_param( sh,
                                   algo_type,
                                   SECAPI_HSH_INPUT_MODE,
                                   &hash_param );
      if ( stat )
      {
          MSG_ERROR("HASH SHA: set param failure", 0, 0, 0);
          break;
      }

      hash_param.common.exec_platform = (data_seg_size < 0x1000) 
        ? SECAPI_EXEC_PLATFORM_ARM
        : SECAPI_EXEC_PLATFORM_ACCELERATOR_PREFERRED;
      stat = secapi_hsh_set_param( sh,
                                   algo_type,
                                   SECAPI_EXEC_PLATFORM,
                                   &hash_param );
      if ( stat )
      {
          MSG_ERROR("HASH SHA: set param failure", 0, 0, 0);
          break;
      }

      /* compute digest, and append result to seg buf */
      stat = secapi_hsh_create_digest( sh,
                                       algo_type,
                                       p_img+data_offset,
                                       data_seg_size,
                                       digest_out );
      if ( stat )
      {
          MSG_ERROR("HASH SHA: failure", 0, 0, 0);
          error = TRUE;
          break;
      }
  } while ( 0 );

  /* clean up */
  stat = secapi_delete(&sh);
  if ( stat || error )
  {
      MSG_ERROR("HASH SHA: secapi delete error", 0, 0, 0);    
      return FALSE;
  }

  if (memcmp(digest_in, digest_out, algo_digest_size) != 0)
  {
    MCFG_MSG_ERROR_2("mcfg_auth_verify_hash() hash doesn't match!! 0x%2x 0x%2x", 
                     *digest_in, digest_out[0]);
  }
  else
  {
    MCFG_MSG_LOW("mcfg_auth_verify_hash() hash matches!!");
    retVal = TRUE;
  }

  return retVal;
} /* mcfg_auth_verify_hash */

/*===========================================================================

  FUNCTION mcfg_auth_img_verify

  DESCRIPTION
    Parses the signed image for secure header, image, signature and
    certificates.
    Verfies the signature of passed in image against passed in attestation
    certificate.
    This function should be replaced with secapi_util_auth_img_verify() once
    that API takes image_dest_ptr to compute signature/cert offsets

  DEPENDENCIES
    None

  PARAMETERS
    config [in] Pointer to signed image
    
  RETURN VALUE
    TRUE - If Image signature and certificate chain are valid. \n
    FALSE - Signature validation failed or cert chain is not valid

  SIDE EFFECTS
    None

===========================================================================*/
static boolean mcfg_auth_img_verify
(
  uint8* signed_img_ptr,
  uint32 signed_img_len
)
{
  static secboot_util_handle_type  secboot_handle_ptr;
  boolean                          retVal = FALSE;
  uint32 auth_enabled;
#ifdef FEATURE_QDST_SECBOOT
  secboot_util_fuse_info_type      fuse_info;
#endif
   
  MCFG_CHECK_NULL_PTR_RET_FALSE(signed_img_ptr);

  if(E_SUCCESS != secboot_util_hw_is_auth_enabled(
       SECBOOT_UTIL_HW_MSS_CODE_SEGMENT, &auth_enabled))
  {
    MCFG_MSG_ERROR("mcfg_auth_img_verify() failed get HW Auth_EN!!");
  }
  else if(!auth_enabled)
  {
#ifndef FEATURE_QDST_SECBOOT
    MCFG_MSG_HIGH("mcfg_auth_img_verify() Auth is disabled");
    retVal = TRUE;
#else
    MCFG_MSG_HIGH("mcfg_auth_img_verify() HW AUTH disabled with QDST built");
    memset(&fuse_info, 0, sizeof(fuse_info));
    fuse_info.root_of_trust = (uint8 *)qd_certs_root_of_trust;
    fuse_info.root_of_trust_len = MCFG_SHA256_HASH_LEN;
    fuse_info.msm_hw_id = 0x0;
    fuse_info.auth_use_serial_num = 0x0; 
    fuse_info.serial_num = 0x0; 
    fuse_info.use_root_of_trust_only = FALSE;
    if(!secboot_util_auth_img_verify(&secboot_handle_ptr,
                                    signed_img_ptr,
                                    signed_img_len,  
                                    SECBOOT_UTIL_HW_MSS_CODE_SEGMENT,
                                    MCFG_AMSS_HASH_TABLE_SW_TYPE,
                                    &fuse_info,
                                    NULL))
    {
      MCFG_MSG_ERROR("mcfg_auth_img_verify() with QDST failed!!");
    }
    else
    {
      MCFG_MSG_HIGH("mcfg_auth_img_verify() with QDST is Successful!!");
      retVal = TRUE;
    }
#endif
  }
  else
  {
    if(!secboot_util_auth_img_verify(&secboot_handle_ptr,
                                    signed_img_ptr,
                                    signed_img_len,  
                                    SECBOOT_UTIL_HW_MSS_CODE_SEGMENT,
                                    MCFG_AMSS_HASH_TABLE_SW_TYPE,
                                    NULL,
                                    NULL))
    {
      MCFG_MSG_ERROR("mcfg_auth_img_verify() failed!!");
    }
    else
    {
      MCFG_MSG_HIGH("mcfg_auth_img_verify() is Successful!!");
      retVal = TRUE;
    }
  }

  return retVal;
} /* mcfg_auth_img_verify() */

/*===========================================================================

  FUNCTION mcfg_auth_init

  DESCRIPTION
    Initialize mcfg auth

  DEPENDENCIES
    None

  PARAMETERS
    None
    
  RETURN VALUE
    None

  SIDE EFFECTS
    None

===========================================================================*/
void mcfg_auth_init
(
  void
)
{
  return;
} /* mcfg_auth_init() */


/*==============================================================================
                    PUBLIC FUNCTION DEFINITIONS FOR MODULE
==============================================================================*/

/*===========================================================================

  FUNCTION mcfg_auth_check_config

  DESCRIPTION
    This function authenticates the config file, the caller should reject
    the config file when loading or populating the configuration into NV/EFS.

  DEPENDENCIES
    None

  PARAMETERS
      config [in] config structure including config size and buffer
                  pointer to be authenticated
    
  RETURN VALUE
    mcfg_error_e

  SIDE EFFECTS
    None

===========================================================================*/
mcfg_error_e_type mcfg_auth_check_config
(
  mcfg_config_s_type *config
)
{
  mcfg_error_e_type error = MCFG_ERR_AUTHENTICATION_FAILED;
  uint32 sig_offset;
  uint32 sig_seg_size = 0;
  uint32 data_offset;
  uint32 data_seg_size = 0;

  MCFG_CHECK_NULL_PTR_RET_ERROR(config, MCFG_ERR_AUTHENTICATION_FAILED);
  MCFG_CHECK_NULL_PTR_RET_ERROR(config->config_addr, 
                                MCFG_ERR_AUTHENTICATION_FAILED);

  if (!mcfg_get_hash_seg_info(config, &sig_offset, &sig_seg_size))
  {
    MCFG_MSG_ERROR(
       "mcfg_auth_check_config Error reading MCFG signature segment");
  }
  else if (!mcfg_get_data_seg_info(config, &data_offset, &data_seg_size))
  {
    MCFG_MSG_ERROR("mcfg_auth_check_config Error reading MCFG data segment");
  }
  else if (!mcfg_auth_verify_hash(config->config_addr, sig_offset, 
                               data_offset, data_seg_size))
  {
    MCFG_MSG_ERROR("mcfg_auth_check_config hash doesn't match");
  }
  else if (!mcfg_auth_img_verify(
              (uint8 *)((uint32)config->config_addr+sig_offset), sig_seg_size))
  {
    MCFG_MSG_ERROR("mcfg_auth_check_config signature auth failed ");
  }
  else
  {  
    error = MCFG_ERR_NONE;
  }
  
  return error;
} /* mcfg_auth_check_config() */

